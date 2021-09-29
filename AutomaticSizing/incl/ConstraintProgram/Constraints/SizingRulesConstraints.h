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

#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_SIZINGRULESCONSTRAINTS_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_CONSTRAINTS_SIZINGRULESCONSTRAINTS_H_

#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "AutomaticSizing/incl/ConstraintProgram/EqualDimensionMap.h"

#include "gecode/minimodel.hh"

namespace Partitioning {

	class Result;
	class BiasPart;
	class TransconductancePart;

}

namespace AutomaticSizing {


	class ComponentToIntVarMap;
	class SearchSpace;
	class CircuitInformation;

	class SizingRulesConstraints
	{
	public:
		SizingRulesConstraints();

		void initialize();

		void setPartitioningResult(const Partitioning::Result & result);
		void setStructureRecognitionResult(const StructRec::StructureCircuits & structRecResult);
		void setCircuitInformation(const CircuitInformation & information);
		void setTransistorToWidthMap(ComponentToIntVarMap & widthMap);
		void setTransistorToLengthMap(ComponentToIntVarMap & lengthMap);
		void setTransistorToCurrentMap(ComponentToIntVarMap & currentMap);
		void setTwoPortToValueMap(ComponentToIntVarMap & twoPortMap);
		void setSpace(SearchSpace & space);

		void createMaps();
		void createConstraints();

		bool isInitialized() const;

		EqualDimensionMap getEqualWidthMap() const;
		EqualDimensionMap getEqualLengthMap() const;

	private:
		const Partitioning::Result & getPartitioningResult() const;
		const StructRec::StructureCircuits & getStructureRecognitionResult() const;
		const CircuitInformation & getCircuitInformation() const;
		ComponentToIntVarMap & getTransistorToWidthMap();
		ComponentToIntVarMap & getTransistorToLengthMap();
		ComponentToIntVarMap & getTransistorToCurrentMap();
		ComponentToIntVarMap & getTwoPortToValueMap();
		SearchSpace & getSpace();

		void createSimpleCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createCascodeCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createFourTransistorCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createImprovedWilsonCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createWideSwingCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createWilsonCurrentMirrorConstraints(const StructRec::Structure & structure);
		void createDifferentialPairConstraints(const StructRec::Structure & structure);
		void createDifferentialStageConstraints();
		void createSystematicOffsetConstraints();
		void createCurrentDensityConstraintInFeedbackCircuits();
		void createSymmericalCommonModeSignalDetector();
		void createSymmetricalCompensationCapaciticies();
		void createThreeTransistorFeedbackCircuitConstraint();
		void createMinimalAreaConstraintForSimpleSlewRateEnhancer();
		void createCurrentConstraintForSimpleSlewRateEnhancer();


		void addSimpleCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addCrossCoupledPairConstraintsToMap(const StructRec::Structure & structure);
		void addCascodeCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addFourTransistorCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addImprovedWilsonCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addWideSwingCascodeCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addWideSwingCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addWilsonCurrentMirrorConstraintsToMap(const StructRec::Structure & structure);
		void addDifferentialPairConstraintsToMap(const StructRec::Structure & structure);
		void addDifferentialStageConstraintsToMap();
		void addEqualLengthVoltageCurrentBiasToMap();
		void addEqualLengthMainBiasCurrentBiasToMap();
		void addSymmetryIn2TransistorCurrentMirrorPartsToMap(std::vector<Partitioning::BiasPart*> currentMirrorParts);
		void addSymmetricalSecondStageConstraintToMap();
		void addSizingRulesdOfCMFBCircuitsToMap();
		void addSymmetricalCompensationResistorToMap();
		void addSymmetricalPositiveFeedbackPartConstraintsToMap();
		void addEqualLengthBiasStagesToMap();




		bool isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const;
		std::vector<const StructRec::StructureNet*> findOutputNets(Partitioning::TransconductancePart & stage) const;

	private:
		const 	Partitioning::Result * partitioningResult_;
		const StructRec::StructureCircuits * structRecResult_;
		ComponentToIntVarMap * transistorToWidthMap_;
		ComponentToIntVarMap* transistorToLengthMap_;
		ComponentToIntVarMap * transistorToCurrentMap_;
		ComponentToIntVarMap * twoPortToValueMap_;
		SearchSpace * space_;
		const CircuitInformation * circuitInformation_;

		EqualDimensionMap equalLengthMap_;
		EqualDimensionMap equalWidthMap_;

		bool isInitialized_;

	};




}


#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_CONSTRAINTS_INTEGERSPECIFICATIONBASEDSIZINGRULESCONSTRAINTS_H_ */
