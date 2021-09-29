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

#include "Partitioning/incl/Parts/PositiveFeedbackPart.h"
#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {


    PositiveFeedbackPart::PositiveFeedbackPart(int & num) :
            isSlewRateEnhancer_(false),
			isGainEnhancer_(false),
			stage_(nullptr)
    {
    	const PartName & name = PartName("PositiveFeedbackPart");
    	PartId id;
    	id.setName(name);
    	id.setId(num);
    	setPartId(id);
    	num++;
    }

    PositiveFeedbackPart::~PositiveFeedbackPart()
    {
    }

    void PositiveFeedbackPart::setIsGainEnhancer(bool isIt)
    {
       isGainEnhancer_ = isIt;
    }

    void PositiveFeedbackPart::setIsSlewRateEnhancer(bool isIt)
    {
       isSlewRateEnhancer_ = isIt;
    }

	void PositiveFeedbackPart::setStage(TransconductancePart & stage)
	{
		stage_ = &stage;
	}

	const TransconductancePart & PositiveFeedbackPart::getStage() const
	{
		assert(stage_ != nullptr);
		return *stage_;

	}

    bool PositiveFeedbackPart::PositiveFeedbackPart::isInitialized() const
    {
        return  hasMainStructures() && hasPartId();
    }

    bool PositiveFeedbackPart::isGainEnhancer() const
    {
        assert(isInitialized());
        return isGainEnhancer_;
    }

    bool PositiveFeedbackPart::isSlewRateEnhancer() const
    {
        assert(isInitialized());
        return isSlewRateEnhancer_;
    }


    void PositiveFeedbackPart::print(std::ostream & stream) const
    {
    	if(isSlewRateEnhancer_ || isGainEnhancer_)
    	{
    		stream << "Belongs to: ";
    		getStage().print(stream);
    		stream << std::endl;
    		stream << " Type: ";
    		if(isSlewRateEnhancer_)
    			stream << "SlewRateEnhancer, " << std::endl;
    		if(isGainEnhancer_)
    			stream << "GainEnhancer, " << std::endl;

    	}
    }

	void PositiveFeedbackPart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& positiveFeedbackPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "positiveFeedbackPart");
	   Part::writeXml(positiveFeedbackPart, doc);
	}

	bool PositiveFeedbackPart::hasGateNetsConnectedToBiasOfFoldedPair(const Result & result) const
	{
		if(result.hasBiasOfFoldedPair() && !result.getBelongingComponents(*this).empty() )
		{
			Component & compFL1B  = result.getBiasOfFoldedPair();
			if(compFL1B.getArray().getStructureName().toStr() == "MosfetNormalArray")
			{
				bool hasThat = true;
				const StructRec::StructureNet & gateNetFL1B = compFL1B.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Gate"));
				for(auto & comp : result.getBelongingComponents(*this))
				{
					const StructRec::StructureNet * gateNetFeedbackStruc = nullptr;
					if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
					{
						gateNetFeedbackStruc = &comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Gate"));
					}
					else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
					{
						gateNetFeedbackStruc =  &comp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray","Drain"));

					}
					assert(gateNetFeedbackStruc != nullptr, "The components in the positive feedback network should be either mosfet diode transistor or mosfet normal transistors");
					if(gateNetFL1B.getIdentifier() != gateNetFeedbackStruc->getIdentifier())
					{
						hasThat = false;
					}
				}
				return hasThat;
			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}

	}

	bool PositiveFeedbackPart::isPositiveFeedbackPart() const
	{
		return true;
	}

	bool PositiveFeedbackPart::isLoadPart() const
	{
		return false;
	}
	bool PositiveFeedbackPart::isBiasPart() const
	{
		return false;
	}

	bool PositiveFeedbackPart::isCapacitancePart() const
	{
		return false;
	}

	bool PositiveFeedbackPart::isTransconductancePart() const
	{
		return false;
	}

	bool PositiveFeedbackPart::isUndefinedPart() const
	{
		return false;
	}

	bool PositiveFeedbackPart::isResistorPart() const
	{
		return false;
	}

	bool PositiveFeedbackPart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

}


