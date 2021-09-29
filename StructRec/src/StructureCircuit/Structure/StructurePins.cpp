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

#include "StructRec/incl/StructureCircuit/Structure/StructurePins.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace StructRec {

	void StructurePins::add(StructurePinType pinType, StructurePin * pin)
	{
		pinTypeToPinMap_.insert(std::pair<StructurePinType,StructurePin*>(pinType,pin));
	}

	StructurePin& StructurePins::findPin (
		const StructurePinType pinType) const
	{
		assert(hasPin(pinType));
		return * pinTypeToPinMap_.at(pinType);
	}

	bool StructurePins::hasPin(const StructurePinType pinType) const
	{
		return pinTypeToPinMap_.find(pinType) != pinTypeToPinMap_.end();
	}

	void StructurePins::deleteAllPins()
	{
		for (std::map<StructurePinType, StructurePin*>::const_iterator it =pinTypeToPinMap_.begin();
			it != pinTypeToPinMap_.end(); ++it)
		{
			StructurePin & currentPin = * it->second;
			delete & currentPin;
		}
	}

	bool StructurePins::isInitialized() const
	{
		return (!pinTypeToPinMap_.empty());
	}

	const std::map<StructurePinType,StructurePin*> & StructurePins::getPinTypeToPinMap() const
	{
		assert(isInitialized());
		return pinTypeToPinMap_;
	}

	void StructurePins::print(std::ostream & stream) const
	{
		int counter = 1;
		stream << "Pins: ";
		for (std::map<StructurePinType, StructurePin*>::const_iterator it = pinTypeToPinMap_.begin(); it != pinTypeToPinMap_.end(); it++)
		{
			StructurePin * pinPointer = it->second;
	        stream << pinPointer->getPinType().getPinName() << "[" << counter << "] ";
	        stream << "(";
	        if(pinPointer->isConnected())
	        {
	        	stream << pinPointer->getNet();
	         }
	        else
	        {
	        	stream << "no connection";
	        }
	        stream << ")" << "; " ;
		}
	}

	void StructurePins::writeXml(Core::XmlNode & pins, Core::XmlDocument & doc) const
	{
		for (std::map<StructurePinType, StructurePin*>::const_iterator it = pinTypeToPinMap_.begin(); it != pinTypeToPinMap_.end(); it++)
		{
			StructurePin * pinPointer = it->second;
			Core::XmlNode & pin = Core::RapidXmlUtils::addNode(pins, doc, "pin");
			Core::RapidXmlUtils::addAttr(pin, doc, "name", pinPointer->getPinType().getPinName().toStr());
			Core::RapidXmlUtils::addAttr(pin, doc, "net", pinPointer->getNet().toStr());
		}
	}

	std::vector<StructurePin*> StructurePins::findAllPins() const
	{
		std::vector<StructurePin*> pins;
		for(std::map<StructurePinType, StructurePin*>::const_iterator it = pinTypeToPinMap_.begin(); it != pinTypeToPinMap_.end(); it++)
		{
			StructurePin * pinPointer = it->second;
			pins.push_back(pinPointer);
		}
		return pins;
	}

}


