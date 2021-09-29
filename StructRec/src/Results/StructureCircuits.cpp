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

#include "StructRec/incl/Results/StructureCircuits.h"

#include "StructRec/incl/Results/CircuitRegister.h"

#include "StructRec/incl/StructureCore.h"

#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructureCircuits::StructureCircuits():
			structureCore_(nullptr)
	{
	}

	StructureCircuits::~StructureCircuits()
	{
		eraseAllCircuits();
		eraseAllInstances();
	}

	void StructureCircuits::setStructureCore(const StructureCore& structureCore)
	{
		structureCore_ = &structureCore;
	}

	void StructureCircuits::addStructureCircuit(
		StructureCircuit & circuit)
	{
		const int level = circuit.getHierarchyLevel();
		circuitMap_.insert(std::pair<const int, StructureCircuit*> (level,&circuit));
	}

	StructureCircuit & StructureCircuits::findStructureCircuit(const int level)
	{
		assert(hasStructureCircuit(level));
		StructureCircuit * circuit = circuitMap_.at(level);
		return * circuit;
	}

	const StructureCircuit & StructureCircuits::findStructureCircuit(const int level) const
	{
		assert(hasStructureCircuit(level));
		StructureCircuit * circuit = circuitMap_.at(level);
		return * circuit;
	}

	void StructureCircuits::addInstance(StructureInstance & instance)
	{
		Core::InstanceId id = instance.getId();
		instanceMap_.insert(std::pair<Core::InstanceId, StructureInstance*>(id, & instance));
	}

	StructureInstance & StructureCircuits::findInstance(Core::InstanceId instanceId) const
	{
		assert(hasInstance(instanceId));
		StructureInstance * instance = instanceMap_.at(instanceId);
		return * instance;
	}

	void StructureCircuits::setCircuitId(Core::CircuitId circuitId)
	{
		circuitId_ = circuitId;
	}

	StructureCircuit& StructureCircuits::findHighestInitializedCircuit()
	{
		int maxHierarchyLevel = getMaxInitializedHierarchyLevel();
		return findStructureCircuit(maxHierarchyLevel);
	}

	std::vector<const Structure*> StructureCircuits::findConnectedStructures(const Core::NetId& netId) const
	{
		std::vector<const Structure*> structures;
		for(int i = getMaxInitializedHierarchyLevel(); i >= 0; i--)
		{
			const StructureCircuit& structureCircuit = findStructureCircuit(i);
			std::vector<const Structure*> connectedStructures = structureCircuit.findConnectedStructures(netId);
			structures.insert(structures.end(), connectedStructures.begin(), connectedStructures.end());
		}
		return structures;
	}

	const StructureCircuit& StructureCircuits::findHighestInitializedCircuit() const
		{
			int maxHierarchyLevel = getMaxInitializedHierarchyLevel();
			return findStructureCircuit(maxHierarchyLevel);
		}


	std::vector<const Structure*> StructureCircuits::findAllStructures() const
	{
		std::vector<const Structure*> structures;
		for(int i = getMaxInitializedHierarchyLevel(); i >= 0; i--)
		{
			const StructureCircuit& structureCircuit = findStructureCircuit(i);
			std::vector<const Structure*> currentStructures = structureCircuit.findAllStructures();
			structures.insert(structures.end(), currentStructures.begin(), currentStructures.end());
		}

		return structures;
	}

	std::vector<const Structure*> StructureCircuits::findStructures(const StructureName& structureName) const
	{
		std::vector<const Structure*> structures;

		if(getStructureCore().hasLevel(structureName)) {
			int level = getStructureCore().findLevel(structureName);
			if(hasStructureCircuit(level)) {
				const StructureCircuit& levelCircuit = findStructureCircuit(level);
				structures = levelCircuit.findStructures(structureName);
			}
		}
		return structures;
	}

	std::vector<Structure*> StructureCircuits::getStructures(const StructureName& structureName)
	{
		std::vector<Structure*> structures;

		if(getStructureCore().hasLevel(structureName)) {
			int level = getStructureCore().findLevel(structureName);
			if(hasStructureCircuit(level)) {
				StructureCircuit& levelCircuit = findStructureCircuit(level);
				structures = levelCircuit.findStructures(structureName);
			}
		}
		return structures;
	}

	std::vector<Structure*> StructureCircuits::findAllStructuresWithoutParents() const
	{
		std::vector<Structure*> structures;
		for(int i = getMaxInitializedHierarchyLevel(); i >= 0; i--)
		{
			const StructureCircuit& structureCircuit = findStructureCircuit(i);
			std::vector<Structure*> structuresWithoutParents = structureCircuit.findAllStructuresWithoutParents();
			structures.insert(structures.end(), structuresWithoutParents.begin(), structuresWithoutParents.end());
		}

		return structures;
	}


	int StructureCircuits::findLevel(const Structure& structure) const
	{
		int level = -1;
		for(std::map<const int, StructureCircuit*>::const_iterator it = circuitMap_.begin(); it != circuitMap_.end(); it++)
		{
			const StructureCircuit* structureCircuit = it->second;
			if(structureCircuit->hasStructure(structure.getIdentifier()))
			{
				level = it->first;
				break;
			}
		}
		assert(level != -1);
		return level;
	}

	const StructureCore& StructureCircuits::getStructureCore() const
	{
		assert(structureCore_ != nullptr);
		return *structureCore_;
	}

	bool StructureCircuits::hasStructureCircuit(const int level) const
	{
		return circuitMap_.find(level) != circuitMap_.end();
	}

	const std::map<const int, StructureCircuit*>& StructureCircuits::getCircuitMap() const
	{
		return circuitMap_;
	}

	std::string StructureCircuits::toStr() const
	{
		std::ostringstream stream;
		stream << "Components: " << std::endl;
		for(int level = getMaxInitializedHierarchyLevel(); level >= 0; level--)
		{
			const StructureCircuit & levelCircuit = findStructureCircuit(level);
			if(!levelCircuit.empty())
			{
				levelCircuit.print(stream);
			}
		}
		return stream.str();
	}

	void StructureCircuits::printStructures(std::ostream& stream, int columns) const
	{
		if(!circuitMap_.empty())
		{
			columns++;
			stream << spaces(columns);
			stream << "Structures: " << std::endl;

			for(int level = getMaxInitializedHierarchyLevel(); level >= 0; level--)
			{
				const StructureCircuit & levelCircuit = findStructureCircuit(level);
				if(!levelCircuit.empty())
				{
					levelCircuit.printResult(stream, columns, *this);
				}
			}
		}
	}

	std::vector<StructureInstance*> StructureCircuits::findAllInstances() const
	{
		std::vector<StructureInstance*> instances;
		for(std::map<Core::InstanceId, StructureInstance*>::const_iterator it = instanceMap_.begin(); it != instanceMap_.end(); it ++)
		{
			StructureInstance * currentInstance = it->second;
			instances.push_back(currentInstance);
		}
		return instances;
	}

	void StructureCircuits::print(std::ostream & stream, int columns, Core::InstanceNamePath instanceNamePath) const
	{
		printStructures(stream, columns);
		printInstances(stream, columns, instanceNamePath);
	}

	void StructureCircuits::printInstances(std::ostream& stream, int columns, Core::InstanceNamePath instanceNamePath) const
	{
		if(!instanceMap_.empty())
		{
			columns++;
			stream << spaces(columns);
			stream << "Instances: " << std::endl;
			columns++;
			for(std::map<Core::InstanceId, StructureInstance*>::const_iterator it = instanceMap_.begin(); it != instanceMap_.end(); it++)
			{
				instanceNamePath.append(it->first.getInstanceName());
				stream << spaces(columns) << "Instance: " << instanceNamePath << std::endl;
				it->second->getSubCircuits().print(stream, columns, instanceNamePath);
				instanceNamePath.pop();
			}
		}
	}

    void StructureCircuits::printSummary(std::ostream& stream) const
    {
        for(int level = getMaxInitializedHierarchyLevel(); level >= 0; level--)
        {
            const StructureCircuit & levelCircuit = findStructureCircuit(level);
            levelCircuit.printSummary(stream);
        }
    }

	void StructureCircuits::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::InstanceNamePath instanceNamePath;
		instanceNamePath.setIsRoot();
		writeXmlStructures(node, doc, instanceNamePath);
		writeXmlInstances(node, doc, instanceNamePath);
	}

	void StructureCircuits::writeXmlStructures(Core::XmlNode& node,
			Core::XmlDocument& doc, Core::InstanceNamePath instanceNamePath) const
	{
		if(!circuitMap_.empty())
		{
			for(int level = getMaxInitializedHierarchyLevel(); level >= 0; level--)
			{
				const StructureCircuit & levelCircuit = findStructureCircuit(level);
				if(!levelCircuit.empty())
				{
					levelCircuit.writeXml(node, doc, instanceNamePath, *this);
				}
			}
		}
	}

	void StructureCircuits::removeUncertainStructures()
	{
		int currentHL = getMaxInitializedHierarchyLevel();
		StructureCircuit& circuit = findStructureCircuit(currentHL);
		for(int level = currentHL - 1; level >= 0; level--)
		{
			StructureCircuit& lowerCircuit = findStructureCircuit(level);
			std::vector<Structure*> lowerStructures = lowerCircuit.findAllStructures();
			for(auto& it : lowerStructures)
			{
				Structure& currentStructure = *it;
				if(!currentStructure.hasMaxPersistence() && !currentStructure.hasParent() &&
				   (currentHL  >= (getStructureCore().findLevel(currentStructure.getIdentifier().getName()) + currentStructure.getPersistence())))
				{
					currentStructure.disConnect(*this);
					delete &currentStructure;
				}
			}
		}
	}

	void StructureCircuits::writeXmlInstances(Core::XmlNode& node, Core::XmlDocument& doc,
			Core::InstanceNamePath instanceNamePath) const
	{

		if(!instanceMap_.empty())
		{
			for(std::map<Core::InstanceId, StructureInstance*>::const_iterator it = instanceMap_.begin(); it != instanceMap_.end(); it++)
			{
				instanceNamePath.append(it->first.getInstanceName());
				it->second->getSubCircuits().writeXmlStructures(node, doc, instanceNamePath);
				it->second->getSubCircuits().writeXmlInstances(node, doc, instanceNamePath);
				instanceNamePath.pop();
			}
		}
	}

	std::map<Core::InstanceId, StructureInstance*> & StructureCircuits::getInstanceMap()
	{
		return instanceMap_;
	}

	const std::map<Core::InstanceId, StructureInstance*>& StructureCircuits::getInstanceToCircuitMap() const
	{
		return instanceMap_;
	}

	bool StructureCircuits::hasInstance(Core::InstanceId id) const
	{
		return instanceMap_.find(id) != instanceMap_.end();
	}

	void StructureCircuits::eraseAllCircuits()
	{
		for (std::map<int, StructureCircuit*>::const_iterator it =circuitMap_.begin();
			it != circuitMap_.end(); ++it)
			{
				StructureCircuit & currentCircuit = * it->second;
				delete & currentCircuit;
			}
	}

	Core::CircuitId StructureCircuits::getCircuitId() const
	{
		assert(hasCircuitId());
		return circuitId_;
	}

	bool StructureCircuits::hasCircuitId() const
	{
		return circuitId_.isInitialized();
	}

	void StructureCircuits::eraseAllInstances()
	{
		for (std::map<Core::InstanceId, StructureInstance*>::const_iterator it =instanceMap_.begin();
				it != instanceMap_.end(); ++it)
		{
			StructureInstance * currentInstance = it->second;
			delete currentInstance;
		}
	}

	int StructureCircuits::getMaxInitializedHierarchyLevel() const
	{
		return  circuitMap_.size() - 1;
	}

	int StructureCircuits::getMaxWithStructuresInitializedHierarchyLevel() const
	{
		int level = 0;
		for(int it_level = getMaxInitializedHierarchyLevel(); it_level >= 0; it_level--)
		{
			const StructureCircuit & circuit = findStructureCircuit(it_level);
			if(circuit.hasStructures())
			{
				level = circuit.getHierarchyLevel();
				break;
			}
		}
		return level;
	}

	void StructureCircuits::addNewInstance(Core::InstanceId instanceId,
			StructureCircuits& subCircuits)
	{
		StructureInstance* instance = new StructureInstance;
		instance->setInstanceId(instanceId);
		instance->setSubCircuits(subCircuits);
		instance->setSuperiorCircuits(*this);
		addInstance(*instance);
	}


	std::string StructureCircuits::spaces(int columns) const
	{
		std::string ret = "";
		for (int i = 1; i <= columns; i++)
			ret += " ";
		return ret;
	}


	Structure& StructureCircuits::findStructureInTopCircuit(StructureId structureId) const
	{
		Structure * foundStructure = nullptr;
		for(std::map<int, StructureCircuit*>::const_iterator it = circuitMap_.begin(); it != circuitMap_.end(); it++)
		{
			StructureCircuit * currentCircuit = it->second;

			if(currentCircuit->hasStructure(structureId))
			{
				foundStructure = & currentCircuit->findStructure(structureId);
				break;
			}

		}
		assert(foundStructure != nullptr);
		return * foundStructure;
	}


}
