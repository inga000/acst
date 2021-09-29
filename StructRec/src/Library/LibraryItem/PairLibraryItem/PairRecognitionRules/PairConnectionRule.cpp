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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairConnectionRule.h"
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	PairConnectionRule::PairConnectionRule() :
			connected_(false)
	{
	}

	PairConnectionRule::~PairConnectionRule()
	{
	}

	PairConnectionRule::PairConnectionRule(const PairConnectionRule& other):
		pinType1_(other.pinType1_),
		pinType2_(other.pinType2_),
		connected_(other.connected_)
	{

	}

	PairConnectionRule* PairConnectionRule::clone()
	{
		return new PairConnectionRule(*this);
	}

	void PairConnectionRule::setPinType1(
		const StructurePinType& pinType1)
	{
		pinType1_ = pinType1;
	}

	bool PairConnectionRule::isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const
	{
		if(!other.isConnectionRule()) {
			return false;
		}

		const PairConnectionRule& otherRule = static_cast<const PairConnectionRule&>(other);

		StructurePinType mappedPinType1 = mappings.findMappedPinType(otherRule.pinType1_);
		StructurePinType mappedPinType2 = mappings.findMappedPinType(otherRule.pinType2_);
		return (pinType1_ == mappedPinType1) && (pinType2_ == mappedPinType2) && (connected_ == otherRule.connected_);
	}

	void PairConnectionRule::update(const LibraryItemMappings& mappings)
	{
		StructurePinType mappedPinType1 = mappings.findMappedPinType(getPinType1());
		StructurePinType mappedPinType2 = mappings.findMappedPinType(getPinType2());
		setPinType1(mappedPinType1);
		setPinType2(mappedPinType2);
	}

	void PairConnectionRule::setPinType2(
		const StructurePinType& pinType2)
	{
		pinType2_ = pinType2;
	}

	void PairConnectionRule::setConnected(bool connected)
	{
		connected_ = connected;
	}

	bool PairConnectionRule::evaluate(CrossInstancePair & pair) const
	{
		if(pair.getChild1().getStructure().isConnected(getPinType1()) && pair.getChild2().getStructure().isConnected(getPinType2()))

		{
			if(pair.getChild1().hasTopCircuitConnection(getPinType1()) && pair.getChild2().hasTopCircuitConnection(getPinType2()))
			{
				const StructureNet& net1 = pair.getChild1().findTopInstanceNet(getPinType1());
				const StructureNet& net2 = pair.getChild2().findTopInstanceNet(getPinType2());
				bool pinsConnected = (net1.getIdentifier() == net2.getIdentifier());
				return pinsConnected == connected_;
			}
			else if(!connected_  && pair.getChild1().getInstanceNamePath() != pair.getChild2().getInstanceNamePath())
			{
				return true;
			}
			return false;
		}

		return false;

	}

	bool PairConnectionRule::evaluate(Pair & pair) const
	{
		if(pair.getChild1().hasPin(getPinType1()))
		{
			const StructureNet& net1 = pair.getChild1().findNet(getPinType1());
			if(pair.getChild2().hasPin(getPinType2()))
			{
				const StructureNet& net2 = pair.getChild2().findNet(getPinType2());
				bool pinsConnected = (net1.getIdentifier() == net2.getIdentifier());
				return pinsConnected == connected_;
			}
				return false;
			}
			else
			{
				return false;
			}
		}

	bool PairConnectionRule::hasPinType1() const
	{
		return pinType1_.isInitialized();
	}

	bool PairConnectionRule::hasPinType2() const
	{
		return pinType2_.isInitialized();
	}

	const StructurePinType& PairConnectionRule::getPinType1() const
	{
		assert(hasPinType1());
		return pinType1_;
	}

	const StructurePinType& PairConnectionRule::getPinType2() const
	{
		assert(hasPinType2());
		return pinType2_;
	}

	bool PairConnectionRule::isTechTypeRule() const
	{
		return false;
	}

	bool PairConnectionRule::isNetRule() const
	{
		return false;
	}

	bool PairConnectionRule::isVoltageLevelRule() const
	{
		return false;
	}

	bool PairConnectionRule::isConnectionRule() const
	{
		return true;
	}

	void PairConnectionRule::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode& connectionRule = Core::RapidXmlUtils::addNode(node,doc, "connectionRule");

		std::string ss;
		if(connected_)
		{
			ss = "true";
		}
		else
		{
			ss = "false";
		}

		Core::RapidXmlUtils::addSimpleNode(connectionRule,doc, "connected", ss);

		Core::XmlNode& firstChildNode = Core::RapidXmlUtils::addNode(connectionRule,doc, "firstChildPinType");
		Core::RapidXmlUtils::addSimpleNode(firstChildNode, doc, "structureName", getPinType1().getStructureName().toStr());
		Core::RapidXmlUtils::addSimpleNode(firstChildNode, doc, "structurePinName", getPinType1().getPinName().toStr());

		Core::XmlNode& secondChildNode = Core::RapidXmlUtils::addNode(connectionRule,doc, "secondChildPinType");
		Core::RapidXmlUtils::addSimpleNode(secondChildNode, doc, "structureName",getPinType2().getStructureName().toStr());
		Core::RapidXmlUtils::addSimpleNode(secondChildNode, doc, "structurePinName", getPinType2().getPinName().toStr());

	}

}

