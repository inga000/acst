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

#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuitMap.h"
#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuit.h"
#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrderingMap.h"
#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrdering.h"
#include "HSpice/incl/OutputFile/DeviceLine.h"
#include "Control/incl/File/OutputFile.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

        SubCircuitMap::SubCircuitMap():
                terminalOrderingMap_(NULL)
        {
        }

        void SubCircuitMap::add(const Core::CircuitId& circuitId, SubCircuit& subCircuit)
        {
            assert(!hasSubCircuit(circuitId));
            subCircuitMap_[circuitId] = subCircuit;
        }

        SubCircuit& SubCircuitMap::findSubCircuit(const Core::CircuitId& circuitId)
        {
            assert(hasSubCircuit(circuitId));
            SubCircuit& subCircuit = subCircuitMap_.at(circuitId);
            return subCircuit;
        }

        bool SubCircuitMap::hasSubCircuit(const Core::CircuitId& circuitId) const
        {
            return subCircuitMap_.find(circuitId) != subCircuitMap_.end();
        }

        int SubCircuitMap::computeDeepestSubCircuitPosition() const
        {
            int deepestSubCircuitPosition = 0;
            for(CircuitIdSubCircuitMap::const_iterator it = subCircuitMap_.begin(); it != subCircuitMap_.end(); it++)
            {
                const SubCircuit& subCircuit = it->second;
                if(deepestSubCircuitPosition < subCircuit.getPosition()) {
                    deepestSubCircuitPosition = subCircuit.getPosition();
                }
            }

            return deepestSubCircuitPosition;
        }

        std::vector<SubCircuit> SubCircuitMap::computeSubCircuitsWithPosition(const int position) const
        {
        	std::vector<SubCircuit> subCircuitItems;
            for(auto& it : subCircuitMap_)
            {
                const SubCircuit& subCircuit = it.second;
                if(subCircuit.getPosition() == position) {
                    subCircuitItems.push_back(subCircuit);
                }
            }
            return subCircuitItems;
        }

        void SubCircuitMap::setTerminalOrderingMap(TerminalOrderingMap& terminalOrderingMap)
        {
            terminalOrderingMap_ = &terminalOrderingMap;
        }

        void SubCircuitMap::buildTerminalOrderingMap()
        {
            for(CircuitIdSubCircuitMap::const_iterator it = subCircuitMap_.begin(); it != subCircuitMap_.end(); it++)
            {
                const SubCircuit& subCircuit = it->second;
                const Core::CircuitId& circuitId = subCircuit.getCircuitIdentifier();

                TerminalOrdering terminalOrdering = subCircuit.computeTerminalOrdering();
                getTerminalOrderingMap().add(circuitId, terminalOrdering);
            }
        }

        void SubCircuitMap::writeSubCircuits(Control::OutputFile& outputFile) const
        {
            int deepestSubCircuitPosition = computeDeepestSubCircuitPosition();
            for(int i = deepestSubCircuitPosition; i > 0; i--)
            {
            	std::vector<SubCircuit> subCircuits = computeSubCircuitsWithPosition(i);

                for(auto& it : subCircuits)
                {
                    SubCircuit subCircuit = it;
                    subCircuit.setTerminalOrderingMap(getTerminalOrderingMap());
                    subCircuit.write(outputFile);
                }
            }

        }

        TerminalOrderingMap& SubCircuitMap::getTerminalOrderingMap()
        {
            assert(terminalOrderingMap_ != NULL);
            return *terminalOrderingMap_;
        }

        const TerminalOrderingMap& SubCircuitMap::getTerminalOrderingMap() const
        {
            assert(terminalOrderingMap_ != NULL);
            return *terminalOrderingMap_;
        }

    }
}
