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

#ifndef CORE_INCL_DEVICETYPEREGISTER_DEVICETYPESFILE_H_
#define CORE_INCL_DEVICETYPEREGISTER_DEVICETYPESFILE_H_

#include "Control/incl/File/XmlInputFile.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include <rapidxml.hpp>
#include <string>

namespace Core {

	class PinType;
	class DeviceType;
	class DeviceTypeRegister;

	class DeviceTypesFile: public Control::XmlInputFile
	{
	public:
		DeviceTypesFile();

		DeviceTypeRegister* parse();

	private:
		void parseDeviceTypes();
		void parseDeviceTypeName(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode);
		void parseTechTypes(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode);
		void parsePinTypes(DeviceType& deviceType, rapidxml::xml_node<>& deviceTypeNode);

		rapidxml::xml_node<>* getFirstDeviceTypeNode() const;
		rapidxml::xml_node<>* getFirstTechTypeNode(rapidxml::xml_node<>& deviceTypeNode) const;
		rapidxml::xml_node<>* getFirstPinTypeNode(rapidxml::xml_node<>& deviceTypeNode) const;

	private:
		bool parseOptionalAttribute(rapidxml::xml_node<>& pinTypeNode) const;
		void parseAutoConnection(PinType& pinType, rapidxml::xml_node<>& pinTypeNode) const;

		bool hasDeviceTypeRegister() const;
		DeviceTypeRegister& getDeviceTypeRegister();

	private:
		static const std::string DEVICETYPESNODE_;
		static const std::string DEVICETYPENODE_;
		static const std::string TECHTYPESNODE_;
		static const std::string TECHTYPENODE_;
		static const std::string PINTYPESNODE_;
		static const std::string PINTYPENODE_;

		static const std::string DEVICETYPENAMEATTRIBUTE_;
		static const std::string PINTYPEOPTIONALATTRIBUTE_;
		static const std::string PINTYPEAUTOCONNECTIONATTRIBUTE_;

		DeviceTypeRegister* deviceTypeRegister_;
	};
}

#endif /* CORE_INCL_DEVICETYPEREGISTER_DEVICETYPESFILE_H_ */
