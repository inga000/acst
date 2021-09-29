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

#include "Core/incl/Schematic/Symbols/SymbolInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        SymbolInstances::SymbolInstances()
        {
        }

        SymbolInstances::~SymbolInstances()
        {
            eraseAll();
        }

        void SymbolInstances::add(const DeviceId& componentId, SymbolInstance * symbolInstance)
        {
            assert(symbolInstance != NULL);
            symbolInstanceMap_.insert(SymbolInstanceMap::value_type(componentId, symbolInstance));
        }

       void SymbolInstances::remove(const DeviceId& componentId)
       {
           assert(has(componentId));
           SymbolInstance* symbolInstance = symbolInstanceMap_[componentId];
           symbolInstanceMap_.erase(componentId);
           delete symbolInstance;
       }

        std::vector<const SymbolInstance*> SymbolInstances::computeSymbolInstances(const TerminalId& terminalId) const
        {
            std::vector<const SymbolInstance*> symbolPointers;
            for(auto& it : symbolInstanceMap_)
            {
                const SymbolInstance* symbolInstance = it.second;
                if(symbolInstance->hasTerminal(terminalId)) {
                    symbolPointers.push_back(symbolInstance);
                }
            }
            return symbolPointers;
        }

        std::vector<const SymbolInstance*> SymbolInstances::getSymbols() const
        {
            std::vector<const SymbolInstance*> symbolPointers;
            for(auto& it : symbolInstanceMap_)
            {
                const SymbolInstance* symbolInstance = it.second;
                symbolPointers.push_back(symbolInstance);
            }
            return symbolPointers;
        }

        SymbolInstance& SymbolInstances::find(const DeviceId& componentId) const
        {
            assert(has(componentId));
            return *symbolInstanceMap_.find(componentId)->second;
        }

        bool SymbolInstances::has(const DeviceId& componentId) const
        {
            return symbolInstanceMap_.find(componentId) != symbolInstanceMap_.end();
        }

        void SymbolInstances::setEvaluatedPinAssignmentId(const DeviceId& componentId,
                const TerminalId& terminalId, const NetId& evaluatedNetId)
        {
            assert(has(componentId));
            SymbolInstance* symbolInstance = symbolInstanceMap_.find(componentId)->second;
            symbolInstance->setEvaluatedPinAssignmentId(terminalId, evaluatedNetId);
        }

        void SymbolInstances::eraseAll()
        {
            for(SymbolInstanceMap::iterator it = symbolInstanceMap_.begin(); it != symbolInstanceMap_.end(); it++) {
                SymbolInstance * symbolInstance = it->second;
                delete symbolInstance;
            }
        }
    }
}

