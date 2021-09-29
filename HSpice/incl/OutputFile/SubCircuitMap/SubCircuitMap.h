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

#ifndef HSPICE_OUTPUTFILE_SUBCIRCUITMAP_H_
#define HSPICE_OUTPUTFILE_SUBCIRCUITMAP_H_

#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrdering.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include <string>
#include <map>

namespace Control {
    class OutputFile;
}

namespace Core {
    class Circuit;
    class InstanceTerminal;
}

namespace HSpice {
    namespace OutputFile {

        class SubCircuit;
        class TerminalOrderingMap;

        class SubCircuitMap
        {
        private:
            typedef std::map<Core::CircuitId, SubCircuit> CircuitIdSubCircuitMap;

        public:
            SubCircuitMap();

            void setTerminalOrderingMap(TerminalOrderingMap& terminalOrderingMap);

            void add(const Core::CircuitId& circuitId, SubCircuit& subCircuit);
            SubCircuit& findSubCircuit(const Core::CircuitId& circuitId);
            bool hasSubCircuit(const Core::CircuitId& circuitId) const;

            void buildTerminalOrderingMap();
            void writeSubCircuits(Control::OutputFile& outputFile) const;

        private:
            int computeDeepestSubCircuitPosition() const;
            std::vector<SubCircuit> computeSubCircuitsWithPosition(const int position) const;

            TerminalOrderingMap& getTerminalOrderingMap();
            const TerminalOrderingMap& getTerminalOrderingMap() const;

        private:
            TerminalOrderingMap* terminalOrderingMap_;
            CircuitIdSubCircuitMap subCircuitMap_;
        };

    }
}


#endif
