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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CharacteristicConnection/CharacteristicConnection.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/PairConnection.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>
#include <vector>
#include <set>

namespace StructRec
{
	PairLibraryItem::PairLibraryItem() :
			isSymmetric_(false),
			isHelperStructure_(false),
			structureCore_(NULL),
			pairConnection_(NULL),
			characteristicConnection_(NULL),
			itemNotOnTopLevelAndWithoutPersistence_(false)
	{
	}

	PairLibraryItem::PairLibraryItem(const PairLibraryItem& other):
		LibraryItem(other),
		isSymmetric_(other.isSymmetric_),
		isHelperStructure_(other.isHelperStructure_),
		structureCore_(other.structureCore_),
		pairConnection_(new PairConnection(other.getPairConnection())),
		characteristicConnection_(new CharacteristicConnection(other.getCharacteristicConnection())),
		itemNotOnTopLevelAndWithoutPersistence_(other.itemNotOnTopLevelAndWithoutPersistence_)
	{
		copyRecognitionRules(other);
	}

	PairLibraryItem::~PairLibraryItem()
	{
		delete pairConnection_;
		delete characteristicConnection_;
		eraseAllRecognitionRules();
	}

	void PairLibraryItem::setPairConnection(PairConnection& pairConnection)
	{
		pairConnection_ = &pairConnection;
	}

	void PairLibraryItem::setCharacteristicConnection(CharacteristicConnection& characteristicConnection)
	{
		characteristicConnection_ = &characteristicConnection;
	}

	void PairLibraryItem::addRecognitionRule(PairRecognitionRule& secondaryRule)
	{
		recognitionRules_.push_back(&secondaryRule);
	}

	void PairLibraryItem::setSymmetric(bool isSymmetric)
	{
		isSymmetric_ = isSymmetric;
	}

	void PairLibraryItem::setHelperStructure(bool isHelperStructure)
	{
		isHelperStructure_ = isHelperStructure;
	}

	CharacteristicConnection & PairLibraryItem::getCharacteristicConnection()
	{
		assert(characteristicConnection_ != NULL);
		return *characteristicConnection_;
	}

	const CharacteristicConnection & PairLibraryItem::getCharacteristicConnection() const
	{
		assert(characteristicConnection_ != NULL);
		return *characteristicConnection_;
	}

	std::vector<PairRecognitionRule*> & PairLibraryItem::getRecognitionRules()
	{
		return recognitionRules_;
	}

	const std::vector<PairRecognitionRule*>& PairLibraryItem::getRecognitionRules() const
	{
		return recognitionRules_;
	}

	const PairConnection& PairLibraryItem::getPairConnection() const
	{
		assert(pairConnection_ != NULL);
		return *pairConnection_;
	}

	PairConnection & PairLibraryItem::getPairConnection()
	{
		assert(pairConnection_ != NULL);
		return *pairConnection_;
	}

	bool PairLibraryItem::checkRecognitionRules(CrossInstancePair& possiblePair) const
	{
		bool isPossibleArray = true;
		const std::vector<PairRecognitionRule*>& secondaryRules = getRecognitionRules();
		for (std::vector<PairRecognitionRule*>::const_iterator it = secondaryRules.begin();
					it != secondaryRules.end(); it++)
		{
			PairRecognitionRule* rule = *it;
			if(!rule->evaluate(possiblePair))
			{
				isPossibleArray = false;
				break;
			}
		}
		return isPossibleArray;
	}

	bool PairLibraryItem::checkRecognitionRules(Pair& possiblePair) const
	{
		bool isPossiblePair = true;
		const std::vector<PairRecognitionRule*>& secondaryRules = getRecognitionRules();
		for (std::vector<PairRecognitionRule*>::const_iterator it = secondaryRules.begin();
				it != secondaryRules.end(); it++)
		{
			PairRecognitionRule* rule = *it;
			if(!rule->evaluate(possiblePair)) {
				isPossiblePair = false;
			break;
			}
		}
		return isPossiblePair;
	}

	void PairLibraryItem::addToCircuit(StructureCircuit & circuit,
		Pair& pair, int idCounter) const
	{
	    StructureId structureId;
	    structureId.setName(getName());
        structureId.setId(idCounter);
        pair.setStructureId(structureId);
        pair.setPersistence(getPersistence());

		if(pair.getChild1().getTechType() == pair.getChild2().getTechType()) {
		    pair.setTechType(pair.getChild1().getTechType());
		} else {
			pair.setTechType(Core::TechType::undefined());
		}
		getPairConnection().connectPair(circuit, pair);
		circuit.addStructure(pair);
	}

	Pair & PairLibraryItem::createNewPair(CrossInstancePair & tempPair, int idCounter, StructureCircuits & circuits) const
	{
		Pair * pair = new Pair;
		StructureId structureId;
		structureId.setName(getName());
		structureId.setId(idCounter);
		pair->setStructureId(structureId);
		pair->setSymmetric(isSymmetric_);
		pair->setPersistence(getPersistence());

		if(tempPair.getChild1().isInTopCircuit())
		{
			pair->setChild1(tempPair.getChild1().getStructure());
		}
		else
		{
			tempPair.getChild1().getStructure().setParent(*pair);
			Structure & child = createNewInInstanceChild(tempPair.getChild1(), circuits);

			pair->setChild1(child);
		}

		if(tempPair.getChild2().isInTopCircuit())
		{

			pair->setChild2(tempPair.getChild2().getStructure());
		}
		else
		{
			tempPair.getChild2().getStructure().setParent(*pair);
			Structure & child = createNewInInstanceChild(tempPair.getChild2(), circuits);

			pair->setChild2(child);
		}

		if(pair->getChild1().getTechType() == pair->getChild2().getTechType())
		{
			pair->setTechType(pair->getChild1().getTechType());
		}
		else
		{
			pair->setTechType(Core::TechType::undefined());
		}

		return * pair;
	}

	void PairLibraryItem::addPairToCircuit(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const
	{
		getPairConnection().connectCrossInstancePair(circuit, pair, tempPair);
		circuit.addStructure(pair);
	}

	void PairLibraryItem::setStructureCore(const StructureCore& core)
	{
		structureCore_ = & core;
	}

	const StructureCore& PairLibraryItem::getStructureCore() const
	{
		assert(structureCore_ != NULL);
		return * structureCore_;
	}

	void PairLibraryItem::eraseAllRecognitionRules()
	{
		const std::vector<PairRecognitionRule*>& secondaryRules = getRecognitionRules();
		for(std::vector<PairRecognitionRule*>::const_iterator it = secondaryRules.begin(); it != secondaryRules.end(); it++)
		{
			delete *it;
		}
	}

	void PairLibraryItem::recognize(StructureCircuits & circuits,
				StructureCircuit& levelCircuit)
	{
//		logDebug("recognizing " << getName());

		const std::vector<CrossInstancePair*> possiblePairs = findPossibleCrossInstancePairs(circuits);

		for(auto it : possiblePairs)
		{

			CrossInstancePair * possiblePair = it;

			bool fullfillsRecognitionRules = checkRecognitionRules(*possiblePair);
			if(fullfillsRecognitionRules)
			{
				if(isHelperStructure())
				{
					Structure& child1 = possiblePair->getChild1().getStructure();
					child1.setPersistence(getMaxPersistence());

					Structure& child2 = possiblePair->getChild2().getStructure();
					child2.setPersistence(getMaxPersistence());
				} else {

					Pair& pair = createNewPair(*possiblePair, getIdCounter(), circuits);
					addPairToCircuit(levelCircuit, pair, *possiblePair);
					getIdCounter()++;
				}
			}
		}
		deleteAllCrossInstanceStructures(possiblePairs);
	}

	std::vector<StructurePinType> PairLibraryItem::getStructurePinTypes() const
	{
		return getPairConnection().getStructurePins();
	}

	void PairLibraryItem::recognizeOnStructuresWithoutParents(StructureCircuits & circuits,
					StructureCircuit& levelCircuit)
	{
//		logDebug("recognizing " << getName());

		const std::vector<CrossInstancePair*> possiblePairs = findPossibleCrossInstancePairs(circuits);
		for(auto it : possiblePairs)
		{

			CrossInstancePair* possiblePair = it;
			bool isPair = checkRecognitionRules(*possiblePair);
			if(isPair && !possiblePair->getChild1().getStructure().hasParent() && !possiblePair->getChild2().getStructure().hasParent())
			{
				Pair & pair = createNewPair(*possiblePair, getIdCounter(), circuits);
				addPairToCircuit(levelCircuit, pair, *possiblePair);
				getIdCounter()++;
			}
		}
			deleteAllCrossInstanceStructures(possiblePairs);
	}

	const std::vector<CrossInstancePair*> PairLibraryItem::findPossibleCrossInstancePairs(
			StructureCircuits & circuits) const
	{
		std::vector<CrossInstancePair*> possiblePairs;
		std::vector<StructurePinType> characteristicConnectionPins1 = getCharacteristicConnection().getFirstChildPinTypes();
		for(auto& it : characteristicConnectionPins1)
		{
			StructurePinType caracteristicConnection1PinType = it;
			int hierarchyLevel = getStructureCore().findLevel(caracteristicConnection1PinType);
			StructureCircuit& circuit = circuits.findStructureCircuit(hierarchyLevel);
			if(!circuit.empty())
			{
				std::vector<StructureNet*> nets = circuit.findAllNets();
				for(std::vector<StructureNet*>::const_iterator netIt = nets.begin(); netIt != nets.end(); netIt++)
				{
					StructureNet * currentNet = *netIt;
					std::vector<CrossInstanceStructure*> possibleChildren1 = currentNet->findConnectedInstanceStructures(caracteristicConnection1PinType, hierarchyLevel);

					if(!possibleChildren1.empty())
					{
						std::vector<StructurePinType> characteristicConnectionPins2 = getCharacteristicConnection().getSecondChildPinTypes();
						std::vector<CrossInstanceStructure*> possibleChildren2 = findPossibleCrossInstanceChildren(*currentNet, circuit, circuits, characteristicConnectionPins2);

						if(!possibleChildren2.empty())
						{
							for(std::vector<CrossInstanceStructure*>::const_iterator child1It = possibleChildren1.begin();
									child1It != possibleChildren1.end(); child1It++)
							{
								for(std::vector<CrossInstanceStructure*>::const_iterator child2It = possibleChildren2.begin();
										child2It != possibleChildren2.end(); child2It++)
								{
									CrossInstanceStructure * possibleChild1 = *child1It;
									CrossInstanceStructure * possibleChild2 = *child2It;

									if(((possibleChild1->isInTopCircuit() && possibleChild2->isInTopCircuit()) ||
										(possibleChild1->getInstanceNamePath() != possibleChild2->getInstanceNamePath()))
										&& !(possibleChild1->getStructure().hasDirectParent(getName()) && possibleChild2->getStructure().hasDirectParent(getName())))
									{
										if(isSymmetric_)
										{
											if(*possibleChild1 < *possibleChild2)
											{
												CrossInstancePair * possiblePair = new CrossInstancePair;
												possiblePair->setChild1(*possibleChild1);
												possiblePair->setChild2(*possibleChild2);
												possiblePairs.push_back(possiblePair);
											}
										}
										else
										{
											if(*possibleChild1 != *possibleChild2)
											{
												CrossInstancePair * possiblePair = new CrossInstancePair;
												possiblePair->setChild1(*possibleChild1);
												possiblePair->setChild2(*possibleChild2);
												possiblePairs.push_back(possiblePair);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return possiblePairs;
	}


	const std::vector<CrossInstanceStructure*> PairLibraryItem::findPossibleCrossInstanceChildren(
			StructureNet& currentNet, StructureCircuit& currentCircuit, StructureCircuits& circuits,
			const std::vector<StructurePinType>& characteristicConnectionPins) const
	{
		std::vector<CrossInstanceStructure*> possibleChildren;
		for (auto& it : characteristicConnectionPins)
		{
			StructurePinType pinType = it;

			StructureNet* structureNet = NULL;
			int pinHierarchyLevel = getStructureCore().findLevel(pinType);
			if(pinHierarchyLevel == currentCircuit.getHierarchyLevel())
			{
				structureNet = &currentNet;
			}
			else
			{
				Core::NetId netId = currentNet.getIdentifier();
		        StructureCircuit & lowerCircuit = circuits.findStructureCircuit(pinHierarchyLevel);

		        if(lowerCircuit.hasStructureNet(netId))
		        {
		            structureNet = &lowerCircuit.findStructureNet(netId);
		        }
			}

			if(structureNet != NULL)
	        {
				std::vector<CrossInstanceStructure*> possibleChildrenForThisPinType;
	           	possibleChildrenForThisPinType = structureNet->findConnectedInstanceStructures(pinType, pinHierarchyLevel);
	           	possibleChildren.insert(possibleChildren.end(), possibleChildrenForThisPinType.begin(), possibleChildrenForThisPinType.end());
	        }
		}
		return possibleChildren;
	}

	void PairLibraryItem::deleteAllCrossInstanceStructures(const std::vector<CrossInstancePair*> & crossInstancePairs) const
	{
		std::set<const CrossInstanceStructure*> structures;

		for(std::vector<CrossInstancePair*>::const_iterator it = crossInstancePairs.begin(); it != crossInstancePairs.end(); it++)
		{
			CrossInstancePair * pair = *it;
			structures.insert(&(pair->getChild1()));
			structures.insert(&(pair->getChild2()));
			delete pair;
		}

		for(std::set<const CrossInstanceStructure*>::const_iterator it = structures.begin(); it != structures.end(); it++)
		{
			delete *it;
		}

	}

	void PairLibraryItem::copyRecognitionRules(const PairLibraryItem& other)
	{
		const std::vector<PairRecognitionRule*> otherRecognitionRules = other.getRecognitionRules();
		for(std::vector<PairRecognitionRule*>::const_iterator it = otherRecognitionRules.begin(); it != otherRecognitionRules.end(); it++)
		{
			PairRecognitionRule* original = *it;
			PairRecognitionRule* copy = original->clone();
			addRecognitionRule(*copy);
		}
	}

	Structure & PairLibraryItem::createNewInInstanceChild(const CrossInstanceStructure & crossInstanceChild, StructureCircuits & circuits) const
	{
		StructureCircuits & circuitsOfChild = crossInstanceChild.findStructureInstance(circuits);
		StructureCircuit & circuitOfChild = circuitsOfChild.findStructureCircuit(getStructureCore().findLevel(crossInstanceChild.getStructure().getStructureName()));

		Structure & child = crossInstanceChild.getStructure().clone(circuitOfChild, circuitsOfChild);

		std::ostringstream stream;
		stream << crossInstanceChild.getInstanceNamePath() << crossInstanceChild.getStructure().getIdentifier().getName();
		StructureName newName(stream.str());
		StructureId newId;
		newId.setName(newName);
		newId.setId(child.getIdentifier().getId());
		child.setStructureId(newId);

		return child;
	}

	bool PairLibraryItem::getIsSymmetric() const
	{
		return isSymmetric_;
	}

	void PairLibraryItem::setItemNotOnTopLevelAndWithoutPersistence(bool value)
	{
		itemNotOnTopLevelAndWithoutPersistence_ = value;
	}

	bool PairLibraryItem::IsItemNotOnTopLevelAndWithoutPersistence() const
	{
		return itemNotOnTopLevelAndWithoutPersistence_;
	}

	bool PairLibraryItem::isIdentical(const PairLibraryItem& other, const LibraryItemMappings& mappings) const
	{
		bool identicalPairConnections =  getPairConnection().isIdentical(other.getPairConnection(), mappings);
		bool identicalCharacteristicConnections = getCharacteristicConnection().isIdentical(other.getCharacteristicConnection(), mappings);

		bool identicalRecognitionRules = true;
		if(recognitionRules_.size() != other.recognitionRules_.size()) {
			identicalRecognitionRules = false;
		} else {
			for(std::vector<PairRecognitionRule*>::const_iterator it = recognitionRules_.begin(); it != recognitionRules_.end(); it++)
			{
				PairRecognitionRule* currentRule = *it;
				bool hasSameRule = false;
				for(std::vector<PairRecognitionRule*>::const_iterator it2 = other.recognitionRules_.begin(); it2 != other.recognitionRules_.end(); it2++)
				{
					PairRecognitionRule* otherRule = *it2;
					if(currentRule->isIdentical(*otherRule, mappings)) {
						hasSameRule = true;
						break;
					}
				}
				if(!hasSameRule) {
					identicalRecognitionRules = false;
					break;
				}
			}
		}
		return identicalPairConnections && identicalCharacteristicConnections && identicalRecognitionRules;
	}

	LibraryItemMapping PairLibraryItem::creatNewMapping(const PairLibraryItem& other, const LibraryItemMappings& mappings) const
	{
		return getPairConnection().creatNewMapping(other.getPairConnection(), mappings);
	}

	void PairLibraryItem::update(const LibraryItemMappings& mappings)
	{
		getPairConnection().update(mappings);
		getCharacteristicConnection().update(mappings);

		std::vector<PairRecognitionRule*> recognitionRules = getRecognitionRules();
		for(std::vector<PairRecognitionRule*>::const_iterator it = recognitionRules.begin(); it != recognitionRules.end(); it++)
		{
			PairRecognitionRule* currentRecognitionRule = *it;
			currentRecognitionRule->update(mappings);
		}
	}

	bool PairLibraryItem::isHelperStructure() const
	{
		return isHelperStructure_;
	}

	void PairLibraryItem::writeXml(Control::OutputFile& file) const
	{
		file.open();

		Core::XmlDocument doc;
		Core::RapidXmlUtils::writeStyleDeclaration(doc);
		Core::XmlNode& root = Core::RapidXmlUtils::createXMLRootNode(doc, "pairLibraryItem");


		getName().writeXml(root, doc);
		writeXmlStructureSymmetry(root,doc);
		getPairConnection().writeXml(root,doc);
		getCharacteristicConnection().writeXml(root,doc);
		writeXmlRecognitionRules(root, doc);

		std::ostringstream oss;
		oss << doc;
		file << oss.str();

		file.close();
	}

	void PairLibraryItem::writeXmlStructureSymmetry(Core::XmlNode& node,
		Core::XmlDocument& doc) const
	{
		std::string ss;
		if(getIsSymmetric())
		{
			ss = "true";
		}
		else
		{
			ss = "false";
		}
		Core::RapidXmlUtils::addSimpleNode(node, doc, "structureSymmetry", ss);
	}

	void PairLibraryItem::writeXmlRecognitionRules(Core::XmlNode& node,
		Core::XmlDocument& doc) const
	{
		Core::XmlNode & recognitionRulesNode = Core::RapidXmlUtils::addNode(node,doc,"recognitionRules");
		Core::XmlNode & netRulesNode = Core::RapidXmlUtils::addNode(recognitionRulesNode, doc, "netRules");
		Core::XmlNode & voltageLevelRulesNode = Core::RapidXmlUtils::addNode(recognitionRulesNode, doc, "supplyLevelRules");
		Core::XmlNode & connectionRules = Core::RapidXmlUtils::addNode(recognitionRulesNode,doc,"connectionRules");

		for(auto& it : getRecognitionRules())
		{
			PairRecognitionRule* recognitionRule = it;
			if(recognitionRule->isTechTypeRule())
			{
				recognitionRule->writeXml(recognitionRulesNode,doc);
			}
			else if(recognitionRule->isConnectionRule())
			{
				recognitionRule->writeXml(connectionRules, doc);
			}
			else if(recognitionRule->isNetRule())
			{
				recognitionRule->writeXml(netRulesNode, doc);
			}
			else if(recognitionRule->isVoltageLevelRule())
			{
				recognitionRule->writeXml(voltageLevelRulesNode, doc);
			}

		}
	}
}

