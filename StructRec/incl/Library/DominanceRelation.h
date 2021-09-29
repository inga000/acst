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

#ifndef STRUCTREC_INCL_LIBRARY_DOMINANCERELATION_H_
#define STRUCTREC_INCL_LIBRARY_DOMINANCERELATION_H_

#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include <set>

namespace StructRec {

	class LibraryItemMappings;
	class DominanceRelation
	{
	private:
		typedef std::set<StructureName> StructureSet;
	public:
		DominanceRelation();
		DominanceRelation(const DominanceRelation& other);

		void setStructureCore(const StructureCore & core);
		void addDominatingStructure(const StructureName& dominatingStructure);
		void addDominatedStructure(const StructureName& dominatedStructure);

		void removeDominatedStructures(StructureCircuits & circuits, int level) const;

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;

		bool isIdentical(const DominanceRelation& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

	protected:
		const StructureCore & getStructureCore() const;
		const StructureSet& getDominatingStructures() const;
		const StructureSet& getDominatedStructures() const;
		int getMaxPersistence() const;

	private:
		void removeDominatedStructures(const StructureName& childName, int childLevel, StructureCircuits & circuits) const;

		bool computeHasDominatingParent(const Structure & structure) const;
		void deleteDominatedParents(const Structure & structure, StructureCircuits & circuits) const;



		bool hasIdenticalStructureSets(const StructureSet& original, const StructureSet& other, const LibraryItemMappings& mappings) const;
		StructureSet computeUpdatedStructureSet(const StructureSet& original, const LibraryItemMappings& mappings) const;
	private:
		static const int MAX_PERSISTENCE_;

		const StructureCore * structureCore_;
		StructureSet dominatingStructures_;
		StructureSet dominatedStructures_;
	};

}


#endif /* STRUCTREC_INCL_LIBRARY_DOMINANCERELATION_H_ */
