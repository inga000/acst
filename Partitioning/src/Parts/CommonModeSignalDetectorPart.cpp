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


#include "Partitioning/incl/Parts/CommonModeSignalDetectorPart.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

    CommonModeSignalDetectorPart::CommonModeSignalDetectorPart(int & num)
    {
    	const PartName & name = PartName("CommonModeSignalDetectorPart");
    	PartId id;
    	id.setName(name);
    	id.setId(num);
    	setPartId(id);
    	num++;
    }

    CommonModeSignalDetectorPart::~CommonModeSignalDetectorPart()
    {
    }


    bool CommonModeSignalDetectorPart::isInitialized() const
    {
        return hasMainStructures() && hasPartId();
    }



    void CommonModeSignalDetectorPart::print(std::ostream & stream) const
    {

    }

	void CommonModeSignalDetectorPart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& CommonModeSignalDetectorPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "CommonModeSignalDetectorPart");
	   Part::writeXml(CommonModeSignalDetectorPart, doc);
	}

	bool CommonModeSignalDetectorPart::hasCapacitors() const
	{
		const StructRec::Structure & mainStruct = **getMainStructures().begin();

		return mainStruct.getStructureName().toStr() == "CommonModeSignalDetector";
	}

	bool CommonModeSignalDetectorPart::isCommonModeSignalDetectorPart() const
	{
		return true;
	}

	bool CommonModeSignalDetectorPart::isLoadPart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isBiasPart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isResistorPart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isCapacitancePart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isTransconductancePart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isUndefinedPart() const
	{
		return false;
	}

	bool CommonModeSignalDetectorPart::isPositiveFeedbackPart() const
	{
		return false;
	}

}
