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
#include "Core/incl/Circuit/Device/DeviceType/BipolarDeviceType.h"

namespace Core {

	const DeviceTypeName BipolarDeviceType::BIPOLAR_ = DeviceTypeName("Bipolar");
	const PinName BipolarDeviceType::COLLECTOR_ = PinName("Collector");
	const PinName BipolarDeviceType::BASE_ = PinName("Base");
	const PinName BipolarDeviceType::EMITTER_ = PinName("Emitter");

	BipolarDeviceType::BipolarDeviceType()
	{
		initialize();
	}

	void BipolarDeviceType::initialize()
	{
		setDeviceTypeName();
		addCollector();
		addBase();
		addEmitter();
	}

	void BipolarDeviceType::setDeviceTypeName()
	{
		name_ = BIPOLAR_;
	}

	void BipolarDeviceType::addCollector()
	{
		PinType pinType;
		pinType.setPinName(COLLECTOR_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(COLLECTOR_, pinType));
	}

	void BipolarDeviceType::addBase()
	{
		PinType pinType;
		pinType.setPinName(BASE_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(BASE_, pinType));
	}

	void BipolarDeviceType::addEmitter()
	{
		PinType pinType;
		pinType.setPinName(EMITTER_);
		pinType.setDeviceTypeName(getName());
		pinTypes_.insert(PinTypeMap::value_type(EMITTER_, pinType));
	}

	DeviceTypeName BipolarDeviceType::getName()
	{
		return BIPOLAR_;
	}

	PinType BipolarDeviceType::collector()
	{
		PinType pinType;
		pinType.setPinName(COLLECTOR_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

	PinType BipolarDeviceType::base()
	{
		PinType pinType;
		pinType.setPinName(BASE_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

	PinType BipolarDeviceType::emitter()
	{
		PinType pinType;
		pinType.setPinName(EMITTER_);
		pinType.setDeviceTypeName(getName());
		return pinType;
	}

}



