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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_ARRAYLIBRARYITEM_PARALLELNETS_PARALLELNETSMAP_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_ARRAYLIBRARYITEM_PARALLELNETS_PARALLELNETSMAP_H_

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNetsMapKey.h"
#include <map>
#include <vector>

namespace Core {

	class Circuit;
	class Instance;
	class VisitedInstances;
}

namespace StructRec {

    class Array;
    class StructureName;
    class StructureCircuit;
    class ArrayConnection;
    class ParallelNetsMap
    {
    private:
        typedef std::map<ParallelNetsMapKey, std::vector<const Core::Device*> > ArrayMap;

    public:
        ParallelNetsMap();

        void add(const ParallelNetsMapKey& parallelNetsMapKey, const Core::Device& device);
        void createArrays(const Core::Circuit & circuit, StructureCircuit& structureCircuit, const ArrayConnection& arrayConnection,
        		const StructureName& arrayName, Core::VisitedInstances & visitedInstances,int & idCounter, const int persistance = -1) const;

    private:
        StructureId createStructureId(const StructureName& structureName, const int id) const;

    private:
        bool hasArray(const ParallelNetsMapKey& parallelNetsMapKey) const;
        std::vector<const Core::Device*>& findArray(const ParallelNetsMapKey& parallelNetsMapKey);

        ArrayMap& getArrayMap();
        const ArrayMap& getArrayMap() const;

    private:
        ArrayMap arrayMap_;
    };
}


#endif
