/**************************************************************************************************
 *  Main authors:
 *     Inga Abel <inga.abel@tum.de>, 
 *	   Maximilian Neuner <maximilian.neuner@tum.de>, 
 *     Michael Eick <michael.eick@tum.de>
 *
 * 
 *  Copyright (C) 2021
 *  Chair of Electronic Design Automation
 *  Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
 *  TU Muenchen
 *  Arcisstrasse 21
 *  D-80333 Muenchen
 *  Germany
 *
 *  This file is part of acst, a analog circuit analysis, sizing and synthesis enviroment:
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *************************************************************************************************/

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"
#include "Core/incl/FlatCircuitRecursion/CircuitsToNetsToInstancesConnectionsMap.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    FlatCircuitRecursion::FlatCircuitRecursion():
            flatCircuit_(nullptr),
            circuitToNetsToInstancesConnectionsMap_(nullptr)
    {
        
    }

    FlatCircuitRecursion::~FlatCircuitRecursion()
    {

    }

    Circuit* FlatCircuitRecursion::createNewFlatCopy(const Circuit& topCircuit)
    {
        flatCircuit_ = new Circuit();
        flatCircuit_->setCircuitIdentifier(topCircuit.getCircuitIdentifier());
        getVisitedInstances().setCircuit(*flatCircuit_);
        createNetsToInstancesConnectionsMap(topCircuit);
        flattenCircuitRecursively(topCircuit);
        addTopLevelTerminals(topCircuit);
        
        delete circuitToNetsToInstancesConnectionsMap_;

        return flatCircuit_;
    }

    void FlatCircuitRecursion::flattenCircuitRecursively(const Circuit& circuit)
   {
    //    logDebug("Add nets to flat copy");
       addNetsToFlatCopy(circuit);
    //    logDebug("Add devices to flat copy");
       addDevicesToFlatCopy(circuit);
    //    logDebug("Add instances to flat copy");
       addInstancesToFlatCopy(circuit);
   }

    void FlatCircuitRecursion::addTopLevelTerminals(const Circuit& topCircuit)
    {
        std::vector<const Terminal*> terminals = topCircuit.findTerminals();
        for(auto& it : terminals)
        {
        	const Terminal* terminal = it;

        	TerminalId terminalId = terminal->getIdentifier();
        	TerminalType terminalType = terminal->getTerminalType();
        	NetId netId = terminal->getNetId();

            Terminal& flatTerminal = getFlatCircuit().addTerminal(terminalId);
        	flatTerminal.setTerminalType(terminalType);
            if(!getCircuitToNetsToInstancesConnectionsMap().isDominatedDoubleNet(topCircuit ,topCircuit.findNet(netId)))
            {
        	    Net& flatNet = getFlatCircuit().findNet(netId);
        	    flatTerminal.setNet(flatNet);
            }
            else
            {
                const Net & dominatingNet = getCircuitToNetsToInstancesConnectionsMap().getDominatingNet(topCircuit, topCircuit.findNet(netId));
                Net& flatNet = getFlatCircuit().findNet(dominatingNet.getIdentifier());
        	    flatTerminal.setNet(flatNet);
            }

        }
    }

   void FlatCircuitRecursion::addNetsToFlatCopy(const Circuit& circuit)
   {
       std::vector<const Net*> netPointers = circuit.findNets();
       for(auto& it : netPointers)
       {
           const Net* net = it;
          
           if(!getCircuitToNetsToInstancesConnectionsMap().isDominatedDoubleNet(circuit, *net))
           {
          
                NetId flatNetId = computeFlatNetIdentifier(*net);
                if(!getFlatCircuit().hasNet(flatNetId)) 
                {
                    Net & newNet = getFlatCircuit().addNet(flatNetId);
                    newNet.setSupply(net->getSupply());
                }
           }
       }
       
   }

   void FlatCircuitRecursion::addDevicesToFlatCopy(const Circuit& circuit)
   {
       std::vector<const Device*> devices = circuit.findDevices();
       for(auto& it : devices)
       {
           const Device* device = it;
           DeviceId flatDeviceId = computeFlatDeviceIdentifier(*device);
           Device& flatDevice = getFlatCircuit().addDevice(flatDeviceId);
           if(device->hasCellTripleId()) {
        	   flatDevice.setCellTripleId(device->getCellTripleId());
           }
           flatDevice.setDeviceType(device->getDeviceType());
           flatDevice.setTechType(device->getTechType());
           connectComponent(circuit, *device, flatDevice);
       }
   }

   void FlatCircuitRecursion::connectComponent(const Circuit & circuit, const Device& component, Device& flatDevice)
   {
       getVisitedInstances().push(component);
       std::vector<const Pin*> pins = component.findAllPins();
       for(auto& it : pins)
       {
           const Pin& pin = *it;
           const PinType& pinType = pin.getPinType();
           Pin& flatPin = flatDevice.addPin(pinType);
           if(!getCircuitToNetsToInstancesConnectionsMap().isDominatedDoubleNet(circuit,pin.getNet()))
           {
                NetId flatNetId = computeFlatNetIdentifier(pin);
                Net& flatNet = getFlatCircuit().findNet(flatNetId);
                flatPin.connect(flatNet);
           }
           else
           {
               const Net & dominatingNet = getCircuitToNetsToInstancesConnectionsMap().getDominatingNet(circuit,pin.getNet());
                NetId flatNetId = computeFlatNetIdentifier(dominatingNet);
                Net& flatNet = getFlatCircuit().findNet(flatNetId);
                flatPin.connect(flatNet);
           }
           
       }
       getVisitedInstances().pop();
   }

   void FlatCircuitRecursion::addInstancesToFlatCopy(const Circuit& circuit)
   {
       if(circuit.hasInstances())
       {
	        std::vector<const Instance*> instances = circuit.findInstances();
            for(auto& it : instances)
            {
                const Instance* instance = it;
                getVisitedInstances().push(*instance);
                const Circuit& master = instance->getMaster();
                flattenCircuitRecursively(master); // recursion
                getVisitedInstances().pop();
                connectInstance(circuit, *instance);
            }
       }

   }

    void FlatCircuitRecursion::connectInstance(const Circuit & circuit, const Instance& instance)
    {
        std::vector<const InstanceTerminal*> instanceTerminals = instance.getInstanceTerminals();
        std::set<NetId> flattenedTerminalNets;
       for(auto& it : instanceTerminals)
       {
           const InstanceTerminal& instanceTerminal = *it;
           if(instanceTerminal.isConnected())
           {
        	   Net& flatInstanceTerminalNet = computeFlatInstanceTerminalNet(instanceTerminal);
        	   Net& flatTerminalNet = computeFlatTerminalNet(instanceTerminal, instance);
        	   if(flatInstanceTerminalNet != flatTerminalNet)
        	   {
        		   getCircuitToNetsToInstancesConnectionsMap().addShortenedNet(flatTerminalNet.getIdentifier(), flatInstanceTerminalNet.getIdentifier());
           		   getFlatCircuit().shortenNets(flatInstanceTerminalNet, flatTerminalNet);
                    
        	   }
           }
       }
    }

    Net& FlatCircuitRecursion::computeFlatInstanceTerminalNet(const InstanceTerminal& instanceTerminal)
    {
        const Net& instanceTerminalNet = instanceTerminal.getNet();
        NetId instanceTerminalNetId;
        if(!getCircuitToNetsToInstancesConnectionsMap().isDominatedDoubleNet(instanceTerminal.getInstance().getCircuit(),instanceTerminalNet))
        {

            instanceTerminalNetId = computeFlatNetIdentifier(instanceTerminalNet);
        }
        else
        {
            const Net & dominatingNet = getCircuitToNetsToInstancesConnectionsMap().getDominatingNet(instanceTerminal.getInstance().getCircuit(), instanceTerminalNet);
            instanceTerminalNetId = computeFlatNetIdentifier(dominatingNet);
        }
        Net& flatInstanceTerminalNet = getFlatCircuit().findNet(instanceTerminalNetId);
        return flatInstanceTerminalNet;
    }

    Net& FlatCircuitRecursion::computeFlatTerminalNet(const InstanceTerminal& instanceTerminal, const Instance & instance)
    {

        const Terminal& terminal = instanceTerminal.getTerminal();
        const Net& terminalNet = terminal.getNet();
        getVisitedInstances().push(instance);
        NetId terminalNetId;
        if(!getCircuitToNetsToInstancesConnectionsMap().isDominatedDoubleNet(instance.getMaster(),terminalNet))
        {
            terminalNetId = computeFlatNetIdentifier(terminalNet);
        }
        else
        {
            const Net & dominatingNet = getCircuitToNetsToInstancesConnectionsMap().getDominatingNet(instance.getMaster(), terminalNet);
            terminalNetId = computeFlatNetIdentifier(dominatingNet);
        }
        getVisitedInstances().pop();



        if(getCircuitToNetsToInstancesConnectionsMap().hasEquivalentShortNet(terminalNetId))
        {
            terminalNetId = getCircuitToNetsToInstancesConnectionsMap().getCorrespondingShortNetId(terminalNetId);
        }

        Net& flatTerminalNet = getFlatCircuit().findNet(terminalNetId);
        return flatTerminalNet;
    }

    NetId FlatCircuitRecursion::computeFlatNetIdentifier(const Pin& pin) const
    {
        NetId flatNetId;
        if(pin.isConnected()) {
            const Net& net = pin.getNet();
            flatNetId = computeFlatNetIdentifier(net);
        } else if(pin.hasNetAssignment()) {
            NetAssignment netAssignment = pin.getNetAssignment();
            flatNetId = getVisitedInstances().resolveAssignment(netAssignment);
        }
        assert(flatNetId.isInitialized());
        return flatNetId;
    }

    NetId FlatCircuitRecursion::computeFlatNetIdentifier(const Net& net) const
    {
        NetId flatNetId;
        if(net.hasAssignment()) {
            NetAssignment assignment = net.getNetAssignment();
            flatNetId = getVisitedInstances().resolveAssignment(assignment);
        }
        else if(net.isGlobal()) {
            flatNetId = net.getIdentifier();
        }
        else {
            flatNetId = net.getIdentifier();
            flatNetId.setPath(getInstanceNamePath());
        }
        return flatNetId;
    }

    DeviceId FlatCircuitRecursion::computeFlatDeviceIdentifier(const Device& component) const
    {
       DeviceId componentId = component.getIdentifier();
       componentId.setPath(getInstanceNamePath());
       return componentId;
    }

    InstanceNamePath FlatCircuitRecursion::getInstanceNamePath() const
    {
        return getVisitedInstances().computeInstanceNamePath();
    }

    bool FlatCircuitRecursion::hasFlatCircuit() const
    {
        return flatCircuit_ != nullptr;
    }

    Circuit& FlatCircuitRecursion::getFlatCircuit()
    {
        assert(hasFlatCircuit());
        return *flatCircuit_;
    }

    const Circuit& FlatCircuitRecursion::getFlatCircuit() const
    {
        assert(hasFlatCircuit());
        return *flatCircuit_;
    }

    const VisitedInstances& FlatCircuitRecursion::getVisitedInstances() const
    {
        return visitedInstances_;
    }

    VisitedInstances& FlatCircuitRecursion::getVisitedInstances()
    {
        return visitedInstances_;
    }

        
    const CircuitsToNetsToInstancesConnectionsMap & FlatCircuitRecursion::getCircuitToNetsToInstancesConnectionsMap() const
    {
        assert(circuitToNetsToInstancesConnectionsMap_->isInitialized());
        return  * circuitToNetsToInstancesConnectionsMap_;
    }

    CircuitsToNetsToInstancesConnectionsMap & FlatCircuitRecursion::getCircuitToNetsToInstancesConnectionsMap()
    {
        assert(circuitToNetsToInstancesConnectionsMap_->isInitialized());
        return  * circuitToNetsToInstancesConnectionsMap_;
    }

    CircuitId FlatCircuitRecursion::createInInstanceCircuitId(const Circuit & circuit, InstanceNamePath path) const
    {
        std::ostringstream newCircuitName;
        newCircuitName << path.getPath();
        newCircuitName << circuit.getCircuitIdentifier().getName();

        CircuitId newCircuitId;
        newCircuitId.setCircuitName(CircuitName(newCircuitName.str()));
        newCircuitId.setId(circuit.getCircuitIdentifier().getId());

        return newCircuitId;
    }

    void FlatCircuitRecursion::createNetsToInstancesConnectionsMap(const Circuit & circuit)
    {
        CircuitsToNetsToInstancesConnectionsMap *  netsToInstancesConnectionsMap = new CircuitsToNetsToInstancesConnectionsMap(circuit);
        circuitToNetsToInstancesConnectionsMap_ = netsToInstancesConnectionsMap;
    }

}


