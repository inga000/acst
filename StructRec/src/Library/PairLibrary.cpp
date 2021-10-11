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

#include "StructRec/incl/Library/PairLibrary.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Net/PairNet.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/Library/DominanceRelation.h"
#include "StructRec/incl/Library/DominanceRelationForCurrentMirrors.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	PairLibrary::PairLibrary():
			structureCore_(nullptr)
	{
	}

	PairLibrary::~PairLibrary()
	{
		eraseHierarchyLevels();
		eraseDominanceRelations();
	}

	void PairLibrary::setFilePath(const std::string& filePath)
	{
		filePath_ = filePath;
	}

	void PairLibrary::setStructureCore(StructureCore& structureCore)
	{
		structureCore_ = &structureCore;
	}

	void PairLibrary::addHierarchyLevel(int level, HierarchyLevel& hierarchyLevel)
	{
		hierarchyLevels_.insert(std::pair<int,HierarchyLevel*>(level, & hierarchyLevel));
	}

	void PairLibrary::addDominanceRelation(DominanceRelation& dominanceRelation)
	{
		dominanceRelations_.push_back(&dominanceRelation);
	}

	void PairLibrary::addDominanceRelationForCurrentMirrors(DominanceRelationForCurrentMirrors& dominanceRelation)
	{
		dominanceRelationsForCurrentMirrors_.push_back(&dominanceRelation);
	}

	void PairLibrary::addItem(PairLibraryItem & pairLibraryItem)
	{
		if(hasHierarchyLevel(pairLibraryItem.getHierarchyLevel()))
		{
			HierarchyLevel & level = findHierarchyLevel(pairLibraryItem.getHierarchyLevel());
			level.addItem(pairLibraryItem);
		}
		else
		{
			HierarchyLevel * level = new HierarchyLevel;
			level->setLevel(pairLibraryItem.getHierarchyLevel());
			level->setStructureCore(getStructureCore());
			level->addItem(pairLibraryItem);

			addHierarchyLevel(pairLibraryItem.getHierarchyLevel(),*level);
		}
	}

	void PairLibrary::addAndMerge(const PairLibrary& other, LibraryItemMappings& mappings)
	{
		const std::vector<PairLibraryItem*> otherItems = other.getPairLibraryItems();
		for(std::vector<PairLibraryItem*>::const_iterator it = otherItems.begin(); it != otherItems.end(); it++)
		{
			PairLibraryItem* currentItem = *it;
			if(!hasIdenticalPairLibraryItem(*currentItem, mappings)) {
				PairLibraryItem* copy = new PairLibraryItem(*currentItem);
				copy->setStructureCore(getStructureCore());
				copy->update(mappings);
				getStructureCore().registerPairLibraryItem(*copy);
				addItem(*copy);
			} else {
				LibraryItemMapping newMapping = creatNewMapping(*currentItem, mappings);
				mappings.addLibraryItemMapping(currentItem->getName(), newMapping);
			}
		}

		for(std::vector<DominanceRelation*>::const_iterator it = other.dominanceRelations_.begin();
				it != other.dominanceRelations_.end(); it++)
		{
			DominanceRelation* currentDominanceRelation = *it;
			if(!hasIdenticalDominanceRelation(*currentDominanceRelation, mappings))
			{
				DominanceRelation* copy = new DominanceRelation(*currentDominanceRelation);
				copy->update(mappings);
				addDominanceRelation(*copy);
			}
		}

		for(auto & currentDominanceRelation : other.dominanceRelationsForCurrentMirrors_)
		{
			if(!hasIdenticalDominanceRelation(*currentDominanceRelation, mappings))
			{
				DominanceRelationForCurrentMirrors* copy = new DominanceRelationForCurrentMirrors(*currentDominanceRelation);
				copy->update(mappings);
				addDominanceRelationForCurrentMirrors(*copy);
			}
		}
	}

	void PairLibrary::recognize(Result& results) const
	{
		for(HierarchyLevelMap::const_iterator it = getHierarchyLevelMap().begin(); it != getHierarchyLevelMap().end(); it++)
		{
			int level = it->first;

			recognizeCircuitsRecursivly(results.getTopLevelResults(), level);
			removeDominatedStructures(results, level);
			removeUncertainStructures(results);
		}
	}

	void PairLibrary::recognizeOnStructuresWithoutParents(StructureCircuits& circuits) const
	{
		for(HierarchyLevelMap::const_iterator it = getHierarchyLevelMap().begin(); it != getHierarchyLevelMap().end(); it++)
		{
			int level = it->first;
			HierarchyLevel& hierarchyLevel = findHierarchyLevel(level);
			hierarchyLevel.setStructureCore(getStructureCore());
			hierarchyLevel.recognizeOnStructuresWithoutParents(circuits);
		}
	}

	PairLibraryItem& PairLibrary::findPairLibraryItem(const StructureName& structureName)
	{
		int level = getStructureCore().findLevel(structureName);
		return findHierarchyLevel(level).findPairLibraryItem(structureName);
	}

	void PairLibrary::flagItemsNotOnTopLevelAndWithoutPersistence() const
	{
		//Function is needed to mark newly created library pairs that are not used for the final description of the structure
		for(HierarchyLevelMap::const_iterator it = getHierarchyLevelMap().begin(); it != --(getHierarchyLevelMap().end()); it++)
		{
			int level = it->first;
			const std::vector<PairLibraryItem*> items = findHierarchyLevel(level).getPairLibraryItems();
			for(std::vector<PairLibraryItem*>::const_iterator it_items = items.begin(); it_items != items.end(); it_items++)
			{
				PairLibraryItem * item = * it_items;
				if(!item->hasPersistence())
				{
					item->setItemNotOnTopLevelAndWithoutPersistence(true);
				}
			}
		}
	}

	std::string PairLibrary::getFilePath() const
	{
		assert(!filePath_.empty());
		return filePath_;
	}

	std::string PairLibrary::getImageFilePath(const StructureName& structureName) const
	{
		const PairLibraryItem& pairLibraryItem = findPairLibraryItem(structureName);
		return pairLibraryItem.getImageFilePath();
	}

	std::vector<StructureName> PairLibrary::getStructureNames() const
	{
		std::vector<StructureName> items;
		for(auto& it : hierarchyLevels_)
		{
			HierarchyLevel* currentLevel = it.second;
			std::vector<StructureName> hierarchLevelStructureNames = currentLevel->getStructureNames();
			items.insert(items.end(), hierarchLevelStructureNames.begin(), hierarchLevelStructureNames.end());
		}
		return items;
	}

	std::vector<StructurePinType> PairLibrary::getStructurePinTypes(const StructureName& structureName) const
	{
		return findPairLibraryItem(structureName).getStructurePinTypes();
	}

	void PairLibrary::writeXml(Control::OutputFile& file) const
	{
		file.open();

		Core::XmlDocument doc;
		Core::RapidXmlUtils::writeStyleDeclaration(doc);
		Core::XmlNode& root = Core::RapidXmlUtils::createXMLRootNode(doc, "pairLibrary");

		writePairLibraryXml(root,doc);

		std::ostringstream oss;
		oss << doc;
		file << oss.str();

		file.close();
	}

	void PairLibrary::recognizeCircuitsRecursivly(StructureCircuits& circuits, const int level) const
	{
		recognizePairsInInstance(circuits, level);

		HierarchyLevel& hierarchyLevel = findHierarchyLevel(level);
		hierarchyLevel.setStructureCore(getStructureCore());
		hierarchyLevel.recognize(circuits);
		completeCircuit(circuits.findStructureCircuit(level), circuits.findStructureCircuit(level-1));

	}

	void PairLibrary::recognizePairsInInstance(StructureCircuits& circuits, const int  level) const
	{
		std::vector<StructureInstance*> instances = circuits.findAllInstances();

		for(std::vector<StructureInstance*>::const_iterator it = instances.begin(); it != instances.end(); it++)
		{
			StructureInstance * currentInstance = *it;
			if(!(level == currentInstance->getSubCircuits().getMaxInitializedHierarchyLevel()))
			{
				recognizeCircuitsRecursivly(currentInstance->getSubCircuits(), level);
			}
		}
	}

	void PairLibrary::completeCircuit(StructureCircuit & circuit, StructureCircuit & lowerCircuit) const
	{
		addInstanceTerminals(circuit, lowerCircuit);
		addTerminals(circuit, lowerCircuit);
	}

	void PairLibrary::addInstanceTerminals(StructureCircuit & circuit, StructureCircuit & lowerCircuit) const
	{
		const std::vector<StructureInstanceTerminal*> instTerms = lowerCircuit.findAllInstanceTerminals();
		for(std::vector<StructureInstanceTerminal*>::const_iterator instTermIt = instTerms.begin(); instTermIt != instTerms.end(); instTermIt++)
		{
			const StructureInstanceTerminal * instTerm = *instTermIt;

			if(instTerm->getInstance().hasTerminalInHighestInitializedCircuit(instTerm->getTerminalId()))
			{
				StructureInstanceTerminal * newInstTerm = new StructureInstanceTerminal;
				newInstTerm->setInstance(instTerm->getInstance());
				newInstTerm->setTerminal(instTerm->getInstance().getTerminalOfHighestInitializedCircuit(instTerm->getTerminalId()));

				if(circuit.hasStructureNet(instTerm->getNet().getIdentifier()))
				{
					newInstTerm->connect(circuit.findStructureNet(instTerm->getNet().getIdentifier()));
				}
				else
				{
					PairNet* newNet = new PairNet;
					newNet->setNetId(instTerm->getNet().getIdentifier());
					newNet->setSupply(instTerm->getNet().getSupply());
					newNet->setPairNet(instTerm->getNet());
					newInstTerm->connect(*newNet);
					circuit.addStructureNet(*newNet);
				}
				circuit.addInstanceTerminal(*newInstTerm);
			}
		}
	}

	void PairLibrary::addTerminals(StructureCircuit & circuit, StructureCircuit & lowerCircuit) const
	{
		const std::vector<StructureTerminal*> terminalsOfLowerCircuit = lowerCircuit.findAllTerminals();
		for(std::vector<StructureTerminal*>::const_iterator it = terminalsOfLowerCircuit.begin(); it != terminalsOfLowerCircuit.end(); it++)
		{
			const StructureTerminal * term = *it;

			StructureTerminal * newTerminal = new StructureTerminal;
			newTerminal->setCircuit(circuit);
			newTerminal->setTerminalId(term->getTerminalId());

			if(term->hasTerminalType()) {
				newTerminal->setTerminalType(term->getTerminalType());
			}

			if(circuit.hasStructureNet(term->getNet().getIdentifier()))
			{
				newTerminal->setNet(circuit.findStructureNet(term->getNet().getIdentifier()));
				circuit.findStructureNet(term->getNet().getIdentifier()).setTerminal(*newTerminal);
			}
			else
			{
				PairNet * newNet = new PairNet;
				newNet->setNetId(term->getNet().getIdentifier());
				newNet->setSupply(term->getNet().getSupply());
				newNet->setPairNet(term->getNet());
				newNet->setTerminal(*newTerminal);
				newTerminal->setNet(*newNet);
				circuit.addStructureNet(*newNet);
			}
			circuit.addTerminal(*newTerminal);
		}
	}

	void PairLibrary::removeUncertainStructures(Result& results) const
	{
		results.removeUncertainStructures();
	}

	void PairLibrary::removeDominatedStructures(Result & results, int level) const
	{
		for(DominanceRelationVector::const_iterator it = dominanceRelations_.begin(); it != dominanceRelations_.end(); it++)
		{
			DominanceRelation* dominanceRelation = *it;
			dominanceRelation->setStructureCore(getStructureCore());

			results.removeDominatedStructures(*dominanceRelation, level);
		}
		for(auto & dominanceRelation : dominanceRelationsForCurrentMirrors_)
		{
			dominanceRelation->setStructureCore(getStructureCore());
			results.removeDominatedStructures(*dominanceRelation, level);
		}

	}

	bool PairLibrary::hasIdenticalDominanceRelation(const DominanceRelation& other, const LibraryItemMappings& mappings) const
	{
		bool hasIt = false;
		for(DominanceRelationVector::const_iterator it = dominanceRelations_.begin(); it != dominanceRelations_.end(); it++)
		{
			DominanceRelation* current = *it;
			if(current->isIdentical(other, mappings))
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	bool PairLibrary::hasIdenticalPairLibraryItem(const PairLibraryItem& item, const LibraryItemMappings& mappings) const
	{
		bool hasIt = false;
		for(HierarchyLevelMap::const_iterator it = getHierarchyLevelMap().begin(); it != getHierarchyLevelMap().end(); it++)
		{
			HierarchyLevel* currentLevel = it->second;
			if(currentLevel->hasIdenticalPairLibraryItem(item, mappings)) {
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	LibraryItemMapping PairLibrary::creatNewMapping(const PairLibraryItem& item, const LibraryItemMappings& mappings) const
	{
		assert(hasIdenticalPairLibraryItem(item, mappings));
		LibraryItemMapping newMapping;
		for(HierarchyLevelMap::const_iterator it = getHierarchyLevelMap().begin(); it != getHierarchyLevelMap().end(); it++)
		{
			HierarchyLevel* currentLevel = it->second;
			if(currentLevel->hasIdenticalPairLibraryItem(item, mappings))
			{
				newMapping = currentLevel->creatNewMapping(item, mappings);
				break;
			}
		}
		return newMapping;
	}

	void PairLibrary::eraseHierarchyLevels()
	{
		for (HierarchyLevelMap::const_iterator it =hierarchyLevels_.begin(); it != hierarchyLevels_.end(); it++)
		{
			delete it->second;
		}
	}

	void PairLibrary::eraseDominanceRelations()
	{
		for(DominanceRelationVector::const_iterator it = dominanceRelations_.begin(); it != dominanceRelations_.end(); it++)
		{
			delete *it;
		}

		for(DominanceRelationForCurrentMirrorsVector::const_iterator it = dominanceRelationsForCurrentMirrors_.begin(); it != dominanceRelationsForCurrentMirrors_.end(); it++)
		{
			delete *it;
		}
	}

	bool PairLibrary::hasHierarchyLevel(const int level) const
	{
		return hierarchyLevels_.find(level) != hierarchyLevels_.end();
	}

	void PairLibrary::writePairLibraryXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode & pairLibraryItemFilesNode = Core::RapidXmlUtils::addNode(node, doc, "pairLibraryItemFiles");
		Core::XmlNode & hierarchyLevelsNode = Core::RapidXmlUtils::addNode(node, doc, "hierarchyLevels");
		Core::XmlNode & dominanceRelationsNode = Core::RapidXmlUtils::addNode(node, doc, "dominanceRelations");

		for(HierarchyLevelMap::const_iterator it_levels = hierarchyLevels_.begin(); it_levels != hierarchyLevels_.end(); it_levels++)
		{
			int level_number = it_levels->first;
			HierarchyLevel* level = it_levels->second;
			const std::vector<PairLibraryItem*>& items =  level->getPairLibraryItems();

			std::stringstream ss;
			ss << level_number;
			std::string hierarchyLevel = ss.str();

			Core::XmlNode& hierarchyLevelNode = Core::RapidXmlUtils::addNode(hierarchyLevelsNode, doc, "hierarchyLevel");
			Core::RapidXmlUtils::addAttr(hierarchyLevelNode, doc, "level", hierarchyLevel);

			for(std::vector<PairLibraryItem*>::const_iterator it_items = items.begin(); it_items != items.end(); it_items++)
			{
				PairLibraryItem* item = *it_items;
				if(!item->IsItemNotOnTopLevelAndWithoutPersistence())
				{
					Core::RapidXmlUtils::addSimpleNode(pairLibraryItemFilesNode, doc, "pairLibraryItemFile", item->getFilePath());
					Core::XmlNode & pairLibraryItemNode = Core::RapidXmlUtils::addNode(hierarchyLevelNode, doc, "pairLibraryItem", item->getName().toStr() );
					if(item->hasPersistence())
					{
						std::stringstream ss;
						ss << item->getPersistence();
						std::string persistence = ss.str();
						Core::RapidXmlUtils::addAttr(pairLibraryItemNode,doc, "persistence", persistence);
					}
				}
			}

		}

		for(DominanceRelationVector::const_iterator it_relations = dominanceRelations_.begin(); it_relations != dominanceRelations_.end(); it_relations++)
		{
			DominanceRelation * relation = *it_relations;
			relation->writeXml(dominanceRelationsNode,doc);
		}
	}

	HierarchyLevel& PairLibrary::findHierarchyLevel(const int level) const
	{
		assert(hasHierarchyLevel(level));
		HierarchyLevel* hierarchyLevel = hierarchyLevels_.at(level);
		return *hierarchyLevel;
	}

	const PairLibraryItem& PairLibrary::findPairLibraryItem(const StructureName& structureName) const
	{
		int hierarchyLevel = getStructureCore().findLevel(structureName);
		return findHierarchyLevel(hierarchyLevel).findPairLibraryItem(structureName);
	}

	StructureCore& PairLibrary::getStructureCore()
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	const StructureCore& PairLibrary::getStructureCore() const
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	const std::vector<PairLibraryItem*> PairLibrary::getPairLibraryItems() const
	{
		std::vector<PairLibraryItem*> allItems;
		for(HierarchyLevelMap::const_iterator it = hierarchyLevels_.begin(); it != hierarchyLevels_.end(); it++)
		{
			HierarchyLevel * currentLevel = it->second;

			const std::vector<PairLibraryItem*> itemsOnThisLevel = currentLevel->getPairLibraryItems();
			allItems.insert(allItems.end(), itemsOnThisLevel.begin(), itemsOnThisLevel.end());
		}

		return allItems;

	}

	PairLibrary::HierarchyLevelMap& PairLibrary::getHierarchyLevelMap()
	{
		return hierarchyLevels_;
	}

	const PairLibrary::HierarchyLevelMap& PairLibrary::getHierarchyLevelMap() const
	{
		return hierarchyLevels_;
	}
}


