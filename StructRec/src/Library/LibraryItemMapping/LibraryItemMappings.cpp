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

#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	LibraryItemMappings::LibraryItemMappings()
	{
	}

	LibraryItemMappings::~LibraryItemMappings()
	{
	}

	void LibraryItemMappings::addLibraryItemMapping(const StructureName& mappedName, const LibraryItemMapping& mapping)
	{
		assert(!hasMapping(mappedName), "Mapping already exists!");
		mappings_.insert(MappingMap::value_type(mappedName, mapping));
	}

	LibraryItemMapping LibraryItemMappings::findMapping(const StructureName& mappedName) const
	{
		assert(hasMapping(mappedName), "Mapping does not exist!");
		return mappings_.find(mappedName)->second;
	}

	StructureName LibraryItemMappings::findMappedName(const StructureName& structureName) const
	{
		if(hasMapping(structureName)) {
			return findMapping(structureName).getMappedName();
		} else {
			return structureName;
		}
	}

	StructurePinType LibraryItemMappings::findMappedPinType(const StructurePinType& structurePinType) const
	{
		StructureName structureName = structurePinType.getStructureName();
		if(hasMapping(structureName)) {
			return findMapping(structureName).findMappedPinType(structurePinType);
		} else {
			return structurePinType;
		}
	}

	bool LibraryItemMappings::hasMapping(const StructureName& mappedName) const
	{
		return mappings_.find(mappedName) != mappings_.end();
	}

	std::string LibraryItemMappings::toStr() const
	{
		std::ostringstream oss;
		oss << mappings_;
		return oss.str();
	}
}

