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


#ifndef CORE_SUPPLY_H_
#define CORE_SUPPLY_H_

#include "Core/incl/Common/Object.h"
#include <string>

namespace Core {

        class Supply : public Object
        {
        public:
            Supply();

            static Supply vdd(int level);
            static Supply vdd();
            static Supply gnd(int level);
            static Supply gnd();
            static Supply noSupply();

            bool isInitialized() const;

            bool isGnd() const;
            bool isVdd() const;
            bool isSupply() const;
            bool isHigher(const Supply & other) const;
            bool isHigherToSameSupply(const Supply & other) const;
            bool isSameSupplyType(const Supply & other) const;
            int getLevel() const;
            std::string getString() const;

            std::string toStr() const;
            std::string supplyTypeToStr() const;

            bool operator<(const Supply& other) const;
            bool operator==(const Supply & other) const;
            bool operator!=(const Supply & other) const;

            void setString(const std::string & string);
            void setLevel(int level);

        private:
            static const std::string STRING_NOT_INITIALIZED_;
            static const std::string STRING_NO_SUPPLY_;
            static const std::string STRING_GND_;
            static const std::string STRING_VDD_;

            static const int LEVEL_NOT_INITIALIZED_;
            static const int LEVEL_NO_LEVEL_;
            static const int LEVEL_LOWEST_;
            static const int LEVEL_HIGHEST_;


            static bool isValidString(const std::string & level);
            static bool isValidLevel(int level);
            bool hasLevel() const;
            bool hasString() const;

            std::string string_;
            int level_;
        };
}


#endif
