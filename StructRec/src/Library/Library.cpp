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
/*
 * Library.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: ga76fuw
 */
#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/Results/Result.h"
#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/Library/ArrayLibrary.h"
#include "StructRec/incl/Library/PairLibrary.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	Library::Library():
			structureCore_(new StructureCore),
			arrayLibrary_(new ArrayLibrary()),
			pairLibrary_(new PairLibrary())
	{
		arrayLibrary_->setStructureCore(getStructureCore());
		pairLibrary_->setStructureCore(getStructureCore());
	}

	Library::~Library()
	{
		delete arrayLibrary_;
		delete pairLibrary_;
	}

	void Library::addAndMergeArrayLibrary(const ArrayLibrary& arrayLibrary)
	{
		addArrayLibraryFilePath(arrayLibrary.getFilePath());
		getArrayLibrary().addAndMerge(arrayLibrary, getLibraryItemMappings());
	}

	void Library::addAndMergePairLibrary(const PairLibrary& pairLibrary)
	{
		addPairLibraryFilePath(pairLibrary.getFilePath());
		getPairLibrary().addAndMerge(pairLibrary, getLibraryItemMappings());
	}

	std::vector<StructurePinType> Library::getPinTypes(const StructureName& structureName) const
	{
		if(getArrayLibrary().hasArrayLibraryItem(structureName))
		{
			return getArrayLibrary().getStructurePinTypes(structureName);
		}else
		{
			return getPairLibrary().getStructurePinTypes(structureName);
		}
	}

	Result* Library::recognize(const Core::Circuit & circuit)
	{
		Result* arrayResult = getArrayLibrary().recognize(circuit);
		getPairLibrary().recognize(*arrayResult);
		return arrayResult;
    }

	void Library::addArrayLibraryFilePath(const std::string& filePath)
	{
		arrayLibraryFilePaths_.push_back(filePath);
	}

	void Library::addPairLibraryFilePath(const std::string& filePath)
	{
		pairLibraryFilePaths_.push_back(filePath);
	}

	const StructureCore& Library::getStructureCore() const
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	StructureCore& Library::getStructureCore()
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	const ArrayLibrary& Library::getArrayLibrary() const
	{
		assert(arrayLibrary_ != NULL);
		return *arrayLibrary_;
	}

	ArrayLibrary& Library::getArrayLibrary()
	{
		assert(arrayLibrary_);
		return *arrayLibrary_;
	}

	const PairLibrary& Library::getPairLibrary() const
	{
		assert(pairLibrary_ != NULL);
		return *pairLibrary_;
	}

	PairLibrary& Library::getPairLibrary()
	{
		assert(pairLibrary_ != NULL);
		return *pairLibrary_;
	}

	const LibraryItemMappings& Library::getLibraryItemMappings() const
	{
		return libraryItemMappings_;
	}

	LibraryItemMappings& Library::getLibraryItemMappings()
	{
		return libraryItemMappings_;
	}

	std::vector<StructureName> Library::getStructureNames()
	{
		std::vector<StructureName> structureNames;
		std::vector<StructureName> arrayStructureNames = getArrayLibrary().getStructureNames();
		std::vector<StructureName> pairStructureNames = getPairLibrary().getStructureNames();
		structureNames.insert(structureNames.end(), arrayStructureNames.begin(), arrayStructureNames.end());
		structureNames.insert(structureNames.end(), pairStructureNames.begin(), pairStructureNames.end());
		return structureNames;
	}

	std::string Library::getImageFilePath(const StructRec::StructureName& structureName)
	{
		std::string imageFilePath;
		if(getArrayLibrary().hasArrayLibraryItem(structureName)) {
			imageFilePath = getArrayLibrary().getImageFilePath(structureName);
		} else {
			imageFilePath = getPairLibrary().getImageFilePath(structureName);
		}
		return imageFilePath;
	}

	void Library::writeXml(Control::OutputFile &file) const
	{
		file.open();

		Core::XmlDocument doc;
		Core::RapidXmlUtils::writeStyleDeclaration(doc);
		Core::XmlNode& root = Core::RapidXmlUtils::createXMLRootNode(doc, "library");

		writeXmlArrayLibrary(root,doc);
		writeXmlPairLibrary(root,doc);

		std::ostringstream oss;
		oss << doc;
		file << oss.str();

		file.close();
	}

	void Library::writeXmlArrayLibrary(Core::XmlNode & libraryNode, Core::XmlDocument & doc) const
	{
		Core::XmlNode & arrayLibrariesNode = Core::RapidXmlUtils::addNode(libraryNode,doc,"arrayLibraries");
		for(LibraryFilePathVector::const_iterator it = arrayLibraryFilePaths_.begin(); it != arrayLibraryFilePaths_.end(); it++)
		{
			std::string libraryFilePath = *it;
			Core::RapidXmlUtils::addSimpleNode(arrayLibrariesNode, doc, "arrayLibraryFile", libraryFilePath);
		}
	}

	void Library::writeXmlPairLibrary(Core::XmlNode & libraryNode, Core::XmlDocument & doc) const
	{
		Core::XmlNode & pairLibrariesNode = Core::RapidXmlUtils::addNode(libraryNode,doc,"pairLibraries");
		for(LibraryFilePathVector::const_iterator it = pairLibraryFilePaths_.begin(); it != pairLibraryFilePaths_.end(); it++)
		{
			std::string libraryFilePath = *it;
			Core::RapidXmlUtils::addSimpleNode(pairLibrariesNode, doc, "pairLibraryFile", libraryFilePath);
		}
	}
}


