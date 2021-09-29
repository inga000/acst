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

#include "Core/incl/Circuit/TopCircuit.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"

namespace Core {

    TopCircuit::~TopCircuit()
    {
        eraseAllMasters();
    }

    void TopCircuit::addMaster(Circuit& master)
    {
        masters_.push_back(&master);
    }

    std::vector<NetId> TopCircuit::findGlobalNetIds() const
    {
        std::vector<NetId> globalNetIdsTopCircuit = Circuit::findGlobalNetIds();
        std::set<NetId> uniqueGlobalNetIds(globalNetIdsTopCircuit.begin(), globalNetIdsTopCircuit.end());
        for(auto& it : masters_)
        {
           const Circuit* master = it;
           std::vector<NetId> masterGlobalNetIds = master->findGlobalNetIds();
           uniqueGlobalNetIds.insert(masterGlobalNetIds.begin(), masterGlobalNetIds.end());
        }

        std::vector<NetId> allGLobalNetIds(uniqueGlobalNetIds.begin(), uniqueGlobalNetIds.end());
        return allGLobalNetIds;
    }

    std::vector<NetId> TopCircuit::findVddSupplyNetIds() const
    {
        std::vector<NetId> vddSupplyNetIdsTopCircuit = Circuit::findVddSupplyNetIds();
        std::set<NetId> uniqueVddSupplyNetIds;(vddSupplyNetIdsTopCircuit.begin(), vddSupplyNetIdsTopCircuit.end());
        for(auto& it : masters_)
        {
           const Circuit* master = it;
           std::vector<NetId> masterVddSupplyNetIds = master->findVddSupplyNetIds();
           uniqueVddSupplyNetIds.insert(masterVddSupplyNetIds.begin(), masterVddSupplyNetIds.end());
        }

        std::vector<NetId> allVddSupplyNetIds(uniqueVddSupplyNetIds.begin(), uniqueVddSupplyNetIds.end());
        return allVddSupplyNetIds;
    }

    std::vector<NetId> TopCircuit::findGndSupplyNetIds() const
    {
        std::vector<NetId> gndSupplyNetIdsTopCircuit = Circuit::findGndSupplyNetIds();
        std::set<NetId> uniqueGndSupplyNetIds;(gndSupplyNetIdsTopCircuit.begin(), gndSupplyNetIdsTopCircuit.end());
        for(auto& it : masters_)
        {
           const Circuit* master = it;
           std::vector<NetId> masterGndSupplyNetIds = master->findGndSupplyNetIds();
           uniqueGndSupplyNetIds.insert(masterGndSupplyNetIds.begin(), masterGndSupplyNetIds.end());
        }

        std::vector<NetId> allGndSupplyNetIds(uniqueGndSupplyNetIds.begin(), uniqueGndSupplyNetIds.end());
        return allGndSupplyNetIds;
    }

    SupplyNets TopCircuit::findSupplyNets() const
    {
    	SupplyNets topSupplyNets = Circuit::findSupplyNets();
    	for(auto& it : masters_)
    	{
    		const Circuit* master = it;
    		SupplyNets masterSupplyNets = master->findSupplyNets();
    		topSupplyNets.add(masterSupplyNets);
    	}
    	return topSupplyNets;
    }

    void TopCircuit::eraseAllMasters()
    {
        for(CircuitVector::const_iterator it = masters_.begin(); it != masters_.end(); it++)
        {
            Circuit* master = *it;
            delete master;
        }
    }

}
