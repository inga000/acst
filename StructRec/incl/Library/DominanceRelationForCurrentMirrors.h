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

#ifndef STRUCTREC_INCL_LIBRARY_DominanceRelationForCurrentMirrors_H_
#define STRUCTREC_INCL_LIBRARY_DominanceRelationForCurrentMirrors_H_

#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/Library/DominanceRelation.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include <set>

namespace StructRec {

	class LibraryItemMappings;
	class DominanceRelationForCurrentMirrors : public DominanceRelation
	{
	private:
		typedef std::set<StructureName> StructureSet;
	public:
		DominanceRelationForCurrentMirrors();
		DominanceRelationForCurrentMirrors(const DominanceRelationForCurrentMirrors& other);

		void removeDominatedStructures(StructureCircuits & circuits, int level) const;
	private:
		std::vector<Structure *> findDominatedParents(const Structure& child1, const Structure & child2, StructureCircuits& circuits) const;

	};

}


#endif /* STRUCTREC_INCL_LIBRARY_DominanceRelationForCurrentMirrors_H_ */
