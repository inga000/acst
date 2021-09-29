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

#ifndef STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYPARSER_H_
#define STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYPARSER_H_

#include "Control/incl/File/XmlInputFile.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml.hpp>

namespace StructRec {

	class PairLibrary;
	class PairLibraryItem;
	class HierarchyLevel;
	class DominanceRelation;
	class PairLibraryParser : public Control::XmlInputFile
	{
	private:
		typedef std::map<StructureName, PairLibraryItem*> PairLibraryItemMap;

	public:
		PairLibraryParser();
		~PairLibraryParser();

		PairLibrary* parse();

	private:
		void parsePairLibraryItemFiles();
		void parseHierarchyLevels();
		void parseDominanceRelations();

		void parseHierarchyLevel(const Core::XmlNode& hierarchyLevelNode);
		void parseDominanceRelation(const Core::XmlNode& dominanceRelationNode);
		void parseDominanceRelationForCurrentMirrors(const Core::XmlNode& dominanceRelationForCurrentMirrorsNode);


		std::string computePairLibraryItemFilePath(const std::string& relativeFilePath) const;
		int parseLevel(const Core::XmlNode& hierarchyLevelNode) const;
		int parsePersistence(const Core::XmlNode& pairLibraryIemNode) const;

		bool hasPairLibraryItem(const StructureName& structureName) const;
		PairLibraryItem* findPairLibraryItem(const StructureName& structureName) const;

	private:
		const Core::XmlNode* getPairLibraryNode() const;
		PairLibrary& getPairLibrary();

	private:
		static const std::string PAIR_LIBRARY_NODE_;

		static const std::string PAIRLIBRARYITEMFILES_NODE_;
		static const std::string PAIRLIBRARYITEMFILE_NODE_;
		static const std::string PAIRLIBRARYITEM_NODE_;

		static const std::string HIERARCHYLEVELS_NODE_;
		static const std::string HIERARCHYLEVEL_NODE_;
		static const std::string LEVEL_ATTRIBUTE_;
		static const std::string PERSISTENCE_ATTRIBUTE_;
		static const int MAX_PERSISTENCE_;

		static const std::string DOMINANCERELATIONS_NODE_;
		static const std::string DOMINANCERELATION_NODE_;
		static const std::string DOMINANCERELATIONFORCURRENTMIRRORS_NODE_;
		static const std::string DOMINATINGSTRUCTURE_NODE_;
		static const std::string DOMINATEDSTRUCTURE_NODE_;

	private:
		PairLibraryItemMap pairLibraryItemMap_;
		PairLibrary* pairLibrary_;
	};
}


#endif /* STRUCTREC_INCL_LIBRARYFILE_PAIRLIBRARYPARSER_H_ */
