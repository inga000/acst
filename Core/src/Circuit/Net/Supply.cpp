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

#include "Core/incl/Circuit/Net/Supply.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    const std::string Supply::STRING_NOT_INITIALIZED_ = "";
    const std::string Supply::STRING_NO_SUPPLY_ = "no_supply";
    const std::string Supply::STRING_GND_ = "gnd";
    const std::string Supply::STRING_VDD_ = "vdd";

    const int Supply::LEVEL_NOT_INITIALIZED_ = -2;
    const int Supply::LEVEL_NO_LEVEL_ = -1;
    const int Supply::LEVEL_LOWEST_ = 0;
    const int Supply::LEVEL_HIGHEST_ = 2;

    Supply::Supply() :
            string_(STRING_NOT_INITIALIZED_),
            level_(LEVEL_NOT_INITIALIZED_)
    {
    }

    Supply Supply::vdd(int level)
    {
        assert(isValidLevel(level));
        Supply vdd;
        vdd.setString(STRING_VDD_);
        vdd.setLevel(level);
        return vdd;
    }

    Supply Supply::vdd()
    {
      Supply vdd;
      vdd.setString(STRING_VDD_);
      vdd.setLevel(LEVEL_NO_LEVEL_);
      return vdd;
     }

    Supply Supply::gnd()
    {
        Supply gnd;
        gnd.setString(STRING_GND_);
        gnd.setLevel(LEVEL_NO_LEVEL_);
        return gnd;
    }

    Supply Supply::gnd(int level)
    {
    	assert(isValidLevel(level));
    	Supply gnd;
    	gnd.setString(STRING_GND_);
    	gnd.setLevel(level);
    	return gnd;
    }

    Supply Supply::noSupply()
    {
        Supply noSupply;
        noSupply.setString(STRING_NO_SUPPLY_);
        noSupply.setLevel(LEVEL_NO_LEVEL_);
        return noSupply;
    }

    bool Supply::isInitialized() const
    {
        return string_ != STRING_NOT_INITIALIZED_ &&
                level_ != LEVEL_NOT_INITIALIZED_;
    }

    bool Supply::isGnd() const
    {
        assert(isInitialized());
        return string_ == STRING_GND_;
    }

    bool Supply::isVdd() const
    {
        assert(isInitialized());
        return string_ == STRING_VDD_;
    }

    bool Supply::isSupply() const
    {
        assert(isInitialized());
        return string_ != STRING_NO_SUPPLY_;
    }

    bool Supply::isHigher(const Supply& other) const
    {
        if(isVdd() && other == gnd()) {
            return true;
        }
        else if (isVdd() && other.isVdd()) {
            return level_ > other.level_;
        }
        else {
            return false;
        }
    }

    bool Supply::isHigherToSameSupply(const Supply& other) const
    {
       if (isVdd() && other.isVdd())
       {
    	   return level_ > other.level_;
       }
       else if(isGnd() && other.isGnd())
       {
    	   return level_> other.level_;
       }
       else
       {
    	   return false;
        }
     }

    int Supply::getLevel() const
    {
        assert(hasLevel());
        return level_;
    }

    std::string Supply::getString() const
    {
    	assert(hasString());
    	return string_;
    }

    std::string Supply::toStr() const
    {
        std::ostringstream stream;
        stream << string_;
        if(level_ != LEVEL_NO_LEVEL_) {
            stream << "_" << level_;
        }
        return stream.str();
    }

    std::string Supply::supplyTypeToStr() const
    {
    	 std::ostringstream stream;
    	 stream << string_;
    	 return stream.str();
    }

    bool Supply::isSameSupplyType(const Supply & other) const
    {
    	assert(isInitialized());
    	return string_ == other.string_;
    }

    bool Supply::operator<(const Supply& other) const
    {
    	assert(isInitialized() && other.isInitialized());
    	if((isVdd() && other.isVdd()) || (isGnd() && other.isGnd())) {
    		return level_ < other.level_;
    	} else if(isGnd() && other.isVdd()) {
    		return true;
    	} else {
    		return false;
    	}
    }

    bool Supply::operator==(const Supply& other) const
    {
        assert(isInitialized());
        return string_ == other.string_ &&
                level_ == other.level_;
    }

    bool Supply::operator!=(const Supply& other) const
    {
        return !(*this == other);
    }

    void Supply::setString(const std::string& string)
    {
        assert(isValidString(string));
        string_ = string;
    }

    void Supply::setLevel(int level)
    {
        assert(isValidLevel(level));
        level_ = level;
    }


    bool Supply::isValidString(const std::string& string)
    {
        return (string == STRING_NO_SUPPLY_) ||
                (string == STRING_GND_)  ||
                (string == STRING_VDD_);
    }

    bool Supply::isValidLevel(int level)
    {
        return ( (level >= LEVEL_LOWEST_)  &&
                  (level <= LEVEL_HIGHEST_)    ) ||
                level == LEVEL_NO_LEVEL_;
    }

    bool Supply::hasLevel() const
    {
        return level_ != LEVEL_NO_LEVEL_;
    }

    bool Supply::hasString() const
    {
    	return string_ != STRING_NOT_INITIALIZED_;
    }

}


