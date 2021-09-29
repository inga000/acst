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

#ifndef CORE_NETSTOINSTANCESCONNECTIONS_H_
#define CORE_NETSTOINSTANCESCONNECTIONS_H_


#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"


namespace Core {

    class Net;
    class Circuit;
    class InstanceTerminal;

    class NetsToInstancesConnections
    {
    public:
        NetsToInstancesConnections(const Circuit & circuit);
        ~NetsToInstancesConnections();

        bool isDominatedDoubleNet(const Net & net) const;
        const Net & getDominatingNet(const Net & net) const;

        const Net & findOuterNet(NetId innerNetId) const;
        std::vector<const Net *> getOuterNets(NetId innerNetId) const;

        std::string print() const;
    
        bool isInitialized() const;

    private:
         void initilaizeMapOfAllNetsPointingToSameInnerInstanceNets(const Circuit & circuit);

        NetId createInnerNetNetId(const InstanceTerminal & instanceTerminal) const;

        bool allNetsSupplyNets(std::vector<const Net*> circuitNets) const;
        bool noNetSupplyNet(std::vector<const Net*> circuitNets) const;

        bool isFirstSupplyNetInVector(std::vector<const Net*> circuitNets, const Net & net) const;
        const Net & getFirstSupplyNetInVector(std::vector<const Net*> circuitNets) const;

        bool isInVector(std::vector<const Net*> circuitNets, const Net & net) const;


        bool isInitialized_;
        std::map<NetId, std::vector<const Net*> > innerInstanceNetToTopCircuitNets_;
    };
}


#endif
