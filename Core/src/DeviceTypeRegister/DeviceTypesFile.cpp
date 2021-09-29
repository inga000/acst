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
#include "Core/incl/DeviceTypeRegister/DeviceTypesFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinName.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

	const std::string DeviceTypesFile::DEVICETYPESNODE_ = "deviceTypes";
	const std::string DeviceTypesFile::DEVICETYPENODE_ = "deviceType";
	const std::string DeviceTypesFile::TECHTYPESNODE_ = "techTypes";
	const std::string DeviceTypesFile::TECHTYPENODE_ = "techType";
	const std::string DeviceTypesFile::PINTYPESNODE_ = "pinTypes";
	const std::string DeviceTypesFile::PINTYPENODE_ = "pinType";

	const std::string DeviceTypesFile::DEVICETYPENAMEATTRIBUTE_ = "name";
	const std::string DeviceTypesFile::PINTYPEOPTIONALATTRIBUTE_ = "optional";
	const std::string DeviceTypesFile::PINTYPEAUTOCONNECTIONATTRIBUTE_ = "autoConnection";

	DeviceTypesFile::DeviceTypesFile():
			deviceTypeRegister_(NULL)
	{
	}

	DeviceTypeRegister* DeviceTypesFile::parse()
	{
		try{
		Control::XmlInputFile::parse();
		deviceTypeRegister_ = new DeviceTypeRegister();
		parseDeviceTypes();
		}
		catch(const ::Core::BacktraceException& ex)
		{
			logDebug(ex.what());
			throw;
		}
		catch(...){
			logDebug("Error!");
			throw;
		}
		return deviceTypeRegister_;
	}

	void DeviceTypesFile::parseDeviceTypes()
	{
		rapidxml::xml_node<>* deviceTypeNode = getFirstDeviceTypeNode();
		while(deviceTypeNode != NULL)
		{
			DeviceType* deviceType = new DeviceType();

			parseDeviceTypeName(*deviceType, *deviceTypeNode);
			parseTechTypes(*deviceType, *deviceTypeNode);
			parsePinTypes(*deviceType,*deviceTypeNode);

			getDeviceTypeRegister().addDeviceType(*deviceType);
			deviceTypeNode = deviceTypeNode->next_sibling(DEVICETYPENODE_.c_str());
		}
	}

	void DeviceTypesFile::parseDeviceTypeName(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode)
	{
		rapidxml::xml_attribute<>* deviceTypeNameAttribute = deviceTypeNode.first_attribute(DEVICETYPENAMEATTRIBUTE_.c_str());
		Core::DeviceTypeName deviceTypeName(deviceTypeNameAttribute->value());
		deviceType.setName(deviceTypeName);
	}

	void DeviceTypesFile::parseTechTypes(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode)
	{
		rapidxml::xml_node<>* techTypeNode = getFirstTechTypeNode(deviceTypeNode);
		while(techTypeNode != NULL)
		{
			std::string techTypeString = techTypeNode->value();
			TechType techType = Core::TechType::createFromString(techTypeString);
			deviceType.addTechType(techType);
			techTypeNode = techTypeNode->next_sibling(TECHTYPENODE_.c_str());
		}
	}

	void DeviceTypesFile::parsePinTypes(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode)
	{
		rapidxml::xml_node<>* pinTypeNode = getFirstPinTypeNode(deviceTypeNode);
		while(pinTypeNode != NULL)
		{
			DeviceTypeName deviceTypeName = deviceType.getName();
			PinName pinName = Core::PinName(pinTypeNode->value());
			PinType pinType;
			pinType.setDeviceTypeName(deviceTypeName);
			pinType.setPinName(pinName);

			bool optional = parseOptionalAttribute(*pinTypeNode);
			pinType.setOptional(optional);

			if(optional) {
				parseAutoConnection(pinType, *pinTypeNode);
			}

			deviceType.addPinType(pinType);
			pinTypeNode = pinTypeNode->next_sibling(PINTYPENODE_.c_str());
		}
	}

	bool DeviceTypesFile::parseOptionalAttribute(rapidxml::xml_node<>& pinTypeNode) const
	{
		bool optional = false;
		rapidxml::xml_attribute<>* pinTypeOptionalAttribute = pinTypeNode.first_attribute(PINTYPEOPTIONALATTRIBUTE_.c_str());
		if(pinTypeOptionalAttribute != NULL)
		{
			std::string optionalString = pinTypeOptionalAttribute->value();
			if(optionalString == "true") {
				optional = true;
			}
		}
		return optional;
	}

	void DeviceTypesFile::parseAutoConnection(PinType& pinType, rapidxml::xml_node<>& pinTypeNode) const
	{
		rapidxml::xml_attribute<>* pinTypeAutoConnectionAttribute = pinTypeNode.first_attribute(PINTYPEAUTOCONNECTIONATTRIBUTE_.c_str());
		if(pinTypeAutoConnectionAttribute != NULL) {
			Core::PinName autoConnectionPinName = Core::PinName(pinTypeAutoConnectionAttribute->value());
			pinType.setAutoConnection(autoConnectionPinName);
		}
	}

	rapidxml::xml_node<>* DeviceTypesFile::getFirstDeviceTypeNode() const
	{
		rapidxml::xml_node<>* deviceTypesNode = getDocumentNode().first_node(DEVICETYPESNODE_.c_str());
		assert(deviceTypesNode != NULL, "Unsupported File Format!");
		return deviceTypesNode->first_node(DEVICETYPENODE_.c_str());
	}

	rapidxml::xml_node<>* DeviceTypesFile::getFirstTechTypeNode(rapidxml::xml_node<>& deviceTypeNode) const
	{
		rapidxml::xml_node<>* techTypesNode = deviceTypeNode.first_node(TECHTYPESNODE_.c_str());
		return techTypesNode->first_node(TECHTYPENODE_.c_str());
	}

	rapidxml::xml_node<>* DeviceTypesFile::getFirstPinTypeNode(rapidxml::xml_node<>& deviceTypeNode) const
	{
		rapidxml::xml_node<>* pinTypesNode = deviceTypeNode.first_node(PINTYPESNODE_.c_str());
		return pinTypesNode->first_node(PINTYPENODE_.c_str());
	}

	bool Core::DeviceTypesFile::hasDeviceTypeRegister() const
	{
		return deviceTypeRegister_ != NULL;
	}

	DeviceTypeRegister& Core::DeviceTypesFile::getDeviceTypeRegister()
	{
		assert(hasDeviceTypeRegister());
		return *deviceTypeRegister_;
	}
}
