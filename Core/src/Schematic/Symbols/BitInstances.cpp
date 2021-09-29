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

#include "Core/incl/Schematic/Symbols/BitInstances.h"
#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        BitInstances::~BitInstances()
        {
            for(SymbolInstanceVector::const_iterator it = bitInstanceVector_.begin(); it != bitInstanceVector_.end(); it++)
            {
                delete *it;
            }
        }

        void BitInstances::addBitInstance(SymbolInstance& symbolInstance)
        {
            bitInstanceVector_.push_back(&symbolInstance);
        }

        std::vector<DeviceId> BitInstances::getBitInstanceNames() const
        {
            assert(isInitialized());

            std::vector<DeviceId> componentIds;
            for(auto& it : bitInstanceVector_)
            {
                const SymbolInstance* symbolInstance = it;
                DeviceId componentId = symbolInstance->getDeviceId();
                componentIds.push_back(componentId);
            }
            return componentIds;
        }

        const SymbolInstance& BitInstances::getBitInstance(int bit) const
        {
            assert(isInitialized());
            return *bitInstanceVector_.at(bit);
        }

        void BitInstances::updatePinAssignments(const PinAssignmentMap& pinAssignments)
        {
            for(SymbolInstanceVector::const_iterator it = bitInstanceVector_.begin(); it != bitInstanceVector_.end(); it++)
            {
                SymbolInstance* symbolInstance = *it;
                symbolInstance->updatePinAssignmentMap(pinAssignments);
            }
        }

        bool BitInstances::isInitialized() const
        {
            return bitInstanceVector_.size() > 1;
        }
    }
}
