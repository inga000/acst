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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_EDGE_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_EDGE_H_

#include "AutomaticSizing/incl/ConstraintProgram/Graph/EdgeKey.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Common/Object.h"

#include <gecode/float.hh>

namespace AutomaticSizing {

	class Node;
	class Edge : public Core::Object
	{
	private:
		friend class Node;
	public:
		Edge();
		~Edge();

		void setStartNode(Node& startNode);
		void setEndNode(Node& endNode);
		void setEdgeKey(const EdgeKey& edgeKey);

		Core::NetId getStartNodeId() const;
		Core::NetId getEndNodeId() const;

		const Node& getStartNode() const;
		const Node& getEndNode() const;

		Node& getStartNode();
		Node& getEndNode();

		bool isParallel() const;
		void setParallel();

		void addParallelEdge(Edge & edge);
		std::set<Edge*> & getAllParallelEdges();
		bool hasParallelEdges() const;

		const EdgeKey& getEdgeKey() const;

		bool isDrainSourceEdge() const;
		bool isCapacitanceEdge() const;
		bool isGateSourceEdge() const;
		bool isGateDrainEdge() const;

		virtual std::string toStr() const;
	protected:

	private:
		Node* startNode_;
		Node* endNode_;

		EdgeKey edgeKey_;

		bool isParallel_;
		std::set<Edge*> parallelEdges_;


	};


}




#endif /* AUTOMATICSIZING_INCL_ANALYSIS_GRAPH_EDGE_H_ */
