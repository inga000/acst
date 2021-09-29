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

#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuits.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuit.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuitMap.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "HSpice/incl/InputFile/ModelName.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/TopCircuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        SubCircuits::SubCircuits()
        {
        }

        SubCircuits::~SubCircuits()
        {
            deleteAllSubCircuits();
        }

        void SubCircuits::add(SubCircuit* subCircuitMaster)
        {
            subCircuitVector_.push_back(subCircuitMaster);
        }

        void SubCircuits::setAllGlobalNetMaps(const GlobalNetMap& globalNetMap)
        {
            for(SubCircuitVector::const_iterator it = subCircuitVector_.begin(); it != subCircuitVector_.end(); it++)
            {
                SubCircuit* subCircuit = *it;
                subCircuit->setGlobalNetMap(globalNetMap);
            }
        }

        void HSpice::InputFile::SubCircuits::setAllSupplyNetIdentifiers(const SupplyNetIdentifiers& supplyNetIdentifiers)
        {
            for(SubCircuitVector::const_iterator it = subCircuitVector_.begin(); it != subCircuitVector_.end(); it++)
            {
                SubCircuit* subCircuit = *it;
                subCircuit->setSupplyNetIdentifiers(supplyNetIdentifiers);
            }
        }

        void SubCircuits::setAllInstantiatedSubCircuitMaps(const InstantiatedSubCircuitMap& instantiatedSubCircuitMap)
        {
            for(SubCircuitVector::const_iterator it = subCircuitVector_.begin(); it != subCircuitVector_.end(); it++)
            {
                SubCircuit* subCircuit = *it;
                subCircuit->setInstantiatedSubCircuitMap(instantiatedSubCircuitMap);
            }
        }

        void SubCircuits::fillInstantiatedSubCircuitMap(InstantiatedSubCircuitMap& instantiatedSubCircuitMap)
        {
            for(SubCircuitVector::const_iterator it = subCircuitVector_.begin(); it != subCircuitVector_.end(); it++)
            {
                SubCircuit* subCircuit = *it;
                InstantiatedSubCircuit* instantiatedSubCircuit = subCircuit->buildNewInstantiatedSubCircuit();
                ModelName modelName = instantiatedSubCircuit->getModelName();
                instantiatedSubCircuitMap.add(modelName, instantiatedSubCircuit);
            }
        }

        void SubCircuits::deleteAllSubCircuits()
        {
            for(SubCircuitVector::iterator it = subCircuitVector_.begin(); it != subCircuitVector_.end(); it++) {
                delete *it;
            }
        }

    }
}

