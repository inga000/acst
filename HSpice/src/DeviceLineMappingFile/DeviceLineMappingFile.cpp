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
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMappingFile.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapping.h"

namespace HSpice {

	const std::string DeviceLineMappingFile::DEVICELINEMAPPER_NODE_ = "deviceLineMapper";
	const std::string DeviceLineMappingFile::DEVICELINEMAPPING_NODE_ = "deviceLineMapping";
	const std::string DeviceLineMappingFile::DEVICETYPENAME_NODE_ = "deviceTypeName";
	const std::string DeviceLineMappingFile::DEVICEIDENTIFIER_NODE_ = "deviceIdentifier";

	const std::string DeviceLineMappingFile::PINS_NODE_ = "pins";
	const std::string DeviceLineMappingFile::PIN_NODE_ = "pin";
	const std::string DeviceLineMappingFile::PINTYPE_ATTRIBUTE_ = "pinType";

	const std::string DeviceLineMappingFile::MODELNAME_NODE_ = "modelName";
	const std::string DeviceLineMappingFile::MODEL_NODE_ = "model";
	const std::string DeviceLineMappingFile::NAME_ATTRIBUTE_ = "name";
	const std::string DeviceLineMappingFile::TECHTYPE_ATTRIBUTE_ = "techType";

	const std::string DeviceLineMappingFile::IDENTIFIER_ATTRIBUTE_ = "identifier";
	const std::string DeviceLineMappingFile::POSITION_ATTRIBUTE_ = "position";

	DeviceLineMappingFile::DeviceLineMappingFile():
			deviceLineMapper_(NULL)
	{
	}

	DeviceLineMappingFile::~DeviceLineMappingFile()
	{
	}

	DeviceLineMapper* DeviceLineMappingFile::parse()
	{
		XmlInputFile::parse();
		deviceLineMapper_ = new DeviceLineMapper();
		parseDeviceLineMappings();
		return &getDeviceLineMapper();
	}

	void DeviceLineMappingFile::parseDeviceLineMappings()
	{
		rapidxml::xml_node<>* deviceLineMapperNode = getDocumentNode().first_node(DEVICELINEMAPPER_NODE_.c_str());
		rapidxml::xml_node<>* deviceLineMappingNode = deviceLineMapperNode->first_node(DEVICELINEMAPPING_NODE_.c_str());
		while(deviceLineMappingNode != NULL)
		{
			DeviceLineMapping* deviceLineMapping = parseDeviceLineMapping(*deviceLineMappingNode);
			getDeviceLineMapper().add(*deviceLineMapping);
			deviceLineMappingNode = deviceLineMappingNode->next_sibling(DEVICELINEMAPPING_NODE_.c_str());
		}
	}

	DeviceLineMapping* DeviceLineMappingFile::parseDeviceLineMapping(const rapidxml::xml_node<>& deviceLineMappingNode)
	{
		DeviceLineMapping* deviceLineMapping = new DeviceLineMapping;

		char identifier = parseIdentifier(deviceLineMappingNode);
		deviceLineMapping->setId(identifier);

		Core::DeviceTypeName deviceTypeName = parseDeviceTypeName(deviceLineMappingNode);
		deviceLineMapping->setDeviceTypeName(deviceTypeName);

		int deviceIdPosition = parseDeviceIdentifierPosition(deviceLineMappingNode);
		deviceLineMapping->setDeviceIdPosition(deviceIdPosition);

		parsePins(deviceLineMappingNode, *deviceLineMapping);
		parseModels(deviceLineMappingNode, *deviceLineMapping);

		return deviceLineMapping;
	}

	char DeviceLineMappingFile::parseIdentifier(const rapidxml::xml_node<>& deviceLineMappingNode) const
	{
		rapidxml::xml_attribute<>* identifierAttribute = deviceLineMappingNode.first_attribute(IDENTIFIER_ATTRIBUTE_.c_str());
		std::string identifierString = identifierAttribute->value();
		assert(identifierString.size() == 1);
		return identifierString[0];
	}

	Core::DeviceTypeName DeviceLineMappingFile::parseDeviceTypeName(const rapidxml::xml_node<>& deviceLineMappingNode) const
	{
		rapidxml::xml_node<>* deviceTypeNameNode = deviceLineMappingNode.first_node(DEVICETYPENAME_NODE_.c_str());
		return Core::DeviceTypeName(deviceTypeNameNode->value());
	}

	int DeviceLineMappingFile::parseDeviceIdentifierPosition(const rapidxml::xml_node<>& deviceLineMappingNode) const
	{
		rapidxml::xml_node<>* deviceIdentifierNode = deviceLineMappingNode.first_node(DEVICEIDENTIFIER_NODE_.c_str());
		return parsePosition(*deviceIdentifierNode);
	}

	void DeviceLineMappingFile::parsePins(const rapidxml::xml_node<>& deviceLineMappingNode, DeviceLineMapping& deviceLineMapping) const
	{
		rapidxml::xml_node<>* pinsNode = deviceLineMappingNode.first_node(PINS_NODE_.c_str());
		rapidxml::xml_node<>* pinNode = pinsNode->first_node(PIN_NODE_.c_str());
		while(pinNode != NULL)
		{
			parsePin(*pinNode, deviceLineMapping);
			pinNode = pinNode->next_sibling(PIN_NODE_.c_str());
		}
	}

	void DeviceLineMappingFile::parsePin(const rapidxml::xml_node<>& pinNode, DeviceLineMapping& deviceLineMapping) const
	{
		Core::PinName pinName = parsePinName(pinNode);
		int position = parsePosition(pinNode);
		deviceLineMapping.addPin(pinName, position);
	}

	Core::PinName DeviceLineMappingFile::parsePinName(const rapidxml::xml_node<>& pinNode) const
	{
		rapidxml::xml_attribute<>* pinTypeAttribute = pinNode.first_attribute(PINTYPE_ATTRIBUTE_.c_str());
		return Core::PinName(pinTypeAttribute->value());
	}

	void DeviceLineMappingFile::parseModels(const rapidxml::xml_node<>& deviceLineMappingNode,	DeviceLineMapping& deviceLineMapping) const
	{
		rapidxml::xml_node<>* modelNameNode = deviceLineMappingNode.first_node(MODELNAME_NODE_.c_str());
		if(modelNameNode != NULL)
		{
			int modelNamePosition = parsePosition(*modelNameNode);
			deviceLineMapping.setModelNamePosition(modelNamePosition);

			rapidxml::xml_node<>* modelNode = modelNameNode->first_node(MODEL_NODE_.c_str());
			while(modelNode != NULL)
			{
				parseModel(*modelNode, deviceLineMapping);
				modelNode = modelNode->next_sibling(MODEL_NODE_.c_str());
			}
		}
	}

	void DeviceLineMappingFile::parseModel(const rapidxml::xml_node<>& modelNode, DeviceLineMapping& deviceLineMapping) const
	{
		HSpice::InputFile::ModelName modelName = parseModelName(modelNode);
		Core::TechType techType = parseTechType(modelNode);
		deviceLineMapping.addModel(modelName, techType);
	}

	HSpice::InputFile::ModelName DeviceLineMappingFile::parseModelName(const rapidxml::xml_node<>& modelNode) const
	{
		rapidxml::xml_attribute<>* modelNameNode = modelNode.first_attribute(NAME_ATTRIBUTE_.c_str());
		return HSpice::InputFile::ModelName(modelNameNode->value());
	}

	Core::TechType DeviceLineMappingFile::parseTechType(const rapidxml::xml_node<>& modelNode) const
	{
		rapidxml::xml_attribute<>* techTypeAttribute = modelNode.first_attribute(TECHTYPE_ATTRIBUTE_.c_str());
		return Core::TechType::createFromString(techTypeAttribute->value());
	}

	int DeviceLineMappingFile::parsePosition(const rapidxml::xml_node<>& node) const
	{
		rapidxml::xml_attribute<>* positionAttribute = node.first_attribute(POSITION_ATTRIBUTE_.c_str());
		return std::atoi(positionAttribute->value());
	}

	DeviceLineMapper& DeviceLineMappingFile::getDeviceLineMapper()
	{
		assert(deviceLineMapper_ != NULL);
		return *deviceLineMapper_;
	}

}
