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

#include "HSpice/incl/InputFile/DeviceLine/Device/MosfetDevice.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        MosfetDevice::MosfetDevice():
            AbstractDevice()
        {
        }

        MosfetDevice::~MosfetDevice()
        {
        }

        void MosfetDevice::setDrainNetId(const Core::NetId& drainNet)
        {
            drainNetId_ = drainNet;
        }

        void MosfetDevice::setGateNetId(const Core::NetId& gateNet)
        {
            gateNetId_ = gateNet;
        }

        void MosfetDevice::setSourceNetId(const Core::NetId& sourceNet)
        {
            sourceNetId_ = sourceNet;
        }

        void MosfetDevice::setBulkNetId(const Core::NetId& bulkNet)
        {
            bulkNetId_ = bulkNet;
        }

        void MosfetDevice::addDeviceToCircuit(Core::Circuit& circuit) const
        {
            assert(isComplete() && (getTechType().isN() || getTechType().isP()));
            Core::MosfetDeviceType mosfetDeviceType;
            Core::DeviceTypeName mosfet = Core::DeviceTypeNames::Mosfet;
            const Core::DeviceType& deviceType = getDeviceTypeRegister().findDeviceType(mosfet);
            Core::PinConnections mosfetPinConnections = deviceType.getEmptyPinToNetConnections();

            mosfetPinConnections.addNet(mosfetDeviceType.drain(), circuit.findNet(drainNetId_));
            mosfetPinConnections.addNet(mosfetDeviceType.gate(), circuit.findNet(gateNetId_));
            mosfetPinConnections.addNet(mosfetDeviceType.source(), circuit.findNet(sourceNetId_));
            mosfetPinConnections.addNet(mosfetDeviceType.bulk(), circuit.findNet(bulkNetId_));

            Core::Device& device = circuit.addDevice(getComponentId());
            device.setDeviceType(deviceType);
            device.setTechType(getTechType());

            mosfetPinConnections.addAndConnectPins(device);
        }

        bool MosfetDevice::isComplete() const
        {
            return drainNetId_.isInitialized() &&
                    gateNetId_.isInitialized() &&
                    sourceNetId_.isInitialized() &&
                    bulkNetId_.isInitialized();
        }
    }
}


