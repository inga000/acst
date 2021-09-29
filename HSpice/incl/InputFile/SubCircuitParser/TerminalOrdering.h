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

#ifndef HSPICE_INPUTFILE_TERMINALORDERING_H_
#define HSPICE_INPUTFILE_TERMINALORDERING_H_

#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Common/Object.h"
#include <vector>

namespace Core {

    class Circuit;
    class Instance;

}

namespace HSpice {
    namespace InputFile {

        class GlobalNetMap;
        class InstanceTerminalOrdering;

        class TerminalOrdering : public Core::Object
        {
        private:
            typedef std::vector<Core::TerminalId> TerminalVector;

        public:
            TerminalOrdering();

            void addTerminalId(const Core::TerminalId& terminalId);
            void addTerminalsToCircuit(Core::Circuit& circuit) const;

            void setCircuit(Core::Circuit& circuit);
            void setInstance(Core::Instance& instance);
            void setInstanceTerminalOrdering(const InstanceTerminalOrdering& instanceTerminalOrdering);
            void setGlobalNetMap(const GlobalNetMap& globalNetMap);
            void connectInstance();

            bool isInitialized() const;

            std::string toStr() const;

        private:
            Core::Circuit& getCircuit();
            Core::Instance& getInstance();
            const InstanceTerminalOrdering& getInstanceTerminalOrdering() const;
            const GlobalNetMap& getGlobalNetMap() const;

            TerminalVector terminalIds_;

            Core::Circuit* circuit_;
            Core::Instance* instance_;
            const InstanceTerminalOrdering * instanceTerminalOrdering_;
            const GlobalNetMap* globalNetMap_;
        };

    }
}


#endif
