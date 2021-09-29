/**************************************************************************************************
 *  Main authors:
 *     Inga Abel <inga.abel@tum.de>, 
 *     Maximilian Neuner <maximilian.neuner@tum.de>, 
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

#ifndef CONTROL_INCL_OPTIONS_HSPICEOPTIONS_H_
#define CONTROL_INCL_OPTIONS_HSPICEOPTIONS_H_

#include "Control/incl/Options/OptionsBase.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Control/incl/File/OutputFile.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMappingFile.h"
#include "Log/incl/LogLevel.h"
#include "Log/incl/LogFile.h"
#include "Control/incl/FileFormat/InputFileFormat.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypesFile.h"

#include "StructRec/incl/LibraryFile/LibraryFile.h"

#include <string>
#include <ostream>

namespace Core {
    class Core;
    class DeviceTypeRegister;
}

namespace Control {

    class AbstractCircuitSource;



    class HspiceOptions : public OptionsBase
    {
    public:
    	HspiceOptions(const std::string& description);

        Core::DeviceTypeRegister* readDeviceTypeRegister() const;
        HSpice::InputFile::SupplyNetIdentifiers readSupplyNetIdentifiers() const;
        HSpice::DeviceLineMapper  * readDeviceLineMapper() const;

         bool hasHSpiceMappingFile() const;
         bool hasHSpiceSupplyNetFile() const;

         virtual bool isCircuitReadInOption() const = 0;

    private:
        static const std::string HSPICE_MAPPING_FILE_OPTION_;
        static const std::string HSPICE_MAPPING_FILE_DESCRIPTION_;
        static const std::string HSPICE_SUPPLYNET_FILE_OPTION_;
        static const std::string HSPICE_SUPPLYNET_FILE_DESCRIPTION_;
        static const std::string DEVICETYPES_FILE_OPTION_;
        static const std::string DEVICETYPES_FILE_DESCRIPTION_;

        void initialize();


        HSpice::DeviceLineMappingFile findHSpiceMappingFile() const;


        HSpice::InputFile::SupplyNetFile findHSpiceSupplyNetFile() const;

        bool hasDeviceTypesFile() const;
        Core::DeviceTypesFile findDeviceTypeFile() const;
    };
}



#endif /* CONTROL_INCL_OPTIONS_HSPICEOPTIONS_H_ */
