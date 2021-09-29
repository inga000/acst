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

#include "Synthesis/incl/LocalOptionsTopologyLibraryGeneration.h"

#include "StructRec/incl/LibraryFile/LibraryFile.h"

#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypesFile.h"

#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {


    const std::string LocalOptionsTopologyLibraryGeneration::HSPICE_NETLIST_DIRECTORY_OPTION_ = "HSPICE-netlist-dir";
    const std::string LocalOptionsTopologyLibraryGeneration::HSPICE_NETLIST_DIRECTORY_DESCRIPTION_ = "Path to the directory to which the generated netlists are written.";
	const std::string LocalOptionsTopologyLibraryGeneration::XML_STRUCTREC_LIBRARY_FILE_OPTION_ = "xml-structrec-library-file";
	const std::string LocalOptionsTopologyLibraryGeneration::XML_STRUCTREC_LIBRARY_FILE_DESCRIPTION_ = "path to the xml-library-file for structure recognition";
    const std::string LocalOptionsTopologyLibraryGeneration::DEVICETYPES_FILE_OPTION_ = "device-types-file";
    const std::string LocalOptionsTopologyLibraryGeneration::DEVICETYPES_FILE_DESCRIPTION_ = "path to the file which defines the supported device types";


    LocalOptionsTopologyLibraryGeneration::LocalOptionsTopologyLibraryGeneration(const std::string& description) :
            Control::OptionsBase(description)
    {
        initialize();
    }

    void LocalOptionsTopologyLibraryGeneration::initialize()
    {
    	Control::OptionsBase::addStringOption(HSPICE_NETLIST_DIRECTORY_OPTION_, HSPICE_NETLIST_DIRECTORY_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(XML_STRUCTREC_LIBRARY_FILE_OPTION_, XML_STRUCTREC_LIBRARY_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(DEVICETYPES_FILE_OPTION_, DEVICETYPES_FILE_DESCRIPTION_);

    }

    bool LocalOptionsTopologyLibraryGeneration::hasStructureLibraryFile() const
    {
    	return hasOptionValue(XML_STRUCTREC_LIBRARY_FILE_OPTION_);
    }

    Core::DeviceTypeRegister* LocalOptionsTopologyLibraryGeneration::readDeviceTypeRegister() const
    {
    	assert(hasDeviceTypesFile());
    	Core::DeviceTypesFile deviceTypesFile = findDeviceTypeFile();
    	return deviceTypesFile.parse();
    }


    StructRec::LibraryFile LocalOptionsTopologyLibraryGeneration::getStructRecLibraryFile() const
    {
    	 std::string optionValue = findStringOptionValue(XML_STRUCTREC_LIBRARY_FILE_OPTION_);
    	 StructRec::LibraryFile xmlLibraryFile;
    	 xmlLibraryFile.setPath(optionValue);
    	 return xmlLibraryFile;
    }

    std::string LocalOptionsTopologyLibraryGeneration::getHSPICENetlistDirectoryPath() const
    {
    	return findStringOptionValue(HSPICE_NETLIST_DIRECTORY_OPTION_);
    }

    bool LocalOptionsTopologyLibraryGeneration::hasDeviceTypesFile() const
    {
    	return hasOptionValue(DEVICETYPES_FILE_OPTION_);
    }

    Core::DeviceTypesFile LocalOptionsTopologyLibraryGeneration::findDeviceTypeFile() const
    {
    	assert(hasDeviceTypesFile());
    	Core::DeviceTypesFile deviceTypesFile;
    	std::string filePath = findStringOptionValue(DEVICETYPES_FILE_OPTION_);
    	deviceTypesFile.setPath(filePath);
    	return deviceTypesFile;
    }

	bool LocalOptionsTopologyLibraryGeneration::isCircuitReadInOption() const
	{
		return false;
	}

}
