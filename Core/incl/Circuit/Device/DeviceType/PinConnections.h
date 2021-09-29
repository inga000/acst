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


#ifndef CORE_PINCONNECTIONS_H_
#define CORE_PINCONNECTIONS_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include <map>
#include <set>

namespace Core {

    class Net;
    class Device;

    class PinConnections : public Object
    {
    private:
        typedef std::set<PinType> PinTypeSet;
        typedef std::map<PinType, Net*> NetMap;
        typedef std::map<PinType, NetAssignment> NetAssignmentMap;
        typedef std::map<PinType, NetId> GlobalNetIdMap;

    public:
        PinConnections();

        void add(const PinType& pinType);
        void add(const std::vector<PinType>& pinTypes);

        void addNet(const PinType& pinType, Net& net);
        void addNetAssignment(const PinType& pinType, const NetAssignment& netAssignment);

        bool hasExcatlyOneConnectionPerPin() const;

        void addAndConnectPins(Device& component) const;

        std::string toStr() const;

    private:
        bool hasPinType(const PinType& pinType) const;

        bool hasNet(const PinType& pinType) const;
        Net & findNet(const PinType& pinType) const;

        bool hasNetAssignment(const PinType& pinType) const;
        NetAssignment findNetAssignment(const PinType& pinType) const;

        bool hasExactlyOneConnection(const PinType& pinType) const;

    private:
        PinTypeSet pinTypeSet_;
        NetMap netMap_;
        NetAssignmentMap netAssignmentMap_;
    };
}
#endif
