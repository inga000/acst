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
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructureNet::StructureNet() :
			terminal_(nullptr)
	{
	}

	StructureNet::~StructureNet()
	{
	}

	void StructureNet::addPin(StructurePin & pin)
	{
		std::vector<StructurePin*>& pins = connectedPins_[pin.getPinType()];
		pins.push_back(&pin);
	}

	bool StructureNet::operator ==(const StructureNet& other) const
	{
		return connectedPins_== other.connectedPins_;
	}

	bool StructureNet::operator !=(const StructureNet& other) const
	{
		return !(*this == other);
	}

	void StructureNet::setNetId(Core::NetId netId)
	{
		netId_ = netId;
	}

	Core::NetId StructureNet::getIdentifier() const
	{
		assert(hasNetId());
		return netId_;
	}

	bool StructureNet::isSupply() const
	{
		if(supply_.isInitialized()) {
			return supply_.isSupply();
		} else {
			return false;
		}
	}

	bool StructureNet::isConnected() const
	{
		return !(connectedPins_.empty()) || hasInstanceTerminals();
	}

	bool StructureNet::hasTerminal() const
	{
		return terminal_ != nullptr;
	}

	void StructureNet::print(std::ostream& stream) const
	{
		stream << getIdentifier();
	}

	bool StructureNet::hasNetId() const
	{
		return netId_.hasNetPath();
	}

	std::string StructureNet::toStr() const
	{
		std::ostringstream stream;
		stream << getIdentifier();
		return stream.str();
	}

	void StructureNet::deletePin(StructurePin& pin)
	{
	    StructurePinType pinType = pin.getPinType();
		std::vector<StructurePin*>& pins = connectedPins_[pinType];

		if(pins.size() == 1) {
		    connectedPins_.erase(pinType);
		} else {
		    for(std::vector<StructurePin*>::iterator j = pins.begin(); j != pins.end(); j++)
            {
		        StructurePin* currentPin = *j;
                if(currentPin == &pin)
                {
                    pins.erase(j);
                    break;
                }
            }
		}
	}

	bool StructureNet::hasPins(StructurePinType pinType) const
	{
		return connectedPins_.find(pinType) != connectedPins_.end();
	}

	void StructureNet::addInstanceTerminal(
		StructureInstanceTerminal& instanceTerminal)
	{
		instanceTerminals_.push_back(&instanceTerminal);
	}

	const std::vector<StructureInstanceTerminal*> StructureNet::getConnectedInstanceTerminals() const
	{
		return instanceTerminals_;
	}

	bool StructureNet::isConnectedToTerminal() const
	{
		return terminal_ != nullptr;
	}

	void StructureNet::disconnectAllPins()
	{
		for(std::map<StructurePinType, std::vector<StructurePin*> >::const_iterator it = connectedPins_.begin();
						it != connectedPins_.end(); it++)
			{
				std::vector<StructurePin*> & pins =  connectedPins_[it->first];
				while (pins.begin() != pins.end())
					{
						std::vector<StructurePin*>::iterator it = pins.begin();
						delete * it;
						pins.erase(it);
					}
			}
	}

	const std::vector<Structure*> StructureNet::findConnectedStructures(
		const StructurePinType & pinType)
	{
		std::vector<Structure*> structures;
		const std::vector<StructurePin*>  pins = findPins(pinType);
		for(std::vector<StructurePin*>::const_iterator it = pins.begin(); it != pins.end(); it++)
		{
			StructurePin * pinPointer = *it;
			structures.push_back(&(pinPointer->getStructure()));
		}

		return structures;
	}

	std::vector<const Structure*> StructureNet::findConnectedStructures(
		const StructurePinType & pinType) const
	{
		std::vector<const Structure*> structures;
		const std::vector<StructurePin*>  pins = findPins(pinType);
		for(std::vector<StructurePin*>::const_iterator it = pins.begin(); it != pins.end(); it++)
		{
			StructurePin * pinPointer = *it;
			structures.push_back(&(pinPointer->getStructure()));
		}

		return structures;
	}


	std::vector<const Structure*> StructureNet::getAllConnectedStructures() const
	{
		std::vector<const Structure*> structures;
		for(StructurePinMap::const_iterator it = connectedPins_.begin(); it != connectedPins_.end(); it++)
		{
			const StructurePinVector pins = it->second;
			for(StructurePinVector::const_iterator pinIt = pins.begin(); pinIt != pins.end(); pinIt++)
			{
				const StructurePin* structurePin = *pinIt;
				const Structure& structure = structurePin->getStructure();


				bool hasIt = false;
				for(auto & struc : structures )
				{
					if(struc->getIdentifier() == structure.getIdentifier())
						hasIt = true;
				}

				if(!hasIt)
					structures.push_back(&structure);
			}
		}
		return structures;
	}


	const std::vector<CrossInstanceStructure*> StructureNet::findConnectedInstanceStructures(
			const StructurePinType & pinType, int hierarchyLevel) const
	{
		Core::InstanceNamePath instanceNamePath;
		instanceNamePath.setIsRoot();

		//find possible children in current instance
		std::vector<CrossInstanceStructure*> crossInstanceStructures = findDirectlyConnectedStructures(pinType, instanceNamePath);

		//find possible children in instances to which the current instance is connected to
		const std::vector<StructureInstanceTerminal*> instanceTerminals = getConnectedInstanceTerminals();
		for(auto instTermIt = instanceTerminals.begin(); instTermIt != instanceTerminals.end(); instTermIt++)
		{
			StructureInstanceTerminal* instTerm = *instTermIt;

			StructureInstance* instance = &instTerm->getInstance();

			std::vector<StructureInstance*> visitedInstances;
			visitedInstances.push_back(instance);
			instanceNamePath.append(instance->getName());

			const StructureNet& net = instTerm->getTerminalNet();
			std::vector<CrossInstanceStructure*> structuresInInstance = net.findConnectedInstanceStructuresInInstance(pinType, hierarchyLevel, visitedInstances, instanceNamePath);
			crossInstanceStructures.insert(crossInstanceStructures.end(), structuresInInstance.begin(), structuresInInstance.end());
			instanceNamePath.pop();
		}
		return crossInstanceStructures;
	}

	const std::vector<CrossInstanceStructure*> StructureNet::findConnectedInstanceStructuresInInstance(const StructurePinType & pinType,
			const int hierarchyLevel, std::vector<StructureInstance*> visitedInstances, Core::InstanceNamePath instanceNamePath) const
	{
		std::vector<CrossInstanceStructure*> structures;
		const std::vector<StructurePin*>  pins = findPins(pinType);
		for(std::vector<StructurePin*>::const_iterator it = pins.begin(); it != pins.end(); it++)
		{
			StructurePin * pinPointer = *it;
			Structure & structure = pinPointer->getStructure();
			std::string structureName = structure.getStructureName();
			if(structureName.find('/') == std::string::npos) //Search only for original structures which are not copies of structures in deeper instances(See pair creation)
			{
				CrossInstanceStructure * instStructure = new CrossInstanceStructure;
				instStructure->setStructure(structure);
				instStructure->setInstanceNamePath(instanceNamePath);
				connectInstanceStructureToTopCircuit(*instStructure, visitedInstances, hierarchyLevel);
				structures.push_back(instStructure);
			}
		}

		//find devices connected to the instTerm recursively
		const std::vector<StructureInstanceTerminal*> instanceTerminals = getConnectedInstanceTerminals();
		for(std::vector<StructureInstanceTerminal*>::const_iterator instTermIt =instanceTerminals.begin(); instTermIt != instanceTerminals.end(); instTermIt++)
		{
			StructureInstanceTerminal * instTerm = *instTermIt;
			visitedInstances.push_back(&instTerm->getInstance());
			const StructureNet & net = instTerm->getTerminal().getNet();
			instanceNamePath.append(instTerm->getInstance().getId().getInstanceName());
			std::vector<CrossInstanceStructure*> structuresInInstance = net.findConnectedInstanceStructuresInInstance(pinType, hierarchyLevel, visitedInstances, instanceNamePath);
			structures.insert(structures.end(), structuresInInstance.begin(), structuresInInstance.end());
			instanceNamePath.pop();
		}

		return structures;
	}


	void StructureNet::connectInstanceStructureToTopCircuit(CrossInstanceStructure & instStructure,
			std::vector<StructureInstance*> instances, const int hierarchyLevel) const
	{
		std::vector<StructurePin*> pinsOfStructure = instStructure.getStructure().findAllPins();
		for(std::vector<StructurePin*>::iterator pinIt = pinsOfStructure.begin(); pinIt != pinsOfStructure.end(); pinIt++)
		{
			StructurePin * pin = *pinIt;
			bool isConnectedToInstance = false;
			if(pin->getNet().hasTerminal())
			{
				StructureInstanceTerminal * oldInstTerm;
				StructureInstanceTerminal * instTerm;
				StructureInstanceTerminal * topLevelTerminal;

				StructureInstance * instance = * instances.rbegin();
				const std::vector<StructureInstanceTerminal*> instTerms = instance->getSuperiorCircuits().findStructureCircuit(hierarchyLevel).findInstanceTerminals(instance->getId());
				for(std::vector<StructureInstanceTerminal*>::const_iterator instTermIt = instTerms.begin(); instTermIt != instTerms.end(); instTermIt++)
				{
					instTerm = *instTermIt;
					if(pin->getNet().getTerminal().getTerminalId() == instTerm->getTerminalId())
					{
						isConnectedToInstance = true;
						topLevelTerminal = instTerm;
						oldInstTerm = instTerm;
						break;
					}
				}

				for(std::vector<StructureInstance*>::reverse_iterator instIt = instances.rbegin() + 1; instIt != instances.rend(); instIt++)
				{
					StructureInstance * instance = *instIt;
					const std::vector<StructureInstanceTerminal*> instTerms = instance->getSuperiorCircuits().findStructureCircuit(hierarchyLevel).findInstanceTerminals(instance->getId());
					for(std::vector<StructureInstanceTerminal*>::const_iterator instTermIt = instTerms.begin(); instTermIt != instTerms.end(); instTermIt++)
					{
						isConnectedToInstance = false;

						StructureInstanceTerminal * instTerm = *instTermIt;
						if(oldInstTerm->getNet().hasTerminal())
						{
							if(oldInstTerm->getNet().getTerminal().getTerminalId() == instTerm->getTerminalId())
							{

								isConnectedToInstance = true;
								oldInstTerm = instTerm;
								topLevelTerminal = instTerm;
								break;
							}
						}
					}

					if(!isConnectedToInstance)
					{
						break;
					}
				}

				if(isConnectedToInstance)
				{
					StructurePinType pinType = pin->getPinType();
					instStructure.addConnection(pinType,*topLevelTerminal);
				}
			}
		}
	}

	std::vector<CrossInstanceStructure*> StructureNet::findDirectlyConnectedStructures(
			const StructurePinType & pinType, Core::InstanceNamePath instanceNamePath) const
	{
		std::vector<CrossInstanceStructure*> structures;
		const std::vector<StructurePin*>  pins = findPins(pinType);
		for(std::vector<StructurePin*>::const_iterator it = pins.begin(); it != pins.end(); it++)
		{
			StructurePin* pinPointer = *it;
			Structure& structure = pinPointer->getStructure();
			std::string structureName = structure.getStructureName();
			if(structureName.find('/') == std::string::npos) //Search only for original structures which are not copies of structures in deeper instances(See pair creation)
			{
				CrossInstanceStructure* instStructure = new CrossInstanceStructure;
				instStructure->setStructure(structure);
				instStructure->setInstanceNamePath(instanceNamePath);
				structures.push_back(instStructure);
			}

		}
		return structures;
	}


	std::vector<StructurePin*> StructureNet::findPins(
		const StructurePinType& pinType) const
	{
		if(hasPins(pinType))
		{
		return connectedPins_.at(pinType);
		}
		else
		{
			std::vector<StructurePin*> emptyVector;
			return emptyVector;
		}
	}

	void StructureNet::setSupply(Core::Supply supply)
	{
		supply_ = supply;
	}

	Core::Supply StructureNet::getSupply() const
	{
		assert(hasSupply());
		return supply_;
	}

	const StructureTerminal & StructureNet::getTerminal() const
	{
		assert(hasTerminal());
		return * terminal_;
	}

	bool StructureNet::hasSupply() const
	{
		return supply_.isInitialized();
	}


	bool StructureNet::hasInstanceTerminals() const
	{
		return !instanceTerminals_.empty();

	}

	void StructureNet::setTerminal(StructureTerminal& terminal)
	{
		terminal_ = & terminal;
	}

	void StructureNet::createAndConnectTerminal(StructureNet & newNet, StructureCircuit & circuit) const
	{
		if(hasTerminal())
		{
			StructureTerminal * newTerminal = new StructureTerminal;

			newTerminal->setNet(newNet);
			newTerminal->setCircuit(circuit);
			newTerminal->setTerminalId(getTerminal().getTerminalId());

			newNet.setTerminal(*newTerminal);

			circuit.addTerminal(*newTerminal);
		}


	}
	void StructureNet::createAndConnectInstanceTerminals(StructureNet & newNet, StructureCircuits & circuits, StructureCircuit & circuit) const
	{
		if(hasInstanceTerminals())
		{
			for(std::vector<StructureInstanceTerminal*>::const_iterator it_terminals = instanceTerminals_.begin(); it_terminals != instanceTerminals_.end(); it_terminals++)
			{
				StructureInstanceTerminal * oldInstanceTerminal = * it_terminals;
				Core::InstanceId instanceId = oldInstanceTerminal->getInstance().getId();

				StructureInstanceTerminal * newInstanceTerminal = new StructureInstanceTerminal;
				newInstanceTerminal->connect(newNet);
				newInstanceTerminal->setInstance(circuits.findInstance(instanceId));

				Core::TerminalId terminalId = oldInstanceTerminal->getTerminalId();
				int level = circuit.getHierarchyLevel();
				StructureTerminal & connectedTerminalInSubCircuit = circuits.findInstance(instanceId).getSubCircuits().findStructureCircuit(level).findTerminal(terminalId);
				newInstanceTerminal->setTerminal(connectedTerminalInSubCircuit);

				circuit.addInstanceTerminal(*newInstanceTerminal);
			}
		}
	}

	const std::vector<StructurePin*> StructureNet::getConnectedPins() const
	{
		assert(isConnected(), " Net must have pins!");
		StructurePinVector connectedPins;
		for(auto & i : connectedPins_)
		{
			StructurePinVector partOfPins = i.second;
			connectedPins.insert(connectedPins.end(), partOfPins.begin(), partOfPins.end());
		}

		return connectedPins;
	}

}

