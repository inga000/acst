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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_NODE_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_NODE_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Common/Object.h"

#include <gecode/float.hh>

namespace AutomaticSizing {

	class Path;
	class Paths;
	class Edge;
	class Node : public Core::Object
	{
	private:
		typedef std::set<Edge*> EdgeSet;

	public:
		Node();
		~Node();

		void setNetId(const Core::NetId& netId);

		void addIncomingEdge(Edge& edge);
		void addOutgoingEdge(Edge& edge);

		void computePathsRecursively(const Node& endNodeOfPath, Path& currentPath, Paths& paths) const;
		void computePathsOnlyWithDrainSourceEdgesRecursively(const Node & endNodeOfPath,Path & currentPath, std::vector<Path> & paths) const;
		void computePathsOnlyWithDrainSourceAndGateSourceEdgesRecursively(const Node & endNode,Path & currentPaths, std::vector<Path> & paths) const;

		void setSupplyNode();
		void setGroundNode();
		void setBiasNode();
		void setInputNode();
		void setOutputNode();

		void setVisited();
		void setNotVisited();
		bool isVisited() const;
		bool isSupplyNode() const;
		bool isGroundNode() const;
		bool isBiasNode() const;
		bool isInputNode() const;
		bool isOutputNode() const;
		bool isSpecificNode() const;

		Core::NetId getNetId() const;

		bool operator==(const Node& other) const;
		bool operator!=(const Node& other) const;
		bool operator<(const Node& other) const;

		EdgeSet getAllEdges() const;
		const EdgeSet & getIncomingEdges() const;
		const EdgeSet  & getOutgoingEdges() const;

		std::string toStr() const;
	private:

		bool visited_;
		Core::NetId netId_;
		bool isSupplyNode_;
		bool isGroundNode_;
		bool isBiasNode_;
		bool isInputNode_;
		bool isOutputNode_;

		EdgeSet incomingEdges_;
		EdgeSet outgoingEdges_;
	};




}



#endif /* AUTOMATICSIZING_INCL_ANALYSIS_GRAPH_NODE_H_ */
