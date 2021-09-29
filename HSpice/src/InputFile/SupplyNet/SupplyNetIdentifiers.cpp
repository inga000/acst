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

#include "HSpice/incl/InputFile/SupplyNet/SupplyNetIdentifiers.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"

namespace HSpice {
    namespace InputFile {

        SupplyNetIdentifiers::SupplyNetIdentifiers():
            initializedFlag_(false)
        {
        }

        void SupplyNetIdentifiers::initializeEmpty()
        {
            initializedFlag_ = true;
        }

        bool SupplyNetIdentifiers::isInitialized() const
        {
            return initializedFlag_;
        }

        void SupplyNetIdentifiers::add(const Core::NetId& netId, const Core::Supply& supply)
        {
            assert(!has(netId));
            getNetIdentifierSupplyMap().insert(NetIdentifierSupplyMap::value_type(netId, supply));
        }

        Core::Supply SupplyNetIdentifiers::findSupply(const Core::NetId& netId) const
        {
            Core::Supply supply = Core::Supply::noSupply();

            if(has(netId)) {
                NetIdentifierSupplyMap supplyMap = getNetIdentifierSupplyMap();
                supply = supplyMap.find(netId)->second;
            }

            return supply;
        }

        bool SupplyNetIdentifiers::has(const Core::NetId& netId) const
        {
            NetIdentifierSupplyMap supplyMap = getNetIdentifierSupplyMap();
            return supplyMap.find(netId) != supplyMap.end();
        }

        SupplyNetIdentifiers::NetIdentifierSupplyMap& SupplyNetIdentifiers::getNetIdentifierSupplyMap()
        {
            assert(isInitialized());
            return netIdentifierSupplyMap_;
        }

        const SupplyNetIdentifiers::NetIdentifierSupplyMap& SupplyNetIdentifiers::getNetIdentifierSupplyMap() const
        {
            assert(isInitialized());
            return netIdentifierSupplyMap_;
        }

        std::string SupplyNetIdentifiers::toStr() const
        {
        	std::ostringstream stream;
        	stream << "SupplyNetIdentifierMap: " << std::endl;
        	for(auto & nets : getNetIdentifierSupplyMap())
        	{
        		stream << nets.first << std::endl;
        	}

        	return stream.str();
        }

    }
}
