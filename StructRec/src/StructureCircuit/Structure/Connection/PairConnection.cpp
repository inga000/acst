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
#include "StructRec/incl/StructureCircuit/Structure/Connection/PairConnection.h"
#include "StructRec/incl/StructureCircuit/Net/PairNet.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PairPin.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Net/Supply.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstancePair.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace StructRec {

	PairConnection::PairConnection()
	{

	}

	PairConnection::PairConnection(const PairConnection& other):
		topPinToChild1PinConnection_(other.topPinToChild1PinConnection_),
		topPinToChild2PinConnection_(other.topPinToChild2PinConnection_)
	{

	}

	PairConnection::~PairConnection()
	{

	}

	void PairConnection::initialize(StructureName newPair ,const NewRecognizedChild & child1, const NewRecognizedChild & child2)
	{
		std::vector<StructurePin*> pinsOfChild1 = child1.getStructure().findAllPins();
		for(std::vector<StructurePin*>::iterator it = pinsOfChild1.begin(); it != pinsOfChild1.end(); it ++)
		{
			const StructurePin * childPin = *it;

			if(childPin->getPinType().getPinName().toStr() != "Bulk" )
			{
				std::string name = createPinName(childPin->getNet().getIdentifier().toStr());

				StructurePinName pinName(name);
				StructurePinType topPin(newPair, pinName);
				addConnection(topPin, childPin->getPinType(), 1);
			}
		}
		std::vector<StructurePin*> pinsOfChild2 = child2.getStructure().findAllPins();
		for(std::vector<StructurePin*>::iterator it = pinsOfChild2.begin(); it != pinsOfChild2.end(); it ++)
		{

			const StructurePin * childPin = *it;
			if(childPin->getPinType().getPinName().toStr() != "Bulk" )
			{
				std::string name = createPinName(childPin->getNet().getIdentifier().toStr());
				StructurePinName pinName(name);
				StructurePinType topPin(newPair, pinName);
				if(!hasChild1Pin(topPin))
				{
					addConnection(topPin, childPin->getPinType(), 2);

				}
			}
		}
	}


	void PairConnection::addConnection(const StructurePinType& topPin,
		const StructurePinType& childPin, const int child)
	{
		assert((child==1 || child==2));
		if(child == 1)
		{
			topPinToChild1PinConnection_.insert(std::pair<StructurePinType,StructurePinType>(topPin, childPin));
		}
		else
		{
			topPinToChild2PinConnection_.insert(std::pair<StructurePinType,StructurePinType>(topPin, childPin));
		}
	}

	StructurePinType PairConnection::findChild1PinType(const StructurePinType& topPin) const
	{
		assert(hasChild1Pin(topPin));
		return topPinToChild1PinConnection_.at(topPin);
	}

	StructurePinType PairConnection::findChild2PinType(const StructurePinType& topPin) const
	{
		assert(hasChild2Pin(topPin));
		return topPinToChild2PinConnection_.at(topPin);
	}

	bool PairConnection::hasChild1Pin(const StructurePinType& topPin) const
	{
		return topPinToChild1PinConnection_.find(topPin) != topPinToChild1PinConnection_.end();
	}

	bool PairConnection::isIdentical(const PairConnection& other, const LibraryItemMappings& mappings) const
	{
		bool sameChildOnePinConnections = hasIdenticalChildPinTypes(topPinToChild1PinConnection_, other.topPinToChild1PinConnection_, mappings);
		bool sameChildTwoPinConnections = hasIdenticalChildPinTypes(topPinToChild2PinConnection_, other.topPinToChild2PinConnection_, mappings);
		return sameChildOnePinConnections && sameChildTwoPinConnections;
	}

	LibraryItemMapping PairConnection::creatNewMapping(const PairConnection& other, const LibraryItemMappings& mappings) const
	{
		assert(isIdentical(other, mappings));
		LibraryItemMapping newMapping;
		createPinMapping(topPinToChild1PinConnection_, other.topPinToChild1PinConnection_, mappings, newMapping);
		createPinMapping(topPinToChild2PinConnection_, other.topPinToChild2PinConnection_, mappings, newMapping);
		return newMapping;
	}

	void PairConnection::update(const LibraryItemMappings& mappings)
	{
		std::map<StructurePinType, StructurePinType>   updatedTopPinToChild1PinConnectionMap;
		for(std::map<StructurePinType, StructurePinType>::const_iterator it = topPinToChild1PinConnection_.begin(); it != topPinToChild1PinConnection_.end(); it++)
		{
			StructurePinType topPinType = it->first;
			StructurePinType updatedChildPinType = mappings.findMappedPinType(it->second);
			updatedTopPinToChild1PinConnectionMap.insert(std::map<StructurePinType, StructurePinType>::value_type(topPinType, updatedChildPinType));
		}
		topPinToChild1PinConnection_ = updatedTopPinToChild1PinConnectionMap;

		std::map<StructurePinType, StructurePinType>   updatedTopPinToChild2PinConnectionMap;
		for(std::map<StructurePinType, StructurePinType>::const_iterator it = topPinToChild2PinConnection_.begin(); it != topPinToChild2PinConnection_.end(); it++)
		{
			StructurePinType topPinType = it->first;
			StructurePinType updatedChildPinType = mappings.findMappedPinType(it->second);
			updatedTopPinToChild2PinConnectionMap.insert(std::map<StructurePinType, StructurePinType>::value_type(topPinType, updatedChildPinType));
		}
		topPinToChild2PinConnection_ = updatedTopPinToChild2PinConnectionMap;
	}

	void PairConnection::createPinMapping(const std::map<StructurePinType, StructurePinType>& original, const std::map<StructurePinType, StructurePinType>& other,
			const LibraryItemMappings& mappings, LibraryItemMapping& newMapping) const
	{
		for(std::map<StructurePinType, StructurePinType>::const_iterator it = other.begin(); it != other.end(); it++)
		{
			StructurePinType childPinType = it->second;
			StructurePinType mappedChildPinType = mappings.findMappedPinType(childPinType);

			for(std::map<StructurePinType, StructurePinType>::const_iterator it2 = original.begin(); it2 != original.end(); it2++)
			{
				StructurePinType originalChildPinType = it2->second;
				if(mappedChildPinType == originalChildPinType) {
					StructurePinType pinType = it->first;
					StructurePinType mappedPinType = it2->first;
					newMapping.addPinMapping(pinType, mappedPinType);
					break;
				}
			}
		}
	}

	bool PairConnection::hasIdenticalChildPinTypes(const std::map<StructurePinType, StructurePinType>& original, const std::map<StructurePinType, StructurePinType>& other,
			const LibraryItemMappings& mappings ) const
	{
		bool sameChildPinTypes = true;
		for(std::map<StructurePinType, StructurePinType>::const_iterator it = other.begin(); it != other.end(); it++)
		{
			StructurePinType childPinType = it->second;
			StructurePinType mappedChildPinType = mappings.findMappedPinType(childPinType);

			bool hasChild = false;
			for(std::map<StructurePinType, StructurePinType>::const_iterator it2 = original.begin(); it2 != original.end(); it2++)
			{
				StructurePinType originalChildPinType = it2->second;
				if(mappedChildPinType == originalChildPinType) {
					hasChild = true;
					break;
				}
			}

			if(!hasChild) {
				sameChildPinTypes = false;
				break;
			}
		}
		return sameChildPinTypes;
	}

	bool PairConnection::hasChild2Pin(const StructurePinType& topPin) const
	{
		return topPinToChild2PinConnection_.find(topPin) != topPinToChild2PinConnection_.end();
	}

	void PairConnection::connectPair(StructureCircuit& circuit, Pair& pair) const
	{
		connectPairToChild1(circuit, pair);
		connectPairToChild2(circuit, pair);
	}

	void PairConnection::connectCrossInstancePair(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const
	{
		if(tempPair.getChild1().isInTopCircuit())
		{
			connectPairToChild1(circuit, pair);
		}
		if(tempPair.getChild2().isInTopCircuit())
		{
			connectPairToChild2(circuit, pair);
		}
		if(!tempPair.getChild1().isInTopCircuit())
		{
			connectPairToCrossInstanceChild1(circuit, pair, tempPair);
		}
		if(!tempPair.getChild2().isInTopCircuit())
		{
			connectPairToCrossInstanceChild2(circuit, pair, tempPair);
		}
	}

	StructureName PairConnection::getFirstChildName() const
	{
		assert(isInitialized());
		StructurePinType firstChildPinType = topPinToChild1PinConnection_.begin()->second;
		return firstChildPinType.getStructureName();
	}

	StructureName PairConnection::getSecondChildName() const
	{
		assert(isInitialized());
		StructurePinType secondChildPinType = topPinToChild2PinConnection_.begin()->second;
		return secondChildPinType.getStructureName();
	}

	bool PairConnection::isInitialized() const
	{
		return !(topPinToChild1PinConnection_.empty()) || !(topPinToChild2PinConnection_.empty());
	}

	const std::map<StructurePinType, StructurePinType>& PairConnection::getTopPinToChild1PinConnection() const
	{
		assert(isInitialized());
		return topPinToChild1PinConnection_;
	}

	const std::map<StructurePinType, StructurePinType>& PairConnection::getTopPinToChild2PinConnection() const
	{
		assert(isInitialized());
		return topPinToChild2PinConnection_;
	}

	StructureNet& PairConnection::findOrCreatePairNet(StructureCircuit& circuit, const StructureNet& childNet) const
    {
        StructureNet* newPairNet;

        Core::NetId childNetId = childNet.getIdentifier();
        if(circuit.hasStructureNet(childNetId))
        {
            newPairNet = &circuit.findStructureNet(childNetId);
        }
        else
        {
            newPairNet = &createNewPairNet(childNet);
            circuit.addStructureNet(*newPairNet);
        }
        return *newPairNet;
    }


	Core::NetId PairConnection::createNewNetId(Structure & structure, Core::NetId childNet,Core::InstanceNamePath childNamePath) const
	{
		Core::NetId netId;

		Core::NetName netName(childNet.getName());
		netId.setPath(childNamePath);
		netId.setName(netName);
		return netId;
	}

	PairNet& PairConnection::createNewPairNet(const StructureNet& net) const
	{
		PairNet* newNet = new PairNet;
		newNet->setNetId(net.getIdentifier());
		newNet->setSupply(net.getSupply());
		newNet->setPairNet(net);
		return *newNet;
	}

	PairNet & PairConnection::createNewEmptyNet(Structure & structure, const StructureNet & childNet, Core::InstanceNamePath childNamePath) const
	{
		PairNet* newNet = new PairNet;
		Core::NetId newNetId = createNewNetId(structure, childNet.getIdentifier(), childNamePath);
		newNet->setNetId(newNetId);
		newNet->setSupply(childNet.getSupply());
		return *newNet;
	}

	void PairConnection::connectPairToChild1(StructureCircuit& circuit, Pair& pair) const
	{
        pair.getChild1().setParent(pair);
		for (std::map<StructurePinType, StructurePinType>::const_iterator it=getTopPinToChild1PinConnection().begin();
				it != getTopPinToChild1PinConnection().end(); ++it)
		{
		    StructurePinType pairPinType = it->first;
		    StructurePinType childPinType = it->second;
		    StructurePin& childPin = pair.findChild1Pin(childPinType);
		    StructureNet& childNet = childPin.getNet();
		    StructureNet& pairNet = findOrCreatePairNet(circuit, childNet);

			PairPin *newPin = new PairPin;
            newPin->setPinType(pairPinType);
			newPin->setChildPin(childPin);
			newPin->setStructure(pair);
            newPin->setNet(pairNet);

            pairNet.addPin(*newPin);
			pair.addPin(pairPinType,newPin);
		}
	}

	void PairConnection::connectPairToChild2(StructureCircuit& circuit, Pair& pair) const
	{
        pair.getChild2().setParent(pair);
		for (std::map<StructurePinType, StructurePinType>::const_iterator it=getTopPinToChild2PinConnection().begin();
								it != getTopPinToChild2PinConnection().end(); ++it)
		{
		    StructurePinType pairPinType = it->first;
            StructurePinType childPinType = it->second;
            StructurePin& childPin = pair.findChild2Pin(childPinType);
            StructureNet& childNet = childPin.getNet();
            StructureNet& pairNet = findOrCreatePairNet(circuit, childNet);

            PairPin *newPin = new PairPin;
            newPin->setPinType(pairPinType);
            newPin->setChildPin(childPin);
            newPin->setStructure(pair);
            newPin->setNet(pairNet);

            pairNet.addPin(*newPin);
            pair.addPin(pairPinType,newPin);
		}
	}

	void PairConnection::connectPairToCrossInstanceChild1(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const
	{
		pair.getChild1().setParent(pair);
		for (std::map<StructurePinType, StructurePinType>::const_iterator it=getTopPinToChild1PinConnection().begin();
				it != getTopPinToChild1PinConnection().end(); ++it)
		{
			StructurePinType pairPinType = it->first;
			StructurePinType childPinType = it->second;
			StructurePin& childPin = pair.findChild1Pin(childPinType);
			PairPin *newPin = new PairPin;
			newPin->setPinType(pairPinType);
			newPin->setChildPin(childPin);
			newPin->setStructure(pair);

			if(tempPair.getChild1().hasTopCircuitConnection(childPinType))
			{
				StructureNet & pairNet = findOrCreatePairNet(circuit, tempPair.getChild1().findConnectedInstanceTerminal(childPinType).getNet());
				newPin->setNet(pairNet);
				pairNet.addPin(*newPin);
			}
			else
			{
				StructureNet & pairNet = createNewEmptyNet(pair, tempPair.getChild1().getStructure().findNet(childPinType), tempPair.getChild1().getInstanceNamePath());
				circuit.addStructureNet(pairNet);
				newPin->setNet(pairNet);
				pairNet.addPin(*newPin);
			}


			pair.addPin(pairPinType,newPin);
		}

	}

	void PairConnection::connectPairToCrossInstanceChild2(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const
	{
		pair.getChild2().setParent(pair);
		for (std::map<StructurePinType, StructurePinType>::const_iterator it=getTopPinToChild2PinConnection().begin();
					it != getTopPinToChild2PinConnection().end(); ++it)
		{
			StructurePinType pairPinType = it->first;
			StructurePinType childPinType = it->second;
			StructurePin& childPin = pair.findChild2Pin(childPinType);
			PairPin *newPin = new PairPin;
			newPin->setPinType(pairPinType);
			newPin->setChildPin(childPin);
			newPin->setStructure(pair);

			if(tempPair.getChild2().hasTopCircuitConnection(childPinType))
			{
				StructureNet & pairNet = findOrCreatePairNet(circuit, tempPair.getChild2().findConnectedInstanceTerminal(childPinType).getNet());
				newPin->setNet(pairNet);
				pairNet.addPin(*newPin);
			}
			else
			{
				StructureNet & pairNet = createNewEmptyNet(pair, tempPair.getChild2().getStructure().findNet(childPinType), tempPair.getChild2().getInstanceNamePath());
				circuit.addStructureNet(pairNet);
				newPin->setNet(pairNet);
				pairNet.addPin(*newPin);

			}
			pair.addPin(pairPinType,newPin);
		}
	}

	std::vector<StructurePinType> PairConnection::getStructurePins() const
	{
		std::vector<StructurePinType> pinTypeVector;
		if(!topPinToChild1PinConnection_.empty()){
			for(auto& it : topPinToChild1PinConnection_)
			{
				pinTypeVector.push_back(it.first);
			}
		}
		if(!topPinToChild2PinConnection_.empty()){
			for(auto& it : topPinToChild2PinConnection_)
			{
				pinTypeVector.push_back(it.first);
			}
		}
		return pinTypeVector;
	}

	void PairConnection::writeXml(Core::XmlNode& node,
			Core::XmlDocument& doc) const
	{
		Core::XmlNode& pairConnection = Core::RapidXmlUtils::addNode(node, doc, "pairConnection");

		const std::map<StructurePinType, StructurePinType>  & topPinToChild1PinConnection = getTopPinToChild1PinConnection();
		const std::map<StructurePinType, StructurePinType> &  topPinToChild2PinConnection = getTopPinToChild2PinConnection();

		for(std::map<StructurePinType, StructurePinType>::const_iterator it_child1 = topPinToChild1PinConnection.begin(); it_child1 != topPinToChild1PinConnection.end(); it_child1++)
		{
			StructurePinType pairPin = it_child1->first;
			StructurePinType childPin = it_child1->second;

			Core::XmlNode& pairPinType = Core::RapidXmlUtils::addNode(pairConnection, doc, "pairPinType");
			Core::XmlNode& structurePinType = Core::RapidXmlUtils::addNode(pairPinType, doc, "structurePinType");
			Core::RapidXmlUtils::addSimpleNode(structurePinType, doc, "structureName", pairPin.getStructureName().toStr());
			Core::RapidXmlUtils::addSimpleNode(structurePinType, doc, "structurePinName", pairPin.getPinName().toStr());

			Core::XmlNode& childPinType = Core::RapidXmlUtils::addNode(pairPinType, doc, "childPinType");

			Core::RapidXmlUtils::addSimpleNode(childPinType, doc, "childNumber", "1");

			Core::XmlNode& childStructurePinType = Core::RapidXmlUtils::addNode(childPinType, doc, "structurePinType");
			Core::RapidXmlUtils::addSimpleNode(childStructurePinType, doc, "structureName", childPin.getStructureName().toStr());
			Core::RapidXmlUtils::addSimpleNode(childStructurePinType, doc, "structurePinName", childPin.getPinName().toStr());

		}

		for(std::map<StructurePinType, StructurePinType>::const_iterator it_child2 = topPinToChild2PinConnection.begin(); it_child2 != topPinToChild2PinConnection.end(); it_child2++)
		{
			StructurePinType pairPin = it_child2->first;
			StructurePinType childPin = it_child2->second;

			Core::XmlNode& pairPinType = Core::RapidXmlUtils::addNode(pairConnection, doc, "pairPinType");
			Core::XmlNode& structurePinType = Core::RapidXmlUtils::addNode(pairPinType, doc, "structurePinType");
			Core::RapidXmlUtils::addSimpleNode(structurePinType, doc, "structureName", pairPin.getStructureName().toStr());
			Core::RapidXmlUtils::addSimpleNode(structurePinType, doc, "structurePinName", pairPin.getPinName().toStr());

			Core::XmlNode& childPinType = Core::RapidXmlUtils::addNode(pairPinType, doc, "childPinType");

			Core::RapidXmlUtils::addSimpleNode(childPinType, doc, "childNumber", "2");

			Core::XmlNode& childStructurePinType = Core::RapidXmlUtils::addNode(childPinType, doc, "structurePinType");
			Core::RapidXmlUtils::addSimpleNode(childStructurePinType, doc, "structureName", childPin.getStructureName().toStr());
			Core::RapidXmlUtils::addSimpleNode(childStructurePinType, doc, "structurePinName", childPin.getPinName().toStr());

		}
	}

	std::string PairConnection::createPinName(std::string netName) const
	{
		size_t positionOfLastSlash = netName.rfind(std::string("/"));
		if(positionOfLastSlash != std::string::npos)
		{
			std::string nameWithoutSlash = netName.substr(positionOfLastSlash+1);
			return nameWithoutSlash;
		}
		else
		{
			return netName;
		}
	}
}
