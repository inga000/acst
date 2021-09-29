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


#include "AutomaticSizing/incl/ConstraintProgram/Constraints/TransistorConstraintsEKV.h"

#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/ConstraintProgram/NetToIntVarMap.h"
#include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"
#include "Partitioning/incl/Results/Result.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"
#include <gecode/minimodel.hh>

#include "Partitioning/incl/Results/Component.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

	TransistorConstraintsEKV::TransistorConstraintsEKV():
			graph_(NULL),
			partitioningResult_(NULL),
			transistorToWidthMap_(NULL),
			transistorToLengthMap_(NULL),
			transistorToCurrentMap_(NULL),
			netToVoltageMap_(NULL),
			circuitInformation_(NULL),
			space_(NULL)
	{
	}

	void TransistorConstraintsEKV::setGraph(
		const Graph& graph)
	{
		graph_ = & graph;
	}

	void TransistorConstraintsEKV::setTransistorToWidthMap(
		ComponentToIntVarMap& widthMap)
	{
		transistorToWidthMap_ = & widthMap;
	}

	void TransistorConstraintsEKV::setTransistorToLengthMap(
		ComponentToIntVarMap& lengthMap)
	{
		transistorToLengthMap_ = & lengthMap;
	}

	void TransistorConstraintsEKV::setTransistorToCurrentMap(
		ComponentToIntVarMap& currentMap)
	{
		transistorToCurrentMap_ = & currentMap;
	}

	void TransistorConstraintsEKV::setNetToVoltageMap(
		NetToIntVarMap& voltageMap)
	{
		netToVoltageMap_ = & voltageMap;
	}

	void TransistorConstraintsEKV::setSpace(
		SearchSpace& space)
	{
		space_ = & space;
	}

	void TransistorConstraintsEKV::createConstraints()
	{
		std::vector<Partitioning::Transistor*> transistor = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it =  transistor.begin(); it != transistor.end(); it++)
		{
			Partitioning::Transistor * tran = * it;
			if(isLowerTransistorOf4TransistorCurrentMirror(*tran))
			{
				createLinearConstraints(*tran); // no current constraint
			}
			else
			{
				createSaturationConstraints(*tran); //no current constraint

			}

		}
	}

	const Graph& TransistorConstraintsEKV::getGraph() const
	{
		assert(graph_ != NULL);
		return * graph_;
	}

	ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToWidthMap()
	{
		assert(transistorToWidthMap_ != NULL);
		return *transistorToWidthMap_;
	}

	ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToLengthMap()
	{
		assert(transistorToLengthMap_ != NULL);
		return * transistorToLengthMap_;
	}

	ComponentToIntVarMap& TransistorConstraintsEKV::getTransistorToCurrentMap()
	{
		assert(transistorToCurrentMap_ != NULL);
		return * transistorToCurrentMap_;
	}

	NetToIntVarMap& TransistorConstraintsEKV::getNetToVoltageMap()
	{
		assert(netToVoltageMap_ != NULL);
		return * netToVoltageMap_;
	}

	SearchSpace& TransistorConstraintsEKV::getSpace()
	{
		assert(space_ != NULL);
		return *space_;
	}

	void TransistorConstraintsEKV::createSaturationVoltageConstraints(Partitioning::Component & component)
	{
		TechnologieSpecificationEKV techSpec;
		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
		for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdge = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdge = edge;
			}
		}
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		if(gateSourceEdge == NULL)
		{
			gateSourceEdge = drainSourceEdge;
		}
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);

		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);

		if(component.getArray().getTechType().isN())
		{
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();

			Gecode::rel(getSpace(), vgs - vth < vds);

			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();

			Gecode::rel(getSpace(), -(vgs - vth) > vds);

			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth <= 0);
		}


	}

	void TransistorConstraintsEKV::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	void TransistorConstraintsEKV::setPartitioningResult(
		const Partitioning::Result & result)
	{
		partitioningResult_ = & result;
	}

	const CircuitInformation& TransistorConstraintsEKV::getCircuitInformation() const
	{
		assert(circuitInformation_ != NULL);
		return * circuitInformation_;
	}

	const Partitioning::Result& TransistorConstraintsEKV::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	void TransistorConstraintsEKV::createLinearVoltageConstraints(Partitioning::Component& component)
	{
		TechnologieSpecificationEKV techSpec;
		Gecode::FloatVal u = getSpace().getScalingFactorMUM();

		Edge * drainSourceEdge = NULL;
		Edge * gateSourceEdge = NULL;
		std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(component);
		for(std::vector<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdge = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdge = edge;
			}
		}
		assert(gateSourceEdge != NULL, "No gateSoureEdge found!");
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);


		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::FloatVal earlyVoltage = techSpec.getEarlyVoltage();
			Gecode::rel(getSpace(), vgs - vth > vds);
			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth <  vds);
			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth < 0);
		}
	}


	bool TransistorConstraintsEKV::isLowerTransistorOf4TransistorCurrentMirror(
			Partitioning::Component& component)
	{
		bool isLowerTransistor = false;
		if(component.getArray().hasParent())
		{
			std::vector<const StructRec::Structure*> topmostParents = component.getArray().getTopmostParents();
			for(auto& it : topmostParents)
			{
				const StructRec::Structure & parent = *it;
				if(parent.getStructureName() == StructRec::StructureName("MosfetFourTransistorCurrentMirror"))
				{
					const StructRec::Pair & fourTransistorCurrentMirror = static_cast<const StructRec::Pair&>(parent);
					const StructRec::Pair & voltageReference1 = static_cast<const StructRec::Pair&>(fourTransistorCurrentMirror.getChild1());
					const StructRec::Pair & currentMirrorLoad = static_cast<const StructRec::Pair&>(fourTransistorCurrentMirror.getChild2());
					const StructRec::Structure & lowerTransistor1 = voltageReference1.getChild2();
					const StructRec::Structure & lowerTransistor2 = currentMirrorLoad.getChild2();
					if(component.getArray() == lowerTransistor1 || component.getArray() == lowerTransistor2)
					{
						isLowerTransistor = true;
					}
				}
			}
		}
		return isLowerTransistor;
	}

	void TransistorConstraintsEKV::createSaturationConstraints(Partitioning::Component& component)
	{
		createSaturationVoltageConstraints(component);
		createGateOverDriveVoltageConstraint(component);
		createMinimalAreaConstraint(component);
	}

	void TransistorConstraintsEKV::createGateOverDriveVoltageConstraint(Partitioning::Component & component)
	{
		float u= getSpace().getScalingFactorMUM();
		float vOverDrive = getCircuitInformation().getCircuitSpecification().getGateOverDriveVoltage();


		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar* u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);

		TechnologieSpecificationEKV  techSpec;

		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationEKVNmos();
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationEKVPmos();
		}

		float muCox = techSpec.getMobilityOxideCapacityCoefficient();

		Gecode::FloatVar widthLengthRatio = Gecode::expr(getSpace(),length * 2 /( muCox * width));
		Gecode::rel(getSpace(), abs(ids)* widthLengthRatio >= pow(vOverDrive, 2));

	}



	void TransistorConstraintsEKV::createLinearConstraints(Partitioning::Component& component)
	{
		createLinearVoltageConstraints(component);
	}

	void TransistorConstraintsEKV::createMinimalAreaConstraint(Partitioning::Component& component)
	{
		float u = getSpace().getScalingFactorMUM();
		Gecode::FloatVal p = 0.000000000001;

		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar * u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar * u);

		Gecode::rel(getSpace(), width * length >= p * getCircuitInformation().getTechnologieSpecificationSHM(component).getMinArea());
		Gecode::rel(getSpace(), length  >= pow(10,-6) * getCircuitInformation().getTechnologieSpecificationSHM(component).getMinLength());
		Gecode::rel(getSpace(), width >= pow(10,-6)* getCircuitInformation().getTechnologieSpecificationSHM(component).getMinWidth());
	}

	Gecode::FloatVar TransistorConstraintsEKV::computeEdgeVoltage(
		const Edge& edge)
	{
		const  Core::NetId startNetId = edge.getStartNodeId();
		const Core::NetId endNetId = edge.getEndNodeId();
		Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));

		Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));

		return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
	}




}

