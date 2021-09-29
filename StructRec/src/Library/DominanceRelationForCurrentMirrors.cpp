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

#include "StructRec/incl/Library/DominanceRelationForCurrentMirrors.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {



	DominanceRelationForCurrentMirrors::DominanceRelationForCurrentMirrors()
	{
	}

	DominanceRelationForCurrentMirrors::DominanceRelationForCurrentMirrors(const DominanceRelationForCurrentMirrors& other)
	: DominanceRelation(other)
	{

	}



	void DominanceRelationForCurrentMirrors::removeDominatedStructures(StructureCircuits& circuits, int level) const
	{
		const StructureSet& dominatingStructures = getDominatingStructures();
		for(StructureSet::const_iterator it = dominatingStructures.begin(); it != dominatingStructures.end(); it++)
		{
			StructureName dominatingStructure = *it;

			int strucLevel = getStructureCore().findLevel(dominatingStructure);
			if(getStructureCore().isRegistered(dominatingStructure) && circuits.hasStructureCircuit(strucLevel))
			{
				StructureCircuit & structureCircuit = circuits.findStructureCircuit(strucLevel);
				std::vector<Structure*> structures = structureCircuit.findStructures(dominatingStructure);

				for(auto& struc : structures)
				{

					assert(struc->isPair(), "Dominating structure can only be a current mirror!");

					Pair* pair =  static_cast<Pair* >(struc);

					const Structure & child1 = pair->getChild1();
					const Structure & child2 = pair->getChild2();

					std::vector<Structure * > dominatedParents = findDominatedParents(child1, child2, circuits);

					for(auto & parent : dominatedParents)
					{
						parent->disConnect(circuits);
						delete parent;
					}
				}
			}
		}
	}

	std::vector<Structure *> DominanceRelationForCurrentMirrors::findDominatedParents(const Structure& child1, const Structure & child2, StructureCircuits& circuits) const
	{
		std::vector<Structure *> dominatedStructures;
		for(auto & dominatedStructure : getDominatedStructures())
		{
			int level = getStructureCore().findLevel(dominatedStructure);

			StructureCircuit & structureCircuit = circuits.findStructureCircuit(level);
			std::vector<Structure*> structures = structureCircuit.findStructures(dominatedStructure);

			for(auto & struc : structures)
			{
				if(struc->hasCommonDevices(child1) && struc->hasCommonDevices(child2))
				{
					dominatedStructures.push_back(struc);
				}
			}
		}
		return dominatedStructures;
	}



}


