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


#ifndef CORE_PROPERTYMAP_H_
#define CORE_PROPERTYMAP_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyName.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyValue.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyAssignment.h"
#include <map>

namespace Core {

    class PropertyValue;
    class PropertyAssignment;

    class PropertyMap : public Object
    {
    private:
        typedef std::map<PropertyName, NetId> PropertyToNetIdentifierMap;
        typedef std::map<PropertyName, NetAssignment> PropertyToAssignmentMap;

    public:
        PropertyMap();

        bool hasValue(const PropertyName& propertyName) const;
        bool hasAssignment(const PropertyName& propertyName) const;

        NetId findValue(const PropertyName& propertyName) const;
        NetAssignment findAssignment(const PropertyName& propertyName) const;

        void addValue(const PropertyValue& propertyValue);
        void addValues(const std::vector<PropertyValue>& propertyValues);

        void addAssignment(const PropertyAssignment& propertyAssignment);
        void addAssignments(const std::vector<PropertyAssignment>& propertyAssignments);

        std::vector<PropertyValue> getPropertyValues() const;
        std::vector<PropertyAssignment> getPropertyAssignments() const;

        std::string toStr() const;

        bool operator ==(const PropertyMap& other) const;

    private:
        void printPropertyToIdentifierMap(std::ostream& stream) const;
        void printPropertyToAssignmentMap(std::ostream& stream) const;

        const PropertyToNetIdentifierMap& getPropertyToNetIdentifierMap() const;
        PropertyToNetIdentifierMap& getPropertyToNetIdentifierMap();

        const PropertyToAssignmentMap& getPropertyToAssignmentMap() const;
        PropertyToAssignmentMap& getPropertyToAssignmentMap();

    private:
        PropertyToNetIdentifierMap propertyToNetIdentifierMap_;
        PropertyToAssignmentMap propertyToAssignmentMap_;
    };
}


#endif
