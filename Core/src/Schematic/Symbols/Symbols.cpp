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

#include "Core/incl/Schematic/Symbols/Symbols.h"
#include "Core/incl/Schematic/Symbols/SymbolMaster.h"
#include "Core/incl/Schematic/Symbols/IteratedInstance.h"
#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        Symbols::Symbols()
        {
        }

        void Symbols::addSymbolInstance(const DeviceId& componentId, SymbolInstance * symbolInstance)
        {
            symbolInstances_.add(componentId, symbolInstance);
        }

        void Symbols::addIteratedInstance(const DeviceId& componentId, IteratedInstance* iteratedInstance)
        {
            iteratedInstances_.add(componentId, iteratedInstance);
        }

        void Symbols::addSymbolMaster(const CellTripleId& cellTriple, SymbolMaster * symbolMaster)
        {
            symbolMasters_.add(cellTriple, symbolMaster);
        }

        bool Symbols::hasSymbolInstance(const DeviceId& componentId) const
        {
            return symbolInstances_.has(componentId);
        }

        bool Symbols::hasSymbolMaster(const CellTripleId& cellTriple) const
        {
            return symbolMasters_.has(cellTriple);
        }

        SymbolInstance& Symbols::findSymbolInstance(const DeviceId& componentId) const
        {
            assert(hasSymbolInstance(componentId));
            return symbolInstances_.find(componentId);
        }

        SymbolMaster& Symbols::findSymbolMaster(const CellTripleId& cellTriple)
        {
            assert(hasSymbolMaster(cellTriple));
            return *symbolMasters_.findMaster(cellTriple);
        }

        const SymbolMaster& Symbols::findSymbolMaster(const CellTripleId& cellTriple) const
        {
            assert(hasSymbolMaster(cellTriple));
            return *symbolMasters_.findMaster(cellTriple);
        }

        std::vector<const SymbolInstance*> Symbols::computeSymbolInstancesWithTerminal(const TerminalId& terminalId) const
        {
            return symbolInstances_.computeSymbolInstances(terminalId);
        }

        std::vector<const SymbolInstance*> Symbols::getSymbolInstances() const
        {
            return symbolInstances_.getSymbols();
        }

        std::vector<const IteratedInstance*> Symbols::getIteratedInstances() const
        {
            return iteratedInstances_.getSymbols();
        }

        void Symbols::setEvaluatedPinAssignmentId(const DeviceId& componentId, const TerminalId& terminalId,
                const NetId& evaluatedNetId)
        {
            if(symbolInstances_.has(componentId)) {
                symbolInstances_.setEvaluatedPinAssignmentId(componentId, terminalId, evaluatedNetId);
            } else if(iteratedInstances_.has(componentId)){
                iteratedInstances_.setEvaluatedPinAssignmentId(componentId, terminalId, evaluatedNetId);
            } else {
                assert(false);
            }
        }
    }
}
