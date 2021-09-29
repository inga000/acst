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

#ifndef LOG_LOGLEVEL_H_
#define LOG_LOGLEVEL_H_

#include <string>
#include <map>

namespace Log {

    class LogLevel
    {
    private:
        enum Level {
            LEVEL_NOT_INTIALIZED,
            LEVEL_TRACE,
            LEVEL_DEBUG,
            LEVEL_WARNING,
            LEVEL_INFO,
            LEVEL_OFF
        };

    public:
        LogLevel();

        static LogLevel trace();
        static LogLevel debug();
        static LogLevel warning();
        static LogLevel info();
        static LogLevel off();
        static LogLevel findLevel(const std::string & string);

        bool isInitialized() const;

        bool operator==(const LogLevel& other) const;
        bool operator!=(const LogLevel& other) const;
        bool operator<(const LogLevel& other) const;
        bool operator>(const LogLevel& other) const;
        bool operator<=(const LogLevel& other) const;
        bool operator>=(const LogLevel& other) const;

    private:
        typedef std::map<std::string, LogLevel> StringToLevelMap;

        static const std::string LEVELSTRING_TRACE_;
        static const std::string LEVELSTRING_DEBUG_;
        static const std::string LEVELSTRING_WARNING_;
        static const std::string LEVELSTRING_INFO_;
        static const std::string LEVELSTRING_OFF_;

        void setLevel(enum Level level);

        static const StringToLevelMap & getStringToLevelMap();
        static bool isValidLevelString(const std::string & string);


        enum Level level_;
    };

}

#endif
