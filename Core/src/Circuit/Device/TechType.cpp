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

#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    const std::string TechType::UNDEFINED_STRING_ = "undefined";
    const std::string TechType::N_STRING_ = "n";
    const std::string TechType::P_STRING_ = "p";

    TechType::TechType() :
            techTypeEnum(TECHTYPE_UNINITIALIZED)
    {
    }

    TechType TechType::createFromString(const std::string string)
    {
       TechType techType;
       techType.techTypeEnum = mapStringToEnum(string);
       return techType;
    }

    TechType TechType::n()
    {
        TechType techType;
        techType.techTypeEnum = TECHTYPE_N;
        return techType;
    }

    TechType TechType::p()
    {
        TechType techType;
        techType.techTypeEnum = TECHTYPE_P;
        return techType;
    }

    TechType TechType::undefined()
    {
        TechType techType;
        techType.techTypeEnum = TECHTYPE_UNDEFINED;
        return techType;
    }

    bool TechType::operator==(const TechType & other) const
    {
        assert(isInitialized());
        return techTypeEnum == other.techTypeEnum;
    }

    bool TechType::operator!=(const TechType & other) const
    {
        assert(isInitialized());
        return techTypeEnum != other.techTypeEnum;
    }

    bool TechType::operator<(const TechType & other) const
    {
        assert(isInitialized());
        return techTypeEnum < other.techTypeEnum;
    }

    bool TechType::isValidString(const std::string & str)
    {
        return getStringToEnumMap().find(str) != getStringToEnumMap().end();
    }

    std::string TechType::toStr() const
    {
        assert(isInitialized());
        return mapEnumToString(techTypeEnum);
    }

    TechType::TechTypeEnum TechType::mapStringToEnum(const std::string & str)
    {
        assert(isValidString(str));
        return getStringToEnumMap().at(str);
    }

    bool TechType::isInitialized() const
    {
        return techTypeEnum != TECHTYPE_UNINITIALIZED;
    }

    bool TechType::isN() const
    {
        assert(isInitialized());
        return techTypeEnum == TECHTYPE_N;
    }

    bool TechType::isP() const
    {
        assert(isInitialized());
        return techTypeEnum == TECHTYPE_P;
    }

    bool TechType::isUndefined() const
    {
        assert(isInitialized());
        return techTypeEnum == TECHTYPE_UNDEFINED;
    }

    std::string TechType::mapEnumToString(const TechTypeEnum & tt)
    {
        return getEnumToStringMap().at(tt);
    }

    const TechType::StringToEnumMap & TechType::getStringToEnumMap()
    {
        static StringToEnumMap theMap;

        if(theMap.empty()) {
            theMap[UNDEFINED_STRING_] = TECHTYPE_UNDEFINED;
            theMap[N_STRING_] = TECHTYPE_N;
            theMap[P_STRING_] = TECHTYPE_P;
        }
        return theMap;
    }

    const TechType::EnumToStringMap & TechType::getEnumToStringMap()
    {
        static EnumToStringMap theMap;

        if(theMap.empty()) {
            theMap[TECHTYPE_UNDEFINED] = UNDEFINED_STRING_;
            theMap[TECHTYPE_N] = N_STRING_;
            theMap[TECHTYPE_P] = P_STRING_;
        }
        return theMap;
    }
}
