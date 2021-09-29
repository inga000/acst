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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CharacteristicConnection/CharacteristicConnection.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	CharacteristicConnection::CharacteristicConnection()
	{
	}

	CharacteristicConnection::CharacteristicConnection(const CharacteristicConnection& other):
		firstChildPinTypes_(other.firstChildPinTypes_),
		secondChildPinTypes_(other.secondChildPinTypes_)
	{

	}

	CharacteristicConnection::~CharacteristicConnection()
	{

	}

	void CharacteristicConnection::addFirstChildPinType(const StructurePinType& structurePinType)
	{
		firstChildPinTypes_.insert(structurePinType);
	}

	void CharacteristicConnection::addSecondChildPinType(const StructurePinType& structurePinType)
	{
		secondChildPinTypes_.insert(structurePinType);
	}

	std::vector<StructurePinType> CharacteristicConnection::getFirstChildPinTypes() const
	{
		assert(!firstChildPinTypes_.empty());
		std::vector<StructurePinType> firstChildPinTypes;
		for(auto& it : firstChildPinTypes_)
		{
			firstChildPinTypes.push_back(it);
		}
		return firstChildPinTypes;
	}
	std::vector<StructurePinType> CharacteristicConnection::getSecondChildPinTypes() const
	{
		assert(!secondChildPinTypes_.empty());
		std::vector<StructurePinType> secondChildPinTypes;
		for(auto& it : secondChildPinTypes_)
		{
			secondChildPinTypes.push_back(it);
		}
		return secondChildPinTypes;
	}

	void CharacteristicConnection::writeXml(Core::XmlNode& node,
			Core::XmlDocument& doc) const
	{
			Core::XmlNode& characteristicConnection = Core::RapidXmlUtils::addNode(node, doc, "characteristicConnection");

			std::vector<StructurePinType> firstChildPins = getFirstChildPinTypes();
			std::vector<StructurePinType> secondChildPins = getSecondChildPinTypes();

			for(auto& it_child1 : firstChildPins)
			{
				StructurePinType firstChildPin = it_child1;
				Core::XmlNode& firstChildPinType = Core::RapidXmlUtils::addNode(characteristicConnection, doc, "firstChildPinType");
				Core::RapidXmlUtils::addSimpleNode(firstChildPinType, doc, "structureName", firstChildPin.getStructureName().toStr());
				Core::RapidXmlUtils::addSimpleNode(firstChildPinType, doc, "structurePinName", firstChildPin.getPinName().toStr());
			}

			for(auto& it_child2 : secondChildPins)
			{
				StructurePinType secondChildPin = it_child2;
				Core::XmlNode& secondChildPinType = Core::RapidXmlUtils::addNode(characteristicConnection, doc, "secondChildPinType");
				Core::RapidXmlUtils::addSimpleNode(secondChildPinType, doc, "structureName", secondChildPin.getStructureName().toStr());
				Core::RapidXmlUtils::addSimpleNode(secondChildPinType, doc, "structurePinName", secondChildPin.getPinName().toStr());
			}
	}

	bool CharacteristicConnection::isIdentical(const CharacteristicConnection& other, const LibraryItemMappings& mappings) const
	{
		bool sameChildOnePinTypes = comparePinTypes(firstChildPinTypes_, other.firstChildPinTypes_, mappings);
		bool sameChildTwoPinTypes = comparePinTypes(secondChildPinTypes_, other.secondChildPinTypes_, mappings);
		return sameChildOnePinTypes && sameChildTwoPinTypes;
	}

	void CharacteristicConnection::update(const LibraryItemMappings& mappings)
	{
		StructurePinTypeSet copyFirstChildPinTypes;
		for(StructurePinTypeSet::const_iterator it = firstChildPinTypes_.begin(); it != firstChildPinTypes_.end(); it++)
		{
			StructurePinType mappedPinType = mappings.findMappedPinType(*it);
			copyFirstChildPinTypes.insert(mappedPinType);
		}
		firstChildPinTypes_ = copyFirstChildPinTypes;

		StructurePinTypeSet copySecondChildPinTypes;
		for(StructurePinTypeSet::const_iterator it = secondChildPinTypes_.begin(); it != secondChildPinTypes_.end(); it++)
		{
			StructurePinType mappedPinType = mappings.findMappedPinType(*it);
			copySecondChildPinTypes.insert(mappedPinType);
		}
		secondChildPinTypes_ = copySecondChildPinTypes;
	}

	bool CharacteristicConnection::isInitialized() const
	{
		return hasFirstChildPinTypes() && hasSecondChildPinTypes();
	}

	bool CharacteristicConnection::comparePinTypes(const StructurePinTypeSet& original, const StructurePinTypeSet& other, const LibraryItemMappings& mappings) const
	{
		bool samePinTypes = true;
		for(StructurePinTypeSet::const_iterator it = original.begin(); it != original.end(); it++)
		{
			StructurePinType originalPinType = *it;

			bool hasIdenticalPinType = false;
			for(StructurePinTypeSet::const_iterator it2 = other.begin(); it2 != other.end(); it2++)
			{
				StructurePinType otherPinType = *it2;
				StructurePinType mappedPinType = mappings.findMappedPinType(otherPinType);
				if(originalPinType == mappedPinType) {
					hasIdenticalPinType = true;
					break;
				}
			}
			 if(!hasIdenticalPinType) {
				 samePinTypes = false;
				 break;
			 }
		}
		return samePinTypes;
	}

	bool CharacteristicConnection::hasFirstChildPinTypes() const
	{
		return !firstChildPinTypes_.empty();
	}

	bool CharacteristicConnection::hasSecondChildPinTypes() const
	{
		return !secondChildPinTypes_.empty();
	}

}

