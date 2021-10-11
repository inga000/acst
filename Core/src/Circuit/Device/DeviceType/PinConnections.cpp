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

#include "Core/incl/Circuit/Device/DeviceType/PinConnections.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    PinConnections::PinConnections()
    {
    }

    void PinConnections::add(const PinType& pinType)
    {
        pinTypeSet_.insert(pinType);
    }

    void PinConnections::add(const std::vector<PinType>& pinTypes)
    {
    	pinTypeSet_.insert(pinTypes.begin(), pinTypes.end());
    }

    void PinConnections::addNet(const PinType & pinType, Net& net)
    {
        assert(hasPinType(pinType));
        netMap_[pinType] = &net;
    }

    void PinConnections::addNetAssignment(const PinType & pinType, const NetAssignment & netAssignment)
    {
        assert(hasPinType(pinType));
        netAssignmentMap_[pinType] = netAssignment;
    }

    bool PinConnections::hasPinType(const PinType & pinType) const
    {
        return pinTypeSet_.find(pinType) != pinTypeSet_.end();
    }

    bool PinConnections::hasNet(const PinType & pinType) const
    {
        assert(hasPinType(pinType));
        return netMap_.find(pinType) != netMap_.end();
    }

    Net& PinConnections::findNet(const PinType & pinType) const
    {
        assert(hasNet(pinType));
        Net * net = netMap_.at(pinType);
        assert(net != NULL);
        return *net;
    }

    bool PinConnections::hasNetAssignment(const PinType & pinType) const
    {
        assert(hasPinType(pinType));
        return netAssignmentMap_.find(pinType) != netAssignmentMap_.end();
    }

    NetAssignment PinConnections::findNetAssignment(const PinType & pinType) const
    {
        assert(hasNetAssignment(pinType));
        return netAssignmentMap_.at(pinType);
    }

    std::string PinConnections::toStr() const
    {
        std::ostringstream oss;
        oss << "PinToNetConnections:" << netMap_;
        return oss.str();
    }

    bool PinConnections::hasExcatlyOneConnectionPerPin() const
    {
        bool connectionsCompleteAndCorrect = true;
        for(PinTypeSet::const_iterator it = pinTypeSet_.begin(); it != pinTypeSet_.end(); it++)
        {
            const PinType & pinType = *it;
            bool hasExactlyOneConnectionFlag = hasExactlyOneConnection(pinType);
            bool pinIsOptionalFlag = pinType.isOptional();
            if (!hasExactlyOneConnectionFlag && !pinIsOptionalFlag) {
                connectionsCompleteAndCorrect = false;
                break;
            }
        }
        return connectionsCompleteAndCorrect;
    }

    void PinConnections::addAndConnectPins(Device& device) const
    {
        assert(hasExcatlyOneConnectionPerPin());
        for(PinTypeSet::const_iterator it = pinTypeSet_.begin(); it != pinTypeSet_.end(); it++)
        {
            const PinType & pinType = *it;
            Pin & newPin = device.addPin(pinType);

            if(hasNet(pinType)) {
                Net & net = findNet(pinType);
                newPin.connect(net);
            }
            else if(hasNetAssignment(pinType)) {
                NetAssignment netAssignment = findNetAssignment(pinType);
                newPin.setNetAssignment(netAssignment);
            }
            else if(pinType.isOptional() && pinType.hasAutoConnection())
            {
            	PinType autoConnection = pinType.getAutoConnection();
            	if(hasNet(autoConnection)) {
					Net & net = findNet(autoConnection);
					newPin.connect(net);
				}
				else if(hasNetAssignment(autoConnection)) {
					NetAssignment netAssignment = findNetAssignment(autoConnection);
					newPin.setNetAssignment(netAssignment);
				}
            } else {
                assert(false, "Nothing works for that pin!");
            }
        }
    }

    bool PinConnections::hasExactlyOneConnection(const PinType& pinType) const
    {
        return ( hasNet(pinType) && !hasNetAssignment(pinType) ) ||
               ( !hasNet(pinType) && hasNetAssignment(pinType) ) ||
               ( !hasNet(pinType) && !hasNetAssignment(pinType) );
    }
}
