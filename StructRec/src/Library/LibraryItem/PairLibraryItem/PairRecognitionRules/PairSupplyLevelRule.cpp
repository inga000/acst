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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairSupplyLevelRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"

namespace StructRec {

	const int PairSupplyLevelRule::CHILDNUMBER_NOT_INITIALIZED_ = -1;

	PairSupplyLevelRule::PairSupplyLevelRule() :
			childNumberOfHigherLevel_(CHILDNUMBER_NOT_INITIALIZED_),
			childNumberOfLowerLevel_(CHILDNUMBER_NOT_INITIALIZED_)
	{
	}

	PairSupplyLevelRule::PairSupplyLevelRule(const PairSupplyLevelRule& other):
		pinTypeOfHigherLevel_(other.pinTypeOfHigherLevel_),
		childNumberOfHigherLevel_(other.childNumberOfHigherLevel_),
		pinTypeOfLowerLevel_(other.pinTypeOfLowerLevel_),
		childNumberOfLowerLevel_(other.childNumberOfLowerLevel_),
		typeOfSupply_(other.typeOfSupply_)
	{

	}

	PairSupplyLevelRule* PairSupplyLevelRule::clone()
	{
		return new PairSupplyLevelRule(*this);
	}

	PairSupplyLevelRule::~PairSupplyLevelRule()
	{
	}

	bool PairSupplyLevelRule::isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const
	{
		if(!other.isVoltageLevelRule()) {
			return false;
		}

		const PairSupplyLevelRule& otherRule = static_cast<const PairSupplyLevelRule&>(other);
		StructurePinType mappedHigherLevelPinType = mappings.findMappedPinType(otherRule.pinTypeOfHigherLevel_);
		StructurePinType mappedLowerLevelPinType = mappings.findMappedPinType(otherRule.pinTypeOfLowerLevel_);
		return (pinTypeOfHigherLevel_ == mappedHigherLevelPinType) && (pinTypeOfLowerLevel_ == mappedLowerLevelPinType) &&
				(childNumberOfHigherLevel_ == otherRule.childNumberOfHigherLevel_) && (childNumberOfLowerLevel_ == otherRule.childNumberOfLowerLevel_) &&
				(typeOfSupply_ == otherRule.typeOfSupply_);
	}


	void PairSupplyLevelRule::update(const LibraryItemMappings& mappings)
	{
		StructurePinType mappedPinTypeOfHigherLevel = mappings.findMappedPinType(getPinTypeOfHigherLevel());
		StructurePinType mappedPinTypeOfLowerLevel = mappings.findMappedPinType(getPinTypeOfLowerLevel());
		setPinTypeOfHigherLevel(mappedPinTypeOfHigherLevel);
		setPinTypeOfLowerLevel(mappedPinTypeOfLowerLevel);
	}

	void PairSupplyLevelRule::setPinTypeOfHigherLevel(const StructurePinType& pinType)
	{
		pinTypeOfHigherLevel_ = pinType;
	}

	void PairSupplyLevelRule::setChildNumberOfHigherLevel(int childNumber)
	{
		assert(childNumber == 1 || childNumber == 2);
		childNumberOfHigherLevel_ = childNumber;
	}


	void PairSupplyLevelRule::setPinTypeOfLowerLevel(const StructurePinType& pinType)
	{
		pinTypeOfLowerLevel_ = pinType;
	}

	void PairSupplyLevelRule::setChildNumberOfLowerLevel(int childNumber)
	{
		assert(childNumber == 1 || childNumber == 2);
		childNumberOfLowerLevel_ = childNumber;
	}


	bool PairSupplyLevelRule::evaluate(CrossInstancePair& pair) const
	{
		Core::Supply supplyHigherLevel;
		Core::Supply supplyLowerLevel;

		if(getChildNumberOfHigherLevel() == 1)
		{
			supplyHigherLevel = pair.getChild1().getStructure().findNet(getPinTypeOfHigherLevel()).getSupply();
			assert(getChildNumberOfLowerLevel() == 2, "Child numbers are equivalent");
			supplyLowerLevel = pair.getChild2().getStructure().findNet(getPinTypeOfLowerLevel()).getSupply();
		}
		else
		{
			supplyHigherLevel = pair.getChild2().getStructure().findNet(getPinTypeOfHigherLevel()).getSupply();
			assert(getChildNumberOfLowerLevel() == 1, "Child numbers are equivalent");
			supplyLowerLevel = pair.getChild1().getStructure().findNet(getPinTypeOfLowerLevel()).getSupply();
		}


			return getTypeOfSupply().isSameSupplyType(supplyHigherLevel) && supplyHigherLevel.isHigherToSameSupply(supplyLowerLevel);
	}

	bool PairSupplyLevelRule::evaluate(Pair& pair) const
	{
		Core::Supply supplyHigherLevel;
		Core::Supply supplyLowerLevel;

		if(getChildNumberOfHigherLevel() == 1)
		{
			supplyHigherLevel = pair.getChild1().findNet(getPinTypeOfHigherLevel()).getSupply();
			assert(getChildNumberOfLowerLevel() == 2, "Child numbers are equivalent");
			supplyLowerLevel = pair.getChild2().findNet(getPinTypeOfLowerLevel()).getSupply();
		}
		else
		{
			supplyHigherLevel = pair.getChild2().findNet(getPinTypeOfHigherLevel()).getSupply();
			assert(getChildNumberOfLowerLevel() == 1, "Child numbers are equivalent");
			supplyLowerLevel = pair.getChild1().findNet(getPinTypeOfLowerLevel()).getSupply();
		}

		if((getTypeOfSupply().isGnd() && supplyHigherLevel.isGnd()) || (getTypeOfSupply().isVdd() && supplyHigherLevel.isVdd()))
		{
			return supplyHigherLevel.isHigherToSameSupply(supplyLowerLevel);
		}
		else
		{
			return false;
		}
	}


	const int& PairSupplyLevelRule::getChildNumberOfHigherLevel() const
	{
		assert(hasChildNumberOfHigherLevel());
		return childNumberOfHigherLevel_;
	}

	const StructurePinType& PairSupplyLevelRule::getPinTypeOfHigherLevel() const
	{
		assert(hasPinTypeOfHigherLevel());
		return pinTypeOfHigherLevel_;
	}

	const int& PairSupplyLevelRule::getChildNumberOfLowerLevel() const
	{
		assert(hasChildNumberOfLowerLevel());
		return childNumberOfLowerLevel_;
	}

	const StructurePinType& PairSupplyLevelRule::getPinTypeOfLowerLevel() const
	{
		assert(hasPinTypeOfLowerLevel());
		return pinTypeOfLowerLevel_;
	}

	bool PairSupplyLevelRule::hasChildNumberOfHigherLevel() const
	{
		return childNumberOfHigherLevel_ != CHILDNUMBER_NOT_INITIALIZED_;
	}

	bool PairSupplyLevelRule::hasChildNumberOfLowerLevel() const
	{
		return childNumberOfLowerLevel_ != CHILDNUMBER_NOT_INITIALIZED_;
	}

	bool PairSupplyLevelRule::isNetRule() const
	{
		return false;
	}

	bool PairSupplyLevelRule::hasPinTypeOfHigherLevel() const
	{
		return pinTypeOfHigherLevel_.isInitialized();
	}

	void PairSupplyLevelRule::setTypeOfSupply(Core::Supply supply)
	{
		typeOfSupply_ = supply;
	}

	const Core::Supply PairSupplyLevelRule::getTypeOfSupply() const
	{
		assert(hasTypeOfSupply());
		return typeOfSupply_;
	}

	bool PairSupplyLevelRule::hasPinTypeOfLowerLevel() const
	{
		return pinTypeOfLowerLevel_.isInitialized();
	}

	bool PairSupplyLevelRule::isTechTypeRule() const
	{
		return false;
	}

	bool PairSupplyLevelRule::isVoltageLevelRule() const
	{
		return true;
	}

	bool PairSupplyLevelRule::isConnectionRule() const
	{
		return false;
	}

	void PairSupplyLevelRule::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode & supplyLevelRuleNode = Core::RapidXmlUtils::addNode(node, doc, "supplyLevelRule");

		Core::RapidXmlUtils::addSimpleNode(supplyLevelRuleNode, doc, "supplyType", getTypeOfSupply().supplyTypeToStr());

		Core::XmlNode & higherLevelNode = Core::RapidXmlUtils::addNode(supplyLevelRuleNode, doc, "higherLevel");
		Core::XmlNode & pinTypeOfHigherLevelNode = Core::RapidXmlUtils::addNode(higherLevelNode, doc, "structurePinType");
		Core::RapidXmlUtils::addSimpleNode(pinTypeOfHigherLevelNode, doc, "structureName", getPinTypeOfHigherLevel().getStructureName().toStr());
		Core::RapidXmlUtils::addSimpleNode(pinTypeOfHigherLevelNode, doc, "structurePinName", getPinTypeOfHigherLevel().getPinName().toStr());
		std::stringstream ss1;
		ss1 << getChildNumberOfHigherLevel();
		std::string number1 = ss1.str();
		Core::RapidXmlUtils::addSimpleNode(higherLevelNode, doc, "childNumber", number1);

		Core::XmlNode & lowerLevelNode = Core::RapidXmlUtils::addNode(supplyLevelRuleNode, doc, "lowerLevel");
		Core::XmlNode & pinTypeOfLowerLevelNode = Core::RapidXmlUtils::addNode(lowerLevelNode, doc, "structurePinType");
		Core::RapidXmlUtils::addSimpleNode(pinTypeOfLowerLevelNode, doc, "structureName", getPinTypeOfLowerLevel().getStructureName().toStr());
		Core::RapidXmlUtils::addSimpleNode(pinTypeOfLowerLevelNode, doc, "structurePinName", getPinTypeOfLowerLevel().getPinName().toStr());
		std::stringstream ss2;
		ss2 << getChildNumberOfLowerLevel();
		std::string number2 = ss2.str();
		Core::RapidXmlUtils::addSimpleNode(lowerLevelNode, doc, "childNumber", number2);

	}

	bool PairSupplyLevelRule::hasTypeOfSupply() const
	{
		return typeOfSupply_.isInitialized();
	}

}





