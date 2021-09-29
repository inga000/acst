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

#ifndef CORE_SCHEMATIC_SYMBOLS_H_
#define CORE_SCHEMATIC_SYMBOLS_H_

#include "Core/incl/Schematic/Symbols/IteratedInstances.h"
#include "Core/incl/Schematic/Symbols/SymbolInstances.h"
#include "Core/incl/Schematic/Symbols/SymbolMasters.h"

namespace Core {
    namespace Schematic {

        class SymbolMaster;

        class Symbols
        {
        private:
            Symbols(const Symbols & other); //not implemented
            void operator=(const Symbols & other); //not implemented

        public:
            Symbols();

            void addSymbolInstance(const DeviceId& componentId, SymbolInstance * symbolInstance);
            void addIteratedInstance(const DeviceId& componentId, IteratedInstance* iteratedInstance);
            void addSymbolMaster(const CellTripleId & cellTriple, SymbolMaster * symbolMaster);

            bool hasSymbolInstance(const DeviceId& componentId) const;
            bool hasSymbolMaster(const CellTripleId & cellTriple) const;
            SymbolInstance& findSymbolInstance(const DeviceId& componentId) const;
            SymbolMaster & findSymbolMaster(const CellTripleId & cellTriple);
            const SymbolMaster & findSymbolMaster(const CellTripleId& cellTriple) const;

            std::vector<const SymbolInstance*> computeSymbolInstancesWithTerminal(const TerminalId& terminalId) const;
            std::vector<const SymbolInstance*> getSymbolInstances() const;
            std::vector<const IteratedInstance*> getIteratedInstances() const;

            void setEvaluatedPinAssignmentId(const DeviceId& componentId,
                            const TerminalId& terminalId, const NetId& evaluatedNetId);

        private:
            SymbolInstances symbolInstances_;
            IteratedInstances iteratedInstances_;
            SymbolMasters symbolMasters_;
        };
    }
}


#endif
