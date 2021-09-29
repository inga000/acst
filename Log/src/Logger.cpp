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

#include "Log/incl/Logger.h"
#include "Log/incl/Detail/Deleter.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Log {

    Logger::Logger() :
            consoleStream_(&std::cout),
            initializedFlag_(false)
    {
    }

    Logger::~Logger()
    {
        if(hasLogFile() && getLogFile().isOpen()) {
            logFile_.close();
        }
    }

    Logger& Logger::getLogger()
    {
        static Deleter deleter;
        if(!deleter.hasLogger()) {
            Logger *  logger = new Logger();
            deleter.setLogger(logger);
        }
        return deleter.getLogger();
    }

    void Logger::setConsoleLevel(const LogLevel& logLevel)
    {
        consoleLevel_ = logLevel;
    }

    void Logger::setFileLevel(const LogLevel& logLevel)
    {
        fileLevel_ = logLevel;
    }

    void Logger::setLogFile(const LogFile& file)
    {
        logFile_ = file;
    }

    void Logger::initialize()
    {
        assert(!isInitialized());

        initializeSmallestLevel();
        if(hasLogFile()) {
            getLogFile().open();
        }
        initializedFlag_ = true;
    }

    bool Logger::isInitialized() const
    {
        return initializedFlag_;
    }

    bool Logger::isEnabled(const LogLevel & logLevel) const
    {
        assert(isInitialized());
        return getSmallestLevel() <= logLevel;
    }

    void Logger::log(const LogLevel& logLevel, const std::ostringstream& message)
    {
        assert(isInitialized());

        if(hasLogFile() && isFileEnabled(logLevel) ) {
            getLogFile().getFileStream() << message.str() << std::endl;
        }
        if(isConsoleEnabled(logLevel)) {
            getConsoleStream() << message.str() << std::endl;
        }
    }

    void Logger::initializeSmallestLevel()
    {
        if(hasFileLevel() && getFileLevel() < getConsoleLevel()) {
            smallestLevel_ = getFileLevel();
        } else {
            smallestLevel_ = getConsoleLevel();
        }
    }

    bool Logger::isFileEnabled(const LogLevel& logLevel) const
    {
        return getFileLevel() <= logLevel;
    }

    bool Logger::isConsoleEnabled(const LogLevel& logLevel) const
    {
        return getConsoleLevel() <= logLevel;
    }

    const LogLevel& Logger::getConsoleLevel() const
    {
        assert(hasConsoleLevel());
        return consoleLevel_;
    }

    const LogLevel& Logger::getFileLevel() const
    {
        assert(hasFileLevel());
        return fileLevel_;
    }

    const LogLevel& Logger::getSmallestLevel() const
    {
        assert(hasSmallestLevel());
        return smallestLevel_;
    }

    LogFile& Logger::getLogFile()
    {
        assert(hasLogFile());
        return logFile_;
    }

    std::ostream& Logger::getConsoleStream()
    {
        assert(hasConsoleStream());
        return *consoleStream_;
    }

    bool Logger::hasConsoleLevel() const
    {
        return consoleLevel_.isInitialized();
    }

    bool Logger::hasFileLevel() const
    {
        return fileLevel_.isInitialized();
    }

    bool Logger::hasLogFile() const
    {
        return logFile_.isInitialized();
    }

    bool Logger::hasSmallestLevel() const
    {
        return smallestLevel_.isInitialized();
    }

    bool Logger::hasConsoleStream() const
    {
        return consoleStream_ != NULL;
    }

}

