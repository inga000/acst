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

#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNetsMap.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/ArrayConnection.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Structure/Array.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

    ParallelNetsMap::ParallelNetsMap()
    {
    }

    void ParallelNetsMap::add(const ParallelNetsMapKey& parallelNetsMapKey, const Core::Device& device)
    {
        if(hasArray(parallelNetsMapKey)) {
        	std::vector<const Core::Device*>& array = findArray(parallelNetsMapKey);
            array.push_back(&device);

        } else {
        	std::vector<const Core::Device*> newArray;
            newArray.push_back(&device);
            getArrayMap().insert(ArrayMap::value_type(parallelNetsMapKey, newArray));
        }
    }

    void ParallelNetsMap::createArrays(const Core::Circuit & circuit, StructureCircuit& structureCircuit, const ArrayConnection& arrayConnection,
               const StructureName& arrayName, Core::VisitedInstances & visitedInstances, int & idCounter, const int persistance) const
       {

           for (ArrayMap::const_iterator it = getArrayMap().begin(); it != getArrayMap().end(); it++)
           {
               const ParallelNetsMapKey& parallelNetsMapKey = it->first;
               const std::vector<const Core::Device*>& devices = it->second;

               StructureId structureId = createStructureId(arrayName, idCounter);
               Core::TechType techType = parallelNetsMapKey.getTechType();

               Array * newArray = new Array();
               newArray->setStructureId(structureId);
               newArray->setTechType(techType);
               newArray->setPersistence(persistance);
               newArray->setDevices(devices);

               arrayConnection.connectArray(circuit, structureCircuit,*newArray, *devices.front(), visitedInstances);
               structureCircuit.addStructure(*newArray);
               idCounter++;
           }
       }

    StructureId ParallelNetsMap::createStructureId(const StructureName& structureName, const int id) const
    {
        StructureId structureId;
        structureId.setName(structureName);
        structureId.setId(id);
        return structureId;
    }

    bool ParallelNetsMap::hasArray(const ParallelNetsMapKey& parallelNetsMapKey) const
    {
    	for(auto &  it : getArrayMap())
    	{
    		const ParallelNetsMapKey& mapKey = it.first;
    		if(mapKey == parallelNetsMapKey)
    		{
    			return true;
    		}
    	}
        return false;
    }

    std::vector<const Core::Device*>& ParallelNetsMap::findArray(const ParallelNetsMapKey& parallelNetsMapKey)
    {
        assert(hasArray(parallelNetsMapKey));
		ParallelNetsMapKey mapKey;

    	for(auto &  it : getArrayMap())
    	{
    		if(it.first == parallelNetsMapKey)
    		{
    			mapKey = it.first;
    			break;
    		}
    	}

        return getArrayMap()[mapKey];
    }

    ParallelNetsMap::ArrayMap& ParallelNetsMap::getArrayMap()
    {
        return arrayMap_;
    }

    const ParallelNetsMap::ArrayMap& ParallelNetsMap::getArrayMap() const
    {
        return arrayMap_;
    }
}


