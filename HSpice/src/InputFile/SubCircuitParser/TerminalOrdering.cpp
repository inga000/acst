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

#include "HSpice/incl/InputFile/SubCircuitParser/TerminalOrdering.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "HSpice/incl/InputFile/InstanceLine/InstanceTerminalOrdering.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include <sstream>

namespace HSpice {
    namespace InputFile {

        TerminalOrdering::TerminalOrdering():
                circuit_(NULL),
                instance_(NULL),
                instanceTerminalOrdering_(NULL),
                globalNetMap_(NULL)
        {
        }

        void TerminalOrdering::addTerminalId(const Core::TerminalId& terminalIdentifier)
        {
            terminalIds_.push_back(terminalIdentifier);
        }

        void TerminalOrdering::addTerminalsToCircuit(Core::Circuit& circuit) const
        {
            logTrace("Terminals to Add: " << terminalIds_);

            for(TerminalVector::const_iterator it = terminalIds_.begin(); it != terminalIds_.end(); it++)
            {
                Core::TerminalId terminalId = *it;
                logTrace("adding Terminal: " << terminalId);
                std::string terminalName = terminalId.getName();
                Core::Terminal& terminal = circuit.addTerminal(terminalId);

                Core::NetId netId = Core::NetName(terminalName).createRootIdentifier();
                Core::Net& net = circuit.findNet(netId);
                terminal.setNet(net);
            }
        }

        void TerminalOrdering::setCircuit(Core::Circuit& circuit)
        {
            circuit_ = &circuit;
        }

        void TerminalOrdering::setInstance(Core::Instance& instance)
        {
            instance_ = &instance;
        }

        void TerminalOrdering::setInstanceTerminalOrdering(
                const InstanceTerminalOrdering& instanceTerminalOrdering)
        {
            instanceTerminalOrdering_ = &instanceTerminalOrdering;
        }

        void TerminalOrdering::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            globalNetMap_ = &globalNetMap;
        }

        void TerminalOrdering::connectInstance()
        {
            assert(getInstanceTerminalOrdering().netIds_.size() == terminalIds_.size());

            std::vector<Core::NetId>::const_iterator netIt = getInstanceTerminalOrdering().netIds_.begin();
            TerminalVector::const_iterator termIt = terminalIds_.begin();

            while(termIt != terminalIds_.end())
            {
                Core::TerminalId terminalId = *termIt;
                Core::NetId netId = *netIt;

                Core::Net* net = NULL;
                if(getGlobalNetMap().hasNet(netId)) {
                    net = &getGlobalNetMap().getNet(netId);
                } else {
                    net = &getCircuit().findNet(netId);
                }

                assert(net != NULL);
                Core::InstanceTerminal& instanceTerminal = getInstance().addInstanceTerminal(terminalId);
                instanceTerminal.connect(*net);

                netIt++;
                termIt++;
            }
        }

        bool TerminalOrdering::isInitialized() const
        {
            return !terminalIds_.empty();
        }

        std::string TerminalOrdering::toStr() const
        {
            std::ostringstream oss;
            for(TerminalVector::const_iterator i =  terminalIds_.begin(); i != terminalIds_.end(); i++) {
                const Core::TerminalId & termId = *i;
                oss << termId << ";";
            }
            return oss.str();
        }

        Core::Circuit& TerminalOrdering::getCircuit()
        {
            assert(circuit_ != NULL);
            return *circuit_;
        }

        Core::Instance& TerminalOrdering::getInstance()
        {
            assert(instance_ != NULL);
            return *instance_;
        }

        const InstanceTerminalOrdering& TerminalOrdering::getInstanceTerminalOrdering() const
        {
            assert(instanceTerminalOrdering_ != NULL);
            return *instanceTerminalOrdering_;
        }

        const GlobalNetMap& TerminalOrdering::getGlobalNetMap() const
        {
            assert(globalNetMap_ != NULL);
            return *globalNetMap_;
        }

    }
}


