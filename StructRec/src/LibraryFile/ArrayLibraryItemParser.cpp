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

#include "StructRec/incl/LibraryFile/ArrayLibraryItemParser.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/ArrayConnection.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayRecognitionRules/ArrayConnectionRule.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayRecognitionRules/ArrayTypeRule.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNets.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string ArrayLibraryItemParser::ARRAYLIBRARYITEM_NODE_ = "arrayLibraryItem";

	const std::string ArrayLibraryItemParser::ARRAYCONNECTION_NODE_ = "arrayConnection";
	const std::string ArrayLibraryItemParser::CONNECTION_NODE_ = "connection";

	const std::string ArrayLibraryItemParser::STRUCTUREPINTYPE_NODE_ = "structurePinType";
	const std::string ArrayLibraryItemParser::DEVICEPINTYPE_NODE_ = "devicePinType";

	const std::string ArrayLibraryItemParser::STRUCTURENAME_NODE_ = "structureName";
	const std::string ArrayLibraryItemParser::STRUCTUREPINNAME_NODE_ = "structurePinName";

	const std::string ArrayLibraryItemParser::DEVICETYPENAME_NODE_ = "deviceTypeName";
	const std::string ArrayLibraryItemParser::DEVICEPINNAME_NODE_ = "devicePinName";

	const std::string ArrayLibraryItemParser::RECOGNITIONRULES_NODE_ = "recognitionRules";

	const std::string ArrayLibraryItemParser::DEVICETYPERULE_NODE_ = "deviceTypeRule";
	const std::string ArrayLibraryItemParser::CONNECTIONRULES_NODE_ = "connectionRules";
	const std::string ArrayLibraryItemParser::CONNECTIONRULE_NODE_ = "connectionRule";
	const std::string ArrayLibraryItemParser::CONNECTED_NODE_ = "connected";

	const std::string ArrayLibraryItemParser::IMAGEFILEPATH_NODE_ = "imageFilePath";

	const std::string ArrayLibraryItemParser::PARALLELNETS_NODE_ = "parallelNets";

	ArrayLibraryItemParser::ArrayLibraryItemParser():
			deviceTypeRegister_(NULL),
			arrayLibraryItem_(NULL)
	{
	}

	void ArrayLibraryItemParser::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
	{
		deviceTypeRegister_ = &deviceTypeRegister;
	}

	ArrayLibraryItem* ArrayLibraryItemParser::parse()
	{
		arrayLibraryItem_ = new ArrayLibraryItem;

		Control::XmlInputFile::parse();
		getArrayLibraryItem().setName(parseStructureName(*getArrayLibraryItemNode()));
		getArrayLibraryItem().setImageFilePath(parseImageFilePath(*getArrayLibraryItemNode()));

		parseArrayConnection();
		parseRecognitionRules();
		parseParallelNets();

		return &getArrayLibraryItem();
	}

	std::string ArrayLibraryItemParser::parseImageFilePath(const Core::XmlNode& node) const
	{
		Core::XmlNode* imageFilePathNode = node.first_node(IMAGEFILEPATH_NODE_.c_str());
		std::string relativePath = imageFilePathNode->value();
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	StructureName ArrayLibraryItemParser::parseStructureName(const Core::XmlNode& node) const
	{
		Core::XmlNode* structureNameNode = node.first_node(STRUCTURENAME_NODE_.c_str());
		return StructureName(structureNameNode->value());
	}

	StructurePinName ArrayLibraryItemParser::parseStructurePinName(const Core::XmlNode& structurePinTypeNode) const
	{
		Core::XmlNode* structurePinNameNode = structurePinTypeNode.first_node(STRUCTUREPINNAME_NODE_.c_str());
		return StructurePinName(structurePinNameNode->value());
	}

	void ArrayLibraryItemParser::parseArrayConnection()
	{
		ArrayConnection* arrayConnection = new ArrayConnection;

		Core::XmlNode* arrayDescriptionNode = getArrayLibraryItemNode()->first_node(ARRAYCONNECTION_NODE_.c_str());
		Core::XmlNode* connectionNode = arrayDescriptionNode->first_node(CONNECTION_NODE_.c_str());
		while(connectionNode != NULL) {

			Core::XmlNode* structurePinTypeNode = connectionNode->first_node(STRUCTUREPINTYPE_NODE_.c_str());
			Core::XmlNode* devicePinTypeNode = connectionNode->first_node(DEVICEPINTYPE_NODE_.c_str());

			StructurePinType structurePinType = parseStructurePinType(*structurePinTypeNode);
			Core::PinType devicePinType = parseDevicePinType(*devicePinTypeNode);

			arrayConnection->addConnection(structurePinType, devicePinType);

			connectionNode = connectionNode->next_sibling(CONNECTION_NODE_.c_str());
		}
		getArrayLibraryItem().setArrayConnection(*arrayConnection);
	}

	void ArrayLibraryItemParser::parseRecognitionRules()
	{
		Core::XmlNode* recognitionRulesNode = getArrayLibraryItemNode()->first_node(RECOGNITIONRULES_NODE_.c_str());
		parseDeviceTypeRule(*recognitionRulesNode);
		parseConnectionRules(*recognitionRulesNode);
	}

	void ArrayLibraryItemParser::parseDeviceTypeRule(const Core::XmlNode& recognitionRulesNode)
	{
		Core::XmlNode* deviceTypeRuleNode = recognitionRulesNode.first_node(DEVICETYPERULE_NODE_.c_str());
		Core::DeviceTypeName deviceTypeName(deviceTypeRuleNode->value());
		assert(getDeviceTypeRegister().hasDeviceType(deviceTypeName));

		ArrayTypeRule* arrayTypeRule = new ArrayTypeRule();
		arrayTypeRule->setType(deviceTypeName);
		getArrayLibraryItem().addRecognitionRule(*arrayTypeRule);
	}

	void ArrayLibraryItemParser::parseConnectionRules(const Core::XmlNode& primaryRulesNode)
	{
		Core::XmlNode* connectionRulesNode = primaryRulesNode.first_node(CONNECTIONRULES_NODE_.c_str());
		Core::XmlNode* connectionRuleNode = connectionRulesNode->first_node(CONNECTIONRULE_NODE_.c_str());
		while(connectionRuleNode != NULL) {
			ArrayConnectionRule* arrayConnectionRule = parseConnectionRule(*connectionRuleNode);
			getArrayLibraryItem().addRecognitionRule(*arrayConnectionRule);
			connectionRuleNode = connectionRuleNode->next_sibling(CONNECTIONRULE_NODE_.c_str());
		}
	}

	ArrayConnectionRule* ArrayLibraryItemParser::parseConnectionRule(const Core::XmlNode& connectionRuleNode) const
	{
		Core::XmlNode* connectedNode = connectionRuleNode.first_node(CONNECTED_NODE_.c_str());
		bool connected = parseConnectedNode(*connectedNode);

		Core::XmlNode* devicePinTypeNode = connectionRuleNode.first_node(DEVICEPINTYPE_NODE_.c_str());
		Core::PinType firstPinType = parseDevicePinType(*devicePinTypeNode);

		devicePinTypeNode = devicePinTypeNode->next_sibling(DEVICEPINTYPE_NODE_.c_str());
		Core::PinType secondPin = parseDevicePinType(*devicePinTypeNode);

		ArrayConnectionRule* arrayConnectionRule = new ArrayConnectionRule();
		arrayConnectionRule->setConnected(connected);
		arrayConnectionRule->setPinType1(firstPinType);
		arrayConnectionRule->setPinType2(secondPin);
		return arrayConnectionRule;
	}

	bool ArrayLibraryItemParser::parseConnectedNode(const Core::XmlNode& connectedNode) const
	{
		std::string connectedString = connectedNode.value();
		bool connected;
		if(connectedString == "true") {
			connected = true;
		} else {
			connected = false;
		}
		return connected;
	}

	void ArrayLibraryItemParser::parseParallelNets()
	{
		Core::XmlNode* parallelNetsNode = getArrayLibraryItemNode()->first_node(PARALLELNETS_NODE_.c_str());
		Core::XmlNode* devicePinTypeNode = parallelNetsNode->first_node(DEVICEPINTYPE_NODE_.c_str());

		ParallelNets* parallelNets = new ParallelNets;
		while(devicePinTypeNode != NULL) {
			Core::PinType pinType = parseDevicePinType(*devicePinTypeNode);
			parallelNets->addParallelNet(pinType);

			devicePinTypeNode = devicePinTypeNode->next_sibling(DEVICEPINTYPE_NODE_.c_str());
		}
		getArrayLibraryItem().setParallelNets(*parallelNets);
	}

	StructurePinType ArrayLibraryItemParser::parseStructurePinType(const Core::XmlNode& structurePinTypeNode) const
	{
		StructureName structureName = parseStructureName(structurePinTypeNode);
		StructurePinName structurePinName = parseStructurePinName(structurePinTypeNode);
		return StructurePinType(structureName, structurePinName);
	}

	Core::PinType ArrayLibraryItemParser::parseDevicePinType(const Core::XmlNode& devicePinTypeNode) const
	{
		Core::DeviceTypeName deviceTypeName = parseDeviceTypeName(devicePinTypeNode);
		Core::PinName pinName = parseDevicePinName(devicePinTypeNode);

		const Core::DeviceType& deviceType = getDeviceTypeRegister().findDeviceType(deviceTypeName);
		return deviceType.findPinType(pinName);
	}

	Core::DeviceTypeName ArrayLibraryItemParser::parseDeviceTypeName(const Core::XmlNode& devicePinTypeNode) const
	{
		Core::XmlNode* deviceTypeNameNode = devicePinTypeNode.first_node(DEVICETYPENAME_NODE_.c_str());
		return Core::DeviceTypeName(deviceTypeNameNode->value());
	}


	Core::PinName ArrayLibraryItemParser::parseDevicePinName(const Core::XmlNode& pinNode) const
	{
		Core::XmlNode* pinNameNode = pinNode.first_node(DEVICEPINNAME_NODE_.c_str());
		return Core::PinName(pinNameNode->value());
	}

	const Core::DeviceTypeRegister& ArrayLibraryItemParser::getDeviceTypeRegister() const
	{
		assert(deviceTypeRegister_ != NULL, "DeviceTypeRegister not set!");
		return *deviceTypeRegister_;
	}

	const Core::XmlNode* ArrayLibraryItemParser::getArrayLibraryItemNode() const
	{
		return getDocumentNode().first_node(ARRAYLIBRARYITEM_NODE_.c_str());
	}

	ArrayLibraryItem& ArrayLibraryItemParser::getArrayLibraryItem()
	{
		assert(arrayLibraryItem_ != NULL);
		return *arrayLibraryItem_;
	}
}

