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

#include "HSpice/incl/OutputFile/TerminalOrderingMap/TerminalOrdering.h"
#include "HSpice/incl/OutputFile/HSpiceString.h"
#include "Control/incl/File/OutputFile.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

        TerminalOrdering::TerminalOrdering():
                instance_(NULL)
        {
        }

        void TerminalOrdering::append(const Core::TerminalId& terminalId)
        {
            terminalIds_.push_back(terminalId);
        }

        void TerminalOrdering::setInstance(const Core::Instance& instance)
        {
            instance_ = &instance;
        }

        void TerminalOrdering::writeTerminals(Control::OutputFile& outputFile)
        {
            for(TerminalVector::const_iterator it = terminalIds_.begin(); it != terminalIds_.end(); it++)
            {
                const Core::TerminalId& termId = *it;
                std::string terminalName = HSpiceString::convertToHSpice(termId);
                outputFile << terminalName << " ";
            }
            outputFile << "\n";
        }

        void TerminalOrdering::writeInstanceTerminals(Control::OutputFile& outputFile)
        {
            for(TerminalVector::const_iterator it = terminalIds_.begin(); it != terminalIds_.end(); it++)
            {
                const Core::TerminalId& termId = *it;
                const Core::Net& net = computeInstanceTerminalNet(termId);
                const Core::NetId& netId = net.getIdentifier();
                std::string netName = HSpiceString::convertToHSpice(netId);
                outputFile << netName << " ";
            }
        }

        const Core::Net& TerminalOrdering::computeInstanceTerminalNet(const Core::TerminalId& terminalId) const
        {
            const Core::Net* net = NULL;
            std::vector<const Core::InstanceTerminal*> instanceTerminals = getInstance().getInstanceTerminals();
            for(auto& it : instanceTerminals)
            {
                const Core::InstanceTerminal& instTerm = *it;
                Core::TerminalId instTermId = instTerm.getIdentifier();
                if(instTermId == terminalId) {
                    const Core::Net& terminalNet = instTerm.getNet();
                    net = &terminalNet;
                    break;
                }
            }

            assert(net != NULL);
            return *net;
        }

        const Core::Instance& TerminalOrdering::getInstance() const
        {
            assert(instance_ != NULL);
            return *instance_;
        }

    }
}

