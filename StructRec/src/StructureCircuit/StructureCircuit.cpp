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

#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/Library/HierarchyLevel.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace StructRec {

	const int StructureCircuit::LEVEL_NOT_INITIALIZED_ = -1;

	StructureCircuit::StructureCircuit() :
			hierarchyLevel_(LEVEL_NOT_INITIALIZED_)
	{
	}

	StructureCircuit::~StructureCircuit()
	{
		eraseAllNets();
		eraseAllStructures();
		eraseAllTerminals();
		eraseAllInstanceTerminals();
	}

	StructureCircuit & StructureCircuit::clone(StructureCircuits& circuits) const
	{
		StructureCircuit * newCircuit = new StructureCircuit;

		newCircuit->setHierarchyLevel(getHierarchyLevel());

		for(std::map<Core::NetId, StructureNet*>::const_iterator it_nets = netMap_.begin(); it_nets != netMap_.end(); it_nets++)
		{
			StructureNet * oldNet = it_nets->second;
			StructureNet & newNet = oldNet->cloneWithoutStructureConnection(*newCircuit, circuits);
			newCircuit->addStructureNet(newNet);
		}

		for(std::map<StructureId, Structure* >::const_iterator it_structures = structureMap_.begin(); it_structures != structureMap_.end(); it_structures++)
		{
			Structure * oldStructure = it_structures->second;
			Structure & newStructure = oldStructure->clone(*newCircuit, circuits);
			newCircuit->addStructure(newStructure);
		}

		return * newCircuit;

	}

	void StructureCircuit::addStructureNet(StructureNet& net)
	{
		netMap_.insert(std::pair<Core::NetId,StructureNet*>(net.getIdentifier(),&net));
	}

	void StructureCircuit::addStructure(Structure& structure)
	{
		structureMap_.insert(std::pair<StructureId, Structure*>(structure.getIdentifier(), &structure));
	}

	void StructureCircuit::setHierarchyLevel(const int level)
	{
		assert(level >= 0);
		hierarchyLevel_ =  level;
	}

	int StructureCircuit::getHierarchyLevel() const
	{
		assert(hasHierarchyLevel());
		return  hierarchyLevel_;
	}

	StructureNet& StructureCircuit::findStructureNet(Core::NetId netId) const
	{
		assert(hasStructureNet(netId));
		StructureNet* net = netMap_.at(netId);
		return * net;
	}

	Structure& StructureCircuit::findStructure(StructureId structureId) const
	{
		assert(hasStructure(structureId));
		Structure* structure = structureMap_.at(structureId);
		return *structure;
	}

	StructureTerminal& StructureCircuit::findTerminal(
		Core::TerminalId terminalId) const
	{
		assert(hasTerminal(terminalId));
		StructureTerminal * terminal = terminalMap_.at(terminalId);
		return *terminal;
	}

	const std::vector<StructureInstanceTerminal*> StructureCircuit::findInstanceTerminals (Core::InstanceId instanceId) const
	{
		if(hasInstanceTerminalsForThisInstance(instanceId))
		{
			return instanceToInstanceTerminalsMap_.at(instanceId);
		}
		else
		{
			std::vector<StructureInstanceTerminal*> emptyVector;
			return emptyVector;
		}
	}

	std::vector<const Structure*> StructureCircuit::findConnectedStructures(const Core::NetId& netId) const
	{
		std::vector<const Structure*> structures;
		if(hasStructureNet(netId)) {
			const StructureNet& structureNet = findStructureNet(netId);
			structures = structureNet.getAllConnectedStructures();
		}
		return structures;
	}

	bool StructureCircuit::hasInstanceTerminalsForThisInstance(Core::InstanceId instanceId) const
	{
		return instanceToInstanceTerminalsMap_.find(instanceId) != instanceToInstanceTerminalsMap_.end();
	}

	bool StructureCircuit::hasStructureNet(Core::NetId netId) const
	{
		return netMap_.find(netId) != netMap_.end();
	}

	bool StructureCircuit::hasStructure(StructureId structureId) const
	{
		return structureMap_.find(structureId) != structureMap_.end();
	}

	int StructureCircuit::getNumStructures() const
	{
		return structureMap_.size();
	}

	int StructureCircuit::getNumNets() const
	{
		return netMap_.size();
	}

	bool StructureCircuit::hasHierarchyLevel() const
	{
		return hierarchyLevel_ != LEVEL_NOT_INITIALIZED_;
	}

	std::string StructureCircuit::toStr() const
	{
		std::ostringstream buf;
		printTerminals(buf);
		printInstanceTerminals(buf);
		printNets(buf);
		print(buf);
		return buf.str();
	}

	void StructureCircuit::print(std::ostream& stream) const
	{
		//printSummary(stream);
		//printNets(stream);
		printStructures(stream);
	}

	void StructureCircuit::printSummary(std::ostream& stream) const
	{
		stream << "Circuit with ";
		stream << getNumStructures() << " Structures, ";
		stream << getNumNets() << " Nets on Hierarchy Level " << getHierarchyLevel() << "."<< std::endl;
	}

	void StructureCircuit::printNets(std::ostream& stream) const
	{
		stream << "Nets: ";
		for(std::map<Core::NetId, StructureNet*>::const_iterator it = netMap_.begin(); it != netMap_.end(); it++)
		{
			const StructureNet & net = *(it->second);
			net.print(stream);
			stream << "; " ;
		}
		stream << std::endl;
	}

	void StructureCircuit::printStructures(std::ostream& stream) const
	{
		int column = 0;
		for(std::map<StructureId, Structure*>::const_iterator it = structureMap_.begin(); it != structureMap_.end(); it++)
		{
			const Structure & structure = *(it->second);
			if(!structure.hasParent())
			{
			structure.print(stream, column);
			}
		}
	}

	void StructureCircuit::printTerminals(std::ostream & stream) const
	{
		stream << "Terminals: ";
		for(std::map<Core::TerminalId, StructureTerminal*>::const_iterator it = terminalMap_.begin(); it != terminalMap_.end(); it++)
		{
			const StructureTerminal  & terminal = *(it->second);
			stream << terminal << "; ";
		}
		stream << std::endl;
	}

	void StructureCircuit::printInstanceTerminals(std::ostream& stream) const
	{
		stream << "InstanceTerminals: ";
		const std::vector<StructureInstanceTerminal*> instanceTerminals = findAllInstanceTerminals();
		for(std::vector<StructureInstanceTerminal*>::const_iterator it = instanceTerminals.begin(); it != instanceTerminals.end(); it++)
		{
			const StructureInstanceTerminal * instTerm = *it;
			stream << instTerm << "; ";
		}
		stream << std::endl;
	}

	void StructureCircuit::printResult(
			std::ostream& stream, int column, const StructureCircuits & structureCircuits) const
	{
		for(std::map<StructureId, Structure*>::const_iterator it = structureMap_.begin(); it != structureMap_.end(); it++)
		{
			const Structure & structure = *(it->second);
			if(!structure.hasParentInSameStructureCircuits(structureCircuits))
			{
				structure.print(stream, column);
			}
		}
	}

	void StructureCircuit::writeXml(Core::XmlNode & node, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath, const StructureCircuits & circuits) const
	{
		for(std::map<StructureId, Structure*>::const_iterator it = structureMap_.begin(); it != structureMap_.end(); it++)
		{
			const Structure & structure = *(it->second);
			if(!structure.hasParentInSameStructureCircuits(circuits))
			{
				structure.writeXml(node, doc, instanceNamePath);
			}
		}
	}

	void StructureCircuit::eraseAllNets()
	{
		for(std::map<Core::NetId, StructureNet*>::const_iterator it = netMap_.begin(); it != netMap_.end(); it++)
		{
			StructureNet * currentNet = it->second;
			delete currentNet;
		}
	}


	void StructureCircuit::eraseAllTerminals()
	{
		for(std::map<Core::TerminalId, StructureTerminal*>::const_iterator it = terminalMap_.begin(); it != terminalMap_.end(); it++)
		{
				StructureTerminal * currentTerminal = it->second;
				delete currentTerminal;
		}
	}

	void StructureCircuit::eraseAllInstanceTerminals()
	{

		for(std::map<Core::InstanceId, std::vector<StructureInstanceTerminal*> >::const_iterator it = instanceToInstanceTerminalsMap_.begin();
				it != instanceToInstanceTerminalsMap_.end(); it++)
		{
			std::vector<StructureInstanceTerminal*> & instanceTerminals =  instanceToInstanceTerminalsMap_[it->first];
			while (instanceTerminals.begin() != instanceTerminals.end())
			{
				std::vector<StructureInstanceTerminal*>::iterator it = instanceTerminals.begin();
				delete * it;
				instanceTerminals.erase(it);
			}
		}
	}

	const std::vector<StructureNet*> StructureCircuit::findAllNets() const
	{
		std::vector<StructureNet*> nets;
		for(std::map<Core::NetId, StructureNet*>::const_iterator it = netMap_.begin(); it != netMap_.end(); it++)
		{
			StructureNet * currentNet = it->second;
			nets.push_back(currentNet);
		}
		return nets;
	}

	const std::vector<StructureTerminal*> StructureCircuit::findAllTerminals() const
	{
		std::vector<StructureTerminal*> terminals;
		for(std::map<Core::TerminalId, StructureTerminal*>::const_iterator it = terminalMap_.begin(); it != terminalMap_.end(); it++)
		{
			StructureTerminal * currentTerminal = it->second;
			terminals.push_back(currentTerminal);
		}
		return terminals;
	}

	const std::vector<StructureInstanceTerminal*> StructureCircuit::findAllInstanceTerminals() const
	{
		std::vector<StructureInstanceTerminal*> instanceTerminals;
		for(std::map<Core::InstanceId, std::vector<StructureInstanceTerminal*> >::const_iterator it = instanceToInstanceTerminalsMap_.begin(); it != instanceToInstanceTerminalsMap_.end(); it++)
		{
			std::vector<StructureInstanceTerminal*> instTerm = it->second;
			instanceTerminals.insert(instanceTerminals.end(),instTerm.begin(),instTerm.end());
		}
		return instanceTerminals;
	}

	std::vector<Structure*> StructureCircuit::findStructures(const StructureName& name)
	{
		std::vector<Structure*> structures;
		for(auto& it : structureMap_)
		{
			Structure* currentStructure = it.second;
			if(currentStructure->getIdentifier().getName() == name) {
				structures.push_back(currentStructure);
			}
		}
		return structures;
	}

	std::vector<const Structure*> StructureCircuit::findStructures(const StructureName& name) const
	{
		std::vector<const Structure*> structures;
		for(auto& it : structureMap_)
		{
			const Structure* currentStructure = it.second;
			if(currentStructure->getIdentifier().getName() == name) {
				structures.push_back(currentStructure);
			}
		}
		return structures;
	}

	std::vector<Structure*> StructureCircuit::findAllStructures()
	{
		std::vector<Structure*> structures;
		for(auto& it : structureMap_)
		{
			structures.push_back(it.second);
		}
		return structures;
	}

	std::vector<const Structure*> StructureCircuit::findAllStructures() const
	{
		std::vector<const Structure*> structures;
		for(auto& it : structureMap_)
		{
			structures.push_back(it.second);
		}
		return structures;
	}

	std::vector<Structure*> StructureCircuit::findAllStructuresWithoutParents() const
	{
		std::vector<Structure*> structures;
		for(auto& it : structureMap_)
		{
			Structure* currentStructure = it.second;
			if(!currentStructure->hasParent())
			{
				structures.push_back(currentStructure);
			}
		}
		return structures;
	}

	void StructureCircuit::eraseAllStructures()
	{
		for(std::map<StructureId, Structure*>::const_iterator it = structureMap_.begin(); it != structureMap_.end(); it++)
		{
			Structure * currentStructure = it->second;
			delete currentStructure;
		}
	}

	void StructureCircuit::deleteNet(StructureNet& net)
	{
	    netMap_.erase(net.getIdentifier());
	}

	void StructureCircuit::deleteStructure(Structure& structure)
	{
	    structureMap_.erase(structure.getIdentifier());
	}

	bool StructureCircuit::hasNets() const
	{
		return !netMap_.empty();
	}

	bool StructureCircuit::hasStructures() const
	{
		return !structureMap_.empty();
	}

	bool StructureCircuit::empty() const
	{
		return !hasNets() && !hasStructures();
	}

	void StructureCircuit::addTerminal(StructureTerminal& terminal)
	{
		terminalMap_.insert(std::pair<Core::TerminalId, StructureTerminal*>(terminal.getTerminalId(), & terminal));
	}

	void StructureCircuit::addInstanceTerminal(
			StructureInstanceTerminal& instanceTerminal)
	{
		std::vector<StructureInstanceTerminal*> & instanceTerminals = instanceToInstanceTerminalsMap_[instanceTerminal.getInstance().getId()];
		instanceTerminals.push_back(&instanceTerminal);
	}

	bool StructureCircuit::hasTerminals() const
	{
		return !terminalMap_.empty();
	}

	bool StructureCircuit::hasInstanceTerminals() const
	{
		return !instanceToInstanceTerminalsMap_.empty();
	}

	bool StructureCircuit::hasTerminal(Core::TerminalId terminalId) const
	{
		return terminalMap_.find(terminalId) != terminalMap_.end();
	}

}


