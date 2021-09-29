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

#ifndef HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPINGFILE_H_
#define HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPINGFILE_H_

#include "Control/incl/File/XmlInputFile.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinName.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "HSpice/incl/InputFile/ModelName.h"
#include <map>

namespace HSpice {

	class DeviceLineMapper;
	class DeviceLineMapping;
	class DeviceLineMappingFile : public Control::XmlInputFile
	{
	public:

		DeviceLineMappingFile();
		~DeviceLineMappingFile();

		DeviceLineMapper* parse();

	private:
		void parseDeviceLineMappings();
		DeviceLineMapping* parseDeviceLineMapping(const rapidxml::xml_node<>& deviceLineMappingNode);
		char parseIdentifier(const rapidxml::xml_node<>& deviceLineMappingNode) const;
		Core::DeviceTypeName parseDeviceTypeName(const rapidxml::xml_node<>& deviceLineMappingNode) const;
		int parseDeviceIdentifierPosition(const rapidxml::xml_node<>& deviceLineMappingNode) const;

		void parsePins(const rapidxml::xml_node<>& deviceLineMappingNode, DeviceLineMapping& deviceLineMapping) const;
		void parsePin(const rapidxml::xml_node<>& pinNode, DeviceLineMapping& deviceLineMapping) const;
		Core::PinName parsePinName(const rapidxml::xml_node<>& pinNode) const;

		void parseModels(const rapidxml::xml_node<>& deviceLineMappingNode, DeviceLineMapping& deviceLineMapping) const;
		void parseModel(const rapidxml::xml_node<>& modelNode, DeviceLineMapping& deviceLineMapping) const;
		HSpice::InputFile::ModelName parseModelName(const rapidxml::xml_node<>& modelNode) const;
		Core::TechType parseTechType(const rapidxml::xml_node<>& modelNode) const;

		int parsePosition(const rapidxml::xml_node<>& node) const;

	private:
		DeviceLineMapper& getDeviceLineMapper();

	private:
		static const std::string DEVICELINEMAPPER_NODE_;
		static const std::string DEVICELINEMAPPING_NODE_;
		static const std::string DEVICETYPENAME_NODE_;
		static const std::string DEVICEIDENTIFIER_NODE_;

		static const std::string PINS_NODE_;
		static const std::string PIN_NODE_;
		static const std::string PINTYPE_ATTRIBUTE_;

		static const std::string MODELNAME_NODE_;
		static const std::string MODEL_NODE_;
		static const std::string NAME_ATTRIBUTE_;
		static const std::string TECHTYPE_ATTRIBUTE_;

		static const std::string IDENTIFIER_ATTRIBUTE_;
		static const std::string POSITION_ATTRIBUTE_;

	private:
		DeviceLineMapper* deviceLineMapper_;
	};
}



#endif /* HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPINGFILE_H_ */
