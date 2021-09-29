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

#include "HSpice/incl/InputFile/DeviceLine/Devices.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/AbstractDevice.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        Devices::Devices()
        {
        }

        Devices::~Devices()
        {
            deleteAllDevices();
        }

        void Devices::addDevice(AbstractDevice& device)
        {
            deviceVector_.push_back(&device);
        }

        void Devices::addDevicesToCircuit(Core::Circuit& circuit) const
        {
            for(DeviceVector::const_iterator it = deviceVector_.begin(); it != deviceVector_.end(); it++)
            {
                const AbstractDevice* current = *it;
                current->addDeviceToCircuit(circuit);
            }
        }

        void Devices::setAllGlobalNetMaps(const GlobalNetMap& globalNetMap)
        {
            for(DeviceVector::iterator it = deviceVector_.begin(); it != deviceVector_.end(); it++)
            {
                AbstractDevice* current = *it;
                current->setGlobalNetMap(globalNetMap);
            }
        }

        void Devices::deleteAllDevices()
        {
            for(DeviceVector::iterator it = deviceVector_.begin(); it != deviceVector_.end(); it++)
            {
                delete *it;
            }
        }

    }
}

