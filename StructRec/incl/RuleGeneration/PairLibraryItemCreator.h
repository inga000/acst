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

#ifndef STRUCTREC_INCL_RULEGENERATION_PAIRLIBRARYITEMCREATOR_H_
#define STRUCTREC_INCL_RULEGENERATION_PAIRLIBRARYITEMCREATOR_H_

#include "Core/incl/Circuit/Device/TechType.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "StructRec/incl/RuleGeneration/NewRecognizedChild.h"
#include "StructRec/incl/RuleGeneration/NewPairLibraryItem.h"
#include <vector>
#include <map>

namespace StructRec {

 	 class PairLibraryItemCreator
	 {
	 private:
 		 typedef std::map<int, std::vector<NewPairLibraryItem*> > ConnectionMap;
 		 typedef std::map<int, std::map<int, std::vector<NewPairLibraryItem*> > > SmallestHierarchyLevelOfOneChildAndHierarchieLevelDifferenceMap;
 		 typedef std::map<int, std::vector<NewPairLibraryItem*> > SmallestHierarchyLevelOfOneChildMap;

 	 public:
 		 PairLibraryItemCreator();
 		 ~PairLibraryItemCreator();

 		std::vector<NewPairLibraryItem*> build(std::vector<Structure*>& children, int & idCounter);
 		void setStructureCore(StructureCore & core);
 		void setPairLibraryItemName(const std::string& pairLibraryItemName);

 	 private:
 		const StructureCore & getStructureCore() const;
 		StructureCore & getStructureCore();
 		bool hasStructureCore() const;

 		std::string getPairLibraryItemName();
 		bool hasPairLibraryItemName() const;

 		bool hasConnectionMap() const;

	 private:
 		void initializeConnectionMap(std::vector<Structure*>& children);
 		void createNewPairLibraryItemsWithNotUsedChildren(std::vector<NewPairLibraryItem*> & possibleItems, std::vector<NewPairLibraryItem*> & newItems, int & idCounter);
 		void addPairLibraryItemToConnectionMap(int numberOfConnections, NewPairLibraryItem & libraryItem);
 		void sortedAndCreatedPairLibraryItemsWithSmallestHierarchyLevelDifference(std::vector<NewPairLibraryItem*> & possibleItems, std::vector<NewPairLibraryItem*> & newItems, int & idCounter);

 		int computeNumberOfConnections(std::vector<StructurePin*> & pinsChild1,
 				std::vector<StructurePin*> & pinsChild2);

 		bool ChildrenOnEqualHierarchyLevel(NewPairLibraryItem & item);
 		bool ChildrenWithEqualTechType(NewPairLibraryItem & item);

 		void deleteEqualItem(std::vector<NewPairLibraryItem*> & createdItems);

 		//Debugging function:
 		void writeConnectionMap();
 	private:
 		ConnectionMap connectionMap_;

 		StructureCore * structureCore_;
 		std::string pairLibraryItemName_;
 	 };




}

#endif /* STRUCTREC_INCL_RULEGENERATION_PAIRLIBRARYITEMCREATOR_H_ */
