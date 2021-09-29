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

#include "StructRec/incl/Library/DominanceRelation.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const int DominanceRelation::MAX_PERSISTENCE_ = -1;

	DominanceRelation::DominanceRelation() :
			structureCore_(NULL)
	{
	}

	DominanceRelation::DominanceRelation(const DominanceRelation& other):
		structureCore_(NULL),
		dominatingStructures_(other.dominatingStructures_),
		dominatedStructures_(other.dominatedStructures_)
	{

	}

	void DominanceRelation::setStructureCore(const StructureCore& core)
	{
		structureCore_ = & core;
	}

	void DominanceRelation::addDominatingStructure(const StructureName& dominatingStructure)
	{
		dominatingStructures_.insert(dominatingStructure);
	}

	void DominanceRelation::addDominatedStructure(const StructureName& dominatedStructure)
	{
		dominatedStructures_.insert(dominatedStructure);
	}

	void DominanceRelation::removeDominatedStructures(StructureCircuits& circuits, int level) const
	{
		const StructureSet& dominatingStructures = getDominatingStructures();
		for(StructureSet::const_iterator it = dominatingStructures.begin(); it != dominatingStructures.end(); it++)
		{
			StructureName dominatingStructure = *it;

			bool expiredPersistence =
					(getStructureCore().findPersistence(dominatingStructure) + getStructureCore().findLevel(dominatingStructure) <= level)
					|| getStructureCore().findPersistence(dominatingStructure) == MAX_PERSISTENCE_;

			if(getStructureCore().isRegistered(dominatingStructure) && expiredPersistence)
			{
				int firstChildLevel = getStructureCore().getFirstChildLevel(dominatingStructure);
				StructureName firstChildName = getStructureCore().getFirstChildName(dominatingStructure);

				int secondChildLevel = getStructureCore().getSecondChildLevel(dominatingStructure);
				StructureName secondChildName = getStructureCore().getSecondChildName(dominatingStructure);

				if(secondChildLevel > firstChildLevel) {
					removeDominatedStructures(secondChildName, secondChildLevel, circuits);
					removeDominatedStructures(firstChildName, firstChildLevel, circuits);
				} else {
					removeDominatedStructures(firstChildName, firstChildLevel, circuits);
					removeDominatedStructures(secondChildName, secondChildLevel, circuits);
				}
			}
		}
	}

	void DominanceRelation::removeDominatedStructures(const StructureName& childName, int childLevel, StructureCircuits & circuits) const
	{

		StructureCircuit & structureCircuit = circuits.findStructureCircuit(childLevel);
		std::vector<Structure*> possibleChildren = structureCircuit.findStructures(childName);
		for(auto& it : possibleChildren)
		{

			Structure& currentStructure = *it;

			std::vector<const Structure*> arrayChildren = currentStructure.getArrayChildren();
			for(auto & it_array: arrayChildren)
			{
				bool hasDominatedParents = computeHasDominatingParent(*it_array);

				if(hasDominatedParents)
				{

					deleteDominatedParents(*it_array, circuits);
				}
			}
		}
	}

	bool DominanceRelation::computeHasDominatingParent(const Structure& structure) const
	{
		const std::vector<Structure*> & parents = structure.getAllHigherLevelParents();

		bool hasDominatedParents = false;
		for(std::vector<Structure*>::const_iterator it = parents.begin(); it != parents.end(); it++)
		{
			Structure * parent = * it;
			if(getDominatingStructures().find(parent->getIdentifier().getName()) !=  getDominatingStructures().end())
			{
				hasDominatedParents = true;
				break;
			}
		}
		return hasDominatedParents;
	}

	void DominanceRelation::deleteDominatedParents(const Structure& structure, StructureCircuits & circuits) const
	{
		const std::vector<Structure*> parents = structure.getAllHigherLevelParents();
		for(std::vector<Structure*>::const_iterator it = parents.begin(); it != parents.end(); it++)
		{
			Structure * parent = * it;
			StructureName parentName = parent->getIdentifier().getName();
			bool isDominated = (getDominatedStructures().find(parentName) != getDominatedStructures().end());
			if(isDominated)
			{
				parent->disConnect(circuits);
				delete parent;
			}
		}
	}

	const StructureCore & DominanceRelation::getStructureCore() const
	{
		assert(structureCore_ != NULL);
		return * structureCore_;
	}

	const DominanceRelation::StructureSet& DominanceRelation::getDominatingStructures() const
	{
		return dominatingStructures_;
	}

	const DominanceRelation::StructureSet& DominanceRelation::getDominatedStructures() const
	{
		return dominatedStructures_;
	}

	int DominanceRelation::getMaxPersistence() const
	{
		return MAX_PERSISTENCE_;
	}

	void DominanceRelation::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		const StructureSet& dominatingStructures = getDominatingStructures();
		const StructureSet& dominatedStructures = getDominatedStructures();

		Core::XmlNode & dominanceRelationNode = Core::RapidXmlUtils::addNode(node, doc, "dominanceRelation");

		for(StructureSet::const_iterator it_dominatingStructures = dominatingStructures.begin();
				it_dominatingStructures != dominatingStructures.end(); it_dominatingStructures++)
		{
			StructureName dominatingStructure = *it_dominatingStructures;
			Core::RapidXmlUtils::addSimpleNode(dominanceRelationNode, doc, "dominatingStructure", dominatingStructure.toStr());
		}

		for(StructureSet::const_iterator it_dominatedStructures = dominatedStructures.begin();
						it_dominatedStructures != dominatedStructures.end(); it_dominatedStructures++)
		{
			StructureName dominatedStructure = *it_dominatedStructures;
			Core::RapidXmlUtils::addSimpleNode(dominanceRelationNode, doc, "dominatedStructure", dominatedStructure.toStr());
		}
	}

	bool DominanceRelation::isIdentical(const DominanceRelation& other, const LibraryItemMappings& mappings) const
	{
		bool hasIdenticalDominatingStructures = hasIdenticalStructureSets(dominatingStructures_, other.dominatingStructures_, mappings);
		bool hasIdenticalDominatedStructures = hasIdenticalStructureSets(dominatedStructures_, other.dominatedStructures_, mappings);
		return hasIdenticalDominatingStructures && hasIdenticalDominatedStructures;
	}

	bool DominanceRelation::hasIdenticalStructureSets(const StructureSet& original, const StructureSet& other, const LibraryItemMappings& mappings) const
	{
		if(original.size() != other.size())
		{
			return false;
		}

		bool hasIdenticalStructures = true;
		for(StructureSet::const_iterator it = original.begin(); it != original.end(); it++)
		{
			StructureName originalStructure = *it;
			bool hasStructure = false;
			for(StructureSet::const_iterator it2 = other.begin(); it2 != other.end(); it2++)
			{
				StructureName otherStructure = *it2;
				StructureName mappedName = mappings.findMappedName(otherStructure);
				if(originalStructure == mappedName) {
					hasStructure = true;
					break;
				}

				if(!hasStructure) {
					hasIdenticalStructures = false;
					break;
				}
			}
		}
		return hasIdenticalStructures;
	}

	void DominanceRelation::update(const LibraryItemMappings& mappings)
	{
		dominatingStructures_ = computeUpdatedStructureSet(dominatingStructures_, mappings);
		dominatedStructures_ = computeUpdatedStructureSet(dominatedStructures_, mappings);
	}

	DominanceRelation::StructureSet DominanceRelation::computeUpdatedStructureSet(const StructureSet& original, const LibraryItemMappings& mappings) const
	{
		StructureSet updatedStructureSet;
		for(StructureSet::const_iterator it = original.begin(); it != original.end(); it++)
		{
			StructureName current = *it;
			StructureName mappedName = mappings.findMappedName(current);
			updatedStructureSet.insert(mappedName);
		}
		return updatedStructureSet;
	}
}


