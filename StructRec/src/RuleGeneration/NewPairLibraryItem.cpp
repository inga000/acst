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
#include "StructRec/incl/RuleGeneration/NewPairLibraryItem.h"
#include "StructRec/incl/RuleGeneration/PinConnectionKey.h"
#include "StructRec/incl/RuleGeneration/PinConnectionKeyMap.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairSupplyLevelRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CharacteristicConnection/CharacteristicConnection.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairConnectionRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairNetRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/PairLibrary.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include <string>

namespace StructRec{


	NewPairLibraryItem::NewPairLibraryItem() :
			child1_(NULL),
			child2_(NULL),
			pinConnectionKeyMap_(NULL)
	{
	}

	NewPairLibraryItem::~NewPairLibraryItem()
	{
		delete pinConnectionKeyMap_;
	}

	void NewPairLibraryItem::setChild1(
		 NewRecognizedChild & child1)
	{
		child1_ = & child1;
	}

	void NewPairLibraryItem::setChild2(
		 NewRecognizedChild & child2)
	{
		child2_ = & child2;
	}

	const NewRecognizedChild& NewPairLibraryItem::getChild1() const
	{
		assert(hasChild1());
		return * child1_;
	}

	const NewRecognizedChild& NewPairLibraryItem::getChild2() const
	{
		assert(hasChild2());
		return * child2_;
	}

	NewRecognizedChild& NewPairLibraryItem::getChild1()
	{
		assert(hasChild1());
		return * child1_;
	}

	NewRecognizedChild& NewPairLibraryItem::getChild2()
	{
		assert(hasChild2());
		return * child2_;
	}


	bool NewPairLibraryItem::hasChild1() const
	{
		return child1_ != NULL;
	}

	bool NewPairLibraryItem::hasChild2() const
	{
		return child2_ != NULL;
	}

	bool NewPairLibraryItem::hasChildren() const
	{
		return hasChild1() && hasChild2();
	}

	void NewPairLibraryItem::build(std::string pairLibraryItemName, int & idCounter)
	{
		initializePinConnectionKeyMap();

		initializeHierarchyLevel();
		initializeCharacteristicConnection(getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(true));
		initializeRecognitionRules();
		setSymmetric(false);

		initializeName(pairLibraryItemName, idCounter);
		PairConnection * pairConnection = new PairConnection;
		pairConnection->initialize(getName(), getChild1(), getChild2());
		setPairConnection(*pairConnection);

		getChild1().setIsPartOfNewPairLibraryItem(true);
		getChild2().setIsPartOfNewPairLibraryItem(true);
	}

	void NewPairLibraryItem::initializeHierarchyLevel()
	{
		int hierarchyLevelChild1 = getStructureCore().findLevel(getChild1().getStructure().getStructureName());
		int hierarchyLevelChild2 = getStructureCore().findLevel(getChild2().getStructure().getStructureName());

		if(hierarchyLevelChild1 < hierarchyLevelChild2)
		{
			setHierarchyLevel(hierarchyLevelChild2 + 1);
		}
		else
		{
			setHierarchyLevel(hierarchyLevelChild1 + 1);
		}
	}

	void NewPairLibraryItem::initializeRecognitionRules()
	{
		initializeTechTypeRule();
		initializeNetRules();
		initializeSupplyLevelRules();
		initializeConnectionRulesOfConnectedPins(getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(true));
		initializeConnectionRulesOfNotConnectedPins(getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(false));
	}

	void NewPairLibraryItem::initializePinConnectionKeyMap()
	{
		std::vector<StructurePin*> child1Pins = getChild1().getStructure().findAllPins();
		std::vector<StructurePin*> child2Pins = getChild2().getStructure().findAllPins();
		PinConnectionKeyMap * pinConnectionKeyMap = new PinConnectionKeyMap;
		for (std::vector<StructurePin*>::iterator child1_iterator = child1Pins.begin(); child1_iterator !=child1Pins.end(); child1_iterator ++)
		{
			StructurePin * child1Pin = *child1_iterator;
			if(child1Pin->getPinType().getPinName().toStr() != "Bulk" )
			{
				PinConnectionKey * connectionKey = new PinConnectionKey;
				connectionKey->setChildNumber(1);
				connectionKey->setPin(*child1Pin);
				for(std::vector<StructurePin*>::iterator child2_iterator = child2Pins.begin(); child2_iterator !=child2Pins.end(); child2_iterator ++)
				{
					StructurePin * child2Pin = * child2_iterator;
					if(child2Pin->getPinType().getPinName().toStr() != "Bulk" )
					{
						if(child1Pin->getNet().getIdentifier() == child2Pin->getNet().getIdentifier())
						{
							connectionKey->setConnected(true);
							connectionKey->setConnectedChildNumber(2);
							connectionKey->setConnectedPin(*child2Pin);
						}
					}
				}
				pinConnectionKeyMap->addPinConnectionKey(*connectionKey);
			}
		}

		for (std::vector<StructurePin*>::iterator child2_iterator = child2Pins.begin(); child2_iterator !=child2Pins.end(); child2_iterator ++)
		{
			StructurePin * child2Pin = *child2_iterator;
			if(child2Pin->getPinType().getPinName().toStr() != "Bulk" )
			{
				PinConnectionKey * connectionKey = new PinConnectionKey;
				connectionKey->setChildNumber(2);
				connectionKey->setPin(*child2Pin);

				for(std::vector<StructurePin*>::iterator child1_iterator = child1Pins.begin(); child1_iterator !=child1Pins.end(); child1_iterator ++)
				{
					StructurePin * child1Pin = * child1_iterator;
					if(child1Pin->getPinType().getPinName().toStr() != "Bulk" )
					{
						if(child1Pin->getNet().getIdentifier()== child2Pin->getNet().getIdentifier())
						{
							connectionKey->setConnected(true);
							connectionKey->setConnectedChildNumber(1);
							connectionKey->setConnectedPin(*child1Pin);
						}
					}
				}
				pinConnectionKeyMap->addPinConnectionKey(*connectionKey);
			}
		}
		setPinConnectionKeyMap(*pinConnectionKeyMap);
	}

	void NewPairLibraryItem::initializeTechTypeRule()
	{
		PairTechTypeRule * techTypeRule = new PairTechTypeRule;
		techTypeRule->initialize(getChild1().getStructure(), getChild2().getStructure());
		addRecognitionRule(*techTypeRule);
	}

	void NewPairLibraryItem::initializeNetRules()
	{

		std::vector<StructurePin*> child1Pins = getChild1().getStructure().findAllPins();
		std::vector<StructurePin*> child2Pins = getChild2().getStructure().findAllPins();
		for (std::vector<StructurePin*>::iterator child1_iterator = child1Pins.begin(); child1_iterator !=child1Pins.end(); child1_iterator ++)
		{
			StructurePin * childPin = * child1_iterator;
			if(childPin->getPinType().getPinName().toStr() != "Bulk" && childPin->getNet().getSupply() != Core::Supply::noSupply())
			{
				PairNetRule * netRule = new PairNetRule;
				netRule->setPinType(childPin->getPinType());
				netRule->setChildNumber(1);
				netRule->setSupply(childPin->getNet().getSupply());
				addRecognitionRule(*netRule);
			}

		}

		for (std::vector<StructurePin*>::iterator child2_iterator = child2Pins.begin(); child2_iterator !=child2Pins.end(); child2_iterator ++)
		{
			StructurePin * childPin = * child2_iterator;
			if(childPin->getPinType().getPinName().toStr() != "Bulk" && childPin->getNet().getSupply() != Core::Supply::noSupply())
			{
				PairNetRule * netRule = new PairNetRule;
				netRule->setPinType(childPin->getPinType());
				netRule->setChildNumber(2);
				netRule->setSupply(childPin->getNet().getSupply());
				addRecognitionRule(*netRule);
			}

		}

	}

	void NewPairLibraryItem::initializeSupplyLevelRules()
	{
		std::vector<StructurePin*> child1Pins = getChild1().getStructure().findAllPins();
		std::vector<StructurePin*> child2Pins = getChild2().getStructure().findAllPins();

		for (std::vector<StructurePin*>::iterator child1_iterator = child1Pins.begin(); child1_iterator !=child1Pins.end(); child1_iterator ++)
		{
			StructurePin * child1Pin = * child1_iterator;
			if(child1Pin->getPinType().getPinName().toStr() != "Bulk" && child1Pin->getNet().getSupply().isSupply())
			{
				Core::Supply child1Supply = child1Pin->getNet().getSupply();
				for (std::vector<StructurePin*>::iterator child2_iterator = child2Pins.begin(); child2_iterator !=child2Pins.end(); child2_iterator ++)
				{
					StructurePin * child2Pin = * child2_iterator;
					if(child2Pin->getPinType().getPinName().toStr() != "Bulk" && child2Pin->getNet().getSupply().isSameSupplyType(child1Supply))
					{
						if(child1Supply.isHigherToSameSupply(child2Pin->getNet().getSupply()))
						{
							PairSupplyLevelRule * voltageLevelRule = new PairSupplyLevelRule;
							voltageLevelRule->setPinTypeOfHigherLevel(child1Pin->getPinType());
							voltageLevelRule->setChildNumberOfHigherLevel(1);
							voltageLevelRule->setPinTypeOfLowerLevel(child2Pin->getPinType());
							voltageLevelRule->setChildNumberOfLowerLevel(2);
							voltageLevelRule->setTypeOfSupply(child1Supply);
							addRecognitionRule(*voltageLevelRule);
						}
						else if(child2Pin->getNet().getSupply().isHigherToSameSupply(child1Supply))
						{
							PairSupplyLevelRule * voltageLevelRule = new PairSupplyLevelRule;
							voltageLevelRule->setPinTypeOfHigherLevel(child2Pin->getPinType());
							voltageLevelRule->setChildNumberOfHigherLevel(2);
							voltageLevelRule->setPinTypeOfLowerLevel(child1Pin->getPinType());
							voltageLevelRule->setChildNumberOfLowerLevel(1);
							voltageLevelRule->setTypeOfSupply(child1Supply);
							addRecognitionRule(*voltageLevelRule);
						}
					}

				}

			}
		}
	}

	void NewPairLibraryItem::initializeConnectionRulesOfConnectedPins(
					std::map<int,std::vector<PinConnectionKey*> > pinConnectionKeysOfConnectedPins)
	{
		std::vector<PinConnectionKey*> & pinConnectionKeysOfChild1 =  pinConnectionKeysOfConnectedPins[1];
		for(std::vector<PinConnectionKey*>::iterator it = pinConnectionKeysOfChild1.begin();
					it != pinConnectionKeysOfChild1.end(); it++)
		{
			PinConnectionKey * pinConnectionKey = *it;
			if(!pinConnectionKey->isChracteristicConnection())
			{
				PairConnectionRule * connectionRule = new PairConnectionRule;

				connectionRule->setPinType1(pinConnectionKey->getPin().getPinType());
				connectionRule->setPinType2(pinConnectionKey->getConnectedPin().getPinType());
				connectionRule->setConnected(pinConnectionKey->getConnected());

				addRecognitionRule(*connectionRule);
			}
		}
	}

	void NewPairLibraryItem::initializeCharacteristicConnection(
			std::map<int, std::vector<PinConnectionKey*> > pinConnectionKeysOfConnectedPins)
	{
		CharacteristicConnection * characteristicConnection = new CharacteristicConnection;

		std::map<int, std::vector<Core::NetId> > sortedNetsMap = sortNetsAfterNumberOfPinConnectionsOnDeviceLevel();

		std::vector<PinConnectionKey*> & pinConnectionKeysOfChild1 =  pinConnectionKeysOfConnectedPins[1];
		for(std::map<int, std::vector<Core::NetId> >::const_reverse_iterator it_connections = sortedNetsMap.rbegin(); it_connections != sortedNetsMap.rend(); it_connections++)
		{
			std::vector<Core::NetId> nets = it_connections->second;

			for(std::vector<Core::NetId>::const_iterator it_nets = nets.begin(); it_nets != nets.end(); it_nets++)
			{
				Core::NetId currentNet = * it_nets;
				for(std::vector<PinConnectionKey*>::iterator it = pinConnectionKeysOfChild1.begin(); it != pinConnectionKeysOfChild1.end(); it++)
				{
					PinConnectionKey* currentKey = *it;
					if(currentKey->getPin().getNet().getIdentifier() == currentNet)
					{
						characteristicConnection->addFirstChildPinType(currentKey->getPin().getPinType());
						characteristicConnection->addSecondChildPinType(currentKey->getConnectedPin().getPinType());
						currentKey->setIsCharacteristicConnection(true);
						setCharacteristicConnection(*characteristicConnection);
						break;
					}
				}
				if(characteristicConnection->isInitialized())
				{
					break;
				}
			}
			if(characteristicConnection->isInitialized())
			{
				break;
			}
		}
	}

	void NewPairLibraryItem::initializeSymmetrie()
	{
		setSymmetric(true);
		if(!pinConnectionKeyMap_->PinConnectionsOfChildrenAreEquivalent())
		{
			setSymmetric(false);
		}

		if(getChild1().getStructure().getTechType() != getChild2().getStructure().getTechType())
		{
			setSymmetric(false);
		}
	}

	void NewPairLibraryItem::initializeConnectionRulesOfNotConnectedPins(
			std::map<int, std::vector<PinConnectionKey*> > pinConnectionKeysOfNotConnectedPins)
	{

		std::vector<PinConnectionKey*> & pinConnectionKeysOfChild1 =  pinConnectionKeysOfNotConnectedPins[1];
		std::vector<PinConnectionKey*> & pinConnectionKeysOfChild2 =  pinConnectionKeysOfNotConnectedPins[2];
		for(std::vector<PinConnectionKey*>::iterator it_child1 = pinConnectionKeysOfChild1.begin();
				it_child1 != pinConnectionKeysOfChild1.end(); it_child1 ++)
		{
			PinConnectionKey * pinConnectionKeyChild1 = *it_child1;
			for(std::vector<PinConnectionKey*>::iterator it_child2 = pinConnectionKeysOfChild2.begin();
				it_child2 != pinConnectionKeysOfChild2.end(); it_child2 ++)
			{
				PinConnectionKey * pinConnectionKeyChild2 = *it_child2;
				PairConnectionRule * connectionRule = new PairConnectionRule;

				connectionRule->setPinType1(pinConnectionKeyChild1->getPin().getPinType());
				connectionRule->setPinType2(pinConnectionKeyChild2->getPin().getPinType());
				connectionRule->setConnected(false);

				addRecognitionRule(*connectionRule);
			}
		}
	}

	std::map<int, std::vector<Core::NetId> > NewPairLibraryItem::sortNetsAfterNumberOfPinConnectionsOnDeviceLevel() const
	{
		std::vector<const Core::Device*> devicesOfChild1 = getChild1().getStructure().getDevices();
		std::vector<const Core::Device*> devicesOfChild2 = getChild2().getStructure().getDevices();

		std::map<int , std::vector<Core::NetId> > numberOfConnectionsToNetMap;
		std::map<Core::NetId, int > netToNumberOfConnectionsMap;
		for(auto& it_child1 : devicesOfChild1)
		{
			const Core::Device &  device = * it_child1;
			std::vector<const Core::Pin*> pins = device.findAllPins();
			for(auto& it_pins : pins)
			{
				const Core::Pin& pin = * it_pins;
				if(pin.getPinType().getPinName().toStr() != "Bulk")
				{
					++netToNumberOfConnectionsMap[pin.getNet().getIdentifier()];
					if(pin.getNet().getSupply().isSupply())
					{
						--netToNumberOfConnectionsMap[pin.getNet().getIdentifier()];
					}
				}
			}
		}

		for(auto& it_child2 : devicesOfChild2)
		{
			const Core::Device & device = * it_child2;
			std::vector<const Core::Pin*> pins = device.findAllPins();
			for(auto& it_pins : pins)
			{
				const Core::Pin& pin = * it_pins;
				if(pin.getPinType().getPinName().toStr() != "Bulk")
				{
					++netToNumberOfConnectionsMap[pin.getNet().getIdentifier()];
					if(pin.getNet().getSupply().isSupply())
					{
						--netToNumberOfConnectionsMap[pin.getNet().getIdentifier()];
					}
				}
			}
		}


		for(std::map<Core::NetId, int>::iterator it_nets = netToNumberOfConnectionsMap.begin(); it_nets != netToNumberOfConnectionsMap.end(); it_nets++)
		{
			Core::NetId currentNet = it_nets->first;
			int numberOfConnections = it_nets->second;
			std::vector<Core::NetId> & netIds = numberOfConnectionsToNetMap[numberOfConnections];
			netIds.push_back(currentNet);
		}

		return numberOfConnectionsToNetMap;
	}

	void NewPairLibraryItem::initializeName(std::string pairLibraryItemName, int & idCounter)
	{
		std::stringstream ss;
		ss << idCounter;
		std::string number = ss.str();

		StructureName  name(pairLibraryItemName + number);
		setName(name);
	}

	void NewPairLibraryItem::setPinConnectionKeyMap(
			PinConnectionKeyMap& pinConnectionKeyMap)
	{
		pinConnectionKeyMap_ = & pinConnectionKeyMap;
	}

	PinConnectionKeyMap& NewPairLibraryItem::getPinConnectionKeyMap()
	{
		assert(hasPinConnectionKeyMap());
		return * pinConnectionKeyMap_;
	}

	bool NewPairLibraryItem::hasPinConnectionKeyMap()
	{
		return pinConnectionKeyMap_ != NULL;
	}

	bool NewPairLibraryItem::isEqualButInName(NewPairLibraryItem & other)
	{
		Structure & child1 = getChild1().getStructure();
		Structure & child2 = getChild2().getStructure();

		Pair dummyPair1;
		Pair dummyPair2;

		dummyPair1.setChild1(child1);
		dummyPair1.setChild2(child2);
		dummyPair2.setChild1(child2);
		dummyPair2.setChild2(child1);

		Structure & otherChild1 = other.getChild1().getStructure();
		Structure & otherChild2 = other.getChild2().getStructure();

		Pair otherDummyPair1;
		Pair otherDummyPair2;

		otherDummyPair1.setChild1(otherChild1);
		otherDummyPair1.setChild2(otherChild2);
		otherDummyPair2.setChild1(otherChild2);
		otherDummyPair2.setChild2(otherChild1);

		bool isEqual = false;
		std::map<int, std::vector<PinConnectionKey*> > & connectedPins = getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(true);
		std::map<int, std::vector<PinConnectionKey*> > & connectedPinsOfOther = other.getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(true);

		std::vector<PinConnectionKey*> connectedPinsChild1 = connectedPins[1];
		std::vector<PinConnectionKey*> connectedPinsOfOtherChild1 = connectedPinsOfOther[1];
		std::vector<PinConnectionKey*> connectedPinsChild2 = connectedPins[2];
		std::vector<PinConnectionKey*> connectedPinsOfOtherChild2 = connectedPinsOfOther[2];

		std::map<int, std::vector<PinConnectionKey*> > & notConnectedPins = getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(false);
		std::map<int, std::vector<PinConnectionKey*> > & notConnectedPinsOfOther = other.getPinConnectionKeyMap().findChildNumberToPinConnectionKeysMap(false);

		std::vector<PinConnectionKey*> notConnectedPinsChild1 = notConnectedPins[1];
		std::vector<PinConnectionKey*> notConnectedPinsOfOtherChild1 = notConnectedPinsOfOther[1];
		std::vector<PinConnectionKey*> notConnectedPinsChild2 = notConnectedPins[2];
		std::vector<PinConnectionKey*> notConnectedPinsOfOtherChild2 = notConnectedPinsOfOther[2];

		if( (getChild1().getStructure().getStructureName() == other.getChild1().getStructure().getStructureName() &&
				hasEqualPinTypes(connectedPinsChild1, connectedPinsOfOtherChild1) && hasEqualPinTypes(notConnectedPinsChild1, notConnectedPinsOfOtherChild1) &&
				getChild2().getStructure().getStructureName() == other.getChild2().getStructure().getStructureName()
				&& hasEqualPinTypes(connectedPinsChild2, connectedPinsOfOtherChild2) && hasEqualPinTypes(notConnectedPinsChild2, notConnectedPinsOfOtherChild2)
				&& checkRecognitionRules(otherDummyPair1) && other.checkRecognitionRules(dummyPair1))
				|| ( getChild1().getStructure().getStructureName() == other.getChild2().getStructure().getStructureName() &&
				hasEqualPinTypes(connectedPinsChild1, connectedPinsOfOtherChild2) && hasEqualPinTypes(notConnectedPinsChild1, notConnectedPinsOfOtherChild2) &&
				getChild2().getStructure().getStructureName() == other.getChild1().getStructure().getStructureName()
				&& hasEqualPinTypes(connectedPinsChild2, connectedPinsOfOtherChild1) && hasEqualPinTypes(notConnectedPinsChild2, notConnectedPinsOfOtherChild1)
				&& checkRecognitionRules(otherDummyPair2) && other.checkRecognitionRules(dummyPair2)))
		{
			isEqual = true;
		}

		return isEqual;
	}

	bool NewPairLibraryItem::hasEqualPinTypes(std::vector<PinConnectionKey*> connectionKeys, std::vector<PinConnectionKey*> connectionKeysOfOther)
	{
		bool equalPinTypes = true;

		if(connectionKeys.size() == connectionKeysOfOther.size())
		{
			std::vector<PinConnectionKey*>::const_iterator it, it_other;

			for(it = connectionKeys.begin(), it_other = connectionKeysOfOther.begin();
					it != connectionKeys.end(), it_other != connectionKeysOfOther.end();
					it++, it_other++)
			{
				PinConnectionKey* connectionKey = *it;
				PinConnectionKey* connectionKeyOfOther = *it_other;

				if(connectionKey->getPin().getPinType() != connectionKeyOfOther->getPin().getPinType())
				{
					equalPinTypes = false;
					break;
				}

				if(connectionKey->getConnected() == true)
				{
					if(connectionKey->getConnectedPin().getPinType() != connectionKeyOfOther->getConnectedPin().getPinType())
					{
						equalPinTypes = false;
						break;
					}
				}

			}
		}

		return equalPinTypes;
	}

	void NewPairLibraryItem::computePersistenceOfChildren(PairLibrary & libraryOfNewItems, ArrayLibrary& arrayLibrary, std::string nameMainItem) const
	{
		computePersistenceOfChild(getChild1().getStructure(), libraryOfNewItems, arrayLibrary, nameMainItem);
		computePersistenceOfChild(getChild2().getStructure(), libraryOfNewItems, arrayLibrary, nameMainItem);
	}

	void NewPairLibraryItem::computePersistenceOfChild(const Structure  & child, PairLibrary & libraryOfNewItems, ArrayLibrary& arrayLibrary ,std::string nameMainItem) const
	{
		int hierarchyLevelChild = getStructureCore().findLevel(child.getStructureName());
		int possiblePersistence = getHierarchyLevel() - hierarchyLevelChild;

		LibraryItem * itemOfChild = NULL;
		std::string nameOfChild = child.getStructureName().toStr();

		if(child.isPair())
		{
			if(nameOfChild.compare(0, nameMainItem.size(), nameMainItem) == 0)
			{
				itemOfChild = &libraryOfNewItems.findPairLibraryItem(nameOfChild);
				if(!itemOfChild->hasPersistence())
				{
					itemOfChild->setPersistence(possiblePersistence);
				}
			}
		}
		else
		{
			itemOfChild = & arrayLibrary.findArrayLibraryItem(child.getStructureName());
		}

		if(itemOfChild != NULL) {
			if(itemOfChild->getPersistence() < possiblePersistence && !itemOfChild->hasMaxPersistence())
			{
				itemOfChild->setPersistence(possiblePersistence);
			}
		}
	}
}
