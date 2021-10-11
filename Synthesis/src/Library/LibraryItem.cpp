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
 *************************************************************************************************/#include "Synthesis/incl/Library/LibraryItem.h"
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Loads.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"



#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Core/incl/Common/BacktraceAssert.h"


namespace Synthesis {

    LibraryItem::LibraryItem()
    {

    }

    LibraryItem::~LibraryItem()
    {
        
    }

    void LibraryItem::addInstanceTerminals(Core::Instance & instance) const
    {
        const Core::Circuit & master = instance.getMaster();

        for(auto & terminal : master.findTerminals())
        {
            instance.addInstanceTerminal(terminal->getIdentifier());
        }
    }

    void LibraryItem::createInstanceId(Core::Instance & instance, Core::InstanceName instanceName) const
    {
        Core::InstanceId instanceId;
        instanceId.setInstanceName(instanceName);

        instance.setIdentifier(instanceId);
    }

     Core::InstanceId LibraryItem::createInstanceId(Core::InstanceName instanceName) const
    {
        Core::InstanceId instanceId;
        instanceId.setInstanceName(instanceName);

        return instanceId;
    }

     Core::TerminalId LibraryItem::createTerminalId(Core::TerminalName terminalName) const
     {
         Core::TerminalId terminalId;
         terminalId.setTerminalName(terminalName);

         return terminalId;
     }

    void LibraryItem::addNetsToCircuit(Core::Circuit & circuit, std::vector<Core::NetId> netNames) const
    {
        for(auto & netName : netNames)
        {
		    circuit.addNet(netName);
        }

    }

    void LibraryItem::addTerminalsToCircuit(Core::Circuit & circuit, std::map<Core::TerminalName, Core::NetId> terminalToNetMap) const
    {
        for(auto & it : terminalToNetMap)
        {
            Core::TerminalName terminalName = it.first;
            Core::NetId netId = it.second;
            Core::TerminalId terminalId;
            terminalId.setTerminalName(terminalName);

            Core::Terminal& terminal = circuit.addTerminal(terminalId);
            Core::Net& net = circuit.findNet(netId);
            terminal.setNet(net);
		    terminal.setTerminalType(Core::TerminalType::input());
        }
    }

    void LibraryItem::addAdditionalTerminalToCircuit(Core::Circuit & circuit, Core::NetId netId, Core::TerminalName terminalName)
    {
        Core::TerminalId terminalId;
        terminalId.setTerminalName(terminalName);

        Core::Terminal& terminal = circuit.addTerminal(terminalId);
        Core::Net& net = circuit.findNet(netId);
        terminal.setNet(net);
		terminal.setTerminalType(Core::TerminalType::input());
    }

    Core::Instance & LibraryItem::createInstance(const Core::Circuit & circuit, Core::InstanceName instanceName) const
    {
        Core::Instance * instance = new Core::Instance;
		instance->setMaster(circuit);
		createInstanceId(*instance, instanceName);
		addInstanceTerminals(*instance);
		return * instance;
    }

    void LibraryItem::connectInstanceTerminal(Core::Circuit & circuit, Core::Instance & instance, Core::TerminalName terminalName, Core::NetId netId) const
    {
        Core::TerminalId terminalId;
        terminalId.setTerminalName(terminalName);
        Core::InstanceTerminal & terminal = instance.findInstanceTerminal(terminalId);
        terminal.connect(circuit.findNet(netId));
    }

    //Can only used on gate nets not connected to a drain
    bool LibraryItem::transistorsWithSameGateNetAreNotSourceConnected(const Core::Circuit & circuit, std::vector<Core::NetId> gateNets) const
    {
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(circuit);

        bool isTrue = true;
        for(auto & gateNet : gateNets)
        {
            const Core::Net & net = flatCircuit->findNet(gateNet);
            
            isTrue = !net.connectsGatesOfTwoTransistorsWithSameSourceNet();
        }

        delete flatCircuit;
        delete flatCircuitRecursion;
        return isTrue;

    }

    bool LibraryItem::isSingleDiodeTransistor(const Core::Circuit & circuit) const
    {

        if(circuit.hasInstances())
        {
            std::vector<const Core::Instance*> instances = circuit.findInstances();
            if(instances.size() == 1)
            {
                const Core::Instance & instance = **instances.begin();
                return isSingleDiodeTransistor(instance.getMaster());
            }
            else
            {
                return false;
            }
        }
        else
        {
            Core::CircuitIds circuitIds;
            return circuit.getCircuitIdentifier() == circuitIds.diodeTransistor();
        }

    }

    //Only useable by gateNets without drain connection
    std::vector<Core::DeviceId>  LibraryItem::getGateConnectedFlatDevices(const Core::Circuit & circuit, std::vector<Core::NetId> gateNets) const
    {
        std::vector<Core::DeviceId> devices;
        Core::MosfetDeviceType mosfet;
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(circuit);

        for(auto & gateNet : gateNets)
        {
            const Core::Net & net = flatCircuit->findNet(gateNet);
            std::vector<const Core::Pin *> gatePins = net.getPins(mosfet.gate());
            for(auto & gatePin : gatePins)
            {
                const Core::Device & device = gatePin->getDevice();
                devices.push_back(device.getIdentifier());
            }
            


        }

        delete flatCircuit;
        delete flatCircuitRecursion;
        return devices;
        
    }

    bool LibraryItem::transistorsAreSourceConnected(const Core::Circuit & circuit, std::vector<Core::DeviceId > devices ) const
    {
        bool areConnected = false;

        Core::MosfetDeviceType mosfet;
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(circuit);

        for(auto & deviceId1 : devices)
        {
            for(auto & deviceId2 : devices)
            {
                if(deviceId1 != deviceId2)
                {
                    const Core::Device & device1 = flatCircuit->findDevice(deviceId1);
                    const Core::Net & sourceNet1 = device1.findNet(mosfet.source());
                    
                    const Core::Device & device2 = flatCircuit->findDevice(deviceId2);
                    const Core::Net & sourceNet2 = device2.findNet(mosfet.source());

                    if(sourceNet1.getIdentifier() == sourceNet2.getIdentifier())
                    {
                        areConnected = true;
                    }
                    else
                    {
                        areConnected = false;
                    }
                }
            }

        }
        
        
        delete flatCircuit; 
        delete flatCircuitRecursion;
     

        return areConnected;
    }

    std::vector<Core::DeviceId> LibraryItem::getDeviceNamesOfFlatCircuit(const Core::Circuit & circuit) const
    {
        std::vector<Core::DeviceId> devicesInCircuit;


        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        const Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(circuit);

        for(auto & device : flatCircuit->findDevices())
        {
            devicesInCircuit.push_back(device->getIdentifier());
        }

        delete flatCircuit;
        delete flatCircuitRecursion;

        return devicesInCircuit;
    }

    bool LibraryItem::bothTransistorStacksAreVoltageBiases(const Core::Instance & loadPart) const
	{
        Core::CircuitIds circuitIds;
        assert(loadPart.getMaster().getCircuitIdentifier() == circuitIds.loadPart() || loadPart.getMaster().getCircuitIdentifier() == circuitIds.voltageBiasLoadPart(), "Instance must be a loadPart!");
		const Core::Instance & transistorStack1 = loadPart.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_));
		const Core::Instance & transistorStack2 = loadPart.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK2_));
		if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
					&& transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}

    bool LibraryItem::hasGCC(const Core::Instance & load) const
    {
        Core::CircuitIds circuitIds;
        assert(load.getMaster().getCircuitIdentifier() == circuitIds.load() || load.getMaster().getCircuitIdentifier() == circuitIds.voltageBiasLoad(), "Instance must be a load!");
		Core::TerminalId innerGCC;
        innerGCC.setTerminalName(Loads::INNERGCC_TERMINAL_);
        if(load.getMaster().hasTerminal(innerGCC))
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    bool LibraryItem::sourceTransistorIsDiodeTransistor(const Core::Circuit & transistorStack) const
    {
        if(getDeviceNamesOfFlatCircuit(transistorStack).size() == 1)
        {
            return isSingleDiodeTransistor(transistorStack);
        }
        else
        {
            assert(transistorStack.hasInstance(createInstanceId(CurrentBiases::SOURCETRANSISTOR_)), "Transistor stack must have a source transistor");
            const Core::Instance & sourceTransistor = transistorStack.findInstance(createInstanceId(CurrentBiases::SOURCETRANSISTOR_));
            return isSingleDiodeTransistor(sourceTransistor.getMaster());
        }
    }

    bool LibraryItem::outputTransistorIsDiodeTransistor(const Core::Circuit & transistorStack) const
    {
        if(getDeviceNamesOfFlatCircuit(transistorStack).size() == 1)
        {
            return isSingleDiodeTransistor(transistorStack);
        }
        else
        {
            assert(transistorStack.hasInstance(createInstanceId(CurrentBiases::OUTPUTTRANSISTOR_)), "Transistor stack must have a source transistor");
            const Core::Instance & outputTransistor = transistorStack.findInstance(createInstanceId(CurrentBiases::OUTPUTTRANSISTOR_));
            return isSingleDiodeTransistor(outputTransistor.getMaster());
        }
    }

}
