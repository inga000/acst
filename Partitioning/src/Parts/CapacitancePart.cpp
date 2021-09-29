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

#include "Partitioning/incl/Parts/CapacitancePart.h"

namespace Partitioning {

	const std::string CapacitancePart::LOAD_STRING_ = "load";
    const std::string CapacitancePart::COMPENSATION_STRING_ = "compensation";

    CapacitancePart::CapacitancePart(int & num) :
            typeEnum_(TYPE_UNINITIALIZED)
    {
    	const PartName & name = PartName("CapacitancePart");
    	PartId id;
    	id.setName(name);
    	id.setId(num);
    	setPartId(id);
    	num++;
    }

    CapacitancePart::~CapacitancePart()
    {
    }

    void CapacitancePart::setType(const std::string string)
    {
       typeEnum_ = mapStringToEnum(string);
    }

    bool CapacitancePart::isValidString(const std::string & str)
    {
        return getStringToEnumMap().find(str) != getStringToEnumMap().end();
    }

    std::string CapacitancePart::getType() const
    {
        assert(isInitialized());
        return mapEnumToString(typeEnum_);
    }

    CapacitancePart::TypeEnum CapacitancePart::mapStringToEnum(const std::string & str)
    {
    	assert(isValidString(str));
        return getStringToEnumMap().at(str);
    }

    bool CapacitancePart::isInitialized() const
    {
        return typeEnum_ != TYPE_UNINITIALIZED && hasMainStructures() && hasPartId();
    }

    bool CapacitancePart::isLoadCapacitance() const
    {
        assert(isInitialized());
        return typeEnum_ == TYPE_LOAD;
    }

    bool CapacitancePart::isCompensationCapacitance() const
    {
        assert(isInitialized());
        return typeEnum_ == TYPE_COMPENSATION;
    }

    std::string CapacitancePart::mapEnumToString(const TypeEnum & tt)
    {
        return getEnumToStringMap().at(tt);
    }

    const CapacitancePart::StringToEnumMap & CapacitancePart::getStringToEnumMap()
    {
        static StringToEnumMap theMap;

        if(theMap.empty()) {
            theMap[LOAD_STRING_] = TYPE_LOAD;
            theMap[COMPENSATION_STRING_] = TYPE_COMPENSATION;
        }
        return theMap;
    }

    const CapacitancePart::EnumToStringMap & CapacitancePart::getEnumToStringMap()
    {
        static EnumToStringMap theMap;

        if(theMap.empty()) {
            theMap[TYPE_LOAD] = LOAD_STRING_;
            theMap[TYPE_COMPENSATION] = COMPENSATION_STRING_;
        }
        return theMap;
    }

    void CapacitancePart::print(std::ostream & stream) const
    {
    	stream << " Type: " << getType() << std::endl;

    }

	void CapacitancePart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& capacitanceNode = Core::RapidXmlUtils::addNode(xmlNode, doc, "capacitance");
	   Core::RapidXmlUtils::addAttr(capacitanceNode, doc, "type", getType());
	   Part::writeXml(capacitanceNode, doc);
	}

	bool CapacitancePart::isCapacitancePart() const
	{
		return true;
	}

	bool CapacitancePart::isLoadPart() const
	{
		return false;
	}

	bool CapacitancePart::isTransconductancePart() const
	{
		return false;
	}

	bool CapacitancePart::isBiasPart() const
	{
		return false;
	}

	bool CapacitancePart::isUndefinedPart() const
	{
		return false;
	}

	bool CapacitancePart::isResistorPart() const
	{
		return false;
	}

	bool CapacitancePart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool CapacitancePart::isPositiveFeedbackPart() const
	{
		return false;
	}
}


