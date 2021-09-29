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

#ifndef HSPICE_INPUTFILE_SUBCIRCUIT_H_
#define HSPICE_INPUTFILE_SUBCIRCUIT_H_

#include "HSpice/incl/InputFile/ModelName.h"
#include "HSpice/incl/InputFile/DeviceLine/Devices.h"
#include "HSpice/incl/InputFile/NetIdentifiers/NetIdentifiers.h"
#include "HSpice/incl/InputFile/SubCircuitParser/TerminalOrdering.h"
#include "HSpice/incl/InputFile/InstanceLine/Instances.h"
#include <vector>

namespace Core {
    class Circuit;
    class NetId;
    class TerminalId;
}

namespace HSpice {
    namespace InputFile {

        class InstantiatedSubCircuit;
        class InstantiatedSubCircuitMap;
        class AbstractDevice;
        class SupplyNetIdentifiers;

        class SubCircuit
        {
        public:
            SubCircuit();
            ~SubCircuit();

            void setModelName(const ModelName& modelName);
            void addTerminalIdentifier(const Core::TerminalId& terminalId);
            void addDevice(AbstractDevice& abstractLine);
            void addNetId(const Core::NetId& netId);
            void addNetIds(const std::vector<Core::NetId>& netIds);
            void addInstance(const Instance& subCircuitInstance);

            void setInstantiatedSubCircuitMap(const InstantiatedSubCircuitMap& instantiatedSubCircuitMap);
            void setGlobalNetMap(const GlobalNetMap& globalNetMap);
            void setSupplyNetIdentifiers(const SupplyNetIdentifiers& supplyNetIdentifiers);

            InstantiatedSubCircuit* buildNewInstantiatedSubCircuit() const;

        private:
            const TerminalOrdering& getTerminalOrdering() const;
            ModelName getModelName() const;

        private:
            ModelName modelName_;
            TerminalOrdering terminalOrdering_;
            NetIdentifiers netIdentifiers_;
            Devices devices_;
            Instances instances_;
        };

    }
}


#endif
