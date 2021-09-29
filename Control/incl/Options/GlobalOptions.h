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


#ifndef CONTROL_GLOBALOPTIONS_H_
#define CONTROL_GLOBALOPTIONS_H_

#include "Control/incl/Options/OptionsBase.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Control/incl/File/OutputFile.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMappingFile.h"
#include "Log/incl/LogLevel.h"
#include "Log/incl/LogFile.h"
#include "Control/incl/FileFormat/InputFileFormat.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypesFile.h"

#include <string>
#include <ostream>

namespace Core {
    class Core;
    class DeviceTypeRegister;
}

namespace Control {

    class AbstractCircuitSource;

    class GlobalOptions : public OptionsBase
    {
    public:
        GlobalOptions();

        bool hasHelp() const;
        void printHelp(std::ostream& out) const;

        std::string findAnalysisType() const;

        Log::LogLevel findConsoleLogLevel() const;
        Log::LogLevel findFileLogLevel() const;

        bool hasLogFile() const;
        Log::LogFile findLogFile() const;

        bool isCircuitReadInOption() const;

    private:
        static const std::string HELP_;
        static const std::string LOG_FILE_;
        static const std::string LOG_LEVEL_CONSOLE_;
        static const std::string LOG_LEVEL_FILE_;
        static const std::string ANALYSIS_;

        void buildOptions();

        std::string computeHelpMessage() const;
        std::string computeLocalOptionsStringDescription() const;

        std::string computeAnalysisDescription() const;
    };
}

#endif
