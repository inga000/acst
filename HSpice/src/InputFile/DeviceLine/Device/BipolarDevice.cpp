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

#include "HSpice/incl/InputFile/DeviceLine/Device/BipolarDevice.h"
#include "Core/incl/Circuit/Device/DeviceType/BipolarDeviceType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        BipolarDevice::BipolarDevice():
                AbstractDevice()
        {
        }

        BipolarDevice::~BipolarDevice()
        {
        }

        void BipolarDevice::setCollectorNetId(const Core::NetId& collectorNet)
        {
            collectorNetId_ = collectorNet;
        }

        void BipolarDevice::setBaseNetId(const Core::NetId& baseNet)
        {
            baseNetId_ = baseNet;
        }

        void BipolarDevice::setEmitterNetId(const Core::NetId& emitterNet)
        {
            emitterNetId_ = emitterNet;
        }

        void BipolarDevice::addDeviceToCircuit(Core::Circuit& circuit) const
        {
            assert(isComplete() && (getTechType().isN() || getTechType().isP()));

            Core::BipolarDeviceType* bipolarDeviceType = new Core::BipolarDeviceType;

            Core::Device& device = circuit.addDevice(getComponentId());
            device.setDeviceType(*bipolarDeviceType);
            device.setTechType(getTechType());

            Core::Pin& collectorPin = device.addPin(bipolarDeviceType->collector());
            collectorPin.connect(circuit.findNet(collectorNetId_));

            Core::Pin& basePin = device.addPin(bipolarDeviceType->base());
            basePin.connect(circuit.findNet(baseNetId_));

            Core::Pin& emitterPin = device.addPin(bipolarDeviceType->emitter());
            emitterPin.connect(circuit.findNet(emitterNetId_));
        }

        bool BipolarDevice::isComplete() const
        {
            return collectorNetId_.isInitialized() &&
                    baseNetId_.isInitialized() &&
                    emitterNetId_.isInitialized();
        }
    }
}


