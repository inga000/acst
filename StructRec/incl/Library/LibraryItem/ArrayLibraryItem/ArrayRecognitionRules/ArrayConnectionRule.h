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

#ifndef STRUCTREC_INCL_LIBRARY_CONNECTIONRULE_H_
#define STRUCTREC_INCL_LIBRARY_CONNECTIONRULE_H_

#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayRecognitionRules/ArrayRecognitionRule.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include <string>

namespace StructRec {

	class ArrayConnectionRule : public ArrayRecognitionRule
	{
	public:
		ArrayConnectionRule();
		ArrayConnectionRule(const ArrayConnectionRule& other);
		~ArrayConnectionRule();

		ArrayConnectionRule* clone() const;

		void setPinType1(const Core::PinType& pinType1);
		void setPinType2(const Core::PinType& pinType2);
		void setConnected(bool connected);

		bool isIdentical(const ArrayRecognitionRule& other) const;
		bool evaluate(const Core::Device& device) const;

		bool isConnectionRule() const;
		bool isTypeRule() const;

	private :
		bool hasPinType1() const;
		bool hasPinType2() const;

		const Core::PinType& getPinType1() const;
		const Core::PinType& getPinType2() const;

	private:
		Core::PinType pinType1_;
		Core::PinType pinType2_;
		bool connected_;
	};

}

#endif /* STRUCTREC_INCL_LIBRARY_CONNECTIONRULE_H_ */
