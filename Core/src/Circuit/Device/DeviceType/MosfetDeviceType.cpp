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

#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"

namespace Core {

	const DeviceTypeName MosfetDeviceType::MOSFET_ = DeviceTypeName("Mosfet");
	const PinName MosfetDeviceType::DRAIN_ = PinName("Drain");
	const PinName MosfetDeviceType::GATE_ = PinName("Gate");
	const PinName MosfetDeviceType::SOURCE_ = PinName("Source");
	const PinName MosfetDeviceType::BULK_ = PinName("Bulk");

	MosfetDeviceType::MosfetDeviceType()
	{
		initialize();
	}

	void MosfetDeviceType::initialize()
	{
		setDeviceTypeName();
		addDrain();
		addGate();
		addSource();
		addBulk();
	}

	void MosfetDeviceType::setDeviceTypeName()
	{
		name_ = MOSFET_;
	}

	void MosfetDeviceType::addDrain()
	{
		PinType pinType;
		pinType.setPinName(DRAIN_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(DRAIN_,pinType));
	}

	void MosfetDeviceType::addGate()
	{
		PinType pinType;
		pinType.setPinName(GATE_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(GATE_,pinType));
	}

	void MosfetDeviceType::addSource()
	{
		PinType pinType;
		pinType.setPinName(SOURCE_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(SOURCE_,pinType));
	}

	void MosfetDeviceType::addBulk()
	{
		PinType pinType;
		pinType.setPinName(BULK_);
		pinType.setDeviceTypeName(getName());
		pinType.setOptional(true);
		pinTypes_.insert(PinTypeMap::value_type(BULK_,pinType));
	}

	DeviceTypeName MosfetDeviceType::getName()
	{
		return MOSFET_;
	}

	PinType MosfetDeviceType::drain()
	{
		PinType pinType;
		pinType.setPinName(DRAIN_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

	PinType MosfetDeviceType::gate()
	{
		PinType pinType;
		pinType.setPinName(GATE_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

	PinType MosfetDeviceType::source()
	{
		PinType pinType;
		pinType.setPinName(SOURCE_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

	PinType MosfetDeviceType::bulk()
	{
		PinType pinType;
		pinType.setPinName(BULK_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}
}


