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

#include "HSpice/incl/InputFile/DeviceLine/Device/DiodeDevice.h"
#include "Core/incl/Circuit/Device/DeviceType/DiodeDeviceType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

    DiodeDevice::DiodeDevice():
                AbstractDevice()
        {
        }

        DiodeDevice::~DiodeDevice()
        {
        }

        void DiodeDevice::setPlusNetId(const Core::NetId& plusNet)
        {
            plusNetId_ = plusNet;
        }

        void DiodeDevice::setMinusNetId(const Core::NetId& minusNet)
        {
            minusNetId_= minusNet;
        }

        void DiodeDevice::addDeviceToCircuit(Core::Circuit& circuit) const
        {
            assert(isComplete() && getTechType().isUndefined());

            Core::DiodeDeviceType* diodeDeviceType = new Core::DiodeDeviceType;

            Core::Device& device = circuit.addDevice(getComponentId());
            device.setDeviceType(*diodeDeviceType);
            device.setTechType(getTechType());

            Core::Pin& plusPin = device.addPin(diodeDeviceType->anode());
            plusPin.connect(circuit.findNet(plusNetId_));

            Core::Pin& minusPin = device.addPin(diodeDeviceType->cathode());
            minusPin.connect(circuit.findNet(minusNetId_));
        }

        bool DiodeDevice::isComplete() const
        {
            return plusNetId_.isInitialized() &&
                    minusNetId_.isInitialized();
        }

    }
}
