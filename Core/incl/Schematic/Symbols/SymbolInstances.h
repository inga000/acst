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

#ifndef CORE_SCHEMATIC_SYMBOLINSTANCES_H_
#define CORE_SCHEMATIC_SYMBOLINSTANCES_H_

#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include <map>

namespace Core {
    namespace Schematic {

        class SymbolInstances
        {
        private:
            typedef std::map<DeviceId, SymbolInstance*> SymbolInstanceMap;

            SymbolInstances(const SymbolInstances & other); // don't implement
            void operator=(const SymbolInstances & other); // don't implement

        public:
            SymbolInstances();
            ~SymbolInstances();

            void add(const DeviceId& componentId, SymbolInstance * symbolInstance);
            void remove(const DeviceId& componentId);

            std::vector<const SymbolInstance*> computeSymbolInstances(const TerminalId& terminalId) const;
            std::vector<const SymbolInstance*> getSymbols() const;
            SymbolInstance& find(const DeviceId& componentId) const;

            bool has(const DeviceId& componentId) const;
            void setEvaluatedPinAssignmentId(const DeviceId& componentId,
                    const TerminalId& terminalId, const NetId& evaluatedNetId);

        private:
            void eraseAll();

            SymbolInstanceMap symbolInstanceMap_;
        };

    }
}




#endif
