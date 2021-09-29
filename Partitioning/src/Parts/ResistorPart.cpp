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


#include "Partitioning/incl/Parts/ResistorPart.h"
#include "Partitioning/incl/Parts/CapacitancePart.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

const std::string ResistorPart::COMPENSATION_STRING_ = "compensation";

    ResistorPart::ResistorPart(int & num) :
		    typeEnum_(TYPE_UNINITIALIZED),
			compensationCapacitor_(nullptr)
    {
    	const PartName & name = PartName("ResistorPart");
    	PartId id;
    	id.setName(name);
    	id.setId(num);
    	setPartId(id);
    	num++;
    }

    void ResistorPart::setType(const std::string string)
    {
       typeEnum_ = mapStringToEnum(string);
    }

    ResistorPart::~ResistorPart()
    {
    }

    bool ResistorPart::isInitialized() const
    {
        return typeEnum_ != TYPE_UNINITIALIZED && hasMainStructures() && hasPartId();
    }

    bool ResistorPart::isValidString(const std::string & str)
    {
        return getStringToEnumMap().find(str) != getStringToEnumMap().end();
    }

    std::string ResistorPart::getType() const
    {
        assert(isInitialized());
        return mapEnumToString(typeEnum_);
    }

    ResistorPart::TypeEnum ResistorPart::mapStringToEnum(const std::string & str)
    {
        assert(isValidString(str));
        return getStringToEnumMap().at(str);
    }

    bool ResistorPart::isCompensationResistor() const
    {
        assert(isInitialized());
        return typeEnum_ == TYPE_COMPENSATION;
    }

    void ResistorPart::setCompensationCapacitor(CapacitancePart &capPart)
    {
    	compensationCapacitor_ = & capPart;
    }

    CapacitancePart& ResistorPart::getCompensationCapacitor() const
    {
    	assert(hasCompensationCapacitor());
    	return *compensationCapacitor_;
    }

    bool ResistorPart::hasCompensationCapacitor() const
    {
    	return compensationCapacitor_ != nullptr;
    }

    std::string ResistorPart::mapEnumToString(const TypeEnum & tt)
    {
        return getEnumToStringMap().at(tt);
    }

    const ResistorPart::StringToEnumMap & ResistorPart::getStringToEnumMap()
    {
        static StringToEnumMap theMap;

        if(theMap.empty()) {
            theMap[COMPENSATION_STRING_] = TYPE_COMPENSATION;
        }
        return theMap;
    }

    const ResistorPart::EnumToStringMap & ResistorPart::getEnumToStringMap()
    {
        static EnumToStringMap theMap;

        if(theMap.empty()) {
            theMap[TYPE_COMPENSATION] = COMPENSATION_STRING_;
        }
        return theMap;
    }



    void ResistorPart::print(std::ostream & stream) const
    {
    	stream << " Type: " << getType() << std::endl;
    	if(hasCompensationCapacitor())
    	{
    		stream << " Compensation Capacitor: " << getCompensationCapacitor().getPartId().toStr() << " ";
    		getCompensationCapacitor().printMainStructures(stream);
    		stream << std::endl ;
    	}
    }

	void ResistorPart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& ResistorPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "ResistorPart");
	   Part::writeXml(ResistorPart, doc);
	}


	bool ResistorPart::isResistorPart() const
	{
		return true;
	}

	bool ResistorPart::isLoadPart() const
	{
		return false;
	}

	bool ResistorPart::isBiasPart() const
	{
		return false;
	}

	bool ResistorPart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool ResistorPart::isCapacitancePart() const
	{
		return false;
	}

	bool ResistorPart::isTransconductancePart() const
	{
		return false;
	}

	bool ResistorPart::isUndefinedPart() const
	{
		return false;
	}
	bool ResistorPart::isPositiveFeedbackPart() const
	{
		return false;
	}

}

