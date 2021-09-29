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
/*
 * HierarchyLevel.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: ga76fuw
 */
#include "StructRec/incl/Library/HierarchyLevel.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec{

	const int HierarchyLevel::LEVEL_NOT_INITIALIZED_ = 0;

	HierarchyLevel::HierarchyLevel():
			structureCore_(nullptr),
			level_(LEVEL_NOT_INITIALIZED_)
	{
	}

	HierarchyLevel::~HierarchyLevel()
	{
		eraseAllLibraryItems();
	}

	void HierarchyLevel::setLevel(int level)
	{
		assert(level > LEVEL_NOT_INITIALIZED_);
		level_=level;
	}

	void HierarchyLevel::addItem(PairLibraryItem & item)
	{
		pairLibraryItems_.push_back(&item);
	}

	void HierarchyLevel::setStructureCore(const StructureCore& structureCore)
	{
		structureCore_ = &structureCore;
	}

	void HierarchyLevel::recognize(StructureCircuits & circuits) const
	{
		StructureCircuit * levelCircuit = new StructureCircuit;
		levelCircuit->setHierarchyLevel(getLevel());
		for(PairLibraryItemVector::const_iterator it = pairLibraryItems_.begin(); it != pairLibraryItems_.end(); it++)
		{
			 PairLibraryItem * pairItem = *it;
		     pairItem->recognize(circuits,* levelCircuit);
		}
		circuits.addStructureCircuit(*levelCircuit);
	}

	void HierarchyLevel::recognizeOnStructuresWithoutParents(StructureCircuits & circuits) const
	{
		StructureCircuit * levelCircuit = new StructureCircuit;
		if(circuits.hasStructureCircuit(getLevel()))
		{
			levelCircuit = & circuits.findStructureCircuit(getLevel());
		}
		else
		{
			levelCircuit->setHierarchyLevel(getLevel());

		}
		for(PairLibraryItemVector::const_iterator it = pairLibraryItems_.begin(); it != pairLibraryItems_.end(); it++)
		{
			 PairLibraryItem * pairItem = *it;
			 pairItem->recognizeOnStructuresWithoutParents(circuits, *levelCircuit);
		}
			circuits.addStructureCircuit(*levelCircuit);

	}

	bool HierarchyLevel::operator <(const HierarchyLevel& other) const
	{
		return level_ < other.level_;
	}

	int HierarchyLevel::getLevel() const
	{
		assert(level_ != LEVEL_NOT_INITIALIZED_);
		return level_;
	}

	const StructureCore& HierarchyLevel::getStructureCore() const
	{
		assert(structureCore_ != nullptr);
		return *structureCore_;
	}

	void HierarchyLevel::eraseAllLibraryItems()
	{
		for(PairLibraryItemVector::iterator it = pairLibraryItems_.begin(); it != pairLibraryItems_.end(); it++)
		{
			delete *it;
		}
	}

	const HierarchyLevel::PairLibraryItemVector& HierarchyLevel::getPairLibraryItems() const
	{
		return pairLibraryItems_;
	}

	std::vector<StructurePinType> HierarchyLevel::getStructurePinTypes(const StructureName& structureName) const
	{
		std::vector<StructurePinType> structurePinType;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it!=getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* pairLibraryItem = *it;
			if(pairLibraryItem->getName() == structureName)
			{
				const PairConnection& pairConnection = pairLibraryItem->getPairConnection();
				structurePinType = pairConnection.getStructurePins();
				break;
			}
		}
		return structurePinType;
	}

	PairLibraryItem & HierarchyLevel::findPairLibraryItem(const StructureName& structureName) const
	{
		PairLibraryItem * item;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it!=getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* pairLibraryItem = *it;

			if(pairLibraryItem->getName() == structureName)
			{
				item = pairLibraryItem;
				break;
			}
		}
		return * item;
	}

	bool HierarchyLevel::hasIdenticalPairLibraryItem(const PairLibraryItem& item, const LibraryItemMappings& mappings) const
	{
		bool hasIt = false;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it!=getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* pairLibraryItem = *it;
			if(pairLibraryItem->isIdentical(item, mappings)) {
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	LibraryItemMapping HierarchyLevel::creatNewMapping(const PairLibraryItem& item, const LibraryItemMappings& mappings) const
	{
		assert(hasIdenticalPairLibraryItem(item, mappings));
		LibraryItemMapping newMapping;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it!=getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* pairLibraryItem = *it;
			if(pairLibraryItem->isIdentical(item, mappings)) {
				newMapping = pairLibraryItem->creatNewMapping(item, mappings);
				newMapping.setMappedName(pairLibraryItem->getName());
				break;
			}
		}
		return newMapping;
	}

	bool HierarchyLevel::hasPairLibraryItem(const StructureName& structureName) const
	{
		bool hasIt = false;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it != getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* currentItem = *it;
			if(currentItem->getName() == structureName)
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	std::string HierarchyLevel::getImageFilePath(const StructureName& structureName) const
	{
		assert(hasPairLibraryItem(structureName));
		std::string imageFilePath;
		for(PairLibraryItemVector::const_iterator it = getPairLibraryItems().begin(); it != getPairLibraryItems().end(); it++)
		{
			PairLibraryItem* currentItem = *it;
			if(currentItem->getName() == structureName)
			{
				imageFilePath = currentItem->getImageFilePath();
				break;
			}
		}
		return imageFilePath;
	}

	std::vector<StructureName> HierarchyLevel::getStructureNames() const
	{
		std::vector<StructureName> items;
		for(auto& it : getPairLibraryItems())
		{
			PairLibraryItem* currentItem = it;
			items.push_back(currentItem->getName());
		}
		return items;
	}

}

