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


#include "Partitioning/incl/Parts/PartId/PartId.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

	const int PartId::ID_NOT_INITIALIZED_ = -1;

	PartId::PartId() :
		id_(ID_NOT_INITIALIZED_)
	{
	}

	PartId::~PartId()
	{
	}

	void PartId::setName(const PartName& name)
	{
		name_ = name;
	}

	void PartId::setId(const int id)
	{
		id_ = id;
	}

	bool PartId::isInitialized() const
	{
		return name_.isInitialized() && hasId();
	}

	PartName PartId::getName() const
	{
		assert(hasName());
		return name_;
	}

	int PartId::getId() const
	{
		assert(hasId());
		return id_;
	}

    bool PartId::operator!=(const PartId& other) const
	{
    	return !operator==(other);
	}

	bool PartId::operator ==(const PartId& other) const
	{
		return hasEqualName(other) && hasEqualId(other);
	}

	std::string PartId::toStr() const
	{
		std::ostringstream stream;
		stream << getName() << "[" << getId() << "] ";
		return stream.str();
	}

	bool PartId::hasName() const
	{
		return name_.isInitialized();
	}

	bool PartId::hasId() const
	{
		return id_ != ID_NOT_INITIALIZED_;
	}

	bool PartId::hasEqualName(const PartId& other) const
	{
		return name_ == other.name_;
	}

	bool PartId::hasEqualId(const PartId& other) const
	{
		return id_ == other.id_;
	}

	bool PartId::operator <(const PartId& other) const
	{
		return (name_ < other.name_) || ((name_ == other.name_) && (id_ < other.id_));
	}

}


