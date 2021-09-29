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

#include "HSpice/incl/InputFile/InstanceLine/Instances.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include "HSpice/incl/InputFile/SubCircuitParser/TerminalOrdering.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuit.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuitMap.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        Instances::Instances():
                instantiatedSubCircuitMap_(NULL),
                globalNetMap_(NULL)
        {
        }

        void Instances::addInstance(const Instance& subcircuitInstance)
        {
            instanceVector_.push_back(subcircuitInstance);
        }

        void Instances::setInstantiatedSubCircuitMap(const InstantiatedSubCircuitMap& instantiatedSubCircuitMap)
        {
            instantiatedSubCircuitMap_ = &instantiatedSubCircuitMap;
        }

        void Instances::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            globalNetMap_ = &globalNetMap;
        }

        void Instances::addInstancesToCircuit(Core::Circuit& circuit) const
        {
            for(auto& it : instanceVector_)
            {
                Instance instance = it;
                Core::InstanceId instanceId = instance.getInstanceId();
                ModelName modelName = instance.getModelName();
                logTrace("adding Instance: \"" << instanceId << "\"");
                logTrace("of Model: \"" << modelName << "\"");

                InstantiatedSubCircuit* instantiatedSubCircuit = getInstantiatedSubCircuitMap().findMasterCircuit(modelName);
                const Core::Circuit& masterCircuit = instantiatedSubCircuit->getMasterCircuit();

                Core::Instance& coreInstance = circuit.addInstance(instanceId);
                coreInstance.setMaster(masterCircuit);

                InstanceTerminalOrdering instanceTerminalOrdering = instance.getInstanceTerminalOrdering();
                logTrace("InstTermOrder: " << instanceTerminalOrdering);
                TerminalOrdering terminalOrdering = instantiatedSubCircuit->getTerminalOrdering();
                logTrace("TermOrder: " << terminalOrdering);
                terminalOrdering.setCircuit(circuit);
                terminalOrdering.setInstance(coreInstance);
                terminalOrdering.setInstanceTerminalOrdering(instanceTerminalOrdering);
                terminalOrdering.setGlobalNetMap(getGlobalNetMap());
                terminalOrdering.connectInstance();
            }
        }

        const InstantiatedSubCircuitMap& Instances::getInstantiatedSubCircuitMap() const
        {
            assert(instantiatedSubCircuitMap_ != NULL);
            return *instantiatedSubCircuitMap_;
        }

        const GlobalNetMap& Instances::getGlobalNetMap() const
        {
            assert(globalNetMap_ != NULL);
            return *globalNetMap_;
        }

    }
}


