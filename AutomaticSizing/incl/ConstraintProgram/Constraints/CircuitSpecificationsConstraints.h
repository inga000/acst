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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_CIRCUITSPECIFICATIONSCONSTRAINTS_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_CIRCUITSPECIFICATIONSCONSTRAINTS_H_

#include <gecode/float.hh>

#include "Partitioning/incl/Results/Component.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"

namespace StructRec {

	 class StructureCircuits;
}


namespace Partitioning {

	class Result;
	class LoadPart;
	class BiasPart;
	class TransconductancePart;

}

namespace AutomaticSizing {


	class CircuitInformation;
	class TechnologieSpecification;
	class Graph;
	class NetToIntVarMap;
	class ComponentToIntVarMap;
	class SearchSpace;
	class Path;
	class Edge;
	class PolesAndZeros;

	class CircuitSpecificationsConstraints
	{
	public:
		CircuitSpecificationsConstraints();

		void setPartitioningResult(const Partitioning::Result & result);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & circuits);
		void setGraph(const Graph & graph);
		void setCircuitInformation(const CircuitInformation & information);
		void setTwoPortToValueMap(ComponentToIntVarMap & valueMap );
		void setTransistorToWidthMap(ComponentToIntVarMap & widthMap );
		void setTransistorToLengthMap(ComponentToIntVarMap & lenghtMap);
		void setTransistorToCurrentMap(ComponentToIntVarMap & currentMap);
		void setNetToVoltageMap(NetToIntVarMap & voltageMap);
		void setSpace(SearchSpace & space);
		void setTransistorModel(std::string model);

		void createConstraints();
		void createOptimizingConstraints();


	private:
		const Partitioning::Result & getPartitioningResult() const;
		const Graph & getGraph() const;
		const CircuitInformation & getCircuitInformation() const;
		const ComponentToIntVarMap & getTransistorToWidthMap() const;
		const ComponentToIntVarMap & getTransistorToLengthMap() const;
		const ComponentToIntVarMap & getTransistorToCurrentMap() const;
		const ComponentToIntVarMap & getTwoPortToValueMap() const;
		const NetToIntVarMap & getNetToVoltageMap() const;
		SearchSpace & getSpace() const;
		std::string getTransistorModel() const;
		const PolesAndZeros& getPolesAndZeros() const;
		const StructRec::StructureCircuits&  getStructureRecognitionResult() const;

		Gecode::FloatVar calculateGain();
		void createGainConstraint(Gecode::FloatVar gain);
		Gecode::FloatVar calculatePowerConsumption();
		void createPowerConsumptionConstraint(Gecode::FloatVar powerConsumption);
		Gecode::FloatVar calculateSlewRate();
		void createSlewRateConstraint(Gecode::FloatVar slewRate);
		Gecode::FloatVar calculateTransientFrequency();
		void createTransientFrequencyConstraint(Gecode::FloatVar transitFrequency);
		Gecode::FloatVar calculateTransientFrequencyWithErrorFactor();
		void createTransientFrequencyConstraintWithErrorFactor(Gecode::FloatVar transitFrequency);
		std::tuple<Gecode::FloatVar, Gecode::FloatVar> calculateOutputVoltageSwing();
		void createOutputVoltageSwingConstraint(Gecode::FloatVar voutMin, Gecode::FloatVar voutMax);
		std::tuple<Gecode::FloatVar, Gecode::FloatVar> calculateCommonModeInputVoltage();
		void createCommonModeInputVoltageConstraint(Gecode::FloatVar vcmMin, Gecode::FloatVar vcmMax);
		std::tuple<Gecode::FloatVar, Gecode::FloatVar> calculatePSRR();
		void createPSRRConstraint(Gecode::FloatVar posPSRR, Gecode::FloatVar negPSRR);
		Gecode::FloatVar calculateCMRR();
		void createCMRRConstraint(Gecode::FloatVar CMRR);
		Gecode::FloatVar calculateArea();
		void createAreaConstraint(Gecode::FloatVar area);
		Gecode::FloatVar calculatePhaseMargin();
		void createPhaseMarginConstraint(Gecode::FloatVar phaseMargin);
		void createComplementaryFirstStageConstraint();
		void createScalibiltyFactorLimitationForSymmetricalOTAConstraint();
		Gecode::FloatVar computeScalingFactorForSymmetricalOTA();
		void createCompensationCapacitanceConstraint();

		void createBiasOfCascodedPairCurrentConstraint();
		void createSimpleSecondStageTransconductanceConstraint();
		void createPolesAndZeros();
		void createInnerVoltageBiasOfTelescopicOpAmpsConstraint();

		void createOutputNetVoltageConstraintFirstStage();

		void createFeedbackCircuitConstraints();

		void createFeedbackGainConstraint(Partitioning::TransconductancePart & feedbackStage);
		void createFeedbackCMRRConstraint(Partitioning::TransconductancePart & feedbackStage);
		void createFeedbackTransitFrequencyConstraint(Partitioning::TransconductancePart & feedbackStage);
		void createFeedbackPhaseMarginConstraint(Partitioning::TransconductancePart & feedbackStage);
		void createFeedbackInputVoltageConstraint(Partitioning::TransconductancePart & feedbackStage);
		void createDetectorResistorConstraints();
		void createFeedbackPowerConsumptionConstraint();

		Gecode::FloatVar calculateSlewRateFirstStage();
		Gecode::FloatVar calculateSlewRateSecondStage();
		Gecode::FloatVar calculateSlewRateThirdStage();
		Gecode::FloatVar calculateSlewRate(Partitioning::TransconductancePart & stage );

		Gecode::FloatVar calculateNonDominatPoleSecondStageCompensationCapacitance();
		Gecode::FloatVar calculateNonDominatPoleHigherStagesCompensationCapacitance(
				Partitioning::TransconductancePart & stage);

		Gecode::FloatVar calculateDominantPole();
		Gecode::FloatVar calculateNonDominantPoleFirstStage();
		Gecode::FloatVar calculatePositiveZeroFirstStage();
		Gecode::FloatVar calculateNonDominantPoleHigherStages(
					Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage);
		Gecode::FloatVar calculateAdditionalNonDominantPoleHigherStages(
				Partitioning::TransconductancePart & currentStage, Partitioning::TransconductancePart & previousStage);
		Gecode::FloatVar calculatePositiveZeroWithCompensationCapacity();
		Gecode::FloatVar calculatePositiveZeroSecondStage();
		Gecode::FloatVar calculatePositiveZeroWithCompensationResistance();

		void calculateNonDominantPolesComplementaryFirstStage(PolesAndZeros & polesAndZeros);
		void calculateFirstStageSecondLoadPolesAndZeros(PolesAndZeros & polesAndZeros);
		void createBiasSecondStageNonDominantPolesAndZeros(PolesAndZeros &polesAndZeros);
		void createBiasHigherStagesNonDominantPolesAndZeros(
				Partitioning::TransconductancePart & stage, PolesAndZeros &polesAndZeros);

		Gecode::FloatVar calculateComplexPoleFrequencyThreeStageOpAmp();

		Gecode::FloatVar calculateErrorFactorTransientFrequency();

		Gecode::FloatVar computeGateSourceCapacitance(Partitioning::Component & component) const;
		Gecode::FloatVar computeDrainBulkCapacitance(Partitioning::Component & component) const;
		Gecode::FloatVar computeSourceBulkCapacitance(Partitioning::Component & component) const;
		Gecode::FloatVar computeGateDrainCapacitance(Partitioning::Component & component) const;
		Gecode::FloatVar computeGateBulkCapacitance(Partitioning::Component & component) const;
		Gecode::FloatVar calculateNetCapacitance(const StructRec::StructureNet & net) const;

		Gecode::FloatVar createGainConstraintFirstStage();
		Gecode::FloatVar createGainConstraintSecondStage();
		Gecode::FloatVar createGainConstraintHigherStage(Partitioning::TransconductancePart & higherStage);
		Gecode::FloatVar createGainConstraintThirdStage();
		Gecode::FloatVar createFirstStageGainFirstStageWithDiodeTransistorLoad();
		Gecode::FloatVar computeGainFirstStageErrorFactor();

		Gecode::FloatVar computeTransconductance(Partitioning::Component & transistor) const;
		Gecode::FloatVar computeTransconductance(Partitioning::TransconductancePart & stage);
		Gecode::FloatVar computeOutputConductance(Partitioning::Component & transistor);
		Gecode::FloatVar computeOutputConductance(Partitioning::Part & part, Partitioning::TransconductancePart & stage);

		Gecode::FloatVar computeOutputResistanceFirstStage();
		Gecode::FloatVar computeOutputResistanceHigherStage(Partitioning::TransconductancePart & higherStage);
		Gecode::FloatVar computeOutputResistance(Partitioning::TransconductancePart & stage);



		Gecode::FloatVar computeEdgeCurrent(Edge & edge);
		float computeSign(const Edge & edge, const Path & path) const;
		Gecode::FloatVar computeEdgeVoltage(const Edge & edge);

		Gecode::FloatVar getOutputCurrent();

		bool isOutputNetFirstStage(Core::NetId outputNet) const;
		bool isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const;
		bool compensationCapacityConnectedBetween(Partitioning::TransconductancePart &stage2, Partitioning::TransconductancePart &stage1) const;
		bool hasCompensationCapacity(const StructRec::StructureNet & net) const;
		bool hasComponentInFeedback(const StructRec::StructureNet & net) const;
		bool hasOutputNetWithCapacitor(Partitioning::TransconductancePart & stage) const;

		const StructRec::StructureNet & findOutputNetWithCapacitor(Partitioning::TransconductancePart & stage);

		std::vector<const StructRec::StructureNet*>  findOutputNetsFirstStage() const;
		std::vector<const StructRec::StructureNet*>  findOutputNets(Partitioning::TransconductancePart & stage) const;

		std::vector<Path> findShortestPaths(std::vector<Path> paths);
		std::vector<Partitioning::Component * > computeInputToSupplyRailPathOverBias(Partitioning::TransconductancePart & stage);
		std::vector<Partitioning::Component * > computeInputToSupplyRailPathOverLoad(Partitioning::TransconductancePart & stage);

	private:
		const Partitioning::Result * partitioningResult_;
		const StructRec::StructureCircuits* structRecResult_;
		const PolesAndZeros * polesAndZeros_;
		const Graph * graph_;
		const CircuitInformation * circuitInformation_;
		ComponentToIntVarMap * transistorToWidthMap_;
		ComponentToIntVarMap* transistorToLengthMap_;
		ComponentToIntVarMap * transistorToCurrentMap_;
		ComponentToIntVarMap * twoPortToValueMap_;
		NetToIntVarMap * netToVoltageMap_;
		SearchSpace * space_;
		std::string transistorModel_;

	};



}



#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_CONSTRAINTS_CIRCUITSPECIFICATIONSCONSTRAINTS_H_ */


