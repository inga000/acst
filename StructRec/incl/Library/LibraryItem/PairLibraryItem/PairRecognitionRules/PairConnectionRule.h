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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRCONNECTIONRULE_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRCONNECTIONRULE_H_

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairRecognitionRule.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"

namespace StructRec {

	class Structure;
	class CrossInstancePair;

	class PairConnectionRule : public PairRecognitionRule
	{
	public:
		PairConnectionRule();
		PairConnectionRule(const PairConnectionRule& other);
		PairConnectionRule* clone();
		~PairConnectionRule();

		bool isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

		void setPinType1(const StructurePinType & pinType1);
		void setPinType2(const StructurePinType & pinType2);
		void setConnected(bool connected);

		bool evaluate(CrossInstancePair & pair) const;
		bool evaluate(Pair & pair) const;

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;

		bool isTechTypeRule() const;
		bool isConnectionRule() const;
		bool isNetRule() const;
		bool isVoltageLevelRule() const;

	private:
		bool hasPinType1() const;
		bool hasPinType2() const;

		const StructurePinType& getPinType1() const;
		const StructurePinType& getPinType2() const;

		StructurePinType pinType1_;
		StructurePinType pinType2_;
		bool connected_;
	};

}



#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRCONNECTIONRULE_H_ */
