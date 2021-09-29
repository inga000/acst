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

#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {


    PinType::PinType():
    		optional_(false)
    {
    }

    PinType::PinType(const DeviceTypeName& deviceTypeName, const PinName& pinName):
		deviceTypeName_(deviceTypeName),
		pinName_(pinName),
		optional_(false)
    {

    }

    PinType::~PinType()
    {
    }

    void PinType::setPinName(const PinName& pinName)
    {
    	pinName_ = pinName;
    }

    void PinType::setDeviceTypeName(const DeviceTypeName& deviceTypeName)
    {
    	deviceTypeName_ = deviceTypeName;
    }

    void PinType::setOptional(bool optional)
    {
    	optional_ = optional;
    }

    void PinType::setAutoConnection(const PinName& pinName)
    {
    	assert(isOptional());
    	autoConnection_ = pinName;
    }

    bool PinType::isInitialized() const
    {
        return pinName_.isInitialized() && deviceTypeName_.isInitialized();
    }

    bool PinType::operator<(const PinType& other) const
    {
    	if(getDeviceTypeName() == other.getDeviceTypeName()) {
    		return getPinName() < other.getPinName();
    	} else {
    		return getDeviceTypeName() < other.getDeviceTypeName();
    	}
    }

    bool PinType::operator==(const PinType& other) const
	{
    	return getDeviceTypeName() == other.getDeviceTypeName() &&
    		   getPinName() == other.getPinName();
	}

    bool PinType::isOptional() const
    {
    	return optional_;
    }

    bool PinType::hasAutoConnection() const
    {
    	return autoConnection_.isInitialized();
    }

    PinName PinType::getPinName() const
    {
    	assert(isInitialized());
        return pinName_;
    }

    PinType PinType::getAutoConnection() const
    {
    	assert(hasAutoConnection());
    	PinType autoConnection;
    	autoConnection.setDeviceTypeName(getDeviceTypeName());
    	autoConnection.setPinName(autoConnection_);
    	return autoConnection;
    }

    DeviceTypeName PinType::getDeviceTypeName() const
    {
    	assert(isInitialized());
    	return deviceTypeName_;
    }

    std::string PinType::toStr() const
    {
        std::ostringstream oss;
        oss << "PinType(" << getDeviceTypeName() << "," << getPinName() << ")";
        return oss.str();
    }
}
