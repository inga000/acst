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

#include "HSpice/incl/InputFile/DeviceLine/Device/AbstractDevice.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapping.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include <sstream>
#include <iterator>

namespace HSpice {
    namespace InputFile {

        AbstractDevice::AbstractDevice():
        		deviceType_(NULL),
				deviceLineMapping_(NULL),
                globalNetMap_(NULL)
        {
        }

        AbstractDevice::~AbstractDevice()
        {
        }

        void AbstractDevice::setDeviceLine(const std::string& deviceLine)
        {
        	deviceLine_ = deviceLine;
        }

        void AbstractDevice::setDeviceType(const Core::DeviceType& deviceType)
        {
        	deviceType_ = &deviceType;
        }

        void AbstractDevice::setDeviceLineMapping(const DeviceLineMapping& deviceLineMapping)
        {
        	deviceLineMapping_ = &deviceLineMapping;
        }

        void AbstractDevice::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            globalNetMap_ = &globalNetMap;
        }

        void AbstractDevice::addDeviceToCircuit(Core::Circuit& circuit) const
        {
        	std::istringstream iss;
        	iss.str(getDeviceLine());

        	std::istream_iterator<std::string> beg(iss), end;
        	std::vector<std::string> tokens(beg, end); // split line at whitespace

        	Core::DeviceId deviceId = Core::DeviceName(tokens[getDeviceLineMapping().getDeviceIdPosition()]).createRootIdentifier();

        	Core::TechType techType = Core::TechType::undefined();
        	if(getDeviceLineMapping().hasModel())
        	{
            	HSpice::InputFile::ModelName modelName(tokens[getDeviceLineMapping().getModelNamePosition()]);
            	techType = getDeviceLineMapping().findTechType(modelName);
        	}

        	Core::PinConnections pinConnections = getDeviceType().getEmptyPinToNetConnections();
        	std::vector<Core::PinType> pinTypes = getDeviceType().getPinTypes();
        	for(auto& it : pinTypes)
        	{
        		Core::PinType pinType = it;
        		Core::PinName pinName = pinType.getPinName();
        		Core::NetId netId = Core::NetName(tokens[getDeviceLineMapping().findPinPosition(pinName)]).createRootIdentifier();
        		pinConnections.addNet(pinType, circuit.findNet(netId));
        	}

        	Core::Device& device = circuit.addDevice(deviceId);
        	device.setDeviceType(getDeviceType());
        	device.setTechType(techType);

        	pinConnections.addAndConnectPins(device);
        }

        Core::Net& AbstractDevice::getGlobalNet(const Core::NetId& netId) const
        {
        	assert(isGlobalNet(netId));
        	Core::Net& net = getGlobalNetMap().getNet(netId);
        	return net;
        }

        Core::Net& AbstractDevice::getNet(Core::Circuit& circuit, const Core::NetId& netId) const
        {
            assert(!isGlobalNet(netId));
            Core::Net& net = circuit.findNet(netId);
            return net;
        }

        bool AbstractDevice::isGlobalNet(const Core::NetId& netId) const
        {
            return getGlobalNetMap().hasNet(netId);
        }

        const std::string AbstractDevice::getDeviceLine() const
        {
        	assert(!deviceLine_.empty());
        	return deviceLine_;
        }

        const Core::DeviceType & AbstractDevice::getDeviceType() const
        {
        	assert(deviceType_ != NULL);
        	return *deviceType_;
        }

        const DeviceLineMapping& AbstractDevice::getDeviceLineMapping() const
        {
        	assert(deviceLineMapping_ != NULL);
        	return *deviceLineMapping_;
        }

        const GlobalNetMap& AbstractDevice::getGlobalNetMap() const
        {
            assert(globalNetMap_ != NULL);
            return *globalNetMap_;
        }

    }
}
