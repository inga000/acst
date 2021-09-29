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
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    Device::Device() :
        techType_(TechType::undefined())
    {
    }

    Device & Device::clone() const
    {
    	Device  * device = new Device;

    	device->setCellTripleId(cellTripleId_);
    	device->setDeviceId(deviceId_);
    	device->setDeviceType(getDeviceType());
    	device->setTechType(techType_);
    	device->setPropertyMap(propertyMap_);

        return *device;
    }

    Device::~Device()
    {
    	for(PinMap::iterator it = pins_.begin(); it != pins_.end(); it++)
    	{
    		Pin* pin = it->second;
    		if(pin != nullptr) {
    			delete pin;
    			pin = nullptr;
    		}
    	}
    }

    void Device::setCellTripleId(const CellTripleId& cellTripleId)
    {
    	cellTripleId_ = cellTripleId;
    }

    void Device::setDeviceId(const DeviceId& componentId)
    {
        deviceId_ = componentId;
    }

    CellTripleId Device::getCellTripleId() const
    {
    	assert(hasCellTripleId(), "CellTripleId not initialized!");
    	return cellTripleId_;
    }

    DeviceId Device::getIdentifier() const
    {
        assert(hasComponentId());
        return deviceId_;
    }

    void Device::setPropertyMap(PropertyMap propertyMap)
    {
    	propertyMap_ = propertyMap;
    }

    Pin & Device::addPin(const PinType & pinType)
    {
        Pin * newPin = new Pin;
        newPin->setDevice(*this);
        newPin->setPinType(pinType);
        pins_.insert(PinMap::value_type(pinType, newPin));
        return *newPin;
    }

    bool Device::hasPin(const PinType & pinType) const
    {
        return pins_.find(pinType) != pins_.end();
    }

    const Pin & Device::findPin(const PinType & pinType) const
    {
        assert(hasPin(pinType));
        const Pin* pin = pins_.find(pinType)->second;
        return *pin;
    }

    Pin & Device::findPin(const PinType & pinType)
    {
        assert(hasPin(pinType));
        Pin* pin = pins_.find(pinType)->second;
        return *pin;
    }

    std::vector<const Pin*> Device::findAllPins() const
    {
    	std::vector<const Pin*> allPins;
    	for(auto& it : pins_)
    	{
    		allPins.push_back(it.second);
    	}
        return allPins;
    }

    const Net& Device::findNet(const PinType & pinType) const
    {
        assert(hasPin(pinType), "Pintype not existing!");
        const Pin & pin = findPin(pinType);
        assert(pin.isConnected(), "The pin is not connected");
        return pin.getNet();
    }

    Net& Device::findNet(const PinType & pinType)
    {
        assert(hasPin(pinType));
        Pin & pin = findPin(pinType);
        assert(pin.isConnected());
        return pin.getNet();
    }

    bool Device::isConnected(const NetId& netId) const
    {
    	for(auto& it : pins_)
    	{
    		const Pin* pin = it.second;
    		NetId pinNetId = pin->getNet().getIdentifier();
    		if(netId == pinNetId) {
    			return true;
    		}
    	}
    	return false;
    }


    bool Device::hasValue(const PropertyName& propertyName) const
    {
        return propertyMap_.hasValue(propertyName);
    }

    bool Device::hasAssignment(const PropertyName& propertyName) const
    {
        return propertyMap_.hasAssignment(propertyName);
    }

    bool Device::hasCellTripleId() const
    {
    	return cellTripleId_.isInitialized();
    }

    NetId Device::findValue(const PropertyName& propertyName) const
    {
        return propertyMap_.findValue(propertyName);
    }

    NetAssignment Device::findAssignment(const PropertyName& propertyName) const
    {
        return propertyMap_.findAssignment(propertyName);
    }

    void Device::addValue(const PropertyValue& propertyValue)
    {
        propertyMap_.addValue(propertyValue);
    }

    void Device::addValues(const std::vector<PropertyValue>& propertyValues)
    {
    	propertyMap_.addValues(propertyValues);
    }

    void Device::addAssignment(const PropertyAssignment& propertyAssignment)
    {
        propertyMap_.addAssignment(propertyAssignment);
    }

    void Device::addAssignments(const std::vector<PropertyAssignment>& propertyAssignments)
    {
    	propertyMap_.addAssignments(propertyAssignments);
    }

    const DeviceType & Device::getDeviceType() const
    {
        assert(hasComponentType());
        return *deviceType_;
    }

    TechType Device::getTechType() const
    {
        assert(hasTechType());
        return techType_;
    }

    void Device::setTechType(const TechType& techType)
    {
        this->techType_ = techType;
    }

    std::string Device::toStr() const
    {
        return deviceId_.toStr();
    }

    bool Device::operator==(const Device& other) const
    {
        return deviceId_ == other.deviceId_;
    }

    bool Device::operator!=(const Device& other) const
    {
        return !(*this == other);
    }

    bool Device::operator<(const Device& other) const
    {
        return deviceId_ < other.deviceId_;
    }

    void Device::print(std::ostream & stream, const int & column) const
    {
        stream << spaces(column);
        stream << getIdentifier();
        stream << " (" << getDeviceType().getName() << "," << getTechType() << ") ";
        stream << propertyMap_ << " ";

        stream << "Pins: ";
        for (PinMap::const_iterator it = pins_.begin(); it != pins_.end(); it++)
        {
        	const Pin& pin = *it->second;
        	std::string pinName = pin.getPinType().getPinName();
			stream << pinName << " ";
			stream << "(";
			if(pin.isConnected()) {
				stream << pin.getNet();
			} else if(pin.hasNetAssignment()) {
				stream << pin.getNetAssignment();
			} else {
				stream << "no connection";
			}
			stream << ")" << "; ";
        }
    }

    bool Device::hasComponentId() const
    {
        return deviceId_.isInitialized();
    }

    void Device::setDeviceType(const DeviceType & componentType)
    {
        deviceType_ = &componentType;
    }

    bool Device::hasComponentType() const
    {
        return deviceType_ != nullptr;
    }

    bool Device::hasTechType() const
    {
        return techType_.isInitialized();
    }

    std::string Device::spaces(int columns)
    {
        std::string ret = "";
        for (int i = 1; i <= columns; i++)
            ret += " ";
        return ret;
    }
}






