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

#ifndef STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYITEMPARSER_H_
#define STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYITEMPARSER_H_

#include "Control/incl/File/XmlInputFile.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinName.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinName.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Common/RapidXml.h"
#include <string>

namespace Core {
	class DeviceTypeRegister;
}

namespace StructRec {

	class ArrayLibraryItem;
	class ArrayConnectionRule;
	class ArrayLibraryItemParser: public Control::XmlInputFile
	{
	public:
		ArrayLibraryItemParser();

		void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);

		ArrayLibraryItem* parse();

	private:
		void parseArrayConnection();
		void parseRecognitionRules();
		void parseParallelNets();

		std::string parseImageFilePath(const Core::XmlNode& node) const;

		void parseDeviceTypeRule(const Core::XmlNode& primaryRulesNode);
		void parseConnectionRules(const Core::XmlNode& primaryRulesNode);
		ArrayConnectionRule* parseConnectionRule(const Core::XmlNode& connectionRuleNode) const;
		bool parseConnectedNode(const Core::XmlNode& connectedNode) const;

		StructurePinType parseStructurePinType(const Core::XmlNode& structurePinTypeNode) const;
		StructureName parseStructureName(const Core::XmlNode& node) const;
		StructurePinName parseStructurePinName(const Core::XmlNode& structurePinTypeNode) const;

		Core::PinType parseDevicePinType(const Core::XmlNode& devicePinTypeNode) const;
		Core::DeviceTypeName parseDeviceTypeName(const Core::XmlNode& devicePinTypeNode) const;
		Core::PinName parseDevicePinName(const Core::XmlNode& devicePinTypeNode) const;

	private:
		const Core::XmlNode* getArrayLibraryItemNode() const;

		const Core::DeviceTypeRegister& getDeviceTypeRegister() const;
		ArrayLibraryItem& getArrayLibraryItem();

	private:
		static const std::string ARRAYLIBRARYITEM_NODE_;

		static const std::string ARRAYCONNECTION_NODE_;
		static const std::string CONNECTION_NODE_;

		static const std::string STRUCTUREPINTYPE_NODE_;
		static const std::string DEVICEPINTYPE_NODE_;

		static const std::string STRUCTURENAME_NODE_;
		static const std::string STRUCTUREPINNAME_NODE_;

		static const std::string DEVICETYPENAME_NODE_;
		static const std::string DEVICEPINNAME_NODE_;

		static const std::string RECOGNITIONRULES_NODE_;

		static const std::string DEVICETYPERULE_NODE_;
		static const std::string CONNECTIONRULES_NODE_;
		static const std::string CONNECTIONRULE_NODE_;
		static const std::string CONNECTED_NODE_;

		static const std::string PARALLELNETS_NODE_;
		static const std::string IMAGEFILEPATH_NODE_;

	private:
		const Core::DeviceTypeRegister* deviceTypeRegister_;

		ArrayLibraryItem* arrayLibraryItem_;
	};
}



#endif /* STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYITEMPARSER_H_ */
