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

#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuitMap.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuit.h"
#include "HSpice/incl/InputFile/DeviceLine/Device/AbstractDevice.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        SubCircuit::SubCircuit()
        {
        }

        SubCircuit::~SubCircuit()
        {
        }

        void SubCircuit::setModelName(const ModelName& modelName)
        {
            modelName_ = modelName;
        }

        void SubCircuit::addTerminalIdentifier(const Core::TerminalId& terminalId)
        {
            terminalOrdering_.addTerminalId(terminalId);
        }

        void SubCircuit::addDevice(AbstractDevice& abstractLine)
        {
            devices_.addDevice(abstractLine);
        }

        void SubCircuit::addNetId(const Core::NetId& netId)
        {
            netIdentifiers_.addNetId(netId);
        }

        void SubCircuit::addNetIds(const std::vector<Core::NetId>& netIds)
        {
            netIdentifiers_.addNetIds(netIds);
        }

        void SubCircuit::addInstance(const Instance& subCircuitInstance)
        {
            instances_.addInstance(subCircuitInstance);
        }

        void SubCircuit::setInstantiatedSubCircuitMap(const InstantiatedSubCircuitMap& instantiatedSubCircuitMap)
        {
            instances_.setInstantiatedSubCircuitMap(instantiatedSubCircuitMap);
        }

        void SubCircuit::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            netIdentifiers_.setGlobalNetMap(globalNetMap);
            devices_.setAllGlobalNetMaps(globalNetMap);
            instances_.setGlobalNetMap(globalNetMap);
        }

        void InputFile::SubCircuit::setSupplyNetIdentifiers(const SupplyNetIdentifiers& supplyNetIdentifiers)
        {
            netIdentifiers_.setSupplyNetIdentifiers(supplyNetIdentifiers);
        }

        InstantiatedSubCircuit*  SubCircuit::buildNewInstantiatedSubCircuit() const
        {
            Core::Circuit* circuit = new Core::Circuit();
            Core::CircuitId circuitId = Core::CircuitName(getModelName().toStr()).createIdentifier();
            circuitId.setId(0);
            circuitId.setTechType(Core::TechType::undefined());
            logTrace("Creating Master for \""  << getModelName() << "\""  );
            circuit->setCircuitIdentifier(circuitId);

            netIdentifiers_.addNetsToCircuit(*circuit);
            devices_.addDevicesToCircuit(*circuit);
            terminalOrdering_.addTerminalsToCircuit(*circuit);
            instances_.addInstancesToCircuit(*circuit);

            InstantiatedSubCircuit* instantiatedSubCircuit = new InstantiatedSubCircuit();
            instantiatedSubCircuit->setModelName(getModelName());
            instantiatedSubCircuit->setMasterCircuit(circuit);
            instantiatedSubCircuit->setTerminalOrdering(getTerminalOrdering());

            return instantiatedSubCircuit;
        }

        ModelName SubCircuit::getModelName() const
        {
            assert(modelName_.isInitialized());
            return modelName_;
        }

        const TerminalOrdering& SubCircuit::getTerminalOrdering() const
        {
            assert(terminalOrdering_.isInitialized());
            return terminalOrdering_;
        }
    }
}


