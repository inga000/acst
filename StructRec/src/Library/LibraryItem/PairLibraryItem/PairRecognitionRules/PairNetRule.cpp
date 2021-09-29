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
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairNetRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"

namespace StructRec {

	const int PairNetRule::CHILDNUMBER_NOT_INITIALIZED_ = -1;

	PairNetRule::PairNetRule() :
			childNumber_(CHILDNUMBER_NOT_INITIALIZED_)
	{
	}

	PairNetRule::PairNetRule(const PairNetRule& other):
		pinType_(other.pinType_),
		childNumber_(other.childNumber_),
		supply_(other.supply_)
	{

	}

	PairNetRule* PairNetRule::clone()
	{
		return new PairNetRule(*this);
	}
	PairNetRule::~PairNetRule()
	{
	}

	void PairNetRule::setPinType(const StructurePinType& pinType)
	{
		pinType_ = pinType;
	}

	bool PairNetRule::isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const
	{
		if(!other.isNetRule()) {
			return false;
		}

		const PairNetRule& otherRule = static_cast<const PairNetRule&>(other);
		StructurePinType mappedPinType = mappings.findMappedPinType(otherRule.pinType_);
		return (pinType_ == mappedPinType) && (childNumber_ == otherRule.childNumber_) && (supply_ == otherRule.supply_);
	}

	void PairNetRule::update(const LibraryItemMappings& mappings)
	{
		StructurePinType mappedPinType = mappings.findMappedPinType(getPinType());
		setPinType(mappedPinType);
	}

	void PairNetRule::setChildNumber(int childNumber)
	{
		assert(childNumber == 1 || childNumber == 2);
		childNumber_ = childNumber;
	}

	bool PairNetRule::evaluate(CrossInstancePair& pair) const
	{
		Core::Supply supply;

		if(getChildNumber() == 1)
		{
			if(pair.getChild1().hasTopCircuitConnection(getPinType()))
			{
				supply = pair.getChild1().findTopInstanceNet(getPinType()).getSupply();
			}
			else
			{
				supply = pair.getChild1().getStructure().findNet(getPinType()).getSupply();
			}

		}
		else
		{
			if(pair.getChild2().hasTopCircuitConnection(getPinType()))
			{
				supply = pair.getChild2().findTopInstanceNet(getPinType()).getSupply();
			}
			else
			{
				supply = pair.getChild2().getStructure().findNet(getPinType()).getSupply();
			}

		}

		return supply.getString() == getSupply().getString();

	}

	bool PairNetRule::evaluate(Pair& pair) const
	{
		Core::Supply supply;

		if(getChildNumber() == 1)
		{
			supply = pair.getChild1().findNet(getPinType()).getSupply();
		}
		else
		{
			supply = pair.getChild2().findNet(getPinType()).getSupply();
		}
		return supply.getString() == getSupply().getString();
	}


	const int& PairNetRule::getChildNumber() const
	{
		assert(hasChildNumber());
		return childNumber_;
	}

	const StructurePinType& PairNetRule::getPinType() const
	{
		assert(hasPinType());
		return pinType_;
	}

	bool PairNetRule::hasChildNumber() const
	{
		return childNumber_ != CHILDNUMBER_NOT_INITIALIZED_;
	}

	void PairNetRule::setSupply(const Core::Supply supply)
	{
		supply_ = supply;
	}

	bool PairNetRule::isNetRule() const
	{
		return true;
	}

	const Core::Supply PairNetRule::getSupply() const
	{
		assert(hasSupply());
		return supply_;
	}

	bool PairNetRule::hasPinType() const
	{
		return pinType_.isInitialized();
	}

	bool PairNetRule::isTechTypeRule() const
	{
		return false;
	}

	bool PairNetRule::isVoltageLevelRule() const
	{
		return false;
	}

	bool PairNetRule::isConnectionRule() const
	{
		return false;
	}

	void PairNetRule::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode & netRuleNode = Core::RapidXmlUtils::addNode(node, doc, "netRule");

		Core::XmlNode & pinTypeNode = Core::RapidXmlUtils::addNode(netRuleNode, doc, "structurePinType");
		Core::RapidXmlUtils::addSimpleNode(pinTypeNode, doc, "structureName", getPinType().getStructureName().toStr());
		Core::RapidXmlUtils::addSimpleNode(pinTypeNode, doc, "structurePinName", getPinType().getPinName().toStr());

		std::stringstream ss;
		ss << getChildNumber();
		std::string number = ss.str();
		Core::RapidXmlUtils::addSimpleNode(netRuleNode, doc, "childNumber", number);
		Core::RapidXmlUtils::addNode(netRuleNode, doc, "supply", getSupply().getString());
	}

	bool PairNetRule::hasSupply() const
	{
		return supply_.isInitialized();
	}

}



