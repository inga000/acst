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


#include "AutomaticSizing/incl/ConstraintProgram/Graph/Paths.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Path.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"

namespace AutomaticSizing {



	Node::Node() :
			visited_(false),
			isSupplyNode_(false),
			isGroundNode_(false),
			isBiasNode_(false),
			isInputNode_(false),
			isOutputNode_(false)
	{
	}

	Node::~Node()
	{
	}

	void Node::setNetId(const Core::NetId& netId)
	{
		netId_ = netId;
	}


	void Node::addIncomingEdge(Edge& edge)
	{
		incomingEdges_.insert(&edge);
	}

	void Node::addOutgoingEdge(Edge& edge)
	{
		outgoingEdges_.insert(&edge);
	}

	void Node::setVisited()
	{
		visited_ = true;
	}

	void Node::setSupplyNode()
	{
		isSupplyNode_ = true;
	}

	void Node::setGroundNode()
	{
		isGroundNode_ = true;
	}

	void Node::setBiasNode()
	{
		isBiasNode_ = true;
	}

	void Node::setInputNode()
	{
		isInputNode_ = true;
	}

	void Node::setOutputNode()
	{
		isOutputNode_ = true;
	}

	void Node::setNotVisited()
	{
		visited_ = false;
	}

	bool Node::isSupplyNode() const
	{
		return isSupplyNode_;
	}

	bool Node::isGroundNode() const
	{
		return isGroundNode_;
	}

	bool Node::isBiasNode() const
	{
		return isBiasNode_;
	}

	bool Node::isInputNode() const
	{
		return isInputNode_;
	}

	bool Node::isOutputNode() const
	{
		return isOutputNode_;
	}

	bool Node::isVisited() const
	{
		return visited_;
	}

	bool Node::isSpecificNode() const
	{
		return isOutputNode_ || isInputNode_ || isBiasNode_ || isSupplyNode_ || isGroundNode_;
	}

	Core::NetId Node::getNetId() const
	{
		assert(netId_.isInitialized());
		return netId_;
	}

	bool Node::operator ==(const Node& other) const
	{
		return getNetId() == other.getNetId();
	}

	bool Node::operator<(const Node& other) const
	{
		return getNetId() < other.getNetId();
	}

	bool Node::operator !=(const Node& other) const
	{
		return getNetId() != other.getNetId();
	}

	std::string Node::toStr() const
	{
		std::ostringstream oss;
		oss << "(" << getNetId() << ",";
		oss << ")";
		return oss.str();
	}


	void Node::computePathsRecursively(const Node& endNodeOfPath, Path & currentPath, Paths& paths) const
	{
		if(getNetId() == endNodeOfPath.getNetId())
		{
			paths.add(currentPath);
			return;
		}


		for(EdgeSet::const_iterator it = outgoingEdges_.begin(); it != outgoingEdges_.end(); it++)
		{
			Edge* edge = *it;
			if(!edge->isParallel())
			{
				Node& endNode = edge->getEndNode();
				if(!currentPath.hasNode(edge->getEndNodeId()) && !(endNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(endNode.isGroundNode() && !endNodeOfPath.isGroundNode()) )
				{
					currentPath.add(*edge);
					endNode.computePathsRecursively(endNodeOfPath, currentPath, paths);
					currentPath.pop_back();
				}
			}
		}
		for(EdgeSet::const_iterator it = incomingEdges_.begin(); it != incomingEdges_.end(); it++)
		{
			Edge* edge = *it;
			if(!edge->isParallel())
			{
				Node& startNode = edge->getStartNode();
				if(!currentPath.hasNode(edge->getStartNodeId()) && !(startNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(startNode.isGroundNode() && !endNodeOfPath.isGroundNode()))
				{
					currentPath.add(*edge);
					startNode.computePathsRecursively(endNodeOfPath, currentPath, paths);
					currentPath.pop_back();
				}
			}
		}

	}


	void Node::computePathsOnlyWithDrainSourceEdgesRecursively(const Node & endNodeOfPath,Path & currentPath, std::vector<Path> & paths) const
	{
		if(getNetId() == endNodeOfPath.getNetId())
		{
			paths.push_back(currentPath);
			return;
		}

		Core::TechType techType;

		if(currentPath.hasAnEdge())
		{
			techType = (*currentPath.getEdges().begin())->getEdgeKey().getComponent().getArray().getTechType();
		}

		for(EdgeSet::const_iterator it = outgoingEdges_.begin(); it != outgoingEdges_.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge() &&  (!techType.isInitialized() || edge->getEdgeKey().getComponent().getArray().getTechType() == techType ) )
			{
				Node& endNode = edge->getEndNode();
				if(!currentPath.hasNode(edge->getEndNodeId()) && !(endNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(endNode.isGroundNode() && !endNodeOfPath.isGroundNode()))
				{
					currentPath.add(*edge);
					endNode.computePathsOnlyWithDrainSourceEdgesRecursively(endNodeOfPath, currentPath, paths);
					currentPath.pop_back();
				}
			}
		}
		for(EdgeSet::const_iterator it = incomingEdges_.begin(); it != incomingEdges_.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge() &&  (!techType.isInitialized() || edge->getEdgeKey().getComponent().getArray().getTechType() == techType ))
			{
				Node& startNode = edge->getStartNode();
				if(!currentPath.hasNode(edge->getStartNodeId()) && !(startNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(startNode.isGroundNode() && !endNodeOfPath.isGroundNode()))
				{
					currentPath.add(*edge);
					startNode.computePathsOnlyWithDrainSourceEdgesRecursively(endNodeOfPath, currentPath, paths);
					currentPath.pop_back();
				}
			}
		}

	}

	void Node::computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(const Node & endNodeOfPath,Path & currentPath, std::vector<Path> & paths) const
	{
		if(getNetId() == endNodeOfPath.getNetId())
		{
			paths.push_back(currentPath);
			return;
		}

		for(EdgeSet::const_iterator it = outgoingEdges_.begin(); it != outgoingEdges_.end(); it++)
		{
			Edge* edge = *it;

			if(edge->isDrainSourceEdge() || edge->isGateSourceEdge() )
			{
				Node& endNode = edge->getEndNode();
				if(!currentPath.hasNode(edge->getEndNodeId()) && !(endNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(endNode.isGroundNode() && !endNodeOfPath.isGroundNode()))
				{
					currentPath.add(*edge);
					endNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNodeOfPath, currentPath, paths);
					currentPath.pop_back();
				}
			}
		}
		for(EdgeSet::const_iterator it = incomingEdges_.begin(); it != incomingEdges_.end(); it++)
		{
			Edge* edge = *it;
			Node& startNode = edge->getStartNode();
			if(!currentPath.hasNode(edge->getStartNodeId()) && !(startNode.isSupplyNode() && endNodeOfPath.isGroundNode() ) && !(startNode.isGroundNode() && !endNodeOfPath.isGroundNode()))
			{
				currentPath.add(*edge);
				startNode.computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(endNodeOfPath, currentPath, paths);
				currentPath.pop_back();
			}

		}


	}


	std::set<Edge*> Node::getAllEdges() const
	{
		EdgeSet allEdges;
		for(EdgeSet::const_iterator it = incomingEdges_.begin(); it != incomingEdges_.end(); it++)
		{
			Edge* edge = * it;
			allEdges.insert(edge);
		}
		for(EdgeSet::const_iterator it = outgoingEdges_.begin(); it != outgoingEdges_.end(); it++)
		{
			Edge* edge = * it;
			allEdges.insert(edge);
		}
		return allEdges;
	}

	const std::set<Edge*> & Node::getIncomingEdges() const
	{
		return incomingEdges_;
	}

	const std::set<Edge*> & Node::getOutgoingEdges() const
	{
		return outgoingEdges_;
	}

}

