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

#include "HSpice/incl/OutputFile/SubCircuitRecursion.h"
#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuit.h"
#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuitMap.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

        SubCircuitRecursion::SubCircuitRecursion():
                subCircuitMap_(NULL)
        {
        }

        SubCircuitRecursion::~SubCircuitRecursion()
        {
        }

        void SubCircuitRecursion::setSubCircuitMap(SubCircuitMap& subCircuitMap)
        {
            subCircuitMap_ = &subCircuitMap;
        }

        void SubCircuitRecursion::fillSubCircuitMapRecursivly(const Core::Circuit& circuit)
        {
            if(circuit.hasInstances())
            {
                std::vector<const Core::Instance*> instancePointers = circuit.findInstances();
                for(auto& it : instancePointers)
                {
                    const Core::Instance* instance = it;
                    instances_.push_back(instance);
                    const Core::Circuit& master = instance->getMaster();
                    fillSubCircuitMapRecursivly(master);

                    int position = instances_.size();
                    Core::CircuitId circuitId = master.getCircuitIdentifier();
                    if( !getSubCircuitMap().hasSubCircuit(circuitId) ) 
                    {
                        SubCircuit subCircuit;
                        subCircuit.setCircuit(master);
                        subCircuit.setPosition(position);
                        getSubCircuitMap().add(circuitId, subCircuit);
                    } 
                    else 
                    {
                        SubCircuit& subCircuit = getSubCircuitMap().findSubCircuit(circuitId);
                        if(subCircuit.getPosition() < position) 
                        {
                            subCircuit.setPosition(position);
                        }
                    }
                    instances_.pop_back();
                }
            }
            
        }

        SubCircuitMap& SubCircuitRecursion::getSubCircuitMap()
        {
            assert(subCircuitMap_ != NULL);
            return * subCircuitMap_;
        }

    }
}
