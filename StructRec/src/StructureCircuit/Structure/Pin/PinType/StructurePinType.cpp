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

#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructurePinType::StructurePinType()
	{
	}

	StructurePinType::StructurePinType(const std::string& structureName, const std::string& pinName)
	{
		setPinName(StructurePinName(pinName));
		setStructureName(StructureName(structureName));
	}

	StructurePinType::StructurePinType(StructureName structureName,
			StructurePinName pinName)
	{
		setPinName(pinName);
		setStructureName(structureName);
	}

	bool StructurePinType::isInitialized() const
	{
		return hasStructureName() && hasPinName();
	}

	bool StructurePinType::operator ==(
		const StructurePinType& other) const
	{
		return (structureName_ == other.structureName_) && (pinName_ == other.pinName_);
	}

	bool StructurePinType::operator !=(
		const StructurePinType& other) const
	{
		return !(*this == other);
	}

	bool StructurePinType::operator <(
		const StructurePinType& other) const
	{
		return (structureName_ < other.structureName_) || ((structureName_ == other.structureName_) && (pinName_ < other.pinName_));
	}

	StructurePinName StructurePinType::getPinName() const
	{
		assert(hasPinName());
		return pinName_;
	}

	StructureName StructurePinType::getStructureName() const
	{
		assert(hasStructureName());
		return structureName_;
	}

	std::string StructurePinType::toStr() const
	{
		return structureName_.toStr() + " " + pinName_.toStr();
	}

	bool StructurePinType::hasPinName() const
	{
		return pinName_.isInitialized();
	}

	bool StructurePinType::hasStructureName() const
	{
		return structureName_.isInitialized();
	}

	void StructurePinType::setPinName(StructurePinName pinName)
	{
		pinName_ = pinName;
	}

	void StructurePinType::setStructureName(StructureName structureName)
	{
		structureName_ = structureName;
	}

}
