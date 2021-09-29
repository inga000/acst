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

#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetIdentifiers.h"
#include "Core/incl/Circuit/Net/Supply.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"

namespace HSpice {
    namespace InputFile {

        const std::string SupplyNetFile::GND_TOKEN_ = "GND";
        const std::string SupplyNetFile::VDD_TOKEN_ = "VDD";
        const int SupplyNetFile::MAX_VDD_LEVELS_ = 2;
        const int SupplyNetFile::MAX_GND_LEVELS_ = 2;
        const int SupplyNetFile::NETIDENTIFIER_COLUMN_ = 1;

        SupplyNetFile::SupplyNetFile():
                supplyNetIdentifiers_(NULL)
        {
        }

        void SupplyNetFile::setSupplyNetIdentifier(SupplyNetIdentifiers& supplyNetIdentifiers)
        {
            supplyNetIdentifiers_ = &supplyNetIdentifiers;
        }

        void SupplyNetFile::parse()
        {
            for(int i = 0; i <= MAX_GND_LEVELS_; i++)
            {
              std::string tokenName = computeTokenString(GND_TOKEN_,i);
              parseToken(tokenName, Core::Supply::gnd(i));
            }

            for(int i = 0; i <= MAX_VDD_LEVELS_; i++)
            {
               std::string tokenName = computeTokenString(VDD_TOKEN_,i);
               parseToken(tokenName, Core::Supply::vdd(i));
            }
          }

        void SupplyNetFile::parseToken(const std::string& tokenName, const Core::Supply& supply)
        {
            std::vector<std::string> netIdentifierTokens = readToken(tokenName, NETIDENTIFIER_COLUMN_);

            for(auto& it : netIdentifierTokens)
            {
                std::string netName = it;
                Core::NetId netIdentifier = Core::NetName(netName).createRootIdentifier();
                getSupplyNetIdentifiers().add(netIdentifier, supply);
            }
        }

        std::string SupplyNetFile::computeTokenString(const std::string& tokenName, const int level) const
        {
            std::ostringstream oss;
            oss << tokenName << "_" << level;
            return oss.str();
        }

        SupplyNetIdentifiers& SupplyNetFile::getSupplyNetIdentifiers()
        {
            assert(supplyNetIdentifiers_ != NULL);
            return *supplyNetIdentifiers_;
        }

    }
}
