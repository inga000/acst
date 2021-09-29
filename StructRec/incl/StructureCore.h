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

#ifndef STRUCTREC_INCL_LIBRARY_STRUCTURECORE_H_
#define STRUCTREC_INCL_LIBRARY_STRUCTURECORE_H_

#include "StructRec/incl/StructureCircuit/Structure/Connection/PairConnection.h"
#include "Core/incl/Common/Object.h"
#include <map>

namespace StructRec {

	class StructureName;
	class StructurePinType;
	class ArrayLibraryItem;
	class PairLibraryItem;
	class StructureCore : public Core::Object
	{
	public:
		StructureCore() = default;
		~StructureCore() = default;

		void registerArrayLibraryItem(const ArrayLibraryItem& arrayLibraryItem);
		void registerPairLibraryItem(const PairLibraryItem& pairLibraryItem);

		bool isRegistered(const StructureName& name) const;
		int findLevel(const StructureName& name) const;
		int findLevel(const StructurePinType& structurePinType) const;
		int findPersistence(const StructureName& name) const;

		int getMaxHierarchyLevel() const;

		int getFirstChildLevel(const StructureName& name) const;
		StructureName getFirstChildName(const StructureName& name) const;

		int getSecondChildLevel(const StructureName& name) const;
		StructureName getSecondChildName(const StructureName& name) const;

		bool hasLevel(const StructureName& name) const;
		bool hasPersistence(const StructureName& name) const;

		std::string toStr() const;
	private:
		void addStructure(const StructureName& name, const int level);
		void addPairConnection(const StructureName& name, const PairConnection& pairConnection);
		void addPersistence(const StructureName & name, const int persistence);

		bool hasPairConnection(const StructureName& name) const;
		PairConnection findPairConnection(const StructureName& name) const;

	private:
		std::map<StructureName, int> hierarchyLevelRegister_;
		std::map<StructureName, PairConnection> pairConnectionRegister_;
		std::map<StructureName, int> persistenceRegister_;
	};
}



#endif /* STRUCTREC_INCL_LIBRARY_STRUCTURECORE_H_ */
