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

#include "StructRec/incl/LibraryFile/LibraryFile.h"
#include "StructRec/incl/LibraryFile/ArrayLibraryParser.h"
#include "StructRec/incl/LibraryFile/PairLibraryParser.h"
#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/Library/ArrayLibrary.h"
#include "StructRec/incl/Library/PairLibrary.h"
#include "StructRec/incl/StructureCore.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Common/Backtrace.h"
#include "Log/incl/LogMacros.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string LibraryFile::LIBRARY_NODE_ = "library";
	const std::string LibraryFile::ARRAY_LIBRARIES_NODE_ = "arrayLibraries";
	const std::string LibraryFile::ARRAY_LIBRARY_FILE_NODE_ = "arrayLibraryFile";
	const std::string LibraryFile::PAIR_LIBRARIES_NODE_ = "pairLibraries";
	const std::string LibraryFile::PAIR_LIBRARY_FILE_NODE_ = "pairLibraryFile";

	LibraryFile::LibraryFile():
		deviceTypeRegister_(NULL),
		library_(NULL)
	{
	}

	LibraryFile::~LibraryFile()
	{

	}

	void LibraryFile::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
	{
		deviceTypeRegister_ = &deviceTypeRegister;
	}

	Library* LibraryFile::parse()
	{
		Control::XmlInputFile::parse();
		library_ = new Library();

		parseArrayLibraries();
		logDebug("Parsed ArrayLibrary successfully")
		parsePairLibraries();
		logDebug("Parsed PairLibrary successfully")
		return &getLibrary();
	}

	Library& LibraryFile::getLibrary()
	{
		assert(library_ != NULL);
		return *library_;
	}

	bool LibraryFile::hasArrayLibrariesNode() const
	{
		Core::XmlNode* libraryNode = getDocumentNode().first_node(LIBRARY_NODE_.c_str());
		Core::XmlNode* arrayLibrariesNode = libraryNode->first_node(ARRAY_LIBRARIES_NODE_.c_str());
		return arrayLibrariesNode != NULL;
	}

	Core::XmlNode* LibraryFile::getFirstArrayLibraryNode() const
	{
		Core::XmlNode* libraryNode = getDocumentNode().first_node(LIBRARY_NODE_.c_str());
		Core::XmlNode* arrayLibrariesNode = libraryNode->first_node(ARRAY_LIBRARIES_NODE_.c_str());
		return arrayLibrariesNode->first_node(ARRAY_LIBRARY_FILE_NODE_.c_str());
	}


	bool LibraryFile::hasPairLibrariesNode() const
	{
		Core::XmlNode* libraryNode = getDocumentNode().first_node(LIBRARY_NODE_.c_str());
		Core::XmlNode* pairlibrariesNode = libraryNode->first_node(PAIR_LIBRARIES_NODE_.c_str());
		return pairlibrariesNode != NULL;
	}

	Core::XmlNode* LibraryFile::getFirstPairLibraryNode() const
	{
		Core::XmlNode* libraryNode = getDocumentNode().first_node(LIBRARY_NODE_.c_str());
		Core::XmlNode* pairLibrariesNode = libraryNode->first_node(PAIR_LIBRARIES_NODE_.c_str());
		return pairLibrariesNode->first_node(PAIR_LIBRARY_FILE_NODE_.c_str());
	}

	void LibraryFile::parseArrayLibraries()
	{
		if(hasArrayLibrariesNode())
		{
			Core::XmlNode* arrayLibraryFileNode = getFirstArrayLibraryNode();
			while(arrayLibraryFileNode != NULL) {
				std::string relativeFilePath = arrayLibraryFileNode->value();
				std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);
				logDebug("Parsing: " << relativeFilePath);

				ArrayLibrary* arrayLibrary = parseArrayLibrary(absoluteFilePath);
				arrayLibrary->setFilePath(relativeFilePath);
				getLibrary().addAndMergeArrayLibrary(*arrayLibrary);
				delete arrayLibrary;

				arrayLibraryFileNode = arrayLibraryFileNode->next_sibling(ARRAY_LIBRARY_FILE_NODE_.c_str());
				logDebug("Parsed " << relativeFilePath << " successfully.");
			}
		}
	}

	void LibraryFile::parsePairLibraries()
	{
		if(hasPairLibrariesNode())
		{
			Core::XmlNode* pairLibraryFileNode = getFirstPairLibraryNode();
			while(pairLibraryFileNode != NULL) {
				std::string relativeFilePath = pairLibraryFileNode->value();
				std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);
				logDebug("Parsing: " << relativeFilePath);

				PairLibrary * pairLibrary = parsePairLibrary(absoluteFilePath);
				pairLibrary->setFilePath(relativeFilePath);
				getLibrary().addAndMergePairLibrary(*pairLibrary);
				delete pairLibrary;

				pairLibraryFileNode = pairLibraryFileNode->next_sibling(PAIR_LIBRARY_FILE_NODE_.c_str());
				logDebug("Parsed " << relativeFilePath << " successfully.");
			}
		}
	}


	ArrayLibrary* LibraryFile::parseArrayLibrary(const std::string& filePath) const
	{
		ArrayLibraryParser arrayLibraryParser;
		arrayLibraryParser.setPath(filePath);
		arrayLibraryParser.setDeviceTypeRegister(getDeviceTypeRegister());
		return arrayLibraryParser.parse();
	}

	PairLibrary* LibraryFile::parsePairLibrary(const std::string& filePath) const
	{
		PairLibraryParser pairLibraryParser;
		pairLibraryParser.setPath(filePath);
		return pairLibraryParser.parse();
	}

	std::string LibraryFile::computeAbsoluteFilePath(const std::string& relativePath) const
	{
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	const Library& LibraryFile::getLibrary() const
	{
		assert(library_ != NULL);
		return *library_;
	}

	const Core::DeviceTypeRegister& LibraryFile::getDeviceTypeRegister() const
	{
		assert(deviceTypeRegister_ != NULL, "DeviceTypeRegister not set!");
		return *deviceTypeRegister_;
	}



}

