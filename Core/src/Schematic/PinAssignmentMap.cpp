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

#include "Core//incl/Schematic/PinAssignmentMap.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        void PinAssignmentMap::add(const TerminalId& termId, const PinAssignment& assignment)
        {
            assignmentMap_.insert(AssignmentMap::value_type(termId, assignment));
        }

        bool PinAssignmentMap::has(const TerminalId& termId) const
        {
            return assignmentMap_.find(termId) != assignmentMap_.end();
        }

        std::vector<TerminalId> PinAssignmentMap::getPinAssignmentTerminalIds() const
        {
        	std::vector<TerminalId> terminalIds;
            for(auto& it : assignmentMap_)
            {
                terminalIds.push_back(it.first);
            }
            return terminalIds;
        }

        std::vector<const PinAssignment*> PinAssignmentMap::getPinAssignments() const
        {
            std::vector<const PinAssignment*> pinAssignments;
            for(auto& it : assignmentMap_)
            {
                pinAssignments.push_back(&it.second);
            }
            return pinAssignments;
        }

        PinAssignment PinAssignmentMap::getAssignment(const TerminalId& termId) const
        {
            assert(has(termId));
            return assignmentMap_.at(termId);
        }

        bool PinAssignmentMap::hasPinAssignments() const
        {
            return !assignmentMap_.empty();
        }

        void PinAssignmentMap::setEvaluatedNetId(const TerminalId& terminalId, const NetId& evaluatedNetId)
        {
            PinAssignment& pinAssignment = assignmentMap_.at(terminalId);
            pinAssignment.setEvaluatedNetId(evaluatedNetId);
        }
    }
}

