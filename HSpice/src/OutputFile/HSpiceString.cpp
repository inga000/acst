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

#include "HSpice/incl/OutputFile/HSpiceString.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include <algorithm>
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

    const std::string  HSpiceString::INSTANCE_ID_ = "x";

    const std::string  HSpiceString::MOSFET_ID_ = "m";
    const std::string  HSpiceString::BIPOLAR_ID_ = "q";
    const std::string  HSpiceString::RESISTOR_ID_ = "r";
    const std::string  HSpiceString::CAPACITOR_ID_ = "c";
    const std::string  HSpiceString::INDUCTOR_ID_ = "l";
    const std::string  HSpiceString::DIODE_ID_ = "d";

    std::string HSpiceString::convertToHSpice(const Core::CircuitId& circuitId)
    {
        std::string circuitName = circuitId.getName();
        convertToLowerCase(circuitName);
        return circuitName;
    }

    std::string HSpiceString::convertToHSpice(const Core::NetId& netId)
    {
        std::string netName = netId.toStr();
        netName.erase(0,1); //erase leading slash

        convertToLowerCase(netName);
        return netName;
    }

    std::string HSpiceString::convertToHSpice(const Core::TerminalId& terminalId)
    {
        std::string terminalName = terminalId.getName();
        convertToLowerCase(terminalName);
        return terminalName;
    }

    std::string HSpiceString::convertToHSpice(const Core::Device& component)
    {
        const Core::DeviceId& componentId = component.getIdentifier();

        std::string deviceName = componentId.toStr();
        deviceName.erase(0,1); // leading erase slash
        convertToLowerCase(deviceName);

        const Core::DeviceType& componentType = component.getDeviceType();
        if(componentType == Core::DeviceTypeNames::Mosfet) {
            insertIdentifier(deviceName, MOSFET_ID_);
        } else if (componentType == Core::DeviceTypeNames::Bipolar) {
            insertIdentifier(deviceName, BIPOLAR_ID_);
        } else if(componentType == Core::DeviceTypeNames::Capacitor) {
            insertIdentifier(deviceName, CAPACITOR_ID_);
        } else if(componentType == Core::DeviceTypeNames::Resistor) {
            insertIdentifier(deviceName, RESISTOR_ID_);
        } else if(componentType == Core::DeviceTypeNames::Inductor) {
            insertIdentifier(deviceName, INDUCTOR_ID_);
        } else if(componentType == Core::DeviceTypeNames::Diode) {
            insertIdentifier(deviceName, DIODE_ID_);
        } else {
            assert(false, "Unsupported ComponentType!");
        }

        return deviceName;
    }

    std::string HSpiceString::convertToHSpice(const Core::InstanceId& instanceId)
    {
        std::string instanceName = instanceId.getName();
        convertToLowerCase(instanceName);
        insertIdentifier(instanceName, INSTANCE_ID_);
        return instanceName;
    }

    void HSpiceString::convertToLowerCase(std::string& stringToConvert)
    {
        std::transform(stringToConvert.begin(), stringToConvert.end(), stringToConvert.begin(), ::tolower);
    }

    void HSpiceString::insertIdentifier(std::string& stringToModify, const std::string & identifier)
    {
        if(stringToModify.find(identifier) != 0) {
            stringToModify.insert(0, identifier);
        }
    }

    }
}


