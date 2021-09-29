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


#ifndef CORE_NET_H_
#define CORE_NET_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Net/Supply.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include <map>

namespace Core {

	class Pin;
    class Device;
    class InstanceTerminal;

    class Net: public Object
    {
    private:
        typedef std::multimap<PinType, Pin*> PinTypeToPinMap;
        typedef std::pair<PinTypeToPinMap::iterator,PinTypeToPinMap::iterator> PinRange;
        typedef std::pair<PinTypeToPinMap::const_iterator,PinTypeToPinMap::const_iterator> PinConstRange;
    public:
        Net();
        ~Net();

        Net & clone() const;

        void setNetId(const NetId & netId);
        void setGlobal(bool global);
        void setSupply(const Supply & supply);
        void setNetAssignment(const NetAssignment& netAssignment);

        void addPin(Pin & pin);
        void removePin(const Pin & pin);

        void addInstanceTerminal(InstanceTerminal & instanceTerminal);

        bool isGlobal() const;
        bool isSupply() const;
        bool isVddSupply() const;
        bool isGndSupply() const;

        bool hasPins() const;
        bool hasGatePin() const;
        bool hasDrainPin() const;
        bool hasAssignment() const;
        bool hasInstanceTerminals() const;

        std::vector<InstanceTerminal*> getInstanceTerminals() const;
        std::vector<Pin *> getPins(PinType pinType);
        std::vector<const Pin *> getPins(PinType pinType) const;
        bool hasPin(PinType pinType) const;

        NetId getIdentifier() const;
        NetAssignment getNetAssignment() const;

        Supply getSupply() const;

        void mergePins(Net& other);

        bool connectsGatesOfTwoTransistorsWithSameSourceNet() const;
        bool isOnlyConnectedToPmosTransistors() const;
        bool isOnlyConnectedToNmosTransistors() const;
        bool connectedDevicesConnectedToASourceNet() const;
    public:

        std::string toStr() const;
        void print(std::ostream & stream) const;

        bool operator<(const Net & other) const;
        bool operator==(const Net & other) const;
        bool operator>(const Net & other) const;
        bool operator!=(const Net & other) const;
    private:
        Net(const Net& other); // don't implement
        void operator=(const Net& other); // don't implement

    private:
        bool hasNetId() const;
        bool hasCircuit() const;
        bool hasPin(const Pin & pin) const;
        bool hasSupply() const;

        std::vector<const Pin *> getPins() const;

        PinRange getPinIndexRange(const Pin & pin);
        PinConstRange getPinIndexRange(const Pin & pin) const;
        bool pinIsInRange(const Pin & pin, const PinConstRange & range) const;
        PinTypeToPinMap::iterator findPinInRange(const Pin & pin, const PinRange & range);

    private:
        bool globalFlag_;
        NetId netId_;
        NetAssignment netAssignment_;
        Supply supply_;
        PinTypeToPinMap pins_;
        std::vector<InstanceTerminal*> instanceTerminals_;
    };
}
#endif
