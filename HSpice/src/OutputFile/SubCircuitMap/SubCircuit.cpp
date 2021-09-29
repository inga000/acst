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

#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuit.h"
#include "HSpice/incl/OutputFile/HSpiceString.h"
#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrderingMap.h"
#include "HSpice/incl/OutputFile/DeviceLine.h"
#include "Control/incl/File/OutputFile.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

        SubCircuit::SubCircuit():
            circuit_(NULL),
            position_(0),
            terminalOrderingMap_(NULL)
        {
        }

        void SubCircuit::setCircuit(const Core::Circuit& circuit)
        {
            circuit_ = &circuit;
        }

        void SubCircuit::setPosition(int position)
        {
            position_ = position;
        }

        void SubCircuit::setTerminalOrderingMap(const TerminalOrderingMap& terminalOrderingMap)
        {
            terminalOrderingMap_ = &terminalOrderingMap;
        }

        int SubCircuit::getPosition() const
        {
            assert(position_ >= 0);
            return position_;
        }

        Core::CircuitId SubCircuit::getCircuitIdentifier() const
        {
            return getCircuit().getCircuitIdentifier();
        }

        TerminalOrdering SubCircuit::computeTerminalOrdering() const
        {
            TerminalOrdering terminalOrdering;
            std::vector<const Core::Terminal*> terminals = getCircuit().findTerminals();
            for(auto& it : terminals)
            {
                Core::TerminalId terminalId = it->getIdentifier();
                terminalOrdering.append(terminalId);
            }
            return terminalOrdering;
        }

        std::string SubCircuit::toStr()
        {
            const Core::CircuitId& circuitId = getCircuit().getCircuitIdentifier();

            std::ostringstream message;
            message << "Circuit: " << circuitId << ", Position: " << position_;
            return message.str();
        }

        void SubCircuit::write(Control::OutputFile& outputFile) const
        {
            const Core::CircuitId& circuitId = getCircuit().getCircuitIdentifier();
            std::string circuitName = HSpiceString::convertToHSpice(circuitId);

            outputFile << ".subckt " << circuitName << " ";
            writeTerminals(outputFile);
            writeDevices(outputFile);
            writeInstances(outputFile);

            outputFile << ".ends " << circuitName << "\n\n";
        }

        void SubCircuit::writeTerminals(Control::OutputFile& outputFile) const
        {
            const Core::CircuitId& circuitId = getCircuit().getCircuitIdentifier();
            TerminalOrdering terminalOrdering = getTerminalOrderingMap().findTerminalOrdering(circuitId);
            terminalOrdering.writeTerminals(outputFile);
        }

        void SubCircuit::writeDevices(Control::OutputFile& outputFile) const
        {
            std::vector<const Core::Device*> devices = getCircuit().findDevices();
            for(auto& it : devices)
            {
                const Core::Device* device = it;
                DeviceLine deviceLine;
                deviceLine.setDevice(*device);
                deviceLine.setOutputFile(outputFile);
                deviceLine.write();
            }
        }

        void SubCircuit::writeInstances(Control::OutputFile& outputFile) const
        {
            std::vector<const Core::Instance*> instances = getCircuit().findInstances();
            for(auto& instIt : instances)
            {
                const Core::Instance* instance = instIt;
                const Core::InstanceId instanceId = instance->getIdentifier();
                std::string instanceName = HSpiceString::convertToHSpice(instanceId);
                outputFile << instanceName << " ";

                Core::CircuitId circuitId = instance->getMaster().getCircuitIdentifier();
                TerminalOrdering terminalOrdering = getTerminalOrderingMap().findTerminalOrdering(circuitId);
                terminalOrdering.setInstance(*instance);
                terminalOrdering.writeInstanceTerminals(outputFile);

                std::string circuitName = HSpiceString::convertToHSpice(circuitId);
                outputFile << circuitName << "\n";
            }
        }

        const Core::Circuit& SubCircuit::getCircuit() const
        {
            assert(circuit_ != NULL);
            return *circuit_;
        }

        const TerminalOrderingMap& SubCircuit::getTerminalOrderingMap() const
        {
            assert(terminalOrderingMap_ != NULL);
            return *terminalOrderingMap_;
        }

    }
}


