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


#include "AutomaticSizing/incl/ConstraintProgram/Constraints/TransistorConstraintsSHM.h"

#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
#include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include<math.h>
#include <gecode/float.hh>

namespace AutomaticSizing {

	TransistorConstraintsSHM::TransistorConstraintsSHM():
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

	void TransistorConstraintsSHM::setGraph(
		const Graph& graph)
	{
		graph_ = & graph;
	}

	void TransistorConstraintsSHM::setTransistorToWidthMap(
		ComponentToIntVarMap& widthMap)
	{
		transistorToWidthMap_ = & widthMap;
	}

	void TransistorConstraintsSHM::setTransistorToLengthMap(
		ComponentToIntVarMap& lengthMap)
	{
		transistorToLengthMap_ = & lengthMap;
	}

	void TransistorConstraintsSHM::setTransistorToCurrentMap(
		ComponentToIntVarMap& currentMap)
	{
		transistorToCurrentMap_ = & currentMap;
	}

	void TransistorConstraintsSHM::setNetToVoltageMap(
		NetToIntVarMap& voltageMap)
	{
		netToVoltageMap_ = & voltageMap;
	}

	void TransistorConstraintsSHM::setSpace(
		SearchSpace& space)
	{
		space_ = & space;
	}

	void TransistorConstraintsSHM::createConstraints()
	{
		std::vector<Partitioning::Transistor*> transistor = getPartitioningResult().getAllTransistors();

		for(std::vector<Partitioning::Transistor*>::const_iterator it =  transistor.begin(); it != transistor.end(); it++)
		{
			Partitioning::Transistor * tran = * it;
			if(isLowerTransistorOf4TransistorCurrentMirror(*tran))
			{
				createLinearConstraints(*tran);
			}
			else if(getPartitioningResult().getPart(tran->getArray()).isResistorPart()
					&& getPartitioningResult().getResistorPart(tran->getArray()).isCompensationResistor())
			{
				createCompensationResistorConstraint(*tran);
			}
			else if(getPartitioningResult().getPart(tran->getArray()).isPositiveFeedbackPart()
					&& getPartitioningResult().getPositiveFeedbackPart(tran->getArray()).hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult()))
			{
				createOffConstraint(*tran);
			}
			else
			{
				createSaturationConstraints(*tran);
			}
		}
	}

	const Graph& TransistorConstraintsSHM::getGraph() const
	{
		assert(graph_ != NULL);
		return * graph_;
	}

	ComponentToIntVarMap& TransistorConstraintsSHM::getTransistorToWidthMap()
	{
		assert(transistorToWidthMap_ != NULL);
		return *transistorToWidthMap_;
	}

	ComponentToIntVarMap& TransistorConstraintsSHM::getTransistorToLengthMap()
	{
		assert(transistorToLengthMap_ != NULL);
		return * transistorToLengthMap_;
	}

	ComponentToIntVarMap& TransistorConstraintsSHM::getTransistorToCurrentMap()
	{
		assert(transistorToCurrentMap_ != NULL);
		return * transistorToCurrentMap_;
	}

	NetToIntVarMap& TransistorConstraintsSHM::getNetToVoltageMap()
	{
		assert(netToVoltageMap_ != NULL);
		return * netToVoltageMap_;
	}

	SearchSpace& TransistorConstraintsSHM::getSpace()
	{
		assert(space_ != NULL);
		return *space_;
	}

	void TransistorConstraintsSHM::createSaturationCurrentConstraintsStrongInversion(Partitioning::Component & component)
	{
//		if(component.getArray().getIdentifier().toStr() == "MosfetNormalArray[7]")
		{
		TechnologieSpecificationSHM techSpec;
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
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		if(gateSourceEdge == NULL)
		{
			gateSourceEdge = drainSourceEdge;
		}
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);

		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);

		Gecode::FloatVar idsHelperVar = getSpace().createChanneledFloatVar(getTransistorToCurrentMap().find(component), -100000000000, 1000000000);
		Gecode::FloatVar ids = Gecode::expr(getSpace(), idsHelperVar * pow(10,-9));

		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		Gecode::channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);



		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::FloatVar lamda(getSpace(), 0.001,0.9);

			Gecode::rel(getSpace(), lamda == techSpec.getChannelLengthCoefficientStrongInversion());

			if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			{
				Gecode::rel(getSpace(), muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   <= 1.1);
				Gecode::rel(getSpace(), muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   >= 0.9);
			}
			else
			{
				Gecode::rel(getSpace(), muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   <= 1.01);
				Gecode::rel(getSpace(), muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /* (1+lamda * vds)*/ / ids   >= 0.99);
			}

			if(getPartitioningResult().getFirstStage().hasHelperStructure() && !getPartitioningResult().hasBiasOfFoldedPair())
			{
				Gecode::rel(getSpace(), vgs - vth  < vds);
				rel(getSpace(), abs(vgs-vth) < 0.3);

			}
			else if( !getPartitioningResult().getFirstStage().isComplementary() &&
					((getCircuitInformation().getCircuitParameter().getSupplyVoltage() - getCircuitInformation().getCircuitParameter().getGroundVoltage()) >=3.5))
			{
				if(getPartitioningResult().hasFirstStageGainEnhancer())
				{
					Gecode::rel(getSpace(), vgs - vth  < vds);
				}
				else if(component.getPart().isTransconductancePart() && getPartitioningResult().getTransconductancePart(component.getArray()).isPrimarySecondStage()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{
					Gecode::rel(getSpace(), vgs - vth  +1.4< vds);
				}
				else if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{

					Gecode::rel(getSpace(), 1.1 +(vgs - vth) < vds);
				}
				else if(isCurrentBias(component.getArray()) && getPartitioningResult().getFirstStage().hasHelperStructure())
				{
					Gecode::rel(getSpace(), vgs - vth  +0.2< vds);
				}
				else
				{
					Gecode::rel(getSpace(), vgs - vth  < vds);
				}
				rel(getSpace(), abs(vgs-vth) < 0.75);
			}
			else
			{
				Gecode::rel(getSpace(), vgs - vth  < vds);

				if(!getPartitioningResult().getFirstStage().isComplementary())
					rel(getSpace(), abs(vgs-vth) < 0.3);

				if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier() ))
				{
					Gecode::rel(getSpace(), 0.3 +(vgs - vth) > vds);
				}
			}

			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);

		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();

			Gecode::FloatVar lamda(getSpace(), 0.01,0.9);
			Gecode::rel(getSpace(), lamda == techSpec.getChannelLengthCoefficientStrongInversion());

			if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCrossCoupledPair(getPartitioningResult()))
			{

				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  <=1.1);
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  >= 0.9);
			}
			else
			{
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  <=1.01);
				Gecode::rel(getSpace(),  -1* muCox* 0.5 *(width/length) * pow(vgs - vth,2)  /** (1-lamda * vds) *//ids  >= 0.99);
			}



			if(getPartitioningResult().getFirstStage().hasHelperStructure() && !getPartitioningResult().hasBiasOfFoldedPair())
			{
				Gecode::rel(getSpace(), (vgs - vth) > vds);
			}
			else if(!getPartitioningResult().getFirstStage().isComplementary() &&
					((getCircuitInformation().getCircuitParameter().getSupplyVoltage() - getCircuitInformation().getCircuitParameter().getGroundVoltage()) >=3.5))
			{
				if(getPartitioningResult().hasFirstStageGainEnhancer())
				{
					Gecode::rel(getSpace(), (vgs - vth) > vds);
				}
				else if(component.getPart().isTransconductancePart() && getPartitioningResult().getTransconductancePart(component.getArray()).isPrimarySecondStage()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{
					Gecode::rel(getSpace(), -1.4 +(vgs - vth) > vds);
				}
				else if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier())
					&& !getCircuitInformation().getCircuitParameter().isVoltageBiasNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Gate")).getIdentifier()))
				{

					Gecode::rel(getSpace(), -1.1 +(vgs - vth) > vds);
				}
				else if(isCurrentBias(component.getArray()) && getPartitioningResult().getFirstStage().hasHelperStructure())
				{
					Gecode::rel(getSpace(), -0.2 +(vgs - vth) > vds);
				}
				else
				{
					Gecode::rel(getSpace(), (vgs - vth) > vds);
				}
				rel(getSpace(), abs(vgs-vth) < 0.75);
			}
			else
			{
				Gecode::rel(getSpace(), (vgs - vth) > vds);

				if(!getPartitioningResult().getFirstStage().isComplementary())
					rel(getSpace(), abs(vgs-vth) < 0.3);
				if(component.getPart().isLoadPart() && getPartitioningResult().getLoadPart(component.getArray()).hasCascodedPair()
					&& isOutputNet(component.getArray().findNet(StructRec::StructurePinType(component.getArray().getStructureName(), "Drain")).getIdentifier()))
				{
					if(getCircuitInformation().getDesignAttributes().transconductancesInWeakInversion(getPartitioningResult()))
					{
						Gecode::rel(getSpace(), -0.3 +(vgs - vth) > vds);
					}
					else
					{
						Gecode::rel(getSpace(), -0.3 +(vgs - vth) < vds);
					}
				}
			}
			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth <= 0);
		}

		}
	}

	void TransistorConstraintsSHM::createSaturationCurrentConstraintsWeakInversion(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
		float Vt = techSpec.getThermalVoltage();
		float vth = techSpec.getThresholdVoltage();
		float n = techSpec.getSlopeFactor();
		float muCox = techSpec.getMobilityOxideCapacityCoefficient();

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
		assert(drainSourceEdge != NULL, "No drainSourceEdge found!");
		if(gateSourceEdge == NULL)
		{
			gateSourceEdge = drainSourceEdge;
		}
		Gecode::FloatVar vds = computeEdgeVoltage(*drainSourceEdge);
		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);
		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));
		//velocity

		Gecode::FloatVar velocity(getSpace(), -10000, 1000);
		rel(getSpace(), velocity * 2 * n * Vt == abs(vgs)-fabs(vth));

		//IC
		Gecode::FloatVar inversionCoefficient(getSpace(), 0, 100);
		Gecode::FloatVar exponent(getSpace(), -100, 100);
		rel(getSpace(), exponent == 2* velocity);

		Gecode::exp(getSpace(), exponent, inversionCoefficient);
		rel(getSpace(),inversionCoefficient <= 0.1);
		dom(getSpace(), inversionCoefficient, 0, 100);

		//IdsSpec
		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);
//
		Gecode::FloatVar idsSpec = expr(getSpace(), muCox/(2*n)* width/length * pow(2*n*Vt, 2));
		dom(getSpace(),idsSpec, 0, 100);


		if(component.getArray().getTechType().isN())
		{

			Gecode::FloatVar idsExact = expr(getSpace(),inversionCoefficient * idsSpec );
			dom(getSpace(), idsExact, 0, 100);

			Gecode::rel(getSpace(), ids <= 1.01 * idsExact);
			Gecode::rel(getSpace(), ids >= 0.99 * idsExact);

			Gecode::rel(getSpace(), vds > 4* Vt);
			Gecode::rel(getSpace(), vgs - vth < 0);
			Gecode::rel(getSpace(), vgs - vth > -0.2);
		}
		else
		{

			Gecode::FloatVar idsExact = expr(getSpace(),inversionCoefficient * idsSpec );
			dom(getSpace(), idsExact, 0, 100);

			Gecode::rel(getSpace(), ids >=  -1* 1.01 * idsExact);
			Gecode::rel(getSpace(), ids <= -1*0.99 *idsExact);

			Gecode::rel(getSpace(), vds < - 4* Vt);
			Gecode::rel(getSpace(), vgs - vth > 0);
			Gecode::rel(getSpace(), vgs - vth < 0.2);
		}
	}


	bool TransistorConstraintsSHM::isOutputNet(Core::NetId net) const
	{

		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			if(net == getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId() || net == getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(net == getCircuitInformation().getCircuitParameter().getOutputPin().getNetId())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}

	void TransistorConstraintsSHM::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	void TransistorConstraintsSHM::setPartitioningResult(
		const Partitioning::Result & result)
	{
		partitioningResult_ = & result;
	}

	const CircuitInformation& TransistorConstraintsSHM::getCircuitInformation() const
	{
		assert(circuitInformation_ != NULL);
		return * circuitInformation_;
	}

	const Partitioning::Result& TransistorConstraintsSHM::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	void TransistorConstraintsSHM::createCompensationResistorConstraint(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec;

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

		Gecode::FloatVar ids = getSpace().createChanneledFloatVar(getTransistorToCurrentMap().find(component), -1000000000, 1000000000);


		rel(getSpace(), ids == 0);
		rel(getSpace(), vds == 0);


		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth < 0);
		}


	}

	void TransistorConstraintsSHM::createOffConstraint(Partitioning::Component & component)
	{
		TechnologieSpecificationSHM techSpec;

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

		Gecode::FloatVar vgs = computeEdgeVoltage(*gateSourceEdge);
		Gecode::IntVar ids = getTransistorToCurrentMap().find(component);

		rel(getSpace(), ids == 0);

		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth < 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
	}

	void TransistorConstraintsSHM::createLinearCurrentConstraints(Partitioning::Component& component)
	{

		TechnologieSpecificationSHM techSpec;
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

		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));

		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar*u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);


		if(component.getArray().getTechType().isN())
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), muCox *(width/length) * ((vgs - vth)-0.5*vds) * vds  == 1 * ids);
			Gecode::rel(getSpace(), vgs - vth > vds);
			Gecode::rel(getSpace(), vds > 0);
			Gecode::rel(getSpace(), vgs - vth > 0);
		}
		else
		{
			techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();
			Gecode::FloatVal vth = techSpec.getThresholdVoltage();
			Gecode::rel(getSpace(), -1* muCox * (width/length) * ((vgs - vth) - vds * 0.5) * vds  / ids == 1);
			Gecode::rel(getSpace(), vgs - vth <  vds);
			Gecode::rel(getSpace(), vds < 0);
			Gecode::rel(getSpace(), vgs - vth < 0);
		}
	}


	bool TransistorConstraintsSHM::isLowerTransistorOf4TransistorCurrentMirror(
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

	void TransistorConstraintsSHM::createSaturationConstraints(Partitioning::Component& component)
	{
		if(getCircuitInformation().getDesignAttributes().isInWeakInversion(component))
		{
			createSaturationCurrentConstraintsWeakInversion(component);
		}
		else
		{
			createSaturationCurrentConstraintsStrongInversion(component);
			createGateOverDriveVoltageConstraint(component);
		}

		createMinimalAreaConstraint(component);
	}

	void TransistorConstraintsSHM::createGateOverDriveVoltageConstraint(Partitioning::Component & component)
	{
			float u= getSpace().getScalingFactorMUM();
			float vOverDrive = getCircuitInformation().getCircuitSpecification().getGateOverDriveVoltage();

			Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(component));

			Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
			Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

			channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
			channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

			Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar* u);
			Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar*u);

			TechnologieSpecificationSHM  techSpec;

			if(component.getArray().getTechType().isN())
			{
				techSpec = getCircuitInformation().getTechnologieSpecificationSHMNmos();
			}
			else
			{
				techSpec = getCircuitInformation().getTechnologieSpecificationSHMPmos();
			}

			float muCox = techSpec.getMobilityOxideCapacityCoefficient();

			Gecode::FloatVar widthLengthRatio = Gecode::expr(getSpace(), length * 2 /( muCox * width));
			Gecode::rel(getSpace(), abs(ids)* widthLengthRatio >= pow(vOverDrive, 2));
	}

	void TransistorConstraintsSHM::createLinearConstraints(Partitioning::Component& component)
	{
		createLinearCurrentConstraints(component);
	}

	void TransistorConstraintsSHM::createMinimalAreaConstraint(Partitioning::Component& component)
	{
		Gecode::FloatVal u = getSpace().getScalingFactorMUM();
		Gecode::FloatVal p = 0.000000000001;

		Gecode::FloatVar widthHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());

		channel(getSpace(), getTransistorToWidthMap().find(component), widthHelperVar);
		channel(getSpace(), getTransistorToLengthMap().find(component), lengthHelperVar);

		Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelperVar * u);
		Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelperVar * u);


		Gecode::rel(getSpace(), width *  length >= 0.99* p * getCircuitInformation().getTechnologieSpecificationSHM(component).getMinArea());
		Gecode::rel(getSpace(), length  >= 0.000001* getCircuitInformation().getTechnologieSpecificationSHM(component).getMinLength()-0.00000001);
		Gecode::rel(getSpace(), width > pow(10,-6)* getCircuitInformation().getTechnologieSpecificationSHM(component).getMinWidth()-0.00000001);

	}

	Gecode::FloatVar TransistorConstraintsSHM::computeEdgeVoltage(
		const Edge& edge)
	{
		const  Core::NetId startNetId = edge.getStartNodeId();
		const Core::NetId endNetId = edge.getEndNodeId();

		Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));

		Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));

		return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
	}

	void TransistorConstraintsSHM::createSameRegionConstraintFoldedPair()
	{
		if(getPartitioningResult().hasFirstStage())
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			if(firstStage.hasHelperStructure())
			{
				const StructRec::Structure & foldedPair = firstStage.getHelperStructure();
				std::vector<Partitioning::Component*> compsFoldedPair = getPartitioningResult().findComponents(foldedPair);
				createSameRegionConstraint(**compsFoldedPair.begin(), **compsFoldedPair.rbegin());
			}
		}
	}

	void TransistorConstraintsSHM::createSameRegionConstraint(Partitioning::Component& transistor1, Partitioning::Component & transistor2)
	{
		Edge * drainSourceEdgeTran1 = NULL;
		Edge * gateSourceEdgeTran1 = NULL;
		std::vector<Edge*> edgesTran1 = getGraph().findToComponentBelongingEdges(transistor1);
		for(std::vector<Edge*>::const_iterator it = edgesTran1.begin(); it != edgesTran1.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdgeTran1 = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdgeTran1 = edge;
			}
		}
		assert(drainSourceEdgeTran1 != NULL, "No drainSourceEdge found!");
		if(gateSourceEdgeTran1 == NULL)
		{
			gateSourceEdgeTran1 = drainSourceEdgeTran1;
		}

		Edge * drainSourceEdgeTran2 = NULL;
		Edge * gateSourceEdgeTran2 = NULL;
		std::vector<Edge*> edgesTran2 = getGraph().findToComponentBelongingEdges(transistor2);
		for(std::vector<Edge*>::const_iterator it = edgesTran2.begin(); it != edgesTran2.end(); it++)
		{
			Edge* edge = *it;
			if(edge->isDrainSourceEdge())
			{
				drainSourceEdgeTran2 = edge;
			}
			else if (edge->isGateSourceEdge())
			{
				gateSourceEdgeTran2 = edge;
			}
		}
		assert(drainSourceEdgeTran2 != NULL, "No drainSourceEdge found!");
		if(gateSourceEdgeTran2 == NULL)
		{
			gateSourceEdgeTran2 = drainSourceEdgeTran2;
		}
		const TechnologieSpecificationSHM & techSpecTran1 =  getCircuitInformation().getTechnologieSpecificationSHM(transistor1);
		Gecode::FloatVar vdsTran1 = computeEdgeVoltage(*drainSourceEdgeTran1);
		Gecode::FloatVar vgsTran1 = computeEdgeVoltage(*gateSourceEdgeTran1);
		Gecode::FloatVal vthTran1 = techSpecTran1.getThresholdVoltage();

		const TechnologieSpecificationSHM & techSpecTran2 = getCircuitInformation().getTechnologieSpecificationSHM(transistor2);
		Gecode::FloatVar vdsTran2 = computeEdgeVoltage(*drainSourceEdgeTran2);
		Gecode::FloatVar vgsTran2 = computeEdgeVoltage(*gateSourceEdgeTran2);
		Gecode::FloatVal vthTran2 = techSpecTran2.getThresholdVoltage();

		if(transistor1.getArray().getTechType().isN() && transistor2.getArray().getTechType().isN())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1  + 0.08) || (vgsTran2 - vthTran2 > vdsTran2  + 0.08));
			Gecode::rel(getSpace(),  !(vgsTran1 - vthTran1 < vdsTran1 - 0.05 ) || (vgsTran2 - vthTran2 < vdsTran2 - 0.05 ));
		}
		else if(transistor1.getArray().getTechType().isP() && transistor2.getArray().getTechType().isP())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.08) || (vgsTran2 - vthTran2 < vdsTran2 - 0.08)  );
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1 + 0.05) || (vgsTran2 - vthTran2 > vdsTran2 + 0.05));
		}
		else if(transistor1.getArray().getTechType().isN() && transistor2.getArray().getTechType().isP())
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1  + 0.08) || (vgsTran2 - vthTran2 < vdsTran2 - 0.08)  );
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.05 ) || (vgsTran2 - vthTran2 > vdsTran2 + 0.05));
		}
		else
		{
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 < vdsTran1 - 0.08) || (vgsTran2 - vthTran2 > vdsTran2  + 0.08));
			Gecode::rel(getSpace(), !(vgsTran1 - vthTran1 > vdsTran1 + 0.05) || (vgsTran2 - vthTran2 < vdsTran2 - 0.05 ));
		}

	}

	bool TransistorConstraintsSHM::isCurrentBias(const StructRec::Structure & structure) const
	{
		bool isCurrentBias = false;

		if(structure.hasParent())
		{
			std::vector<const StructRec::Structure*> parents = structure.getTopmostParents();
			for(auto& it_parents: parents)
			{
				if(isCurrentMirror(*it_parents))
				{
					const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(it_parents);

					if(parentPair->getChild2().hasCommonDevices(structure))
					{
						isCurrentBias = true;
					}
				}
			}

		}

		return isCurrentBias;
	}

	bool TransistorConstraintsSHM::isCurrentMirror(const StructRec::Structure & structure) const
	{
	   const StructRec::StructureName& simpleCurrentMirror = StructRec::StructureName("MosfetSimpleCurrentMirror");
	   const StructRec::StructureName& cascodeCurrentMirror = StructRec::StructureName("MosfetCascodeCurrentMirror");
	   const StructRec::StructureName& improvedWilsonCurrentMirror = StructRec::StructureName("MosfetImprovedWilsonCurrentMirror");
	   const StructRec::StructureName& wideSwingCascodeCurrentMirror = StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror");
	   const StructRec::StructureName& wideSwingCurrentMirror = StructRec::StructureName("MosfetWideSwingCurrentMirror");
	   const StructRec::StructureName& fourTransistorCurrentMirror = StructRec::StructureName("MosfetFourTransistorCurrentMirror");
	   const StructRec::StructureName& wilsonCurrentMirror = StructRec::StructureName("MosfetWilsonCurrentMirror");


	    return (structure.getStructureName() == simpleCurrentMirror) ||
	            (structure.getStructureName() == cascodeCurrentMirror) ||
	              (structure.getStructureName() == improvedWilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCascodeCurrentMirror) ||
	                (structure.getStructureName() == fourTransistorCurrentMirror) ||
	                (structure.getStructureName() == wilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCurrentMirror);
	}





}

