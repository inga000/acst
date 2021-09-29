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
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {

    Pin::Pin():
		device_(NULL),
		net_(NULL)
    {
    }

    Pin::~Pin()
    {
    	device_ = NULL;
        if (isConnected()) {
            net_ = NULL;
        }
    }

    void Pin::setDevice(Device& component)
    {
        device_ = &component;
    }

    void Pin::setPinType(const PinType& pinType)
    {
        pinType_ = pinType;
    }

    std::string Pin::toStr() const
    {
        std::string netString = "NULL";
        if(isConnected()) {
            netString = getNet().getIdentifier().toStr();
        }
        std::ostringstream stream;
        stream << "Pin(" << getPinType() << "," << getDevice() << "," << netString << ")";
        return stream.str();
    }

    const Net & Pin::getNet() const
    {
        assert(isConnected());
        return *net_;
    }

    Net & Pin::getNet()
    {
        assert(isConnected());
        return *net_;
    }

    PinType Pin::getPinType() const
    {
        assert(hasPinType());
        return pinType_;
    }

    const Device & Pin::getDevice() const
    {
        assert(hasDevice());
        return *device_;
    }

    Device & Pin::getDevice()
    {
        assert(hasDevice());
        return *device_;
    }

    void Pin::connect(Net & net)
    {
        assert(!isConnected());
        net_ = &net;
        net_->addPin(*this);
    }

    void Pin::disconnect()
    {
    	assert(isConnected());
    	net_->removePin(*this);
    	net_ = NULL;
    }

    bool Pin::isConnected() const
    {
        return net_ != NULL;
    }

    bool Pin::hasNetAssignment() const
    {
        return netAssignment_.isInitialized();
    }

    bool Pin::operator<(const Pin& other) const
    {
        if( hasEqualDevice(other)) {
            return getPinType() < other.getPinType();
        } else {
            return getDevice() < other.getDevice();
        }
    }

    bool Pin::operator==(const Pin& other) const
	{
        if( hasEqualDevice(other)) {
            return getPinType() == other.getPinType();
        } else {
            return false;
        }
	}

    bool Pin::hasEqualDevice(const Pin& other) const
    {
    	return device_ == other.device_;
    }

    void Pin::setNetAssignment(const NetAssignment& netAssignment)
    {
        assert(netAssignment.isInitialized());
        netAssignment_ = netAssignment;
    }

    NetAssignment Pin::getNetAssignment() const
    {
        assert(hasNetAssignment());
        return netAssignment_;
    }

    bool Pin::hasEqualPinType(const Pin & other) const
    {
        return getPinType()  == other.getPinType();
    }

    bool Pin::hasDevice() const
    {
        return device_ != NULL;
    }

    bool Pin::hasPinType() const
    {
        return pinType_.isInitialized();
    }

}


