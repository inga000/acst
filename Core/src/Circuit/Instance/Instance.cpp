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

#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

namespace Core {

    Instance::Instance () :
          master(NULL),
          circuit(NULL)
    {

    }

    Instance & Instance::clone() const
    {
    	Instance * newInstance = new Instance;
    	newInstance->setIdentifier(getIdentifier());
    	newInstance->setMaster(getMaster().clone());
    	newInstance->setPropertyMap(getPropertyMap());

    	for(auto & it : instanceTerminals)
    	{
    		newInstance->addInstanceTerminal(it.first);
    	}

    	return *newInstance;
    }

    void Instance::setMaster(const Circuit& circuit)
    {
        this->master = & circuit;
    }

    void Instance::setCircuit(const Circuit& circuit)
    {
        this->circuit = &circuit;
    }

    void Instance::setPropertyMap(PropertyMap propertyMap)
    {
    	this->propertyMap = propertyMap;
    }

    void Instance::setIdentifier(const InstanceId& identifier)
    {
        this->identifier = identifier;
    }

    void Instance::addPropertyValue(const PropertyValue& propertyValue)
    {
        getPropertyMap().addValue(propertyValue);
    }

    void Instance::addPropertyValues(const std::vector<PropertyValue>& propertyValues)
    {
        getPropertyMap().addValues(propertyValues);
    }

    void Instance::addPropertyAssignment(const PropertyAssignment& propertyAssignment)
    {
        getPropertyMap().addAssignment(propertyAssignment);
    }

    void Instance::addPropertyAssignments(const std::vector<PropertyAssignment>& propertyAssignments)
    {
        getPropertyMap().addAssignments(propertyAssignments);
    }

    bool Instance::hasPropertyValue(const PropertyName& propertyName) const
    {
        return getPropertyMap().hasValue(propertyName);
    }

    bool Instance::hasPropertyAssignment(const PropertyName& propertyName) const
    {
        return getPropertyMap().hasAssignment(propertyName);
    }

    NetId Instance::findPropertyValue(const PropertyName& propertyName) const
    {
        return getPropertyMap().findValue(propertyName);
    }

    NetAssignment Instance::findPropertyAssignment(const PropertyName& propertyName) const
    {
        return getPropertyMap().findAssignment(propertyName);
    }

    InstanceTerminal& Instance::addInstanceTerminal(const TerminalId& terminalId)
    {
        InstanceTerminal* instanceTerminal = new InstanceTerminal;
        instanceTerminal->setInstance(*this);
        const Terminal& terminal = getMaster().findTerminal(terminalId);
        instanceTerminal->setTerminal(terminal);
        getInstanceTerminalMap().insert(InstanceTerminalMap::value_type(terminalId, instanceTerminal));
        return *instanceTerminal;
    }

    const InstanceTerminal & Instance::findInstanceTerminal(const TerminalId & terminalIdentifier) const
    {
    	assert(instanceTerminals.find(terminalIdentifier) != instanceTerminals.end());
    	return * instanceTerminals.at(terminalIdentifier);
    }

    InstanceTerminal & Instance::findInstanceTerminal(const TerminalId & terminalIdentifier)
    {
    	assert(instanceTerminals.find(terminalIdentifier) != instanceTerminals.end());
    	return * instanceTerminals.at(terminalIdentifier);
    }

    InstanceTerminal & Instance::getInstanceTerminal(const TerminalId & terminalIdentifier) const
    {
    	assert(instanceTerminals.find(terminalIdentifier) != instanceTerminals.end());
    	return * instanceTerminals.at(terminalIdentifier);
    }

    bool Instance::hasInstanceTerminal(const TerminalId & terminalIdentifier) const
    {
    	return instanceTerminals.find(terminalIdentifier) != instanceTerminals.end();
    }

    std::vector<const InstanceTerminal*> Instance::getInstanceTerminals() const
    {
    	std::vector<const InstanceTerminal*> instanceTerminals;
    	for(auto& it : getInstanceTerminalMap())
    	{
    		instanceTerminals.push_back(it.second);
    	}
    	return instanceTerminals;
    }

    std::vector<InstanceTerminal*> Instance::getInstanceTerminals()
    {
    	std::vector<InstanceTerminal*> instanceTerminals;
    	for(auto& it : getInstanceTerminalMap())
    	{
    		instanceTerminals.push_back(it.second);
    	}
    	return instanceTerminals;
    }

    const Circuit& Instance::getMaster() const
    {
        assert(hasMaster());
        return *master;
    }

    const InstanceId& Instance::getIdentifier() const
    {
        assert(hasIdentifier());
        return identifier;
    }

    InstanceId Instance::getInstanceIdentifier() const
    {
        assert(hasIdentifier());
        return identifier;
    }


    std::string Instance::toStr() const
    {
        return getIdentifier().toStr();
    }

    void Instance::print(std::ostream& stream) const
    {
        stream << getIdentifier() << std::endl;
        printProperties(stream);
        printInstanceTerminals(stream);
        stream << getMaster();
    }

    void Instance::printProperties(std::ostream& stream) const
    {
        stream << getPropertyMap();
    }

    void Instance::printInstanceTerminals(std::ostream& stream) const
    {
        stream << "InstanceTerminals: " << std::endl;
        for(auto& it : getInstanceTerminalMap())
        {
            const InstanceTerminal* instanceTerminal = it.second;
            instanceTerminal->print(stream);
            stream << ", " << std::endl;
        }
        stream << std::endl;
    }

    const Circuit& Instance::getCircuit() const
    {
        assert(hasCircuit());
        return *circuit;
    }

    Instance::InstanceTerminalMap& Instance::getInstanceTerminalMap()
    {
        return instanceTerminals;
    }

    const Instance::InstanceTerminalMap& Instance::getInstanceTerminalMap() const
    {
        return instanceTerminals;
    }

    const PropertyMap& Instance::getPropertyMap() const
    {
        return propertyMap;
    }

    PropertyMap& Instance::getPropertyMap()
    {
        return propertyMap;
    }

    bool Instance::hasMaster() const
    {
        return master != NULL;
    }

    bool Instance::hasCircuit() const
    {
        return circuit != NULL;
    }

    bool Instance::hasIdentifier() const
    {
        return identifier.isInitialized();
    }
}
