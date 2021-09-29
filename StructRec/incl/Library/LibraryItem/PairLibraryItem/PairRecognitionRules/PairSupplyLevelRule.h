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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRSUPPLYLEVELRULE_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRSUPPLYLEVELRULE_H_

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairRecognitionRule.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"

#include "Core/incl/Circuit/Net/Supply.h"


namespace StructRec {


	class PairSupplyLevelRule : public PairRecognitionRule
	{
	public:
		PairSupplyLevelRule();
		PairSupplyLevelRule(const PairSupplyLevelRule& other);
		PairSupplyLevelRule* clone();
		~PairSupplyLevelRule();

		bool isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

		void setPinTypeOfHigherLevel(const StructurePinType & pinType);
		void setChildNumberOfHigherLevel(int childNumber);

		void setPinTypeOfLowerLevel(const StructurePinType & pinType);
		void setChildNumberOfLowerLevel(int childNumber);

		void setTypeOfSupply(Core::Supply supply);

		bool evaluate(CrossInstancePair & pair) const;
		bool evaluate(Pair & pair) const;

		bool isTechTypeRule() const;
		bool isConnectionRule() const;
		bool isNetRule() const;
		bool isVoltageLevelRule() const;

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;

	private:
		const int & getChildNumberOfHigherLevel() const;
		const StructurePinType& getPinTypeOfHigherLevel() const;

		const int & getChildNumberOfLowerLevel() const;
		const StructurePinType& getPinTypeOfLowerLevel() const;

		const Core::Supply getTypeOfSupply()const;

		bool hasChildNumberOfHigherLevel() const;
		bool hasPinTypeOfHigherLevel() const;

		bool hasChildNumberOfLowerLevel() const;
		bool hasPinTypeOfLowerLevel() const;

		bool hasTypeOfSupply()const;

		static const int CHILDNUMBER_NOT_INITIALIZED_;

		StructurePinType pinTypeOfHigherLevel_;
		int childNumberOfHigherLevel_;

		StructurePinType pinTypeOfLowerLevel_;
		int childNumberOfLowerLevel_;

		Core::Supply typeOfSupply_;
	};

}


#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRSUPPLYLEVELRULE_H_ */
