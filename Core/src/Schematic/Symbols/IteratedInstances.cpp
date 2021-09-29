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

#include "Core/incl/Schematic/Symbols/IteratedInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        IteratedInstances::IteratedInstances()
        {
        }

        IteratedInstances::~IteratedInstances()
        {
            eraseAll();
        }

        void IteratedInstances::add(const DeviceId& componentId, IteratedInstance * iteratedInstance)
        {
            assert(iteratedInstance != NULL);
            iteratedInstanceMap_.insert(IteratedInstanceMap::value_type(componentId,iteratedInstance));
        }

        std::vector<const IteratedInstance*> IteratedInstances::getSymbols() const
        {
            std::vector<const IteratedInstance*> iteratedInstancesPointers;
            for(auto& it : iteratedInstanceMap_)
            {
                iteratedInstancesPointers.push_back(it.second);
            }
            return iteratedInstancesPointers;
        }

        bool IteratedInstances::has(const Core::DeviceId& componentId) const
        {
            return iteratedInstanceMap_.find(componentId) != iteratedInstanceMap_.end();
        }

        void IteratedInstances::setEvaluatedPinAssignmentId(const DeviceId& componentId,
                const TerminalId& terminalId, const NetId& evaluatedNetId)
        {
            assert(has(componentId));
            IteratedInstance* iteratedInstance = iteratedInstanceMap_.at(componentId);
            iteratedInstance->setEvaluatedPinAssignmentId(terminalId, evaluatedNetId);
            iteratedInstance->updatePinAssignmentsOfBitInstances();
        }

        void IteratedInstances::eraseAll()
        {
            for(IteratedInstanceMap::iterator it = iteratedInstanceMap_.begin(); it != iteratedInstanceMap_.end(); it++) {
                IteratedInstance * iteratedInstance = it->second;
                delete iteratedInstance;
            }
        }

    }
}
