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
/*
 * ArrayConnectionRule.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: ga76fuw
 */


#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayRecognitionRules/ArrayConnectionRule.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace StructRec {

	ArrayConnectionRule::ArrayConnectionRule():
			connected_(false)
	{
	}

	ArrayConnectionRule::ArrayConnectionRule(const ArrayConnectionRule& other):
		pinType1_(other.pinType1_),
		pinType2_(other.pinType2_),
		connected_(other.connected_)
	{

	}

	ArrayConnectionRule::~ArrayConnectionRule()
	{
	}

	ArrayConnectionRule* ArrayConnectionRule::clone() const
	{
		return new ArrayConnectionRule(*this);
	}

	void ArrayConnectionRule::setPinType1(const Core::PinType& pinType1)
	{
		pinType1_ = pinType1;
	}

	void ArrayConnectionRule::setPinType2(const Core::PinType& pinType2)
	{
		pinType2_ = pinType2;
	}

	void ArrayConnectionRule::setConnected(bool connected)
	{
		connected_ = connected;
	}

	bool ArrayConnectionRule::isIdentical(const ArrayRecognitionRule& other) const
	{
		if(!other.isConnectionRule()) {
			return false;
		}

		const ArrayConnectionRule& otherRule = static_cast<const ArrayConnectionRule&>(other);
		return (getPinType1() == otherRule.getPinType1()) &&
			   (getPinType2() == otherRule.getPinType2()) &&
			   (connected_ == otherRule.connected_);
	}

	bool ArrayConnectionRule::evaluate(const Core::Device& device) const
	{
		const Core::Net& net1 = device.findNet(getPinType1());
		const Core::Net& net2 = device.findNet(getPinType2());
		bool pinsConnected = (net1 == net2);
		return pinsConnected == connected_;
	}

	bool ArrayConnectionRule::isConnectionRule() const
	{
		return true;
	}

	bool ArrayConnectionRule::isTypeRule() const
	{
		return false;
	}

	bool ArrayConnectionRule::hasPinType1() const
	{
		return pinType1_.isInitialized();
	}

	bool ArrayConnectionRule::hasPinType2() const
	{
		return pinType2_.isInitialized();
	}

	const Core::PinType& ArrayConnectionRule::getPinType1() const
	{
		assert(hasPinType1());
		return pinType1_;
	}

	const Core::PinType& ArrayConnectionRule::getPinType2() const
	{
		assert(hasPinType2());
		return pinType2_;
	}
}

