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

#ifndef HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPING_H_
#define HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPING_H_

#include "HSpice/incl/InputFile/ModelName.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinName.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include <map>

namespace HSpice {

	class DeviceLineMapping
	{
	private:
		typedef std::map<Core::PinName, int> PinPositionMap;
		typedef std::map<InputFile::ModelName, Core::TechType> ModelTechTypeMap;

	public:
		DeviceLineMapping();
		~DeviceLineMapping();

		void setId(char identifier);
		void setDeviceTypeName(const Core::DeviceTypeName& deviceTypeName);
		void setDeviceIdPosition(int position);
		void setModelNamePosition(int position);
		void addPin(const Core::PinName& pinName, int position);
		void addModel(const InputFile::ModelName& modelName, const Core::TechType& techType);

		std::vector<Core::NetId> computeNetIds(const std::string& line) const;

		char getIdentifier() const;
		Core::DeviceTypeName getDeviceTypeName() const;
		int getDeviceIdPosition() const;
		int getModelNamePosition() const;
		int findPinPosition(const Core::PinName& pinName) const;
		Core::TechType findTechType(const InputFile::ModelName& modelName) const;

		bool hasModel() const;

	private:
		static const char IDENTIFIER_NOT_INITIALIZED_;
		static const int POSITION_NOT_INITIALIZED_;

	private:
		char identifier_;
		Core::DeviceTypeName deviceTypeName_;

		int deviceIdPosition_;
		int modelNamePosition_;
		PinPositionMap pinPositions_;
		ModelTechTypeMap modelTechTypeMap_;
	};
}



#endif /* HSPICE_INCL_DEVICELINEMAPPINGFILE_DEVICELINEMAPPING_H_ */
