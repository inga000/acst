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


#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	void StructureCore::registerArrayLibraryItem(const ArrayLibraryItem& arrayLibraryItem)
	{
		StructureName structureName = arrayLibraryItem.getName();
		addStructure(structureName, arrayLibraryItem.getHierarchyLevel());
		addPersistence(structureName, arrayLibraryItem.getPersistence());
	}

	void StructureCore::registerPairLibraryItem(const PairLibraryItem& pairLibraryItem)
	{
		StructureName structureName = pairLibraryItem.getName();
		addStructure(structureName, pairLibraryItem.getHierarchyLevel());
		addPairConnection(structureName, pairLibraryItem.getPairConnection());
		addPersistence(structureName, pairLibraryItem.getPersistence());
	}

	void StructureCore::addStructure(const StructureName& name, const int level)
	{
		hierarchyLevelRegister_.insert(std::pair<StructureName, int> (name, level));
	}

	void StructureCore::addPairConnection(const StructureName& name, const PairConnection& pairConnection)
	{
		pairConnectionRegister_.insert(std::map<StructureName, PairConnection>::value_type(name, pairConnection));
	}

	bool StructureCore::isRegistered(const StructureName& name) const
	{
		return hasLevel(name);
	}

	int StructureCore::findLevel(const StructureName& name) const
	{
		assert(hasLevel(name));
		return hierarchyLevelRegister_.at(name);
	}

	int StructureCore::findLevel(const StructurePinType& structurePinType) const
	{
		return findLevel(structurePinType.getStructureName());
	}

	int StructureCore::getFirstChildLevel(const StructureName& name) const
	{
		StructureName firstChildName = findPairConnection(name).getFirstChildName();
		return findLevel(firstChildName);
	}

	StructureName StructureCore::getFirstChildName(const StructureName& name) const
	{
		return findPairConnection(name).getFirstChildName();
	}

	int StructureCore::getSecondChildLevel(const StructureName& name) const
	{
		StructureName secondChildName = findPairConnection(name).getSecondChildName();
		return findLevel(secondChildName);
	}

	StructureName StructureCore::getSecondChildName(const StructureName& name) const
	{
		return findPairConnection(name).getSecondChildName();
	}

	std::string StructureCore::toStr() const
	{
		std::ostringstream oss;
		oss << "HierarchyLevelRegister: " << hierarchyLevelRegister_ << std::endl;
		return oss.str();
	}

	bool StructureCore::hasLevel(const StructureName& name) const
	{
		return hierarchyLevelRegister_.find(name) != hierarchyLevelRegister_.end();
	}

	bool StructureCore::hasPairConnection(const StructureName& name) const
	{
		return pairConnectionRegister_.find(name) != pairConnectionRegister_.end();
	}

	void StructureCore::addPersistence(const StructureName& name,
		const int persistence)
	{
		persistenceRegister_.insert(std::map<StructureName, int>::value_type(name, persistence));
	}

	int StructureCore::findPersistence(const StructureName& name) const
	{
		assert(hasPersistence(name));
		return persistenceRegister_.at(name);
	}

	bool StructureCore::hasPersistence(const StructureName& name) const
	{
		return persistenceRegister_.find(name) != persistenceRegister_.end();
	}

	PairConnection StructureCore::findPairConnection(const StructureName& name) const
	{
		assert(hasPairConnection(name));
		return pairConnectionRegister_.find(name)->second;
	}

	int StructureCore::getMaxHierarchyLevel() const
	{
		int level =-1;
		for(auto & it_level : hierarchyLevelRegister_)
		{
			if(it_level.second > level)
			{
				level = it_level.second;
			}
		}
		return level;
	}

}


