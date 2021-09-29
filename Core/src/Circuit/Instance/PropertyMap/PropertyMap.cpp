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

#include "Core/incl/Circuit/Instance/PropertyMap/PropertyMap.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    PropertyMap::PropertyMap()
    {
    }

    bool PropertyMap::hasValue(const PropertyName& propertyName) const
    {
        return getPropertyToNetIdentifierMap().find(propertyName) != getPropertyToNetIdentifierMap().end();
    }

    bool PropertyMap::hasAssignment(const PropertyName& propertyName) const
    {
        return getPropertyToAssignmentMap().find(propertyName) != getPropertyToAssignmentMap().end();
    }

    NetId PropertyMap::findValue(const PropertyName& propertyName) const
    {
        assert(hasValue(propertyName));
        NetId requestedNetId = getPropertyToNetIdentifierMap().find(propertyName)->second;
        return requestedNetId;
    }

    NetAssignment PropertyMap::findAssignment(const PropertyName& propertyName) const
    {
        assert(hasAssignment(propertyName));
        NetAssignment requestedNetAssignment = getPropertyToAssignmentMap().find(propertyName)->second;
        return requestedNetAssignment;
    }

    void PropertyMap::addValue(const PropertyValue& propertyValue)
    {
        PropertyName propertyName = propertyValue.getPropertyName();
        NetId netId = propertyValue.getNetIdentifier();
        assert(!hasValue(propertyName) && !hasAssignment(propertyName));
        getPropertyToNetIdentifierMap()[propertyName] = netId;
    }

    void PropertyMap::addValues(const std::vector<PropertyValue>& propertyValues)
    {
    	for(auto& it : propertyValues)
    	{
    		addValue(it);
    	}
    }

    void PropertyMap::addAssignment(const PropertyAssignment& propertyAssignment)
    {
        PropertyName propertyName = propertyAssignment.getPropertyName();
        NetAssignment netAssignment = propertyAssignment.getNetAssignment();
        assert(!hasValue(propertyName) && !hasAssignment(propertyName));
        getPropertyToAssignmentMap()[propertyName] = netAssignment;
    }

    void PropertyMap::addAssignments(const std::vector<PropertyAssignment>& propertyAssignments)
    {
    	for(auto& it : propertyAssignments)
    	{
    		addAssignment(it);
    	}
    }

    std::vector<PropertyValue> PropertyMap::getPropertyValues() const
    {
    	 std::vector<PropertyValue> propertyValues;
        for(auto& it : getPropertyToNetIdentifierMap())
        {
            PropertyValue propertyValue;
            propertyValue.setPropertyName(it.first);
            propertyValue.setNetIdentifier(it.second);
            propertyValues.push_back(propertyValue);
        }
        return propertyValues;
    }

    std::vector<PropertyAssignment> PropertyMap::getPropertyAssignments() const
    {
    	std::vector<PropertyAssignment> propertyAssognments;
        for(auto& it : getPropertyToAssignmentMap())
        {
            PropertyAssignment propertyAssignment;
            propertyAssignment.setPropertyName(it.first);
            propertyAssignment.setNetAssignment(it.second);
            propertyAssognments.push_back(propertyAssignment);
        }
        return propertyAssognments;
    }

    std::string PropertyMap::toStr() const
    {
        std::ostringstream oss;
        oss << "[";
        printPropertyToIdentifierMap(oss);
        oss << "][";
        printPropertyToAssignmentMap(oss);
        oss << "]";
        return oss.str();
    }

    bool PropertyMap::operator ==(const PropertyMap& other) const
    {
        return getPropertyToNetIdentifierMap() == other.getPropertyToNetIdentifierMap() &&
                getPropertyToAssignmentMap() == other.getPropertyToAssignmentMap();
    }

    void PropertyMap::printPropertyToIdentifierMap(std::ostream& stream) const
    {
			for(auto& it : getPropertyToNetIdentifierMap())
			{
				PropertyName name = it.first;
				NetId netId = it.second;
				stream << name << ", " << netId << ", ";
			}
    }

    void PropertyMap::printPropertyToAssignmentMap(std::ostream& stream) const
    {
        for(auto& it : getPropertyToAssignmentMap())
        {
            PropertyName name = it.first;
            NetAssignment assignment = it.second;
            stream << name << ", " << assignment << ", ";
        }
    }

    const PropertyMap::PropertyToNetIdentifierMap& PropertyMap::getPropertyToNetIdentifierMap() const
    {
        return propertyToNetIdentifierMap_;
    }

    PropertyMap::PropertyToNetIdentifierMap& PropertyMap::getPropertyToNetIdentifierMap()
    {
        return propertyToNetIdentifierMap_;
    }

    const PropertyMap::PropertyToAssignmentMap& PropertyMap::getPropertyToAssignmentMap() const
    {
        return propertyToAssignmentMap_;
    }

    PropertyMap::PropertyToAssignmentMap& PropertyMap::getPropertyToAssignmentMap()
    {
        return propertyToAssignmentMap_;
    }
}
