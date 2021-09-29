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

#ifndef CORE_INSTANCE_H_
#define CORE_INSTANCE_H_

#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyName.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyMap.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Common/Object.h"
#include <ostream>

namespace Core {

    class Circuit;
    class InstanceTerminal;
    class TerminalId;

    class Instance : public Object
    {
        friend class InstanceTerminal;
    private:
        typedef std::map<TerminalId, InstanceTerminal*> InstanceTerminalMap;

    public:
        Instance();

        Instance & clone() const;

        void setCircuit(const Circuit & circuit);//Circit in which the instance is in
        void setMaster(const Circuit & circuit);//Inner circuit of the instance
        void setIdentifier(const InstanceId & identifier);
        void setPropertyMap(PropertyMap propertyMap);

        void addPropertyValue(const PropertyValue& propertyValue);
        void addPropertyValues(const std::vector<PropertyValue>& propertyValues);

        void addPropertyAssignment(const PropertyAssignment& propertyAssignment);
        void addPropertyAssignments(const std::vector<PropertyAssignment>& propertyAssignments);

        bool hasPropertyValue(const PropertyName& propertyName) const;
        bool hasPropertyAssignment(const PropertyName& propertyName) const;

        NetId findPropertyValue(const PropertyName& propertyName) const;
        NetAssignment findPropertyAssignment(const PropertyName& propertyName) const;

        InstanceTerminal & addInstanceTerminal(const TerminalId & terminalIdentifier);
        const InstanceTerminal & findInstanceTerminal(const TerminalId & terminalIdentifier) const;
        InstanceTerminal & findInstanceTerminal(const TerminalId & terminalIdentifier);
        InstanceTerminal & getInstanceTerminal(const TerminalId & terminalIdentifier) const;

        bool hasInstanceTerminal(const TerminalId & terminalIdentifier) const;

        std::vector<const InstanceTerminal*> getInstanceTerminals() const;
        std::vector<InstanceTerminal*> getInstanceTerminals();

        const Circuit & getMaster() const;

        const InstanceId & getIdentifier() const;
        InstanceId getInstanceIdentifier() const;

        std::string toStr() const;
        void print(std::ostream& stream) const;

        const Circuit & getCircuit() const;

    private:

        void printInstanceTerminals(std::ostream& stream) const;
        void printProperties(std::ostream& stream) const;

        InstanceTerminalMap& getInstanceTerminalMap();
        const InstanceTerminalMap& getInstanceTerminalMap() const;
        const PropertyMap& getPropertyMap() const;
        PropertyMap& getPropertyMap();

        bool hasCircuit() const;
        bool hasMaster() const;
        bool hasIdentifier() const;

    private:
        const Circuit * circuit;
        const Circuit * master;
        InstanceId identifier;
        InstanceTerminalMap instanceTerminals;
        PropertyMap propertyMap;
    };
}

#endif
