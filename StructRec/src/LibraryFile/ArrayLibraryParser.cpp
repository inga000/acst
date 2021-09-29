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

#include "StructRec/incl/LibraryFile/ArrayLibraryParser.h"
#include "StructRec/incl/LibraryFile/ArrayLibraryItemParser.h"
#include "StructRec/incl/Library/ArrayLibrary.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string ArrayLibraryParser::ARRAY_LIBRARY_NODE_ = "arrayLibrary";
	const std::string ArrayLibraryParser::ARRAYLIBRARYITEMFILES_NODE_ = "arrayLibraryItemFiles";
	const std::string ArrayLibraryParser::ARRAYLIBRARYITEMFILE_NODE_ = "arrayLibraryItemFile";

	const std::string ArrayLibraryParser::ARRAYLIBRARYITEMS_NODE_ = "arrayLibraryItems";
	const std::string ArrayLibraryParser::ARRAYLIBRARYITEM_NODE_ = "arrayLibraryItem";
	const std::string ArrayLibraryParser::HIERARCHYLEVEL_ATTRIBUTE_ = "level";

	const std::string ArrayLibraryParser::PERSISTENCE_ATTRIBUTE_ = "persistence";
	const int ArrayLibraryParser::MAX_PERSISTENCE_ = -1;

	ArrayLibraryParser::ArrayLibraryParser():
		deviceTypeRegister_(NULL),
		arrayLibrary_(NULL)
	{
	}

	ArrayLibraryParser::~ArrayLibraryParser()
	{

	}

	void ArrayLibraryParser::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
	{
		deviceTypeRegister_ = &deviceTypeRegister;
	}

	ArrayLibrary* ArrayLibraryParser::parse()
	{
		arrayLibrary_ = new ArrayLibrary;
		Control::XmlInputFile::parse();
		parseArrayLibraryItemFiles();
		parseArrayLibraryItems();
		return arrayLibrary_;
	}

	void ArrayLibraryParser::parseArrayLibraryItemFiles()
	{
		Core::XmlNode* arrayLibraryItemFilesNode = getArrayLibraryNode()->first_node(ARRAYLIBRARYITEMFILES_NODE_.c_str());
		Core::XmlNode* arrayLibraryItemFileNode = arrayLibraryItemFilesNode->first_node(ARRAYLIBRARYITEMFILE_NODE_.c_str());

		while(arrayLibraryItemFileNode != NULL) {
			std::string relativeFilePath = arrayLibraryItemFileNode->value();
			std::string absoluteFilePath = computeArrayLibraryItemFilePath(relativeFilePath);

			ArrayLibraryItemParser arrayLibraryItemParser;
			arrayLibraryItemParser.setPath(absoluteFilePath);
			arrayLibraryItemParser.setDeviceTypeRegister(getDeviceTypeRegister());
			ArrayLibraryItem* arrayLibraryItem = arrayLibraryItemParser.parse();
			arrayLibraryItem->setFilePath(relativeFilePath);
			arrayLibraryItemMap_.insert(ArrayLibraryItemMap::value_type(arrayLibraryItem->getName(), arrayLibraryItem));
			arrayLibraryItemFileNode = arrayLibraryItemFileNode->next_sibling(ARRAYLIBRARYITEMFILE_NODE_.c_str());
		}
	}

	void ArrayLibraryParser::parseArrayLibraryItems()
	{
		Core::XmlNode* arrayLibraryItemsNode = getArrayLibraryNode()->first_node(ARRAYLIBRARYITEMS_NODE_.c_str());
		int level = parseHierarchyLevel(*arrayLibraryItemsNode);

		Core::XmlNode* arrayLibraryItemNode = arrayLibraryItemsNode->first_node(ARRAYLIBRARYITEM_NODE_.c_str());
		while(arrayLibraryItemNode != NULL) {
			int persistence = parsePersistence(*arrayLibraryItemNode);
			StructureName structureName(arrayLibraryItemNode->value());

			ArrayLibraryItem* arrayLibraryItem = findArrayLibraryItem(structureName);
			arrayLibraryItem->setHierarchyLevel(level);
			arrayLibraryItem->setPersistence(persistence);
			getArrayLibrary().addItem(*arrayLibraryItem);

			arrayLibraryItemNode = arrayLibraryItemNode->next_sibling(ARRAYLIBRARYITEM_NODE_.c_str());
		}
	}

	int ArrayLibraryParser::parseHierarchyLevel(const Core::XmlNode& arrayLibraryItemNode) const
	{
		Core::XmlAttribute* levelAttribute = arrayLibraryItemNode.first_attribute(HIERARCHYLEVEL_ATTRIBUTE_.c_str());
		return std::atoi(levelAttribute->value());
	}

	int ArrayLibraryParser::parsePersistence(const Core::XmlNode& arrayLibraryItemNode) const
	{
		Core::XmlAttribute* persistenceAttribute = arrayLibraryItemNode.first_attribute(PERSISTENCE_ATTRIBUTE_.c_str());
		if(persistenceAttribute != NULL) {
			return std::atoi(persistenceAttribute->value());
		} else {
			return MAX_PERSISTENCE_;
		}
	}

	std::string ArrayLibraryParser::computeArrayLibraryItemFilePath(const std::string& relativePath) const
	{
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	bool ArrayLibraryParser::hasArrayLibraryItem(const StructureName& structureName) const
	{
		return arrayLibraryItemMap_.find(structureName) != arrayLibraryItemMap_.end();
	}

	ArrayLibraryItem* ArrayLibraryParser::findArrayLibraryItem(const StructureName& structureName) const
	{
		assert(hasArrayLibraryItem(structureName));
		return arrayLibraryItemMap_.find(structureName)->second;
	}

	const Core::DeviceTypeRegister& ArrayLibraryParser::getDeviceTypeRegister() const
	{
		assert(deviceTypeRegister_ != NULL, "DeviceTypeRegister not set!");
		return *deviceTypeRegister_;
	}

	const Core::XmlNode* ArrayLibraryParser::getArrayLibraryNode() const
	{
		return getDocumentNode().first_node(ARRAY_LIBRARY_NODE_.c_str());
	}

	const ArrayLibrary& ArrayLibraryParser::getArrayLibrary() const
	{
		assert(arrayLibrary_ != NULL);
		return *arrayLibrary_;
	}

	ArrayLibrary& ArrayLibraryParser::getArrayLibrary()
	{
		assert(arrayLibrary_ != NULL);
		return *arrayLibrary_;
	}
}




