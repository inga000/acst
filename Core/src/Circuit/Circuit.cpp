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
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/PinConnections.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Net/LocalGlobalNet.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Net/SupplyNet.h"
#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"


#include "HSpice/incl/InputFile/SubCircuitParser/TerminalOrdering.h"


#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    Circuit::Circuit()
    {
    }

    Circuit::~Circuit()
    {
        eraseDevices();
        eraseNets();
        eraseInstances();
        eraseTerminals();

    }

    Circuit & Circuit::clone() const
    {
    	Circuit * newCircuit = new Circuit;

        newCircuit->setCircuitIdentifier(circuitIdentifier_);
        for(auto & it : netMap_)
        {
        	Net * net = it.second;
        	Net & newNet = net->clone();
        	newCircuit->addNet(newNet);
        }
        
        for(auto & it : deviceMap_)
        {
        	Device * device = it.second;
        	Device & newDevice = device->clone();
        	newCircuit->addDevice(newDevice);

        	PinConnections pinConnections = newDevice.getDeviceType().getEmptyPinToNetConnections();
        	std::vector<PinType> pinTypes = newDevice.getDeviceType().getPinTypes();
        	for(auto& it : pinTypes)
        	{
        		PinType pinType = it;
        		NetId netId = device->findNet(pinType).getIdentifier();
        		pinConnections.addNet(pinType,newCircuit->findNet(netId));
        	}

        	pinConnections.addAndConnectPins(newDevice);

        }

        for(auto & it : terminalMap_)
        {
        	Terminal * terminal = it.second;
        	Terminal & newTerminal = newCircuit->addTerminal(terminal->getIdentifier());
        	newTerminal.setNet(newCircuit->findNet(terminal->getNetId()));
        	newTerminal.setTerminalType(terminal->getTerminalType());
        }

        for(auto & it: instanceMap_)
        {
        	Instance * oldInstance = it.second;
        	Instance & newInstance = oldInstance->clone();
        	newInstance.setCircuit(*this);
        	for(auto & instanceTerminal : newInstance.getInstanceTerminals())
        	{
        		const InstanceTerminal & oldInstanceTerminal = oldInstance->findInstanceTerminal(instanceTerminal->getIdentifier());
        		instanceTerminal->connect(newCircuit->findNet(oldInstanceTerminal.getNet().getIdentifier()));
        	}


        }


    	return * newCircuit;
    }

    void Circuit::eraseDevices()
    {

    	for(auto& it : getDeviceMap())
    	{
    		if(it.second != nullptr) {
    			delete it.second;
    			it.second = nullptr;
    		}
    	}
    	getDeviceMap().clear();

    }

    void Circuit::eraseNets()
    {
    	for(auto& it : getNetMap())
    	{
    		if(it.second != nullptr) {
    			delete it.second;
    			it.second = nullptr;
    		}
    	}
        getNetMap().clear();
    }

    void Circuit::eraseInstances()
    {
    	for(auto& it : getInstanceMap())
    	{
    		if(it.second != nullptr) {
    			delete it.second;
    			it.second = nullptr;
    		}
    	}
    	getInstanceMap().clear();
    }

    void Circuit::eraseTerminals()
    {
    	for(auto& it : getTerminalMap())
    	{
    		if(it.second != nullptr) {
    			delete it.second;
    			it.second = nullptr;
    		}
    	}
        getTerminalMap().clear();
    }

    Supply Circuit::computeMergedSupply(Net& net1, Net& net2)
    {
        Supply supply1 = net1.getSupply();
        Supply supply2 = net2.getSupply();
        if(supply1.isSupply() && supply2.isSupply()) {
            assert(supply1 == supply2);
            return supply1;
        }
        if (supply1.isSupply() && !supply2.isSupply()) {
            return supply1;
        }
        if (!supply1.isSupply() && supply2.isSupply()) {
            return supply2;
        }
        return Supply::noSupply();
    }

    std::vector<const Device*> Circuit::findDevices() const
    {
    	std::vector<const Device*> devices;
    	for(auto& it : getDeviceMap())
    	{
    		devices.push_back(it.second);
    	}
    	return devices;
    }

    std::vector<const Terminal*> Circuit::findTerminals() const
    {
    	std::vector<const Terminal*> terminals;
    	for(auto& it : getTerminalMap())
    	{
    		terminals.push_back(it.second);
    	}
        return terminals;
    }

    std::vector<Terminal*> Circuit::findTerminals()
    {
    	std::vector<Terminal*> terminals;
    	for(auto& it : getTerminalMap())
    	{
    		terminals.push_back(it.second);
    	}
        return terminals;
    }

    std::vector<const Instance*> Circuit::findInstances() const
    {
        assert(hasInstances());
    	std::vector<const Instance*> instances;
    	for(auto& it : getInstanceMap())
    	{
    		instances.push_back(it.second);
    	}
        return instances;
    }

    std::vector<Instance*> Circuit::findInstances() 
    {
        assert(hasInstances());
    	std::vector<Instance*> instances;
    	for(auto& it : getInstanceMap())
    	{
    		instances.push_back(it.second);
    	}
        return instances;
    }

    bool Circuit::hasInstances() const
    {
        return !instanceMap_.empty();
    }

    bool Circuit::hasTerminals() const
    {
        return !terminalMap_.empty();
    }

    const Net& Circuit::findNet(const NetId & identifier) const
    {
    	assert(hasNet(identifier));
    	return *(getNetMap().find(identifier)->second);
    }

    Net& Circuit::findNet(const NetId & identifier)
    {
    	assert(hasNet(identifier));
    	return *(getNetMap().find(identifier)->second);
    }
    const Device& Circuit::findDevice(const DeviceId& deviceId) const
    {
        assert(hasDevice(deviceId));
        return *(getDeviceMap().find(deviceId)->second);
    }

    Device& Circuit::findDevice(const DeviceId& componentId)
    {
        assert(hasDevice(componentId));
        return *(getDeviceMap().find(componentId)->second);
    }

    bool Circuit::hasNet(const NetId& identifier) const
    {
        return getNetMap().find(identifier) != getNetMap().end();
    }

    bool Circuit::hasDevice(const DeviceId& identifier) const
    {
    	return getDeviceMap().find(identifier) != getDeviceMap().end();
    }

    std::vector<const Net*> Circuit::findNets() const
    {
    	std::vector<const Net*> allNets;
        for(auto& it : getNetMap())
        {
			allNets.push_back(it.second);
        }
        return allNets;
    }


    std::vector<Device*> Circuit::getDevices() const
	{
    	std::vector<Device*> allDevices;
        for(auto& it : getDeviceMap())
        {
			allDevices.push_back(it.second);
        }
        return allDevices;
	}

    std::vector<Net*> Circuit::getNets() const
	{
    	std::vector<Net*> allNets;
        for(auto& it : getNetMap())
        {
			allNets.push_back(it.second);
        }
        return allNets;
	}

    std::vector<NetId> Circuit::findVddSupplyNetIds() const
    {
    	std::vector<NetId> vddSupplyNetIds;
        for(auto& it : getNetMap())
        {
            const Net* net = it.second;
            if(net->isVddSupply())
            {
                vddSupplyNetIds.push_back(it.first);
            }
        }
        return vddSupplyNetIds;
    }

    std::vector<NetId> Circuit::findGndSupplyNetIds() const
    {
		std::vector<NetId> gndSupplyNetIds;
		for(auto& it : getNetMap())
		{
			const Net* net = it.second;
			if(net->isGndSupply())
			{
				gndSupplyNetIds.push_back(it.first);
			}
		}
		return gndSupplyNetIds;
    }

    SupplyNets Circuit::findSupplyNets() const
    {
    	SupplyNets supplyNets;
    	for(auto& it : getNetMap())
    	{
    		const Net* net = it.second;
    		if(net->isSupply()) {
    			NetId netId = it.first;
    			Supply supply = net->getSupply();

    			SupplyNet supplyNet;
    			supplyNet.setNetId(netId);
    			supplyNet.setSupply(supply);
    			supplyNets.add(supplyNet);
    		}
    	}
    	return supplyNets;
    }

    std::vector<NetId> Circuit::findGlobalNetIds() const
    {
		std::vector<NetId> globalNetIds;
		for(auto& it : getNetMap())
		{
			const Net* net = it.second;
			if(net->isGlobal())
			{
				globalNetIds.push_back(it.first);
			}
		}
		return globalNetIds;
    }

    Device& Circuit::addDevice(const DeviceId& deviceId)
    {
        Device * newDevice = new Device();
        newDevice->setDeviceId(deviceId);
        getDeviceMap().insert(DeviceMap::value_type(deviceId, newDevice));
        return *newDevice;
    }

    void Circuit::addDevice(Device & device)
    {
    	getDeviceMap().insert(DeviceMap::value_type(device.getIdentifier(), &device));
    }

    void Circuit::addNet(Net & net)
    {
    	getNetMap().insert(NetMap::value_type(net.getIdentifier(), &net));
    }

    void Circuit::addInstance(Instance & instance)
    {
    	getInstanceMap().insert(InstanceMap::value_type(instance.getIdentifier(), & instance));
    }

    Instance& Circuit::addInstance(const InstanceId& instanceId)
    {
        Instance* instance = new Instance;
        instance->setIdentifier(instanceId);
        instance->setCircuit(*this);
        getInstanceMap().insert(InstanceMap::value_type(instanceId,instance));
        return *instance;
    }

    Terminal& Circuit::addTerminal(const TerminalId& terminalId)
    {
        Terminal* terminal = new Terminal();
        terminal->setTerminalId(terminalId);
        getTerminalMap().insert(TerminalMap::value_type(terminalId, terminal));
        return *terminal;
    }

    bool Circuit::hasTerminal(const TerminalId& terminalId) const
    {
        return getTerminalMap().find(terminalId) != getTerminalMap().end();
    }

    bool Circuit::hasTerminal(const NetId& netId) const
    {
    	bool hasIt = false;
    	for(auto & potentialTerminal : findTerminals())
    	{
    		if(potentialTerminal->getNet().getIdentifier() == netId)
    	    {
    	         hasIt = true;
    	         break;
    	    }
    	}

    	return hasIt;
    }

    bool Circuit::hasInstance(const InstanceId& instanceId) const
    {
        return getInstanceMap().find(instanceId) != getInstanceMap().end();
    }

    CircuitId Circuit::getCircuitIdentifier() const
    {
        assert(circuitIdentifier_.isInitialized());
        return circuitIdentifier_;
    }

    const Terminal& Circuit::findTerminal(const TerminalId& terminalId) const
    {
        assert(hasTerminal(terminalId));
        return *(getTerminalMap().find(terminalId)->second);
    }

    Terminal& Circuit::findTerminal(const TerminalId& terminalId)
    {
        assert(hasTerminal(terminalId));
        return *(getTerminalMap().find(terminalId)->second);
    }

    const Terminal& Circuit::findTerminal(NetId netId) const
    {
    	assert(hasTerminal(netId), "Net must have a terminal!");
    	const Terminal * terminal = nullptr;
    	for(auto & potentialTerminal : findTerminals())
    	{
    		if(potentialTerminal->getNet().getIdentifier() == netId)
    	    {
    	          terminal = potentialTerminal;
    	    }
    	}
    	assert(terminal != nullptr, "Terminal must be found!");
    	return * terminal;
    }

    Terminal& Circuit::findTerminal(NetId netId)
    {
    	assert(hasTerminal(netId), "Net must have a terminal!");
    	Terminal * terminal = nullptr;
    	for(auto & potentialTerminal : findTerminals())
    	{
    		if(potentialTerminal->getNet().getIdentifier() == netId)
    	    {
    	          terminal = potentialTerminal;
    	    }
    	}
    	assert(terminal != nullptr, "Terminal must be found!");
    	return * terminal;
    }

    const Instance& Circuit::findInstance(const InstanceId& instanceId) const
    {
        assert(hasInstance(instanceId));
        return *(getInstanceMap().find(instanceId)->second);
    }

    Instance& Circuit::findInstance(const InstanceId& instanceId)
    {
        assert(hasInstance(instanceId));
        return *(getInstanceMap().find(instanceId)->second);
    }

    void Circuit::removeNet(Net& net)
    {
        assert(hasNet(net.getIdentifier()));
        assert(!net.hasPins());
        getNetMap().erase(net.getIdentifier());

        Net* netPtr = &net;
        delete netPtr;
        netPtr = nullptr;
    }

    void Circuit::shortenNets(Net& net1, Net& net2)
    {
    	assert(hasNet(net1.getIdentifier()));
    	assert(hasNet(net2.getIdentifier()));
    	assert(net1 != net2);
    	// no support for hierarchical circuits implemented.
    	assert(!net1.hasAssignment());
    	assert(!net2.hasAssignment());
    	assert(!net1.hasInstanceTerminals());
    	assert(!net2.hasInstanceTerminals());
    	assert(!net1.isGlobal());
    	assert(!net2.isGlobal());

    	Supply mergedSupply = computeMergedSupply(net1, net2);
    	net1.setSupply(mergedSupply);
    	net1.mergePins(net2);
    	removeNet(net2);
    }

    void Circuit::setCircuitIdentifier(const CircuitId& circuitIdentifier)
    {
        circuitIdentifier_ = circuitIdentifier;
    }

    Net& Circuit::addNet(const NetId& identifier)
    {
        Net* newNet = new Net();
        newNet->setNetId(identifier);
        getNetMap().insert(NetMap::value_type(identifier, newNet));
        return *newNet;
    }

    LocalGlobalNet & Circuit::addLocalGlobalNet(const NetId& identifier)
    {
    	LocalGlobalNet* newNet = new LocalGlobalNet();
    	newNet->setNetId(identifier);
        getNetMap().insert(NetMap::value_type(identifier, newNet));
    	return *newNet;
    }

    void Circuit::print(std::ostream& stream) const
    {
        printSummary(stream);
        printTerminals(stream);
        printNets(stream);
        printInstances(stream);
        printDevices(stream);
    }

    void Circuit::printSummary(std::ostream& stream) const
    {
        stream << "Circuit with ";
        stream << getDeviceMap().size() << " Devices, ";
        stream << getNetMap().size() << " Nets.";
        stream << "Number of gate-nets not connected to a drain: " ;
        stream << getGateNetsNotConnectedToADrain().size() << std::endl;
        if(hasGateNetsNotConnectedToADrain())
        {
            stream << "Gate nets with no drain connection: ";
            for(auto & net : getGateNetsNotConnectedToADrain())
            {
                stream << net.toStr() << ", ";
            }
            stream << std::endl;
        }
        stream << " Every Gate net is only connectec to one drain? " << everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType() << std::endl;
    }

    void Circuit::printTerminals(std::ostream& stream) const
    {
        stream << "Terminals: ";
        for(auto& it : getTerminalMap())
        {
            const Terminal* terminal = it.second;
            stream << terminal << "; ";
        }
        stream << std::endl;
    }

    void Circuit::printNets(std::ostream& stream) const
    {
        stream << "Nets: ";
        for(auto& it : getNetMap())
        {
            const Net* net = it.second;
            net->print(stream);
            stream << "; ";
        }
        stream << std::endl;
    }

    void Circuit::printDevices(std::ostream& stream) const
    {
        const int column = 0;
        stream << "Components: " << std::endl;
        for(auto& it : getDeviceMap())
        {
            const Device* component = it.second;
            component->print(stream, column);
            stream << std::endl;
        }
    }

    void Circuit::printInstances(std::ostream& stream) const
    {
        stream << "Instances:" << std::endl;
        for(auto& it : getInstanceMap())
		{
            const Instance* instance = it.second;
            instance->print(stream);
            stream << std::endl;
        }
    }

    std::string Circuit::toStr() const
    {
        std::ostringstream buf;

        print(buf);
        return buf.str();
    }

    const Circuit::NetMap& Circuit::getNetMap() const
    {
        return netMap_;
    }

    Circuit::NetMap& Circuit::getNetMap()
    {
        return netMap_;
    }

    const Circuit::DeviceMap& Circuit::getDeviceMap() const
    {
        return deviceMap_;
    }

    Circuit::DeviceMap& Circuit::getDeviceMap()
    {
        return deviceMap_;
    }

    const Circuit::TerminalMap& Circuit::getTerminalMap() const
    {
        return terminalMap_;
    }

    Circuit::TerminalMap& Circuit::getTerminalMap()
    {
        return terminalMap_;
    }

    const Circuit::InstanceMap& Circuit::getInstanceMap() const
    {
        return instanceMap_;
    }

    Circuit::InstanceMap& Circuit::getInstanceMap()
    {
        return instanceMap_;
    }

    bool Circuit::hasGateNetsNotConnectedToADrain() const
    {
        bool hasIt = false;

        FlatCircuitRecursion * flatCircuitRecursion = new FlatCircuitRecursion;
        Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*this);

        std::vector<Net*> nets;

        if(hasInstances())
        {
        	nets = flatCircuit->getNets();
        }
        else
        {
        	nets = getNets();
        }

        for(auto & itNets : nets)
        {
            Net * net = itNets;
            if(net->hasGatePin() && ! net->hasDrainPin())
            {
                if(!net->hasInstanceTerminals())
                {
                    hasIt = true;
                }

                
                break;
            }

        }
         delete flatCircuitRecursion;
         delete flatCircuit;
        return hasIt; 
    }

    std::vector<NetId> Circuit::getGateNetsNotConnectedToADrain() const
    {
        std::vector<NetId> nets;

        FlatCircuitRecursion * flatCircuitRecursion = new FlatCircuitRecursion;
        Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*this);

        std::vector<Net*> searchNets;

        if(hasInstances())
        {
        	searchNets = flatCircuit->getNets();
        }
        else
        {
        	searchNets = getNets();
        }

        for(auto & it : searchNets)
        {
            Net * net = it;
            if(net->hasGatePin() && ! net->hasDrainPin())
            {
                nets.push_back(net->getIdentifier());
            }
         }
        delete flatCircuitRecursion;
        delete flatCircuit;
        return nets;  
    }

    bool Circuit::GatesOfInstanceTransistorsConnectedToSameNet(const Instance & instance) const
    {
        bool isSameNet = true;
        assert(instance.getCircuit().getCircuitIdentifier() == this->getCircuitIdentifier(), "Instance must be in this circuit");
        FlatCircuitRecursion * flatCircuitRecursion = new FlatCircuitRecursion;
        Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*this);
        Circuit * flatInstanceCircuit = flatCircuitRecursion->createNewFlatCopy(instance.getMaster());

        std::vector<const Device*> instanceDevices = flatInstanceCircuit->findDevices();

        MosfetDeviceType mosfet;

        for(auto & device1 : instanceDevices)
        {
            if(device1->getDeviceType() == mosfet)
            {
                DeviceId newDeviceId1 = createInInstanceDeviceId(instance.getIdentifier().getInstanceName(), device1->getIdentifier());
                const Device & device1InCircuit = flatCircuit->findDevice(newDeviceId1);
                for(auto & device2 : instanceDevices)
                {
                    if(device2->getDeviceType() == mosfet)
                    {
                        DeviceId newDeviceId2 = createInInstanceDeviceId(instance.getIdentifier().getInstanceName(), device2->getIdentifier());
                        const Device & device2InCircuit = flatCircuit->findDevice(newDeviceId2);
                        const Net& gateNet1 = device1InCircuit.findNet(mosfet.gate());
                        const Net& gateNet2 = device2InCircuit.findNet(mosfet.gate());
                        if(gateNet1.getIdentifier() != gateNet2.getIdentifier())
                        {
                            isSameNet =false;
                            break;
                        }
                    }
                }
                if(!isSameNet)
                    break;

            }
        }

        delete flatCircuitRecursion;
        delete flatCircuit;
        delete flatInstanceCircuit;

        return isSameNet;
    }

    // Debugging the synthesized Circuits
     bool Circuit::everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType() const
     {
        bool isConnectedToOnlyOneDrain = true;
        FlatCircuitRecursion * flatCircuitRecursion = new FlatCircuitRecursion;
        Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*this);

        std::vector<Net*> nets;

        if(hasInstances())
        {
        	nets = flatCircuit->getNets();
        }
        else
        {
        	nets = getNets();
        }

        for(auto & itNets : nets)
        {
            Net * net = itNets;
            if(net->hasGatePin() && net->hasDrainPin())
            {


                MosfetDeviceType mosfet;
                std::vector<Pin*> gatePins = net->getPins(mosfet.gate());
                std::vector<Pin*> drainPins= net->getPins(mosfet.drain());

                if(gatePinsAreOnlyNDoped(gatePins))
                {
                    if(moreThanOneNDopdedDrainPin(drainPins))
                        return false;
                }
                else if(gatePinsAreOnlyPDoped(gatePins))
                {
                    if(moreThanOnePDopdedDrainPin(drainPins))
                        return false;
                }
                else
                {
                    if(moreThanOnePDopdedDrainPin(drainPins) || moreThanOneNDopdedDrainPin(drainPins))
                        return false;
                }

            }

         }

        delete flatCircuitRecursion;
        delete flatCircuit;
        return isConnectedToOnlyOneDrain;
     }

    bool Circuit::gatePinsAreOnlyNDoped(std::vector<Pin*> gatePins) const
    {
        bool isTrue = true;
        for(auto & pin : gatePins)
        {
            const Device & device = pin->getDevice();
            if(device.getTechType().isP())
            {
                isTrue = false;
                break;
             }
        }
        return isTrue;
    }

    bool Circuit::gatePinsAreOnlyPDoped(std::vector<Pin*> gatePins) const
    {
        bool isTrue = true;
         for(auto & pin : gatePins)
         {
            const Device & device = pin->getDevice();
            if(device.getTechType().isN())
            {
                isTrue = false;
                break;
            }
         }
         return isTrue;
    }

    bool Circuit::moreThanOnePDopdedDrainPin(std::vector<Pin*> drainPins) const
    {

        bool isTrue = false;
        int counter = 0;

        for(auto & pin : drainPins)
        {
            const Device & device = pin->getDevice();

            if(device.getTechType().isP())
            {
                counter++;
                
            }
            if(counter >=2)
            {
                isTrue = true;
                break;
            }
         } 
         return isTrue;
    }

    bool Circuit::moreThanOneNDopdedDrainPin(std::vector<Pin*> drainPins) const
    {
        bool isTrue = false;
        int counter = 0;
        for(auto & pin : drainPins)
        {
            const Device & device = pin->getDevice();
            if(device.getTechType().isN())
            {
                counter++;
                
            }
            if(counter >=2)
            {
                isTrue = true;
                break;
            }
         } 
         return isTrue;
    }

    DeviceId Circuit::createInInstanceDeviceId(InstanceName instanceName, DeviceId oldDeviceId) const
    {
        InstanceNamePath instanceNamePath;
        instanceNamePath.setIsRoot();

        instanceNamePath.append(instanceName);

        InstanceNamePath oldInstanceNamePath = oldDeviceId.getInstanceNamePath();
        while(!oldInstanceNamePath.isRoot())
        {
            InstanceName name = oldInstanceNamePath.pop();
            instanceNamePath.append(name);
        }

        DeviceId newDeviceId;
        newDeviceId.setDeviceName(DeviceName(oldDeviceId.getDeviceName()));
        newDeviceId.setPath(instanceNamePath);
        return newDeviceId;
    }


}

