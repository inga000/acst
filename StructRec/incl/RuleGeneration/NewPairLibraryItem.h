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

#ifndef STRUCTREC_INCL_RULEGENERATION_NEWPAIRLIBRARYITEM_H_
#define STRUCTREC_INCL_RULEGENERATION_NEWPAIRLIBRARYITEM_H_

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "StructRec/incl/Library/ArrayLibrary.h"
#include "StructRec/incl/RuleGeneration/NewRecognizedChild.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairTechTypeRule.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"

namespace Control {

	class OutputFile;

}

namespace StructRec {

	class PinConnectionKey;
	class PinConnectionKeyMap;

	class NewPairLibraryItem : public PairLibraryItem
	{
	public:
		NewPairLibraryItem();
		~NewPairLibraryItem();

		void setChild1(NewRecognizedChild & child1);
		void setChild2(NewRecognizedChild & child2);

		const NewRecognizedChild & getChild1() const;
		const NewRecognizedChild & getChild2() const;

		NewRecognizedChild & getChild1();
		NewRecognizedChild & getChild2();

		PinConnectionKeyMap & getPinConnectionKeyMap();

		bool hasChildren() const;

		void build(std::string pairLibraryItemName, int & idCounter);

		bool isEqualButInName(NewPairLibraryItem & other);

		void computePersistenceOfChildren(PairLibrary & libraryOfNewItems, ArrayLibrary& arrayLibrary, std::string nameMainItem) const;

	private:
		void setPinConnectionKeyMap(PinConnectionKeyMap & pinConnectionKeyMap);

		bool hasPinConnectionKeyMap();

		bool hasChild1() const;
		bool hasChild2() const;

		void initializeHierarchyLevel();
		void initializeCharacteristicConnection(
				std::map<int, std::vector<PinConnectionKey*> > pinConnectionKeysOfConnectedPins);
		void initializeRecognitionRules();
		void initializeSymmetrie();
		void initializeName(std::string pairLibraryItemName, int & idCounter);

		void initializePinConnectionKeyMap();

		void initializeNetRules();
		void initializeSupplyLevelRules();
		void initializeTechTypeRule();
		void initializeConnectionRulesOfConnectedPins(
				std::map<int, std::vector<PinConnectionKey*> > pinConnectionKeysOfConnectedPins);
		void initializeConnectionRulesOfNotConnectedPins(
				std::map<int, std::vector<PinConnectionKey*> > pinConnectionKeysOfNotConnectedPins);

		std::map<int, std::vector<Core::NetId> > sortNetsAfterNumberOfPinConnectionsOnDeviceLevel() const;

		bool hasEqualPinTypes(std::vector<PinConnectionKey*> connectionKeys, std::vector<PinConnectionKey*> connectionKeysOfOther);

		void computePersistenceOfChild(const Structure  & child, PairLibrary & libraryfNewItems,
				ArrayLibrary& arrayLibrary, std::string nameMainItem) const;

	private:
		NewRecognizedChild * child1_;
		NewRecognizedChild * child2_;

		PinConnectionKeyMap * pinConnectionKeyMap_;
	};



}

#endif /* STRUCTREC_INCL_RULEGENERATION_NEWRECOGNIZEDPAIR_H_ */
