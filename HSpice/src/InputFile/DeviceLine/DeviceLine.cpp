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

#include "HSpice/incl/InputFile/DeviceLine/DeviceLine.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapping.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/AbstractDevice.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "HSpice/incl/InputFile/TechTypeMapper.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/MosfetDevice.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/BipolarDevice.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/ResistorDevice.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/CapacitorDevice.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/InductorDevice.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/DiodeDevice.h"
#include "HSpice/incl/InputFile/ModelName.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        DeviceLine::DeviceLine():
                abstractDevice_(NULL),
                deviceTypeRegister_(NULL),
				deviceLineMapper_(NULL)
        {

        }

        void DeviceLine::setLine(const std::string& line)
        {
            line_ = line;
        }

        void DeviceLine::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
        {
        	deviceTypeRegister_ = &deviceTypeRegister;
        }

        void DeviceLine::setDeviceLineMapper(const DeviceLineMapper& deviceLineMapper)
        {
        	deviceLineMapper_ = &deviceLineMapper;
        }

        bool DeviceLine::isDevice(char identifier)
        {
            return getDeviceLineMapper().isDevice(identifier);
        }

        void DeviceLine::parse()
        {
            char identifier = getLine()[0];
            assert(isDevice(identifier));

            const DeviceLineMapping& deviceLineMapping = getDeviceLineMapper().find(identifier);
            netIds_ = deviceLineMapping.computeNetIds(getLine());

			Core::DeviceTypeName deviceTypeName = deviceLineMapping.getDeviceTypeName();

			const Core::DeviceType& deviceType = getDeviceTypeRegister().findDeviceType(deviceTypeName);

            abstractDevice_ = new AbstractDevice;
            abstractDevice_->setDeviceLine(getLine());
            abstractDevice_->setDeviceType(deviceType);
            abstractDevice_->setDeviceLineMapping(deviceLineMapping);
        }

        AbstractDevice* DeviceLine::getDevice() const
        {
            assert(abstractDevice_ != NULL);
            return abstractDevice_;
        }

        std::vector<Core::NetId> DeviceLine::getNetIds() const
        {
            assert(!netIds_.empty());
            return netIds_;
        }

        bool DeviceLine::hasDeviceTypeRegister() const
        {
        	return deviceTypeRegister_ != NULL;
        }

        bool DeviceLine::hasLine() const
        {
            return !line_.empty();
        }

        bool DeviceLine::hasDeviceLineMapper() const
        {
            return deviceLineMapper_ != NULL;
        }

        const std::string& DeviceLine::getLine() const
        {
            assert(hasLine());
            return line_;
        }

        const Core::DeviceTypeRegister& DeviceLine::getDeviceTypeRegister() const
        {
        	assert(hasDeviceTypeRegister());
        	return *deviceTypeRegister_;
        }

        const DeviceLineMapper& DeviceLine::getDeviceLineMapper() const
        {
            assert(hasDeviceLineMapper());
            return *deviceLineMapper_;
        }

    }
}
