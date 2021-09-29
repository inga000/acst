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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRTECHTYPERULE_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRTECHTYPERULE_H_

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairRecognitionRule.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"

namespace StructRec {

	class CrossInstancePair;

	class PairTechTypeRule : public PairRecognitionRule
	{
	public:
		PairTechTypeRule();
		PairTechTypeRule(const PairTechTypeRule& other);
		PairTechTypeRule* clone();
		~PairTechTypeRule();

		bool isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

		void initialize(const Structure & child1, const Structure & child2);

		void setTechTypeRule(const std::string& typeRule);
		void setTechTypeChild1(Core::TechType techTypeChild1);
		void setTechTypeChild2(Core::TechType techTypeChild2);

		bool evaluate(CrossInstancePair & pair) const;
		bool evaluate(Pair & pair) const;

		bool operator==(PairTechTypeRule & other) const;

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;

		bool isTechTypeRule() const;
		bool isConnectionRule() const;
		bool isNetRule() const;
		bool isVoltageLevelRule() const;

		std::string getTypeRule() const;

		Core::TechType getTechTypeChild1() const;
		Core::TechType getTechTypeChild2() const;

	private:

		bool hasTechTypeChild1() const;
		bool hasTechTypeChild2() const;

	private:
		static const std::string SAME_TYPE_;
		static const std::string DIFFERENT_TYPE_;
		static const std::string NO_RULE_;

	private:
		std::string typeRule_;
		Core::TechType techTypeChild1_;
		Core::TechType techTypeChild2_;
	};

}



#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRECOGNITIONRULES_PAIRTECHTYPERULE_H_ */
