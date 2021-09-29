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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_SEARCHSPACE_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_SEARCHSPACE_H_

#include "AutomaticSizing/incl/ConstraintProgram/NetToIntVarMap.h"

#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"

#include <gecode/int.hh>
#include <gecode/float.hh>
#include <gecode/minimodel.hh>
#include "ComponentToIntVarMap.h"
#include "Constraints/SizingRulesConstraints.h"

namespace Partitioning{

	class Result;
	class TransconductancePart;

}

namespace AutomaticSizing {

	class Variables;
	class CircuitInformation;
	class Graph;
	class Result;
	class Pin;
	class Node;
	class Edge;
	class PolesAndZeros;
	class Transistor;

	class SearchSpace : public Gecode::Space
	{
	public:
		SearchSpace();
		~SearchSpace();
		SearchSpace(SearchSpace & other);

		void setResult(const Partitioning::Result & result);
		void setGraph(const Graph & graph);
		void setCircuitInformation(const CircuitInformation & information);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & structRecResult);
		void setTransistorModel(std::string transistorModel);
		void setDefinition(std::string definition);
		void setPolesAndZeros(PolesAndZeros &polesAndZeros);
		void setUsesHSpiceLibrary(bool usesIt);

		void setGain(Gecode::FloatVar gain);
		void setFeedbackGain(Gecode::FloatVar feedBackGain);
		void setPowerConsumption(Gecode::FloatVar power);
		void setPowerConsumptionOpAmp(Gecode::FloatVar power);
		void setPowerConsumptionFeedback(Gecode::FloatVar power);
		void setTransitFrequency(Gecode::FloatVar ft);
		void setTransitFrequencyWithErrorFactor(Gecode::FloatVar ft);
		void setFeedbackTransitFrequency(Gecode::FloatVar ft);
		void setSlewRate(Gecode::FloatVar slewRate);
		void setCMRR(Gecode::FloatVar CMRR);
		void setFeedbackCMRR(Gecode::FloatVar CMRR);
		void setPosPSRR(Gecode::FloatVar posPSRR);
		void setNegPSRR(Gecode::FloatVar negPSRR);
		void setMaxOutputVoltageSwing(Gecode::FloatVar voutMax);
		void setMinOutputVoltageSwing(Gecode::FloatVar voutMin);
		void setArea(Gecode::FloatVar area);
		void setPhaseMargin(Gecode::FloatVar phaseMargin);
		void setFeedbackPhaseMargin(Gecode::FloatVar feedBackPhaseMargin);
		void setSettlingTime(Gecode::FloatVar settlingTime);
		void setMaximumOutputVoltage(Gecode::FloatVar voutMax);
		void setMinimumOutputVoltage(Gecode::FloatVar voutMin);
		void setMinimumCommonModeInputVoltage(Gecode::FloatVar vcmMin);
		void setMaximumCommonModeInputVoltage(Gecode::FloatVar vcmMax);
		void setFeedbackMinimumInputVoltage(Gecode::FloatVar vinMin);
		void setFeedbackMaximumInputVoltage(Gecode::FloatVar vinMax);
		void setHelper(Gecode::FloatVar helper);


		Gecode::FloatVar getTransitFrequencyVariable();
		Gecode::FloatVar getFeedbackTransitFrequencyVariable();
		Gecode::FloatVar getFeedbackGainVariable();

		const Result &  computeResult();

		void initialize();
		void initializeIntegerPolesAndZeros();

		virtual SearchSpace * copy() = 0;

		virtual void constrain(const Gecode::Space& other) = 0;
		virtual bool master(const Gecode::MetaInfo & mi);

		std::string toStr() const;
		std::string printIndexToVariableMap() const;

		std::string getDefinition() const;
		int getLengthUpperBound() const;
		int getWidthUpperBound() const;
		float getScalingFactorMUM() const;


		float getGain() const;
		float getBestCircuitPerformance() const;
		float getPowerConsumption() const;
		float getTransitFrequency() const;
		float getTransitFrequencyWithErrorFactor() const;
		float getPhaseMargin() const;
		float getArea() const;
		float getSlewRate() const;
		float getCMRR() const;
		float getNegPSRR() const;
		float getPosPSRR() const;
		float getSettlingTime() const;
		float getMaxOutputVoltage() const;
		float getMinOutputVoltage() const;
		float getMaxCommonModeInputVoltage() const;
		float getMinCommonModeInputVoltage() const;
		const PolesAndZeros & getPolesAndZeros() const;
		PolesAndZeros & getPolesAndZeros();

		Gecode::FloatVar createChanneledFloatVar(Gecode::IntVar integer, long int lb, long int ub);
		Gecode::FloatVar createFloatWidth(Gecode::IntVar intWidth);
		Gecode::FloatVar createFloatLength(Gecode::IntVar intLength);
		Gecode::FloatVar createFloatCurrent(Gecode::IntVar intCurrent);
		Gecode::FloatVar createFloatVoltage(Gecode::IntVar intVoltage);

		void print() const;
		void eraseVariables() const;


	protected:
		void createMaps();

		const CircuitInformation & getCircuitInformation() const;
		const Graph & getGraph() const;
		const Partitioning::Result & getPartitioningResult() const;
		const StructRec::StructureCircuits & getStructureRecognitionResult() const;
		SizingRulesConstraints & getSizingRulesConstraints();
		std::string getTransistorModel() const;

		Variables & getVariables();

		bool fulfillsSpecifications() const;
		bool fulfillsFirstStageSpecifications() const;

	private:
		void initializeBranching();
		void initializeCost();
		void initializeScalingValues();
		void initializeTransistorMap();
		void initializeVariableMap();

		void createVariables();
		void initializePolesAndZeros();

		void createIndexes();
		void createTransistorVariableMaps();
		void createTransistorVariableIndexMaps(int & index);
		void createWidthMap();
		void createLengthMap();
		void createWidthIndexMap(int & index);
		void createLengthIndexMap(int & index);
		void createTwoPortValueMap();
		void createTwoPortIndexMap(int & index);
		void createResistorToValueMap();
		void createVoltageMap();
		void createCurrentMap();
		void createCurrentIndexMap(int & index);
		void createVoltageIndexMap(int & index);
		void createPolesAndZerosIndexVectors(int & index);
		void createSpecificationIndexMap(int & index);

		void createConstraints();
		void createDomainConstraints();
		void createSizingConstraints();
		void createLoadCapacityConstraint();
		void createVoltageConstraints();
		void createKCLConstraints();
		virtual void createCircuitSpecificationConstraints() = 0;
		void createSizingRulesConstraints();
		void createTransistorConstraints();

		void createVoltageConstraints(const Pin & pins);
		void createOutputVoltageConstraint();
		void createStageBiasVoltagePinsConstraints();


		void branchComponent(Partitioning::Component & component);
		void branchBiasInputTransistor();
		bool hasCircuitInformation() const;
		bool hasGraph() const;
		bool hasResult() const;

		const Variables & getVariables() const;


		bool isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const;
		std::vector<const StructRec::StructureNet*> findOutputNets(Partitioning::TransconductancePart & stage) const;
		bool usesHSpiceLibrary() const;

		void setNetIndexOfTransistors(Core::NetId netId, int & index);



		//only for testing:
		void setWidthAndLength();


	protected:
		static const float NOT_INITIALIZED_;
		static const std::string GAIN_;
		static const std::string FEEDBACK_GAIN_;
		static const std::string POWER_;
		static const std::string POWER_OP_AMP_;
		static const std::string POWER_FEEDBACK_;
		static const std::string TRANSIT_FREQUENCY_;
		static const std::string FEEDBACK_TRANSIT_FREQUENCY_;
		static const std::string TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_;
		static const std::string SLEW_RATE_;
		static const std::string SETTLING_TIME_;
		static const std::string CMRR_;
		static const std::string FEEDBACK_CMRR_;
		static const std::string NEG_PSRR_;
		static const std::string POS_PSRR_;
		static const std::string VOUT_MAX_;
		static const std::string VOUT_MIN_;
		static const std::string VCM_MIN_;
		static const std::string VCM_MAX_;
		static const std::string FEEDBACK_VCM_MIN_;
		static const std::string FEEDBACK_VCM_MAX_;
		static const std::string AREA_;
		static const std::string PHASE_MARGIN_;
		static const std::string FEEDBACK_PHASE_MARGIN_;


	protected:
		PolesAndZeros * polesAndZeros_;
		const Partitioning::Result* result_;
		const StructRec::StructureCircuits* structRecResult_;
		const CircuitInformation * circuitInformation_;
		const Graph * graph_;
		Variables * variablesMap_;

		SizingRulesConstraints sizingRulesConstraints_;
		std::string transistorModel_;
		std::string definition_;


		ComponentToIntVarMap transistorToWidthMap_;
		ComponentToIntVarMap transistorToLengthMap_;
		ComponentToIntVarMap twoPortToValueMap_;
		ComponentToIntVarMap transistorToCurrentMap_;

		NetToIntVarMap netToVoltageMap_;

		int lengthUpperBound_;
		int widthUpperBound_;
		float scalingFactorMUM_;

		bool usesHSpiceLibrary_;


		//Order: Lengths, Widths, Currents, Voltages,
		Gecode::IntVarArray variables_;
		Gecode::FloatVar cost_;

		Gecode::FloatVar helper_;
		std::map<int, std::string> indexToVariableMap_;


	};

}




#endif /* AUTOMATICSIZING_INCL_ANALYSIS_GECODESYNTHESISSPACE_H_ */
