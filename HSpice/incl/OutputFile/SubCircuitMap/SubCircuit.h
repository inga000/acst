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

#ifndef HSPICE_OUTPUTFILE_SUBCIRCUIT_H_
#define HSPICE_OUTPUTFILE_SUBCIRCUIT_H_

#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrdering.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Common/Object.h"

namespace Control {
    class OutputFile;
}

namespace Core {
    class Circuit;
}

namespace HSpice {
    namespace OutputFile {

        class TerminalOrderingMap;

        class SubCircuit : public Core::Object
        {
        public:
            SubCircuit();

            void setCircuit(const Core::Circuit& circuit);
            void setTerminalOrderingMap(const TerminalOrderingMap& terminalOrderingMap);
            void setPosition(int position);

            int getPosition() const;

            Core::CircuitId getCircuitIdentifier() const;
            TerminalOrdering computeTerminalOrdering() const;

            std::string toStr();

            void write(Control::OutputFile& outputFile) const;

        private:
            void writeTerminals(Control::OutputFile &outputFile) const;
            void writeDevices(Control::OutputFile& outputFile) const;
            void writeInstances(Control::OutputFile& outputFile) const;

            const Core::Circuit& getCircuit() const;
            const TerminalOrderingMap& getTerminalOrderingMap() const;

        private:
            int position_;

            const Core::Circuit* circuit_;
            const TerminalOrderingMap* terminalOrderingMap_;
        };

    }
}

#endif
