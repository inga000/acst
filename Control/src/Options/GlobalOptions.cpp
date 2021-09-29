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


#include "Control/incl/Options/GlobalOptions.h"
#include "Control/incl/FactoryMap/AnalysesFactoryMap.h"
#include "HSpice/incl/InputFile/InputFile.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Common/BacktraceAssert.h"

#include <iostream>


namespace Control {

    const std::string GlobalOptions::HELP_ = "help";
    const std::string GlobalOptions::LOG_FILE_ = "log-file";
    const std::string GlobalOptions::LOG_LEVEL_CONSOLE_ = "log-level-console";
    const std::string GlobalOptions::LOG_LEVEL_FILE_ = "log-level-file";

    const std::string GlobalOptions::ANALYSIS_ = "analysis";




    GlobalOptions::GlobalOptions() :
            OptionsBase("GlobalOptions")
    {
        buildOptions();
    }

    void GlobalOptions::buildOptions()
    {

        std::string analysisDescription = computeAnalysisDescription();
        addBoolOption(HELP_, "produce help message");
        addStringOption(LOG_FILE_, "path to logfile");
        addStringOption(LOG_LEVEL_CONSOLE_, "log level for console", "INFO");
        addStringOption(LOG_LEVEL_FILE_, "log level for log file", "INFO");
        addStringOption(ANALYSIS_, analysisDescription);
    }

    bool GlobalOptions::hasHelp() const
    {
        return hasOptionValue(HELP_);
    }




    void GlobalOptions::printHelp(std::ostream& out) const
    {
        out << computeHelpMessage();
    }


    Log::LogLevel GlobalOptions::findConsoleLogLevel() const
    {
        std::string logLevelString = findStringOptionValue(LOG_LEVEL_CONSOLE_);
        Log::LogLevel logLevel = Log::LogLevel::findLevel(logLevelString);
        return logLevel;
    }

    Log::LogLevel GlobalOptions::findFileLogLevel() const
    {
        std::string logLevelString = findStringOptionValue(LOG_LEVEL_FILE_);
        Log::LogLevel logLevel = Log::LogLevel::findLevel(logLevelString);
        return logLevel;
    }

    bool GlobalOptions::hasLogFile() const
    {
        return hasOptionValue(LOG_FILE_);
    }

    Log::LogFile GlobalOptions::findLogFile() const
    {
        assert(hasLogFile());
        std::string logFileString = findStringOptionValue(LOG_FILE_);
        Log::LogFile logFile;
        logFile.setPath(logFileString);
        return logFile;
    }

    std::string GlobalOptions::findAnalysisType() const
    {
        assert(hasOptionValue(ANALYSIS_), "No analysis specified!");
        return findStringOptionValue(ANALYSIS_);
    }


    std::string  GlobalOptions::computeAnalysisDescription() const
    {
        std::ostringstream ostringstream;
        ostringstream << "select analysis. Available Analyses:";
        const AnalysesFactoryMap* analysesFactoryMap = AnalysesFactoryMap::getInstance();
        ostringstream << analysesFactoryMap->getDescription();
        return ostringstream.str();
    }

    std::string GlobalOptions::computeLocalOptionsStringDescription() const
    {
        const AnalysesFactoryMap* anaList = AnalysesFactoryMap::getInstance();
        return anaList->getLocalOptionsStringDescriptions();
    }

    std::string GlobalOptions::computeHelpMessage() const
    {
        std::ostringstream oss;
        oss << getStringDescription();
        oss << computeLocalOptionsStringDescription();
        return oss.str();
    }

    bool GlobalOptions::isCircuitReadInOption() const
    {
    	return false;
    }

}


