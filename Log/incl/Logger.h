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

#ifndef LOG_LOGGER_H_
#define LOG_LOGGER_H_

#include "Log/incl/LogLevel.h"
#include "Log/incl/LogFile.h"
#include <ostream>
#include <sstream>
#include <iostream>

namespace Log {

    class Logger
    {
    public:
        ~Logger();

        static Logger & getLogger();

        void setConsoleLevel(const LogLevel & logLevel);
        void setFileLevel(const LogLevel & logLevel);
        void setLogFile(const LogFile & file);

        void initialize();

        bool isInitialized() const;
        bool isEnabled(const LogLevel & logLevel) const;

        void log(const LogLevel & logLevel, const std::ostringstream & message);

    private:
        Logger();
        Logger(const Logger & other); // don't implement
        void operator=(const Logger & other); // don't implement

        void initializeSmallestLevel();

        bool isFileEnabled(const LogLevel & logLevel) const;
        bool isConsoleEnabled(const LogLevel & logLevel) const;

        const LogLevel & getConsoleLevel() const;
        const LogLevel & getFileLevel() const;
        const LogLevel & getSmallestLevel() const;
        LogFile & getLogFile();
        std::ostream & getConsoleStream();

        bool hasConsoleLevel() const;
        bool hasFileLevel() const;
        bool hasSmallestLevel() const;
        bool hasLogFile() const;
        bool hasConsoleStream() const;

        bool initializedFlag_;

        LogLevel consoleLevel_;
        LogLevel fileLevel_;
        LogLevel smallestLevel_;

        LogFile logFile_;
        std::ostream * consoleStream_;
    };

}


#endif
