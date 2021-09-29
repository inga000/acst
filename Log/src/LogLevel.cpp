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

#include "Log/incl/LogLevel.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Log {

    const std::string LogLevel::LEVELSTRING_TRACE_ = "TRACE";
    const std::string LogLevel::LEVELSTRING_DEBUG_ = "DEBUG";
    const std::string LogLevel::LEVELSTRING_WARNING_ = "WARNING";
    const std::string LogLevel::LEVELSTRING_INFO_ = "INFO";
    const std::string LogLevel::LEVELSTRING_OFF_ = "OFF";

    LogLevel::LogLevel() :
            level_(LEVEL_NOT_INTIALIZED)
    {
    }

    LogLevel LogLevel::trace()
    {
        LogLevel trace;
        trace.setLevel(LEVEL_TRACE);
        return trace;
    }

    LogLevel LogLevel::debug()
    {
        LogLevel debug;
        debug.setLevel(LEVEL_DEBUG);
        return debug;
    }

    LogLevel LogLevel::warning()
    {
        LogLevel warning;
        warning.setLevel(LEVEL_WARNING);
        return warning;
    }

    LogLevel LogLevel::info()
    {
        LogLevel info;
        info.setLevel(LEVEL_INFO);
        return info;
    }

    LogLevel LogLevel::off()
    {
        LogLevel off;
        off.setLevel(LEVEL_OFF);
        return off;
    }

    bool LogLevel::isInitialized() const
    {
        return level_ != LEVEL_NOT_INTIALIZED;
    }

    bool LogLevel::operator ==(const LogLevel& other) const
    {
        assert(isInitialized());
        return level_ == other.level_;
    }

    bool LogLevel::operator !=(const LogLevel& other) const
    {
        return !(*this == other);
    }

    bool LogLevel::operator <(const LogLevel& other) const
    {
        return level_ < other.level_;
    }

    bool LogLevel::operator >(const LogLevel& other) const
    {
        return !(*this == other) && !(*this < other);
    }


    bool LogLevel::operator <=(const LogLevel& other) const
    {
        return (*this == other) || (*this < other);
    }

    bool LogLevel::operator >=(const LogLevel& other) const
    {
        return (*this == other) || (*this > other);
    }

    void LogLevel::setLevel(enum Level level)
    {
        level_ = level;
    }

    LogLevel LogLevel::findLevel(const std::string& string)
    {
        assert(isValidLevelString(string));
        LogLevel level = getStringToLevelMap().at(string);
        return level;
    }

    const LogLevel::StringToLevelMap& LogLevel::getStringToLevelMap()
    {
        static StringToLevelMap stringToLevelMap;
        if(stringToLevelMap.empty()) {
            stringToLevelMap[LEVELSTRING_TRACE_] = LogLevel::trace();
            stringToLevelMap[LEVELSTRING_DEBUG_] = LogLevel::debug();
            stringToLevelMap[LEVELSTRING_WARNING_] = LogLevel::warning();
            stringToLevelMap[LEVELSTRING_INFO_] = LogLevel::info();
            stringToLevelMap[LEVELSTRING_OFF_] = LogLevel::off();
        }

        return stringToLevelMap;
    }

    bool LogLevel::isValidLevelString(const std::string& string)
    {
        return getStringToLevelMap().find(string) != getStringToLevelMap().end();
    }

}

