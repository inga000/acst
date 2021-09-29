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


#include "Control/incl/Control.h"
#include "Control/incl/FactoryMap/AnalysesFactoryMap.h"
#include "Control/incl/Options/OptionsBase.h"
#include "Control/incl/Options/CircuitReadInOptions.h"
#include "Control/incl/Analysis/AnalysisFactory.h"
#include "Control/incl/Analysis/CircuitAnalysis.h"
#include "Control/incl/File/OutputFile.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Control/incl/AbstractCircuitSource.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/Logger.h"

#include "Log/incl/LogMacros.h"

namespace Control {


    Control::Control()
    {
    }

    Control::~Control()
    {
    }

    void Control::run(int argc, char *argv[])
    {
        globalOptions_.parse(argc,argv);
        initializeLogger();


        if(globalOptions_.hasHelp())
        {
            globalOptions_.printHelp(std::cout);
            return;
        }
        else {
            const OptionsBase & localOptions = parseLocalOptions(argc, argv);
            logDebug("Option description: ");
            logDebug(localOptions.getStringDescription());

            AbstractAnalysis* analysis = createNewAnalysis(localOptions);


            analysis->compute();
            analysis->write();
            delete analysis; // TODO: What happens here sometimes

        }
    }

    void Control::initializeLogger() const
    {
        Log::LogLevel consoleLogLevel = globalOptions_.findConsoleLogLevel();
        Log::Logger::getLogger().setConsoleLevel(consoleLogLevel);

        if(globalOptions_.hasLogFile()) {
            Log::LogLevel fileLogLevel = globalOptions_.findFileLogLevel();
            Log::LogFile logFile = globalOptions_.findLogFile();
            Log::Logger::getLogger().setFileLevel(fileLogLevel);
            Log::Logger::getLogger().setLogFile(logFile);
        }
        Log::Logger::getLogger().initialize();
    }

    const OptionsBase & Control::parseLocalOptions(int argc, char *argv[]) const
    {
        logDebug( "Parsing Local Options...");
        std::string analysisType = globalOptions_.findAnalysisType();
        OptionsBase & localOptions = findLocalOptions(analysisType);
        localOptions.parse(argc, argv);

        return localOptions;
    }

    AbstractAnalysis* Control::createNewAnalysis(const OptionsBase & localOptions) const
    {
        logDebug( "Setting up Analysis...");
        std::string analysisType = globalOptions_.findAnalysisType();
        AbstractAnalysis* analysis = createNewAnalysis(analysisType);

        analysis->setLocalOptions(localOptions);
        analysis->initialize();



        return analysis;
    }

    AbstractAnalysis* Control::createNewAnalysis(const std::string & analysisType) const
    {
        const AnalysisFactory & analysisFactory = findAnalysisFactory(analysisType);
        AbstractAnalysis* analysis = analysisFactory.create();
        return analysis;
    }

    OptionsBase & Control::findLocalOptions(const std::string & analysisType) const
    {
        AnalysisFactory & analysisFactory = findAnalysisFactory(analysisType);
        OptionsBase & localOptions = analysisFactory.getLocalOptions();
        return localOptions;
    }

    AnalysisFactory & Control::findAnalysisFactory(const std::string& analysisType)
    {
        AnalysesFactoryMap* analysisFactoryMap = AnalysesFactoryMap::getInstance();
        AnalysisFactory* analysisFactory = analysisFactoryMap->find(FactoryMapKey(analysisType));
        return *analysisFactory;
    }

}
