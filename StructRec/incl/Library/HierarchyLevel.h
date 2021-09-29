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

#ifndef STRUCTREC_INCL_LIBRARY_HIERARCHYLEVEL_H_
#define STRUCTREC_INCL_LIBRARY_HIERARCHYLEVEL_H_

#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include <vector>

namespace StructRec {

	class StructureCore;
	class HierarchyLevel
	{
	private:
		typedef std::vector<PairLibraryItem*> PairLibraryItemVector;
	public:
		HierarchyLevel();
		~HierarchyLevel();

		void setLevel(int level);
		void setStructureCore(const StructureCore& structureCore);
		void addItem(PairLibraryItem & item);

		void recognize(StructureCircuits& circuits) const;
		void recognizeOnStructuresWithoutParents(StructureCircuits& circuits) const;

		PairLibraryItem & findPairLibraryItem(const StructureName& structureName) const;
		const PairLibraryItemVector& getPairLibraryItems() const;

		//functions for combining pair libraries
		bool hasIdenticalPairLibraryItem(const PairLibraryItem& item, const LibraryItemMappings& mappings) const;
		LibraryItemMapping creatNewMapping(const PairLibraryItem& item, const LibraryItemMappings& mappings) const;

		//user interface function
		bool hasPairLibraryItem(const StructureName& structureName) const;
		std::string getImageFilePath(const StructureName& structureName) const;

		std::vector<StructurePinType> getStructurePinTypes(const StructureName& structureName) const;
		std::vector<StructureName> getStructureNames() const;

		bool operator<(const HierarchyLevel & other) const;

	private:
		int  getLevel() const;
		const StructureCore& getStructureCore() const;

		void eraseAllLibraryItems();

	private:
		static const int LEVEL_NOT_INITIALIZED_;

	private:
		int level_;
		const StructureCore* structureCore_;
		PairLibraryItemVector pairLibraryItems_;
	};
}


#endif /* STRUCTREC_INCL_LIBRARY_HIERARCHYLEVEL_H_ */
