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

#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    DeviceType::DeviceType()
    {
    }

    DeviceType::~DeviceType()
    {
    }

    void DeviceType::setName(const DeviceTypeName& Name)
    {
    	name_ = Name;
    }

    void DeviceType::addPinType(const PinType& pinType)
    {
    	assert(pinType.getDeviceTypeName() == getName());
    	pinTypes_.insert(PinTypeMap::value_type(pinType.getPinName(), pinType));
    }

    void DeviceType::addTechType(const TechType& techType)
    {
    	techTypes_.insert(techType);
    }

    DeviceTypeName DeviceType::getName() const
    {
       assert(name_.isInitialized());
        return name_;
    }

    std::string DeviceType::toStr() const
    {
        return getName();
    }

	bool DeviceType::operator==(const DeviceTypeName& other) const
	{
		return getName() == other;
	}

	bool DeviceType::operator==(const DeviceType& other) const
	{
		return getName() == other.getName();
	}

    PinType DeviceType::findPinType(const PinName& pinName) const
    {
    	assert(hasPinType(pinName));
    	return pinTypes_.find(pinName)->second;
    }

    std::vector<PinType> DeviceType::getPinTypes() const
    {
    	std::vector<PinType> pinTypes;
    	for(auto& it : pinTypes_)
    	{
    		pinTypes.push_back(it.second);
    	}
    	return pinTypes;
    }

    PinConnections DeviceType::getEmptyPinToNetConnections() const
    {
    	PinConnections pinConnections;
    	pinConnections.add(getPinTypes());
    	return pinConnections;
    }

    bool DeviceType::hasPinType(const PinName& pinName) const
    {
    	return pinTypes_.find(pinName) != pinTypes_.end();
    }

    bool DeviceType::isInitialized() const
    {   
        return name_.isInitialized() && !pinTypes_.empty();
    }
}
