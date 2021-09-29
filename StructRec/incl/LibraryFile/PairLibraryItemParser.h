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

#ifndef STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYITEMPARSER_H_
#define STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYITEMPARSER_H_

#include "Control/incl/File/XmlInputFile.h"
#include "StructRec/incl/LibraryFile/ChildPinType.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinName.h"
#include "Core/incl/Circuit/Net/Supply.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml.hpp>

namespace StructRec {

	class PairLibraryItem;
	class PairConnectionRule;
	class CharacteristicConnection;
	class PairLibraryItemParser : public Control::XmlInputFile
	{
	public:
		PairLibraryItemParser();
		~PairLibraryItemParser();

		PairLibraryItem* parse();

	private:
		void parseStructureSymmetry();
		void parsePairConnection();
		void parseCharacteristicConnection();
		void parseRecognitionRules();

		bool parseHelperStructureNode(const Core::XmlNode& node) const;
		std::string parseImageFilePath(const Core::XmlNode& node) const;
		std::string computeImageFilePath(const std::string& relativePath) const;

		void parseFirstChildPinTypes(CharacteristicConnection& characteristicConnection, const Core::XmlNode& characteristicConnectionNode);
		void parseSecondChildPinTypes(CharacteristicConnection& characteristicConnection, const Core::XmlNode& characteristicConnectionNode);

		ChildPinType parseChildPinType(const Core::XmlNode& childPinTypeNode) const;
		StructurePinType parseStructurePinType(const Core::XmlNode& structurePinTypeNode) const;
		StructureName parseStructureName(const Core::XmlNode& node) const;
		StructurePinName parseStructurePinName(const Core::XmlNode& structurePinTypeNode) const;
		int parseChildNumber(const Core::XmlNode& childNumberNode) const;
		Core::Supply parseSupply(const Core::XmlNode& supplyNode) const;

		void parseTechTypeRule(const Core::XmlNode& recognitionRulesNode);
		void parseNetRules(const Core::XmlNode& recognitionRulesNode);
		void parseSupplyLevelRules(const Core::XmlNode& recognitionRulesNode);
		void parseConnectionRules(const Core::XmlNode& recognitionRulesNode);
		PairConnectionRule* parseConnectionRule(const Core::XmlNode& connectionRuleNode);
		bool parseConnectedNode(const Core::XmlNode& connectedNode);

	private:
		const Core::XmlNode* getPairLibraryItemNode() const;

		PairLibraryItem& getPairLibraryItem();

		bool hasHelperStructureNode(const Core::XmlNode& node) const;
		bool hasImageFilePathNode(const Core::XmlNode& node) const;

	private:

		static const std::string PAIRLIBRARYITEM_NODE_;
		static const std::string STRUCTURESYMMETRY_NODE_;
		static const std::string HELPER_STRUCTURE_NODE_;

		static const std::string PAIRCONNECTION_NODE_;
		static const std::string PAIRPINTYPE_NODE_;
		static const std::string STRUCTUREPINTYPE_NODE_;
		static const std::string STRUCTURENAME_NODE_;
		static const std::string STRUCTUREPINNAME_NODE_;

		static const std::string CHILDPINTYPE_NODE_;
		static const std::string CHILDNUMBER_NODE_;

		static const std::string CHARACTERISTICCONNECTION_NODE_;
		static const std::string FIRSTCHILDPINTYPE_NODE_;
		static const std::string SECONDCHILDPINTYPE_NODE_;

		static const std::string RECOGNITIONRULES_NODE_;
		static const std::string TECHTYPERULE_NODE_;
		static const std::string FIRSTCHILDTECHTYPE_NODE_;
		static const std::string SECONDCHILDTECHTYPE_NODE_;
		static const std::string NETRULES_NODE_;
		static const std::string NETRULE_NODE_;
		static const std::string SUPPLYLEVELRULES_NODE_;
		static const std::string SUPPLYLEVELRULE_NODE_;
		static const std::string SUPPLYTYPE_NODE_;
		static const std::string HIGHERLEVEL_NODE_;
		static const std::string LOWERLEVEL_NODE_;
		static const std::string CONNECTIONRULES_NODE_;
		static const std::string CONNECTIONRULE_NODE_;
		static const std::string IMAGEFILEPATH_NODE_ ;

		static const std::string CONNECTED_NODE_;
		static const std::string SUPPLY_NODE_;
		static const std::string TECHTYPE_ATTRIBUTE_;

		static const int DEFAULT_SUPPLY_LEVEL_;

	private:
		PairLibraryItem* pairLibraryItem_;
	};
}



#endif /* STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYITEMPARSER_H_ */
