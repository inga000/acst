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

#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMapping.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	LibraryItemMapping::LibraryItemMapping()
	{
	}

	LibraryItemMapping::~LibraryItemMapping()
	{
	}

	void LibraryItemMapping::setMappedName(const StructureName& mappedName)
	{
		mappedName_ = mappedName;
	}

	void LibraryItemMapping::addPinMapping(const StructurePinType& pinType, const StructurePinType& mappedPinType)
	{
		pinTypeMapping_.insert(PinTypeMapping::value_type(pinType, mappedPinType));
	}

	StructureName LibraryItemMapping::getMappedName() const
	{
		assert(mappedName_.isInitialized(), "Mapped name not set!");
		return mappedName_;
	}

	StructurePinType LibraryItemMapping::findMappedPinType(const StructurePinType& pinType) const
	{
		if(hasMappedPinType(pinType)) {
			return pinTypeMapping_.find(pinType)->second;
		} else {
			return pinType;
		}
	}

	std::string LibraryItemMapping::toStr() const
	{
		std::ostringstream oss;
		oss << "MappedName: " << getMappedName() << "\n";
		oss << "PinTypeMapping: " << pinTypeMapping_ << "\n";
		return oss.str();
	}

	bool LibraryItemMapping::hasMappedPinType(const StructurePinType& pinType) const
	{
		return pinTypeMapping_.find(pinType) != pinTypeMapping_.end();
	}
}
