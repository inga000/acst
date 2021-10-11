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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARY_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARY_H_

#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>
#include <map>

namespace Core {
	class Circuit;
}

namespace StructRec {

	class Result;
	class ArrayLibrary;
	class PairLibrary;
	class StructureCore;
	class Library
	{
	private:
		typedef std::vector<std::string> LibraryFilePathVector;
	public:
		Library();
		~Library();

		void addArrayLibraryFilePath(const std::string& filePath);
		void addPairLibraryFilePath(const std::string& filePath);

		void addAndMergeArrayLibrary(const ArrayLibrary& arrayLibrary);
		void addAndMergePairLibrary(const PairLibrary& pairLibrary);

		Result* recognize(const Core::Circuit & circuit);

		void writeXml(Control::OutputFile &file) const;

		//information required by the user interface
		std::string getImageFilePath(const StructRec::StructureName& structureName);
		std::vector<StructureName> getStructureNames();
		std::vector<StructurePinType> getPinTypes(const StructureName& structureName) const;


		const StructureCore& getStructureCore() const;
		StructureCore& getStructureCore();

		const ArrayLibrary& getArrayLibrary() const;
		ArrayLibrary& getArrayLibrary();

	private:
		void writeXmlArrayLibrary(Core::XmlNode & libraryNode, Core::XmlDocument & doc) const;
		void writeXmlPairLibrary(Core::XmlNode & libraryNode, Core::XmlDocument & doc) const;

	private:
		const PairLibrary& getPairLibrary() const;
		PairLibrary& getPairLibrary();

		const LibraryItemMappings& getLibraryItemMappings() const;
		LibraryItemMappings& getLibraryItemMappings();

	private:
		StructureCore* structureCore_;
		ArrayLibrary* arrayLibrary_;
		PairLibrary* pairLibrary_;

		LibraryItemMappings libraryItemMappings_;
		LibraryFilePathVector arrayLibraryFilePaths_;
		LibraryFilePathVector pairLibraryFilePaths_;
	};
}

#endif /* STRUCTREC_INCL_LIBRARY_LIBRARY_H_ */
