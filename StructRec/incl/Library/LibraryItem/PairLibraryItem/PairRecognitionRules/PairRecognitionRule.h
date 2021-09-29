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
#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRULES_SECONDARYRULES_PAIRSECONDARYRULE_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRULES_SECONDARYRULES_PAIRSECONDARYRULE_H_
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>


namespace StructRec {

	class Pair;
	class CrossInstancePair;
	class LibraryItemMappings;
	class PairRecognitionRule
	{
	public:
		virtual ~PairRecognitionRule();
		virtual PairRecognitionRule* clone() = 0 ;

		virtual bool isIdentical(const PairRecognitionRule& other, const LibraryItemMappings& mappings) const = 0;
		virtual void update(const LibraryItemMappings& mappings) = 0;

		virtual bool evaluate (CrossInstancePair & pair) const = 0;
		virtual bool evaluate (Pair & pair) const = 0;
		virtual void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const = 0;

		virtual bool isTechTypeRule() const = 0;
		virtual bool isConnectionRule() const = 0;
		virtual bool isNetRule() const = 0;
		virtual bool isVoltageLevelRule() const = 0;
	};

}



#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_PAIRRULES_SECONDARYRULES_PAIRSECONDARYRULE_H_ */
