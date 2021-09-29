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

#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/DeviceId/DevicePath.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {

    DeviceId::DeviceId():
            devicePath_(new DevicePath())
    {
    }

    DeviceId::~DeviceId()
    {
    }

    DeviceId::DeviceId(const DeviceId & other):
        devicePath_(new DevicePath(other.getDevicePath()))
    {
    }

    void DeviceId::operator =(const DeviceId& other)
    {
        devicePath_ = new DevicePath(other.getDevicePath());
    }

    void DeviceId::setPathToRoot()
    {
        getDevicePath().setPathToRoot();
    }

    void DeviceId::setPath(const InstanceNamePath& instanceNamePath)
    {
        getDevicePath().setPath(instanceNamePath);
    }

    void DeviceId::setDeviceName(const DeviceName& deviceName)
    {
        getDevicePath().setName(deviceName);
    }

    bool DeviceId::isInitialized() const
    {
        return devicePath_->isInitialized();
    }

    std::string DeviceId::toStr() const
    {
    	return getDevicePath().toStr();
    }

    std::string DeviceId::getDeviceName() const
    {
        std::string deviceName = getDevicePath().getName();
        return deviceName;
    }

    InstanceNamePath DeviceId::getInstanceNamePath() const
    {
        assert(isInitialized());
        return getDevicePath().getInstanceNamePath();
    }

    std::string DeviceId::getGuiName() const
    {
        return toStr();
    }

    bool DeviceId::operator ==(const DeviceId& other) const
    {
        assert(isInitialized());
        return getDevicePath() == other.getDevicePath();
    }

    bool DeviceId::operator !=(const DeviceId& other) const
    {
        return !(*this == other);
    }

    bool DeviceId::operator <(const DeviceId& other) const
    {
        assert(isInitialized());
        return getDevicePath() < other.getDevicePath();
    }

    bool DeviceId::operator >(const DeviceId& other) const
    {
        return (!(*this == other) && !(*this < other));
    }

    bool DeviceId::hasDevicePath() const
    {
        return devicePath_ != NULL;
    }

    DevicePath& DeviceId::getDevicePath()
    {
        assert(hasDevicePath());
        return *devicePath_;
    }

    const DevicePath& DeviceId::getDevicePath() const
    {
        assert(hasDevicePath());
        assert(isInitialized());
        return *devicePath_;
    }
}
