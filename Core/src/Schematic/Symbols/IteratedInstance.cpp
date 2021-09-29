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

#include "Core/incl/Schematic/Symbols/IteratedInstance.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        IteratedInstance::IteratedInstance(const IteratedInstance& other) :
                SymbolInstance(other)
        {
            addBitInstances(other.getBitInstanceIdentifiers());
        }

        bool IteratedInstance::hasBitInstances() const
        {
            return true;
        }

        std::vector<DeviceId> IteratedInstance::getBitInstanceIdentifiers() const
        {
            assert(hasBitInstances());
            return bitInstances_.getBitInstanceNames();
        }

        void IteratedInstance::addBitInstance(const DeviceId& bitId)
        {
            SymbolInstance* bitInstance = new SymbolInstance(*this);
            bitInstance->setComponentId(bitId);
            bitInstances_.addBitInstance(*bitInstance);
        }

        void IteratedInstance::addBitInstances(const std::vector<DeviceId>& bitIds)
        {
        	for(auto& it : bitIds)
        	{
        		addBitInstance(it);
        	}
        }

        const SymbolInstance& IteratedInstance::getBit(int bit) const
        {
            assert(hasBitInstances());
            return bitInstances_.getBitInstance(bit);
        }

        void IteratedInstance::updatePinAssignmentsOfBitInstances()
        {
            assert(hasBitInstances());
            bitInstances_.updatePinAssignments(getPinAssignmentMap());
        }
    }
}

