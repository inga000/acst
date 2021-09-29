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

#include "StructRec/incl/LibraryFile/PairLibraryItemParser.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairSupplyLevelRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CharacteristicConnection/CharacteristicConnection.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairConnectionRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairTechTypeRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairNetRule.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/PairConnection.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string PairLibraryItemParser::PAIRLIBRARYITEM_NODE_ = "pairLibraryItem";

	const std::string PairLibraryItemParser::STRUCTURESYMMETRY_NODE_ = "structureSymmetry";
	const std::string PairLibraryItemParser::HELPER_STRUCTURE_NODE_ = "helperStructure";

	const std::string PairLibraryItemParser::PAIRCONNECTION_NODE_ = "pairConnection";
	const std::string PairLibraryItemParser::PAIRPINTYPE_NODE_ = "pairPinType";
	const std::string PairLibraryItemParser::STRUCTUREPINTYPE_NODE_ = "structurePinType";
	const std::string PairLibraryItemParser::STRUCTURENAME_NODE_ = "structureName";
	const std::string PairLibraryItemParser::STRUCTUREPINNAME_NODE_ = "structurePinName";

	const std::string PairLibraryItemParser::CHILDPINTYPE_NODE_ = "childPinType";
	const std::string PairLibraryItemParser::CHILDNUMBER_NODE_ = "childNumber";

	const std::string PairLibraryItemParser::CHARACTERISTICCONNECTION_NODE_ = "characteristicConnection";
	const std::string PairLibraryItemParser::FIRSTCHILDPINTYPE_NODE_ = "firstChildPinType";
	const std::string PairLibraryItemParser::SECONDCHILDPINTYPE_NODE_ = "secondChildPinType";

	const std::string PairLibraryItemParser::RECOGNITIONRULES_NODE_ = "recognitionRules";
	const std::string PairLibraryItemParser::TECHTYPERULE_NODE_ = "techTypeRule";
	const std::string PairLibraryItemParser::FIRSTCHILDTECHTYPE_NODE_ = "firstChildTechType";
	const std::string PairLibraryItemParser::SECONDCHILDTECHTYPE_NODE_ = "secondChildTechType";
	const std::string PairLibraryItemParser::NETRULES_NODE_ = "netRules";
	const std::string PairLibraryItemParser::NETRULE_NODE_ = "netRule";
	const std::string PairLibraryItemParser::SUPPLYLEVELRULES_NODE_ = "supplyLevelRules";
	const std::string PairLibraryItemParser::SUPPLYLEVELRULE_NODE_ = "supplyLevelRule";
	const std::string PairLibraryItemParser::SUPPLYTYPE_NODE_ = "supplyType";
	const std::string PairLibraryItemParser::HIGHERLEVEL_NODE_="higherLevel";
	const std::string PairLibraryItemParser::LOWERLEVEL_NODE_="lowerLevel";
	const std::string PairLibraryItemParser::CONNECTIONRULES_NODE_ = "connectionRules";
	const std::string PairLibraryItemParser::CONNECTIONRULE_NODE_ = "connectionRule";
	const std::string PairLibraryItemParser::CONNECTED_NODE_ = "connected";
	const std::string PairLibraryItemParser::SUPPLY_NODE_ = "supply";
	const std::string PairLibraryItemParser::IMAGEFILEPATH_NODE_ = "imageFilePath";

	const std::string PairLibraryItemParser::TECHTYPE_ATTRIBUTE_ = "attribute";

	const int PairLibraryItemParser::DEFAULT_SUPPLY_LEVEL_ = -1;

	PairLibraryItemParser::PairLibraryItemParser():
			pairLibraryItem_(NULL)
	{
	}

	PairLibraryItemParser::~PairLibraryItemParser()
	{

	}

	PairLibraryItem* PairLibraryItemParser::parse()
	{
		Control::XmlInputFile::parse();
		pairLibraryItem_ = new PairLibraryItem;
		StructureName structureName = parseStructureName(*getPairLibraryItemNode());
		getPairLibraryItem().setName(structureName);
		parseStructureSymmetry();
		parsePairConnection();
		parseCharacteristicConnection();
		parseRecognitionRules();


		if(hasImageFilePathNode(*getPairLibraryItemNode()))
		{
			std::string relativePath = parseImageFilePath(*getPairLibraryItemNode());
			std::string absolutePath = computeImageFilePath(relativePath);
			getPairLibraryItem().setImageFilePath(absolutePath);
		}

		if(hasHelperStructureNode(*getPairLibraryItemNode()))
		{
			bool helperStructure = parseHelperStructureNode(*getPairLibraryItemNode());
			getPairLibraryItem().setHelperStructure(helperStructure);
		}

		return &getPairLibraryItem();
	}

	bool PairLibraryItemParser::parseHelperStructureNode(const Core::XmlNode& node) const
	{
		Core::XmlNode* helperStructureNode = node.first_node(HELPER_STRUCTURE_NODE_.c_str());
		std::string helperStructureString = helperStructureNode->value();
		return (helperStructureString == "true") ? true : false;
	}

	std::string PairLibraryItemParser::parseImageFilePath(const Core::XmlNode& node) const
	{
		Core::XmlNode* imageFilePathNode = node.first_node(IMAGEFILEPATH_NODE_.c_str());
		return imageFilePathNode->value();
	}

	std::string PairLibraryItemParser::computeImageFilePath(const std::string& relativePath) const
	{
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	void PairLibraryItemParser::parseStructureSymmetry()
	{
		Core::XmlNode* structureSymmetryNode = getPairLibraryItemNode()->first_node(STRUCTURESYMMETRY_NODE_.c_str());
		std::string structureSymmetryString = structureSymmetryNode->value();
		bool structureSymmetry = (structureSymmetryString == "true") ? true : false;
		getPairLibraryItem().setSymmetric(structureSymmetry);
	}

	void PairLibraryItemParser::parsePairConnection()
	{
		PairConnection* pairConnection = new PairConnection;
		Core::XmlNode* pairDescriptionNode = getPairLibraryItemNode()->first_node(PAIRCONNECTION_NODE_.c_str());
		Core::XmlNode* pairPinTypeNode = pairDescriptionNode->first_node(PAIRPINTYPE_NODE_.c_str());
		while(pairPinTypeNode != NULL) {

			Core::XmlNode* structurePinTypeNode = pairPinTypeNode->first_node(STRUCTUREPINTYPE_NODE_.c_str());
			StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);

			Core::XmlNode* childPinTypeNode = pairPinTypeNode->first_node(CHILDPINTYPE_NODE_.c_str());
			ChildPinType childPinType = parseChildPinType(*childPinTypeNode);

			pairConnection->addConnection(structurePinType, childPinType.getStructurePinType(), childPinType.getChildNumber());
			pairPinTypeNode = pairPinTypeNode->next_sibling(PAIRPINTYPE_NODE_.c_str());
		}
		getPairLibraryItem().setPairConnection(*pairConnection);
	}

	void PairLibraryItemParser::parseCharacteristicConnection()
	{
		Core::XmlNode* characteristicConnectionNode = getPairLibraryItemNode()->first_node(CHARACTERISTICCONNECTION_NODE_.c_str());

		CharacteristicConnection* characteristicConnection = new CharacteristicConnection;
		parseFirstChildPinTypes(*characteristicConnection, *characteristicConnectionNode);
		parseSecondChildPinTypes(*characteristicConnection, *characteristicConnectionNode);
		getPairLibraryItem().setCharacteristicConnection(*characteristicConnection);
	}

	void PairLibraryItemParser::parseFirstChildPinTypes(CharacteristicConnection& characteristicConnection,
			const rapidxml::xml_node<>& characteristicConnectionNode)
	{
		Core::XmlNode* structurePinTypeNode = characteristicConnectionNode.first_node(FIRSTCHILDPINTYPE_NODE_.c_str());
		while(structurePinTypeNode != NULL) {
			StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);
			characteristicConnection.addFirstChildPinType(structurePinType);
			structurePinTypeNode = structurePinTypeNode->next_sibling(FIRSTCHILDPINTYPE_NODE_.c_str());
		}
	}

	void PairLibraryItemParser::parseSecondChildPinTypes(CharacteristicConnection& characteristicConnection,
			const rapidxml::xml_node<>& characteristicConnectionNode)
	{
		Core::XmlNode* structurePinTypeNode = characteristicConnectionNode.first_node(SECONDCHILDPINTYPE_NODE_.c_str());
		while(structurePinTypeNode != NULL) {
			StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);
			characteristicConnection.addSecondChildPinType(structurePinType);
			structurePinTypeNode = structurePinTypeNode->next_sibling(SECONDCHILDPINTYPE_NODE_.c_str());
		}
	}

	void PairLibraryItemParser::parseRecognitionRules()
	{
		Core::XmlNode* recognitionRulesNode = getPairLibraryItemNode()->first_node(RECOGNITIONRULES_NODE_.c_str());
		parseTechTypeRule(*recognitionRulesNode);
		parseNetRules(*recognitionRulesNode);
		parseSupplyLevelRules(*recognitionRulesNode);
		parseConnectionRules(*recognitionRulesNode);

	}

	void PairLibraryItemParser::parseTechTypeRule(const Core::XmlNode& recognitionRulesNode)
	{
		Core::XmlNode* techTypeRuleNode = recognitionRulesNode.first_node(TECHTYPERULE_NODE_.c_str());
		Core::XmlAttribute* techTypeAttribute = techTypeRuleNode->first_attribute(TECHTYPE_ATTRIBUTE_.c_str());

		std::string techType(techTypeAttribute->value());

		PairTechTypeRule* pairTechTypeRule = new PairTechTypeRule;
		pairTechTypeRule->setTechTypeRule(techType);

		if(techType == "different")
		{
			Core::XmlNode* firstChildTechTypeNode = techTypeRuleNode->first_node(FIRSTCHILDTECHTYPE_NODE_.c_str());
			if(firstChildTechTypeNode != NULL)
			{
				Core::XmlNode* secondChildTechTypeNode = techTypeRuleNode->first_node(SECONDCHILDTECHTYPE_NODE_.c_str());
				Core::TechType techTypeChild1 = Core::TechType::createFromString(firstChildTechTypeNode->value());
				Core::TechType techTypeChild2 = Core::TechType::createFromString(secondChildTechTypeNode->value());
				pairTechTypeRule->setTechTypeChild1(techTypeChild1);
				pairTechTypeRule->setTechTypeChild2(techTypeChild2);
			}

		}
		getPairLibraryItem().addRecognitionRule(*pairTechTypeRule);
	}

	void PairLibraryItemParser::parseNetRules(const Core::XmlNode& recognitionRulesNode)
	{
		Core::XmlNode* netRulesNode = recognitionRulesNode.first_node(NETRULES_NODE_.c_str());
		if(netRulesNode != NULL)
		{
			Core::XmlNode* netRuleNode = netRulesNode->first_node(NETRULE_NODE_.c_str());
			while(netRuleNode != NULL)
			{
				Core::XmlNode* structurePinTypeNode = netRuleNode->first_node(STRUCTUREPINTYPE_NODE_.c_str());
				StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);

				Core::XmlNode* childNumberNode = netRuleNode->first_node(CHILDNUMBER_NODE_.c_str());
				int childNumber = parseChildNumber(*childNumberNode);

				Core::XmlNode* supplyNode = netRuleNode->first_node(SUPPLY_NODE_.c_str());
				Core::Supply supply = parseSupply(*supplyNode);

				PairNetRule* netRule = new PairNetRule;
				netRule->setPinType(structurePinType);
				netRule->setChildNumber(childNumber);
				netRule->setSupply(supply);
				getPairLibraryItem().addRecognitionRule(*netRule);

				netRuleNode = netRuleNode->next_sibling(NETRULE_NODE_.c_str());
			}
		}
	}

	void PairLibraryItemParser::parseSupplyLevelRules(const Core::XmlNode& recognitionRulesNode)
	{
		Core::XmlNode* supplyLevelRulesNode = recognitionRulesNode.first_node(SUPPLYLEVELRULES_NODE_.c_str());
		if(supplyLevelRulesNode != NULL)
		{
			Core::XmlNode* supplyLevelRuleNode = supplyLevelRulesNode->first_node(SUPPLYLEVELRULE_NODE_.c_str());
			while(supplyLevelRuleNode != NULL)
			{
				Core::XmlNode* supplyTypeNode = supplyLevelRuleNode->first_node(SUPPLYTYPE_NODE_.c_str());
				Core::Supply  typeOfSupply = parseSupply(*supplyTypeNode);

				Core::XmlNode* higherLevelNode = supplyLevelRuleNode->first_node(HIGHERLEVEL_NODE_.c_str());
				Core::XmlNode* structurePinTypeOfHigherLevelNode = higherLevelNode->first_node(STRUCTUREPINTYPE_NODE_.c_str());
				StructurePinType structurePinTypeOfHigherLevel = parseStructurePinType(*structurePinTypeOfHigherLevelNode);

				Core::XmlNode* childNumberOfHigherLevelNode = higherLevelNode->first_node(CHILDNUMBER_NODE_.c_str());
				int childNumberOfHigherLevel = parseChildNumber(*childNumberOfHigherLevelNode);

				Core::XmlNode* lowerLevelNode = supplyLevelRuleNode->first_node(LOWERLEVEL_NODE_.c_str());
				Core::XmlNode* structurePinTypeOfLowerLevelNode = lowerLevelNode->first_node(STRUCTUREPINTYPE_NODE_.c_str());
				StructurePinType structurePinTypeOfLowerLevel = parseStructurePinType(*structurePinTypeOfLowerLevelNode);

				Core::XmlNode* childNumberOfLowerLevelNode = lowerLevelNode->first_node(CHILDNUMBER_NODE_.c_str());
				int childNumberOfLowerLevel = parseChildNumber(*childNumberOfLowerLevelNode);

				PairSupplyLevelRule* supplyLevelRule = new PairSupplyLevelRule;
				supplyLevelRule->setTypeOfSupply(typeOfSupply);
				supplyLevelRule->setPinTypeOfHigherLevel(structurePinTypeOfHigherLevel);
				supplyLevelRule->setChildNumberOfHigherLevel(childNumberOfHigherLevel);
				supplyLevelRule->setPinTypeOfLowerLevel(structurePinTypeOfLowerLevel);
				supplyLevelRule->setChildNumberOfLowerLevel(childNumberOfLowerLevel);
				getPairLibraryItem().addRecognitionRule(*supplyLevelRule);

				supplyLevelRuleNode = supplyLevelRuleNode->next_sibling(SUPPLYLEVELRULE_NODE_.c_str());
			}
		}
	}

	void PairLibraryItemParser::parseConnectionRules(const Core::XmlNode& recognitionRulesNode)
	{
		Core::XmlNode* connectionRulesNode = recognitionRulesNode.first_node(CONNECTIONRULES_NODE_.c_str());
		if(connectionRulesNode != NULL)
		{
			Core::XmlNode* connectionRuleNode = connectionRulesNode->first_node(CONNECTIONRULE_NODE_.c_str());
			while(connectionRuleNode != NULL) {
				PairConnectionRule* pairConnectionRule = parseConnectionRule(*connectionRuleNode);
				getPairLibraryItem().addRecognitionRule(*pairConnectionRule);
				connectionRuleNode = connectionRuleNode->next_sibling(CONNECTIONRULE_NODE_.c_str());
			}
		}
	}

	PairConnectionRule* PairLibraryItemParser::parseConnectionRule(const rapidxml::xml_node<>& connectionRuleNode)
	{
		Core::XmlNode* connectedNode = connectionRuleNode.first_node(CONNECTED_NODE_.c_str());
		bool connected = parseConnectedNode(*connectedNode);

		Core::XmlNode* firstChildPinTypeNode = connectionRuleNode.first_node(FIRSTCHILDPINTYPE_NODE_.c_str());
		StructurePinType firstPinType = parseStructurePinType(*firstChildPinTypeNode);

		Core::XmlNode* secondChildPinTypeNode = connectionRuleNode.first_node(SECONDCHILDPINTYPE_NODE_.c_str());
		StructurePinType secondPinType = parseStructurePinType(*secondChildPinTypeNode);

		PairConnectionRule* pairConnectionRule = new PairConnectionRule;
		pairConnectionRule->setConnected(connected);
		pairConnectionRule->setPinType1(firstPinType);
		pairConnectionRule->setPinType2(secondPinType);
		return pairConnectionRule;
	}

	bool PairLibraryItemParser::parseConnectedNode(const Core::XmlNode& connectedNode)
	{
		std::string connectedString = connectedNode.value();
		return (connectedString == "true");
	}

	ChildPinType PairLibraryItemParser::parseChildPinType(const Core::XmlNode& childPinTypeNode) const
	{
		Core::XmlNode* childNumberNode = childPinTypeNode.first_node(CHILDNUMBER_NODE_.c_str());
		int childNumber = parseChildNumber(*childNumberNode);

		Core::XmlNode* structurePinTypeNode = childPinTypeNode.first_node(STRUCTUREPINTYPE_NODE_.c_str());
		StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);

		ChildPinType childPinType;
		childPinType.setChildNumber(childNumber);
		childPinType.setStructurePinType(structurePinType);
		return childPinType;
	}

	StructurePinType PairLibraryItemParser::parseStructurePinType(const Core::XmlNode& structurePinTypeNode) const
	{
		StructureName structureName = parseStructureName(structurePinTypeNode);
		StructurePinName pinName = parseStructurePinName(structurePinTypeNode);
		return StructurePinType(structureName, pinName);
	}

	StructureName PairLibraryItemParser::parseStructureName(const Core::XmlNode& node) const
	{
		Core::XmlNode* structureNameNode = node.first_node(STRUCTURENAME_NODE_.c_str());
		return StructureName(structureNameNode->value());
	}

	StructurePinName PairLibraryItemParser::parseStructurePinName(const Core::XmlNode& structurePinTypeNode) const
	{
		Core::XmlNode* pinNameNode = structurePinTypeNode.first_node(STRUCTUREPINNAME_NODE_.c_str());
		return StructurePinName(pinNameNode->value());
	}

	int PairLibraryItemParser::parseChildNumber(const Core::XmlNode& childNumberNode) const
	{
		return std::atoi(childNumberNode.value());
	}

	Core::Supply PairLibraryItemParser::parseSupply(const Core::XmlNode& supplyNode) const
	{
		Core::Supply supply;
		supply.setString(supplyNode.value());
		supply.setLevel(DEFAULT_SUPPLY_LEVEL_);
		return supply;
	}

	const Core::XmlNode* PairLibraryItemParser::getPairLibraryItemNode() const
	{
		return getDocumentNode().first_node(PAIRLIBRARYITEM_NODE_.c_str());
	}

	PairLibraryItem& PairLibraryItemParser::getPairLibraryItem()
	{
		assert(pairLibraryItem_ != NULL);
		return *pairLibraryItem_;
	}

	bool PairLibraryItemParser::hasHelperStructureNode(const Core::XmlNode& node) const
	{
		Core::XmlNode* helperStructureNode = node.first_node(HELPER_STRUCTURE_NODE_.c_str());
		return helperStructureNode != NULL;
	}

	bool PairLibraryItemParser::hasImageFilePathNode(const Core::XmlNode& node) const
	{
		Core::XmlNode* imageFilePathNode = node.first_node(IMAGEFILEPATH_NODE_.c_str());
		return imageFilePathNode != NULL;
	}

}
