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

#include "HSpice/incl/InputFile/NetIds/NetIds.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "Core/incl/Circuit/Circuit.h"

namespace HSpice {
    namespace InputFile {

    NetIds::NetIds():
                globalNetMap_(NULL)
        {
        }

    NetIds::~NetIds()
        {
        }

        void NetIds::addNetId(const Core::NetId& netId)
        {
            netlistIds_.insert(netId);
        }

        void NetIds::addNetIds(const Core::NetIdItems& netIdItems)
        {
            for(Core::NetIdItems::const_iterator it = netIdItems.begin(); it != netIdItems.end(); it++)
            {
                netlistIds_.insert(*it);
            }
        }

        void NetIds::addNetsToCircuit(Core::Circuit& circuit) const
        {
            for(std::set<Core::NetId>::const_iterator it = netlistIds_.begin(); it != netlistIds_.end(); it++)
            {
                Core::NetId netId = *it;
                if(!getGlobalNetMap().hasNet(netId) && !circuit.hasNet(netId)) {
                    circuit.addNet(*it);
                }
            }
        }

        void NetIds::setGlobalNetMap(const GlobalNetMap& globalNetMap)
        {
            globalNetMap_ = &globalNetMap;
        }

        const GlobalNetMap& NetIds::getGlobalNetMap() const
        {
            assert(globalNetMap_ != NULL);
            return *globalNetMap_;
        }

    }
}



