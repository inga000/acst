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

#include "StructRec/incl/LibraryFile/PairLibraryParser.h"
#include "StructRec/incl/LibraryFile/PairLibraryItemParser.h"
#include "StructRec/incl/Library/PairLibrary.h"
#include "StructRec/incl/Library/HierarchyLevel.h"
#include "StructRec/incl/Library/DominanceRelation.h"
#include "StructRec/incl/Library/DominanceRelationForCurrentMirrors.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string PairLibraryParser::PAIR_LIBRARY_NODE_ = "pairLibrary";

	const std::string PairLibraryParser::PAIRLIBRARYITEMFILES_NODE_ = "pairLibraryItemFiles";
	const std::string PairLibraryParser::PAIRLIBRARYITEMFILE_NODE_ = "pairLibraryItemFile";

	const std::string PairLibraryParser::HIERARCHYLEVELS_NODE_ = "hierarchyLevels";
	const std::string PairLibraryParser::HIERARCHYLEVEL_NODE_ = "hierarchyLevel";
	const std::string PairLibraryParser::PAIRLIBRARYITEM_NODE_ = "pairLibraryItem";
	const std::string PairLibraryParser::LEVEL_ATTRIBUTE_ = "level";
	const std::string PairLibraryParser::PERSISTENCE_ATTRIBUTE_ = "persistence";
	const int PairLibraryParser::MAX_PERSISTENCE_ = -1;

	const std::string PairLibraryParser::DOMINANCERELATIONS_NODE_ = "dominanceRelations";
	const std::string PairLibraryParser::DOMINANCERELATION_NODE_ = "dominanceRelation";
	const std::string PairLibraryParser::DOMINANCERELATIONFORCURRENTMIRRORS_NODE_ = "dominanceRelationForCurrentMirrors";
	const std::string PairLibraryParser::DOMINATINGSTRUCTURE_NODE_ = "dominatingStructure";
	const std::string PairLibraryParser::DOMINATEDSTRUCTURE_NODE_ = "dominatedStructure";

	PairLibraryParser::PairLibraryParser():
			pairLibrary_(NULL)
	{
	}

	PairLibraryParser::~PairLibraryParser()
	{

	}

	PairLibrary* PairLibraryParser::parse()
	{
		pairLibrary_ = new PairLibrary;
		Control::XmlInputFile::parse();
		parsePairLibraryItemFiles();
		parseHierarchyLevels();
		parseDominanceRelations();
		return &getPairLibrary();
	}

	void PairLibraryParser::parsePairLibraryItemFiles()
	{
		Core::XmlNode* pairLibraryItemFilesNode = getPairLibraryNode()->first_node(PAIRLIBRARYITEMFILES_NODE_.c_str());
		Core::XmlNode* pairLibraryItemFileNode = pairLibraryItemFilesNode->first_node(PAIRLIBRARYITEMFILE_NODE_.c_str());
		while(pairLibraryItemFileNode != NULL) {
			std::string relativeFilePath = pairLibraryItemFileNode->value();
			std::string absoluteFilePath = computePairLibraryItemFilePath(relativeFilePath);

			PairLibraryItemParser pairLibraryItemParser;
			pairLibraryItemParser.setPath(absoluteFilePath);
			PairLibraryItem* pairLibraryItem = pairLibraryItemParser.parse();
			pairLibraryItem->setFilePath(relativeFilePath);
			pairLibraryItemMap_.insert(PairLibraryItemMap::value_type(pairLibraryItem->getName(), pairLibraryItem));

			pairLibraryItemFileNode = pairLibraryItemFileNode->next_sibling(PAIRLIBRARYITEMFILE_NODE_.c_str());
		}

	}

	void PairLibraryParser::parseHierarchyLevels()
	{
		Core::XmlNode* hierarchyLevelsNode = getPairLibraryNode()->first_node(HIERARCHYLEVELS_NODE_.c_str());
		Core::XmlNode* hierarchyLevelNode = hierarchyLevelsNode->first_node(HIERARCHYLEVEL_NODE_.c_str());
		while(hierarchyLevelNode != NULL) {
			parseHierarchyLevel(*hierarchyLevelNode);
			hierarchyLevelNode = hierarchyLevelNode->next_sibling(HIERARCHYLEVEL_NODE_.c_str());
		}
	}

	void PairLibraryParser::parseDominanceRelations()
	{
		Core::XmlNode* dominanceRelationsNode = getPairLibraryNode()->first_node(DOMINANCERELATIONS_NODE_.c_str());
		Core::XmlNode* dominanceRelationNode = dominanceRelationsNode->first_node(DOMINANCERELATION_NODE_.c_str());
		while(dominanceRelationNode != NULL) {
			parseDominanceRelation(*dominanceRelationNode);
			dominanceRelationNode = dominanceRelationNode->next_sibling(DOMINANCERELATION_NODE_.c_str());
		}
		Core::XmlNode* dominanceRelationForCurrentMirrorsNode = dominanceRelationsNode->first_node(DOMINANCERELATIONFORCURRENTMIRRORS_NODE_.c_str());
		while(dominanceRelationForCurrentMirrorsNode != NULL) {
			parseDominanceRelationForCurrentMirrors(*dominanceRelationForCurrentMirrorsNode);
			dominanceRelationForCurrentMirrorsNode = dominanceRelationForCurrentMirrorsNode->next_sibling(DOMINANCERELATIONFORCURRENTMIRRORS_NODE_.c_str());
		}
	}

	void PairLibraryParser::parseHierarchyLevel(const Core::XmlNode& hierarchyLevelNode)
	{
		int level = parseLevel(hierarchyLevelNode);

		HierarchyLevel* hierarchyLevel = new HierarchyLevel;
		hierarchyLevel->setLevel(level);

		Core::XmlNode* pairLibraryItemNode = hierarchyLevelNode.first_node(PAIRLIBRARYITEM_NODE_.c_str());
		while(pairLibraryItemNode != NULL) {
			int persistence = parsePersistence(*pairLibraryItemNode);
			StructureName structureName(pairLibraryItemNode->value());

			PairLibraryItem* pairLibraryItem = findPairLibraryItem(structureName);
			pairLibraryItem->setHierarchyLevel(level);
			pairLibraryItem->setPersistence(persistence);
			hierarchyLevel->addItem(*pairLibraryItem);

			pairLibraryItemNode = pairLibraryItemNode->next_sibling(PAIRLIBRARYITEM_NODE_.c_str());
		}

		getPairLibrary().addHierarchyLevel(level, *hierarchyLevel);
	}

	void PairLibraryParser::parseDominanceRelation(const Core::XmlNode& dominanceRelationNode)
	{
		DominanceRelation* dominanceRelation = new DominanceRelation;

		Core::XmlNode* dominatingStructureNode = dominanceRelationNode.first_node(DOMINATINGSTRUCTURE_NODE_.c_str());
		while(dominatingStructureNode != NULL) {
			StructureName dominatingStructure = StructureName(dominatingStructureNode->value());
			dominanceRelation->addDominatingStructure(dominatingStructure);
			dominatingStructureNode = dominatingStructureNode->next_sibling(DOMINATINGSTRUCTURE_NODE_.c_str());
		}

		Core::XmlNode* dominatedStructureNode = dominanceRelationNode.first_node(DOMINATEDSTRUCTURE_NODE_.c_str());
		while(dominatedStructureNode != NULL)
		{
			StructureName dominatedStructure = StructureName(dominatedStructureNode->value());
			dominanceRelation->addDominatedStructure(dominatedStructure);
			dominatedStructureNode = dominatedStructureNode->next_sibling(DOMINATEDSTRUCTURE_NODE_.c_str());
		}

		getPairLibrary().addDominanceRelation(*dominanceRelation);
	}

	void PairLibraryParser::parseDominanceRelationForCurrentMirrors(const Core::XmlNode& dominanceRelationForCurrentMirrorsNode)
	{
		DominanceRelationForCurrentMirrors* dominanceRelationForCurrentMirrors = new DominanceRelationForCurrentMirrors;

		Core::XmlNode* dominatingStructureNode = dominanceRelationForCurrentMirrorsNode.first_node(DOMINATINGSTRUCTURE_NODE_.c_str());
		while(dominatingStructureNode != NULL) {
			StructureName dominatingStructure = StructureName(dominatingStructureNode->value());
			dominanceRelationForCurrentMirrors->addDominatingStructure(dominatingStructure);
			dominatingStructureNode = dominatingStructureNode->next_sibling(DOMINATINGSTRUCTURE_NODE_.c_str());
		}

		Core::XmlNode* dominatedStructureNode = dominanceRelationForCurrentMirrorsNode.first_node(DOMINATEDSTRUCTURE_NODE_.c_str());
		while(dominatedStructureNode != NULL)
		{
			StructureName dominatedStructure = StructureName(dominatedStructureNode->value());
			dominanceRelationForCurrentMirrors->addDominatedStructure(dominatedStructure);
			dominatedStructureNode = dominatedStructureNode->next_sibling(DOMINATEDSTRUCTURE_NODE_.c_str());
		}

		getPairLibrary().addDominanceRelationForCurrentMirrors(*dominanceRelationForCurrentMirrors);
	}

	std::string PairLibraryParser::computePairLibraryItemFilePath(const std::string& relativeFilePath) const
	{
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativeFilePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	int PairLibraryParser::parseLevel(const Core::XmlNode& hierarchyLevelNode) const
	{
		Core::XmlAttribute* levelAttribute = hierarchyLevelNode.first_attribute(LEVEL_ATTRIBUTE_.c_str());
		return std::atoi(levelAttribute->value());
	}

	int PairLibraryParser::parsePersistence(const rapidxml::xml_node<>& pairLibraryIemNode) const
	{
		Core::XmlAttribute* persistenceAttribute = pairLibraryIemNode.first_attribute(PERSISTENCE_ATTRIBUTE_.c_str());
		if(persistenceAttribute != NULL) {
			return std::atoi(persistenceAttribute->value());
		} else {
			return MAX_PERSISTENCE_;
		}
	}

	bool PairLibraryParser::hasPairLibraryItem(const StructureName& structureName) const
	{
		return pairLibraryItemMap_.find(structureName) != pairLibraryItemMap_.end();
	}

	PairLibraryItem* PairLibraryParser::findPairLibraryItem(const StructureName& structureName) const
	{
		assert(hasPairLibraryItem(structureName), structureName.toStr());
		return pairLibraryItemMap_.find(structureName)->second;
	}

	const Core::XmlNode* PairLibraryParser::getPairLibraryNode() const
	{
		return getDocumentNode().first_node(PAIR_LIBRARY_NODE_.c_str());
	}

	PairLibrary& PairLibraryParser::getPairLibrary()
	{
		assert(pairLibrary_ != NULL);
		return *pairLibrary_;
	}
}



