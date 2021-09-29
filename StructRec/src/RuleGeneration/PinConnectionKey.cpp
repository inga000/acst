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

#include "StructRec/incl/RuleGeneration/PinConnectionKey.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const int PinConnectionKey::NUMBER_NOT_INITIALIZED_ = -1;

	PinConnectionKey::PinConnectionKey() :
			childNumber_(NUMBER_NOT_INITIALIZED_),
			connectedChildNumber_(NUMBER_NOT_INITIALIZED_),
			connected_(false),
			isCharacteristicConnection_(false),
			pin_(NULL),
			connectedPin_(NULL)
	{
	}

	PinConnectionKey::~PinConnectionKey()
	{
	}

	void PinConnectionKey::setChildNumber(int childNumber)
	{
		childNumber_ = childNumber;
	}

	void PinConnectionKey::setPin(const StructurePin & pin)
	{
		pin_ = & pin;
	}

	void PinConnectionKey::setConnected(bool connected)
	{
		connected_ = connected;
	}

	bool PinConnectionKey::isConnected() const
	{
		return connected_;
	}

	void PinConnectionKey::setConnectedChildNumber(
		 int childNumber)
	{
		connectedChildNumber_ = childNumber;
	}

	void PinConnectionKey::setConnectedPin(
		const StructurePin & pin)
	{
		connectedPin_ = & pin;
	}

	int PinConnectionKey::getChildNumber() const
	{
		assert(hasChildNumber());
		return childNumber_;
	}

	const StructurePin & PinConnectionKey::getPin() const
	{
		assert(hasPin());
		return * pin_;
	}

	int PinConnectionKey::getConnectedChildNumber() const
	{
		assert(hasConnectedChildNumber());
		return childNumber_;
	}

	const StructurePin &  PinConnectionKey::getConnectedPin() const
	{
		assert(hasConnectedPin());
		return  * connectedPin_;
	}

	bool PinConnectionKey::hasChildNumber() const
	{
		return childNumber_ != NUMBER_NOT_INITIALIZED_;
	}

	bool PinConnectionKey::hasPin() const
	{
		return pin_ != NULL;
	}

	bool PinConnectionKey::hasConnectedChildNumber() const
	{
		return connectedChildNumber_ != NUMBER_NOT_INITIALIZED_;
	}

	bool PinConnectionKey::hasConnectedPin() const
	{
		return connectedPin_ != NULL;
	}

	bool PinConnectionKey::getConnected()
	{
		return connected_;
	}

	void PinConnectionKey::setIsCharacteristicConnection(
			bool isCharacteristicConnection)
	{
		isCharacteristicConnection_ = isCharacteristicConnection;
	}

	bool PinConnectionKey::isChracteristicConnection()
	{
		return isCharacteristicConnection_;
	}

}


