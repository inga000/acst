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

#include "Partitioning/incl/Parts/BiasPart.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

	const std::string BiasPart::VOLTAGEBIAS_STRING_ = "voltageBias";
    const std::string BiasPart::CURRENTBIAS_STRING_ = "currentBias";

    BiasPart::BiasPart(int & num) :
            typeEnum_(TYPE_UNINITIALIZED)
    {
    	const PartName & name = PartName("BiasPart");
    	PartId id;
    	id.setName(name);
    	id.setId(num);
    	setPartId(id);
    	num++;
    }

    BiasPart::~BiasPart()
    {
    }

    void BiasPart::setType(const std::string string)
    {
       typeEnum_ = mapStringToEnum(string);
    }

    bool BiasPart::isValidString(const std::string & str)
    {
        return getStringToEnumMap().find(str) != getStringToEnumMap().end();
    }

    std::string BiasPart::getType() const
    {
        assert(isInitialized());
        return mapEnumToString(typeEnum_);
    }

    BiasPart::TypeEnum BiasPart::mapStringToEnum(const std::string & str)
    {
        assert(isValidString(str));
        return getStringToEnumMap().at(str);
    }

    bool BiasPart::isInitialized() const
    {
        return typeEnum_ != TYPE_UNINITIALIZED && hasMainStructures() && hasPartId();
    }

    bool BiasPart::isVoltageBias() const
    {
        assert(isInitialized());
        return typeEnum_ == TYPE_VOLTAGEBIAS;
    }

    bool BiasPart::isCurrentBias() const
    {
        assert(isInitialized());
        return typeEnum_ == TYPE_CURRENTBIAS;
    }

    void BiasPart::addBiasedPart(Part &biasedPart)
    {
    	biasedParts_.push_back(&biasedPart);
    }

    std::vector<Part*>& BiasPart::getBiasedParts()
    {
    	assert(hasBiasedParts());
    	return biasedParts_;
    }

    bool BiasPart::hasBiasedParts() const
    {
    	return !biasedParts_.empty();
    }

    std::string BiasPart::mapEnumToString(const TypeEnum & tt)
    {
        return getEnumToStringMap().at(tt);
    }

    const BiasPart::StringToEnumMap & BiasPart::getStringToEnumMap()
    {
        static StringToEnumMap theMap;

        if(theMap.empty()) {
            theMap[VOLTAGEBIAS_STRING_] = TYPE_VOLTAGEBIAS;
            theMap[CURRENTBIAS_STRING_] = TYPE_CURRENTBIAS;
        }
        return theMap;
    }

    const BiasPart::EnumToStringMap & BiasPart::getEnumToStringMap()
    {
        static EnumToStringMap theMap;

        if(theMap.empty()) {
            theMap[TYPE_VOLTAGEBIAS] = VOLTAGEBIAS_STRING_;
            theMap[TYPE_CURRENTBIAS] = CURRENTBIAS_STRING_;
        }
        return theMap;
    }

    void BiasPart::print(std::ostream & stream) const
    {
    	stream << " Type: " << getType() << std::endl;
    	if(hasBiasedParts())
    	{
    		stream << " BiasedParts: " ;
    		for(std::vector<Part*>::const_iterator it = biasedParts_.begin(); it != biasedParts_.end(); it++)
    		{
    			Part* part = *it;
    			stream << part->getPartId().toStr() << " ";
    			part->printMainStructures(stream);
    			stream << ", ";
    		}
    		stream << std::endl;

    	}
    }

	void BiasPart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& biasPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "biasPart");
	   Part::writeXml(biasPart, doc);
	}

    bool BiasPart::hasBiasedPart(Part & part) const
    {
    	bool hasPart = false;
    	for(std::vector<Part*>::const_iterator it = biasedParts_.begin(); it != biasedParts_.end(); it++)
    	{
    		Part* biasedPart = *it;
    		if(biasedPart->getPartId() == part.getPartId())
    		{
    			hasPart = true;
    		}
    	}
    	return hasPart;
    }

	bool BiasPart::isBiasPart() const
	{
		return true;
	}

	bool BiasPart::isLoadPart() const
	{
		return false;
	}

	bool BiasPart::isCapacitancePart() const
	{
		return false;
	}

	bool BiasPart::isTransconductancePart() const
	{
		return false;
	}

	bool BiasPart::isUndefinedPart() const
	{
		return false;
	}

	bool BiasPart::isResistorPart() const
	{
		return false;
	}

	bool BiasPart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool BiasPart::isPositiveFeedbackPart() const
	{
		return false;
	}

}


