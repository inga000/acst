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
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapping.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include <sstream>
#include <iterator>
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {

	const char DeviceLineMapping::IDENTIFIER_NOT_INITIALIZED_ = '\0';
	const int DeviceLineMapping::POSITION_NOT_INITIALIZED_ = -1;

	DeviceLineMapping::DeviceLineMapping():
			identifier_(IDENTIFIER_NOT_INITIALIZED_),
			deviceIdPosition_(POSITION_NOT_INITIALIZED_),
			modelNamePosition_(POSITION_NOT_INITIALIZED_)
	{
	}

	DeviceLineMapping::~DeviceLineMapping()
	{
	}

	void DeviceLineMapping::setId(char identifier)
	{
		assert(identifier != IDENTIFIER_NOT_INITIALIZED_);
		identifier_ = identifier;
	}

	void DeviceLineMapping::setDeviceTypeName(const Core::DeviceTypeName& deviceTypeName)
	{
		assert(deviceTypeName.isInitialized());
		deviceTypeName_ = deviceTypeName;
	}

	void DeviceLineMapping::setDeviceIdPosition(int position)
	{
		assert(position > POSITION_NOT_INITIALIZED_);
		deviceIdPosition_ = position;
	}

	void DeviceLineMapping::setModelNamePosition(int position)
	{
		assert(position > POSITION_NOT_INITIALIZED_);
		modelNamePosition_ = position;
	}

	void DeviceLineMapping::addPin(const Core::PinName& pinName, int position)
	{
		assert(position > POSITION_NOT_INITIALIZED_);
		pinPositions_.insert(PinPositionMap::value_type(pinName, position));
	}

	void DeviceLineMapping::addModel(const InputFile::ModelName& modelName, const Core::TechType& techType)
	{
		modelTechTypeMap_.insert(ModelTechTypeMap::value_type(modelName, techType));
	}

	std::vector<Core::NetId> DeviceLineMapping::computeNetIds(const std::string& line) const
	{
		std::istringstream buf(line);
		std::istream_iterator<std::string> beg(buf), end;
		std::vector<std::string> tokens(beg, end);

		std::vector<Core::NetId> netIds;
		for(PinPositionMap::const_iterator it = pinPositions_.begin(); it != pinPositions_.end(); it++)
		{
			int position = it->second;
			Core::NetId netId = Core::NetName(tokens[position]).createRootIdentifier();
			netIds.push_back(netId);
		}
		return netIds;
	}

	char DeviceLineMapping::getIdentifier() const
	{
		assert(identifier_ != IDENTIFIER_NOT_INITIALIZED_);
		return identifier_;
	}

	Core::DeviceTypeName DeviceLineMapping::getDeviceTypeName() const
	{
		assert(deviceTypeName_.isInitialized());
		return deviceTypeName_;
	}

	int DeviceLineMapping::getDeviceIdPosition() const
	{
		assert(deviceIdPosition_ > POSITION_NOT_INITIALIZED_);
		return deviceIdPosition_;
	}

	int DeviceLineMapping::getModelNamePosition() const
	{
		assert(modelNamePosition_ > POSITION_NOT_INITIALIZED_);
		return modelNamePosition_;
	}

	int DeviceLineMapping::findPinPosition(const Core::PinName& pinName) const
	{
		assert(pinPositions_.find(pinName) != pinPositions_.end());
		return pinPositions_.find(pinName)->second;
	}

	Core::TechType DeviceLineMapping::findTechType(const InputFile::ModelName& modelName) const
	{
		assert(modelTechTypeMap_.find(modelName) != modelTechTypeMap_.end());
		return modelTechTypeMap_.find(modelName)->second;
	}

	bool DeviceLineMapping::hasModel() const
	{
		return !modelTechTypeMap_.empty();
	}
}


