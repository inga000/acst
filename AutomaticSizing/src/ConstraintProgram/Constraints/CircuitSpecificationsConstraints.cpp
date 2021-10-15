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



#include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"
#include "Partitioning/incl/Results/Result.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Path.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Node.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/CircuitInformation/LoadCapacity.h"
#include "AutomaticSizing/incl/ConstraintProgram/NetToIntVarMap.h"
#include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Common/BacktraceAssert.h"

#include <math.h>
#include <gecode/minimodel.hh>
#include <gecode/float.hh>

#include "../../../incl/ConstraintProgram/Constraints/CircuitSpecificationsConstraints.h"
#include "../../../incl/ConstraintProgram/Constraints/PolesAndZeros.h"

namespace AutomaticSizing {

	CircuitSpecificationsConstraints::CircuitSpecificationsConstraints() :
			partitioningResult_(NULL),
			structRecResult_(NULL),
			polesAndZeros_(NULL),
			circuitInformation_(NULL),
			graph_(NULL),
			transistorToWidthMap_(NULL),
			transistorToLengthMap_(NULL),
			transistorToCurrentMap_(NULL),
			twoPortToValueMap_(NULL),
			netToVoltageMap_(NULL),
			space_(NULL)
	{
	}

	void CircuitSpecificationsConstraints::setPartitioningResult(
		const Partitioning::Result& result)
	{
		partitioningResult_ = & result;
	}

	void CircuitSpecificationsConstraints::setStructureRecognitionResult(const StructRec::StructureCircuits& circuits)
	{
		structRecResult_ = &circuits;
	}

	void CircuitSpecificationsConstraints::setGraph(
		const Graph& graph)
	{
		graph_ = & graph;
	}

	void CircuitSpecificationsConstraints::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	void CircuitSpecificationsConstraints::setTransistorToWidthMap(
		ComponentToIntVarMap& widthMap)
	{
		transistorToWidthMap_ = & widthMap;
	}

	void CircuitSpecificationsConstraints::setTransistorToLengthMap(
		ComponentToIntVarMap& lengthMap)
	{
		transistorToLengthMap_ = & lengthMap;
	}

	void CircuitSpecificationsConstraints::setTransistorToCurrentMap(
		ComponentToIntVarMap& currentMap)
	{
		transistorToCurrentMap_ = & currentMap;
	}

	void CircuitSpecificationsConstraints::setNetToVoltageMap(
		NetToIntVarMap& voltageMap)
	{
		netToVoltageMap_ = & voltageMap;
	}

	void CircuitSpecificationsConstraints::createConstraints()
	{
		logDebug("      CreatePolesAndZeros");
		createPolesAndZeros();

		logDebug("     Create gain constraint");
		Gecode::FloatVar gain = calculateGain();
		createGainConstraint(gain);

		logDebug("     Create simple second stage transconductance constraint");
		createSimpleSecondStageTransconductanceConstraint();

		logDebug("     Create Power Constraint");
		Gecode::FloatVar powerConsumption = calculatePowerConsumption();
		createPowerConsumptionConstraint(powerConsumption);

		logDebug("      Calculate TransientFrequency Constraints");
		Gecode::FloatVar transitFrequency = calculateTransientFrequency();
		logDebug("      Create TransientFrequency Constraints");
		createTransientFrequencyConstraint(transitFrequency);
		logDebug("      Calculate TransientFrequency Constraints With Error Factor");
		Gecode::FloatVar transitFrequencyWithErrorFactor = calculateTransientFrequencyWithErrorFactor();
		logDebug("      Create TransientFrequency Constraints With Error Factor");
		createTransientFrequencyConstraintWithErrorFactor(transitFrequencyWithErrorFactor);

		logDebug("     Create CompensationCapacitance Constraints");
		createCompensationCapacitanceConstraint();

		logDebug("     Create Area Constraint");
		Gecode::FloatVar area = calculateArea();
		createAreaConstraint(area);

		logDebug("      CreatePhaseMarginConstraint");
		Gecode::FloatVar phaseMargin = calculatePhaseMargin();
		createPhaseMarginConstraint(phaseMargin);

		logDebug("     Create SlewRate Constraints");
 		Gecode::FloatVar slewRate = calculateSlewRate();
		createSlewRateConstraint(slewRate);

		logDebug("     Create CommonModeInputVoltageConstraint");
		Gecode::FloatVar vcmMin, vcmMax;
		std::tie(vcmMin, vcmMax ) = calculateCommonModeInputVoltage();
		createCommonModeInputVoltageConstraint(vcmMin, vcmMax);

		logDebug("     createBiasOfCascodedPairCurrentConstrain");
		createBiasOfCascodedPairCurrentConstraint();

		logDebug("     Create CMRR Constraints");
		Gecode::FloatVar CMRR = calculateCMRR();
		createCMRRConstraint(CMRR);

		logDebug("     Create OutputVoltageSwing Constraints");
		Gecode::FloatVar voutMin, voutMax;
		std::tie(voutMin, voutMax) = calculateOutputVoltageSwing();
		createOutputVoltageSwingConstraint(voutMin, voutMax);

		logDebug("     Create PSRR Constraints");
		Gecode::FloatVar negPSRR, posPSRR;
		std::tie(negPSRR, posPSRR) = calculatePSRR();
		createPSRRConstraint(negPSRR, posPSRR);

		logDebug("    Create Complementary First Stage Constraint");
		createComplementaryFirstStageConstraint();

		logDebug("       Create Scalability Factor Limitation for symmetrical OTA Constraint");
		createScalibiltyFactorLimitationForSymmetricalOTAConstraint();

		createInnerVoltageBiasOfTelescopicOpAmpsConstraint();

		logDebug("             Creating Feed Back Circuit Constraint");
		createFeedbackCircuitConstraints();


		createOutputNetVoltageConstraintFirstStage();
	}

	void CircuitSpecificationsConstraints::createOptimizingConstraints()
	{
		logDebug("      CreatePolesAndZeros");
		createPolesAndZeros();

		logDebug("     Create gain constraint");
		calculateGain();

		logDebug("     Create simple second stage transconductance constraint");
		createSimpleSecondStageTransconductanceConstraint();

		logDebug("     Create Power Constraint");
		Gecode::FloatVar powerConsumption = calculatePowerConsumption();
		createPowerConsumptionConstraint(powerConsumption);

		logDebug("      Calculate TransientFrequency Constraints");
		calculateTransientFrequency();
		logDebug("      Create TransientFrequency Constraints");
		logDebug("      Calculate TransientFrequency Constraints With Error Factor");
		calculateTransientFrequencyWithErrorFactor();
		logDebug("      Create TransientFrequency Constraints With Error Factor");

		logDebug("     Create CompensationCapacitance Constraints");
		createCompensationCapacitanceConstraint();

		logDebug("     Create Area Constraint");
		Gecode::FloatVar area = calculateArea();
		createAreaConstraint(area);

		logDebug("      CreatePhaseMarginConstraint");
		Gecode::FloatVar phaseMargin = calculatePhaseMargin();
		createPhaseMarginConstraint(phaseMargin);

		logDebug("     Create SlewRate Constraints");
 		calculateSlewRate();

 		logDebug("     Create CommonModeInputVoltageConstraint");
		Gecode::FloatVar vcmMin, vcmMax;
		std::tie(vcmMin, vcmMax ) = calculateCommonModeInputVoltage();
		createCommonModeInputVoltageConstraint(vcmMin, vcmMax);

		logDebug("     createBiasOfCascodedPairCurrentConstrain");
		createBiasOfCascodedPairCurrentConstraint();

		logDebug("     Create CMRR Constraints");
		Gecode::FloatVar CMRR = calculateCMRR();
		createCMRRConstraint(CMRR);

		logDebug("     Create OutputVoltageSwing Constraints");
		calculateOutputVoltageSwing();

		logDebug("     Create PSRR Constraints");
		calculatePSRR();

		logDebug("    Create Complementary First Stage Constraint");
		createComplementaryFirstStageConstraint();

		logDebug("       Create Scalability Factor Limitation for symmetrical OTA Constraint");
		createScalibiltyFactorLimitationForSymmetricalOTAConstraint();

		createInnerVoltageBiasOfTelescopicOpAmpsConstraint();

		logDebug("             Creating Feed Back Circuit Constraint");
		createFeedbackCircuitConstraints();

		createOutputNetVoltageConstraintFirstStage();
	}

	void CircuitSpecificationsConstraints::setTwoPortToValueMap(
			ComponentToIntVarMap& valueMap)
	{
		twoPortToValueMap_ = & valueMap;
	}

	const ComponentToIntVarMap& CircuitSpecificationsConstraints::getTwoPortToValueMap() const
	{
		assert(twoPortToValueMap_ != NULL);
		return * twoPortToValueMap_;
	}


	const Partitioning::Result& CircuitSpecificationsConstraints::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	const Graph& CircuitSpecificationsConstraints::getGraph() const
	{
		assert(graph_ != NULL);
		return * graph_;
	}

	const CircuitInformation& CircuitSpecificationsConstraints::getCircuitInformation() const
	{
		assert(circuitInformation_ != NULL);
		return * circuitInformation_;
	}

	const ComponentToIntVarMap& CircuitSpecificationsConstraints::getTransistorToWidthMap() const
	{
		assert(transistorToWidthMap_ != NULL);
		return *transistorToWidthMap_;
	}

	const ComponentToIntVarMap& CircuitSpecificationsConstraints::getTransistorToLengthMap() const
	{
		assert(transistorToLengthMap_ != NULL);
		return *transistorToLengthMap_;
	}

	const ComponentToIntVarMap& CircuitSpecificationsConstraints::getTransistorToCurrentMap() const
	{
		assert(transistorToCurrentMap_ != NULL);
		return *transistorToCurrentMap_;
	}

	const NetToIntVarMap& CircuitSpecificationsConstraints::getNetToVoltageMap() const
	{
		assert(netToVoltageMap_ != NULL);
		return *netToVoltageMap_;
	}

	void CircuitSpecificationsConstraints::createGainConstraint(Gecode::FloatVar gain)
	{
		rel(getSpace(), gain >= getCircuitInformation().getCircuitSpecification().getGain());
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateGain()
	{
		Gecode::FloatVar gain(getSpace(), 0, 10000000000000000000000);

		Gecode::FloatVar gainFirstStage = createGainConstraintFirstStage();
		Gecode::FloatVar gainSecondStage = createGainConstraintSecondStage();
		Gecode::FloatVar gainThirdStage = createGainConstraintThirdStage();

		gain = Gecode::expr(getSpace(),  gainFirstStage * gainSecondStage * gainThirdStage);


		Gecode::FloatVar logGain(getSpace(), -100, 200);
		Gecode::rel(getSpace(), log(gain) == logGain );
		dom(getSpace(), logGain, -100, 200);
		getSpace().setGain(Gecode::expr(getSpace(), 8.685889638 * logGain));
		return Gecode::expr(getSpace(), 8.685889638 * logGain);
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateSlewRate()
	{
		std::vector<Gecode::FloatVar> slewRates;

		if(hasOutputNetWithCapacitor(getPartitioningResult().getFirstStage()))
		{
			slewRates.push_back(calculateSlewRateFirstStage());
		}
		if(getPartitioningResult().hasSecondStage() && hasOutputNetWithCapacitor(getPartitioningResult().getPrimarySecondStage()))
		{
			slewRates.push_back(calculateSlewRateSecondStage());
		}
		if(getPartitioningResult().hasThirdStage() && hasOutputNetWithCapacitor(getPartitioningResult().getThirdStage()))
		{
			slewRates.push_back(calculateSlewRate(getPartitioningResult().getThirdStage()));
		}
		if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage())
			&& !hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin())	)
		{
			float p =  0.0000000000001;
			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
			Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
			channel(getSpace(), valueHelperVarCompCap, compCapValue);
			Gecode::FloatVar cc = Gecode::expr(getSpace(), valueHelperVarCompCap * p);


			Partitioning::BiasPart & biasFirstStage = **getPartitioningResult().getFirstStage().getBiasPart().begin();
			Partitioning::Component & biasComp = **getPartitioningResult().getBelongingComponents(biasFirstStage).begin();
			Gecode::FloatVar biasCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp));

			slewRates.push_back(expr(getSpace(), abs(biasCurrent/cc)));
		}

		Gecode::FloatVarArgs slewRateArray(slewRates.size());
		int index = 0;
		for(auto & it_slewRates : slewRates)
		{
			slewRateArray[index] = it_slewRates;
			index++;
		}
		Gecode::FloatVar slewRate(getSpace(), 0, 1000000000000000000000000000);
		min(getSpace(), slewRateArray, slewRate);

		getSpace().setSlewRate(slewRate);
		return slewRate;

	}

	void CircuitSpecificationsConstraints::createSlewRateConstraint(Gecode::FloatVar slewRate)
	{
		rel(getSpace(), slewRate >= getCircuitInformation().getCircuitSpecification().getSlewRate());
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateSlewRateFirstStage()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		if(getPartitioningResult().getFirstStage().getLoadPart().size() == 2 )
		{

			if(!firstStage.isComplementary())
			{
				//Finding the smallest of the currents, which have an effect on the output
				Gecode::FloatVar current(getSpace(), -1,1);
				Partitioning::BiasPart & biasPart = **firstStage.getBiasPart().begin();
				Partitioning::Component& biasDiffPairComp = **getPartitioningResult().getBelongingComponents(biasPart).begin();
				Gecode::FloatVar biasDiffPairCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasDiffPairComp));

				const StructRec::StructureNet & outputNet = findOutputNetWithCapacitor(firstStage);
				Gecode::FloatVar capacitance =calculateNetCapacitance(outputNet);

				if(getPartitioningResult().hasBiasOfFoldedPair())
				{
					Gecode::FloatVar outputCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(**(getPartitioningResult().findComponents(firstStage.getHelperStructure()).begin())));

					if(getPartitioningResult().hasSlewRateEnhancer()
							&& (**getPartitioningResult().getAllSlewRateEnhancer().begin()).hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult()))
					{
						//Finding bias of folded Pair bias
						Partitioning::LoadPart * loadPartWithCascodePair = nullptr;
						for(auto & loadPart : firstStage.getLoadPart())
						{
							if(loadPart->hasCurrentBiasOfFoldedPair())
							{
								loadPartWithCascodePair = loadPart;
								break;
							}
						}
						assert(loadPartWithCascodePair != nullptr, "There should be a load part with a cascode pair and a bias of it");

						Partitioning::Component * biasFL1B = nullptr;
						Partitioning::Component& FL1B = getPartitioningResult().getBiasOfFoldedPair();

						for(auto & biasPart : loadPartWithCascodePair->getBiasParts())
						{
							for(auto & comp : getPartitioningResult().getBelongingComponents(*biasPart))
							{
								if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray"
										&& comp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).getIdentifier()
										== FL1B.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier() )
								{
									biasFL1B = comp;
								}
							}
						}

						assert(biasFL1B != nullptr, "The bias of the folded cascode op amp should have a external bias transistor otherwise the new slew rate current cannot be calculated");
						Gecode::FloatVar ids0FL1B = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(FL1B));
						Gecode::FloatVar ids0BiasFL1B = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasFL1B));

						Gecode::FloatVar a(getSpace(), 0,1000); //current ratio of FL1B and biasFL1B
						rel(getSpace(), a * abs(ids0BiasFL1B) == abs(ids0FL1B));
						dom(getSpace(), a , 1, 1000);
						rel(getSpace(), a >= 10);
						rel(getSpace(), a <= 50);

						Gecode::FloatVar idBiasFL1B(getSpace(), 0, 1);
						rel(getSpace(), idBiasFL1B * (1+a) == abs(biasDiffPairCurrent) + abs(ids0BiasFL1B));
						dom(getSpace(), idBiasFL1B, 0, 1);
						rel(getSpace(), abs(idBiasFL1B) > 0.000005 + 1.2* abs(ids0BiasFL1B));
						Gecode::FloatVar idFL1B = expr(getSpace(), a * idBiasFL1B);
						dom(getSpace(), idFL1B, 0, 1);

						return Gecode::expr(getSpace(), abs(idFL1B)/capacitance);
					}
					else
					{
						Gecode::rel(getSpace(), current == min(abs(biasDiffPairCurrent), abs(outputCurrent)));
						return Gecode::expr(getSpace(), abs(current)/capacitance);
					}

				}
				else
				{
					return Gecode::expr(getSpace(), abs(biasDiffPairCurrent)/capacitance);
				}
			}
			else
			{
				Gecode::FloatVar current(getSpace(), -1,1);
				Partitioning::Component * biasDiffPairCompP = nullptr;
				Partitioning::Component * biasDiffPairCompN = nullptr;
				Partitioning::Component * supplyLoadP = nullptr;
				Partitioning::Component * supplyLoadN = nullptr;
				const StructRec::StructureNet & outputNet = **findOutputNets(firstStage).begin();
				Gecode::FloatVar capacitance = calculateNetCapacitance(outputNet);

		// Sorting the two bias parts according to techtype
				Partitioning::BiasPart & biasPart1 = **firstStage.getBiasPart().begin();
				Partitioning::BiasPart & biasPart2 = **std::next(firstStage.getBiasPart().begin());
				Partitioning::Component& biasDiffPairComp1= **getPartitioningResult().getBelongingComponents(biasPart1).begin();
				Partitioning::Component& biasDiffPairComp2 =  **getPartitioningResult().getBelongingComponents(biasPart2).begin();

				assert(biasDiffPairComp1.getArray().getTechType() != biasDiffPairComp2.getArray().getTechType(), "The two Bias Parts of the first stage should have different tech types!");

				if(biasDiffPairComp1.getArray().getTechType().isP())
				{
					biasDiffPairCompP = &biasDiffPairComp1;
					biasDiffPairCompN = &biasDiffPairComp2;
				}
				else
				{
					biasDiffPairCompP = &biasDiffPairComp2;
					biasDiffPairCompN = &biasDiffPairComp1;
				}

				Gecode::FloatVar biasDiffPairCurrentN = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasDiffPairCompN));
				Gecode::FloatVar biasDiffPairCurrentP = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*biasDiffPairCompP));

				//Finding one supply transistor of each load and sorting them according the tech type
				Partitioning::Component * supplyComp1 = nullptr;
				Partitioning::Component * supplyComp2 = nullptr;
				Partitioning::LoadPart & loadPart1 = **firstStage.getLoadPart().begin();
				Partitioning::LoadPart & loadPart2 = **std::next(firstStage.getLoadPart().begin());

				for(auto & comp1 : getPartitioningResult().getBelongingComponents(loadPart1))
				{
					if(comp1->getArray().findNet(StructRec::StructurePinType(comp1->getArray().getStructureName(), "Source")).isSupply())
					{
						supplyComp1 = comp1;
						break;
					}
				}
				for(auto & comp2 : getPartitioningResult().getBelongingComponents(loadPart2))
				{
					if(comp2->getArray().findNet(StructRec::StructurePinType(comp2->getArray().getStructureName(), "Source")).isSupply())
					{
						supplyComp2 = comp2;
						break;
					}
				}
				assert(supplyComp1->getArray().getTechType() != supplyComp2->getArray().getTechType(), "The two loads should have different tech types");
				if(supplyComp1->getArray().getTechType().isP())
				{
					supplyLoadP = supplyComp1;
					supplyLoadN = supplyComp2;
				}
				else
				{
					supplyLoadP = supplyComp2;
					supplyLoadN = supplyComp1;
				}

				Gecode::FloatVar supplyLoadCurrentN = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*supplyLoadN));
				Gecode::FloatVar supplyLoadCurrentP = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*supplyLoadP));

				Gecode::FloatVar biasDiffPairCurrent = expr(getSpace(), abs(biasDiffPairCurrentN) + abs(biasDiffPairCurrentP));
				Gecode::FloatVar supplyLoadCurrent = expr(getSpace(), abs(supplyLoadCurrentN) + abs(supplyLoadCurrentP));
				Gecode::FloatVar mixedCurrentN = expr(getSpace(), abs(biasDiffPairCurrentN) + abs(supplyLoadCurrentN));
				Gecode::FloatVar mixedCurrentP = expr(getSpace(), abs(biasDiffPairCurrentP) + abs(supplyLoadCurrentP));

				Gecode::FloatVar current1 = expr(getSpace(), min(biasDiffPairCurrent, supplyLoadCurrent));
				Gecode::FloatVar current2 = expr(getSpace(), min(mixedCurrentP, mixedCurrentN));
				rel(getSpace(), current == min(current1, current2));
				return Gecode::expr(getSpace(), abs(current)/capacitance);
			}
		}
		else
		{
			return calculateSlewRate(firstStage);
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateSlewRateSecondStage()
	{
		Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
		if((*getPartitioningResult().getFirstStage().getLoadPart().begin())->hasCrossCoupledPair(getPartitioningResult()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::BiasPart & FB = **firstStage.getBiasPart().begin();

			Gecode::FloatVar currentFB = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(**getPartitioningResult().getBelongingComponents(FB).begin()));

			Gecode::FloatVar B = computeScalingFactorForSymmetricalOTA();
			const StructRec::StructureNet & outputNet = findOutputNetWithCapacitor(secondStage);
			Gecode::FloatVar capacitance = calculateNetCapacitance(outputNet);

			return Gecode::expr(getSpace(), abs(B * currentFB/(capacitance)));
		}
		else if(getPartitioningResult().hasSecondarySecondStage())
		{
			Partitioning::BiasPart & biasPart =**(secondStage.getBiasPart().begin());

			Gecode::FloatVar biasCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(**getPartitioningResult().getBelongingComponents(biasPart).begin()));
			const StructRec::StructureNet & outputNet = findOutputNetWithCapacitor(secondStage);
			Gecode::FloatVar capacitance = calculateNetCapacitance(outputNet);

			return Gecode::expr(getSpace(), abs(2*biasCurrent/(capacitance)));
		}
		else
		{
			return calculateSlewRate(secondStage);
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateSlewRateThirdStage()
	{
		return calculateSlewRate(getPartitioningResult().getThirdStage());
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateSlewRate(Partitioning::TransconductancePart & stage )
	{
		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
		Partitioning::Component& biasComp = **getPartitioningResult().getBelongingComponents(biasPart).begin();
		Gecode::FloatVar biasCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp));

		const StructRec::StructureNet & net = findOutputNetWithCapacitor(stage);

		Gecode::FloatVar cout = calculateNetCapacitance(net);


		return expr(getSpace(), abs(biasCurrent/cout));

	}


	Gecode::FloatVar CircuitSpecificationsConstraints::calculateTransientFrequency()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();

		Gecode::FloatVar gmTran(getSpace(), 0,1);

		if(!firstStage.isComplementary())
		{
			Partitioning::Component& tranComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			rel(getSpace(), gmTran == computeTransconductance(tranComp));
		}
		else
		{
			Partitioning::Component* pmosComp = nullptr;
			Partitioning::Component * nmosComp = nullptr;
			for(auto& comp: getPartitioningResult().getBelongingComponents(firstStage))
			{
				if(comp->getArray().getTechType().isP() && pmosComp == nullptr)
				{
					pmosComp = comp;
				}
				else if(comp->getArray().getTechType().isN() && nmosComp == nullptr)
				{
					nmosComp = comp;
				}
			}

			assert(pmosComp != nullptr && nmosComp != nullptr, "In a complementary first stage there should be both, nmos and pmos, devices!");

			Gecode::FloatVar gmPmos = computeTransconductance(*pmosComp);
			Gecode::FloatVar gmNmos = computeTransconductance(*nmosComp);

			rel(getSpace(), gmTran == gmPmos +gmNmos);
		}

		Gecode::FloatVar outputCapacitance (getSpace(), 0, 1);

		if(getPartitioningResult().getFirstStage().hasLoadPart() == 1
				&& getPartitioningResult().hasSecondarySecondStage())
		{

			Gecode::FloatVar B = computeScalingFactorForSymmetricalOTA();
			outputCapacitance = calculateNetCapacitance(findOutputNetWithCapacitor(getPartitioningResult().getPrimarySecondStage()));

			Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();

			if(loadFirstStage.hasCrossCoupledPair(getPartitioningResult()))
			{
				getSpace().setTransitFrequency( Gecode::expr(getSpace(),2* B * gmTran /(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(),2* B * gmTran /(2 * 3.141592653589793 * outputCapacitance));
			}
			else
			{
				getSpace().setTransitFrequency( Gecode::expr(getSpace(), B * gmTran /(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(), B * gmTran /(2 * 3.141592653589793 * outputCapacitance));
			}
		}
		else
		{

			const StructRec::StructureNet& outputNetFirstStage = **findOutputNetsFirstStage().begin();

			if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(firstStage, getPartitioningResult().getThirdStage())
					&& !hasCompensationCapacity(outputNetFirstStage))
			{
				float p =  0.0000000000001;
				Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
				Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
				Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
				Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
				channel(getSpace(), valueHelperVarCompCap, compCapValue);
				outputCapacitance = Gecode::expr(getSpace(), valueHelperVarCompCap * p);

			}
			else
			{
				outputCapacitance = calculateNetCapacitance(outputNetFirstStage);
			}

			if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
			{
				getSpace().setTransitFrequency(Gecode::expr(getSpace(), gmTran/(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(), gmTran/(2 * 3.141592653589793 * outputCapacitance));
			}
			else
			{
				getSpace().setTransitFrequency(Gecode::expr(getSpace(), gmTran/(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(), gmTran/(2 * 3.141592653589793 * outputCapacitance));
			}
		}

	}

	void CircuitSpecificationsConstraints::createTransientFrequencyConstraint(Gecode::FloatVar transitFrequency)
	{
		rel(getSpace(), transitFrequency > getCircuitInformation().getCircuitSpecification().getTransientFrequency());
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateTransientFrequencyWithErrorFactor()
	{

		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();

		Gecode::FloatVar gmTran(getSpace(), 0,1);

		if(!firstStage.isComplementary())
		{
			Partitioning::Component& tranComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			rel(getSpace(), gmTran == computeTransconductance(tranComp));
		}
		else
		{
			Partitioning::Component* pmosComp = nullptr;
			Partitioning::Component * nmosComp = nullptr;
			for(auto& comp: getPartitioningResult().getBelongingComponents(firstStage))
			{
				if(comp->getArray().getTechType().isP() && pmosComp == nullptr)
				{
					pmosComp = comp;
				}
				else if(comp->getArray().getTechType().isN() && nmosComp == nullptr)
				{
					nmosComp = comp;
				}
			}

			assert(pmosComp != nullptr && nmosComp != nullptr, "In a complementary first stage there should be both, nmos and pmos, devices!");

			Gecode::FloatVar gmPmos = computeTransconductance(*pmosComp);
			Gecode::FloatVar gmNmos = computeTransconductance(*nmosComp);

			rel(getSpace(), gmTran == gmPmos +gmNmos);
		}



		Gecode::FloatVar outputCapacitance (getSpace(), 0, 1);
		Gecode::FloatVar errorFactor = calculateErrorFactorTransientFrequency();
		dom(getSpace(), errorFactor, 0, 1000000000000000);

		if(getPartitioningResult().getFirstStage().hasLoadPart() == 1
				&& getPartitioningResult().hasSecondarySecondStage())
		{
			Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();
			Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();


			outputCapacitance = calculateNetCapacitance(findOutputNetWithCapacitor(secondStage));
			Gecode::FloatVar B = computeScalingFactorForSymmetricalOTA();


			if(loadFirstStage.hasCrossCoupledPair(getPartitioningResult()))
			{
				getSpace().setTransitFrequencyWithErrorFactor( Gecode::expr(getSpace(),2* B * gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(),2* B * gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance));
			}
			else
			{
				getSpace().setTransitFrequencyWithErrorFactor( Gecode::expr(getSpace(), B * gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance)));
				return Gecode::expr(getSpace(), B * gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance));
			}
		}
		else
		{

			const StructRec::StructureNet& outputNetFirstStage = **findOutputNetsFirstStage().begin();
			if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(firstStage, getPartitioningResult().getThirdStage())
					&& !hasCompensationCapacity(outputNetFirstStage))
			{
				float p =  0.0000000000001;
				Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
				Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
				Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
				Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
				channel(getSpace(), valueHelperVarCompCap, compCapValue);
				outputCapacitance = Gecode::expr(getSpace(), valueHelperVarCompCap * p);

			}
			else
			{
				outputCapacitance = calculateNetCapacitance(outputNetFirstStage);
			}


			getSpace().setTransitFrequencyWithErrorFactor(Gecode::expr(getSpace(), gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance)));
			return Gecode::expr(getSpace(), gmTran * errorFactor/(2 * 3.141592653589793 * outputCapacitance));

		}

	}

	void CircuitSpecificationsConstraints::createTransientFrequencyConstraintWithErrorFactor(Gecode::FloatVar transitFrequency)
	{
		dom(getSpace(), transitFrequency, 0, pow(10, 15));
		rel(getSpace(), transitFrequency >= getCircuitInformation().getCircuitSpecification().getTransientFrequency());
	}


	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePowerConsumption()
	{
		Gecode::FloatVar power(getSpace(), 0, getCircuitInformation().getCircuitSpecification().getPowerConsumption());
		const Node & supplyNode = getGraph().getSupplyNode();
		const Node & groundNode = getGraph().getGroundNode();

		float vddValue = getCircuitInformation().getCircuitParameter().getValue(supplyNode.getNetId()) - getCircuitInformation().getCircuitParameter().getValue(groundNode.getNetId());
		Gecode::FloatValArgs vdd;
		Gecode::FloatVarArgs current;
		const std::set<Edge*> incomingEdges = supplyNode.getIncomingEdges();
		const std::set<Edge*> outgoingEdges = supplyNode.getOutgoingEdges();
		Gecode::FloatValArgs vddInitial(incomingEdges.size() + outgoingEdges.size());
		Gecode::FloatVarArgs currentInitial(incomingEdges.size() + outgoingEdges.size());


		float biasCurrent = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getValue();
		if(biasCurrent<= 0)
		{
			vdd = vddInitial;
			current = currentInitial;
		}
		else
		{
			Gecode::FloatValArgs vddAddOn(1);
			Gecode::FloatVarArgs currentAddOn(1);
			vdd = vddInitial + vddAddOn;
			current = currentInitial + currentAddOn;

		}
		int index = 0;
		for(std::set<Edge*>::const_iterator it_edges = incomingEdges.begin(); it_edges != incomingEdges.end(); it_edges++ )
		{
			Edge * edge = *it_edges;

			current[index] = computeEdgeCurrent(*edge);
			vdd[index] = -1* vddValue;
			index++;
		}
		for(std::set<Edge*>::const_iterator it_edges = outgoingEdges.begin(); it_edges != outgoingEdges.end(); it_edges++ )
		{
			Edge * edge = *it_edges;
			current[index] = computeEdgeCurrent(*edge);

			vdd[index] =  vddValue;
			index++;
		}
		if(biasCurrent > 0)
		{
			vdd[index] = vddValue;
			Gecode::FloatVal biasCurrentGecode = biasCurrent;

			current[index] = Gecode::expr(getSpace(), biasCurrentGecode);
		}


		Gecode::linear(getSpace(), vdd, current, Gecode::FRT_EQ, power);

		getSpace().setPowerConsumption(power);
		return power;
	}

	void CircuitSpecificationsConstraints::createPowerConsumptionConstraint(Gecode::FloatVar powerConsumption)
	{
		Gecode::rel(getSpace(), powerConsumption <= getCircuitInformation().getCircuitSpecification().getPowerConsumption());
	}

	std::tuple<Gecode::FloatVar, Gecode::FloatVar> CircuitSpecificationsConstraints::calculateOutputVoltageSwing()
	{
		float u = getSpace().getScalingFactorMUM();
		float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();
		float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();

		std::vector<Path> vddToOutputPaths = getGraph().computeVddToOuputPathsWithOnlyDrainSourceEdges();
		std::vector<Path> outputToGroundPaths = getGraph().computeOutputToGroundPathsWithOnlyDrainSourceEdges();

		Gecode::FloatVarArray voutMaxValues(getSpace(),vddToOutputPaths.size());
		int vddToOutputPathIndex = 0;

		for(auto & it_vddToOutputPath : vddToOutputPaths)
		{

			std::vector<const Edge*> vddToOutputPath = it_vddToOutputPath.getEdges();
			Gecode::FloatValArgs signVddToOutputPath(vddToOutputPath.size());
			Gecode::FloatVarArgs currentLengthWidthRatioVddTouOutputPath(vddToOutputPath.size());
			Gecode::FloatVarArgs voltageVddTouOutputPath(vddToOutputPath.size());
			int indexVddToOutputPath = 0;
			float numTran= 0;

			for(auto & edge : vddToOutputPath)
			{
				Partitioning::Component & comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());
				const Edge* gateSourceEdge = nullptr;
				if(comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
					gateSourceEdge = edge;
				else
				{
					numTran++;
					for(auto & it: getGraph().findToComponentBelongingEdges(comp))
					{
						if(it->isGateSourceEdge())
						{
							gateSourceEdge = it;
						}

					}
				}
				assert(gateSourceEdge != nullptr, "There must be a edge for every component!");

				voltageVddTouOutputPath[indexVddToOutputPath] = computeEdgeVoltage(*gateSourceEdge);
				signVddToOutputPath[indexVddToOutputPath] =1;
				indexVddToOutputPath++;
			}

			Partitioning::Component & edgeComp = getPartitioningResult().findComponent((**it_vddToOutputPath.getEdges().begin()).getEdgeKey().getStructureId());

			if(edgeComp.getArray().isPartOfCurrentMirror())
			{
				for(auto & parent : edgeComp.getArray().getTopmostParents())
				{
					if(parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
					{
						numTran = numTran-1;
						break;
					}
				}
			}

			float vthP = getCircuitInformation().getTechnologieSpecificationSHMPmos().getThresholdVoltage();

			Gecode::FloatVar voutMaxValue(getSpace(), -1000000000000, 1000000000000);
			voutMaxValues[vddToOutputPathIndex] = voutMaxValue;
			vddToOutputPathIndex++;

			Gecode::FloatVar rightSideValue = expr(getSpace(), voutMaxValue -vdd+ numTran * vthP);
			dom(getSpace(), rightSideValue, -10000000000000, 100000000000000);
//			Gecode::linear(getSpace(), signVddToOutputPath, currentLengthWidthRatioVddTouOutputPath, Gecode::FRT_EQ, voltageSquaredVddValue);
			Gecode::linear(getSpace(), signVddToOutputPath, voltageVddTouOutputPath, Gecode::FRT_EQ, rightSideValue);
		}

		Gecode::FloatVarArray voutMinValues(getSpace(),outputToGroundPaths.size());
		int outputToGroundPathIndex = 0;
		for(auto & it_outputToGroundPath : outputToGroundPaths)
		{
			std::vector<const Edge*> outputToGroundPath = it_outputToGroundPath.getEdges();
			Gecode::FloatValArgs signOutputToGroundPath(outputToGroundPath.size());
			Gecode::FloatVarArgs currentLengthWidthRatioOutputToGroundPath(outputToGroundPath.size());
			Gecode::FloatVarArgs voltageOutputToGroundPath(outputToGroundPath.size());
			int indexOutputToGroundPath = 0;
			float numTran = 0;
			for(auto & edge : outputToGroundPath)
			{
				Partitioning::Component & comp = getPartitioningResult().findComponent(edge->getEdgeKey().getStructureId());

				const Edge* gateSourceEdge = nullptr;
				if(comp.getArray().getStructureName().toStr() == "MosfetDiodeArray")
					gateSourceEdge = edge;
				else
				{
					numTran++;
					for(auto & it: getGraph().findToComponentBelongingEdges(comp))
					{
						if(it->isGateSourceEdge())
							gateSourceEdge = it;
					}
				}
				assert(gateSourceEdge != nullptr, "There must be a edge for every component!");

				voltageOutputToGroundPath[indexOutputToGroundPath] = computeEdgeVoltage(*gateSourceEdge);
				signOutputToGroundPath[indexOutputToGroundPath] =1;
				indexOutputToGroundPath++;
			}

			Partitioning::Component & edgeComp = getPartitioningResult().findComponent((**it_outputToGroundPath.getEdges().begin()).getEdgeKey().getStructureId());

			if(edgeComp.getArray().isPartOfCurrentMirror())
			{
				for(auto & parent : edgeComp.getArray().getTopmostParents())
				{
					if(parent->getStructureName().toStr() == "MosfetCascodeCurrentMirror")
					{
						numTran = numTran-1;
						break;
					}
				}
			}

			float vthN = getCircuitInformation().getTechnologieSpecificationSHMNmos().getThresholdVoltage();
			Gecode::FloatVar voutMinValue(getSpace(), -10000, 10000);
			voutMinValues[outputToGroundPathIndex] = voutMinValue;
			outputToGroundPathIndex++;

			Gecode::FloatVar rightSideValue = expr(getSpace(),voutMinValue - vss + numTran * vthN);
			Gecode::linear(getSpace(), signOutputToGroundPath, voltageOutputToGroundPath, Gecode::FRT_EQ, rightSideValue);
		}

		Gecode::FloatVar voutMaxValue(getSpace(), -100000000,100000000);

		Gecode::FloatVar voutMinValue(getSpace(), -10000000, 100000000);

		min(getSpace(), voutMaxValues, voutMaxValue);
		max(getSpace(), voutMinValues, voutMinValue);

		getSpace().setMaximumOutputVoltage(voutMaxValue);
		getSpace().setMinimumOutputVoltage(voutMinValue);
		return std::make_tuple(voutMinValue, voutMaxValue);
	}

	void CircuitSpecificationsConstraints::createOutputVoltageSwingConstraint(Gecode::FloatVar voutMin, Gecode::FloatVar voutMax)
	{
		rel(getSpace(), voutMin <= getCircuitInformation().getCircuitSpecification().getMinOutputVoltage());
		rel(getSpace(), voutMax >= getCircuitInformation().getCircuitSpecification().getMaxOutputVoltage());
	}


	std::tuple<Gecode::FloatVar, Gecode::FloatVar> CircuitSpecificationsConstraints::calculateCommonModeInputVoltage()
	{
		float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();
		float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();
		Gecode::FloatVar vcmMinValue(getSpace(), -10000 , 10000);
		Gecode::FloatVar vcmMaxValue(getSpace(), -100000, 100000);

		if(!getPartitioningResult().getFirstStage().isComplementary())
		{
		float u = getSpace().getScalingFactorMUM();
		std::vector<Partitioning::Component * > inputToSupplyRailPathOverBias = computeInputToSupplyRailPathOverBias(getPartitioningResult().getFirstStage());
		std::vector<Partitioning::Component *> inputToSupplyRailPathOverLoad = computeInputToSupplyRailPathOverLoad(getPartitioningResult().getFirstStage());

		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		Partitioning::Component * diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
		float vthFirstStage;
		if(getTransistorModel() =="SHM")
		{
				const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*diffStageTran);
				vthFirstStage = techSpec.getThresholdVoltage();
		}
		else
		{
				const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*diffStageTran);
				vthFirstStage = techSpec.getThresholdVoltage();
		}

		Gecode::FloatValArgs signInputToSupplyRailPathOverLoad(inputToSupplyRailPathOverLoad.size() -1);
		Gecode::FloatVarArgs voltageInputToSupplyRailPathOverLoad(inputToSupplyRailPathOverLoad.size() -1);
		int indexInputToSupplyRailPathOverLoad = 0;
		for(auto & comp : inputToSupplyRailPathOverLoad)
		{
			if(!comp->getPart().isTransconductancePart())
			{

				Edge * drainSourceEdge = NULL;
				Edge * gateSourceEdge = NULL;
				for(auto& edge: getGraph().findToComponentBelongingEdges(*comp) )
				{
					if(edge->isDrainSourceEdge())
					{
						drainSourceEdge = edge;
					}
					else if(edge->isGateSourceEdge())
					{
						gateSourceEdge = edge;
					}
				}

				if(gateSourceEdge == nullptr)
					gateSourceEdge = drainSourceEdge;

				assert(drainSourceEdge != NULL, "Every transistor should have a drain source Edge");

				voltageInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = computeEdgeVoltage(*gateSourceEdge);


				if(getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp))
					signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = -1;
				else
					signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] =1;
				indexInputToSupplyRailPathOverLoad++;
			}
		}

		Gecode::FloatValArgs signInputToSupplyRailPathOverBias(inputToSupplyRailPathOverBias.size());

		Gecode::FloatVarArgs voltageInputToSupplyRailPathOverBias(inputToSupplyRailPathOverBias.size());
		int indexInputToSupplyRailPathOverBias = 0;
		for(auto & comp : inputToSupplyRailPathOverBias)
		{
			Edge * gateSourceEdge = NULL;
			for(auto& edge: getGraph().findToComponentBelongingEdges(*comp) )
			{
				if(edge->isGateSourceEdge())
				{
					gateSourceEdge = edge;
				}
				else if(comp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
					&& edge->isDrainSourceEdge())
				{
					gateSourceEdge = edge;
				}
			}
			assert(gateSourceEdge != NULL, "Every transistor in the transconductance to bias path should have a gate source Edge");

			voltageInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] = computeEdgeVoltage(*gateSourceEdge);


			signInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] =1;

			indexInputToSupplyRailPathOverBias++;

		}
		Gecode::FloatVar rightSideEqualityLoad(getSpace(), -100000, 100000);
		Gecode::FloatVar rightSideEqualityBias(getSpace(), -100000, 100000);

		if(diffStageTran->getArray().getTechType().isP())
		{
			float constantInputToSupplyRailPathOverLoad = 0;
			for(auto & comp : inputToSupplyRailPathOverLoad)
			{
				float vth;
				if(getTransistorModel() =="SHM")
				{
					const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
					vth = techSpec.getThresholdVoltage();
				}
				else
				{
					const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
					vth = techSpec.getThresholdVoltage();
				}

				if(comp->getPart().isTransconductancePart())
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad +vth;
				}
				else if(getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp))
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad +vth;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray" )
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad-vth;
				}
			}
			rightSideEqualityLoad = expr(getSpace(), vcmMinValue - constantInputToSupplyRailPathOverLoad - vss);
			rightSideEqualityBias = expr(getSpace(),  vcmMaxValue -vdd + vthFirstStage);




		}
		else
		{
			float constantInputToSupplyRailPathOverLoad = 0;
			for(auto & comp : inputToSupplyRailPathOverLoad)
			{
				float vth;
				if(getTransistorModel() =="SHM")
				{
					const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
					vth = techSpec.getThresholdVoltage();
				}
				else
				{
					const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
					vth = techSpec.getThresholdVoltage();
				}

				if(comp->getPart().isTransconductancePart())
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad+vth;
				}
				else if(getPartitioningResult().isPartOfHelperStructureOfDifferentialPair(*comp))
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad +vth;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray" )
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad-vth;
				}
			}



			rightSideEqualityBias = expr(getSpace(), vcmMinValue - vss + vthFirstStage);
			rightSideEqualityLoad = expr(getSpace(),  vcmMaxValue - constantInputToSupplyRailPathOverLoad  - vdd);

		}


		getSpace().setHelper(rightSideEqualityLoad);
		Gecode::linear(getSpace(), signInputToSupplyRailPathOverLoad, voltageInputToSupplyRailPathOverLoad, Gecode::FRT_EQ, rightSideEqualityLoad);
		Gecode::linear(getSpace(), signInputToSupplyRailPathOverBias, voltageInputToSupplyRailPathOverBias, Gecode::FRT_EQ, rightSideEqualityBias);


		getSpace().setMinimumCommonModeInputVoltage(vcmMinValue);
		getSpace().setMaximumCommonModeInputVoltage(vcmMaxValue);
		return std::make_tuple(vcmMinValue,vcmMaxValue);
		}
		else
		{
			rel(getSpace(), vcmMinValue == vss);
			rel(getSpace(), vcmMaxValue == vdd);
			return std::make_tuple(vcmMinValue,vcmMaxValue);
		}
	}

	void CircuitSpecificationsConstraints::createCommonModeInputVoltageConstraint(Gecode::FloatVar vcmMinVar, Gecode::FloatVar vcmMaxVar)
	{
		if(!getPartitioningResult().getFirstStage().isComplementary())
		{
			float vcmMin;
			float vcmMax;

			float vinPlus = getCircuitInformation().getCircuitParameter().getInputPinPlus().getValue();
			float vinMinus = getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue();
			float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();
			float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();

			if(vinMinus<vinPlus)
			{
				vcmMin = vinMinus +getCircuitInformation().getCircuitSpecification().getMinCommonModeInputVoltage();
				vcmMax = vinPlus + getCircuitInformation().getCircuitSpecification().getMaxCommonModeInputVoltage();
			}
			else
			{
				vcmMin = vinPlus +getCircuitInformation().getCircuitSpecification().getMinCommonModeInputVoltage();
				vcmMax = vinMinus + getCircuitInformation().getCircuitSpecification().getMaxCommonModeInputVoltage();
			}
			rel(getSpace(), vcmMinVar <= vcmMin);
			rel(getSpace(), vcmMaxVar >= vcmMax);
		}
	}


	std::tuple<Gecode::FloatVar, Gecode::FloatVar> CircuitSpecificationsConstraints::calculatePSRR()
	{
		Gecode::FloatVar posPSRR(getSpace(), 0, 1000000000000);

		Gecode::FloatVar negPSRR(getSpace(), -10000000000000000, 10000000000000000);
		if(!getPartitioningResult().getFirstStage().hasHelperStructure() && getPartitioningResult().hasSecondStage())
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component * diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
			Gecode::FloatVar gmDiffStage = computeTransconductance(*diffStageTran);
			Gecode::FloatVar  gdDiffStage = computeOutputConductance(*diffStageTran);

			Partitioning::BiasPart * biasOfFirstStage = *firstStage.getBiasPart().begin();
			Partitioning::Component * biasTranFirstStage = *getPartitioningResult().getBelongingComponents(*biasOfFirstStage).begin();
			Gecode::FloatVar  gdBiasFirstStage= computeOutputConductance(*biasTranFirstStage);

			Partitioning::TransconductancePart & secondStage = **getPartitioningResult().getSecondStages().begin();
			Partitioning::Component * secondStageTran = *getPartitioningResult().getBelongingComponents(secondStage).begin();
			Gecode::FloatVar gmSecondStage = computeTransconductance(*secondStageTran);
			Gecode::FloatVar gdSecondStage = computeOutputConductance(*secondStageTran);

			Partitioning::BiasPart * biasOfSecondStage = *secondStage.getBiasPart().begin();
			Partitioning::Component * biasTranSecondStage = *getPartitioningResult().getBelongingComponents(*biasOfSecondStage).begin();
			Gecode::FloatVar gdBiasSecondStage = computeOutputConductance(*biasTranSecondStage);

			Gecode::FloatVar firstStageGain = createGainConstraintFirstStage();
			dom(getSpace(), firstStageGain, 0, 100000000000000000000000);
			std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
			if(loadParts.size() ==1)  //Miller Operational Amplifier
			{
				Partitioning::LoadPart * loadOfFirstStage = *firstStage.getLoadPart().begin();
				Partitioning::Component* loadTranFirstStage = *getPartitioningResult().getBelongingComponents(*loadOfFirstStage).begin();

				Gecode::FloatVar gmLoadFirstStage = computeTransconductance(*loadTranFirstStage);
				Gecode::FloatVar  gdLoadFirstStage = computeOutputConductance(*loadTranFirstStage);


				if(getPartitioningResult().getSecondStages().size() == 1)
				{

					if(diffStageTran->getArray().getTechType().isP())
					{

						// Allan/Holberg + Paper Geometric Algorithm
						Gecode::rel(getSpace(),(2* firstStageGain* gmLoadFirstStage * gmSecondStage)/ ((2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage )) == posPSRR);

						Gecode::rel(getSpace(),(firstStageGain * gmSecondStage)/ ( gdSecondStage) == negPSRR);

					}
					else
					{
						// Allan/Holberg + Paper Geometric Algorithm
						Gecode::FloatVar minuend =expr(getSpace(), 2* gmLoadFirstStage * gdBiasSecondStage);
						dom(getSpace(), minuend, 0, 10000);

						Gecode::FloatVar subtrahend =expr(getSpace(), gmSecondStage * gdBiasFirstStage );
						dom(getSpace(), subtrahend, 0, 10000);

						Gecode::FloatVar factor =expr(getSpace(), (minuend - subtrahend));
						dom(getSpace(), factor, -1000, 100000);

						Gecode::FloatVar multiplication =expr(getSpace(), negPSRR * factor);
						dom(getSpace(), multiplication, -1000, 10000000000000);

						Gecode::rel(getSpace(),(2* firstStageGain * gmLoadFirstStage * gmSecondStage)  == multiplication );
						Gecode::rel(getSpace(),(firstStageGain * gmSecondStage)/ (gdSecondStage) == posPSRR);

					}
				}
				else
				{

					if(diffStageTran->getArray().getTechType().isP())

					{
						// positive PSRR
						Gecode::rel(getSpace(), (2* firstStageGain *gmLoadFirstStage * gmSecondStage)/ ( (2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage ))> pow(10, getCircuitInformation().getCircuitSpecification().getPosPSRR()/20));

						// negative PSRR
						Gecode::rel(getSpace(), (firstStageGain * gmSecondStage)/ ( gdSecondStage) >= pow(10, getCircuitInformation().getCircuitSpecification().getNegPSRR()/20));


						// Allan/Holberg + Paper Geometric Algorithm
						Gecode::rel(getSpace(),(2* firstStageGain * gmLoadFirstStage * gmSecondStage)/ ( abs(2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage )) == posPSRR);

						Gecode::rel(getSpace(),(firstStageGain * gmSecondStage)/ (gdSecondStage) == negPSRR);

					}
					else
					{
//						 negative PSRR
						Gecode::rel(getSpace(), (2* firstStageGain * gmLoadFirstStage * gmSecondStage)/ ( abs(2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage ))> pow(10, getCircuitInformation().getCircuitSpecification().getNegPSRR()/20));

						// positive PSRR
						Gecode::rel(getSpace(), (firstStageGain * gmSecondStage)/ ( gdSecondStage) >= pow(10, getCircuitInformation().getCircuitSpecification().getPosPSRR()/20));


						// Allan/Holberg + Paper Geometric Algorithm
						Gecode::rel(getSpace(),(2* firstStageGain * gmLoadFirstStage * gmSecondStage)/ ( abs(2* gmLoadFirstStage * gdBiasSecondStage - gmSecondStage * gdBiasFirstStage )) == negPSRR);

						Gecode::rel(getSpace(),(firstStageGain * gmSecondStage)/ (gdSecondStage) == posPSRR);

					}

				}

				Gecode::FloatVar logNegPSRR(getSpace(), 0, 300);
				Gecode::rel(getSpace(), log(negPSRR) == logNegPSRR );
				dom(getSpace(), logNegPSRR, 0, 200);
				getSpace().setNegPSRR(Gecode::expr(getSpace(), 8.685889638 * logNegPSRR));


				Gecode::FloatVar logPosPSRR(getSpace(), 0, 200);
				Gecode::rel(getSpace(), log(posPSRR) == logPosPSRR );
				dom(getSpace(), logPosPSRR, 0, 200);
				getSpace().setPosPSRR(Gecode::expr(getSpace(), 8.685889638 * logPosPSRR));
				return std::make_tuple(negPSRR, posPSRR);

			}
			else
			{
				rel(getSpace(), posPSRR == getCircuitInformation().getCircuitSpecification().getPosPSRR()+10);
				rel(getSpace(), negPSRR == getCircuitInformation().getCircuitSpecification().getNegPSRR()+10);
				return std::make_tuple(negPSRR, posPSRR);
			}
		}
		else
		{
			rel(getSpace(), posPSRR == getCircuitInformation().getCircuitSpecification().getPosPSRR()+10);
			rel(getSpace(), negPSRR == getCircuitInformation().getCircuitSpecification().getNegPSRR()+10);
			return std::make_tuple(negPSRR, posPSRR);
		}

	}

	void CircuitSpecificationsConstraints::createPSRRConstraint(Gecode::FloatVar negPSRR, Gecode::FloatVar posPSRR)
	{
		if(!getPartitioningResult().getFirstStage().hasHelperStructure() && getPartitioningResult().hasSecondStage())
		{
			rel(getSpace(), posPSRR >= getCircuitInformation().getCircuitSpecification().getPosPSRR());
			rel(getSpace(), negPSRR >= getCircuitInformation().getCircuitSpecification().getNegPSRR());
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateCMRR()
	{

		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		Partitioning::Component * diffStageTran = *getPartitioningResult().getBelongingComponents(firstStage).begin();
		Partitioning::BiasPart * biasOfFirstStage = *firstStage.getBiasPart().begin();
		Partitioning::Component * biasTranFirstStage = *getPartitioningResult().getBelongingComponents(*biasOfFirstStage).begin();



		Gecode::FloatVar firstStageGain = createGainConstraintFirstStage();
		dom(getSpace(), firstStageGain, 0, 10000000000000000);
		Gecode::FloatVar  gdBiasFirstStage = computeOutputConductance(*biasTranFirstStage);

		std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		Partitioning::LoadPart * loadPart = nullptr;
		for(auto & loadPartIt : loadParts)
		{
			if(!loadPartIt->hasCascodedPair())
			{
				loadPart = loadPartIt;
				break;
			}
		}
		assert(loadPart != nullptr, "Every first stage should have load part without gate connected couple");

		Core::MosfetDeviceType mosfet;
		std::vector<const Core::Net*> outputNetsFirstStageTrans = firstStage.getOutputNets();

		Partitioning::Component * loadComp = nullptr;

		for(auto & loadCompIt : getPartitioningResult().getBelongingComponents(*loadPart))
		{
			const Core::Device & loadDevice = **loadCompIt->getArray().getDevices().begin();
			const Core::Net & gateNet = loadDevice.findNet(mosfet.gate());

			for(auto & outputNetFirstStage : outputNetsFirstStageTrans)
			{

				if(gateNet.getIdentifier() == outputNetFirstStage->getIdentifier())
				{
					loadComp = loadCompIt;
					break;
				}
				if(loadComp != nullptr)
					break;
			}
		}

		if(loadComp == nullptr)
		{
			for(auto & loadCompIt : getPartitioningResult().getBelongingComponents(*loadPart))
			{
				const Core::Device & loadDevice = **loadCompIt->getArray().getDevices().begin();
				const Core::Net & drainNet = loadDevice.findNet(mosfet.drain());

				for(auto & outputNetFirstStage : outputNetsFirstStageTrans)
				{
					if(drainNet.getIdentifier() == outputNetFirstStage->getIdentifier())
					{
						loadComp = loadCompIt;
						break;
					}
				}
				if(loadComp != nullptr)
					break;
			}
			assert(loadComp != nullptr, "there must be a load component which is connected with its drain to the output of the first stage");
		}

		Gecode::FloatVar gmLoad = computeTransconductance(*loadComp);
		Gecode::FloatVar CMRR(getSpace(), 0, 1000000000000);


		if(getPartitioningResult().hasSecondarySecondStage())
		{
			Gecode::FloatVar secondStageGain = createGainConstraintSecondStage();
			Gecode::FloatVar opAmpGain = Gecode::expr(getSpace(), firstStageGain *secondStageGain);
			dom(getSpace(), opAmpGain, 0, pow(10,12));

			Gecode::FloatVar inverseCommonModeGain = Gecode::expr(getSpace(), 2*gmLoad/ gdBiasFirstStage);
			dom(getSpace(), inverseCommonModeGain, 0, pow(10,12));

			Gecode::rel(getSpace(), opAmpGain* inverseCommonModeGain == CMRR);


			Gecode::FloatVar logCMRR(getSpace(), 0, 200);
			Gecode::rel(getSpace(), log(CMRR) == logCMRR );

			getSpace().setCMRR(Gecode::expr(getSpace(), 8.685889638 * logCMRR));
			return Gecode::expr(getSpace(), 8.685889638 * logCMRR);
		}
		else
		{
			Gecode::rel(getSpace(),2* firstStageGain * gmLoad/ gdBiasFirstStage== CMRR);

			Gecode::FloatVar logCMRR(getSpace(), 0, 200);
			Gecode::rel(getSpace(), log(CMRR) == logCMRR );

			getSpace().setCMRR(Gecode::expr(getSpace(), 8.685889638 * logCMRR));
			return Gecode::expr(getSpace(), 8.685889638 * logCMRR);
		}

	}

	void CircuitSpecificationsConstraints::createCMRRConstraint(Gecode::FloatVar CMRR)
	{
		rel(getSpace(), CMRR >= getCircuitInformation().getCircuitSpecification().getCMRR());
	}

	void CircuitSpecificationsConstraints::setSpace(SearchSpace& space)
	{
		space_ = & space;
	}

	SearchSpace& CircuitSpecificationsConstraints::getSpace() const
	{
		assert(space_ != NULL);
		return *space_;
	}

	void CircuitSpecificationsConstraints::createCompensationCapacitanceConstraint()
	{
		float p =  0.0000000000001;
		if(getPartitioningResult().hasCompensationCapacitance())
		{

			if(!getPartitioningResult().hasThirdStage())
			{
				Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
				Partitioning::Component  *compensationCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
				Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compensationCapComp);
				Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
				channel(getSpace(),  valueHelperVarCompCap, compCapValue);

				Partitioning::CapacitancePart & loadCap = getPartitioningResult().getLoadCapacity(getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getDeviceId());
				Partitioning::Component * loadCapComp = *getPartitioningResult().getBelongingComponents(loadCap).begin();
				Gecode::IntVar loadCapValue = getTwoPortToValueMap().find(*loadCapComp);
				Gecode::FloatVar valueHelperVarLoadCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
				channel(getSpace(), valueHelperVarLoadCap, loadCapValue);

				Gecode::rel(getSpace(), valueHelperVarLoadCap * 0.22 < valueHelperVarCompCap);
			}
			else
			{
				Gecode::FloatVar cc1(getSpace(), 0, 1);
				Gecode::FloatVar cc2(getSpace(), 0, 1);


				for(auto & cap : getPartitioningResult().getCompensationCapacitors())
				{
					Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(*cap).begin();
					Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
					Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
					channel(getSpace(), valueHelperVarCompCap, compCapValue);

					Gecode::FloatVar normedCompCapValue = Gecode::expr(getSpace(), valueHelperVarCompCap * p);
					dom(getSpace(), normedCompCapValue, 0, 1);



					if(compensationCapacityConnectedBetween(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage()))
					{
						rel(getSpace(), cc1 == normedCompCapValue);
					}
					else if(compensationCapacityConnectedBetween(getPartitioningResult().getThirdStage(), getPartitioningResult().getFirstStage()))
					{
						rel(getSpace(), cc2 == normedCompCapValue);
					}
				}

				Partitioning::CapacitancePart & loadPart = **getPartitioningResult().getLoadCapacities().begin();
				Partitioning::Component * loadCapComp = *getPartitioningResult().getBelongingComponents(loadPart).begin();
				Gecode::IntVar loadCapValue = getTwoPortToValueMap().find(*loadCapComp);
				Gecode::FloatVar valueHelperVarLoadCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
				channel(getSpace(), valueHelperVarLoadCap, loadCapValue);
				Gecode::FloatVar normedLoadCapValue = Gecode::expr(getSpace(), valueHelperVarLoadCap * p);
				dom(getSpace(), normedLoadCapValue, 0, 1);

				Gecode::FloatVar gmFirstStage = computeTransconductance(getPartitioningResult().getFirstStage());
				dom(getSpace(), gmFirstStage, 0, 1);
				Gecode::FloatVar gmSecondStage = computeTransconductance(getPartitioningResult().getPrimarySecondStage());
				dom(getSpace(), gmSecondStage, 0, 1);
				Gecode::FloatVar gmThirdStage = computeTransconductance(getPartitioningResult().getThirdStage());
				dom(getSpace(), gmThirdStage, 0, 1);

				Gecode::FloatVar gm1Cc2 = Gecode::expr(getSpace(), 2* gmFirstStage* cc2);
				dom(getSpace(), gm1Cc2, 0, 1);
				Gecode::FloatVar gm2Cc1 = Gecode::expr(getSpace(), 2* gmSecondStage* cc1);
				dom(getSpace(), gm2Cc1, 0, 1);

				Gecode::rel(getSpace(),  gm1Cc2 >= 0.9 * gm2Cc1);
				Gecode::rel(getSpace(),  gm1Cc2 <= 1.1 * gm2Cc1);

				Gecode::FloatVar gm1Cl = Gecode::expr(getSpace(), 4* gmFirstStage* normedLoadCapValue);
				dom(getSpace(), gm1Cl, 0, 1);
				Gecode::FloatVar gm3Cc1 = Gecode::expr(getSpace(), 2* gmThirdStage* cc1);
				dom(getSpace(), gm3Cc1, 0, 1);

				Gecode::rel(getSpace(),  gm1Cl >= 0.9 * gm3Cc1);
				Gecode::rel(getSpace(),  gm1Cl <= 1.1 * gm3Cc1);

				Gecode::rel(getSpace(), 10* gmFirstStage < gmThirdStage);
				Gecode::rel(getSpace(), 10* gmSecondStage < gmThirdStage);



			}

		}
	}

	void CircuitSpecificationsConstraints::createBiasOfCascodedPairCurrentConstraint()
	{
		if(getPartitioningResult().hasBiasOfFoldedPair() && !getPartitioningResult().hasThirdStage()&& !(getPartitioningResult().hasSlewRateEnhancer() || getPartitioningResult().hasFirstStageGainEnhancer()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component * transComp = *getPartitioningResult().getBelongingComponents(firstStage).begin();
			Gecode::FloatVar idsDiffPair = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*transComp));
			std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
			for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
			{
				Partitioning::LoadPart * loadPart = * it;
				 //Only LoadParts Consisting of 4 transistors are supported
				if(loadPart->hasCascodedPair())
				{

					std::vector<const StructRec::Structure*> biasStructures =  loadPart->getCurrentBiasOfFoldedPair();
					Partitioning::Component& biasCompCascodedPair = getPartitioningResult().findComponent((*biasStructures.begin())->getIdentifier());


					Gecode::FloatVar idsBiasCascodedPair = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasCompCascodedPair));

					Gecode::FloatVar absIdsDiffPair = expr(getSpace(), abs(idsDiffPair));
					Gecode::FloatVar absIdsBiasCascodedPair = expr(getSpace(), abs(idsBiasCascodedPair));
					dom(getSpace(), absIdsDiffPair, 0, 1);
					dom(getSpace(), absIdsBiasCascodedPair, 0, 1);


					Gecode::rel(getSpace(), 2.4 * absIdsDiffPair < absIdsBiasCascodedPair);
					Gecode::rel(getSpace(), 3 * absIdsDiffPair > absIdsBiasCascodedPair);


				}
			}
		}
	}

	void CircuitSpecificationsConstraints::createSimpleSecondStageTransconductanceConstraint()
	{
		//Requirements on the the transconductance of first and second stage

		if(getPartitioningResult().hasSecondStage() && getPartitioningResult().getSecondStages().size() ==1
				&& !getCircuitInformation().getDesignAttributes().transconductancesInWeakInversion(getPartitioningResult()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component * firstStageComp = *getPartitioningResult().getBelongingComponents(firstStage).begin();
			Gecode::FloatVar gmFirstStage = computeTransconductance(*firstStageComp);

			Partitioning::TransconductancePart & secondStage = **getPartitioningResult().getSecondStages().begin();
			Partitioning::Component * secondStageComp = *getPartitioningResult().getBelongingComponents(secondStage).begin();
			Gecode::FloatVar gmSecondStage = computeTransconductance(*secondStageComp);

			rel(getSpace(), gmSecondStage > 10 * gmFirstStage);
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateArea()
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		Gecode::IntVarArgs tranArea(transistors.size());
		Gecode::IntArgs sign(transistors.size());
		int index = 0;
		for(std::vector<Partitioning::Transistor*>::const_iterator it_tran = transistors.begin(); it_tran != transistors.end(); it_tran++)
		{
			Partitioning::Transistor* tran = *it_tran;

			tranArea[index] = Gecode::expr(getSpace(), getTransistorToWidthMap().find(*tran)* getTransistorToLengthMap().find(*tran));
			sign[index]= 1;
			index++;
		}

		Gecode::IntVar area = Gecode::expr(getSpace() ,sum(tranArea));

		Gecode::FloatVar helpArea( getSpace(), 1, 10000000);
		channel(getSpace(), area, helpArea);

		if(getSpace().getDefinition() == "1mum")
		{
			Gecode::FloatVar normedArea = Gecode::expr(getSpace(), helpArea);
			getSpace().setArea(normedArea);
			return normedArea;

		}
		else //Definition 0.1 mum
		{
			Gecode::FloatVar normedArea = Gecode::expr(getSpace(), 0.01 * helpArea);
			getSpace().setArea(normedArea);
			return normedArea;
		}
	}

	void CircuitSpecificationsConstraints::createAreaConstraint(Gecode::FloatVar area)
	{
		Gecode::rel(getSpace(), area <= getCircuitInformation().getCircuitSpecification().getMaxArea());
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::createGainConstraintFirstStage()
	{
		Gecode::FloatVar Rout(getSpace(), 0, 100000000000000);
		Partitioning::TransconductancePart & firstStage =  getPartitioningResult().getFirstStage();
		std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		Rout = computeOutputResistanceFirstStage();
		dom(getSpace(), Rout, 0, 10000000000000);

		Gecode::FloatVar firstStageTransconductance = computeTransconductance(firstStage);

		if(!getPartitioningResult().hasBiasOfFoldedPair())
		{
			if(getPartitioningResult().hasSecondarySecondStage())
			{
				return createFirstStageGainFirstStageWithDiodeTransistorLoad();
			}
			else
			{
				return Gecode::expr(getSpace(), firstStageTransconductance * Rout );
			}
		}
		else if(getPartitioningResult().hasFirstStageGainEnhancer())
		{
			Gecode::FloatVar errorFactor = computeGainFirstStageErrorFactor();

			dom(getSpace(), errorFactor, 0, 100000000);

			const Partitioning::PositiveFeedbackPart & FPF = getPartitioningResult().getFirstStageGainEnhancer();
			Partitioning::Component & compFPF = **getPartitioningResult().getBelongingComponents(FPF).begin();
			Gecode::FloatVar gmFPF = computeTransconductance(compFPF);
			dom(getSpace(), gmFPF, 0, 10000000000);


			Gecode::FloatVar subtractor = expr(getSpace(), /*errorFactor**/  gmFPF * Rout);
			dom(getSpace(), subtractor, 0, 100000000);
			Gecode::FloatVar k = expr(getSpace(), 1 - subtractor);

			dom(getSpace(), k, -10000000, 1000000000);
			rel(getSpace(), k<1);
			rel(getSpace(), k>0);

			Gecode::FloatVar gain(getSpace(), 1, 100000000000000000000000);


			rel(getSpace(), gain * abs(k) == /*errorFactor**/  firstStageTransconductance * Rout);

			dom(getSpace(), gain, 0, 1000000000000000);

			return gain;
		}
		else
		{
			Gecode::FloatVar k(getSpace(), 0, 10000);
			Gecode::FloatVar gLoad2(getSpace(), 0, 1);
			Gecode::FloatVar gLoad1(getSpace(), 0, 1);

			for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
			{
				Partitioning::LoadPart * loadPart = * it;
				 //Only LoadParts Consisting of 4 transistors are supported
				if(loadPart->hasCascodedPair())
				{
					gLoad1 = computeOutputConductance(*loadPart, firstStage);
					dom(getSpace(), gLoad1, 0, 1);
				 }
				else
				{
					gLoad2 = computeOutputConductance(*loadPart, firstStage);
					dom(getSpace(), gLoad1, 0, 1);
				}

			}
			Gecode::rel(getSpace(), gLoad1  == k *gLoad2);
			return Gecode::expr(getSpace(), ((2+k)/(2+2*k)) *firstStageTransconductance * Rout);

		}
	}


	void CircuitSpecificationsConstraints::createComplementaryFirstStageConstraint()
	{
		Gecode::FloatVal mu = getSpace().getScalingFactorMUM();

		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		if(firstStage.isComplementary())
		{
			//Same K= mu* Cox W/L factor for pmos and nmos transistors
			Partitioning::Component * pmosComp = nullptr;
			Partitioning::Component * nmosComp = nullptr;

			for(auto & comp: getPartitioningResult().getBelongingComponents(firstStage))
			{
				if(comp->getArray().getTechType().isP() && pmosComp == nullptr)
				{
					pmosComp = comp;
				}
				else if(comp->getArray().getTechType().isN() && nmosComp == nullptr)
				{
					nmosComp = comp;
				}
			}

			assert(nmosComp != nullptr && pmosComp != nullptr, " There should be a nmos component and a pmos component in the first stage!");

			if(getTransistorModel()  == "SHM")
			{
				const TechnologieSpecificationSHM & techSpecPmos = getCircuitInformation().getTechnologieSpecificationSHM(*pmosComp);
				Gecode::FloatVal muCoxPmos = techSpecPmos.getMobilityOxideCapacityCoefficient();
				Gecode::FloatVar widthHelperPmos(getSpace(), 1, getSpace().getWidthUpperBound());
				Gecode::FloatVar lengthHelperPmos(getSpace(), 1, getSpace().getLengthUpperBound());
				channel(getSpace(), widthHelperPmos, getTransistorToWidthMap().find(*pmosComp));
				channel(getSpace(), lengthHelperPmos, getTransistorToLengthMap().find(*pmosComp));

				Gecode::FloatVar widthPmos = Gecode::expr(getSpace(),widthHelperPmos*mu);
				Gecode::FloatVar lengthPmos = Gecode::expr(getSpace(), lengthHelperPmos*mu);

				const TechnologieSpecificationSHM & techSpecNmos = getCircuitInformation().getTechnologieSpecificationSHM(*nmosComp);
				Gecode::FloatVal muCoxNmos = techSpecNmos.getMobilityOxideCapacityCoefficient();
				Gecode::FloatVar widthHelperNmos(getSpace(), 1, getSpace().getWidthUpperBound());
				Gecode::FloatVar lengthHelperNmos(getSpace(), 1, getSpace().getLengthUpperBound());
				channel(getSpace(), widthHelperNmos, getTransistorToWidthMap().find(*nmosComp));
				channel(getSpace(), lengthHelperNmos, getTransistorToLengthMap().find(*nmosComp));

				Gecode::FloatVar widthNmos = Gecode::expr(getSpace(),widthHelperNmos*mu);
				Gecode::FloatVar lengthNmos = Gecode::expr(getSpace(), lengthHelperNmos*mu);

//				rel(getSpace(), 2.1 * (widthNmos/lengthNmos) >=   (widthPmos/lengthPmos) );
//				rel(getSpace(), 1.9 *(widthNmos/lengthNmos) <=(widthPmos/lengthPmos) );
				rel(getSpace(), muCoxNmos * (widthNmos/lengthNmos) >= 0.99 * muCoxPmos * (widthPmos/lengthPmos) );
				rel(getSpace(), muCoxNmos * (widthNmos/lengthNmos) <= 1.01 * muCoxPmos * (widthPmos/lengthPmos) );
			}


			//Nearly Equal bias current

			Partitioning::BiasPart & biasPart1 =  **firstStage.getBiasPart().begin();
			Partitioning::BiasPart & biasPart2 =  **std::next(firstStage.getBiasPart().begin());

			Partitioning::Component & biasComp1 = **getPartitioningResult().getBelongingComponents(biasPart1).begin();
			Partitioning::Component & biasComp2 = **getPartitioningResult().getBelongingComponents(biasPart2).begin();

			Gecode::FloatVar biasCurrent1 = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp1));
			Gecode::FloatVar biasCurrent2 = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(biasComp2));

			rel(getSpace(), abs(biasCurrent1) >= 0.99 * abs(biasCurrent2));
			rel(getSpace(), abs(biasCurrent1) <= 1.01 * abs(biasCurrent2));
		}
	}


	void CircuitSpecificationsConstraints::createScalibiltyFactorLimitationForSymmetricalOTAConstraint()
	{
		if(getPartitioningResult().hasSecondarySecondStage())
		{
			Gecode::FloatVar B = computeScalingFactorForSymmetricalOTA();
			dom(getSpace(), B, 0, 20);
			rel(getSpace(), B >=0.9);
			rel(getSpace(), B <=3.1);

		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::computeScalingFactorForSymmetricalOTA()
	{
		Partitioning::TransconductancePart firstStage = getPartitioningResult().getFirstStage();
		Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();
		Partitioning::Component * compFL = nullptr;
		 for(auto & loadComp : getPartitioningResult().getBelongingComponents(loadFirstStage))
		 {
			 const StructRec::StructureNet * gateNet = nullptr;
			if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
				&& loadComp->getArray().isPartOfCurrentMirror())
			{
				gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
			}
			else if(loadComp->getArray().isPartOfCurrentMirror())
			{
				gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
			}
			if(gateNet != nullptr)
			{
				for(auto & outputNet : findOutputNetsFirstStage() )
				{
					if(gateNet->getIdentifier() == outputNet->getIdentifier())
					{
						compFL = loadComp;
						break;
					}
				}
			}

			if(compFL != nullptr)
				break;
		 }
		 assert(compFL != nullptr, "There must be one load comp being part of a current mirror connected to the first stage output");

		Gecode::IntVar widthLoadFirstStage = getTransistorToWidthMap().find(*compFL);
		Gecode::IntVar lengthLoadFirstStage = getTransistorToLengthMap().find(*compFL);
		Gecode::FloatVar widthLoadFirstStageHelperVar(getSpace(),1, getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthLoadFirstStageHelperVar(getSpace(),1, getSpace().getLengthUpperBound());
		channel(getSpace(), widthLoadFirstStage, widthLoadFirstStageHelperVar);
		channel(getSpace(), lengthLoadFirstStage, lengthLoadFirstStageHelperVar);

		Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
		Partitioning::Component * compSecondStage =  nullptr;
		for(auto it : getPartitioningResult().getBelongingComponents(secondStage))
		{
			if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
					compSecondStage = it;
			}
		}
		assert(compSecondStage != nullptr, "There should be a second stage component connect to a supply net!");

		Gecode::IntVar widthSecondStage = getTransistorToWidthMap().find(*compSecondStage);
		Gecode::IntVar lengthSecondStage = getTransistorToLengthMap().find(*compSecondStage);
		Gecode::FloatVar widthSecondStageHelperVar(getSpace(),1, getSpace().getWidthUpperBound());
		Gecode::FloatVar lengthSecondStageHelperVar(getSpace(),1, getSpace().getLengthUpperBound());
		channel(getSpace(), widthSecondStage, widthSecondStageHelperVar);
		channel(getSpace(), lengthSecondStage, lengthSecondStageHelperVar);



		Gecode::FloatVar B = Gecode::expr(getSpace(),widthSecondStageHelperVar * lengthLoadFirstStageHelperVar /(lengthSecondStageHelperVar * widthLoadFirstStageHelperVar));
		dom(getSpace(), B, 0, 20);
		return B;
	}



	Gecode::FloatVar CircuitSpecificationsConstraints::createGainConstraintSecondStage()
	{
		Gecode::FloatVar gainSecondStage(getSpace(), 0, 100000000);

		if(getPartitioningResult().hasSecondStage())
		{
			gainSecondStage = createGainConstraintHigherStage(getPartitioningResult().getPrimarySecondStage());
		}
		else
		{
			Gecode::rel(getSpace(), gainSecondStage == 1);
		}
		return gainSecondStage;
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::createGainConstraintThirdStage()
	{
		Gecode::FloatVar gainThirdStage(getSpace(), 0, 100000000);

		if(getPartitioningResult().hasThirdStage())
		{
			gainThirdStage = createGainConstraintHigherStage(getPartitioningResult().getThirdStage());
		}
		else
		{
			Gecode::rel(getSpace(), gainThirdStage == 1);
		}
		return gainThirdStage;
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::createGainConstraintHigherStage(Partitioning::TransconductancePart & higherStage)
	{
		assert(higherStage.isPrimarySecondStage() || higherStage.isSecondarySecondStage() || higherStage.isThirdStage(),
				"Stage needs top be either second or third stage");

		Partitioning::Component * supplyComp = nullptr;

		for(auto & comp : getPartitioningResult().getBelongingComponents(higherStage))
		{
			if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
			{
				supplyComp = comp;
			}
		}

		assert(supplyComp != nullptr, "Every higher stage should have a to supply connected component");
		Gecode::FloatVar gmTrans = computeTransconductance(*supplyComp);


		Gecode::FloatVar Rout = computeOutputResistanceHigherStage(higherStage);
		dom(getSpace(), Rout, 0, 10000000000000000000);
		return  Gecode::expr(getSpace(), abs(gmTrans *Rout));


	}

	Gecode::FloatVar CircuitSpecificationsConstraints::computeGainFirstStageErrorFactor()
	{
		Gecode::FloatVar errorFactor(getSpace(), 0, 100000000000000);
		if(getPartitioningResult().hasBiasOfFoldedPair())
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			Gecode::FloatVar gdFT = computeOutputConductance(compFT);

			const StructRec::Structure & cascodePair = firstStage.getHelperStructure();
			Partitioning::Component & compFL1C = **getPartitioningResult().findComponents(cascodePair).begin();
			Partitioning::Component & compFL1B = getPartitioningResult().getBiasOfFoldedPair();
			Gecode::FloatVar gmFL1C = computeTransconductance(compFL1C);
			Gecode::FloatVar gdFL1C = computeOutputConductance(compFL1C);
			Gecode::FloatVar gdFL1B = computeOutputConductance(compFL1B);

			if(getPartitioningResult().hasFirstStageGainEnhancer())
			{
				const Partitioning::PositiveFeedbackPart & FPF = getPartitioningResult().getFirstStageGainEnhancer();
				Partitioning::Component & compFPF = **getPartitioningResult().getBelongingComponents(FPF).begin();
				Gecode::FloatVar gmFPF = computeTransconductance(compFPF);
				Gecode::FloatVar gdFPF = computeOutputConductance(compFPF);

				rel(getSpace(), gmFPF *10 <= gmFL1C);

				rel(getSpace(), errorFactor * (gdFT + gdFL1B + gmFL1C + gdFL1C + gdFPF + gmFPF) ==  (gmFL1C + gdFL1C));
			}
			else
			{
				rel(getSpace(), errorFactor * (gdFT + gdFL1B + gmFL1C + gdFL1C) ==  (gmFL1C + gdFL1C));
			}
		}
		else
		{
			rel(getSpace(), errorFactor == 1);
		}
		return errorFactor;
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::createFirstStageGainFirstStageWithDiodeTransistorLoad()
	{

		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		Partitioning::Component * compFirstStage = *getPartitioningResult().getBelongingComponents(firstStage).begin();
		Gecode::FloatVar gmFirstStage = computeTransconductance(*compFirstStage);

		Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();

		if(loadFirstStage.hasCrossCoupledPair(getPartitioningResult()))
		{
			Partitioning::Component * diodeComp = nullptr;
			Partitioning::Component * normalComp = nullptr;
			for(auto & comp: getPartitioningResult().getBelongingComponents(loadFirstStage))
			{
				if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
				{
					normalComp = comp;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					diodeComp = comp;
				}
			}
			assert(diodeComp != nullptr && normalComp != nullptr, "There should be a diode connected component and a normal component in the load of the first stage" );

			Gecode::FloatVar gmFL = computeTransconductance(*diodeComp);
			Gecode::FloatVar gmFLPF = computeTransconductance(*normalComp);

			rel(getSpace(), gmFLPF >= 0.75 * gmFL);
			rel(getSpace(), gmFLPF <= 0.8 * gmFL);

			return Gecode::expr(getSpace(), gmFirstStage/(gmFL -gmFLPF));

		}
		else
		{
			Partitioning::Component * compFL = nullptr;
			 for(auto & loadComp : getPartitioningResult().getBelongingComponents(loadFirstStage))
			 {
				 const StructRec::StructureNet * gateNet = nullptr;
				if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
					&& loadComp->getArray().isPartOfCurrentMirror())
				{
					gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
				}
				else if(loadComp->getArray().isPartOfCurrentMirror())
				{
					gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				}
				if(gateNet != nullptr)
				{
					for(auto & outputNet : findOutputNetsFirstStage() )
					{
						if(gateNet->getIdentifier() == outputNet->getIdentifier())
						{
							compFL = loadComp;
							break;
						}
					}
				}

				if(compFL != nullptr)
					break;
			 }
			 assert(compFL != nullptr, "There must be one load comp being part of a current mirror connected to the first stage output");
			Gecode::FloatVar gmLoadFirstStage = computeTransconductance(*compFL);
			return Gecode::expr(getSpace(), gmFirstStage/gmLoadFirstStage);
		}

	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePhaseMargin()
	{

		//calculation of the exact phaseMargin
		Gecode::FloatVar phaseMargin(getSpace(), -4,4);

		Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(getPartitioningResult().getFirstStage()).begin();
		Gecode::FloatVar gmFT = computeTransconductance(compFT);

		Gecode::FloatVar transientFrequency = getSpace().getTransitFrequencyVariable();


		Gecode::dom(getSpace(), transientFrequency, 0, 1000000000000 );


		Gecode::FloatVar dominantPole = getPolesAndZeros().getDominantPole();
		Gecode::dom(getSpace(), dominantPole, 0, 1000000000000 );

		for(auto  & fndp : getPolesAndZeros().getImportantNonDominantPoles())
		{
			Gecode::dom(getSpace(), fndp, 0, 1000000000000 );
			Gecode::rel(getSpace(), fndp * 0.1 > dominantPole);
		}
		for(auto  & fpz : getPolesAndZeros().getImportantZeros())
		{
			Gecode::dom(getSpace(), fpz, 0, 1000000000000 );
			Gecode::rel(getSpace(), fpz * 0.1 > dominantPole);
		}


		if(getPartitioningResult().hasCompensationCapacitance())
		{

			Gecode::FloatValArgs sign(getPolesAndZeros().getImportantNonDominantPoles().size()+getPolesAndZeros().getImportantZeros().size()+1);
			Gecode::FloatVarArgs arcTanValues(getPolesAndZeros().getImportantNonDominantPoles().size()+getPolesAndZeros().getImportantZeros().size()+1);
			int index = 0;
			for(auto & fndp :getPolesAndZeros().getImportantNonDominantPoles())
			{
				dom(getSpace(), fndp, 0, 10000000000000000);
				Gecode::FloatVar arcTanFndp = Gecode::expr(getSpace(), atan(transientFrequency/fndp));
				dom(getSpace(), arcTanFndp, 0, 2);
				sign[index] = 1;
				arcTanValues[index] = arcTanFndp;
				index++;
			}
			for(auto & fz :getPolesAndZeros().getImportantZeros())
			{
				dom(getSpace(), fz, 0, 10000000000000000);
				Gecode::FloatVar arcTanFz = Gecode::expr(getSpace(), atan(transientFrequency/fz));
				dom(getSpace(), arcTanFz, 0, 2);
				sign[index] = -1;
				arcTanValues[index] = arcTanFz;
				index++;
			}

			Gecode::FloatVar fpz = getPolesAndZeros().getPositiveZero();
			dom(getSpace(), fpz, 0, 10000000000000000);
			Gecode::FloatVar arcTanFpz = Gecode::expr(getSpace(), atan(transientFrequency/fpz));
			dom(getSpace(), arcTanFpz, 0, 2);
			sign[index] = 1;
			arcTanValues[index] = arcTanFpz;
			index++;

			Gecode::FloatVar piHalfMinusPhaseMargin = expr(getSpace(), 3.1415926536 *0.5 - phaseMargin);
			Gecode::linear(getSpace(), sign, arcTanValues, Gecode::FRT_EQ, piHalfMinusPhaseMargin);

		}
		else
		{
			Gecode::FloatValArgs sign(getPolesAndZeros().getImportantNonDominantPoles().size()+getPolesAndZeros().getImportantZeros().size());
			Gecode::FloatVarArgs arcTanValues(getPolesAndZeros().getImportantNonDominantPoles().size()+getPolesAndZeros().getImportantZeros().size());
			int index = 0;
			for(auto & fndp :getPolesAndZeros().getImportantNonDominantPoles())
			{
				dom(getSpace(), fndp, 0, 10000000000000000);
				Gecode::FloatVar arcTanFndp = Gecode::expr(getSpace(), atan(transientFrequency/fndp));
				dom(getSpace(), arcTanFndp, 0, 2);
				sign[index] = 1;
				arcTanValues[index] = arcTanFndp;
				index++;
			}
			for(auto & fpz :getPolesAndZeros().getImportantZeros())
			{
				dom(getSpace(), fpz, 0, 10000000000000000);
				Gecode::FloatVar arcTanFpz = Gecode::expr(getSpace(), atan(transientFrequency/fpz));
				dom(getSpace(), arcTanFpz, 0, 2);
				sign[index] = -1;
				arcTanValues[index] = arcTanFpz;
				index++;
			}
			Gecode::FloatVar piHalfMinusPhaseMargin = expr(getSpace(), 3.1415926536 *0.5 - phaseMargin);
			Gecode::linear(getSpace(), sign, arcTanValues, Gecode::FRT_EQ, piHalfMinusPhaseMargin);
		}

		getSpace().setPhaseMargin(phaseMargin);
		return phaseMargin;
	}

	void CircuitSpecificationsConstraints::createPhaseMarginConstraint(Gecode::FloatVar phaseMargin)
	{
		rel(getSpace(), phaseMargin >= getCircuitInformation().getCircuitSpecification().getPhaseMargin());
	}

	void CircuitSpecificationsConstraints::createInnerVoltageBiasOfTelescopicOpAmpsConstraint()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		for(auto & loadPart : firstStage.getLoadPart())
		{
			if(loadPart->hasCascodedPair() && loadPart->hasBiasPart())
			{
				Partitioning::Part & biasPart = **loadPart->getBiasParts().begin();
				Partitioning::Component & biasComp = **getPartitioningResult().getBelongingComponents(biasPart).begin();

				if(loadPart->getBiasParts().size() == 1 && getPartitioningResult().getBelongingComponents(biasPart).size() == 1
						&& biasComp.getArray().getStructureName().toStr() == "MosfetDiodeArray" )
				{
					//Source net of bias Comp and Diff pair of first stage must be equals

					Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();

					if(compFT.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).getIdentifier()
							== biasComp.getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Source")).getIdentifier())
					{
						Partitioning::Component & cascodePairComp = **getPartitioningResult().findComponents(loadPart->getCascodedPair()).begin();

						Edge * gateSourceEdgeCascodePairComp = NULL;
						for(auto & edgeCascodePairComp: getGraph().findToComponentBelongingEdges(cascodePairComp))
						{
							if(edgeCascodePairComp->isGateSourceEdge())
							{
								gateSourceEdgeCascodePairComp = edgeCascodePairComp;
							}
						}

						Edge * gateSourceEdgeCompFT = NULL;
						for(auto & edgeCompFT: getGraph().findToComponentBelongingEdges(compFT))
						{
							if(edgeCompFT->isGateSourceEdge())// Is Diode transistor array -> no gateSource Edge exist.
							{
								gateSourceEdgeCompFT = edgeCompFT;
							}
						}

						Edge * gateSourceEdgeBiasComp = NULL;
						for(auto & edgeBiasComp: getGraph().findToComponentBelongingEdges(biasComp))
						{
							if(edgeBiasComp->isDrainSourceEdge())// Is Diode transistor array -> no gateSource Edge exist.
							{
								gateSourceEdgeBiasComp = edgeBiasComp;
							}
						}

						float vthFT = getCircuitInformation().getTechnologieSpecificationSHM(compFT).getThresholdVoltage();
						Gecode::FloatVar vgsCompFT = computeEdgeVoltage(*gateSourceEdgeCompFT);
						Gecode::FloatVar vgsBiasComp = computeEdgeVoltage(*gateSourceEdgeBiasComp);
						Gecode::FloatVar vgsCascodePairComp = computeEdgeVoltage(*gateSourceEdgeCascodePairComp);

						rel(getSpace(), abs(vgsBiasComp) >= 0.95 * abs((vgsCompFT - vthFT) + vgsCascodePairComp));
						rel(getSpace(), abs(vgsBiasComp) <= 1.05* abs((vgsCompFT - vthFT) + vgsCascodePairComp));
					}
				}
			}
		}
	}

	void CircuitSpecificationsConstraints::createOutputNetVoltageConstraintFirstStage()
	{
		if(getPartitioningResult().hasPrimarySecondStage() && getPartitioningResult().hasThirdStage() && !getCircuitInformation().getCircuitParameter().isComplementary())
		{
			Partitioning::Component * comp1 = nullptr;
			Partitioning::Component * comp2 = nullptr;


			std::vector<Partitioning::Component* > comps = getPartitioningResult().getBelongingComponents(getPartitioningResult().getFirstStage());
			for(auto & comp : comps)
			{
				if(comp1 == nullptr)
				{
					comp1 = comp;
				}
				else if(comp2 == nullptr)
				{
					comp2 = comp;
				}
				else
				{
					assert(false, " There should not be more than two component in the first stage");
				}
			}

			assert(comp1 != nullptr && comp2 != nullptr, "There should be exactly two components in the first stage");
			Core::NetId outputNetFirstStage1 = comp1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();
			Core::NetId outputNetFirstStage2 = comp2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();

			Gecode::FloatVar outputNetFirstStage1Voltage = getSpace().createFloatVoltage(netToVoltageMap_->find(outputNetFirstStage1));
			Gecode::FloatVar outputNetFirstStage2Voltage = getSpace().createFloatVoltage(netToVoltageMap_->find(outputNetFirstStage2));
			Gecode::rel(getSpace(), outputNetFirstStage1Voltage >=  outputNetFirstStage2Voltage - 0.5);
			Gecode::rel(getSpace(), outputNetFirstStage1Voltage <=  outputNetFirstStage2Voltage + 0.5);
		}
	}

	void CircuitSpecificationsConstraints::createFeedbackCircuitConstraints()
	{
		for(auto & transPart : getPartitioningResult().getAllTransconductanceParts())
		{
			if(transPart->isFeedBack())
			{
				logDebug("                 Creating gain constraint");
				createFeedbackGainConstraint(*transPart);
				logDebug("                  Creating CMRR constraint");
				createFeedbackCMRRConstraint(*transPart);
				logDebug("                 Creating transit frequency constraint");
				createFeedbackTransitFrequencyConstraint(*transPart);
				logDebug("                 Creating phase margin constraint");
				createFeedbackPhaseMarginConstraint(*transPart);
				logDebug("                 Creating input voltage constraint");
				createFeedbackInputVoltageConstraint(*transPart);
				logDebug("                 Creating power consumption constraint");
				createFeedbackPowerConsumptionConstraint();
				logDebug("                 Creating detector resistor constraint");
				createDetectorResistorConstraints();
			}
		}
	}

	void CircuitSpecificationsConstraints::createFeedbackGainConstraint(Partitioning::TransconductancePart & feedbackStage)
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		Partitioning::Component & firstStageComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();
		Gecode::FloatVar gmFT = computeTransconductance(firstStageComp);

		Gecode::FloatVar RoutFirstStage = computeOutputResistance(firstStage);

		Partitioning::Component & feedbackStageComp = feedbackStage.findComponentNotConnectedToVref(getCircuitInformation().getCircuitParameter(), getPartitioningResult());
		Gecode::FloatVar gmCMFB_T = computeTransconductance(feedbackStageComp);

		Partitioning::LoadPart & loadFeedbackStage = **feedbackStage.getLoadPart().begin();
		Partitioning::Component & loadFeedbackStageComp = **getPartitioningResult().getBelongingComponents(loadFeedbackStage).begin();
		Gecode::FloatVar currentLoadFeedbackStage = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(loadFeedbackStageComp));
		Gecode::FloatVar RoutCMFB = computeOutputResistance(feedbackStage);

		Partitioning::Component * firstStageLoadPartComp = nullptr;

		for(auto & feedbackLoadPart : feedbackStage.getLoadPart())
		{
			for(auto & feedbackLoadComp : getPartitioningResult().getBelongingComponents(*feedbackLoadPart))
			{
				const StructRec::StructureNet & feedbackLoadCompGateNet = feedbackLoadComp->getArray().findNet(StructRec::StructurePinType(feedbackLoadComp->getArray().getStructureName(),"Drain"));
				std::vector<const StructRec::Structure * > gateConnectedStructs = feedbackLoadCompGateNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				for(auto & gateConnectedStruct : gateConnectedStructs)
				{
					Partitioning::Component & gateConnectedComp = getPartitioningResult().findComponent(gateConnectedStruct->getIdentifier());

					if(getPartitioningResult().isLoadPartOfFirstStage(gateConnectedComp.getPart()))
					{
						firstStageLoadPartComp = &gateConnectedComp;
						break;
					}
				}
			}
		}

		assert(firstStageLoadPartComp != nullptr, "There should be at least one first stage load component connected to the load of the feedback circuit");
		Gecode::FloatVar currentLoadFirstStage = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*firstStageLoadPartComp));

		//Structured Analog Design
		Gecode::FloatVar gain = expr(getSpace(), currentLoadFirstStage /currentLoadFeedbackStage * gmCMFB_T  *RoutFirstStage);

//		Internet (Testing might which equation might be better, higher value with the one of the Internet)
//		Gecode::FloatVar gain = expr(getSpace(), gmFT* RoutFirstStage * gmCMFB_T * RoutCMFB);

		dom(getSpace(), gain, 0, 1000000000000000000000000000);
		Gecode::FloatVar logGain(getSpace(), -100, 200);
		Gecode::rel(getSpace(), log(gain) == logGain );
		dom(getSpace(), logGain, -100, 200);

		getSpace().setFeedbackGain(Gecode::expr(getSpace(), 8.685889638 * logGain));

	}

	void CircuitSpecificationsConstraints::createFeedbackCMRRConstraint(Partitioning::TransconductancePart & feedbackStage)
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();

		Partitioning::BiasPart * biasOfFirstStage = *feedbackStage.getBiasPart().begin();
		Partitioning::Component * biasTranFirstStage = *getPartitioningResult().getBelongingComponents(*biasOfFirstStage).begin();
		Gecode::FloatVar  gdFB = computeOutputConductance(*biasTranFirstStage);

		Gecode::FloatVar firstStageGain = createGainConstraintFirstStage();
		dom(getSpace(), firstStageGain, 0, 10000000000000000);

		Gecode::FloatVar logFeedbackGain = getSpace().getFeedbackGainVariable();
		dom(getSpace(), logFeedbackGain, -1000, 1000);

		Gecode::FloatVar unnormedLogFeedbackGain(getSpace(), -10000, 10000);

		rel(getSpace(),8.685889638 *unnormedLogFeedbackGain == logFeedbackGain);
		dom(getSpace(), unnormedLogFeedbackGain, -10000, 10000);
		Gecode::FloatVar feedbackGain(getSpace(), 0, 100000000000000000000);
		rel(getSpace(), log(feedbackGain) == unnormedLogFeedbackGain);
		dom(getSpace(), feedbackGain, 0, 100000000000000000000);

		Partitioning::BiasPart * biasFeedbackStage = *feedbackStage.getBiasPart().begin();
		Partitioning::Component * compCMFB_B = *getPartitioningResult().getBelongingComponents(*biasFeedbackStage).begin();
		Gecode::FloatVar  gdCMFB_B = computeOutputConductance(*compCMFB_B);



		std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		if(loadParts.size() ==1)  //Miller Operational Amplifier
		{
			Partitioning::LoadPart * loadOfFirstStage = *firstStage.getLoadPart().begin();
			Partitioning::Component* loadTran = *getPartitioningResult().getBelongingComponents(*loadOfFirstStage).begin();

			Gecode::FloatVar gmLoad = computeTransconductance(*loadTran);

			Gecode::FloatVar CMRR(getSpace(), 0, 1000000000000);

			Gecode::rel(getSpace(),feedbackGain* firstStageGain * gmLoad/ gdFB== CMRR);

			Gecode::FloatVar logCMRR(getSpace(), 0, 2000);
			Gecode::rel(getSpace(), log(CMRR) == logCMRR );

			getSpace().setFeedbackCMRR(Gecode::expr(getSpace(), 8.685889638 * logCMRR));
		}
		else if(getPartitioningResult().hasBiasOfFoldedPair()) //Folded Cascode OTA
		{
			for(std::vector<Partitioning::LoadPart*>::iterator it = loadParts.begin(); it != loadParts.end(); it++)
			{
				Partitioning::LoadPart * loadPart = * it;
				if(loadPart->hasCascodedPair())
				{
					const StructRec::Structure & helperStruc = loadPart->getCascodedPair();
					Gecode::FloatVar gmHelperStruc = computeTransconductance(**(getPartitioningResult().findComponents(helperStruc)).begin());
					const StructRec::Structure & biasGCC = **loadPart->getCurrentBiasOfFoldedPair().begin();
					Gecode::FloatVar gmBiasGCC = computeTransconductance(getPartitioningResult().findComponent(biasGCC.getIdentifier()));
					dom(getSpace(), gmHelperStruc, 0, 0.1);

					Gecode::FloatVar CMRR(getSpace(), 0, 1000000000000000000000);

					if(getPartitioningResult().hasFirstStageGainEnhancer())
					{
						Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();
						Gecode::FloatVar gmFT = computeTransconductance(compFT);
						dom(getSpace(), gmFT, 0, 1000);
						Gecode::FloatVar fraction = expr(getSpace(), gmFT/gdFB);
						dom(getSpace(), fraction, 0, 10000000);
						Gecode::FloatVar firstStageInfluenceFactor = expr(getSpace(), 1+ 2 *fraction);
						dom(getSpace(), firstStageInfluenceFactor, 0, 100000000000);

						Gecode::FloatVar errorFactorFirstStageGain = computeGainFirstStageErrorFactor();
						dom(getSpace(), errorFactorFirstStageGain, 0, 1000000);
						Gecode::FloatVar Rout = computeOutputResistance(firstStage);

						const Partitioning::PositiveFeedbackPart & FPF = getPartitioningResult().getFirstStageGainEnhancer();
						Partitioning::Component & compFPF = **getPartitioningResult().getBelongingComponents(FPF).begin();
						Gecode::FloatVar gmFPF = computeTransconductance(compFPF);


						Gecode::FloatVar subtractor = expr(getSpace(), errorFactorFirstStageGain * gmFPF * Rout);
						dom(getSpace(), subtractor, 0, 10);
						Gecode::FloatVar CMFBInfluenceFactor = expr(getSpace(), 1 + feedbackGain - subtractor);

						dom(getSpace(), CMFBInfluenceFactor, 0, 10000000000);

						rel(getSpace(), CMRR == CMFBInfluenceFactor * firstStageInfluenceFactor);

						rel(getSpace(), CMRR >= pow(10, getCircuitInformation().getCircuitSpecification().getCMRR()/20));
					}
					else
					{
						Gecode::rel(getSpace(), 2* feedbackGain* gmBiasGCC / gdCMFB_B == CMRR);
					}
					Gecode::FloatVar logCMRR(getSpace(), 0, 2000);
					Gecode::rel(getSpace(), log(CMRR) == logCMRR );
					getSpace().setFeedbackCMRR(Gecode::expr(getSpace(), 8.685889638 * logCMRR));

				}
			}
		}
		else
		{
			for(std::vector<Partitioning::LoadPart*>::iterator it = loadParts.begin(); it != loadParts.end(); it++)
			{
				Partitioning::LoadPart * loadPart = * it;
				if(!loadPart->hasCascodedPair())
				{
					Partitioning::Component * outputComp = nullptr;
					for(auto & comp: getPartitioningResult().getBelongingComponents(*loadPart) )
					{
						if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")),firstStage))
						{
							outputComp= comp;
							break;
						}
					}
					assert(outputComp != nullptr, "Ever load should have a component connected to the output net of the stage!");
					Gecode::FloatVar gmLoad = computeTransconductance(*outputComp);

					Gecode::FloatVar CMRR(getSpace(), 0, 1000000000000);

					Gecode::rel(getSpace(),feedbackGain* gmLoad * firstStageGain/ gdFB == CMRR);

					Gecode::FloatVar logCMRR(getSpace(), 0, 2000);
					Gecode::rel(getSpace(), log(CMRR) == logCMRR );

					getSpace().setFeedbackCMRR(Gecode::expr(getSpace(), 8.685889638 * logCMRR));
				}
			}
		}
	}

	void CircuitSpecificationsConstraints::createFeedbackTransitFrequencyConstraint(Partitioning::TransconductancePart & feedbackStage)
	{
		//Calculating transit frequency
		Partitioning::Component & compCMFB_T = feedbackStage.findComponentNotConnectedToVref(getCircuitInformation().getCircuitParameter(), getPartitioningResult());
		Gecode::FloatVar gmCMFB_T = computeTransconductance(compCMFB_T);

		Gecode::FloatVar  outputCapFirstStage = calculateNetCapacitance(**findOutputNetsFirstStage().begin());

		Partitioning::LoadPart & loadFeedback = **feedbackStage.getLoadPart().begin();
		Partitioning::Component & compCMFB_L = **getPartitioningResult().getBelongingComponents(loadFeedback).begin();
		Gecode::FloatVar idsCMFB_L = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(compCMFB_L));

		Partitioning::Component * firstStageLoadPartComp = nullptr;
		for(auto & feedbackLoadPart : feedbackStage.getLoadPart())
		{
			for(auto & feedbackLoadComp : getPartitioningResult().getBelongingComponents(*feedbackLoadPart))
			{
				const StructRec::StructureNet & feedbackLoadCompGateNet = feedbackLoadComp->getArray().findNet(StructRec::StructurePinType(feedbackLoadComp->getArray().getStructureName(),"Drain"));
				std::vector<const StructRec::Structure * > gateConnectedStructs = feedbackLoadCompGateNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				for(auto & gateConnectedStruct : gateConnectedStructs)
				{
					Partitioning::Component & gateConnectedComp = getPartitioningResult().findComponent(gateConnectedStruct->getIdentifier());

					if(getPartitioningResult().isLoadPartOfFirstStage(gateConnectedComp.getPart()))
					{
						firstStageLoadPartComp = &gateConnectedComp;
						break;
					}
				}
			}
		}



		assert(firstStageLoadPartComp != nullptr, "There should be at least one first stage load component connected to the load of the feedback circuit");
		Gecode::FloatVar idsFL = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*firstStageLoadPartComp));
		Gecode::FloatVar ftCMFB = expr(getSpace(), idsFL/idsCMFB_L *gmCMFB_T/(4*3.1415926536 * outputCapFirstStage));
		dom(getSpace(), ftCMFB, 0, 1000000000000000000);
		getSpace().setFeedbackTransitFrequency(ftCMFB);
		rel(getSpace(),ftCMFB >getSpace().getTransitFrequencyVariable());


	}

	void CircuitSpecificationsConstraints::createFeedbackPhaseMarginConstraint(Partitioning::TransconductancePart & feedbackStage)
	{
		//calculation of the exact phaseMargin
		Gecode::FloatVar phaseMargin(getSpace(), -4,4);

		Gecode::FloatVar ftCMFB = getSpace().getFeedbackTransitFrequencyVariable();
		dom(getSpace(), ftCMFB, 0, 100000000000);


		//Calculating non dominant pole CMFB circuit
		Gecode::FloatVar fndpCMFB(getSpace(), 0, 1000000000000);
		Partitioning::LoadPart & loadFeedback = **feedbackStage.getLoadPart().begin();
		Partitioning::Component & compCMFB_L = **getPartitioningResult().getBelongingComponents(loadFeedback).begin();
		Gecode::FloatVar gmCMFB_L = computeTransconductance(compCMFB_L);

		Gecode::FloatVar outputCapCMFB = calculateNetCapacitance(**findOutputNets(feedbackStage).begin());

		fndpCMFB = expr(getSpace(), gmCMFB_L/(2*3.1415926536 * outputCapCMFB));
		Gecode::dom(getSpace(), fndpCMFB, 0, 1000000000000 );


		//Non dominant pole FirstStage
		Gecode::FloatVar fndpFirstStage = calculateNonDominantPoleFirstStage();
		Gecode::dom(getSpace(), fndpFirstStage, 0, 1000000000000 );

		//Dominant pole first stage
		Gecode::FloatVar fp = getPolesAndZeros().getDominantPole();
		Gecode::dom(getSpace(), fp, 0, 1000000000000 );



		if(getPartitioningResult().hasCommonModeSignalDetectorParts())
		{
			//Fndp CommonMode Signal detector
			Partitioning::CommonModeSignalDetectorPart detectPart = **getPartitioningResult().getAllCommonModeSignalDetectorParts().begin();
			const StructRec::Structure & mainStructDetectPart = **detectPart.getMainStructures().begin();

			const StructRec::StructureNet & innerNet = mainStructDetectPart.findNet(StructRec::StructurePinType(mainStructDetectPart.getStructureName(), "Inner"));
			const StructRec::StructureNet & innerArrayNet = innerNet.findArrayNet();
			Gecode::FloatVar cDetect = calculateNetCapacitance(innerArrayNet);
			dom(getSpace(), cDetect, 0, 1000);

			Partitioning::Component * res = nullptr;
			for(auto & comp : getPartitioningResult().getBelongingComponents(detectPart))
			{
				if(comp->getArray().getStructureName().toStr() == "ResistorArray")
				{
					res = comp;
					break;
				}
			}
			assert(res != nullptr, "There should be a resistor in a common mode signal detector ");

			Gecode::IntVar resValue = getTwoPortToValueMap().find(*res);
			Gecode::FloatVar resValueHelperVar(getSpace(), 0, 1000000000000000);
			channel(getSpace(), resValue, resValueHelperVar);
			Gecode::FloatVar normedRes = expr(getSpace(), resValueHelperVar * 1000);

			Gecode::FloatVar fndpDetector(getSpace(), 0,1000000000000000);
			rel(getSpace(), fndpDetector * 2 *3.1415926536 * 2* normedRes * cDetect ==1);

			if(getPartitioningResult().hasSecondStage() && detectPart.hasCapacitors())
			{
				// Fndp Second stage
				Gecode::FloatVar fndpSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
				Gecode::dom(getSpace(), fndpSecondStage, 0, 1000000000000 );

				//Fpz feedback
				Partitioning::Component * cap = nullptr;
				for(auto & comp : getPartitioningResult().getBelongingComponents(detectPart))
				{
					if(comp->getArray().getStructureName().toStr() == "CapacitorArray")
					{
						cap = comp;
						break;
					}
				}
				assert(cap != nullptr, "There should be a capacitor in this common mode signal detector ");

				float p = pow(10,-13);
				Gecode::IntVar capValue = getTwoPortToValueMap().find(*cap);
				rel(getSpace(), capValue > 10);
				Gecode::FloatVar capValueHelperVar(getSpace(), 0, 1000000000000000);
				channel(getSpace(), capValue, capValueHelperVar);
				Gecode::FloatVar normedCap = expr(getSpace(), capValueHelperVar * p);

				Gecode::FloatVar fpzCMFB(getSpace(), 0, 10000000000000000);
				rel(getSpace(), fpzCMFB * 2* 3.1415926536 * normedRes * normedCap ==1);
				dom(getSpace(), fpzCMFB, 0, 100000000000000000);


				Gecode::rel(getSpace(), fndpSecondStage * 0.1 > fp);
				Gecode::rel(getSpace(), fndpCMFB * 0.1 > fp);
				Gecode::rel(getSpace(), fndpDetector * 0.1 > fp);

				Gecode::FloatVar arcTanFndpSecondStage = Gecode::expr(getSpace(), atan(ftCMFB/fndpSecondStage));
				Gecode::FloatVar arcTanFndpCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fndpCMFB));
				Gecode::FloatVar arcTanFndpDetector = Gecode::expr(getSpace(), atan(ftCMFB/fndpDetector));
				Gecode::FloatVar arcTanFpzCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fpzCMFB));

				Gecode::dom(getSpace(), arcTanFndpSecondStage, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpCMFB, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpDetector, 0, 2);
				Gecode::dom(getSpace(), arcTanFpzCMFB, 0, 2);

				Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpSecondStage - arcTanFndpCMFB - arcTanFndpDetector + arcTanFpzCMFB  == phaseMargin);
				Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpSecondStage - arcTanFndpCMFB - arcTanFndpDetector + arcTanFpzCMFB >= 1.04719755);

			}
			else if(getPartitioningResult().hasSecondStage())
			{
				//fndp second stage
				Gecode::FloatVar fndpSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
				Gecode::dom(getSpace(), fndpSecondStage, 0, 1000000000000 );

				Gecode::rel(getSpace(), fndpSecondStage * 0.1 > fp);
				Gecode::rel(getSpace(), fndpCMFB * 0.1 > fp);
				Gecode::rel(getSpace(), fndpDetector * 0.1 > fp);

				Gecode::FloatVar arcTanFndpSecondStage = Gecode::expr(getSpace(), atan(ftCMFB/fndpSecondStage));
				Gecode::FloatVar arcTanFndpCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fndpCMFB));
				Gecode::FloatVar arcTanFndpDetector = Gecode::expr(getSpace(), atan(ftCMFB/fndpDetector));

				Gecode::dom(getSpace(), arcTanFndpSecondStage, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpCMFB, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpDetector, 0, 2);

				Gecode::rel(getSpace(), 3.1415926536 *0.5- arcTanFndpSecondStage - arcTanFndpCMFB - arcTanFndpDetector  == phaseMargin);
				Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpSecondStage - arcTanFndpCMFB - arcTanFndpDetector >= 1.04719755);
			}
			else if(detectPart.hasCapacitors())
			{
				//Fpz feedback
				Partitioning::Component * cap = nullptr;
				for(auto & comp : getPartitioningResult().getBelongingComponents(detectPart))
				{
					if(comp->getArray().getStructureName().toStr() == "CapacitorArray")
					{
						cap = comp;
						break;
					}
				}
				assert(cap != nullptr, "There should be a capacitor in this common mode signal detector ");

				float p = pow(10,-13);
				Gecode::IntVar capValue = getTwoPortToValueMap().find(*cap);
				Gecode::FloatVar capValueHelperVar(getSpace(), 0, 1000000000000000);
				channel(getSpace(), capValue, capValueHelperVar);
				Gecode::FloatVar normedCap = expr(getSpace(), capValueHelperVar * p);

				Gecode::FloatVar fpzCMFB(getSpace(), 0, 10000000000000000);
				rel(getSpace(), fpzCMFB * 2* 3.1415926536 * normedRes * normedCap ==1);
				dom(getSpace(), fpzCMFB, 0, 100000000000000000);

				Gecode::rel(getSpace(), fndpFirstStage * 0.1 > fp);
				Gecode::rel(getSpace(), fndpCMFB * 0.1 > fp);
				Gecode::rel(getSpace(), fndpDetector * 0.1 > fp);

				Gecode::FloatVar arcTanFndpFirstStage = Gecode::expr(getSpace(), atan(ftCMFB/fndpFirstStage));
				Gecode::FloatVar arcTanFndpCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fndpCMFB));
				Gecode::FloatVar arcTanFndpDetector = Gecode::expr(getSpace(), atan(ftCMFB/fndpDetector));
				Gecode::FloatVar arcTanFpzCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fpzCMFB));

				Gecode::dom(getSpace(), arcTanFndpFirstStage, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpCMFB, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpDetector, 0, 2);
				Gecode::dom(getSpace(), arcTanFpzCMFB, 0, 2);

				Gecode::rel(getSpace(), 3.1415926536 *0.5- arcTanFndpFirstStage - arcTanFndpCMFB - arcTanFndpDetector + arcTanFpzCMFB  == phaseMargin);
				Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpFirstStage - arcTanFndpCMFB - arcTanFndpDetector + arcTanFpzCMFB >= 1.04719755);

			}
			else
			{
				Gecode::rel(getSpace(), fndpFirstStage * 0.1 > fp);
				Gecode::rel(getSpace(), fndpCMFB * 0.1 > fp);
				Gecode::rel(getSpace(), fndpDetector * 0.1 > fp);

				Gecode::FloatVar arcTanFndpFirstStage = Gecode::expr(getSpace(), atan(ftCMFB/fndpFirstStage));
				Gecode::FloatVar arcTanFndpCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fndpCMFB));
				Gecode::FloatVar arcTanFndpDetector = Gecode::expr(getSpace(), atan(ftCMFB/fndpDetector));

				Gecode::dom(getSpace(), arcTanFndpFirstStage, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpCMFB, 0, 2);
				Gecode::dom(getSpace(), arcTanFndpDetector, 0, 2);


				Gecode::rel(getSpace(), 3.1415926536 *0.5- arcTanFndpFirstStage - arcTanFndpCMFB - arcTanFndpDetector  == phaseMargin);
				Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpFirstStage - arcTanFndpCMFB - arcTanFndpDetector >= 1.04719755);
			}

		}
		else
		{
			Gecode::rel(getSpace(), fndpFirstStage * 0.1 > fp);
			Gecode::rel(getSpace(), fndpCMFB * 0.1 > fp);

			Gecode::FloatVar arcTanFndpFirstStage = Gecode::expr(getSpace(), atan(ftCMFB/fndpFirstStage));
			Gecode::FloatVar arcTanFndpCMFB = Gecode::expr(getSpace(), atan(ftCMFB/fndpCMFB));


			Gecode::dom(getSpace(), arcTanFndpFirstStage, 0, 2);
			Gecode::dom(getSpace(), arcTanFndpCMFB, 0, 2);

			Gecode::rel(getSpace(), 3.1415926536 *0.5- arcTanFndpFirstStage - arcTanFndpCMFB  == phaseMargin);
			Gecode::rel(getSpace(), 3.1415926536 *0.5 - arcTanFndpFirstStage - arcTanFndpCMFB  >= 1.04719755);
		}

		getSpace().setFeedbackPhaseMargin(phaseMargin);
	}

	void CircuitSpecificationsConstraints::createFeedbackInputVoltageConstraint(Partitioning::TransconductancePart & feedbackStage)
	{
		if(!getPartitioningResult().hasCommonModeSignalDetectorParts())
		{
		float u = getSpace().getScalingFactorMUM();
		float voutMin =getCircuitInformation().getCircuitSpecification().getMinOutputVoltage();
		float voutMax = getCircuitInformation().getCircuitSpecification().getMaxOutputVoltage();

		float vss = getCircuitInformation().getCircuitParameter().getGroundVoltage();
		float vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage();


		std::vector<Partitioning::Component * > inputToSupplyRailPathOverBias = computeInputToSupplyRailPathOverBias(feedbackStage);
		std::vector<Partitioning::Component *> inputToSupplyRailPathOverLoad = computeInputToSupplyRailPathOverLoad(feedbackStage);

		Partitioning::Component & compCMFB_T = feedbackStage.findComponentNotConnectedToVref(getCircuitInformation().getCircuitParameter(), getPartitioningResult());
		float vthFeedbackStage;
		if(getTransistorModel() =="SHM")
		{
				const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(compCMFB_T);
				vthFeedbackStage = techSpec.getThresholdVoltage();
		}
		else
		{
				const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(compCMFB_T);
				vthFeedbackStage = techSpec.getThresholdVoltage();
		}

		Gecode::FloatValArgs signInputToSupplyRailPathOverLoad(inputToSupplyRailPathOverLoad.size() -1);
		Gecode::FloatVarArgs currentLengthWidthRatioInputToSupplyRailPathOverLoad(inputToSupplyRailPathOverLoad.size() -1);
		int indexInputToSupplyRailPathOverLoad = 0;
		for(auto & comp : inputToSupplyRailPathOverLoad)
		{
			if(!comp->getPart().isTransconductancePart())
			{

				Gecode::FloatVar drainSourceCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*comp));
				float muCox;

				if(getTransistorModel() =="SHM")
				{
					const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
					muCox = techSpec.getMobilityOxideCapacityCoefficient();
				}
				else
				{
					const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
					muCox = techSpec.getMobilityOxideCapacityCoefficient();
				}

				Gecode::IntVar width = getTransistorToWidthMap().find(*comp);
				Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
				channel(getSpace(), width, widthHelper);
				Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u );

				Gecode::IntVar length = getTransistorToLengthMap().find(*comp);
				Gecode::FloatVar lengthHelper(getSpace(), 1, getSpace().getLengthUpperBound());
				channel(getSpace(), length, lengthHelper);
				Gecode::FloatVar normedLength = Gecode::expr(getSpace(), lengthHelper * u );
				Gecode::FloatVar lengthWidthRatio = Gecode::expr(getSpace(),normedLength* 2/(muCox * normedWidth));
				dom(getSpace(), lengthWidthRatio, 0, 1000000000000000000000000000);

				Gecode::FloatVar helper = expr(getSpace(), sqrt(abs(drainSourceCurrent)* lengthWidthRatio));
				dom(getSpace(), helper, 0, 10000000000000000000000000);
				currentLengthWidthRatioInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] = helper;

				if(comp->getArray().getTechType().isN())
					signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] =1;
				else
					signInputToSupplyRailPathOverLoad[indexInputToSupplyRailPathOverLoad] =-1;
				indexInputToSupplyRailPathOverLoad++;
			}
		}
		Gecode::FloatValArgs signInputToSupplyRailPathOverBias(inputToSupplyRailPathOverBias.size());
		Gecode::FloatVarArgs currentLengthWidthRatioInputToSupplyRailPathOverBias(inputToSupplyRailPathOverBias.size());
		int indexInputToSupplyRailPathOverBias = 0;
		for(auto & comp : inputToSupplyRailPathOverBias)
		{
			Gecode::FloatVar drainSourceCurrent = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(*comp));
			float muCox;

			if(getTransistorModel() =="SHM")
			{
				const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
				muCox = techSpec.getMobilityOxideCapacityCoefficient();
			}
			else
			{
				const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
				muCox = techSpec.getMobilityOxideCapacityCoefficient();
			}

			Gecode::IntVar width = getTransistorToWidthMap().find(*comp);
			Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
			channel(getSpace(), width, widthHelper);
			Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u );

			Gecode::IntVar length = getTransistorToLengthMap().find(*comp);
			Gecode::FloatVar lengthHelper(getSpace(), 1, getSpace().getLengthUpperBound());
			channel(getSpace(), length, lengthHelper);
			Gecode::FloatVar normedLength = Gecode::expr(getSpace(), lengthHelper * u );
			Gecode::FloatVar lengthWidthRatio = Gecode::expr(getSpace(),normedLength* 2/(muCox * normedWidth));
			dom(getSpace(), lengthWidthRatio, 0, 1000000000000000000000000000);

			Gecode::FloatVar helper = expr(getSpace(), sqrt(abs(drainSourceCurrent)* lengthWidthRatio));
			dom(getSpace(), helper, 0, 10000000000000000000000000);
			currentLengthWidthRatioInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] = helper;
			if(comp->getArray().getTechType().isN())
				signInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] =1;
			else
				signInputToSupplyRailPathOverBias[indexInputToSupplyRailPathOverBias] =-1;
			indexInputToSupplyRailPathOverBias++;

		}

		Gecode::FloatVar vinMinValue(getSpace(), vss, vdd);
		Gecode::FloatVar vinMaxValue(getSpace(), vss, vdd);

		float rightSideInequalityLoad;
		Gecode::FloatVar rightSideEqualityLoad(getSpace(), vss, vdd);

		float rightSideInequalityBias;
		Gecode::FloatVar rightSideEqualityBias(getSpace(), vss, vdd);

		if(compCMFB_T.getArray().getTechType().isP())
		{
			float constantInputToSupplyRailPathOverLoad = 0;
			for(auto & comp : inputToSupplyRailPathOverLoad)
			{
				float vth;
				if(getTransistorModel() =="SHM")
				{
					const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
					vth = techSpec.getThresholdVoltage();
				}
				else
				{
					const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
					vth = techSpec.getThresholdVoltage();
				}

				if(comp->getPart().isTransconductancePart())
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad-vth;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad+vth;
				}
				else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")),feedbackStage))
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad+vth;
				}
			}
			rightSideInequalityLoad = voutMin - constantInputToSupplyRailPathOverLoad - vss;
			rightSideEqualityLoad = expr(getSpace(), vinMinValue - constantInputToSupplyRailPathOverLoad - vss);


			rightSideInequalityBias = voutMax - vthFeedbackStage - vdd;
			rightSideEqualityBias = expr(getSpace(), vinMaxValue - vthFeedbackStage - vdd);

			Gecode::linear(getSpace(), signInputToSupplyRailPathOverLoad, currentLengthWidthRatioInputToSupplyRailPathOverLoad, Gecode::FRT_LQ, rightSideInequalityLoad);
			Gecode::linear(getSpace(), signInputToSupplyRailPathOverBias, currentLengthWidthRatioInputToSupplyRailPathOverBias, Gecode::FRT_GQ, rightSideInequalityBias);
		}
		else
		{
			float constantInputToSupplyRailPathOverLoad = 0;
			for(auto & comp : inputToSupplyRailPathOverLoad)
			{
				float vth;
				if(getTransistorModel() =="SHM")
				{
					const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(*comp);
					vth = techSpec.getThresholdVoltage();
				}
				else
				{
					const TechnologieSpecificationEKV & techSpec= getCircuitInformation().getTechnologieSpecificationEKV(*comp);
					vth = techSpec.getThresholdVoltage();
				}

				if(comp->getPart().isTransconductancePart())
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad-vth;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad+vth;
				}
				else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")), feedbackStage))
				{
					constantInputToSupplyRailPathOverLoad = constantInputToSupplyRailPathOverLoad+vth;
				}
			}


			rightSideInequalityBias = voutMin - vss - vthFeedbackStage;
			rightSideEqualityBias = expr(getSpace(), vinMinValue - vss - vthFeedbackStage);
			rightSideInequalityLoad =  voutMax -constantInputToSupplyRailPathOverLoad - vdd;
			rightSideEqualityLoad = expr(getSpace(), vinMaxValue - constantInputToSupplyRailPathOverLoad - vdd);

			Gecode::linear(getSpace(), signInputToSupplyRailPathOverLoad, currentLengthWidthRatioInputToSupplyRailPathOverLoad, Gecode::FRT_GQ, rightSideInequalityLoad);
			Gecode::linear(getSpace(), signInputToSupplyRailPathOverBias, currentLengthWidthRatioInputToSupplyRailPathOverBias, Gecode::FRT_LQ, rightSideInequalityBias);
		}

		assert(rightSideInequalityLoad, "Design not feasible!");

		assert(rightSideInequalityBias, "Design not feasible!");

		Gecode::linear(getSpace(), signInputToSupplyRailPathOverBias, currentLengthWidthRatioInputToSupplyRailPathOverBias, Gecode::FRT_EQ, rightSideEqualityBias);
		Gecode::linear(getSpace(), signInputToSupplyRailPathOverLoad, currentLengthWidthRatioInputToSupplyRailPathOverLoad, Gecode::FRT_EQ, rightSideEqualityLoad);


		getSpace().setFeedbackMinimumInputVoltage(vinMinValue);
		getSpace().setFeedbackMaximumInputVoltage(vinMaxValue);
		}

	}

	void CircuitSpecificationsConstraints::createDetectorResistorConstraints()
	{
		if(getPartitioningResult().hasCommonModeSignalDetectorParts())
		{
			Partitioning::CommonModeSignalDetectorPart & detectPart = **getPartitioningResult().getAllCommonModeSignalDetectorParts().begin();

			Partitioning::Component * res1 = nullptr;
			Partitioning::Component * res2 = nullptr;

			for(auto & comp : getPartitioningResult().getBelongingComponents(detectPart))
			{
				if(comp->getArray().getStructureName().toStr() == "ResistorArray"
						&& res1 == nullptr)
				{
					res1 = comp;
				}
				else if(comp->getArray().getStructureName().toStr() == "ResistorArray")
				{
					res2 = comp;
				}
			}
			assert(res1 != nullptr && res2 != nullptr, " There should be two resistors in the common mode signal detector" );

			Gecode::IntVar res1Value = getTwoPortToValueMap().find(*res1);
			Gecode::FloatVar res1ValueHelperVar(getSpace(), 0, 1000000000000000);
			channel(getSpace(), res1Value, res1ValueHelperVar);
			Gecode::FloatVar normedRes1 = expr(getSpace(), res1ValueHelperVar * 1000);

			Gecode::IntVar res2Value = getTwoPortToValueMap().find(*res2);
			Gecode::FloatVar res2ValueHelperVar(getSpace(), 0, 1000000000000000);
			channel(getSpace(), res2Value, res2ValueHelperVar);
			Gecode::FloatVar normedRes2 = expr(getSpace(), res2ValueHelperVar * 1000);

			Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
			 Gecode::FloatVar rout = computeOutputResistance(secondStage);

			 rel(getSpace(), normedRes1 >= 10* rout);
			 rel(getSpace(), normedRes2 >= 10* rout);
		}
	}

	void CircuitSpecificationsConstraints::createFeedbackPowerConsumptionConstraint()
	{
		const Node & supplyNode = getGraph().getSupplyNode();
		const Node & groundNode = getGraph().getGroundNode();
		float vddValue = getCircuitInformation().getCircuitParameter().getValue(supplyNode.getNetId()) - getCircuitInformation().getCircuitParameter().getValue(groundNode.getNetId());



		const std::set<Edge*> incomingEdges = supplyNode.getIncomingEdges();
		const std::set<Edge*> outgoingEdges = supplyNode.getOutgoingEdges();

		int numberOfCompsInFeedback = 0;
		for(auto & incomingEdge : incomingEdges)
		{
			if(getPartitioningResult().isInFeedback(incomingEdge->getEdgeKey().getComponent()))
			{
				numberOfCompsInFeedback++;
			}
		}
		for(auto & outgoingEdge : outgoingEdges)
		{
			if(getPartitioningResult().isInFeedback(outgoingEdge->getEdgeKey().getComponent()))
			{
				numberOfCompsInFeedback++;
			}
		}
		Gecode::FloatValArgs vddFeedback(numberOfCompsInFeedback);
		Gecode::FloatVarArgs currentFeedback(numberOfCompsInFeedback);
		Gecode::FloatValArgs vddOpAmp;
		Gecode::FloatVarArgs currentOpAmp;
		float biasCurrent = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getValue();
		if(biasCurrent <= 0)
		{
			Gecode::FloatValArgs vddInitial(incomingEdges.size() + outgoingEdges.size() - numberOfCompsInFeedback);
			Gecode::FloatVarArgs currentInitial(incomingEdges.size() + outgoingEdges.size() - numberOfCompsInFeedback);
			vddOpAmp = vddInitial;
			currentOpAmp = currentInitial;
		}
		else
		{
			Gecode::FloatValArgs vddInitial(incomingEdges.size() + outgoingEdges.size() - numberOfCompsInFeedback +1);
			Gecode::FloatVarArgs currentInitial(incomingEdges.size() + outgoingEdges.size() - numberOfCompsInFeedback+1);
			vddOpAmp = vddInitial;
			currentOpAmp = currentInitial;
		}
		int indexFeedback = 0;
		int indexOpAmp = 0;
		for(std::set<Edge*>::const_iterator it_edges = incomingEdges.begin(); it_edges != incomingEdges.end(); it_edges++ )
		{
			Edge * edge = *it_edges;
			if(getPartitioningResult().isInFeedback(edge->getEdgeKey().getComponent()))
			{
				currentFeedback[indexFeedback] = computeEdgeCurrent(*edge);
				vddFeedback[indexFeedback] = -1* vddValue;
				indexFeedback++;
			}
			else
			{
				currentOpAmp[indexOpAmp] = computeEdgeCurrent(*edge);
				vddOpAmp[indexOpAmp] = -1* vddValue;
				indexOpAmp++;
			}

		}

		for(std::set<Edge*>::const_iterator it_edges = outgoingEdges.begin(); it_edges != outgoingEdges.end(); it_edges++ )
		{
			Edge * edge = *it_edges;
			if(getPartitioningResult().isInFeedback(edge->getEdgeKey().getComponent()))
			{
				currentFeedback[indexFeedback] = computeEdgeCurrent(*edge);
				vddFeedback[indexFeedback] = -1* vddValue;
				indexFeedback++;
			}
			else
			{
				currentOpAmp[indexOpAmp] = computeEdgeCurrent(*edge);
				vddOpAmp[indexOpAmp] = -1* vddValue;
				indexOpAmp++;
			}
		}

		if(biasCurrent > 0)
		{
			vddOpAmp[indexOpAmp] = vddValue;
			Gecode::FloatVal biasCurrentGecode = biasCurrent;

			currentOpAmp[indexOpAmp] = Gecode::expr(getSpace(), biasCurrentGecode);
		}
		Gecode::FloatVar powerOpAmp(getSpace(), 0, 1000000000000000000000000);
		Gecode::FloatVar powerFeedback(getSpace(), 0, 1000000000000000000000000);
		Gecode::linear(getSpace(), vddOpAmp, currentOpAmp, Gecode::FRT_EQ, powerOpAmp);

		Gecode::linear(getSpace(), vddFeedback, currentFeedback, Gecode::FRT_EQ, powerFeedback);

		rel(getSpace(), powerFeedback < powerOpAmp);
		getSpace().setPowerConsumptionFeedback(powerFeedback);
		getSpace().setPowerConsumptionOpAmp(powerOpAmp);
	}

	void CircuitSpecificationsConstraints::createPolesAndZeros()
	{
		logDebug("Get poles ande zeros");
		PolesAndZeros & polesAndZeros = getSpace().getPolesAndZeros();
		logDebug("Initialize poles ande zeros");
		polesAndZeros.initialize(getSpace());
		logDebug("Set poles ande zeros");
		polesAndZeros_ = &polesAndZeros;
		logDebug("Dominant pole");
		Gecode::FloatVar dominantPole = calculateDominantPole();

		polesAndZeros.setDominantePole(dominantPole);

		if(getPartitioningResult().getFirstStage().isComplementary())
		{
			logDebug("Non dominant pole complementary first stage");
			calculateNonDominantPolesComplementaryFirstStage(polesAndZeros);
		}
		else if(getPartitioningResult().getFirstStage().getLoadPart().size() == 2 ||
			(getPartitioningResult().getBelongingComponents(**getPartitioningResult().getFirstStage().getLoadPart().begin()).size() >1	&&
					(!getPartitioningResult().hasSecondStage() || getPartitioningResult().hasSecondarySecondStage())))
		{
			logDebug("Non dominant first stage");
			Gecode::FloatVar nonDominantPoleFirstStage = calculateNonDominantPoleFirstStage();
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleFirstStage);
		}
		if(getPartitioningResult().hasCompensationCapacitance())
		{
			logDebug("positive zero compensation capacity");
			Gecode::FloatVar positiveZero = calculatePositiveZeroWithCompensationCapacity();
			polesAndZeros.setPositiveZero(positiveZero);
		}
		if(!getPartitioningResult().hasSecondStage() && getPartitioningResult().getFirstStage().getLoadPart().size() == 1)
		{
			logDebug("Calculate positive zero first stage");
			Gecode::FloatVar positiveZero = calculatePositiveZeroFirstStage();
			polesAndZeros.addImportantZero(positiveZero);

		}
		if(getPartitioningResult().getFirstStage().getLoadPart().size() == 2)
		{
			logDebug("Calculate unimportant second load poles and zeros");
			calculateFirstStageSecondLoadPolesAndZeros(polesAndZeros);
		}
		if(getPartitioningResult().hasSecondStage())
		{
			logDebug("Calculate non dominat pole second stage");
			Gecode::FloatVar nonDominantPoleSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleSecondStage);
			if(getPartitioningResult().hasCompensationCapacitance()
					&& getPartitioningResult().getBelongingComponents(getPartitioningResult().getPrimarySecondStage()).size() == 2)
			{
				logDebug("Calculate additional non dominant pole second stage");
				Gecode::FloatVar additionalNonDominantPoleSecondStage = calculateAdditionalNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
				polesAndZeros.addImportantNonDominantPole(additionalNonDominantPoleSecondStage);
			}
			logDebug("Calculate higher stages poles and zeros");
			createBiasHigherStagesNonDominantPolesAndZeros(getPartitioningResult().getPrimarySecondStage(),polesAndZeros);

			if(getPartitioningResult().hasSecondarySecondStage())
			{
				if(getPartitioningResult().getBelongingComponents(getPartitioningResult().getPrimarySecondStage()).size() == 1)
				{
					logDebug("Create secondary second stage positive zero");
					Gecode::FloatVar positiveZeroSecondStage = calculatePositiveZeroSecondStage();
					polesAndZeros.addImportantZero(positiveZeroSecondStage);
				}
			}

		}
		if(getPartitioningResult().hasThirdStage())
		{
			Gecode::FloatVar nonDominantPoleThirdStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getThirdStage(), getPartitioningResult().getPrimarySecondStage());
			logDebug("Create third stage non dominat pole");
			polesAndZeros.addImportantNonDominantPole(nonDominantPoleThirdStage);
			logDebug("Create third stage bias unimportant poles and zeros");
			createBiasHigherStagesNonDominantPolesAndZeros(getPartitioningResult().getThirdStage(), polesAndZeros);


		}

		getSpace().initializeIntegerPolesAndZeros();
	}


	Gecode::FloatVar CircuitSpecificationsConstraints::calculateDominantPole()
	{
		Gecode::FloatVar dominantPole(getSpace(), 0, 1000000000000000000000);
		float p =  0.0000000000001;

		Core::NetId outputNet = getCircuitInformation().getCircuitParameter().findOutputPinWithBiggestLoadCapacity(getPartitioningResult()).getNetId();
		Gecode::FloatVar Cout = calculateNetCapacitance(getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(outputNet));
		dom(getSpace(), Cout , 0, 100000);

		if(getPartitioningResult().hasCompensationCapacitance() )
		{
			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
			Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
			channel(getSpace(), valueHelperVarCompCap, compCapValue);
			Gecode::FloatVar normedCompCapValue = Gecode::expr(getSpace(), valueHelperVarCompCap * p);

			if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(),getPartitioningResult().getPrimarySecondStage() ))
			{
				Gecode::FloatVar gainSecondStage = createGainConstraintSecondStage();
				dom(getSpace(), gainSecondStage, 0, 100000000000000000000);
				Gecode::FloatVar Rout1 = computeOutputResistanceFirstStage();
				dom(getSpace(), Rout1, 0, 100000000000000000000);

				Gecode::rel(getSpace(), dominantPole * (2* 3.14159265 * Rout1 * gainSecondStage * normedCompCapValue) == 1);

			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getPrimarySecondStage(),getPartitioningResult().getThirdStage() ))
			{
				Gecode::FloatVar gainThirdStage = createGainConstraintThirdStage();
				dom(getSpace(), gainThirdStage, 0, 100000000000000000000);
				Gecode::FloatVar Rout2 = computeOutputResistanceHigherStage(getPartitioningResult().getPrimarySecondStage());
				dom(getSpace(), Rout2, 0, 100000000000000000000);

				Gecode::rel(getSpace(), dominantPole * (2* 3.14159265 * Rout2 * gainThirdStage * normedCompCapValue) == 1);
			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage()))
			{
				Gecode::FloatVar gainThirdStage = createGainConstraintThirdStage();
				dom(getSpace(), gainThirdStage, 0, 100000000000000000000);
				Gecode::FloatVar gainSecondStage = createGainConstraintSecondStage();
				dom(getSpace(), gainSecondStage, 0, 100000000000000000000);
				Gecode::FloatVar Rout1 = computeOutputResistanceFirstStage();
				dom(getSpace(), Rout1, 0, 100000000000000000000);
				Gecode::FloatVar Rout3 = computeOutputResistanceHigherStage(getPartitioningResult().getThirdStage());
				dom(getSpace(), Rout3, 0, 100000000000000000000);

				Gecode::FloatVar summandWithCc = expr(getSpace(), Rout1 * gainThirdStage *gainSecondStage * normedCompCapValue );
				Gecode::FloatVar summandWithCout = expr(getSpace(), Rout3 * Cout);

				dom(getSpace(), summandWithCc, 0, 1000000000000000);
				dom(getSpace(), summandWithCout, 0, 10000000000000000000);

				Gecode::rel(getSpace(), dominantPole * (2* 3.14159265 * (summandWithCc+ summandWithCout)) == 1);

			}
		}
		else if(getPartitioningResult().hasSecondStage())
		{
			Gecode::FloatVar Rout2 = computeOutputResistanceHigherStage(getPartitioningResult().getPrimarySecondStage());
			dom(getSpace(), Rout2, 0, 100000000000000000000);
			Gecode::rel(getSpace(), dominantPole * (2* 3.14159265 * Rout2  *Cout) == 1);

		}
		else
		{
			Gecode::FloatVar Rout1 = computeOutputResistanceFirstStage();
			dom(getSpace(), Rout1, 0, 100000000000000000000);
			Gecode::rel(getSpace(), dominantPole * (2* 3.14159265 * Rout1  *Cout) == 1);

		}
		return dominantPole;
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePositiveZeroWithCompensationCapacity()
	{
		float p =  0.0000000000001;
		Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();

		Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();

		Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
		Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
		channel(getSpace(), valueHelperVarCompCap, compCapValue);
		Gecode::FloatVar normedCapValue = Gecode::expr(getSpace(), valueHelperVarCompCap * p);

		if(getPartitioningResult().hasCompensationResistor())
		{
			Gecode::FloatVar fpz = calculatePositiveZeroWithCompensationResistance();
			dom(getSpace(), fpz, 1, 10000000000000000);
			return fpz;
		}
		else if(getPartitioningResult().hasThirdStage() && compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage())
				&& !hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			assert(firstStage.hasHelperStructure(), "If compensation capacity is not connected to  first stage output the first stage should have a golded pair ");

			Partitioning::Component & gcc = **getPartitioningResult().findComponents(firstStage.getHelperStructure()).begin();
			Gecode::FloatVar gmGcc = computeTransconductance(gcc);

			return Gecode::expr(getSpace(), 2* gmGcc/(2* 3.14159265 * normedCapValue));
		}
		else
		{
			Partitioning::TransconductancePart * stage = nullptr;

			if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getPrimarySecondStage()))
			{
				stage = &getPartitioningResult().getPrimarySecondStage();
			}
			else if(compensationCapacityConnectedBetween(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getThirdStage()))
			{
				stage = &getPartitioningResult().getThirdStage();
			}
			Partitioning::Component * transComp = *getPartitioningResult().getBelongingComponents(*stage).begin();
			Gecode::FloatVar gmTrans = computeTransconductance(*transComp);

			return Gecode::expr(getSpace(), gmTrans/(2* 3.14159265 * normedCapValue));

		}
	}

	Gecode::FloatVar  CircuitSpecificationsConstraints::calculateNonDominantPoleFirstStage()
	{
		Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();
		std::vector<Partitioning::Component*> firstStageComps = getPartitioningResult().getBelongingComponents(firstStage);
		const StructRec::StructureNet *drainNet = nullptr;
		for(auto & it : firstStageComps)
		{
			if(!isOutputNetFirstStage(it->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier()))
			{
				 drainNet = &it->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				break;
			}
		}
		if(drainNet == nullptr)
		{
			drainNet = &(**firstStageComps.begin()).getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		}
		std::vector<Partitioning::LoadPart*> loadParts = firstStage.getLoadPart();
		Partitioning::Component * compLoadPart = nullptr;
		if(loadParts.size() == 1)
		{
			for(auto & it : getPartitioningResult().getBelongingComponents((**loadParts.begin())))
			{
				if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet)
				{
					compLoadPart = it;
					break;
				}
			}
			assert(compLoadPart != nullptr, "Should have a connection of load and first stage transconductance via drain net!");
		}
		else if(firstStage.hasHelperStructure())
		{
			for(auto & it_loads :loadParts)
			{
				if(it_loads->hasCascodedPair())
				{
					const StructRec::Structure & cascodedPair =it_loads->getCascodedPair();
					compLoadPart = *getPartitioningResult().findComponents(cascodedPair).begin();
					break;
				}
			}
			assert(compLoadPart != nullptr, "One load part should have cascoded pair!");
		}
		else
		{
			for(auto & it_loads :loadParts)
			{
				for(auto & it : getPartitioningResult().getBelongingComponents(*it_loads))
				{
					if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet
							&& it->getArray().getStructureName().toStr() == "MosfetDiodeArray")
					{
						compLoadPart = it;
						break;
					}
				}
				if(compLoadPart != nullptr)
				{
					break;
				}
			}
			if(compLoadPart == nullptr)
			{
				for(auto & it_loads :loadParts)
				{
					for(auto & it : getPartitioningResult().getBelongingComponents(*it_loads))
					{
						if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Drain"))== *drainNet)
						{
							compLoadPart = it;
							break;
						}
					}
					if(compLoadPart != nullptr)
					{
						break;
					}
				}
			}
		}
		//When input is a three stage op amp with compensation capacity between first and third stage and first stage is a folded variant and compensation capacity connected between doffPair and GCC
		if(getPartitioningResult().hasThirdStage() && getPartitioningResult().hasCompensationCapacitance()
				&& compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage()) &&
				!hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin()))
		{
			Gecode::FloatVar rout1 = computeOutputResistance(getPartitioningResult().getFirstStage());
			dom(getSpace(), rout1, 0, 10000000000000);
			Gecode::FloatVar cout1 = calculateNetCapacitance(**findOutputNets(getPartitioningResult().getFirstStage()).begin());
			dom(getSpace(), cout1, 0, 100);
			Gecode::FloatVar rout2 = computeOutputResistance(getPartitioningResult().getPrimarySecondStage());
			dom(getSpace(), rout2, 0, 10000000000000);
			Gecode::FloatVar cout2 = calculateNetCapacitance(**findOutputNets(getPartitioningResult().getPrimarySecondStage()).begin());
			dom(getSpace(), cout2, 0, 100);

			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
			Gecode::FloatVar compCapValueHelperVar(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
			Gecode::FloatVal p = 0.0000000000001;
			channel(getSpace(), compCapValueHelperVar, compCapValue);
			Gecode::FloatVar normedCompCapValue = Gecode::expr(getSpace() ,compCapValueHelperVar*p);

			Gecode::FloatVar gmLoad = computeTransconductance(*compLoadPart);
			dom(getSpace(), gmLoad, 0, 1000);

			Gecode::FloatVar rc1 = expr(getSpace(), rout1 * cout1);
			dom( getSpace(), rc1, 0, 100000000000);

			Gecode::FloatVar rc2 = expr(getSpace(), rout2 * cout2);
			dom( getSpace(), rc2, 0, 100000000000);

			Gecode::FloatVar summandWithCc = expr(getSpace(), (rc1 + rc2 ) * normedCompCapValue);
			dom(getSpace(), summandWithCc, 0, 100000000000000);

			Gecode::FloatVar summandWithGcc = expr(getSpace(), gmLoad * rout1 * rout2 * cout1 * cout2);
			dom(getSpace(), summandWithGcc, 0, 100000000000000);

			Gecode::FloatVar denominator = expr(getSpace(),rout1 * rout2 * cout1 * cout2 * normedCompCapValue );
			dom(getSpace(), denominator, 0, 100000000000000);

			return Gecode::expr(getSpace(), (summandWithCc + summandWithGcc)/ (denominator));
		}
		else if(getPartitioningResult().hasSecondarySecondStage()
				&& (*loadParts.begin())->hasCrossCoupledPair(getPartitioningResult()))
		{
			Partitioning::LoadPart & loadFirstStage = **firstStage.getLoadPart().begin();

			Partitioning::Component * diodeComp = nullptr;
			Partitioning::Component * normalComp = nullptr;
			for(auto & comp: getPartitioningResult().getBelongingComponents(loadFirstStage))
			{
				if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
				{
					normalComp = comp;
				}
				else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					diodeComp = comp;
				}
			}
			assert(diodeComp != nullptr && normalComp != nullptr, "There should be a diode connected component and a normal component in the load of the first stage" );

			Gecode::FloatVar gmFL = computeTransconductance(*diodeComp);
			Gecode::FloatVar gmFLPF = computeTransconductance(*normalComp);

			Gecode::FloatVar gmLoad = expr(getSpace(), gmFL - gmFLPF);
			dom(getSpace(), gmLoad, 0, 1000);
			Gecode::FloatVar Cdrain = calculateNetCapacitance(*drainNet);
			dom(getSpace(), Cdrain, 0, 1000);
			return expr(getSpace(), gmLoad/(2* 3.14159265 * Cdrain));
		}
		else //For any other circuit
		{
			Gecode::FloatVar gmLoad = computeTransconductance(*compLoadPart);
			dom(getSpace(), gmLoad, 0, 1000);
			Gecode::FloatVar Cdrain = calculateNetCapacitance(*drainNet);
			dom(getSpace(), Cdrain, 0, 1000);
			return expr(getSpace(), gmLoad/(2* 3.14159265 * Cdrain));

		}
	}

	void CircuitSpecificationsConstraints::calculateNonDominantPolesComplementaryFirstStage(PolesAndZeros & polesAndZeros)
	{
		Partitioning::TransconductancePart& firstStage = getPartitioningResult().getFirstStage();
		const StructRec::StructureNet & outputNet = **findOutputNets(firstStage).begin();

		for(auto & connectedStructure : outputNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")))
		{
			if(getPartitioningResult().getPart(*connectedStructure).isLoadPart())
			{
				Partitioning::Component & comp = getPartitioningResult().findComponent(connectedStructure->getIdentifier());

				Gecode::FloatVar gm = computeTransconductance(comp);
				dom(getSpace(), gm, 0, 1);

				const StructRec::StructureNet & sourceNet = connectedStructure->findNet(StructRec::StructurePinType("MosfetNormalArray" ,"Source"));
				Gecode::FloatVar capacitance = calculateNetCapacitance(sourceNet);
				dom(getSpace(), capacitance, 0, 1);

				Gecode::FloatVar fndp = expr(getSpace(), gm/(2*3.14159265* capacitance));
				dom(getSpace(), fndp, 0,1000000000000000000);
				polesAndZeros.addImportantNonDominantPole(fndp);
			}
		}
		assert(polesAndZeros.getImportantNonDominantPoles().size() ==2, "A complementary firstStage should have two non dominant poles");
	}


	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePositiveZeroFirstStage()
	{
		Gecode::FloatVar fndFirstStage = calculateNonDominantPoleFirstStage();
		dom(getSpace(), fndFirstStage, 0, 100000000000000000000000000000);
		return expr(getSpace(), 2* fndFirstStage);
	}


	Gecode::FloatVar CircuitSpecificationsConstraints::calculateNonDominantPoleHigherStages(
			Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage)
	{
		if(getPartitioningResult().hasCompensationCapacitance() && compensationCapacityConnectedBetween(currentStage, previousStage))
		{
			return calculateNonDominatPoleHigherStagesCompensationCapacitance(currentStage);
		}
		else
		{
			Partitioning::TransconductancePart * stage = nullptr;

			if(getPartitioningResult().getSecondStages().size() == 2)
			{
				stage = &getPartitioningResult().getSecondarySecondStage();
			}
			else
			{
				stage = & currentStage;
			}

			const StructRec::StructureNet * poleNet = nullptr;
			Partitioning::Component * poleComp = nullptr;
			if(getPartitioningResult().getBelongingComponents(*stage).size() == 2)
			{
				for(auto & comp : getPartitioningResult().getBelongingComponents(*stage))
				{
					if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
							poleComp = comp;
							poleNet = & poleComp->getArray().findNet(StructRec::StructurePinType(poleComp->getArray().getStructureName().toStr(), "Source"));
							break;
					}
				}
				assert(poleComp != nullptr, "Second Stage should have an not to supply connected transistor!");
			}
			else
			{

					Partitioning::Component & compST = **getPartitioningResult().getBelongingComponents(*stage).begin();
					const StructRec::StructureNet & drainNet = compST.getArray().findNet(StructRec::StructurePinType(compST.getArray().getStructureName().toStr(), "Drain"));

					for( auto & structure : drainNet.getAllConnectedStructures() )
					{
						if(structure->hasPin(StructRec::StructurePinType(structure->getStructureName().toStr(), "Drain"))
								&& structure->findNet(StructRec::StructurePinType(structure->getStructureName().toStr(), "Drain")) == drainNet
								&& structure->getTechType() != compST.getArray().getTechType())
						{
							for(auto & compSB : getPartitioningResult().getBelongingComponents(**stage->getBiasPart().begin()))
							{
								if(compSB->getArray() == *structure)
								{
									poleComp = compSB;
									poleNet = & drainNet;
									break;
								}
							}
						}
					}
					assert(poleComp != nullptr, "Should have a via drain connected bias component");
			}
			Gecode::FloatVar gm = computeTransconductance(*poleComp);
			dom(getSpace(), gm, 0, 100);
			Gecode::FloatVar C = calculateNetCapacitance(*poleNet);
			dom(getSpace(), C,0, 100);
			return expr(getSpace(), gm/(C*2*3.14159265));
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateAdditionalNonDominantPoleHigherStages(
			Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage)
	{
		assert(getPartitioningResult().getBelongingComponents(currentStage).size() == 2, "There exist only an additional non dominant pol in the second stage, if there is a compensation capacity in the circuit");
		const StructRec::StructureNet * poleNet = nullptr;
		Partitioning::Component * poleComp = nullptr;
		for(auto & comp : getPartitioningResult().getBelongingComponents(currentStage))
		{
			const StructRec::StructureNet & drainNet =comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Drain"));
			if(drainNet.findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				&& drainNet.findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			{
				poleNet = & drainNet;
			}
			if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
				poleComp = comp;
			}
		}
		assert(poleComp != nullptr, "Second Stage should have an not to supply connected transistor!");
		Gecode::FloatVar gm = computeTransconductance(*poleComp);
		dom(getSpace(), gm, 0, 100);
		Gecode::FloatVar C = calculateNetCapacitance(*poleNet);
		dom(getSpace(), C,0, 100);
		return expr(getSpace(), gm/(C*2*3.14159265));

	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePositiveZeroSecondStage()
	{
		Gecode::FloatVar fndpSecondStage = calculateNonDominantPoleHigherStages(getPartitioningResult().getPrimarySecondStage(), getPartitioningResult().getFirstStage());
		dom(getSpace(), fndpSecondStage, 1, 100000000000000);
		return expr(getSpace(), 2 * fndpSecondStage );

	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculatePositiveZeroWithCompensationResistance()
	{
		Gecode::FloatVar fpz(getSpace(), 0, 100000000000000000);
		if(getPartitioningResult().hasSecondStage())
		{
			Partitioning::TransconductancePart & ST = **getPartitioningResult().getSecondStages().begin();
			Partitioning::Component * supplyComp = nullptr;
			for(auto & comp : getPartitioningResult().getBelongingComponents(ST))
			{
				if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Source")).isSupply())
				{
					supplyComp = comp;
				}
			}
			assert(supplyComp != nullptr, "A second stage should have a to supply connected component");

			Gecode::FloatVar gmST = computeTransconductance(*supplyComp);

			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::Component & compFT = **getPartitioningResult().getBelongingComponents(firstStage).begin();
			Gecode::FloatVar gmFT = computeTransconductance(compFT);

			Partitioning::CapacitancePart & compCapPart = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component & compCapComp = **getPartitioningResult().getBelongingComponents(compCapPart).begin();
			Gecode::IntVar capValue = getTwoPortToValueMap().find(compCapComp);
			Gecode::FloatVar capValueHelperVar(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
			Gecode::FloatVal p = 0.0000000000001;
			channel(getSpace(), capValueHelperVar, capValue);
			Gecode::FloatVar normedCapValue = Gecode::expr(getSpace() ,capValueHelperVar*p);


			Partitioning::ResistorPart & resPart = getPartitioningResult().getCompensationResistor();
			Partitioning::Component & resComp = **getPartitioningResult().getBelongingComponents(resPart).begin();
			Gecode::FloatVar Rc(getSpace(), 0, 100000000000000);
			Gecode::FloatVar gdsRc = computeOutputConductance(resComp);

			rel(getSpace(), Rc * gdsRc ==1);
			rel(getSpace(), Rc  * gmST >= 1);

			Gecode::FloatVar RgmST(getSpace(), 0, 10000000000000000);
			rel(getSpace(), RgmST * gmST == 1);
			dom(getSpace(), RgmST, 0, 100000000000000000000);

			Gecode::FloatVar substraction = expr(getSpace(), abs(RgmST - Rc));
			dom(getSpace(), substraction, 0, 10000000000000000000);

			rel(getSpace(), fpz * 2 * 3.14159265 * normedCapValue * substraction == 1);
		}

		return fpz;
	}

	void CircuitSpecificationsConstraints::calculateFirstStageSecondLoadPolesAndZeros(PolesAndZeros & polesAndZeros)
	{
		std::vector<Partitioning::LoadPart*> loadsFirstStage = getPartitioningResult().getFirstStage().getLoadPart();

		Partitioning::Component * outputConnectedComp = nullptr;
		Partitioning::Component * supplyConnectedComp = nullptr;
		if(loadsFirstStage.size() == 2)
		{
			for(auto & load : loadsFirstStage)
			{
				if(!load->hasCascodedPair() && getPartitioningResult().getBelongingComponents(*load).size() > 2)
				{
					for(auto & comp : getPartitioningResult().getBelongingComponents(*load))
					{
						if(isOutputNetFirstStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Drain")).getIdentifier()))
						{
							outputConnectedComp = comp;

							const StructRec::StructureNet & sourceNet = outputConnectedComp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source"));
							for(auto & sourceConnectedStructure : sourceNet.getAllConnectedStructures())
							{
								if(sourceConnectedStructure->findNet(StructRec::StructurePinType(sourceConnectedStructure->getStructureName().toStr(), "Drain")) == sourceNet)
								{
									for(auto & comp2 : getPartitioningResult().getBelongingComponents(*load))
									{
										if(*sourceConnectedStructure == comp2->getArray())
										{
											supplyConnectedComp = comp2;
											break;
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
			}
			if(outputConnectedComp != nullptr && supplyConnectedComp != nullptr)
			{
				Gecode::FloatVar CsourceNet = calculateNetCapacitance(outputConnectedComp->getArray().findNet(StructRec::StructurePinType(outputConnectedComp->getArray().getStructureName().toStr(), "Source")));
				dom(getSpace(), CsourceNet, 0, 1000);
				Gecode::FloatVar CgateNet = calculateNetCapacitance(outputConnectedComp->getArray().findNet(StructRec::StructurePinType(outputConnectedComp->getArray().getStructureName().toStr(), "Gate")));
				dom(getSpace(), CgateNet, 0, 1000);
				Gecode::FloatVar gmOutputConnectedComp = computeTransconductance(*outputConnectedComp);
				dom(getSpace(), gmOutputConnectedComp, 0, 10000);
				Gecode::FloatVar gmSupplyConnectedComp = computeTransconductance(*supplyConnectedComp);
				dom(getSpace(), gmSupplyConnectedComp, 0, 10000);
				Gecode::FloatVar fndUpper = expr(getSpace(),gmOutputConnectedComp/(2* 3.14159265 * CsourceNet) );
				dom(getSpace(), fndUpper, 0, 100000000000000000000000000000000000);
				Gecode::FloatVar fndLower = expr(getSpace(),gmSupplyConnectedComp/(2* 3.14159265 * CgateNet) );
				dom(getSpace(), fndLower, 0, 100000000000000000000000000000000000);
				Gecode::FloatVar pzUpper = expr(getSpace(), 2 * fndUpper);
				dom(getSpace(), pzUpper, 0, 100000000000000000000000000000000000);
				Gecode::FloatVar pzLower = expr(getSpace(), 2 * fndLower);
				dom(getSpace(), pzLower, 0, 100000000000000000000000000000000000);

				polesAndZeros.addUnimportantNonDominantPole(fndUpper);
				polesAndZeros.addUnimportantNonDominantPole(fndLower);
				polesAndZeros.addUnimportantZero(pzUpper);
				polesAndZeros.addUnimportantZero(pzLower);

			}
		}
	}

	void CircuitSpecificationsConstraints::createBiasSecondStageNonDominantPolesAndZeros(PolesAndZeros &polesAndZeros)
	{
		if(getPartitioningResult().hasSecondStage())
		{
			Partitioning::BiasPart & biasPartPrimaryStage = **getPartitioningResult().getPrimarySecondStage().getBiasPart().begin();
			if(getPartitioningResult().getBelongingComponents(biasPartPrimaryStage).size()==2)
			{
				for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPartPrimaryStage))
				{
					if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
						Gecode::FloatVar gm = computeTransconductance(*biasComp);
						dom(getSpace(), gm, 0, 1000);
						Gecode::FloatVar C = calculateNetCapacitance(sourceNet);
						dom(getSpace(), C, 0, 1000 );
						Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
						dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
						Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
						polesAndZeros.addUnimportantNonDominantPole(fnd);
						polesAndZeros.addUnimportantZero(pz);
					}
					else if(getPartitioningResult().getSecondStages().size() == 2
							&& biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						const StructRec::StructureNet& gateNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Gate"));
						Gecode::FloatVar gm = computeTransconductance(*biasComp);
						dom(getSpace(), gm, 0, 1000);
						Gecode::FloatVar C = calculateNetCapacitance(gateNet);
						dom(getSpace(), C, 0, 1000 );
						Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
						dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
						Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
						polesAndZeros.addUnimportantNonDominantPole(fnd);
						polesAndZeros.addUnimportantZero(pz);
					}
				}
			}
		}
		if(getPartitioningResult().getSecondStages().size() == 2)
		{
			Partitioning::BiasPart & biasPartSecondaryStage = **getPartitioningResult().getSecondarySecondStage().getBiasPart().begin();
			if(getPartitioningResult().getBelongingComponents(biasPartSecondaryStage).size()==2)
			{
				for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPartSecondaryStage))
				{
					if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
						Gecode::FloatVar gm = computeTransconductance(*biasComp);
						dom(getSpace(), gm, 0, 1000);
						Gecode::FloatVar C = calculateNetCapacitance(sourceNet);
						dom(getSpace(), C, 0, 1000 );
						Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
						dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
						Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
						polesAndZeros.addUnimportantNonDominantPole(fnd);
						polesAndZeros.addUnimportantZero(pz);
					}
				}
			}
		}
	}

	void CircuitSpecificationsConstraints::createBiasHigherStagesNonDominantPolesAndZeros(Partitioning::TransconductancePart & stage, PolesAndZeros &polesAndZeros)
	{
		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
		if(getPartitioningResult().getBelongingComponents(biasPart).size()==2)
		{
			for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPart))
			{
				if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
					Gecode::FloatVar gm = computeTransconductance(*biasComp);
					dom(getSpace(), gm, 0, 1000);
					Gecode::FloatVar C = calculateNetCapacitance(sourceNet);
					dom(getSpace(), C, 0, 1000 );
					Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
					dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
					Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
					polesAndZeros.addUnimportantNonDominantPole(fnd);
					polesAndZeros.addUnimportantZero(pz);
				}
				else if(stage.isPrimarySecondStage() && getPartitioningResult().getSecondStages().size() == 2
						&& biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					const StructRec::StructureNet* gateNet = nullptr;
					if(biasComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray"))
					{
						gateNet = &biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Drain"));
					}
					else
					{
						gateNet = &biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Gate"));
					}

					Gecode::FloatVar gm = computeTransconductance(*biasComp);
					dom(getSpace(), gm, 0, 1000);
					Gecode::FloatVar C = calculateNetCapacitance(*gateNet);
					dom(getSpace(), C, 0, 1000 );
					Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
					dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
					Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
					polesAndZeros.addUnimportantNonDominantPole(fnd);
					polesAndZeros.addUnimportantZero(pz);
				}
			}
		}

		if(stage.isPrimarySecondStage() && getPartitioningResult().hasSecondarySecondStage())
		{
			Partitioning::BiasPart & biasPartSecondaryStage = **getPartitioningResult().getSecondarySecondStage().getBiasPart().begin();
			if(getPartitioningResult().getBelongingComponents(biasPartSecondaryStage).size()==2)
			{
				for(auto &biasComp : getPartitioningResult().getBelongingComponents(biasPartSecondaryStage))
				{
					if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{
						const StructRec::StructureNet& sourceNet = biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source"));
						Gecode::FloatVar gm = computeTransconductance(*biasComp);
						dom(getSpace(), gm, 0, 1000);
						Gecode::FloatVar C = calculateNetCapacitance(sourceNet);
						dom(getSpace(), C, 0, 1000 );
						Gecode::FloatVar fnd = expr(getSpace(), gm/(2* 3.14159265 * C));
						dom(getSpace(), C, 0, 10000000000000000000000000000000000000);
						Gecode::FloatVar pz =expr(getSpace(), 2* fnd);
						polesAndZeros.addUnimportantNonDominantPole(fnd);
						polesAndZeros.addUnimportantZero(pz);
					}
				}
			}
		}
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateErrorFactorTransientFrequency()
	{
		Gecode::FloatVar errorFactor(getSpace(), 0, 1);
		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			rel(getSpace(), errorFactor == 0.5);
		}
		else if(!getPartitioningResult().hasCompensationCapacitance())
		{
			rel(getSpace(), errorFactor == 1);
		}
		else
		{
			assert(getPartitioningResult().hasSecondStage(), "If the the circuit has a compensation capacity, it should have at least a second stage! ");


			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();

			Gecode::FloatVar outputCapacitanceFirstStage = calculateNetCapacitance(**findOutputNets(firstStage).begin());

			Gecode::FloatVar outputCapacitanceSecondStage = calculateNetCapacitance(findOutputNetWithCapacitor(secondStage));

			Gecode::FloatVar rFT = computeOutputResistance(firstStage);
			dom(getSpace(), rFT, 0, 1000000000000000);

			Gecode::FloatVar rST = computeOutputResistance(secondStage);
			dom(getSpace(), rST, 0, 100000000000000);

			Gecode::FloatVar gmST = computeTransconductance(secondStage);
			dom(getSpace(), gmST, 0, 1000000000000000);

			Gecode::FloatVar transconductanceResistanceFactor(getSpace(), 0, 10000000000000000);


			if(!getPartitioningResult().hasThirdStage())
			{
				Gecode::FloatVar capacitiesDivisor = expr(getSpace(), outputCapacitanceSecondStage/ outputCapacitanceFirstStage);
				dom(getSpace(), capacitiesDivisor, 0, 1000000000000000);
				rel(getSpace(), transconductanceResistanceFactor * rFT*gmST == 1);
				dom(getSpace(),transconductanceResistanceFactor, 0, 1000000000000);
				Gecode::FloatVar summandDifferrentFromOne = expr(getSpace(), transconductanceResistanceFactor*(capacitiesDivisor));
				dom(getSpace(), summandDifferrentFromOne, 0, 1000000000000000);
				rel(getSpace(), errorFactor * ( 1 + summandDifferrentFromOne) == 1 );
			}
			else
			{
				Partitioning::TransconductancePart & thirdStage = getPartitioningResult().getThirdStage();

				Gecode::FloatVar outputCapacitanceThirdStage = calculateNetCapacitance(**findOutputNets(thirdStage).begin());
				Gecode::FloatVar gmTT = computeTransconductance(thirdStage);

				if(compensationCapacityConnectedBetween(secondStage, firstStage))
				{
					Gecode::FloatVar capacitiesDivisor = expr(getSpace(), outputCapacitanceSecondStage/ outputCapacitanceFirstStage);
					dom(getSpace(), capacitiesDivisor, 0, 1000000000000000);

					rel(getSpace(), transconductanceResistanceFactor * rFT*gmST == 1);
					dom(getSpace(),transconductanceResistanceFactor, 0, 1000000000000);
					Gecode::FloatVar summandDifferrentFromOne = expr(getSpace(), transconductanceResistanceFactor*(1+ capacitiesDivisor));
					dom(getSpace(), summandDifferrentFromOne, 0, 1000000000000000);

					rel(getSpace(), errorFactor * ( 1 + summandDifferrentFromOne) == 1 );
				}
				else if(compensationCapacityConnectedBetween(thirdStage, secondStage))
				{
					Gecode::FloatVar capacitiesDivisor = expr(getSpace(), outputCapacitanceThirdStage/ outputCapacitanceSecondStage);
					dom(getSpace(), capacitiesDivisor, 0, 1000000000000000);
					rel(getSpace(), transconductanceResistanceFactor * rST*gmTT == 1);
					dom(getSpace(),transconductanceResistanceFactor, 0, 1000000000000);
					Gecode::FloatVar summandDifferrentFromOne = expr(getSpace(), transconductanceResistanceFactor*(1+ capacitiesDivisor));
					dom(getSpace(), summandDifferrentFromOne, 0, 1000000000000000);

					rel(getSpace(), errorFactor * ( 1 + summandDifferrentFromOne) == 1 );
				}
				else
				{
					Gecode::FloatVar capacitiesDivisor = expr(getSpace(), outputCapacitanceThirdStage/ outputCapacitanceFirstStage);
					dom(getSpace(), capacitiesDivisor, 0, 1000000000000000);
					rel(getSpace(), transconductanceResistanceFactor * rFT * rST*gmST * gmTT == 1);
					dom(getSpace(),transconductanceResistanceFactor, 0, 1000000000000);

					Gecode::FloatVar summandDifferrentFromOne = expr(getSpace(), transconductanceResistanceFactor*(1+ capacitiesDivisor));
					dom(getSpace(), summandDifferrentFromOne, 0, 1000000000000000);

					rel(getSpace(), errorFactor * ( 1 + summandDifferrentFromOne) == 1 );
				}
			}


		}
		return errorFactor;
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::calculateComplexPoleFrequencyThreeStageOpAmp()
	{
		assert(getPartitioningResult().hasThirdStage() && getPartitioningResult().hasCompensationCapacitance(), "To have a important complex pole frequency, the circuit needs to have a third stage and a compensation capacity!");

		Gecode::FloatVar fndpComplex(getSpace(), 0, 1000000000000000000);
		if(compensationCapacityConnectedBetween(getPartitioningResult().getFirstStage(), getPartitioningResult().getThirdStage())
				&& !hasCompensationCapacity(**findOutputNets(getPartitioningResult().getFirstStage()).begin()))
		{
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
			Partitioning::TransconductancePart & thirdStage = getPartitioningResult().getThirdStage();

			Gecode::FloatVar cout = calculateNetCapacitance(**findOutputNets(thirdStage).begin());
			dom(getSpace(), cout, 0, 10000000000);
			Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
			Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
			Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
			Gecode::FloatVar compCapValueHelperVar(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
			Gecode::FloatVal p = 0.0000000000001;
			channel(getSpace(), compCapValueHelperVar, compCapValue);
			Gecode::FloatVar cc = Gecode::expr(getSpace() ,compCapValueHelperVar*p);

			Partitioning::Component & gcc = **getPartitioningResult().findComponents(firstStage.getHelperStructure()).begin();
			Gecode::FloatVar gmGcc = computeTransconductance(gcc);
			dom(getSpace(), gmGcc, 0, 10000);

			Gecode::FloatVar rout1 = computeOutputResistance(firstStage);
			Gecode::FloatVar cout1 = calculateNetCapacitance(**findOutputNets(firstStage).begin());
			dom(getSpace(), rout1, 0, 100000000000000000);
			dom(getSpace(), cout1, 0, 10000);

			Gecode::FloatVar rc1 = expr(getSpace(), rout1 * cout1);
			dom(getSpace(), rc1, 0, 1000000000);

			Gecode::FloatVar rout2 = computeOutputResistance(secondStage);
			Gecode::FloatVar cout2 = calculateNetCapacitance(**findOutputNets(secondStage).begin());
			Gecode::FloatVar gm2 = computeTransconductance(secondStage);
			dom(getSpace(), rout2, 0, 100000000000000000);
			dom(getSpace(), cout2, 0, 10000);
			dom(getSpace(), gm2, 0, 10000);

			Gecode::FloatVar rc2 = expr(getSpace(), rout2 * cout2);
			dom(getSpace(), rc2, 0, 1000000000);


			Gecode::FloatVar gm3 = computeTransconductance(thirdStage);
			dom(getSpace(), gm3, 0, 10000);

			Gecode::FloatVar denominatorCcSummand = expr(getSpace(), gm2* gm3 * rout1 * rout2 * cc);
			dom(getSpace(), denominatorCcSummand, 0, 100000000000000);
			Gecode::FloatVar denominator = expr(getSpace(), cout + denominatorCcSummand);
			dom(getSpace(), denominator, 0, 100000000000000000);

			Gecode::FloatVar divisorCcGmGcc = expr(getSpace(), cc/gmGcc);
			dom(getSpace(), divisorCcGmGcc, 0, 100000000000000);


			Gecode::FloatVar a1 = expr(getSpace(), (cout* (divisorCcGmGcc + rc1 + rc2 )) / (denominator));


			Gecode::FloatVar rc1rc2 = expr(getSpace(), rc1 * rc2);
			dom(getSpace(), rc1rc2, 0, 10000000000000);
			Gecode::FloatVar enumeratorFractionWithGmGcc = expr(getSpace(), cc *(rc1 + rc2));
			dom(getSpace(), enumeratorFractionWithGmGcc, 0, 100000);

			Gecode::FloatVar fractionWithGmGcc = expr(getSpace(), enumeratorFractionWithGmGcc/gmGcc);
			dom(getSpace(), fractionWithGmGcc, 1, 100000000000);
			Gecode::FloatVar sumFractionOfGmGccRc1rc2 = expr(getSpace(),fractionWithGmGcc+ rc1rc2 );
			dom(getSpace(), sumFractionOfGmGccRc1rc2, 0, 10000000000 );


			Gecode::FloatVar a2 = expr(getSpace(), (cout* sumFractionOfGmGccRc1rc2   ) / (denominator));

			Gecode::FloatVar ft =  getSpace().getTransitFrequencyVariable();
			Gecode::FloatVar wt = expr(getSpace(), 2 * 3.14159265 * ft);
			dom(getSpace(), wt , 0, 10000000000000000);

			Gecode::FloatVar denominatorOfFndp = expr(getSpace(), a1 * wt);
			dom(getSpace(), denominatorOfFndp, 0, 10000000000000000000);

			Gecode::FloatVar a2Term = expr(getSpace(), a2 * pow(wt,2));
			dom(getSpace(), a2Term, 0, 10000000000000000000);

			Gecode::FloatVar enumeratorFndp = expr(getSpace(), abs(1 - a2Term ));
				dom(getSpace(), enumeratorFndp, 0, 10000000000000000000);

			rel(getSpace(),fndpComplex == (enumeratorFndp) / (denominatorOfFndp));

		}
		else
		{
			rel(getSpace(), fndpComplex == 1000000000000000000000000000);
		}
		return fndpComplex;
	}



	Gecode::FloatVar CircuitSpecificationsConstraints::computeTransconductance(Partitioning::TransconductancePart & stage)
	{
		std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(stage);
		if(stage.isFirstStage())
		{
			if(stage.isComplementary())
			{
				assert(comps.size() == 4, "A complementary first stage should have 4 devices!");
				Partitioning::Component * pmosComp1 = nullptr;
				Partitioning::Component * pmosComp2 = nullptr;
				Partitioning::Component * nmosComp1 = nullptr;
				Partitioning::Component * nmosComp2 = nullptr;

				for(auto & comp : comps)
				{
					if(comp->getArray().getTechType().isP() && pmosComp1 == nullptr)
					{
						pmosComp1 = comp;
					}
					else if(comp->getArray().getTechType().isP())
					{
						pmosComp2 = comp;
					}
					else if(comp->getArray().getTechType().isN() && nmosComp1 == nullptr)
					{
						nmosComp1 = comp;
					}
					else
					{
						nmosComp2 = comp;
					}
				}
				assert(nmosComp1 != nullptr && nmosComp2 != nullptr && pmosComp1 != nullptr && pmosComp2 != nullptr, "There should be two devices for each tech type!" );

				Gecode::FloatVar nmos1Transconductance = computeTransconductance(*nmosComp1);
				Gecode::FloatVar nmos2Transconductance = computeTransconductance(*nmosComp2);
				Gecode::rel(getSpace(), nmos1Transconductance == nmos2Transconductance);

				Gecode::FloatVar pmos1Transconductance = computeTransconductance(*pmosComp1);
				Gecode::FloatVar pmos2Transconductance = computeTransconductance(*pmosComp2);
				Gecode::rel(getSpace(), pmos1Transconductance == pmos2Transconductance);

				return expr(getSpace(), nmos1Transconductance + pmos1Transconductance);
			}
			else
			{
				assert(comps.size() == 2, "There is no first stage consisting of differential pair");
				Gecode::FloatVar firstTranTransconductance = computeTransconductance(**comps.begin());
				Gecode::FloatVar secondTranTransconductance = computeTransconductance(**std::next(comps.begin()));
				Gecode::rel(getSpace(), firstTranTransconductance == secondTranTransconductance);

				return firstTranTransconductance;
			}

		}
		else
		{
			assert(stage.isHigherStage(), "If stage is not complementary or first stage, it needs to be from type higher stage!");
			Partitioning::Component * supplyComp = nullptr;

			for(auto & comp : getPartitioningResult().getBelongingComponents(stage))
			{
				if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
				{
					supplyComp = comp;
				}
			}

			assert(supplyComp != nullptr, "Every higher stage should have a to supply connected component");
			return computeTransconductance(*supplyComp);
		}

	}

	Gecode::FloatVar CircuitSpecificationsConstraints::computeTransconductance(Partitioning::Component& transistor) const
	{
		Gecode::FloatVal mu = getSpace().getScalingFactorMUM();
		Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(transistor));
		if(getTransistorModel()  == "SHM")
		{
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(transistor);

			if(getCircuitInformation().getDesignAttributes().isInWeakInversion(transistor))
			{
				float n = techSpec.getSlopeFactor();
				float Vt = techSpec.getThermalVoltage();
				float Cox = techSpec.getGateOxideCapacity();
				float Cj = techSpec.getZeroBiasBulkJunctionCapacitance();
				float factor = (Cj/Cox +1) * 1/(n*Vt);

				return Gecode::expr(getSpace(),factor* abs(ids));
			}
			else
			{

				Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();

				Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
				Gecode::FloatVar lengthHelper(getSpace(), 1, getSpace().getLengthUpperBound());

				channel(getSpace(), widthHelper, getTransistorToWidthMap().find(transistor));
				channel(getSpace(), lengthHelper, getTransistorToLengthMap().find(transistor));

				Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelper*mu);
				Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelper*mu);


				return Gecode::expr(getSpace(), sqrt(2* muCox * (width/length) * abs(ids)));
			}
		}
		else
		{
			//Change code suited for ekv model
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(transistor);

			Gecode::FloatVal muCox = techSpec.getMobilityOxideCapacityCoefficient();

			Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
			Gecode::FloatVar lengthHelper(getSpace(), 1, getSpace().getLengthUpperBound());

			channel(getSpace(), widthHelper, getTransistorToWidthMap().find(transistor));
			channel(getSpace(), lengthHelper, getTransistorToLengthMap().find(transistor));

			Gecode::FloatVar width = Gecode::expr(getSpace(),widthHelper*mu);
			Gecode::FloatVar length = Gecode::expr(getSpace(), lengthHelper*mu);
			return Gecode::expr(getSpace(), sqrt(2* muCox * (width/length) * abs(ids) ));
		}
	}

	 Gecode::FloatVar CircuitSpecificationsConstraints::computeOutputResistanceFirstStage()
	 {
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		Partitioning::Component & transPartTransistor = **getPartitioningResult().getBelongingComponents(firstStage).begin();
		std::vector<Partitioning::LoadPart *> loadParts = firstStage.getLoadPart();
		Gecode::FloatVar rout(getSpace(), 0, 100000000000000);
		Gecode::FloatVar gdTran = computeOutputConductance(transPartTransistor);

		if(getPartitioningResult().hasSecondarySecondStage())
		{
			 std::vector<Partitioning::Component*> loadComps = getPartitioningResult().getBelongingComponents(**loadParts.begin());
			 Partitioning::Component * outputComp = nullptr;
			 for(auto & loadComp : loadComps)
			 {
				 const StructRec::StructureNet * gateNet = nullptr;
				if(loadComp->getArray().getStructureName() == StructRec::StructureName("MosfetDiodeArray")
					&& loadComp->getArray().isPartOfCurrentMirror())
				{
					gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
				}
				else if(loadComp->getArray().isPartOfCurrentMirror())
				{
					gateNet = &loadComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				}
				if(gateNet != nullptr)
				{
					for(auto & outputNet : findOutputNetsFirstStage() )
					{
						if(gateNet->getIdentifier() == outputNet->getIdentifier())
						{
							outputComp = loadComp;
							break;
						}
					}
				}

				if(outputComp != nullptr)
					break;
			 }
			 assert(outputComp != nullptr, "There must be one load comp connected to the first stage output");
			 Gecode::FloatVar gmLoad = computeTransconductance(*outputComp);
			Gecode::rel(getSpace(), rout * gmLoad == 1);
		}
		else if(loadParts.size() == 1)
		{
			Gecode::FloatVar gdLoad = computeOutputConductance(**loadParts.begin(), firstStage);
			 dom(getSpace(), gdLoad, 0, 1);
			Gecode::rel(getSpace(), rout * (gdLoad + gdTran) == 1);
		}
		else if(firstStage.hasHelperStructure() || firstStage.isComplementary())
		{
			Gecode::FloatVar gdLoad1 = computeOutputConductance(**loadParts.begin(), firstStage);
			dom(getSpace(), gdLoad1, 0, 1);
			Gecode::FloatVar gdLoad2 = computeOutputConductance(**std::next(loadParts.begin()), firstStage);
			dom(getSpace(), gdLoad2, 0, 1);
			Gecode::rel(getSpace(), rout * (gdLoad1 + gdLoad2) == 1);

		}
		else
		{
			Gecode::FloatVar gdLoad1 = computeOutputConductance(**loadParts.begin(), firstStage);
			dom(getSpace(), gdLoad1, 0, 100);
			Gecode::FloatVar gdLoad2 = computeOutputConductance(**std::next(loadParts.begin()), firstStage);
			dom(getSpace(), gdLoad2, 0, 100);
			Gecode::rel(getSpace(), rout * (gdLoad1 + gdLoad2 + gdTran) == 1);
		 }
		return rout;
	 }

		Gecode::FloatVar CircuitSpecificationsConstraints::computeOutputResistanceHigherStage(Partitioning::TransconductancePart & higherStage)
		{

			assert(higherStage.isHigherStage(), "The used stage should be of type higher stage!");

			Partitioning::BiasPart & biasPart = **higherStage.getBiasPart().begin();

			Gecode::FloatVar Rout(getSpace(), 0, 100000000000000000000000000000);

			Gecode::FloatVar gST(getSpace(), 0, 10000000000);
			Gecode::FloatVar gSB(getSpace(), 0, 100000000000);


			gST = computeOutputConductance(higherStage, higherStage);
			dom(getSpace(), gST, 0, 100);

			gSB =  computeOutputConductance(biasPart, higherStage);
			dom(getSpace(), gSB, 0, 100);

			rel(getSpace(), Rout * (gST + gSB) == 1);
			return Rout;

		}

		Gecode::FloatVar CircuitSpecificationsConstraints::computeOutputResistance(Partitioning::TransconductancePart & stage)
		{
			Gecode::FloatVar rout(getSpace(), 0, 100000000000000000);
			if(stage.isFirstStage() || stage.isFeedBack())
			{

				rout = computeOutputResistanceFirstStage();
			}
			else
			{
				rout = computeOutputResistanceHigherStage(stage);
			}
			return rout;
		}

	 Gecode::FloatVar CircuitSpecificationsConstraints::computeOutputConductance(Partitioning::Component& transistor)
	 {
		 Gecode::FloatVar ids = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(transistor));
		 if(getTransistorModel() == "SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(transistor);
			if(transistor.getPart().isResistorPart() && getPartitioningResult().getResistorPart(transistor.getArray()).isCompensationResistor())
			{
				float muCox = techSpec.getMobilityOxideCapacityCoefficient();
				float vth = techSpec.getThresholdVoltage();

				 Gecode::IntVar length = getTransistorToLengthMap().find(transistor);
				 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
				 channel(getSpace(), lengthHelperVar, length);

				 Gecode::IntVar width = getTransistorToWidthMap().find(transistor);
				 Gecode::FloatVar widthHelperVar(getSpace(), 1, getSpace().getWidthUpperBound());
				 channel(getSpace(), widthHelperVar, width);

				Edge * drainSourceEdge = NULL;
				Edge * gateSourceEdge = NULL;
				std::vector<Edge*> edges = getGraph().findToComponentBelongingEdges(transistor);
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

				return Gecode::expr(getSpace(), muCox * widthHelperVar/lengthHelperVar * abs((vgs -vth)));
			}
			else
			{
				 float lamda;
				 if(getCircuitInformation().getDesignAttributes().isInWeakInversion(transistor))
				 {
					 lamda = techSpec.getChannelLengthCoefficientWeakInversion();
				 }
				 else
				 {
					 lamda = techSpec.getChannelLengthCoefficientStrongInversion();
				 }
				 if(transistor.getArray().getTechType().isN())
				 {
					 return Gecode::expr(getSpace(), lamda * ids);
				 }
				 else
				 {
					 return Gecode::expr(getSpace(), -1* lamda * ids);
				 }
			}
		 }
		 else
		 { //Change Code suitable for EKV model
			 const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(transistor);

		 	//Needed for a varying lamda
		 	 Gecode::FloatVal earlyVoltage = techSpec.getEarlyVoltage();
		 	 Gecode::IntVar length = getTransistorToLengthMap().find(transistor);
		 	 Gecode::FloatVar lengthHelperVar(getSpace(), 1, getSpace().getLengthUpperBound());
		 	 channel(getSpace(), lengthHelperVar, length);
		 	 Gecode::FloatVar lamda(getSpace(), 0.01, 0.9);
		 	 //Varying lamda
		 	 // Gecode::rel(getSpace(), lamda* earlyVoltage * lengthHelperVar == 1);

		 	 //Constant lamda
		 	 Gecode::rel(getSpace(), lamda == techSpec.getChannelLengthCoefficient());
		 	 if(transistor.getArray().getTechType().isN())
		 	 {
		 		 return Gecode::expr(getSpace(), lamda  *ids);
		 	 }
		 	 else
		 	 {
		 		 return Gecode::expr(getSpace(), -1* lamda * ids);
		 	 }

		 }
	 }

	 Gecode::FloatVar CircuitSpecificationsConstraints::computeEdgeCurrent(Edge & edge)
	 {
	 	if(edge.isDrainSourceEdge())
	 	{
	 		return getSpace().createFloatCurrent(getTransistorToCurrentMap().find(edge.getEdgeKey().getStructureId()));
	 	}
	 	else
	 	{
	 		Gecode::FloatVar current(getSpace(), 0,1);
	 		Gecode::rel(getSpace(), current == 0);
	 		return current;
	 	}
	}

	 Gecode::FloatVar CircuitSpecificationsConstraints::computeEdgeVoltage(const Edge& edge)
	 {

		const  Core::NetId startNetId = edge.getStartNodeId();
	 	const Core::NetId endNetId = edge.getEndNodeId();
	 	Gecode::FloatVar startNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(startNetId));
	 	Gecode::FloatVar endNetVoltage = getSpace().createFloatVoltage(netToVoltageMap_->find(endNetId));
	 	return Gecode::expr(getSpace(), startNetVoltage - endNetVoltage);
	}

	 float CircuitSpecificationsConstraints::computeSign(const Edge & edge, const Path & path) const
	 {
	 	float sign = -1;
	 	const std::vector<const Edge*> edges = path.getEdges();
	 	if(edge.getEdgeKey() == (*edges.begin())->getEdgeKey())
	 	{
	 		if(edge.getStartNodeId() == path.getStartNode().getNetId())
	 		{
	 			sign = 1;
	 		}
	 	}
	 	else
	 	{
	 		for(std::vector<const Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
	 		{
	 			const Edge* posEdge = * std::next(it);
	 			if(edge.getEdgeKey() == posEdge->getEdgeKey())
	 			{
	 				const Edge*  previousEdge = *it;
	 				if(edge.getStartNodeId() == previousEdge->getEndNodeId() || edge.getStartNodeId() == previousEdge->getStartNodeId())
	 				{
	 					sign = 1;
	 				}
	 			}
	 		}
	 	}
	 	return sign;
	 }



	 Gecode::FloatVar CircuitSpecificationsConstraints::calculateNonDominatPoleHigherStagesCompensationCapacitance(
			 Partitioning::TransconductancePart & stage)
	 {
		 float tenToTheThree = pow (10, 3);
		 float tenToTheFivetheen = pow(10,15);
		 float fento = pow(10, -15);

		 Partitioning::Component * transComp = nullptr;

		for(auto &compStage :getPartitioningResult().getBelongingComponents(stage))
		{
			if(compStage->getArray().findNet(StructRec::StructurePinType(compStage->getArray().getStructureName(), "Source")).isSupply())
			{
				transComp = compStage;
			}
		}
		assert(transComp != nullptr, "Stage should have a supply connected component");

		Gecode::FloatVar gmTrans = computeTransconductance(*transComp);

		const StructRec::StructureNet * outputNet = nullptr;
		std::vector<const StructRec::StructureNet*> outputNets = findOutputNets(stage);
		for(auto & net : outputNets)
		{
			if(hasCompensationCapacity(*net))
			{
				outputNet = net;
				break;
			}
		}
		assert(outputNet != nullptr);
		const StructRec::StructureNet & gateNet = transComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));

		Gecode::FloatVar cout = calculateNetCapacitance(*outputNet);
		dom(getSpace(), cout, 0, 1000);
		Gecode::FloatVar fentoCout = Gecode::expr(getSpace(), cout * tenToTheFivetheen);

		Partitioning::CapacitancePart & compCap = getPartitioningResult().getCompensationCapacitance();
		Partitioning::Component * compCapComp = *getPartitioningResult().getBelongingComponents(compCap).begin();
		Gecode::IntVar compCapValue = getTwoPortToValueMap().find(*compCapComp);
		Gecode::FloatVar valueHelperVarCompCap(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
		channel(getSpace(), valueHelperVarCompCap, compCapValue);
		Gecode::FloatVar fentoCompCapValue = Gecode::expr(getSpace(), valueHelperVarCompCap * tenToTheThree);

		Gecode::FloatVar cgSecondStage = calculateNetCapacitance(gateNet);
		dom(getSpace(), cgSecondStage, 0, 1000);

		Gecode::FloatVar fentoCgSecondStage = Gecode::expr(getSpace(), cgSecondStage * tenToTheFivetheen);


		Gecode::FloatVar summand1 = Gecode::expr(getSpace(), (( fentoCompCapValue * fentoCgSecondStage)));
		dom(getSpace(), summand1, 0, 10000000000000000000000000000000000000);
		Gecode::FloatVar summand2 = Gecode::expr(getSpace(), (( fentoCout * fentoCgSecondStage)));
		dom(getSpace(), summand2, 0, 100000000000000000000000000000000000000);
		Gecode::FloatVar summand3 = Gecode::expr(getSpace(), (( fentoCout * fentoCompCapValue)));
		dom(getSpace(), summand3, 0, 10000000000000000000000000000000000000);
		Gecode::FloatVar divisor = Gecode::expr(getSpace(), 2* 3.1415926535* fento *(( summand1 +summand2 +summand3)));
		return Gecode::expr(getSpace(), gmTrans  * fentoCompCapValue /(divisor));

	 }





	 Gecode::FloatVar CircuitSpecificationsConstraints::computeGateSourceCapacitance(Partitioning::Component& component) const
	 {
		 float u = getSpace().getScalingFactorMUM();

		 float overlapCapacity;
		 float gateOxideCapacity;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			overlapCapacity = techSpec.getOverlapCapacity();
			gateOxideCapacity = techSpec.getGateOxideCapacity();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			overlapCapacity = techSpec.getOverlapCapacity();
			gateOxideCapacity = techSpec.getGateOxideCapacity();
		}

		 Gecode::IntVar width = getTransistorToWidthMap().find(component);
		 Gecode::IntVar length = getTransistorToLengthMap().find(component);

		 Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
		 Gecode::FloatVar lengthHelper(getSpace(), 1, getSpace().getLengthUpperBound());

		 channel(getSpace(), width, widthHelper);
		 channel(getSpace(), length, lengthHelper);

		 Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u);


		 return Gecode::expr(getSpace(), 0.66666667 * lengthHelper * u* widthHelper * u * gateOxideCapacity + normedWidth * overlapCapacity);

	 }

	 Gecode::FloatVar CircuitSpecificationsConstraints::computeGateBulkCapacitance(Partitioning::Component& component) const
	 {
		 float u = getSpace().getScalingFactorMUM();


		 float gateOxideCapacity;
		 float lateralDiffusionLength;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		}

	 	 Gecode::IntVar width = getTransistorToWidthMap().find(component);
	 	 Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
	 	 channel(getSpace(), width, widthHelper);
	 	 Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u);


	 	return Gecode::expr(getSpace(), normedWidth * lateralDiffusionLength * gateOxideCapacity);

	 }

	Gecode::FloatVar CircuitSpecificationsConstraints::computeGateDrainCapacitance(Partitioning::Component& component) const
	{
		float u = getSpace().getScalingFactorMUM();

		 float gateOxideCapacity;
		 float lateralDiffusionLength;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			gateOxideCapacity = techSpec.getGateOxideCapacity();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
		}


		Gecode::IntVar width = getTransistorToWidthMap().find(component);
		Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
		channel(getSpace(), width, widthHelper);
		Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u);


		return Gecode::expr(getSpace(), gateOxideCapacity* lateralDiffusionLength* normedWidth);
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::computeDrainBulkCapacitance(Partitioning::Component& component) const
	{

		float u = getSpace().getScalingFactorMUM();


		Core::NetId drainNet = component.getArray().findNet(StructRec::StructurePinType(component.getArray().getIdentifier().getName().toStr(),"Drain")).getIdentifier();
		Core::NetId bulkNet = component.getArray().findNet(StructRec::StructurePinType(component.getArray().getIdentifier().getName().toStr(),"Bulk")).getIdentifier();

		Gecode::FloatVar drainVoltage = getSpace().createFloatVoltage(getNetToVoltageMap().find(drainNet));
		Gecode::FloatVar bulkVoltage = getSpace().createFloatVoltage(getNetToVoltageMap().find(bulkNet));
		Gecode::FloatVar vdb = expr(getSpace(), abs(drainVoltage - bulkVoltage));

		 float zeroBiasBulkJunctionCapacitance;
		 float lateralDiffusionLength;
		 float zeroBiasSidewallBulkJunctionCapacitance;
		 float bulkJunctionContactPotential;
		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();
			bulkJunctionContactPotential = techSpec.getBulkJunctionContactPotential();
		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();
			bulkJunctionContactPotential = techSpec.getBulkJunctionContactPotential();
		}

		Gecode::IntVar width = getTransistorToWidthMap().find(component);
		Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
		channel(getSpace(), width, widthHelper);
		Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u);
		Gecode::FloatVar vdbDividedByBulkJunctionPotential(getSpace(), 0, 1000000000000000000000000000000000);
		Gecode::rel(getSpace(), vdb == vdbDividedByBulkJunctionPotential * bulkJunctionContactPotential);

		Gecode::FloatVar squareRootOnePlusVddDividedByBulkJunktionPotential = expr(getSpace(), sqrt(1+ vdbDividedByBulkJunctionPotential));
		Gecode::FloatVar oneDividedBySquareRootOnePlusVddDividedByBulkJunktionPotential(getSpace(), 0, 10000000000000000000000000000000);
		Gecode::rel(getSpace(), oneDividedBySquareRootOnePlusVddDividedByBulkJunktionPotential * squareRootOnePlusVddDividedByBulkJunktionPotential == 1);

		return Gecode::expr(getSpace(), (zeroBiasBulkJunctionCapacitance* lateralDiffusionLength* normedWidth + (2 * lateralDiffusionLength + normedWidth) * zeroBiasSidewallBulkJunctionCapacitance )* oneDividedBySquareRootOnePlusVddDividedByBulkJunktionPotential);
	}

	Gecode::FloatVar CircuitSpecificationsConstraints::computeSourceBulkCapacitance(Partitioning::Component& component) const
	{
		float u = getSpace().getScalingFactorMUM();

		 float zeroBiasBulkJunctionCapacitance;
		 float lateralDiffusionLength;
		 float zeroBiasSidewallBulkJunctionCapacitance;

		 if(getTransistorModel() =="SHM")
		 {
			const TechnologieSpecificationSHM & techSpec = getCircuitInformation().getTechnologieSpecificationSHM(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();

		 }
		 else
		 {
			const TechnologieSpecificationEKV & techSpec = getCircuitInformation().getTechnologieSpecificationEKV(component);
			zeroBiasBulkJunctionCapacitance = techSpec.getZeroBiasBulkJunctionCapacitance();
			lateralDiffusionLength = techSpec.getLateralDiffusionLength();
			zeroBiasSidewallBulkJunctionCapacitance = techSpec.getZeroBiasSidewallBulkJunctionCapacitance();

		}

		Gecode::IntVar width = getTransistorToWidthMap().find(component);
		Gecode::FloatVar widthHelper(getSpace(), 1, getSpace().getWidthUpperBound());
		channel(getSpace(), width, widthHelper);
		Gecode::FloatVar normedWidth = Gecode::expr(getSpace(), widthHelper * u);
		return Gecode::expr(getSpace(), (zeroBiasBulkJunctionCapacitance* lateralDiffusionLength* normedWidth + (2 * lateralDiffusionLength + normedWidth) * zeroBiasSidewallBulkJunctionCapacitance));
	}

	void CircuitSpecificationsConstraints::setTransistorModel(std::string model)
	{
		transistorModel_ = model;
	}

	std::string CircuitSpecificationsConstraints::getTransistorModel() const
	{
		assert(transistorModel_ =="EKV" || transistorModel_ == "SHM", "Transistor model not supported");
		return transistorModel_;
	}

	const StructRec::StructureCircuits&  CircuitSpecificationsConstraints::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return *structRecResult_;
	}

	const PolesAndZeros& CircuitSpecificationsConstraints::getPolesAndZeros() const
	{
		assert(polesAndZeros_ != NULL);
		return *polesAndZeros_;
	}

	 Gecode::FloatVar CircuitSpecificationsConstraints::getOutputCurrent()
	 {
		 const Node  & outputNode = getGraph().getOutputNode();
		 Gecode::FloatVar current(getSpace(), -1, 1);

		 std::set<Edge*> edges = outputNode.getAllEdges();

		 for(std::set<Edge*>::const_iterator it = edges.begin(); it != edges.end(); it++)
		 {
			 Edge * edge = *it;
			 if(edge->isDrainSourceEdge())
			 {
				 current = getSpace().createFloatCurrent(getTransistorToCurrentMap().find(edge->getEdgeKey().getStructureId()));
				 break;
			 }
		 }
		 return current;
	 }

		Gecode::FloatVar CircuitSpecificationsConstraints::calculateNetCapacitance(const StructRec::StructureNet & net) const
		{
			Gecode::FloatVar netCapacitance(getSpace(), 0, 10000);
			const std::vector<StructRec::StructurePin*> pins = net.getConnectedPins();
			std::vector<Gecode::FloatVar> capacitances;

			for(auto & it_pins : pins)
			{
				StructRec::StructurePin & pin = *it_pins;

				Partitioning::Component & component = getPartitioningResult().findComponent(pin.getStructure().getIdentifier());
				if(pin.getPinType() == StructRec::StructurePinType("CapacitorArray", "Plus") || pin.getPinType() == StructRec::StructurePinType("CapacitorArray", "Minus"))
				{
					Gecode::IntVar capValue = getTwoPortToValueMap().find(component);
					Gecode::FloatVar capValueHelperVar(getSpace(), 0, getCircuitInformation().getCircuitParameter().getMaxLoadCapacity().getValue()*10+10);
					Gecode::FloatVal p = 0.0000000000001;
					channel(getSpace(), capValueHelperVar, capValue);
					Gecode::FloatVar normedCapValue = Gecode::expr(getSpace() ,capValueHelperVar*p);
					capacitances.push_back(normedCapValue);
				}
				else if(pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Gate"))
				{
					if(!(hasCompensationCapacity(net) && component.getPart().isTransconductancePart() && getPartitioningResult().getTransconductancePart(component.getArray()).isHigherStage()))
					{
						Gecode::FloatVar cgs = computeGateSourceCapacitance(component);
						Gecode::FloatVar cgd = computeGateDrainCapacitance(component);
						Gecode::FloatVar cgb = computeGateBulkCapacitance(component);
						capacitances.push_back(cgs);
						capacitances.push_back(cgd);
						capacitances.push_back(cgb);
					}
				}
				else if(pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Drain"))
				{
					Gecode::FloatVar cgd = computeGateDrainCapacitance(component);
					Gecode::FloatVar cdb = computeDrainBulkCapacitance(component);
					dom(getSpace(), cdb, 0, 100);

					capacitances.push_back(cdb);
					capacitances.push_back(cgd);
				}
				else if(pin.getPinType() == StructRec::StructurePinType("MosfetNormalArray", "Source") || pin.getPinType() == StructRec::StructurePinType("MosfetDiodeArray", "Source"))
				{
					Gecode::FloatVar cgs = computeGateSourceCapacitance(component);
					Gecode::FloatVar csb = computeSourceBulkCapacitance(component);

					capacitances.push_back(cgs);
					capacitances.push_back(csb);
				}
				else if(pin.getPinType() == StructRec::StructurePinType("MosfetDiodeArray", "Drain"))
				{
					Gecode::FloatVar cgd = computeGateDrainCapacitance(component);
					Gecode::FloatVar cdb = computeDrainBulkCapacitance(component);
					Gecode::FloatVar cgs = computeGateSourceCapacitance(component);
					Gecode::FloatVar cgb = computeGateBulkCapacitance(component);
					dom(getSpace(), cdb, 0, 100);

					capacitances.push_back(cdb);
					capacitances.push_back(cgd);
					capacitances.push_back(cgb);
					capacitances.push_back(cgs);
				}
			}

			Gecode::FloatValArgs sign(capacitances.size());
			Gecode::FloatVarArgs capValues(capacitances.size());
			int index = 0;
			for(auto & it_cap: capacitances)
			{
				capValues[index] = it_cap;
				sign[index] =1;
				index++;
			}
			Gecode::linear(getSpace(), sign, capValues, Gecode::FRT_EQ, netCapacitance);

			return netCapacitance;
		}

		bool CircuitSpecificationsConstraints::isOutputNetFirstStage(Core::NetId outputNet) const
		{
			bool isOutputNet = false;

			if(getPartitioningResult().hasSecondStage())
			{
				for(auto & secondStage : getPartitioningResult().getSecondStages())
				{
					Partitioning::Component & compSecondStage = **getPartitioningResult().getBelongingComponents(*secondStage).begin();
					if(compSecondStage.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier() == outputNet)
					{
						isOutputNet = true;
					}
				}
			}
			else
			{
				if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
				{
					if(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId() == outputNet
							|| getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId() == outputNet)
					{
						isOutputNet = true;
					}
				}
				else
				{
					if(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId() == outputNet)
					{
						isOutputNet = true;
					}
				}
			}

			return isOutputNet;
		}

		bool CircuitSpecificationsConstraints::compensationCapacityConnectedBetween(Partitioning::TransconductancePart &stage1, Partitioning::TransconductancePart &stage2) const
		{
			bool isConnected = false;
			const StructRec::StructureNet * minusNet = nullptr;
			const StructRec::StructureNet * plusNet = nullptr;

			for(auto & it : getPartitioningResult().getCompensationCapacitors())
			{
			Partitioning::CapacitancePart & compCap = *it;
			Partitioning::Component & compCompCap = **getPartitioningResult().getBelongingComponents(compCap).begin();
			const StructRec::StructureNet & minusNetCap = compCompCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Minus"));
			const StructRec::StructureNet & plusNetCap = compCompCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Plus"));

			if(getPartitioningResult().hasCompensationResistor())
			{
				Partitioning::ResistorPart & compRes = getPartitioningResult().getCompensationResistor();
				Partitioning::Component & compCompRes = **getPartitioningResult().getBelongingComponents(compRes).begin();
				if(compCompRes.getArray().getStructureName().toStr() == "ResistorArray")
				{
					const StructRec::StructureNet & minusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("ResistorArray","Minus"));
					const StructRec::StructureNet & plusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("ResistorArray","Plus"));
					if(minusNetRes.getIdentifier() == minusNetCap.getIdentifier())
					{
						plusNet = & plusNetRes;
						minusNet = & plusNetCap;
					}
					else if(minusNetRes.getIdentifier() == plusNetCap.getIdentifier())
					{
						plusNet = & plusNetRes;
						minusNet = & minusNetCap;
					}
					else if(plusNetRes.getIdentifier() == plusNetCap.getIdentifier())
					{
						plusNet = & minusNetRes;
						minusNet = & minusNetCap;
					}
					else
					{
						plusNet = & minusNetRes;
						minusNet = & plusNetCap;
					}
				}
				else
				{
					const StructRec::StructureNet & minusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Drain"));
					const StructRec::StructureNet & plusNetRes = compCompRes.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Source"));
					if(minusNetRes.getIdentifier() == minusNetCap.getIdentifier())
					{
						plusNet = & plusNetRes;
						minusNet = & plusNetCap;
					}
					else
					{
						plusNet = & plusNetRes;
						minusNet = & minusNetCap;
					}
				}
			}
			else
			{
				minusNet = & minusNetCap;
				plusNet = & plusNetCap;
			}

			if((stage1.isFirstStage()) && stage1.hasHelperStructure())
			{
				Partitioning::Component & diffPair1Comp = **getPartitioningResult().getBelongingComponents(stage1).begin();
				Partitioning::Component & diffPair2Comp = **std::next(getPartitioningResult().getBelongingComponents(stage1).begin());
				const StructRec::StructureNet & outputNetDiffPair1 =  diffPair1Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				const StructRec::StructureNet & outputNetDiffPair2 =  diffPair2Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));


				isConnected = (isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet, stage2))
						|| (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet, stage2))
						|| (outputNetDiffPair1.getIdentifier() == minusNet->getIdentifier() && (isOutputNetStage(*plusNet, stage2)))
						|| (outputNetDiffPair1.getIdentifier() == plusNet->getIdentifier() && (isOutputNetStage(*minusNet, stage2)))
						|| (outputNetDiffPair2.getIdentifier() == minusNet->getIdentifier() && (isOutputNetStage(*plusNet, stage2)))
						|| (outputNetDiffPair2.getIdentifier() == plusNet->getIdentifier() && (isOutputNetStage(*minusNet, stage2)));

			}
			else if((stage2.isFirstStage()) && stage2.hasHelperStructure())
			{
				Partitioning::Component & diffPair1Comp = **getPartitioningResult().getBelongingComponents(stage2).begin();
				Partitioning::Component & diffPair2Comp = **std::next(getPartitioningResult().getBelongingComponents(stage2).begin());
				const StructRec::StructureNet & outputNetDiffPair1 =  diffPair1Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				const StructRec::StructureNet & outputNetDiffPair2 =  diffPair2Comp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));


				isConnected = (isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet,stage2))
							|| (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet,stage2))
							||	(isOutputNetStage(*minusNet, stage1) && outputNetDiffPair1.getIdentifier() == plusNet->getIdentifier())
							|| (isOutputNetStage(*plusNet, stage1) && outputNetDiffPair1.getIdentifier() == minusNet->getIdentifier())
							||	(isOutputNetStage(*minusNet, stage1) && outputNetDiffPair2.getIdentifier() == plusNet->getIdentifier())
							|| (isOutputNetStage(*plusNet, stage1) && outputNetDiffPair2.getIdentifier() == minusNet->getIdentifier());

			}
			else
			{
				isConnected = ((isOutputNetStage(*minusNet, stage1) && isOutputNetStage(*plusNet, stage2))
						|| (isOutputNetStage(*plusNet, stage1) && isOutputNetStage(*minusNet, stage2)));
			}
			if(isConnected)
				return  isConnected;
			}
			return isConnected;

		}

		std::vector<const StructRec::StructureNet*>  CircuitSpecificationsConstraints::findOutputNetsFirstStage() const
		{
			std::vector<const StructRec::StructureNet *> outputNets;
			if(getPartitioningResult().hasSecondStage())
			{
				Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
				std::vector<Partitioning::Component *> compsSecondStage = getPartitioningResult().getBelongingComponents(secondStage);
				for(auto & comp : compsSecondStage)
				{
					if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
					{
						outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")));
					}

				}
			}
			else
			{
				if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
				{
					outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId()));
					outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId()));
				}
				else
				{
					outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId()));
				}
			}
			assert(!outputNets.empty(), "There should be a first stage with at leas one output net");

			return outputNets;
		}

		std::vector<const StructRec::StructureNet*>  CircuitSpecificationsConstraints::findOutputNets(Partitioning::TransconductancePart & stage) const
		{
			assert(stage.isFirstStage() || stage.isPrimarySecondStage() || stage.isThirdStage() || stage.isFeedBack(), "Transconductance might only be a helper Structure");

			std::vector<const StructRec::StructureNet *> outputNets;

			if((stage.isPrimarySecondStage() || stage.isThirdStage()) && getPartitioningResult().getBelongingComponents(stage).size() > 1)
			{
				std::vector<Partitioning::Component *> compsStage = getPartitioningResult().getBelongingComponents(stage);
				for(auto & comp : compsStage)
				{
						outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")));
				}
			}
			else if(stage.isFirstStage())
			{
				if(getPartitioningResult().hasSecondStage())
				{
					Partitioning::TransconductancePart & secondStage = getPartitioningResult().getPrimarySecondStage();
					std::vector<Partitioning::Component *> compsSecondStage = getPartitioningResult().getBelongingComponents(secondStage);
					for(auto & comp : compsSecondStage)
					{
						if(comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
						{
							outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")));
							break;
						}

					}
					assert(!outputNets.empty(), "There should be at least one transistor in the second stage connected to supply");
				}
				else
				{

					if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
					{
						outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinMinus().getNetId()));
						outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPinPlus().getNetId()));
					}
					else
					{
						outputNets.push_back(&getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(getCircuitInformation().getCircuitParameter().getOutputPin().getNetId()));
					}
				}

			}
			else if(stage.isFeedBack())
			{
				bool foundNet = false;
				for(auto &loadPart : stage.getLoadPart())
				{
					for(auto & loadComp : getPartitioningResult().getBelongingComponents(*loadPart))
					{
						//We are for the gate net of a diode array -> because of gate drain connection is called drain.
						const StructRec::StructureNet & gateNet = loadComp->getArray().findNet(StructRec::StructurePinType(loadComp->getArray().getStructureName(),"Drain"));
						std::vector<const StructRec::Structure*> gateConnectedStructures = gateNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
						for(auto & gateConnectedStruct : gateConnectedStructures)
						{
							if(getPartitioningResult().isLoadPartOfFirstStage(getPartitioningResult().getPart(*gateConnectedStruct)))
							{
								outputNets.push_back(&gateNet);
								foundNet = true;
								break;
							}
						}
						if(foundNet)
							break;
					}
					if(foundNet)
						break;
				}

				assert(foundNet, "There should be a direct net connection between the load of the feedback circuit and the load of the first stage.");
			}
			else
			{
				Partitioning::Component & compStage = **getPartitioningResult().getBelongingComponents(stage).begin();
				outputNets.push_back(&compStage.getArray().findNet(StructRec::StructurePinType(compStage.getArray().getStructureName(), "Drain")));
			}
			return outputNets;
		}

		bool CircuitSpecificationsConstraints::isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const
		{
			bool isIt = false;

			std::vector<const StructRec::StructureNet* > outputNets = findOutputNets(stage);
			for(auto & outputNet : outputNets)
			{
				if(outputNet->getIdentifier() == net.getIdentifier())
				{
					isIt = true;
					break;
				}
			}
			return isIt;
		}

		Gecode::FloatVar CircuitSpecificationsConstraints::computeOutputConductance(Partitioning::Part & part, Partitioning::TransconductancePart & stage)
		{
			 std::vector<Partitioning::Component * > comps = getPartitioningResult().getBelongingComponents(part);
			 if(part.isTransconductancePart() && stage.isFirstStage())
			 {
				 Gecode::FloatVar gdTran1 = computeOutputConductance(**comps.begin());
				 Gecode::FloatVar gdTran2 = computeOutputConductance(**(std::next(comps.begin())));
				 Gecode::rel(getSpace(), gdTran1 == gdTran2);

			 	 return gdTran1;
			 }
			 else
			 {
				Partitioning::Component * outputComp1 =nullptr;
				Partitioning::Component * outputComp2 =nullptr;
				Partitioning::Component * supplyComp1 =nullptr;
				Partitioning::Component * supplyComp2 =nullptr;
				Partitioning::Component * supplyComp3 =nullptr;
				Partitioning::Component * supplyComp4 =nullptr;

				for(auto & comp : comps)
				{
					if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							&& supplyComp1 == nullptr)
					{
						supplyComp1 = comp;
					}
					else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							&& supplyComp2 == nullptr)
					{
						supplyComp2 = comp;
					}
					else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							&& supplyComp3 == nullptr)
					{
						supplyComp3 = comp;
					}
					else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() )
					{
						supplyComp4 = comp;
					}
					else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")), stage) && outputComp1 == nullptr)
					{
						outputComp1 = comp;
					}
					else
					{
						outputComp2 = comp;
					}
				}
				assert(supplyComp1 != nullptr ||(outputComp1 != nullptr && outputComp2!= nullptr), "Part should have at least one component connected to supply or two components connected to the output!" );

				if(supplyComp2 == nullptr && outputComp1 == nullptr && outputComp2 ==nullptr)
				{
					return computeOutputConductance(*supplyComp1);
				}
				else if(outputComp1 == nullptr && outputComp2 ==nullptr)
				{
					 Gecode::FloatVar gdComp1 = computeOutputConductance(*supplyComp1);
					 Gecode::FloatVar gdComp2 = computeOutputConductance(*supplyComp2);
					 Gecode::rel(getSpace(), gdComp1 == gdComp2);

					 return gdComp1;
				}
				else if(supplyComp2 == nullptr && outputComp2 ==nullptr)
				{
					Gecode::FloatVar gdSupply = computeOutputConductance(*supplyComp1);
					Gecode::FloatVar gdOutput = computeOutputConductance(*outputComp1);
					Gecode::FloatVar gmOutput = computeTransconductance(*outputComp1);

					return expr(getSpace(), gdSupply * gdOutput/gmOutput);
				}
				else if(supplyComp1 == nullptr && supplyComp2 == nullptr)
				{
					Gecode::FloatVar gdTran = computeOutputConductance(**getPartitioningResult().getBelongingComponents(stage).begin());
					dom(getSpace(), gdTran, 0, 1);

					Gecode::FloatVar gdOutput = computeOutputConductance(*outputComp1);
					 dom(getSpace(), gdOutput, 0, 1);
					Gecode::FloatVar gmOutput = computeTransconductance(*outputComp1);
					 dom(getSpace(), gmOutput, 0, 1);

					return expr(getSpace(), gdTran * gdOutput/gmOutput);
				}
				else if(outputComp2 ==nullptr)
				{
					Gecode::FloatVar gdSupply1 = computeOutputConductance(*supplyComp1);
					Gecode::FloatVar gdSupply2 = computeOutputConductance(*supplyComp2);
					Gecode::FloatVar gdOutput = computeOutputConductance(*outputComp1);
					Gecode::FloatVar gmOutput = computeTransconductance(*outputComp1);

					rel(getSpace(), gdSupply1 == gdSupply2);

					return expr(getSpace(), gdSupply1 * gdOutput/gmOutput);
				}
				else
				{
					if(part.isLoadPart() && (getPartitioningResult().getLoadPart(**part.getMainStructures().begin()).hasCascodedPair() || stage.isComplementary()))
					{
						Gecode::FloatVar gmCascodedPair = computeTransconductance(*outputComp1);
						 dom(getSpace(), gmCascodedPair, 0, 1);

						Gecode::FloatVar gdCascodedPair = computeOutputConductance(*outputComp1);
						dom(getSpace(), gdCascodedPair, 0, 1);

						Gecode::FloatVar gdBias1(getSpace(), 0, 100000000);
						if(supplyComp3 == nullptr && supplyComp4 == nullptr)
						{
							gdBias1 = computeOutputConductance(*supplyComp1);
						}
						else
						{
							for(auto & comp : comps)
							{
								const StructRec::StructureNet & gateNetLoad = comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));

								if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
										&& !hasComponentInFeedback(gateNetLoad))
								{
									gdBias1 = computeOutputConductance(*comp);
								}
							}
						}


						Partitioning::Component * compFirstStage = nullptr;
						if(stage.isComplementary())
						{
							for(auto & comp : getPartitioningResult().getBelongingComponents(stage))
							{
								if(comp->getArray().getTechType() != outputComp1->getArray().getTechType())
								{
									compFirstStage = comp;
									break;
								}
							}

							assert(compFirstStage != nullptr, "There should be a device in the first stage with opposite tech type as the load");
						}
						else
						{
							compFirstStage = *getPartitioningResult().getBelongingComponents(stage).begin();
						}
						Gecode::FloatVar gdFT = computeOutputConductance(*compFirstStage);
						dom(getSpace(), gdFT, 0, 1);

						return Gecode::expr(getSpace(),  gdCascodedPair* (gdFT + gdBias1)/ gmCascodedPair );

					}
					else
					{
						Gecode::FloatVar gdSupply1 = computeOutputConductance(*supplyComp1);
						 dom(getSpace(), gdSupply1, 0, 1);
						Gecode::FloatVar gdSupply2 = computeOutputConductance(*supplyComp2);
						 dom(getSpace(), gdSupply2, 0, 1);
						Gecode::FloatVar gdOutput = computeOutputConductance(*outputComp1);
						 dom(getSpace(), gdOutput, 0, 1);
						Gecode::FloatVar gmOutput = computeTransconductance(*outputComp1);
						 dom(getSpace(), gmOutput, 0, 1);

						rel(getSpace(), gdSupply1 == gdSupply2);


						return expr(getSpace(), gdSupply1 * gdOutput/gmOutput);
					}
				}
			 }
		}


	std::vector<Path> CircuitSpecificationsConstraints::findShortestPaths(std::vector<Path> paths)
	{
		unsigned int shortest = 100000000;
		std::vector<Path> shortestPaths;
		for(auto & path : paths)
		{
			if(path.getEdges().size() < shortest)
			{
				shortest = path.getEdges().size();
				std::vector<Path> newVector;
				newVector.push_back(path);
				shortestPaths = newVector;
			}
			else if(path.getEdges().size() == shortest)
			{
				shortestPaths.push_back(path);
			}
		}
		return shortestPaths;
	}

	std::vector<Partitioning::Component * > CircuitSpecificationsConstraints::computeInputToSupplyRailPathOverBias(Partitioning::TransconductancePart & stage)
	{
		assert(stage.isFirstStage() || stage.isFeedBack(), "the Input stage must be either a first stage or a feedback stage");
		std::vector<Partitioning::Component * > path;

		path.push_back(*getPartitioningResult().getBelongingComponents(stage).begin());

		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();

		for(auto & compBias : getPartitioningResult().getBelongingComponents(biasPart))
		{
			path.push_back(compBias);
		}

		return path;
	}

	std::vector<Partitioning::Component * > CircuitSpecificationsConstraints::computeInputToSupplyRailPathOverLoad(Partitioning::TransconductancePart & stage)
	{
		assert(stage.isFirstStage() || stage.isFeedBack(), "the Input stage must be either a first stage or a feedback stage");
		std::vector<Partitioning::Component * > path;

		path.push_back(*getPartitioningResult().getBelongingComponents(stage).begin());

		std::vector<Partitioning::LoadPart *> loadParts = stage.getLoadPart();

		if(getPartitioningResult().hasBiasOfFoldedPair())
		{
			path.push_back(&getPartitioningResult().getBiasOfFoldedPair());
			return path;
		}
		else if(stage.hasHelperStructure())
		{
			path.push_back(*getPartitioningResult().findComponents(stage.getHelperStructure()).begin());
		}

		for(auto & loadPart : loadParts)
		{
			if((**loadPart->getMainStructures().begin()).getTechType() !=
					(**stage.getMainStructures().begin()).getTechType())
			{
				Partitioning::Component * outputComp1 =nullptr;
				Partitioning::Component * outputComp2 =nullptr;
				Partitioning::Component * supplyComp1 =nullptr;
				Partitioning::Component * supplyComp2 =nullptr;
				for(auto & comp : getPartitioningResult().getBelongingComponents(*loadPart))
				{
					if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
							&& supplyComp1 == nullptr)
					{
						supplyComp1 = comp;
					}
					else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() )
					{
						supplyComp2 = comp;
					}
					else if(isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")), stage)
							 && outputComp1 == nullptr)
					{
						outputComp1 = comp;
					}
					else
					{
						outputComp2 = comp;
					}
				}
				if(supplyComp1 != nullptr && supplyComp1->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					path.push_back(supplyComp1);
				}
				else if(supplyComp2 != nullptr && supplyComp2->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					path.push_back(supplyComp2);
				}
				else if(supplyComp1 != nullptr && supplyComp1->getArray().getStructureName().toStr() == "MosfetNormalArray"
					&&	( (outputComp1 != nullptr && supplyComp1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					== outputComp1->getArray().findNet(StructRec::StructurePinType(outputComp1->getArray().getStructureName(), "Drain")).getIdentifier())
							|| (outputComp2 != nullptr && supplyComp1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					== outputComp2->getArray().findNet(StructRec::StructurePinType(outputComp2->getArray().getStructureName(), "Drain")).getIdentifier()) ))
				{
					path.push_back(supplyComp1);
					return path;
				}
				else if(supplyComp2 != nullptr && supplyComp2->getArray().getStructureName().toStr() == "MosfetNormalArray"
					&&	( (outputComp1 != nullptr && supplyComp2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					== outputComp1->getArray().findNet(StructRec::StructurePinType(outputComp1->getArray().getStructureName(), "Drain")).getIdentifier())
							|| (outputComp2 != nullptr && supplyComp2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
					== outputComp2->getArray().findNet(StructRec::StructurePinType(outputComp2->getArray().getStructureName(), "Drain")).getIdentifier()) ))
				{
					path.push_back(supplyComp2);
					return path;
				}
				else
				{
					path.push_back(supplyComp1);
				}
				if(outputComp1 != nullptr && outputComp1->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					path.push_back(outputComp1);
				}
				else if(outputComp2 != nullptr && outputComp2->getArray().getStructureName().toStr() == "MosfetDiodeArray")
				{
					path.push_back(outputComp2);

				}
				else if(outputComp1 != nullptr)
				{
					path.push_back(outputComp1);
				}
				return path;


			}
		}
		return path;
	}

	bool CircuitSpecificationsConstraints::hasCompensationCapacity(const StructRec::StructureNet & net) const
	{
		bool hasIt = false;

		if(getPartitioningResult().hasCompensationCapacitance())
		{
			for(auto & structure : net.getAllConnectedStructures())
			{
				Partitioning::Component & comp =getPartitioningResult().findComponent(structure->getIdentifier());
				if(comp.getPart().isCapacitancePart() && getPartitioningResult().getCapacitancePart(*structure).isCompensationCapacitance())
				{
					hasIt = true;
				}
			}
		}
		return hasIt;
	}

	bool CircuitSpecificationsConstraints::hasComponentInFeedback(const StructRec::StructureNet & net) const
	{
		bool hasIt = false;
		const StructRec::StructureNet & arrayNet = net.findArrayNet();

		for(auto & structure : arrayNet.getAllConnectedStructures())
		{
			Partitioning::Component & comp =getPartitioningResult().findComponent(structure->getIdentifier());
			if(getPartitioningResult().isInFeedback(comp))
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	bool CircuitSpecificationsConstraints::hasOutputNetWithCapacitor(Partitioning::TransconductancePart & stage) const
	{
		const StructRec::StructureNet * outputNet = nullptr;
		for(auto & net: findOutputNets(stage))
		{
			if(!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				||!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			{
				outputNet = net;
				break;
			}
		}
		return outputNet != nullptr;
	};

	const StructRec::StructureNet & CircuitSpecificationsConstraints::findOutputNetWithCapacitor(Partitioning::TransconductancePart & stage)
	{
		assert(hasOutputNetWithCapacitor(stage) , "No output net has an capacitor!");

		const StructRec::StructureNet * outputNet = nullptr;
		for(auto & net: findOutputNets(stage))
		{
			if(!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Minus")).empty()
				||!net->findConnectedStructures(StructRec::StructurePinType("CapacitorArray", "Plus")).empty())
			{
				outputNet = net;
				break;
			}
		}
		return *outputNet;


	}

}





