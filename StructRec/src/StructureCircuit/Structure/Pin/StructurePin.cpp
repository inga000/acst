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
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructurePin::StructurePin() :
		structure_(NULL),
		net_(NULL)
	{
	}

	StructurePin::~StructurePin()
	{
	}

	void StructurePin::setPinType(StructurePinType name)
	{
		pinType_ = name;
	}

	StructurePinType StructurePin::getPinType() const
	{
		assert(hasPinType());
		return pinType_;
	}

	void StructurePin::setNet(StructureNet& net)
	{
		net_ = & net;
	}

	const StructureNet& StructurePin::getNet() const
	{
		assert(hasNet());
		return *net_;
	}

	StructureNet& StructurePin::getNet()
	{
		assert(hasNet());
		return *net_;
	}

	void StructurePin::setStructure(Structure& structure)
	{
		structure_ = & structure;
	}

	const Structure& StructurePin::getStructure() const
	{
		assert(hasStructure());
		return *structure_;
	}

	Structure& StructurePin::getStructure()
	{
		assert(hasStructure());
		return *structure_;
	}

	bool StructurePin::operator ==(const StructurePin& other) const
	{
		return ( hasEqualStructure(other) && hasEqualPinType(other) );
	}

	bool StructurePin::hasPinType() const
	{
		return pinType_.isInitialized();
	}

	bool StructurePin::hasStructure() const
	{
		return structure_ != NULL;
	}

	bool StructurePin::hasNet() const
	{
		return net_ != NULL;
	}

	bool StructurePin::hasEqualStructure(const StructurePin& other) const
	{
		return getStructure() == other.getStructure();
	}

	bool StructurePin::isConnected() const
	{
		return net_ != NULL;
	}

	bool StructurePin::hasEqualPinType(const StructurePin& other) const
	{
		return getPinType() == other.getPinType();
	}

	std::string StructurePin::toStr() const
    {
       std::ostringstream stream;
       stream << "Pin(" << getPinType() << "," << getStructure() << getNet() << ")";
       return stream.str();
    }

	bool StructurePin::isInitialized() const
	{
		return ( hasPinType() && hasNet() && hasStructure());
	}

	bool StructurePin::isPairPin() const
	{
		return false;
	}

	bool StructurePin::isArrayPin() const
	{
		return false;
	}
}
