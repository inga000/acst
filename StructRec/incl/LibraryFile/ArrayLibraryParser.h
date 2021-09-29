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

#ifndef STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYPARSER_H_
#define STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYPARSER_H_

#include "Control/incl/File/XmlInputFile.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "Core/incl/Common/RapidXml.h"
#include<rapidxml.hpp>
#include <string>

namespace Core {
	class DeviceTypeRegister;
}

namespace StructRec {

	class ArrayLibrary;
	class ArrayLibraryItem;
	class ArrayLibraryParser : public Control::XmlInputFile
	{
	private:
		typedef std::map<StructureName, ArrayLibraryItem*> ArrayLibraryItemMap;
	public:
		ArrayLibraryParser();
		~ArrayLibraryParser();

		void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);

		ArrayLibrary* parse();

	private:
		void parseArrayLibraryItemFiles();
		void parseArrayLibraryItems();

		int parseHierarchyLevel(const Core::XmlNode& arrayLibraryItemNode) const;
		int parsePersistence(const Core::XmlNode& arrayLibraryItemNode) const;

		std::string computeArrayLibraryItemFilePath(const std::string& relativePath) const;

		bool hasArrayLibraryItem(const StructureName& structureName) const;
		ArrayLibraryItem* findArrayLibraryItem(const StructureName& structureName) const;

	private:
		const Core::DeviceTypeRegister& getDeviceTypeRegister() const;
		const Core::XmlNode* getArrayLibraryNode() const;

		const ArrayLibrary& getArrayLibrary() const;
		ArrayLibrary& getArrayLibrary();
	private:
		static const std::string ARRAY_LIBRARY_NODE_;
		static const std::string ARRAYLIBRARYITEMFILES_NODE_;
		static const std::string ARRAYLIBRARYITEMFILE_NODE_;

		static const std::string ARRAYLIBRARYITEMS_NODE_;
		static const std::string ARRAYLIBRARYITEM_NODE_;
		static const std::string HIERARCHYLEVEL_ATTRIBUTE_;

		static const std::string PERSISTENCE_ATTRIBUTE_;
		static const int MAX_PERSISTENCE_;

	private:
		const Core::DeviceTypeRegister* deviceTypeRegister_;

		ArrayLibraryItemMap arrayLibraryItemMap_;
		ArrayLibrary* arrayLibrary_;
	};
}



#endif /* STRUCTREC_INCL_LIBRARYFILE_ARRAYLIBRARYPARSER_H_ */
