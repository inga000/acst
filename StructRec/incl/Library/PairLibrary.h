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

#ifndef STRUCTREC_INCL_LIBRARY_PAIRLIBRARY_H_
#define STRUCTREC_INCL_LIBRARY_PAIRLIBRARY_H_

#include "StructRec/incl/Library/HierarchyLevel.h"
#include "StructRec/incl/Results/Result.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include <vector>
#include <map>

namespace StructRec {

	class StructureCircuits;
	class DominanceRelation;
	class DominanceRelationForCurrentMirrors;
	class PairLibrary
	{
	private:
		typedef std::map<int,HierarchyLevel*> HierarchyLevelMap;
		typedef std::vector<DominanceRelation*> DominanceRelationVector;
		typedef std::vector<DominanceRelationForCurrentMirrors*> DominanceRelationForCurrentMirrorsVector;

	public:
		PairLibrary();
		~PairLibrary();

		void setFilePath(const std::string& filePath);
		void setStructureCore(StructureCore& structureCore);
		void addHierarchyLevel(const int level , HierarchyLevel& hierarchyLevel);
		void addDominanceRelation(DominanceRelation& dominanceRelation);
		void addDominanceRelationForCurrentMirrors(DominanceRelationForCurrentMirrors& dominanceRelation);
		void addItem(PairLibraryItem& pairLibraryItem);
		void addAndMerge(const PairLibrary& other, LibraryItemMappings& mappings);

		void recognize(Result& results) const;
		void recognizeOnStructuresWithoutParents(StructureCircuits& circuits) const;


		PairLibraryItem& findPairLibraryItem(const StructureName& structureName);
		void flagItemsNotOnTopLevelAndWithoutPersistence() const;
		//

		//user interface function
		std::string getFilePath() const;
		std::string getImageFilePath(const StructureName& structureName) const;
		std::vector<StructureName> getStructureNames() const;
		std::vector<StructurePinType> getStructurePinTypes(const StructureName& structureName) const;

		void writeXml(Control::OutputFile& file) const;

	private:
		void recognizeCircuitsRecursivly(StructureCircuits & circuits, const int level) const;
		void recognizePairsInInstance(StructureCircuits& circuits,  const int level) const;

		void completeCircuit(StructureCircuit& circuit, StructureCircuit& lowerCircuit) const;
		void addInstanceTerminals(StructureCircuit& circuit, StructureCircuit& lowerCircuit) const;
		void addTerminals(StructureCircuit& circuit, StructureCircuit& lowerCircuit) const;

		void removeUncertainStructures(Result& results) const;
		void removeDominatedStructures(Result& results, int level) const;

		//Functions for combining pairLibraries
		bool hasIdenticalDominanceRelation(const DominanceRelation& other, const LibraryItemMappings& mappings) const;
		bool hasIdenticalPairLibraryItem(const PairLibraryItem& item, const LibraryItemMappings& mappings) const;
		LibraryItemMapping creatNewMapping(const PairLibraryItem& item, const LibraryItemMappings& mappings) const;
		//
		void eraseHierarchyLevels();
		void eraseDominanceRelations();

		bool hasHierarchyLevel(const int level) const;

		void writePairLibraryXml(Core::XmlNode& node, Core::XmlDocument& doc) const;

	private:
		HierarchyLevel& findHierarchyLevel(const int level) const;
		const PairLibraryItem& findPairLibraryItem(const StructureName& structureName) const;

		const StructureCore& getStructureCore() const;
		StructureCore& getStructureCore();

		const std::vector<PairLibraryItem*> getPairLibraryItems() const;

		HierarchyLevelMap& getHierarchyLevelMap();
		const HierarchyLevelMap& getHierarchyLevelMap() const;

	private:
		StructureCore* structureCore_;

		HierarchyLevelMap hierarchyLevels_;
		DominanceRelationVector dominanceRelations_;
		DominanceRelationForCurrentMirrorsVector dominanceRelationsForCurrentMirrors_;

		std::string filePath_;
	};
}



#endif /* STRUCTREC_INCL_LIBRARY_PAIRLIBRARY_H_ */
