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
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <set>

namespace Core {

    Net::Net() :
        globalFlag_(false),
        supply_(Supply::noSupply())
    {
    }

    Net::~Net()
    {
    }

    Net & Net::clone() const
    {
    	Net * newNet = new Net;

    	newNet->setGlobal(globalFlag_);
        newNet->setNetId(netId_);
        newNet->setNetAssignment(netAssignment_);
        newNet->setSupply(supply_);
        //PinTypeToPinMap pins_;
        //std::vector<InstanceTerminal*> instanceTerminals_;

        return * newNet;
    }

    void Net::setNetId(const NetId& netId)
	{
		this->netId_ = netId;
	}

	NetId Net::getIdentifier() const
	{
		assert(netId_.isInitialized());
		return netId_;
	}

    void Net::setGlobal(bool global)
    {
        globalFlag_ = global;
    }

    bool Net::isGlobal() const
    {
        return globalFlag_;
    }

    bool Net::isSupply() const
    {
    	return hasSupply();
    }

    bool Net::isVddSupply() const
    {
    	return hasSupply() ? getSupply().isVdd() : false;
    }

    bool Net::isGndSupply() const
    {
    	return hasSupply() ? getSupply().isGnd() : false;
    }

    void Net::setNetAssignment(const NetAssignment& netAssignment)
    {
        this->netAssignment_ = netAssignment;
    }

    bool Net::hasAssignment() const
    {
        return netAssignment_.isInitialized();
    }

    NetAssignment Net::getNetAssignment() const
    {
        assert(hasAssignment());
        return netAssignment_;
    }

    void Net::addPin(Pin & pin)
	{
		pins_.insert(PinTypeToPinMap::value_type(pin.getPinType(),&pin));
	}

	void Net::removePin(const Pin & pin)
	{
		assert(hasPin(pin));
		PinRange range = getPinIndexRange(pin);
		PinTypeToPinMap::iterator it = findPinInRange(pin, range);
		assert(it != pins_.end());
		pins_.erase(it);
	}

    void Net::setSupply(const Supply& supply)
    {
        supply_ = supply;
    }

    Supply Net::getSupply() const
    {
        assert(supply_.isInitialized());
        return supply_;
    }

    bool Net::hasPins() const
    {
        return !pins_.empty();
    }

    bool Net::hasInstanceTerminals() const
    {
        return !instanceTerminals_.empty();
    }

    std::string Net::toStr() const
    {
    	std::ostringstream oss;
    	oss << getIdentifier().toStr();
//    	oss << std::endl;

//    	if(hasPins())
//    	{
//    		 oss << "Pins: " << std::endl;
//    		 for(auto pin : getPins())
//    		 {
//    			 oss << pin->toStr() << std::endl;
//    		 }
//    	}
//    	else
//    	{
//    		oss << "Net with no pins " << std::endl;
//    	}
	 return oss.str();
    }

    void Net::print(std::ostream & stream) const
    {
        stream << getIdentifier() << "(";
        stream << getSupply();
        if(hasAssignment()) {
            stream << "," << getNetAssignment();
        }
        stream << ")";
    }

    bool Net::operator <(const Net& other) const
    {
        return getIdentifier() < other.getIdentifier();
    }

    bool Net::operator ==(const Net& other) const
    {
        return getIdentifier() == other.getIdentifier();
    }

    bool Net::operator >(const Net& other) const
    {
        return !(*this < other) && !(*this == other);
    }

    bool Net::operator !=(const Net& other) const
    {
        return !(*this == other);
    }

    void Net::addInstanceTerminal(InstanceTerminal& instanceTerminal)
    {
        instanceTerminals_.push_back(&instanceTerminal);
    }

    bool Net::hasPin(const Pin & pin) const
    {
        PinConstRange range = getPinIndexRange(pin);
        return pinIsInRange(pin, range);
    }

    bool Net::hasPin(PinType pinType) const
    {
        return pins_.find(pinType) != pins_.end();
    }

	bool Net::hasSupply() const
	{
	   return getSupply().isSupply();
	}

    Net::PinRange Net::getPinIndexRange(const Pin& pin)
    {
        PinRange range = pins_.equal_range(pin.getPinType());
        return range;
    }

    Net::PinConstRange Net::getPinIndexRange(const Pin& pin) const
    {
        PinConstRange range = pins_.equal_range(pin.getPinType());
        return range;
    }

    bool Net::pinIsInRange(const Pin & pin, const PinConstRange& range) const
    {
        bool pinFound = false;
        for (PinTypeToPinMap::const_iterator it = range.first; it != range.second; it++)
        {
            const Pin & currentPin = *it->second;
            if(currentPin == pin) {
                pinFound = true;
                break;
            }
        }
        return pinFound;
    }

    Net::PinTypeToPinMap::iterator Net::findPinInRange(const Pin& pin, const PinRange& range)
    {
        PinTypeToPinMap::iterator searchedPinIterator= pins_.end();
        for (PinTypeToPinMap::iterator it = range.first; it != range.second; it++)
        {
            const Pin & currentPin = *it->second;
            if(currentPin == pin) {
                searchedPinIterator = it;
                break;
            }
        }
        return searchedPinIterator;
    }

    void Net::mergePins(Net& other)
    {
    	PinTypeToPinMap copyOfOtherPins(other.pins_);
    	for (PinTypeToPinMap::const_iterator it=copyOfOtherPins.begin(); it != copyOfOtherPins.end(); it++)
    	{
    		Pin * pin = it->second;
    		pin->disconnect();
    		pin->connect(*this); // modifies other.pins!
		}
	}

    bool Net::hasGatePin() const
    {
        MosfetDeviceType mosfet;
        bool hasIt = false;
        
        for(auto & it : pins_)
        {
            PinType pin = it.first;
            if(pin == mosfet.gate())
            {
                hasIt = true;
                break;
            }
        }
        return hasIt;
    }

    bool Net::hasDrainPin() const
    {
        MosfetDeviceType mosfet;
        bool hasIt = false;
        
        for(auto & it : pins_)
        {
            PinType pin = it.first;
            if(pin == mosfet.drain())
            {
                hasIt = true;
                break;
            }
        }
        return hasIt;
    }


    std::vector<Pin *> Net::getPins(PinType pinType)
    {
        assert(hasPin(pinType));
        std::vector<Pin *> pins;

        PinRange pinRange = pins_.equal_range(pinType);
        for(PinTypeToPinMap::iterator it=pinRange.first; it!=pinRange.second; ++it)
        {
            pins.push_back(it->second);
        }

        return pins;

    }

    std::vector<const Pin *> Net::getPins(PinType pinType) const
    {
        assert(hasPin(pinType));
        std::vector<const Pin *> pins;

        PinConstRange pinRange = pins_.equal_range(pinType);
        for(PinTypeToPinMap::const_iterator it=pinRange.first; it!=pinRange.second; ++it)
        {
            pins.push_back(it->second);
        }
        
        return pins;
    }

    std::vector<const Pin *> Net::getPins() const
    {
        assert(hasPins(), "Has no Pins!!!");
        std::vector<const Pin *> pins;

        for(auto & itMap : pins_)
        {
            PinConstRange pinRange = pins_.equal_range(itMap.first);
            
            for(PinTypeToPinMap::const_iterator itRange = pinRange.first; itRange!=pinRange.second; ++itRange)
            {
                pins.push_back(itRange->second);
            }
        }

        return pins;

    }

    std::vector<InstanceTerminal*> Net::getInstanceTerminals() const
    {
        assert(hasInstanceTerminals());
        return instanceTerminals_;
    }

    bool Net::connectsGatesOfTwoTransistorsWithSameSourceNet() const
    {
        bool sameNet = false;
        MosfetDeviceType mosfet;
        std::vector<const Pin*> gatePins = getPins(mosfet.gate());

        for(auto & pin1 : gatePins)
        {
            const Device & device1 = pin1->getDevice();
            const Net & sourceNet1 = device1.findNet(mosfet.source());
            for(auto & pin2 : gatePins)
            {
                const Device & device2 = pin2->getDevice();
                if(device1.getIdentifier() != device2.getIdentifier())
                {
                    const Net & sourceNet2 = device2.findNet(mosfet.source());

                    if(sourceNet1.getIdentifier() == sourceNet2.getIdentifier())
                    {
                        sameNet =true;
                    }
                    else
                    {
                        sameNet = false;
                    }
                }
            }
                

        }
        return sameNet;
    }

    bool Net::isOnlyConnectedToPmosTransistors() const
    {
        bool allPmos = true;
        for(auto & pin : getPins())
        {
            if(!pin->getDevice().getTechType().isP())
            {
                allPmos = false;
                break;
            }
        }
        return allPmos;
    }

    bool Net::isOnlyConnectedToNmosTransistors() const
    {
        bool allPmos = true;
        for(auto & pin : getPins())
        {
            if(!pin->getDevice().getTechType().isN())
            {
                allPmos = false;
                break;
            }
        }
        return allPmos;
    }
        
    bool Net::connectedDevicesConnectedToASourceNet() const
    {

        bool allDevicesConnectedToASourceNet = true;
        
        for(auto & pin : getPins())
        {
            const Device & device = pin->getDevice();
            bool connectedToASourceNet = false;
            for(auto & devicePin : device.findAllPins())
            {
                std::string netName = devicePin->getNet().getIdentifier().getName();
                if( netName == "source" || netName == "sourceNmos" || netName == "sourcePmos")
                {
                    connectedToASourceNet = true;
                }
            }
            if(!connectedToASourceNet)
            {
                allDevicesConnectedToASourceNet = false; 
            }
        }
        return allDevicesConnectedToASourceNet;
    }
}

