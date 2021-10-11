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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairTechTypeRule.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const std::string PairTechTypeRule::SAME_TYPE_ = "same";
	const std::string PairTechTypeRule::DIFFERENT_TYPE_ = "different";
	const std::string PairTechTypeRule::NO_RULE_ = "noRule";

	PairTechTypeRule::PairTechTypeRule()
	{
	}

	PairTechTypeRule::PairTechTypeRule(const PairTechTypeRule& other):
		typeRule_(other.typeRule_),
		techTypeChild1_(other.techTypeChild1_),
		techTypeChild2_(other.techTypeChild2_)
	{

	}

	PairTechTypeRule* PairTechTypeRule::clone()
	{
		return new PairTechTypeRule(*this);
	}

	PairTechTypeRule::~PairTechTypeRule()
	{
	}

	bool PairTechTypeRule::isIdentical(const PairRecognitionRule& other, const LibraryItemMappings&) const
	{
		if(!other.isTechTypeRule()) {
			return false;
		}

		const PairTechTypeRule& otherRule = static_cast<const PairTechTypeRule&>(other);
		return (typeRule_ == otherRule.typeRule_);
	}

	void PairTechTypeRule::update(const LibraryItemMappings&)
	{
		//not implemented
	}

	void PairTechTypeRule::initialize(const Structure & child1, const Structure & child2)
	{
		if(child1.getTechType() == child2.getTechType())
		{
			setTechTypeRule(SAME_TYPE_);
		}
		else
		{
			setTechTypeRule(DIFFERENT_TYPE_);
			setTechTypeChild1(child1.getTechType());
			setTechTypeChild2(child2.getTechType());
		}
	}

	void PairTechTypeRule::setTechTypeRule(const std::string& typeRule)
	{
		assert(typeRule == SAME_TYPE_ || typeRule == DIFFERENT_TYPE_ || typeRule == NO_RULE_);
		typeRule_ = typeRule;
	}

	void PairTechTypeRule::setTechTypeChild1(Core::TechType techTypeChild1)
	{
		techTypeChild1_ = techTypeChild1;
	}

	void PairTechTypeRule::setTechTypeChild2(Core::TechType techTypeChild2)
	{
		techTypeChild2_ = techTypeChild2;
	}

	bool PairTechTypeRule::evaluate(CrossInstancePair & pair) const
	{
		bool ruleFulFilled = false;
		if(getTypeRule() == SAME_TYPE_) {
			ruleFulFilled = (pair.getChild1().getStructure().getTechType() == pair.getChild2().getStructure().getTechType());
		} else if(getTypeRule() == DIFFERENT_TYPE_) {
			if(hasTechTypeChild1())
			{
				ruleFulFilled = (pair.getChild1().getStructure().getTechType() == getTechTypeChild1() && pair.getChild2().getStructure().getTechType() == getTechTypeChild2());
			}
			else
			{
				ruleFulFilled = pair.getChild1().getStructure().getTechType() != pair.getChild2().getStructure().getTechType();
			}
		} else if(getTypeRule() == NO_RULE_) {
			ruleFulFilled = true;
		} else {
			assert(false, "Rule not known!");
		}
		return ruleFulFilled;
	}

	bool PairTechTypeRule::evaluate(Pair & pair) const
	{
		bool ruleFulFilled = false;
		if(getTypeRule() == SAME_TYPE_)
		{
			ruleFulFilled = (pair.getChild1().getTechType() == pair.getChild2().getTechType());
		}
		else if(getTypeRule() == DIFFERENT_TYPE_)
		{
			if(hasTechTypeChild1())
			{
				ruleFulFilled = (pair.getChild1().getTechType() == getTechTypeChild1() && pair.getChild2().getTechType() == getTechTypeChild2());
			}
			else
			{
				ruleFulFilled = pair.getChild1().getTechType() != pair.getChild2().getTechType();
			}
		}
		else if(getTypeRule() == NO_RULE_)
		{
				ruleFulFilled = true;
		}
		else
		{
			assert(false, "Rule not known!");
		}
		return ruleFulFilled;
	}

	std::string PairTechTypeRule::getTypeRule() const
	{
		assert(!typeRule_.empty(), "TechTypeRule not set!");
		return typeRule_;
	}

	void PairTechTypeRule::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode& techTypeNode = Core::RapidXmlUtils::addNode(node,doc, "techTypeRule");
		Core::RapidXmlUtils::addAttr(techTypeNode, doc, "attribute", getTypeRule());
		if(getTypeRule() == DIFFERENT_TYPE_)
		{
			Core::RapidXmlUtils::addSimpleNode(techTypeNode, doc, "firstChildTechType", getTechTypeChild1().toStr());
			Core::RapidXmlUtils::addSimpleNode(techTypeNode, doc, "secondChildTechType", getTechTypeChild2().toStr());
		}
	}

	bool PairTechTypeRule::isTechTypeRule() const
	{
		return true;
	}

	bool PairTechTypeRule::isVoltageLevelRule() const
	{
		return false;
	}

	bool PairTechTypeRule::isConnectionRule() const
	{
		return false;
	}

	bool PairTechTypeRule::isNetRule() const
	{
		return false;
	}

	Core::TechType PairTechTypeRule::getTechTypeChild1() const
	{
		assert(hasTechTypeChild1());
		return techTypeChild1_;

	}

	Core::TechType PairTechTypeRule::getTechTypeChild2() const
	{
		assert(hasTechTypeChild2());
		return techTypeChild2_;
	}

	bool PairTechTypeRule::hasTechTypeChild1() const
	{
		return techTypeChild1_.isInitialized();
	}

	bool PairTechTypeRule::hasTechTypeChild2() const
	{
		return techTypeChild2_.isInitialized();
	}

	bool PairTechTypeRule::operator==(PairTechTypeRule & other) const
	{
		bool isEqual = false;
		if(getTypeRule() == other.getTypeRule())
		{
			if(getTypeRule() == DIFFERENT_TYPE_)
			{
				if(getTechTypeChild1() == other.getTechTypeChild1() && getTechTypeChild2() == other.getTechTypeChild2())
				{
					isEqual = true;
				}
			}
			else
			{
				isEqual = true;
			}
		}
		return isEqual;
	}

}


