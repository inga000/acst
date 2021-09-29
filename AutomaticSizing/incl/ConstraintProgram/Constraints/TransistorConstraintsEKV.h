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

#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_TRANSISTORCONSTRAINTSEKV_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_TRANSISTORCONSTRAINTSEKV_H_


#include <gecode/float.hh>

namespace Partitioning {

	class Result;
	class Component;

}

namespace AutomaticSizing
{
	class Graph;
	class NetToIntVarMap;
	class ComponentToIntVarMap;
	class CircuitInformation;
	class SearchSpace;
	class Edge;

	class TransistorConstraintsEKV
	{
	public:
		TransistorConstraintsEKV();

		void setGraph(const Graph & graMph);
		void setCircuitInformation(const CircuitInformation & information);
		void setPartitioningResult(const Partitioning::Result & result);
		void setTransistorToWidthMap(ComponentToIntVarMap & widthMap );
		void setTransistorToLengthMap(ComponentToIntVarMap & lenghtMap);
		void setTransistorToCurrentMap(ComponentToIntVarMap & currentMap);
		void setNetToVoltageMap(NetToIntVarMap & voltageMap);
		void setSpace(SearchSpace & space);

		void createConstraints();

	private:
		const Graph & getGraph() const;
		const CircuitInformation & getCircuitInformation() const;
		const Partitioning::Result & getPartitioningResult() const;
		ComponentToIntVarMap & getTransistorToWidthMap();
		ComponentToIntVarMap & getTransistorToLengthMap();
		ComponentToIntVarMap & getTransistorToCurrentMap();
		NetToIntVarMap & getNetToVoltageMap();
		SearchSpace & getSpace();

		void createSaturationConstraints(Partitioning::Component & component);
		void createLinearConstraints(Partitioning::Component & component);

		void createSameRegionConstraintFoldedPair();
		void createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2);

		void createLinearVoltageConstraints(Partitioning::Component & component);
		void createSaturationVoltageConstraints(Partitioning::Component & component);
		void createDependencyConstraints(Partitioning::Component & component);

		void createMinimalAreaConstraint(Partitioning::Component & component);

		void createGateOverDriveVoltageConstraint(Partitioning::Component & component);


		bool isLowerTransistorOf4TransistorCurrentMirror(Partitioning::Component & component);

		Gecode::FloatVar computeEdgeVoltage(const Edge & edge);

	private:
		const Graph * graph_;
		const CircuitInformation * circuitInformation_;
		const Partitioning::Result * partitioningResult_;
		ComponentToIntVarMap * transistorToWidthMap_;
		ComponentToIntVarMap* transistorToLengthMap_;
		ComponentToIntVarMap * transistorToCurrentMap_;
		NetToIntVarMap * netToVoltageMap_;
		SearchSpace * space_;

};

}



#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_CONSTRAINTS_INTEGERSPECIFICATIONTRANSISTORCONSTRAINTS_H_ */
