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


#include "../../incl/Options/HspiceOptions.h"
#include "Control/incl/FactoryMap/AnalysesFactoryMap.h"
#include "HSpice/incl/InputFile/InputFile.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Common/BacktraceAssert.h"


namespace Control {


    const std::string HspiceOptions::HSPICE_MAPPING_FILE_OPTION_ = "hspice-mapping-file";
    const std::string HspiceOptions::HSPICE_MAPPING_FILE_DESCRIPTION_ = "path to the file which defines the device mapping in the hspice file";
    const std::string HspiceOptions::HSPICE_SUPPLYNET_FILE_OPTION_ = "hspice-supplynet-file";
    const std::string HspiceOptions::HSPICE_SUPPLYNET_FILE_DESCRIPTION_ = "path to the file which defines the supply nets in the hspice netlist";
    const std::string HspiceOptions::DEVICETYPES_FILE_OPTION_ = "device-types-file";
    const std::string HspiceOptions::DEVICETYPES_FILE_DESCRIPTION_ = "path to the file which defines the supported device types";


    HspiceOptions::HspiceOptions(const std::string& description) :
            OptionsBase(description)
    {
    	initialize();
    }


    Core::DeviceTypeRegister* HspiceOptions::readDeviceTypeRegister() const
    {
    	assert(hasDeviceTypesFile());
    	Core::DeviceTypesFile deviceTypesFile = findDeviceTypeFile();
    	return deviceTypesFile.parse();
    }

    HSpice::InputFile::SupplyNetIdentifiers HspiceOptions::readSupplyNetIdentifiers() const
    {
		 HSpice::InputFile::SupplyNetIdentifiers  supplyNetIds;

		 supplyNetIds.initializeEmpty();
		 HSpice::InputFile::SupplyNetFile supplyNetFile = findHSpiceSupplyNetFile();
		 supplyNetFile.setSupplyNetIdentifier(supplyNetIds);
		 supplyNetFile.parse();
		 return supplyNetIds;
    }
    HSpice::DeviceLineMapper * HspiceOptions::readDeviceLineMapper() const
    {
    	assert(hasHSpiceMappingFile());
		HSpice::DeviceLineMappingFile deviceLineMappingFile = findHSpiceMappingFile();
		return deviceLineMappingFile.parse();
    }

    bool HspiceOptions::hasHSpiceMappingFile() const
    {
        return hasOptionValue(HSPICE_MAPPING_FILE_OPTION_);
    }

    HSpice::DeviceLineMappingFile HspiceOptions::findHSpiceMappingFile() const
    {
        assert(hasHSpiceMappingFile());
        HSpice::DeviceLineMappingFile deviceLineMappingFile;
        std::string mappingFilePath = findStringOptionValue(HSPICE_MAPPING_FILE_OPTION_);
        deviceLineMappingFile.setPath(mappingFilePath);
        return deviceLineMappingFile;
    }

    bool HspiceOptions::hasHSpiceSupplyNetFile() const
    {
        return hasOptionValue(HSPICE_SUPPLYNET_FILE_OPTION_);
    }

    HSpice::InputFile::SupplyNetFile HspiceOptions::findHSpiceSupplyNetFile() const
    {
        assert(hasHSpiceMappingFile());
        HSpice::InputFile::SupplyNetFile supplyNetFile;
        std::string filePath = findStringOptionValue(HSPICE_SUPPLYNET_FILE_OPTION_);
        supplyNetFile.setPath(filePath);
        return supplyNetFile;
    }

    bool HspiceOptions::hasDeviceTypesFile() const
    {
    	return hasOptionValue(DEVICETYPES_FILE_OPTION_);
    }

    Core::DeviceTypesFile HspiceOptions::findDeviceTypeFile() const
    {
    	assert(hasDeviceTypesFile());
    	Core::DeviceTypesFile deviceTypesFile;
    	std::string filePath = findStringOptionValue(DEVICETYPES_FILE_OPTION_);
    	deviceTypesFile.setPath(filePath);
    	return deviceTypesFile;
    }

    void HspiceOptions::initialize()
    {
    	Control::OptionsBase::addStringOption(DEVICETYPES_FILE_OPTION_, DEVICETYPES_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(HSPICE_SUPPLYNET_FILE_OPTION_, HSPICE_SUPPLYNET_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(HSPICE_MAPPING_FILE_OPTION_, HSPICE_MAPPING_FILE_DESCRIPTION_);
    }

}


