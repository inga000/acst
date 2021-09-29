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


#include "AutomaticSizing/incl/ConstraintProgram/Graph/Node.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Paths.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include <stack>
#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

	Graph::Graph() :
			paths_(NULL),
			circuitParameter_(NULL)
	{
	}

	Graph::~Graph()
	{
		eraseNodes();
		eraseEdges();
	}


	void Graph::addEdges(Partitioning::Component & component)
	{

		std::vector<StructRec::StructurePin*> Pin = component.getArray().findAllPins();


		for(std::vector<StructRec::StructurePin*>::const_iterator it_Pin1 = Pin.begin(); it_Pin1 != Pin.end(); it_Pin1++)
		{
			StructRec::StructurePin  * pin1 = *it_Pin1;

			StructRec::StructurePinType pinType1 = pin1->getPinType();
			if(pinType1.getPinName() != StructRec::StructurePinName("Bulk"))
			{
				for(std::vector<StructRec::StructurePin*>::const_iterator it_Pin2 = it_Pin1+1; it_Pin2 != Pin.end(); it_Pin2++)
				{
					StructRec::StructurePin * pin2 = *it_Pin2;

					StructRec::StructurePinType pinType2 = pin2->getPinType();
					if(pinType2.getPinName() != StructRec::StructurePinName("Bulk"))
					{
						const EdgeKey & edgeKey = computeEdgeKey(pinType1, pinType2, component);
						addEdge(edgeKey);
					}

				}
			}
		}
	}

	void Graph::addEdge(const EdgeKey& edgeKey)
	{
		Core::NetId startNodeId = edgeKey.getStartNodeId();
		if(!hasNode(startNodeId)) {
			addNode(startNodeId);
		}

		Core::NetId endNodeId = edgeKey.getEndNodeId();
		if(!hasNode(endNodeId)) {
			addNode(endNodeId);
		}

		Node& startNode = findNode(startNodeId);
		Node& endNode = findNode(endNodeId);
		if(startNode != endNode) {
			Edge* newEdge = new Edge();
			newEdge->setStartNode(startNode);
			newEdge->setEndNode(endNode);
			newEdge->setEdgeKey(edgeKey);
			edgeMap_.insert(EdgeMap::value_type(edgeKey, newEdge));
			startNode.addOutgoingEdge(*newEdge);
			endNode.addIncomingEdge(*newEdge);
		}
	}
	const EdgeKey & Graph::computeEdgeKey(StructRec::StructurePinType & pinType1, StructRec::StructurePinType pinType2, Partitioning::Component & component)
	{
		EdgeKey * edgeKey = new EdgeKey;
		edgeKey->setComponent(component);
		if(pinType1.getPinName() == StructRec::StructurePinName("Gate"))
		{
			edgeKey->setStartPinType(pinType1);
			edgeKey->setEndPinType(pinType2);
		}
		else if(pinType2.getPinName() == StructRec::StructurePinName("Gate"))
		{
			edgeKey->setStartPinType(pinType2);
			edgeKey->setEndPinType(pinType1);
		}
		else if(pinType2.getPinName() == StructRec::StructurePinName("Drain"))
		{
			edgeKey->setStartPinType(pinType2);
			edgeKey->setEndPinType(pinType1);
		}
		else if(pinType2.getPinName() == StructRec::StructurePinName("Plus"))
		{
			edgeKey->setStartPinType(pinType2);
			edgeKey->setEndPinType(pinType1);
		}
		else
		{
			edgeKey->setStartPinType(pinType1);
			edgeKey->setEndPinType(pinType2);

		}
		return * edgeKey;
	}

	std::string Graph::toStr() const
	{
		std::ostringstream oss;
		oss << "Nodes:\n";
		for(NodeMap::const_iterator it = nodeMap_.begin(); it != nodeMap_.end(); it++)
		{
			Node* node = it->second;
			oss << *node << "\n";
		}
		oss << "Edges:\n";
		for(EdgeMap::const_iterator it = edgeMap_.begin(); it != edgeMap_.end(); it++)
		{
			Edge* edge = it->second;
			oss << *edge << "\n";
		}
		return oss.str();
	}

	void Graph::addNode(const Core::NetId& netId)
	{
		assert(!hasNode(netId));
		Node* newNode = new Node();
		newNode->setNetId(netId);
		if(getCircuitParameter().isInputNet(netId))
		{
			newNode->setInputNode();
		}
		else if(getCircuitParameter().isCurrentBiasNet(netId))
		{
			newNode->setBiasNode();
		}
		else if(getCircuitParameter().isOutputNet(netId))
		{
			newNode->setOutputNode();
		}
		else if(getCircuitParameter().isGroundNet(netId))
		{
			newNode->setGroundNode();
		}
		else if(getCircuitParameter().isSupplyNet(netId))
		{
			newNode->setSupplyNode();
		}
		nodeMap_.insert(NodeMap::value_type(netId, newNode));
	}

	bool Graph::hasNode(const Core::NetId& netId) const
	{
		return nodeMap_.find(netId) != nodeMap_.end();
	}

	Node& Graph::findNode(const Core::NetId& netId)
	{
		assert(hasNode(netId));
		Node* node = nodeMap_[netId];
		return *node;
	}

	const Node & Graph::findNode(const Core::NetId & netId) const
	{
		assert(hasNode(netId));
		const Node* node = nodeMap_.at(netId);
		return *node;
	}

	void Graph::computeVddToGroundPaths()
	{
		const Pin &  supplyPin = getCircuitParameter().getSupplyVoltagePin();
		const Pin & groundPin = getCircuitParameter().getGroundPin();


		Path path;
		Node& startNode = findNode(supplyPin.getNetId());
		Node& endNode = findNode(groundPin.getNetId());
		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsRecursively(endNode, path, getPaths());

	}

	void Graph::setCircuitParameter(const CircuitParameter& parameter)
	{
		circuitParameter_ = & parameter;
	}

	const CircuitParameter& Graph::getCircuitParameter() const
	{
		assert(circuitParameter_ != NULL);
		return * circuitParameter_;
	}

	void Graph::computeInputToGroundPaths()
	{
		const Pin & inputPinMinus = getCircuitParameter().getInputPinMinus();
		const Pin & inputPinPlus = getCircuitParameter().getInputPinPlus();
		const Pin & groundPin = getCircuitParameter().getGroundPin();

		Path path1;
		Path path2;
		Node& startNode1 = findNode(inputPinMinus.getNetId());
		Node& startNode2 = findNode(inputPinPlus.getNetId());
		Node & endNode = findNode(groundPin.getNetId());
		path1.setStartNode(startNode1);
		path2.setStartNode(startNode2);
		path1.setEndNode(endNode);
		path2.setEndNode(endNode);
		startNode1.computePathsRecursively(endNode, path1, getPaths());
		startNode2.computePathsRecursively(endNode, path2, getPaths());
	}

	void Graph::computeOutputToGroundPaths()
	{
		const Pin * outputPin = nullptr;
		if(getCircuitParameter().isFullyDifferential())
		{
			outputPin = &getCircuitParameter().getOutputPinMinus();
		}
		else
		{
			outputPin = &getCircuitParameter().getOutputPin();
		}

		const Pin & groundPin = getCircuitParameter().getGroundPin();

		Path path;
		Node& startNode = findNode(outputPin->getNetId());
		Node & endNode = findNode(groundPin.getNetId());
		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsRecursively(endNode, path, getPaths());

	}

	std::vector<Path> Graph::computeVddToOuputPathsWithOnlyDrainSourceEdges() const
	{
		std::vector<Path> paths;
		const Pin & supplyPin = getCircuitParameter().getSupplyVoltagePin();
		const Pin * outputPin = nullptr;
		if(getCircuitParameter().isFullyDifferential())
		{
			outputPin = &getCircuitParameter().getOutputPinMinus();
		}
		else
		{
			outputPin = &getCircuitParameter().getOutputPin();
		}

		Path path;
		const Node& startNode = findNode(supplyPin.getNetId());
		const Node & endNode = findNode(outputPin->getNetId());
		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;

	}



	std::vector<Path> Graph::computeOutputToGroundPathsWithOnlyDrainSourceEdges() const
	{
		std::vector<Path> paths;
		const Pin * outputPin = nullptr;
		if(getCircuitParameter().isFullyDifferential())
		{
			outputPin = &getCircuitParameter().getOutputPinMinus();
		}
		else
		{
			outputPin = &getCircuitParameter().getOutputPin();
		}
		const Pin & groundPin = getCircuitParameter().getGroundPin();

		Path path;
		const Node& startNode = findNode(outputPin->getNetId());
		const Node & endNode = findNode(groundPin.getNetId());
		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;
	}

	std::vector<Path> Graph::computeInputMinusToVddPathWithOnlyDrainSourceAndGateSourceEdges() const
	{
		std::vector<Path> paths;
		const Pin & supplyPin = getCircuitParameter().getSupplyVoltagePin();
		const Pin & inputPinMinus = getCircuitParameter().getInputPinMinus();

		Path path;

		const Node& startNode = findNode(inputPinMinus.getNetId());
		const Node & endNode = findNode(supplyPin.getNetId());


		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;
	}

	std::vector<Path> Graph::computeInputMinusToGroundPathWithOnlyDrainSourceAndGateSourceEdges() const
	{
		std::vector<Path> paths;

		const Pin & groundPin = getCircuitParameter().getGroundPin();
		const Pin & inputPinMinus = getCircuitParameter().getInputPinMinus();


		Path path;
		const Node& startNode = findNode(inputPinMinus.getNetId());
		const Node & endNode = findNode(groundPin.getNetId());


		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;
	}

	std::vector<Path> Graph::computeInputPlusToVddPathWithOnlyDrainSourceAndGateSourceEdges() const
	{
		std::vector<Path> paths;
		const Pin & supplyPin = getCircuitParameter().getSupplyVoltagePin();
		const Pin & inputPinPlus = getCircuitParameter().getInputPinPlus();

		Path path;

		const Node& startNode = findNode(inputPinPlus.getNetId());
		const Node & endNode = findNode(supplyPin.getNetId());


		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;
	}

	std::vector<Path> Graph::computeInputPlusToGroundPathWithOnlyDrainSourceAndGateSourceEdges() const
	{
		std::vector<Path> paths;

		const Pin & groundPin = getCircuitParameter().getGroundPin();
		const Pin & inputPinPlus = getCircuitParameter().getInputPinPlus();


		Path path;
		const Node& startNode = findNode(inputPinPlus.getNetId());
		const Node & endNode = findNode(groundPin.getNetId());


		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNode, path, paths);

		assert(!paths.empty(), "No Paths found!");
		return paths;
	}

	void Graph::computeVddToBiasPaths()
	{
		const Pin & supplyPin = getCircuitParameter().getSupplyVoltagePin();
		const Pin & biasPin = getCircuitParameter().getCurrentBiasPin();

		Path path;
		Node& startNode = findNode(supplyPin.getNetId());
		Node & endNode = findNode(biasPin.getNetId());
		path.setStartNode(startNode);
		path.setEndNode(endNode);
		startNode.computePathsRecursively(endNode, path, getPaths());
	}

	void Graph::equalizeParallelEdges()
	{
		for(EdgeMap::const_iterator it1 = edgeMap_.begin(); it1 != edgeMap_.end(); it1++)
		{
			Edge* edge1 = it1->second;

			for(EdgeMap::const_iterator it2 = std::next(it1); it2 != edgeMap_.end(); it2++)
			{
				Edge* edge2 = it2->second;
				if(!edge1->isParallel() &&
					( (edge1->getEndNodeId() == edge2->getEndNodeId() && edge1->getStartNodeId() == edge2->getStartNodeId())
					|| (edge1->getStartNodeId() == edge2->getEndNodeId() && edge1->getEndNodeId() == edge2->getStartNodeId()) ))
				{
					edge2->setParallel();
					edge1->addParallelEdge(*edge2);
				}
			}
		}
	}

	const std::vector<Node*> Graph::getAllNodes() const
	{
		std::vector<Node*> nodes;
		for(NodeMap::const_iterator it = nodeMap_.begin(); it != nodeMap_.end(); it++)
		{
			Node * node = it->second;
			nodes.push_back(node);
		}
		return nodes;
	}

	std::vector<Edge*> Graph::findToComponentBelongingEdges(Partitioning::Component & component) const
	{
		std::vector<Edge*> componentEdges;
		for(EdgeMap::const_iterator it = edgeMap_.begin(); it != edgeMap_.end(); it++)
		{
			EdgeKey edgeKey = it->first;
			if(edgeKey.getStructureId() == component.getArray().getIdentifier())
			{
				componentEdges.push_back(it->second);
			}
		}
		return componentEdges;
	}


	const Node & Graph::getBiasNode() const
	{
		Node * biasNode = NULL;
		const std::vector<Node*> nodes = getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = * it;
			if(node->isBiasNode())
			{
				biasNode = node;
				break;
			}
		}
		assert(biasNode != NULL, "No biasNode in the circuit");
		return *biasNode;
	}

	const Node & Graph::getOutputNode() const
	{
		Node * outputNode = NULL;
		const std::vector<Node*> nodes = getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = * it;
			if(node->isOutputNode())
			{
				outputNode = node;
				break;
			}
		}
		assert(outputNode != NULL, "No outputNode in the circuit");
		return *outputNode;
	}


	const Node & Graph::getSupplyNode() const
	{
		Node * supplyNode = NULL;
		const std::vector<Node*> nodes = getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = * it;
			if(node->isSupplyNode())
			{
				supplyNode = node;
				break;
			}
		}
		assert(supplyNode != NULL, "No supplyNode in the circuit");
		return *supplyNode;
	}

	const Node & Graph::getGroundNode() const
	{
		Node * groundNode = NULL;
		const std::vector<Node*> nodes = getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = * it;
			if(node->isGroundNode())
			{
				groundNode = node;
				break;
			}
		}
		assert(groundNode != NULL, "No groundNode in the circuit");
		return *groundNode;
	}


	void Graph::eraseNodes()
	{
		for(NodeMap::iterator it = nodeMap_.begin(); it != nodeMap_.end(); it++)
		{
			Node * node = it->second;
			delete node;
		}
	}

	const Paths& Graph::getPaths() const
	{
		assert(paths_ != NULL);
		return * paths_;
	}

	void Graph::setPaths(Paths& path)
	{
		paths_ = &path;
	}

	Paths& Graph::getPaths()
	{
		assert(paths_ != NULL);
		return * paths_;
	}

	void Graph::eraseEdges()
	{
		for(EdgeMap::iterator it = edgeMap_.begin(); it != edgeMap_.end(); it++)
		{
			Edge * edge = it->second;
			delete edge;
		}
	}


}

