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



#include "AutomaticSizing/incl/ConstraintProgram/Graph/EdgeKey.h"

namespace AutomaticSizing {

	EdgeKey::EdgeKey() :
		component_(NULL)
	{
	}

	EdgeKey::~EdgeKey()
	{
	}

	void EdgeKey::setStartPinType(
			const StructRec::StructurePinType& startPinType)
	{
		startPinType_ = startPinType;
	}

	void EdgeKey::setEndPinType(
			const StructRec::StructurePinType& endPinType)
	{
		endPinType_ = endPinType;
	}

	void EdgeKey::setComponent(Partitioning::Component& component)
	{
		component_ = &component;
	}

	Core::NetId EdgeKey::getStartNodeId() const
	{
		return  getComponent().getArray().findNet(getStartPinType()).getIdentifier();
	}

	Core::NetId EdgeKey::getEndNodeId() const
	{
		return getComponent().getArray().findNet(getEndPinType()).getIdentifier();
	}

	StructRec::StructureId EdgeKey::getStructureId() const
	{
		return getComponent().getArray().getIdentifier();
	}

	bool EdgeKey::isInitialized() const
	{
		return startPinType_.isInitialized() && endPinType_.isInitialized() && component_ != NULL;
	}

	const StructRec::StructurePinType& EdgeKey::getStartPinType() const
	{
		assert(startPinType_.isInitialized());
		return startPinType_;
	}

	const StructRec::StructurePinType& EdgeKey::getEndPinType() const
	{
		assert(endPinType_.isInitialized());
		return endPinType_;
	}

	const Partitioning::Component& EdgeKey::getComponent() const
	{
		assert(component_ != NULL);
		return * component_;
	}

	bool EdgeKey::operator<(const EdgeKey& other) const
	{
		if(getComponent().getArray() == other.getComponent().getArray())
		{
			if(getStartPinType() == other.getStartPinType())
			{
				return getEndPinType() < other.getEndPinType();
			} else
			{
				return getStartPinType() < other.getStartPinType();
			}
		}
		else
		{
			return getComponent().getArray() < other.getComponent().getArray();
		}
	}

	bool EdgeKey::operator==(const EdgeKey & other) const
	{
		if(getComponent().getArray() == other.getComponent().getArray() &&
				getStartPinType() == other.getStartPinType() && getEndPinType() == other.getEndPinType())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool EdgeKey::isDrainSourceEdge() const
	{
		if(getStartPinType().getPinName() == StructRec::StructurePinName("Drain")
			&& getEndPinType().getPinName() == StructRec::StructurePinName("Source"))
			return true;
		else
			return false;
	}

	bool EdgeKey::isCapacitanceEdge() const
	{
		if(getComponent().getArray().getStructureName() == StructRec::StructureName("CapacitorArray"))
			return true;
		else
			return false;
	}

	bool EdgeKey::isGateSourceEdge() const
	{
		if(getStartPinType().getPinName() == StructRec::StructurePinName("Gate")
			&& getEndPinType().getPinName() == StructRec::StructurePinName("Source"))
			return true;
		else
			return false;
	}

	bool EdgeKey::isGateDrainEdge() const
	{
		if(getStartPinType().getPinName() == StructRec::StructurePinName("Drain")
					&& getEndPinType().getPinName() == StructRec::StructurePinName("Source"))
			return true;
		else
			return false;
	}





}

