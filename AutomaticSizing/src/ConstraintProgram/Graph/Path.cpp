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


#include "AutomaticSizing/incl/ConstraintProgram/Graph/Path.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Node.h"

namespace AutomaticSizing {


	Path::Path() :
			startNode_(NULL),
			endNode_(NULL)
	{

	}

	Path::~Path()
	{
	}

	void Path::add(const Edge& edge)
	{
		edgeVector_.push_back(&edge);
	}

	void Path::pop_back()
	{
		assert(!edgeVector_.empty());
		edgeVector_.pop_back();
	}

	bool Path::hasAnEdge() const
	{
		return !edgeVector_.empty();
	}

	std::string Path::toStr() const
	{
		std::ostringstream oss;
		oss << "(";
		for(EdgeVector::const_iterator it = edgeVector_.begin(); it != edgeVector_.end(); it++)
		{
			const Edge* edge = *it;
			oss << *edge;
			if(edge != edgeVector_.back()) {
				oss << ",";
			}
		}
		oss << ")";
		return oss.str();
	}

	bool Path::hasNode(const Core::NetId& netId) const
	{
		bool hasIt = false;
		for(EdgeVector::const_iterator it = edgeVector_.begin(); it != edgeVector_.end(); it++)
		{
			const Edge* edge = *it;
			if(edge->getStartNodeId() == netId || edge->getEndNodeId() == netId)
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}


	bool Path::isVddToGroundPath() const
	{
		return getStartNode().isSupplyNode() && getEndNode().isGroundNode();
	}

	bool Path::isInputToGroundPath() const
	{
		return getStartNode().isInputNode() && getEndNode().isGroundNode();
	}

	bool Path::isOutputToGroundPath() const
	{
		return getStartNode().isOutputNode() && getEndNode().isGroundNode();
	}

	bool Path::isVddToBiasPath() const
	{
		return getStartNode().isSupplyNode() && getEndNode().isBiasNode();
	}

	const std::vector<const Edge*> & Path::getEdges() const
	{
		assert(!edgeVector_.empty());
		return edgeVector_;
	}

	const Node & Path::getStartNode() const
	{
		assert(startNode_ != NULL);
		return * startNode_;
	}

	const Node & Path::getEndNode() const
	{
		assert(endNode_ != NULL);
		return *endNode_;
	}

	void Path::setStartNode(const Node & node)
	{
		startNode_ = &node;
	}

	void Path::setEndNode(const Node & node)
	{
		endNode_ = & node;
	}
}

