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

namespace AutomaticSizing {

	Edge::Edge() :
			startNode_(NULL),
			endNode_(NULL),
			isParallel_(false)
	{
	}

	Edge::~Edge()
	{
	}

	void Edge::setStartNode(Node& startNode)
	{
		startNode_ = & startNode;
	}

	void Edge::setEndNode(Node& endNode)
	{
		endNode_ = & endNode;
	}

	void Edge::setEdgeKey(const EdgeKey& edgeKey)
	{
		edgeKey_ = edgeKey;
	}

	Core::NetId Edge::getStartNodeId() const
	{
		return getStartNode().getNetId();
	}

	Core::NetId Edge::getEndNodeId() const
	{
		return getEndNode().getNetId();
	}

	std::string Edge::toStr() const
	{
		std::ostringstream oss;
		oss << "(" << getStartNodeId() << "," << getEndNodeId() << "," << getEdgeKey().getStructureId() << ")";
		return oss.str();
	}

	Node& Edge::getStartNode()
	{
		assert(startNode_ != NULL);
		return *startNode_;
	}

	Node& Edge::getEndNode()
	{
		assert(endNode_ != NULL);
		return *endNode_;
	}

	const Node& Edge::getStartNode() const
	{
		assert(startNode_ != NULL);
		return *startNode_;
	}

	const Node& Edge::getEndNode() const
	{
		assert(endNode_ != NULL);
		return *endNode_;
	}

	const EdgeKey& Edge::getEdgeKey() const
	{
		assert(edgeKey_.isInitialized());
		return edgeKey_;
	}

	bool Edge::isParallel() const
	{
		return isParallel_;
	}

	void Edge::setParallel()
	{
		isParallel_ = true;
	}

	void Edge::addParallelEdge(Edge& edge)
	{
		parallelEdges_.insert(& edge);
	}

	std::set<Edge*>& Edge::getAllParallelEdges()
	{
		assert(hasParallelEdges());
		return parallelEdges_;
	}

	bool Edge::hasParallelEdges() const
	{
		return !parallelEdges_.empty();
	}

	bool Edge::isDrainSourceEdge() const
	{
		return getEdgeKey().isDrainSourceEdge();
	}

	bool Edge::isCapacitanceEdge() const
	{
		return getEdgeKey().isCapacitanceEdge();
	}

	bool Edge::isGateSourceEdge() const
	{
		return getEdgeKey().isGateSourceEdge();
	}

	bool Edge::isGateDrainEdge() const
	{
		return getEdgeKey().isGateDrainEdge();
	}

}

