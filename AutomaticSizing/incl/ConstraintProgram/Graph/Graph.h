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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_GRAPH_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_GRAPH_H_
#include "Partitioning/incl/Results/Component.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/EdgeKey.h"

#include "AutomaticSizing/incl/ConstraintProgram/Graph/Path.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Common/Object.h"

namespace AutomaticSizing {

	class CircuitParameter;
	class Paths;
	class EdgeBasedLoops;
	class Node;
	class Edge;

	class Graph : public Core::Object
	{
	private:
		typedef std::map<Core::NetId, Node*> NodeMap;
		typedef std::map<EdgeKey, Edge*> EdgeMap;
	public:
		Graph();
		~Graph();

		void addEdges(Partitioning::Component & component);
		void equalizeParallelEdges();

		const Paths & getPaths() const;

		const std::vector<Node*> getAllNodes() const;
		const Node & getBiasNode() const;
		const Node & getOutputNode() const;
		const Node & getSupplyNode() const;
		const Node & getGroundNode() const;

		void setCircuitParameter(const CircuitParameter & parameter);


		// Can be used for computation of output Voltage Swing
		std::vector<Path> computeVddToOuputPathsWithOnlyDrainSourceEdges() const;
		std::vector<Path> computeVddToOuputPathsWithOnlyDrainSourceEdgesAndSpecificTechType(Core::TechType techType) const;
		std::vector<Path> computeOutputToGroundPathsWithOnlyDrainSourceEdges() const;
		std::vector<Path> computeOutputToGroundPathsWithOnlyDrainSourceEdgesAndSpecificTechType(Core::TechType techType) const;

		std::vector<Path> computeInputPlusToVddPathWithOnlyDrainSourceAndGateSourceEdges() const;
		std::vector<Path> computeInputPlusToGroundPathWithOnlyDrainSourceAndGateSourceEdges() const;
		std::vector<Path> computeInputMinusToVddPathWithOnlyDrainSourceAndGateSourceEdges() const;
		std::vector<Path> computeInputMinusToGroundPathWithOnlyDrainSourceAndGateSourceEdges() const;


		std::vector<Edge*> findToComponentBelongingEdges(Partitioning::Component & component) const;

		std::string toStr() const;
	private:
		void addEdge(const EdgeKey& edgeKey);
		void addNode(const Core::NetId& netId);

		bool hasNode(const Core::NetId& netId) const;
		Node& findNode(const Core::NetId& netId);

		const Node & findNode(const Core::NetId & netId) const;

		void setPaths(Paths & path);

		const CircuitParameter & getCircuitParameter() const;
		Paths & getPaths();

		const EdgeKey & computeEdgeKey(StructRec::StructurePinType & pinType1, StructRec::StructurePinType pinType2, Partitioning::Component & component);


		//This typ of path computation is not used in the moment
		void computeVddToGroundPaths();
		void computeInputToGroundPaths();
		void computeOutputToGroundPaths();
		void computeVddToBiasPaths();


		void eraseNodes();
		void eraseEdges();

	private:
		NodeMap nodeMap_;
		EdgeMap edgeMap_;

		//No Paths are created in the moment
		Paths * paths_;

		const CircuitParameter * circuitParameter_;
	};




}



#endif /* AUTOMATICSIZING_INCL_ANALYSIS_GRAPH_GRAPH_H_ */
