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

#include "StructRec/incl/RuleGeneration/PairLibraryItemCreator.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	PairLibraryItemCreator::PairLibraryItemCreator() :
			structureCore_(NULL)
	{
	}

	PairLibraryItemCreator::~PairLibraryItemCreator()
	{
	}

	std::vector<NewPairLibraryItem*> PairLibraryItemCreator::build(std::vector<Structure*>& children, int & idCounter)
	{
		initializeConnectionMap(children);
		//For Debugging
		writeConnectionMap();

		std::vector<NewPairLibraryItem*> createdItems;
		for(std::map<int, std::vector<NewPairLibraryItem*> >::reverse_iterator it_connections = connectionMap_.rbegin();
				it_connections != connectionMap_.rend(); it_connections++)
		{
			std::vector<NewPairLibraryItem*> itemOnSameHierarchyLevelButDifferentTechType;
			std::vector<NewPairLibraryItem*> itemOnDifferentHierarchyLevelButSameTechType;
			std::vector<NewPairLibraryItem*> itemOnDifferentHierarchyLevelAndDifferentTechType;

			std::vector<NewPairLibraryItem*> possibleItems = it_connections->second;
			for(std::vector<NewPairLibraryItem*>::const_iterator it_items = possibleItems.begin(); it_items != possibleItems.end(); it_items++)
			{
				NewPairLibraryItem * item = *it_items;
				if(ChildrenOnEqualHierarchyLevel(*item) && ChildrenWithEqualTechType(*item))
				{
					if((!item->getChild1().isPartOfNewPairLibraryItem()) && (!item->getChild2().isPartOfNewPairLibraryItem()))
					{
						item->setStructureCore(getStructureCore());
						item->build(getPairLibraryItemName(), idCounter );
						getStructureCore().registerPairLibraryItem(*item);
						idCounter++;
						createdItems.push_back(item);
					}
					else
					{
						delete item;
					}
				}
				else if(ChildrenWithEqualTechType(*item))
				{
					itemOnDifferentHierarchyLevelButSameTechType.push_back(item);
				}
				else
				{
					itemOnDifferentHierarchyLevelAndDifferentTechType.push_back(item);
				}
			}
			createNewPairLibraryItemsWithNotUsedChildren(itemOnSameHierarchyLevelButDifferentTechType, createdItems, idCounter);
			sortedAndCreatedPairLibraryItemsWithSmallestHierarchyLevelDifference(itemOnDifferentHierarchyLevelButSameTechType, createdItems, idCounter);
			sortedAndCreatedPairLibraryItemsWithSmallestHierarchyLevelDifference(itemOnDifferentHierarchyLevelAndDifferentTechType, createdItems, idCounter);

		}
		deleteEqualItem(createdItems);
		return createdItems;

	}

	bool PairLibraryItemCreator::ChildrenOnEqualHierarchyLevel(NewPairLibraryItem & item)
	{
		int levelChild1 = getStructureCore().findLevel(item.getChild1().getStructure().getStructureName());
		int levelChild2 = getStructureCore().findLevel(item.getChild2().getStructure().getStructureName());
		return levelChild1 == levelChild2;
	}

	bool PairLibraryItemCreator::ChildrenWithEqualTechType(NewPairLibraryItem & item)
	{
		Core::TechType techTypeChild1 = item.getChild1().getStructure().getTechType();
		Core::TechType techTypeChild2 = item.getChild2().getStructure().getTechType();
		return techTypeChild1 == techTypeChild2 && !techTypeChild1.isUndefined();
	}

	void PairLibraryItemCreator::initializeConnectionMap(std::vector<Structure*>& children)
	{
		std::vector<NewRecognizedChild*> possibleChildren;
		possibleChildren.clear();

		for(auto& it_structures : children)
		{
			Structure & structure = *it_structures;
			NewRecognizedChild * child = new NewRecognizedChild;
			child->setStructure(structure);
			possibleChildren.push_back(child);
		}

		std::map<int, std::vector<NewPairLibraryItem*> > connectionMap;

		for(std::vector<NewRecognizedChild*>::const_iterator it_children1 = possibleChildren.begin();
				it_children1 != possibleChildren.end(); it_children1++)
		{
			NewRecognizedChild * child1 = * it_children1;
			std::vector<StructurePin*> pinsChild1 = child1->getStructure().findAllPins();

			for(std::vector<NewRecognizedChild*>::const_iterator it_children2 = it_children1+1;
								it_children2 != possibleChildren.end(); it_children2++)
			{
				NewRecognizedChild * child2 = * it_children2;
				std::vector<StructurePin*> pinsChild2 = child2->getStructure().findAllPins();

				NewPairLibraryItem * possibleItem = new NewPairLibraryItem;
				possibleItem->setChild1(*child1);
				possibleItem->setChild2(*child2);

				int numberOfConnections = computeNumberOfConnections(pinsChild1, pinsChild2);
				if(numberOfConnections != 0)
				{
					addPairLibraryItemToConnectionMap(numberOfConnections, *possibleItem);
				}
			}


		}

	}

	void PairLibraryItemCreator::sortedAndCreatedPairLibraryItemsWithSmallestHierarchyLevelDifference(std::vector<NewPairLibraryItem*> &possibleItems, std::vector<NewPairLibraryItem*> &newItems, int & idCounter)
	{
		SmallestHierarchyLevelOfOneChildAndHierarchieLevelDifferenceMap sortedItems;


		for(std::vector<NewPairLibraryItem*>::const_iterator it_items = possibleItems.begin(); it_items != possibleItems.end(); it_items++)
		{
			NewPairLibraryItem * possibleItem = * it_items;
			int levelChild1 = getStructureCore().findLevel(possibleItem->getChild1().getStructure().getStructureName());
			int levelChild2 = getStructureCore().findLevel(possibleItem->getChild2().getStructure().getStructureName());
			if(levelChild1<levelChild2)
			{
				int levelDifference = levelChild2-levelChild1;
				std::map<int, std::vector<NewPairLibraryItem*> > & helperMap = sortedItems[levelChild1];
				std::vector<NewPairLibraryItem*> & libraryItems = helperMap[levelDifference];
				libraryItems.push_back(possibleItem);
			}
			else
			{
				int levelDifference = levelChild1-levelChild2;
				std::map<int, std::vector<NewPairLibraryItem*> > & helperMap = sortedItems[levelChild2];
				std::vector<NewPairLibraryItem*> & libraryItems = helperMap[levelDifference];
				libraryItems.push_back(possibleItem);
			}
		}


		for(SmallestHierarchyLevelOfOneChildAndHierarchieLevelDifferenceMap::iterator it_levels = sortedItems.begin(); it_levels !=sortedItems.end(); it_levels++)
		{
			std::map<int, std::vector<NewPairLibraryItem*> > childrenOnSameLevel = it_levels->second;

			for(std::map<int,std::vector<NewPairLibraryItem*> >::const_iterator it_levelDifferences = childrenOnSameLevel.begin(); it_levelDifferences != childrenOnSameLevel.end(); it_levelDifferences++)
			{
				std::vector<NewPairLibraryItem*> childrenWithSameLevelDifference = it_levelDifferences->second;


				createNewPairLibraryItemsWithNotUsedChildren(childrenWithSameLevelDifference, newItems, idCounter );
			}
		}

	}


	void PairLibraryItemCreator::addPairLibraryItemToConnectionMap(
				int numberOfConnections, NewPairLibraryItem & libraryItem)
	{
		std::vector<NewPairLibraryItem*> & libraryItems = connectionMap_[numberOfConnections];
		libraryItems.push_back(&libraryItem);
	}

	void PairLibraryItemCreator::createNewPairLibraryItemsWithNotUsedChildren(std::vector<NewPairLibraryItem*> &possibleItems, std::vector<NewPairLibraryItem*> & newItems, int & idCounter)
	{
		for(std::vector<NewPairLibraryItem*>::const_iterator it_items = possibleItems.begin();
							it_items != possibleItems.end(); it_items++)
		{
			NewPairLibraryItem * possibleItem = *it_items;
			if((!possibleItem->getChild1().isPartOfNewPairLibraryItem()) && (!possibleItem->getChild2().isPartOfNewPairLibraryItem()))
			{
				possibleItem->setStructureCore(getStructureCore());
				possibleItem->build(getPairLibraryItemName(), idCounter );
				getStructureCore().registerPairLibraryItem(*possibleItem);
				idCounter++;
				newItems.push_back(possibleItem);

			}
			else
			{
				delete possibleItem;
			}

		}

	}


	void PairLibraryItemCreator::setStructureCore(StructureCore & structureCore)
	{
		structureCore_ = & structureCore;
	}

	const StructureCore & PairLibraryItemCreator::getStructureCore() const
	{
		assert(hasStructureCore());
		return * structureCore_;
	}

	StructureCore & PairLibraryItemCreator::getStructureCore()
	{
		assert(hasStructureCore());
		return * structureCore_;
	}

	bool PairLibraryItemCreator::hasStructureCore() const
	{
		return structureCore_ != NULL;
	}

	int PairLibraryItemCreator::computeNumberOfConnections(std::vector<StructurePin*> & pinsChild1,
	 				std::vector<StructurePin*> & pinsChild2)
	{
		int connectionCounter = 0;

		for(std::vector<StructurePin*>::const_iterator it_pins1 = pinsChild1.begin(); it_pins1 != pinsChild1.end(); it_pins1 ++)
		{
			StructurePin * pin1 = *it_pins1;

			if(pin1->getPinType().getPinName().toStr() != "Bulk")
			{
				for(std::vector<StructurePin*>::iterator it_pins2 = pinsChild2.begin(); it_pins2 != pinsChild2.end(); it_pins2++)
				{
					StructurePin* pin2 = *it_pins2;

					if(pin2->getPinType().getPinName().toStr() != "Bulk")
					{
						if(pin1->getNet().getIdentifier() == pin2->getNet().getIdentifier())
						{
							if(pin1->getNet().getSupply().isSupply())
							{
								connectionCounter=connectionCounter+2;
							}
							else
							{
								connectionCounter=connectionCounter+3;
							}
						}
					}
				}
			}
		}
		return connectionCounter;
	}

	void PairLibraryItemCreator::setPairLibraryItemName(const std::string& pairLibraryItemName)
	{
		pairLibraryItemName_ = pairLibraryItemName;
	}

	std::string PairLibraryItemCreator::getPairLibraryItemName()
	{
		assert(hasPairLibraryItemName());
		return pairLibraryItemName_;
	}

	bool PairLibraryItemCreator::hasPairLibraryItemName() const
	{
		return !pairLibraryItemName_.empty();
	}

	void PairLibraryItemCreator::deleteEqualItem(std::vector<NewPairLibraryItem*> & createdItems)
	{
		for(std::vector<NewPairLibraryItem*>::iterator it1 = createdItems.begin(); it1 != createdItems.end(); it1++)
		{
			NewPairLibraryItem * item = *it1;
			for(std::vector<NewPairLibraryItem*>::iterator it2 = it1+1; it2 != createdItems.end(); it2++)
			{
				NewPairLibraryItem* other = *it2;
				if(item->isEqualButInName(*other))
				{
					delete other;
					createdItems.erase(it2);
					it2--;
				}
			}
		}
	}

	bool PairLibraryItemCreator::hasConnectionMap() const
	{
		return !connectionMap_.empty();
	}

	void PairLibraryItemCreator::writeConnectionMap()
	{
		int id = 1;
		for(ConnectionMap::const_iterator it = connectionMap_.begin(); it != connectionMap_.end(); it++)
		{
			int numberOfConnections = it->first;
			std::vector<NewPairLibraryItem*> items = it->second;

			for(std::vector<NewPairLibraryItem*>::const_iterator it_items = items.begin(); it_items != items.end(); it_items++)
			{
				NewPairLibraryItem * item = * it_items;
				id++;
			}
		}
	}
}
