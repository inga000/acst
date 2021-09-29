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

#ifndef CORE_DEVICE_H_
#define CORE_DEVICE_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/CellTripleId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyMap.h"
#include <string>
#include <ostream>

namespace Core {

    class Core;
    class PinType;
    class Pin;
    class Net;
    class Circuit;

    class Device: public Object
    {
    private:
        typedef std::map<PinType, Pin*> PinMap;

    public:
        Device();
        ~Device();

        Device & clone() const;

        void setCellTripleId(const CellTripleId& cellTripleId);
        void setDeviceId(const DeviceId & componentId);
        void setDeviceType(const DeviceType & componentType);
        void setTechType(const TechType & techType);
        Pin & addPin(const PinType & pinType);

        void addValue(const PropertyValue& propertyValue);
        void addValues(const std::vector<PropertyValue>& propertyValues);

        void addAssignment(const PropertyAssignment& propertyAssignment);
        void addAssignments(const std::vector<PropertyAssignment>& propertyAssignments);

        CellTripleId getCellTripleId() const;
        DeviceId getIdentifier() const;
        const DeviceType & getDeviceType() const;
        TechType getTechType() const;

        const Pin & findPin(const PinType & pinType) const;
        const Net & findNet(const PinType & pinType) const;
        Net & findNet(const PinType & pinType);
        std::vector<const Pin*> findAllPins() const;

        NetId findValue(const PropertyName& propertyName) const;
        NetAssignment findAssignment(const PropertyName& propertyName) const;

        bool isConnected(const NetId& netId) const;
        bool hasValue(const PropertyName& propertyName) const;
        bool hasAssignment(const PropertyName& propertyName) const;
        bool hasCellTripleId() const;
        bool hasComponentId() const;

        void print(std::ostream & stream, const int & column) const;
        std::string toStr() const;

        bool operator==(const Device & other) const;
        bool operator!=(const Device & other) const;
        bool operator<(const Device & other) const;

    private:
        void setPropertyMap(PropertyMap propertyMap);
        bool hasComponentType() const;
        bool hasTechType() const;

        bool hasPin(const PinType & pinType) const;
        Pin & findPin(const PinType & pinType);

        static std::string spaces(int columns);

    private:
        Device(const Device& other); //do not implement!
        void operator=(const Device& other); //do not implement!

    private:
        CellTripleId cellTripleId_;
        DeviceId deviceId_;

        const DeviceType * deviceType_;
        TechType techType_;

        PinMap pins_;
        PropertyMap propertyMap_;
    };
}

#endif
