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

#include "HSpice/incl/InputFile/NetIdentifiers/NetIdentifiers.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetIdentifiers.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/LocalGlobalNet.h"

namespace HSpice {
    namespace InputFile {

        NetIdentifiers::NetIdentifiers():
                globalNetMap_(NULL)
        {
        }

        NetIdentifiers::~NetIdentifiers()
        {
        }

        void NetIdentifiers::addNetId(const Core::NetId& netId)
        {
            netlistIds_.insert(netId);
        }

        void NetIdentifiers::addNetIds(const std::vector<Core::NetId>& netIds)
        {
                netlistIds_.insert(netIds.begin(), netIds.end());
        }

        void NetIdentifiers::addNetsToCircuit(Core::Circuit& circuit) const
        {
            for(auto& it : netlistIds_)
            {
                Core::NetId netId = it;
                if(!getGlobalNetMap().hasNet(netId) && !circuit.hasNet(netId)) {
                    Core::Net & net = circuit.addNet(netId);
                    if(hasSupplyNetIdentifiers()) {
                        Core::Supply supply = getSupplyNetIdentifiers().findSupply(netId);
                        net.setSupply(supply);
                    }
                }
                 if(getGlobalNetMap().hasNet(netId) && !circuit.hasNet(netId)) {
                     Core::LocalGlobalNet & net = circuit.addLocalGlobalNet(netId);
                     net.setGlobalNet(getGlobalNetMap().getNet(netId));
                     Core::Supply supply = getGlobalNetMap().getNet(netId).getSupply();
                     net.setSupply(supply);
                 }
            }
        }

        void NetIdentifiers::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            globalNetMap_ = &globalNetMap;
        }

        void NetIdentifiers::setSupplyNetIdentifiers(SupplyNetIdentifiers supplyNetIdentifiers)
        {
            supplyNetIdentifiers_ = supplyNetIdentifiers;
        }

        const GlobalNetMap& NetIdentifiers::getGlobalNetMap() const
        {
            assert(globalNetMap_ != NULL);
            return *globalNetMap_;
        }

        bool NetIdentifiers::hasSupplyNetIdentifiers() const
        {
            return supplyNetIdentifiers_.isInitialized();
        }

        SupplyNetIdentifiers NetIdentifiers::getSupplyNetIdentifiers() const
        {
            assert(hasSupplyNetIdentifiers());
            return supplyNetIdentifiers_;
        }

    }
}



