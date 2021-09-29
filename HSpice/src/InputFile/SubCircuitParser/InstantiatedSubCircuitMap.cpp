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

#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuitMap.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuit.h"
#include "Core/incl/Circuit/TopCircuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        InstantiatedSubCircuitMap::InstantiatedSubCircuitMap()
        {
        }

        InstantiatedSubCircuitMap::~InstantiatedSubCircuitMap()
        {
            subCircuitMap_.clear();
        }

        void InstantiatedSubCircuitMap::add(const ModelName& modelName, InstantiatedSubCircuit* masterCircuit)
        {
            assert(!has(modelName));
            subCircuitMap_[modelName] = masterCircuit;
        }

        InstantiatedSubCircuit* InstantiatedSubCircuitMap::findMasterCircuit(const ModelName& modelName) const
        {
            logTrace("Looking up: " << modelName);
            assert(has(modelName));
            return subCircuitMap_.find(modelName)->second;
        }

        void InstantiatedSubCircuitMap::copyMasterPointersIntoTopCircuit(Core::TopCircuit& topCircuit) const
        {
            for(SubCircuitMap::const_iterator it = subCircuitMap_.begin(); it != subCircuitMap_.end(); it++)
            {
                InstantiatedSubCircuit* instantiatedSubCircuit = it->second;;
                Core::Circuit& circuit = instantiatedSubCircuit->getMasterCircuit();
                topCircuit.addMaster(circuit);
            }
        }

        bool InstantiatedSubCircuitMap::has(const ModelName& modelName) const
        {
            return subCircuitMap_.find(modelName) != subCircuitMap_.end();
        }

    }
}
