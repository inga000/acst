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

#ifndef CORE_CIRCUITSTONETSTOINSTANCESCONNECTIONSMAP_H_
#define CORE_CIRCUITSTONETSTOINSTANCESCONNECTIONSMAP_H_


#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"

namespace Core {

    class Net;
    class Circuit;
    class InstanceTerminal;
    class NetsToInstancesConnections;

    class CircuitsToNetsToInstancesConnectionsMap
    {
    public:
        CircuitsToNetsToInstancesConnectionsMap(const Circuit & circuit);
        ~CircuitsToNetsToInstancesConnectionsMap();

        void addShortenedNet(NetId longNet, NetId shortNet);
        bool hasEquivalentShortNet(NetId longNet) const;
        NetId getCorrespondingShortNetId(NetId longNet) const;
        

        bool isDominatedDoubleNet(const Circuit & circuit, const Net & net) const;
        const Net & getDominatingNet(const Circuit & circuit, const Net & net) const;

        std::string print() const;

        std::string printShortenedNets() const;
    
        bool isInitialized() const;

    private:
        void initialize(const Circuit & circuit);
        void addCircuitToNetsToInstancesConnectionMap(const Circuit & circuit);
        void eraseNetsToInstancesConnectionMap();

         NetId createInnerNetId(NetId netId , InstanceName instanceName) const;

        const NetsToInstancesConnections & getNetsToInstancesConnections(const Circuit & circuit) const;
        std::set<const Net * > getIdenticalNets(const Circuit & circuit, const Net & net) const;

        bool allAreTheSame(std::vector<const Net*> nets) const;
        bool allNetsSupplyNets(std::set<const Net*> nets) const;
        bool noNetSupplyNet(std::set<const Net*> nets) const;
        bool isFirstSupplyNetInVector(std::set<const Net*> nets, const Net & net) const;

        const Net & getFirstSupplyNetInVector(std::set<const Net*> nets) const;
        
        std::map<CircuitId, const NetsToInstancesConnections*> circuitToNetsToInstancesConnectionsMap_;
        std::map<NetId, NetId> netToShortenedNetMap_;
    };
}

#endif
