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


#include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"


#include "../../../incl/ConstraintProgram/ComponentToIntVarMap.h"
#include "../../../incl/ConstraintProgram/Constraints/SizingRulesConstraints.h"

namespace AutomaticSizing {

	SizingRulesConstraints::SizingRulesConstraints() :
				partitioningResult_(NULL),
				structRecResult_(NULL),
				transistorToWidthMap_(NULL),
				transistorToLengthMap_(NULL),
				transistorToCurrentMap_(NULL),
				circuitInformation_(NULL),
				space_(NULL),
				isInitialized_(false),
				twoPortToValueMap_(nullptr)
	{
	}

	void SizingRulesConstraints::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	void SizingRulesConstraints::setPartitioningResult(
		const Partitioning::Result& result)
	{
		partitioningResult_ = & result;
	}

	void SizingRulesConstraints::setStructureRecognitionResult(
		const StructRec::StructureCircuits& structRecResult)
	{
		structRecResult_ = & structRecResult;
	}

	void SizingRulesConstraints::initialize()
	{
		isInitialized_ = true;
	}

	void SizingRulesConstraints::setTransistorToWidthMap(
		ComponentToIntVarMap& widthMap)
	{
		transistorToWidthMap_ = & widthMap;
	}

	void SizingRulesConstraints::setTransistorToLengthMap(
		ComponentToIntVarMap& lengthMap)
	{
		transistorToLengthMap_ = & lengthMap;
	}

	void SizingRulesConstraints::setTransistorToCurrentMap(
		ComponentToIntVarMap& currentMap)
	{
		transistorToCurrentMap_ = & currentMap;
	}

	void SizingRulesConstraints::setSpace(
		SearchSpace& space)
	{
		space_ = & space;
	}

	EqualDimensionMap SizingRulesConstraints::getEqualWidthMap() const
	{
		return equalWidthMap_;
	}

	EqualDimensionMap SizingRulesConstraints::getEqualLengthMap() const
	{
		return equalLengthMap_;
	}

	void SizingRulesConstraints::createConstraints()
	{
		std::vector<const StructRec::Structure*> structures = getStructureRecognitionResult().findAllStructures();
		for(auto& it : structures)
		{
			const StructRec::Structure & structure = * it;
			if(structure.getStructureName() == StructRec::StructureName("MosfetSimpleCurrentMirror"))
			{
				createSimpleCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetCascodeCurrentMirror"))
			{
				createCascodeCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetFourTransistorCurrentMirror"))
			{
				createFourTransistorCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetImprovedWilsonCurrentMirror"))
			{
				createImprovedWilsonCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetWideSwingCurrentMirror"))
			{
				createWideSwingCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetWilsonCurrentMirror"))
			{
				createWilsonCurrentMirrorConstraints(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetDifferentialPair"))
			{
				createDifferentialPairConstraints(structure);
			}
		}


		createDifferentialStageConstraints();
		createSystematicOffsetConstraints();
		createSymmericalCommonModeSignalDetector();
		createSymmetricalCompensationCapaciticies();

		createThreeTransistorFeedbackCircuitConstraint();
		createMinimalAreaConstraintForSimpleSlewRateEnhancer();
		createCurrentConstraintForSimpleSlewRateEnhancer();
	}

	void SizingRulesConstraints::createMaps()
	{
		std::vector<const StructRec::Structure*> structures = getStructureRecognitionResult().findAllStructures();
		for(auto& it : structures)
		{
			const StructRec::Structure & structure = * it;

			if(structure.getStructureName() == StructRec::StructureName("MosfetSimpleCurrentMirror"))
			{
				addSimpleCurrentMirrorConstraintsToMap(structure);

			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetCrossCoupledPair"))
			{
				addCrossCoupledPairConstraintsToMap(structure);

			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetCascodeCurrentMirror"))
			{
				addCascodeCurrentMirrorConstraintsToMap(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetFourTransistorCurrentMirror"))
			{
				addFourTransistorCurrentMirrorConstraintsToMap(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetImprovedWilsonCurrentMirror"))
			{
				addImprovedWilsonCurrentMirrorConstraintsToMap(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror"))
			{
				addWideSwingCascodeCurrentMirrorConstraintsToMap(structure);

			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetWideSwingCurrentMirror"))
			{
				addWideSwingCurrentMirrorConstraintsToMap(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetWilsonCurrentMirror"))
			{
				addWilsonCurrentMirrorConstraintsToMap(structure);
			}
			else if(structure.getStructureName() == StructRec::StructureName("MosfetDifferentialPair"))
			{

				addDifferentialPairConstraintsToMap(structure);

			}

		}

		addEqualLengthVoltageCurrentBiasToMap();
		addSymmetryIn2TransistorCurrentMirrorPartsToMap(getPartitioningResult().getAllVoltageBiases());
		addSymmetryIn2TransistorCurrentMirrorPartsToMap(getPartitioningResult().getAllCurrentBiases());
		addDifferentialStageConstraintsToMap();
		addSizingRulesdOfCMFBCircuitsToMap();
		addSymmetricalCompensationResistorToMap();
		addSymmetricalPositiveFeedbackPartConstraintsToMap();
		addEqualLengthBiasStagesToMap();

		if(getPartitioningResult().hasSecondStage() && getPartitioningResult().getSecondStages().size() == 2)
		{
			addSymmetricalSecondStageConstraintToMap();
		}
	}

	const Partitioning::Result& SizingRulesConstraints::getPartitioningResult() const
	{
		assert(partitioningResult_ != NULL);
		return * partitioningResult_;
	}

	const StructRec::StructureCircuits& SizingRulesConstraints::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return* structRecResult_;
	}

	ComponentToIntVarMap& SizingRulesConstraints::getTransistorToWidthMap()
	{
		assert(transistorToWidthMap_ != NULL);
		return * transistorToWidthMap_;
	}

	ComponentToIntVarMap& SizingRulesConstraints::getTransistorToLengthMap()
	{
		assert(transistorToLengthMap_ != NULL);
		return * transistorToLengthMap_;
	}

	ComponentToIntVarMap& SizingRulesConstraints::getTransistorToCurrentMap()
	{
		assert(transistorToCurrentMap_ != NULL);
		return *transistorToCurrentMap_;
	}

	SearchSpace& SizingRulesConstraints::getSpace()
	{
		assert(space_ != NULL);
		return *space_;
	}

	const CircuitInformation& SizingRulesConstraints::getCircuitInformation() const
	{
		assert(circuitInformation_ != NULL);
		return * circuitInformation_;
	}

	void SizingRulesConstraints::createSimpleCurrentMirrorConstraints(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & currentMirror = static_cast<const StructRec::Pair &>(structure);
		Gecode::IntVar widthChild1 = getTransistorToWidthMap().find(currentMirror.getChild1().getIdentifier());
		Gecode::IntVar currentChild1 = getTransistorToCurrentMap().find(currentMirror.getChild1().getIdentifier());
		Gecode::IntVar widthChild2 = getTransistorToWidthMap().find(currentMirror.getChild2().getIdentifier());
		Gecode::IntVar currentChild2 = getTransistorToCurrentMap().find(currentMirror.getChild2().getIdentifier());


		Gecode::FloatVar widthChild1HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthChild1 , widthChild1HelperVar);

		Gecode::FloatVar currentChild1HelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentChild1, currentChild1HelperVar);

		Gecode::FloatVar widthChild2HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthChild2 , widthChild2HelperVar);

		Gecode::FloatVar currentChild2HelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentChild2, currentChild2HelperVar);
	}

	void SizingRulesConstraints::addSimpleCurrentMirrorConstraintsToMap(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & currentMirror = static_cast<const StructRec::Pair &>(structure);
		StructRec::StructureId voltageBias = currentMirror.getChild1().getIdentifier();
		StructRec::StructureId currentBias = currentMirror.getChild2().getIdentifier();

		equalLengthMap_.add(voltageBias, getPartitioningResult().findComponent(currentBias));
	}

	void SizingRulesConstraints::addCrossCoupledPairConstraintsToMap(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & crossCouplePair = static_cast<const StructRec::Pair &>(structure);
		StructRec::StructureId firstChild = crossCouplePair.getChild1().getIdentifier();
		StructRec::StructureId secondChild = crossCouplePair.getChild2().getIdentifier();

		equalLengthMap_.add(firstChild,getPartitioningResult().findComponent(secondChild));
		equalWidthMap_.add(firstChild,getPartitioningResult().findComponent(secondChild));
	}

	void SizingRulesConstraints::createCascodeCurrentMirrorConstraints(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & cascodeCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & diodeStack = static_cast<const StructRec::Pair &>(cascodeCurrentMirror.getChild1());
		const StructRec::Pair & cascodePair = static_cast<const StructRec::Pair &>(cascodeCurrentMirror.getChild2());
		Gecode::IntVar widthLowerDTA = getTransistorToWidthMap().find(diodeStack.getChild2().getIdentifier());
		Gecode::IntVar widthUpperDTA = getTransistorToWidthMap().find(diodeStack.getChild1().getIdentifier());
		Gecode::IntVar widthLowerNTA = getTransistorToWidthMap().find(cascodePair.getChild2().getIdentifier());
		Gecode::IntVar widthUpperNTA = getTransistorToWidthMap().find(cascodePair.getChild1().getIdentifier());

		Gecode::FloatVar widthLowerDTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLowerDTA , widthLowerDTAHelperVar);

		Gecode::FloatVar widthUpperDTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthUpperDTA , widthUpperDTAHelperVar);

		Gecode::FloatVar widthLowerNTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLowerNTA , widthLowerNTAHelperVar);

		Gecode::FloatVar widthUpperNTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthUpperNTA , widthUpperNTAHelperVar);

		Gecode::rel(getSpace(), (widthLowerNTA* widthUpperDTA )/(widthLowerDTA*widthUpperNTA) >= 0.95);
		Gecode::rel(getSpace(), (widthLowerNTA* widthUpperDTA)/(widthLowerDTA *widthUpperNTA ) <= 1.05  );
	}

	void SizingRulesConstraints::addCascodeCurrentMirrorConstraintsToMap(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & cascodeCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & diodeStack = static_cast<const StructRec::Pair &>(cascodeCurrentMirror.getChild1());
		const StructRec::Pair & cascodePair = static_cast<const StructRec::Pair &>(cascodeCurrentMirror.getChild2());
		const StructRec::StructureId upperDTA = diodeStack.getChild1().getIdentifier();
		const StructRec::StructureId lowerDTA = diodeStack.getChild2().getIdentifier();
		const StructRec::StructureId upperNTA = cascodePair.getChild1().getIdentifier();
		const StructRec::StructureId lowerNTA = cascodePair.getChild2().getIdentifier();

		equalLengthMap_.add(upperDTA, getPartitioningResult().findComponent(lowerDTA));
		equalLengthMap_.add(upperDTA, getPartitioningResult().findComponent(lowerNTA));
		equalLengthMap_.add(upperDTA, getPartitioningResult().findComponent(upperNTA));
	}

	void SizingRulesConstraints::createFourTransistorCurrentMirrorConstraints(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & fourTransistorCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & vref1 = static_cast<const StructRec::Pair &>(fourTransistorCurrentMirror.getChild1());
		const StructRec::Pair & currentMirrorLoad = static_cast<const StructRec::Pair &>(fourTransistorCurrentMirror.getChild2());
		Gecode::IntVar widthUpperVref1 = getTransistorToWidthMap().find(vref1.getChild1().getIdentifier());
		Gecode::IntVar currentUpperVref1 = getTransistorToCurrentMap().find(vref1.getChild1().getIdentifier());
		Gecode::IntVar widthUpperCML = getTransistorToWidthMap().find(currentMirrorLoad.getChild1().getIdentifier());
		Gecode::IntVar currentUpperCML = getTransistorToCurrentMap().find(currentMirrorLoad.getChild1().getIdentifier());


		Gecode::FloatVar widthUpperVref1HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthUpperVref1 , widthUpperVref1HelperVar);

		Gecode::FloatVar currentUpperVref1HelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentUpperVref1 , currentUpperVref1HelperVar);

		Gecode::FloatVar widthUpperCMLHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthUpperCML , widthUpperCMLHelperVar);

		Gecode::FloatVar currentUpperCMLHelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentUpperCML , currentUpperCMLHelperVar);
	}


	void SizingRulesConstraints::addFourTransistorCurrentMirrorConstraintsToMap(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & fourTransistorCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & vref1 = static_cast<const StructRec::Pair &>(fourTransistorCurrentMirror.getChild1());
		const StructRec::Pair & currentMirrorLoad = static_cast<const StructRec::Pair &>(fourTransistorCurrentMirror.getChild2());
		const StructRec::StructureId upperVref1 = vref1.getChild1().getIdentifier();
		const StructRec::StructureId lowerVref1 = vref1.getChild2().getIdentifier();
		const StructRec::StructureId upperCML = currentMirrorLoad.getChild1().getIdentifier();
		const StructRec::StructureId lowerCML = currentMirrorLoad.getChild2().getIdentifier();

		equalWidthMap_.add(upperVref1,getPartitioningResult().findComponent(lowerVref1));
		equalWidthMap_.add(upperCML,getPartitioningResult().findComponent(lowerCML));

		equalLengthMap_.add(upperVref1, getPartitioningResult().findComponent(lowerVref1));
		equalLengthMap_.add(upperVref1, getPartitioningResult().findComponent(lowerCML));
		equalLengthMap_.add(upperVref1, getPartitioningResult().findComponent(upperCML));

		}

	void SizingRulesConstraints::createImprovedWilsonCurrentMirrorConstraints(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & improvedWilsonCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & mixedPair1 = static_cast<const StructRec::Pair &>(improvedWilsonCurrentMirror.getChild1());
		const StructRec::Pair & mixedPair2 = static_cast<const StructRec::Pair &>(improvedWilsonCurrentMirror.getChild2());
		Gecode::IntVar widthLower1 = getTransistorToWidthMap().find(mixedPair1.getChild2().getIdentifier());
		Gecode::IntVar currentLower1 = getTransistorToCurrentMap().find(mixedPair1.getChild2().getIdentifier());
		Gecode::IntVar widthLower2 = getTransistorToWidthMap().find(mixedPair2.getChild2().getIdentifier());
		Gecode::IntVar currentLower2 = getTransistorToCurrentMap().find(mixedPair2.getChild2().getIdentifier());

		Gecode::FloatVar widthLower1HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLower1, widthLower1HelperVar);

		Gecode::FloatVar currentLower1HelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentLower1,currentLower1HelperVar);

		Gecode::FloatVar widthLower2HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLower2, widthLower2HelperVar);

		Gecode::FloatVar currentLower2HelperVar(getSpace(), -1000000000 , 1000000000);
		channel(getSpace(), currentLower2, currentLower2HelperVar);
	}


	void SizingRulesConstraints::addImprovedWilsonCurrentMirrorConstraintsToMap(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & improvedWilsonCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & mixedPair1 = static_cast<const StructRec::Pair &>(improvedWilsonCurrentMirror.getChild1());
		const StructRec::Pair & mixedPair2 = static_cast<const StructRec::Pair &>(improvedWilsonCurrentMirror.getChild2());
		const StructRec::StructureId upper1 = mixedPair1.getChild1().getIdentifier();
		const StructRec::StructureId lower1 = mixedPair1.getChild2().getIdentifier();
		const StructRec::StructureId upper2 = mixedPair2.getChild1().getIdentifier();
		const StructRec::StructureId lower2 = mixedPair2.getChild2().getIdentifier();

		equalWidthMap_.add(upper1,getPartitioningResult().findComponent(lower1));
		equalWidthMap_.add(upper2,getPartitioningResult().findComponent(lower2));

		equalLengthMap_.add(upper1, getPartitioningResult().findComponent(upper2));
		equalLengthMap_.add(lower1, getPartitioningResult().findComponent(lower2));

	}

	void SizingRulesConstraints::addWideSwingCascodeCurrentMirrorConstraintsToMap(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & wideSwingCascodeCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & vref2 = static_cast<const StructRec::Pair &>(wideSwingCascodeCurrentMirror.getChild1());
		const StructRec::Pair & cascodePair = static_cast<const StructRec::Pair &>(wideSwingCascodeCurrentMirror.getChild2());
		const StructRec::StructureId upperVref2 = vref2.getChild1().getIdentifier();
		const StructRec::StructureId lowerVref2 = vref2.getChild2().getIdentifier();
		const StructRec::StructureId upperCP = cascodePair.getChild1().getIdentifier();
		const StructRec::StructureId lowerCP = cascodePair.getChild2().getIdentifier();

		equalLengthMap_.add(upperVref2, getPartitioningResult().findComponent(upperCP));
		equalLengthMap_.add(lowerVref2, getPartitioningResult().findComponent(lowerCP));

	}

	void SizingRulesConstraints::createWideSwingCurrentMirrorConstraints(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & wideSwingCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & vref1 = static_cast<const StructRec::Pair &>(wideSwingCurrentMirror.getChild1());

		Gecode::IntVar widthLowerVref1 = getTransistorToWidthMap().find(vref1.getChild2().getIdentifier());
		Gecode::IntVar currentLowerVref1 = getTransistorToCurrentMap().find(vref1.getChild2().getIdentifier());
		Gecode::IntVar widthNTA = getTransistorToWidthMap().find(wideSwingCurrentMirror.getChild2().getIdentifier());
		Gecode::IntVar currentNTA = getTransistorToCurrentMap().find(wideSwingCurrentMirror.getChild2().getIdentifier());


		Gecode::FloatVar widthLowerVref1HelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLowerVref1, widthLowerVref1HelperVar);

		Gecode::FloatVar  currentLowerVref1HelperVar = getSpace().createChanneledFloatVar(currentLowerVref1, -1000000000, 1000000000);

		Gecode::FloatVar widthNTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthNTA , widthNTAHelperVar);

		Gecode::FloatVar  currentNTAHelperVar = getSpace().createChanneledFloatVar(currentNTA, -1000000000, 1000000000);
	}

	void SizingRulesConstraints::addWideSwingCurrentMirrorConstraintsToMap(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & wideSwingCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & vref1 = static_cast<const StructRec::Pair &>(wideSwingCurrentMirror.getChild1());

		const StructRec::StructureId lowerVref1 = vref1.getChild2().getIdentifier();
		const StructRec::StructureId nta  = wideSwingCurrentMirror.getChild2().getIdentifier();

		equalLengthMap_.add(lowerVref1, getPartitioningResult().findComponent(nta));
	}

	void SizingRulesConstraints::createWilsonCurrentMirrorConstraints(
		const StructRec::Structure& structure)
	{
		const StructRec::Pair & wilsonCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & mixedPair = static_cast<const StructRec::Pair &>(wilsonCurrentMirror.getChild2());

		Gecode::IntVar widthNTA = getTransistorToWidthMap().find(wilsonCurrentMirror.getChild1().getIdentifier());
		Gecode::IntVar currentNTA = getTransistorToCurrentMap().find(wilsonCurrentMirror.getChild1().getIdentifier());
		Gecode::IntVar widthLowerMP = getTransistorToWidthMap().find(mixedPair.getChild2().getIdentifier());
		Gecode::IntVar currentLowerMP = getTransistorToCurrentMap().find(mixedPair.getChild2().getIdentifier());

		Gecode::FloatVar widthNTAHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthNTA , widthNTAHelperVar);

		Gecode::FloatVar  currentNTAHelperVar = getSpace().createChanneledFloatVar(currentNTA, -1000000000, 1000000000);

		Gecode::FloatVar widthLowerMPHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
		channel(getSpace(), widthLowerMP, widthLowerMPHelperVar);

		Gecode::FloatVar  currentLowerMPHelperVar = getSpace().createChanneledFloatVar(currentLowerMP, -1000000000, 1000000000);
	}

	void SizingRulesConstraints::addWilsonCurrentMirrorConstraintsToMap(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & wilsonCurrentMirror = static_cast<const StructRec::Pair &>(structure);
		const StructRec::Pair & mixedPair = static_cast<const StructRec::Pair &>(wilsonCurrentMirror.getChild2());

		const StructRec::StructureId nta =  wilsonCurrentMirror.getChild1().getIdentifier();
		const StructRec::StructureId lowerMP = mixedPair.getChild2().getIdentifier();

		equalLengthMap_.add(nta, getPartitioningResult().findComponent(lowerMP));

	}

	void SizingRulesConstraints::addDifferentialPairConstraintsToMap(
		const StructRec::Structure& structure)
	{
		if(!getPartitioningResult().getTransconductancePart(structure).isFeedBack())
		{
			const StructRec::Pair & diffPair = static_cast<const StructRec::Pair &>(structure);
			const StructRec::StructureId child1 = diffPair.getChild1().getIdentifier();
			const StructRec::StructureId child2 = diffPair.getChild2().getIdentifier();

			equalLengthMap_.add(child1, getPartitioningResult().findComponent(child2));
			equalWidthMap_.add(child1, getPartitioningResult().findComponent(child2));
		}
	}

	void SizingRulesConstraints::addDifferentialStageConstraintsToMap()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		//LoadPart Constraints

		std::vector<Partitioning::LoadPart*>  loadParts = firstStage.getLoadPart();
		for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
		{
			Partitioning::LoadPart * loadPart = *it;
			std::vector<Partitioning::Component*> components = getPartitioningResult().getBelongingComponents(* loadPart);
			if(loadPart->consistsOfTwoDiodeArraysConnectedToSecondStageInput(getPartitioningResult()))
			{
				std::vector<Partitioning::Component*> diodeComps;
				std::vector<Partitioning::Component*> normalComps;
				for(auto & comp : components )
				{
					if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
						normalComps.push_back(comp);
					else if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
						diodeComps.push_back(comp);
				}

				assert(diodeComps.size() == 2, "There should be at least two diode comps  in the load!");

				equalLengthMap_.add(components, getPartitioningResult());
				equalWidthMap_.add(diodeComps, getPartitioningResult());

				if(normalComps.size() == 2)
					equalWidthMap_.add(normalComps, getPartitioningResult());
			}
			else
			{
				if(components.size() > 1)
				{
					std::map<Core::NetId, std::vector<Partitioning::Component*> > sameGateNetMap;

					for(auto & comp :components)
					{
						Core::NetId gateNet;
						if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
						{
							gateNet = comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Gate")).getIdentifier();

						}
						else
						{
							gateNet = comp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray","Drain")).getIdentifier();
						}
						if(sameGateNetMap.find(gateNet) == sameGateNetMap.end())
						{
							std::vector<Partitioning::Component* > sameGateNetComps;
							sameGateNetComps.push_back(comp);
							sameGateNetMap.insert(std::pair<Core::NetId,std::vector<Partitioning::Component*> >(gateNet, sameGateNetComps));
						}
						else
						{
							sameGateNetMap.at(gateNet).push_back(comp);
						}
					}

					for(auto & it : sameGateNetMap)
					{
						std::vector<Partitioning::Component*> sameGateNetComps = it.second;
						if(sameGateNetComps.size() >1)
						{
							equalLengthMap_.add(sameGateNetComps, getPartitioningResult());
							equalWidthMap_.add(sameGateNetComps, getPartitioningResult());
						}
					}
				}
			}
		}

	}

	void SizingRulesConstraints::createDifferentialPairConstraints(
			const StructRec::Structure& structure)
	{
		const StructRec::Pair & diffPair = static_cast<const StructRec::Pair &>(structure);
		const StructRec::StructureId child1 = diffPair.getChild1().getIdentifier();
		const StructRec::StructureId child2 = diffPair.getChild2().getIdentifier();

		Gecode::IntVar currentChild1 = getTransistorToCurrentMap().find(child1);
		Gecode::IntVar currentChild2 = getTransistorToCurrentMap().find(child2);

		const Partitioning::TransconductancePart & transPart = getPartitioningResult().getTransconductancePart(structure);
		if(transPart.isFirstStage() ||
				( transPart.isFeedBack()&& getPartitioningResult().getBelongingComponents(transPart).size() ==4 ))
		{
			if(getPartitioningResult().getBelongingComponents(**getPartitioningResult().getFirstStage().getLoadPart().begin()).size() >1)
			{
				Gecode::rel(getSpace(), currentChild1 == currentChild2);
			}
			else
			{
				Gecode::rel(getSpace(), currentChild1 >= 0.9 *currentChild2);
				Gecode::rel(getSpace(), currentChild1 <= 1.1 *currentChild2);
			}
		}
	}

	void SizingRulesConstraints::createDifferentialStageConstraints()
	{
		Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
		std::vector<Partitioning::LoadPart*>  loadParts = firstStage.getLoadPart();
		for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
		{
			Partitioning::LoadPart * loadPart = *it;
			std::vector<Partitioning::Component*> components = getPartitioningResult().getBelongingComponents(* loadPart);
			if(components.size() == 2)
			{
				Gecode::IntVar currentChild1 = getTransistorToCurrentMap().find(**components.begin());
				Gecode::IntVar currentChild2 = getTransistorToCurrentMap().find(**std::next(components.begin()));

				Gecode::rel(getSpace(), currentChild1/currentChild2 >=  0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild2 <=  1.05);

			}
			else if(components.size()== 4 && loadPart->hasCascodedPair())
			{
				const StructRec::Structure & cascodedPair = loadPart->getCascodedPair();
				std::vector<Partitioning::Component*> compCascodedPair = getPartitioningResult().findComponents(cascodedPair);

				Gecode::IntVar currentCascodedPairChild1 = getTransistorToCurrentMap().find(**compCascodedPair.begin());
				Gecode::IntVar currentCascodedPairChild2 = getTransistorToCurrentMap().find(**std::next(compCascodedPair.begin()));

				Gecode::rel(getSpace(), currentCascodedPairChild1/currentCascodedPairChild2 >= 0.95);
				Gecode::rel(getSpace(), currentCascodedPairChild1/currentCascodedPairChild2 <= 1.05);

				std::vector<const StructRec::Structure*> biasStructs =loadPart->getCurrentBiasOfFoldedPair();
				Gecode::IntVar currentBiasStructChild1 = getTransistorToCurrentMap().find((**biasStructs.begin()).getIdentifier());
				Gecode::IntVar currentBiasStructChild2 = getTransistorToCurrentMap().find((**std::next(biasStructs.begin())).getIdentifier());

				Gecode::rel(getSpace(), currentBiasStructChild1/currentBiasStructChild2 >= 0.95);
				Gecode::rel(getSpace(), currentBiasStructChild1/currentBiasStructChild2 <= 1.05 );
			}
			else if(components.size()== 4 && loadPart->hasCrossCoupledPair(getPartitioningResult()))
			{
				Partitioning::Component * nta1 = nullptr;
				Partitioning::Component * nta2 = nullptr;
				Partitioning::Component * dta1 = nullptr;
				Partitioning::Component * dta2 = nullptr;

				for(auto & comp: components)
				{
					if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray" && nta1 == nullptr)
					{
						nta1 = comp;
					}
					else if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
					{
						nta2 = comp;
					}
					else if(dta1 == nullptr)
					{
						dta1 = comp;
					}
					else
					{
						dta2 = comp;
					}
				}

				assert(dta1 != nullptr && dta2 != nullptr && nta1 != nullptr && nta2 != nullptr, "Every Load device should be assigned to a component!");
				assert(dta1 != dta2 && dta1 != nta1 && dta1 != nta2 &&
						dta2 != nta1  && dta2 != nta2 && nta1 != nta2, "Every assigned component should be different!");

				Gecode::IntVar currentNta1 = getTransistorToCurrentMap().find(*nta1);
				Gecode::IntVar currentNta2 = getTransistorToCurrentMap().find(*nta2);
				Gecode::IntVar currentDta1 = getTransistorToCurrentMap().find(*dta1);
				Gecode::IntVar currentDta2 = getTransistorToCurrentMap().find(*dta2);

				Gecode::rel(getSpace(), currentNta1/currentNta2 >= 0.95);
				Gecode::rel(getSpace(), currentNta1/currentNta2 <= 1.05);

				Gecode::rel(getSpace(), currentDta1/currentDta2 >= 0.95);
				Gecode::rel(getSpace(), currentDta1/currentDta2 <= 1.05 );
			}
			else if(components.size() == 4 && firstStage.isComplementary() )
			{
				Partitioning::Component * output1 = nullptr;
				Partitioning::Component * output2 = nullptr;
				Partitioning::Component * supply1 = nullptr;
				Partitioning::Component * supply2 = nullptr;

				for(auto & comp: components)
				{
					if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() && supply1 == nullptr)
					{
						supply1 = comp;
					}
					else if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
					{
						supply2 = comp;
					}
					else if(output1 == nullptr)
					{
						output1 = comp;
					}
					else
					{
						output2 = comp;
					}
				}

				assert(supply1 != nullptr && supply2 != nullptr && output1 != nullptr && output2 != nullptr, "Every Load device should be assigned to a component!");
				assert(supply1 != supply2 && supply1 != output1 && supply1 != output2 &&
						supply2 != output1  && supply2 != output2 && output1 != output2, "Every assigned component should be different!");


				Gecode::IntVar currentSupply1 = getTransistorToCurrentMap().find(*supply1);
				Gecode::IntVar currentSupply2 = getTransistorToCurrentMap().find(*supply2);
				Gecode::IntVar currentOutput1 = getTransistorToCurrentMap().find(*output1);
				Gecode::IntVar currentOutput2 = getTransistorToCurrentMap().find(*output2);


				Gecode::rel(getSpace(), currentSupply1/currentSupply2 >= 0.95 );
				Gecode::rel(getSpace(), currentSupply1/currentSupply2 <= 1.05  );

				Gecode::rel(getSpace(), currentOutput1/currentOutput2 >= 0.95 );
				Gecode::rel(getSpace(), currentOutput1/currentOutput2 <= 1.05  );
			}
			else if(components.size() == 4)
			{
				Gecode::IntVar currentChild1 = getTransistorToCurrentMap().find(**components.begin());
				Gecode::IntVar currentChild2 = getTransistorToCurrentMap().find(**std::next(components.begin()));
				Gecode::IntVar currentChild3 = getTransistorToCurrentMap().find(**std::next(components.rbegin()));
				Gecode::IntVar currentChild4 = getTransistorToCurrentMap().find(**components.rbegin());

				Gecode::rel(getSpace(), currentChild1/currentChild2 >= 0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild2 <= 1.05);

				Gecode::rel(getSpace(), currentChild1/currentChild3 >= 0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild3 <= 1.05);

				Gecode::rel(getSpace(), currentChild1/currentChild4 >= 0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild4 <= 1.05);
			}
			else if(components.size() == 3)
			{
				Gecode::IntVar currentChild1 = getTransistorToCurrentMap().find(**components.begin());
				Gecode::IntVar currentChild2 = getTransistorToCurrentMap().find(**std::next(components.begin()));
				Gecode::IntVar currentChild3 = getTransistorToCurrentMap().find(**std::next(components.rbegin()));

				Gecode::rel(getSpace(), currentChild1/currentChild2 >= 0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild2 <= 1.05);

				Gecode::rel(getSpace(), currentChild1/currentChild3 >= 0.95);
				Gecode::rel(getSpace(), currentChild1/currentChild3 <= 1.05);
			}

		}
	}

	void SizingRulesConstraints::addEqualLengthBiasStagesToMap()
	{
		if(getPartitioningResult().hasPrimarySecondStage() && !getPartitioningResult().hasSecondarySecondStage())
				{
					std::vector<Partitioning::BiasPart*> firstStageBiases = getPartitioningResult().getFirstStage().getBiasPart();
					std::vector<Partitioning::Component*> supplyFirstStageBiasComps;
					for(auto & biasPartFirstStage : firstStageBiases)
					{
						std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(*biasPartFirstStage);
						for(auto & comp: comps)
						{
							if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
							{
								supplyFirstStageBiasComps.push_back(comp);
							}
						}
					}

					std::vector<Partitioning::BiasPart*> secondStageBiases = (**getPartitioningResult().getSecondStages().begin()).getBiasPart();
					std::vector<Partitioning::Component*> supplySecondStageBiasComps;
					for(auto & biasPartSecondStage : secondStageBiases)
					{
						std::vector<Partitioning::Component*> comps = getPartitioningResult().getBelongingComponents(*biasPartSecondStage);
						for(auto & comp: comps)
						{
							if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply())
							{

								supplySecondStageBiasComps.push_back(comp);
							}
						}
					}

					if((**supplyFirstStageBiasComps.begin()).getArray().getTechType() ==(**supplySecondStageBiasComps.begin()).getArray().getTechType())
					{
						std::vector<Partitioning::Component*> equalLengthComps;
						Core::PinType gatePin = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName( "Gate"));
						const StructRec::StructureCircuit & arrayCircuit = getStructureRecognitionResult().findStructureCircuit(0);

						Partitioning::Component & biasCompFirstStage = **supplyFirstStageBiasComps.begin();
						equalLengthComps.push_back(&biasCompFirstStage);



						for(auto & biasCompSecondStage : supplySecondStageBiasComps)
						{
							const Core::Device & deviceSecondStageBias = **biasCompSecondStage->getArray().getDevices().begin();
							Core::NetId gateNetBiasCompSecondStage = deviceSecondStageBias.findNet(gatePin).getIdentifier();
							if(arrayCircuit.findStructureNet(gateNetBiasCompSecondStage).getAllConnectedStructures().size() ==1)
							{
								equalLengthComps.push_back(biasCompSecondStage);
							}
						}
						if(equalLengthComps.size() == 2)
						{
							equalLengthMap_.add(equalLengthComps, getPartitioningResult());
						}
					}

				}
	}

	void SizingRulesConstraints::createSystematicOffsetConstraints()
	{

		if(getPartitioningResult().hasSecondStage() && !getPartitioningResult().hasThirdStage())
		{
			//Second stage supply comp:
			Partitioning::Component * secondStageSupplyComp = nullptr;
			for(auto & comp : getPartitioningResult().getBelongingComponents(getPartitioningResult().getPrimarySecondStage()))
			{
				if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
				{
					secondStageSupplyComp = comp;
					break;
				}
			}
			assert(secondStageSupplyComp != nullptr, "Every second stage should have a supply connected component");

			Gecode::IntVar widthTransSecondStage = getTransistorToWidthMap().find(*secondStageSupplyComp);
			Gecode::IntVar lengthTransSecondStage = getTransistorToLengthMap().find(*secondStageSupplyComp);

			Gecode::FloatVar widthTransSecondStageHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
			channel(getSpace(), widthTransSecondStage , widthTransSecondStageHelperVar);
			Gecode::FloatVar lengthTransSecondStageHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
			channel(getSpace(), lengthTransSecondStage , lengthTransSecondStageHelperVar);

			Gecode::IntVar currentSecondStage = getTransistorToCurrentMap().find(*secondStageSupplyComp);
			Gecode::FloatVar currentSecondStageHelperVar = getSpace().createChanneledFloatVar(currentSecondStage, -1000000000, 1000000000);

			//The type of load is important:
			Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
			std::vector<Partitioning::LoadPart*>  loadParts = firstStage.getLoadPart();

			for(std::vector<Partitioning::LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
			{
				Partitioning::LoadPart * loadPart = *it;
				std::vector<Partitioning::Component*> compLoad = getPartitioningResult().getBelongingComponents(* loadPart);
				if(!loadPart->hasCascodedPair() && (**compLoad.begin()).getArray().getTechType() == secondStageSupplyComp->getArray().getTechType())
				{
					if(compLoad.size() == 2 && !loadPart->hasBiasPart())
					{
						Gecode::IntVar widthLoad = getTransistorToWidthMap().find(**compLoad.begin());
						Gecode::IntVar lengthLoad = getTransistorToLengthMap().find(**compLoad.begin());

						Gecode::FloatVar widthLoadHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
						channel(getSpace(), widthLoad , widthLoadHelperVar);
						Gecode::FloatVar lengthLoadHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
						channel(getSpace(), lengthLoad , lengthLoadHelperVar);

						Gecode::IntVar currentLoad = getTransistorToCurrentMap().find(**compLoad.begin());
						Gecode::FloatVar currentLoadHelperVar = getSpace().createChanneledFloatVar(currentLoad, -1000000000, 1000000000);


						Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthLoadHelperVar*currentLoadHelperVar)
												/(widthLoadHelperVar * lengthTransSecondStageHelperVar * currentSecondStageHelperVar) >= 0.9));
						Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthLoadHelperVar*currentLoadHelperVar)
												/(widthLoadHelperVar * lengthTransSecondStageHelperVar*currentSecondStageHelperVar) <= 1.1));
					}
					else if(compLoad.size() > 2)
					{

						//Search for upper component not to output connected
						Partitioning::Component * innerComp = nullptr;
						for(auto & comp : compLoad)
						{
							if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
									&& isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")), firstStage))
							{

								innerComp = comp;
								break;
							}
						}
						assert(loadPart->hasCrossCoupledPair(getPartitioningResult()) || compLoad.size() !=4 || innerComp != nullptr, "A four transistor load should have a inner comp!");
						assert(compLoad.size() !=3 || innerComp != nullptr, "A three transistor load must not have a inner comp!");

						if(innerComp != nullptr)
						{
							if(innerComp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
							{
								const StructRec::StructureNet & sourceNet = innerComp->getArray().findNet(StructRec::StructurePinType("MosfetDiodeArray", "Source"));
								if(!sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).empty())
								{
									assert(sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).size() == 1, "there should only be one diode connected transistor on the source of the other diode transistor");
									Partitioning::Component & supplyComp = getPartitioningResult().findComponent((**sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).begin()).getIdentifier());

									float muCox = getCircuitInformation().getTechnologieSpecificationSHM(*innerComp).getMobilityOxideCapacityCoefficient();

									Gecode::IntVar currentLoad = getTransistorToCurrentMap().find(*innerComp);
									Gecode::FloatVar currentLoadHelperVar = getSpace().createChanneledFloatVar(currentLoad, -1000000000, 1000000000);


									Gecode::IntVar widthInnerComp = getTransistorToWidthMap().find(*innerComp);
									Gecode::IntVar lengthInnerComp = getTransistorToLengthMap().find(*innerComp);

									Gecode::FloatVar widthInnerCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
									channel(getSpace(), widthInnerComp , widthInnerCompHelperVar);
									Gecode::FloatVar lengthInnerCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
									channel(getSpace(), lengthInnerComp , lengthInnerCompHelperVar);

									Gecode::IntVar widthSupplyComp = getTransistorToWidthMap().find(supplyComp);
									Gecode::IntVar lengthSupplyComp = getTransistorToLengthMap().find(supplyComp);

									Gecode::FloatVar widthSupplyCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
									channel(getSpace(), widthSupplyComp , widthSupplyCompHelperVar);
									Gecode::FloatVar lengthSupplyCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
									channel(getSpace(), lengthSupplyComp , lengthSupplyCompHelperVar);


									rel(getSpace(), sqrt(2* currentLoadHelperVar* lengthSupplyCompHelperVar/ (muCox *widthSupplyCompHelperVar))
											+ sqrt(2* currentLoadHelperVar* lengthInnerCompHelperVar/ (muCox *widthInnerCompHelperVar))
											>= 0.9 * sqrt(2* currentSecondStageHelperVar* lengthTransSecondStageHelperVar/ (muCox *widthTransSecondStageHelperVar)) );

									rel(getSpace(), sqrt(2* currentLoadHelperVar* lengthSupplyCompHelperVar/ (muCox *widthSupplyCompHelperVar))
											+ sqrt(2* currentLoadHelperVar* lengthInnerCompHelperVar/ (muCox *widthInnerCompHelperVar))
											<= 1.1 * sqrt(2* currentSecondStageHelperVar* lengthTransSecondStageHelperVar/ (muCox *widthTransSecondStageHelperVar)) );

								}
							}
							else
							{
								const StructRec::StructureNet & sourceNet = innerComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source"));
								if(!sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).empty())
								{
									assert(sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() == 1, "There should only be one normal transistor on the source of the normal transistor");
									Partitioning::Component & supplyComp = getPartitioningResult().findComponent((**sourceNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).begin()).getIdentifier());
									{
										//There  should be a gate drain connection between the supply comp and the inner comp
										if(supplyComp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier()
												== innerComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier())
										{
											Gecode::IntVar currentLoad = getTransistorToCurrentMap().find(*innerComp);
											Gecode::IntVar widthSupplyComp = getTransistorToWidthMap().find(supplyComp);
											Gecode::IntVar lengthSupplyComp = getTransistorToLengthMap().find(supplyComp);

											Gecode::FloatVar widthSupplyCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
											channel(getSpace(), widthSupplyComp , widthSupplyCompHelperVar);
											Gecode::FloatVar lengthSupplyCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
											channel(getSpace(), lengthSupplyComp , lengthSupplyCompHelperVar);


											Gecode::FloatVar currentLoadHelperVar = getSpace().createChanneledFloatVar(currentLoad, -1000000000, 1000000000);



											Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthSupplyCompHelperVar*currentLoadHelperVar)
																	/(widthSupplyCompHelperVar * lengthTransSecondStageHelperVar * currentSecondStageHelperVar) >= 0.9));
											Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthSupplyCompHelperVar*currentLoadHelperVar)
																	/(widthSupplyCompHelperVar * lengthTransSecondStageHelperVar*currentSecondStageHelperVar) <= 1.1));
										}
									}
								}
							}
						}
						else
						{
							Partitioning::Component * innerSupplyComp = nullptr;

							if(loadPart->hasCrossCoupledPair(getPartitioningResult()))
							{

								for(auto & comp : compLoad)
								{
									if(comp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
									{
										innerSupplyComp = comp;
										break;
									}
								}
							}
							else
							{
								Partitioning::Component * outputComp = nullptr;
								for(auto & comp : compLoad)
								{
									if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
										&& isOutputNetStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")),firstStage))
									{
										outputComp = comp;
										break;
									}
								}
								assert( outputComp != nullptr,"One of the three transistors of the load should be a output comp");

								for(auto & comp : compLoad)
								{
									if(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply()
										&& comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")).getIdentifier()
										!= outputComp->getArray().findNet(StructRec::StructurePinType(outputComp->getArray().getStructureName(), "Source")).getIdentifier() )
									{
										innerSupplyComp = comp;
										break;
									}
								}
							}
							assert( innerSupplyComp != nullptr ,"There should be a inner transistor connected to the supply and not to any other transistor of the same load!");

							if(innerSupplyComp->getArray().getStructureName().toStr() == "MosfetDiodeArray")
							{
								Gecode::IntVar currentLoad = getTransistorToCurrentMap().find(*innerSupplyComp);
								Gecode::IntVar widthInnerSupplyComp = getTransistorToWidthMap().find(*innerSupplyComp);
								Gecode::IntVar lengthInnerSupplyComp = getTransistorToLengthMap().find(*innerSupplyComp);

								Gecode::FloatVar widthInnerSupplyCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
								channel(getSpace(), widthInnerSupplyComp , widthInnerSupplyCompHelperVar);
								Gecode::FloatVar lengthInnerSupplyCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
								channel(getSpace(), lengthInnerSupplyComp , lengthInnerSupplyCompHelperVar);

								Gecode::FloatVar currentLoadHelperVar = getSpace().createChanneledFloatVar(currentLoad, -1000000000, 1000000000);


								Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthInnerSupplyCompHelperVar*currentLoadHelperVar)
														/(widthInnerSupplyCompHelperVar * lengthTransSecondStageHelperVar * currentSecondStageHelperVar) >= 0.9));
								Gecode::rel(getSpace(), ((widthTransSecondStageHelperVar*lengthInnerSupplyCompHelperVar*currentLoadHelperVar)
														/(widthInnerSupplyCompHelperVar * lengthTransSecondStageHelperVar*currentSecondStageHelperVar) <= 1.1));
							}
						}
					}

				}

			}
		}

	}

	void SizingRulesConstraints::createCurrentDensityConstraintInFeedbackCircuits()
	{
		for(auto & transPart : getPartitioningResult().getAllTransconductanceParts())
		{
			if(transPart->isFeedBack())
			{
				std::vector<Partitioning::Component*> gateConnectedOpAmpLoadPartComps;
				std::vector<Partitioning::LoadPart*> feedbackLoadParts = transPart->getLoadPart();

				for(auto & feedbackLoadPart : feedbackLoadParts)
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
								gateConnectedOpAmpLoadPartComps.push_back(&gateConnectedComp);
							}
						}
					}
				}

				for(auto & feedbackLoadPart : feedbackLoadParts)
				{
					for(auto & feedbackLoadComp : getPartitioningResult().getBelongingComponents(*feedbackLoadPart))
					{
						Gecode::IntVar currentFeedbackLoadComp = getTransistorToCurrentMap().find(*feedbackLoadComp);
						Gecode::IntVar widthFeedbackLoadComp = getTransistorToWidthMap().find(*feedbackLoadComp);
						Gecode::IntVar lengthFeedbackLoadComp = getTransistorToLengthMap().find(*feedbackLoadComp);

						Gecode::FloatVar widthFeedbackLoadCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
						channel(getSpace(), widthFeedbackLoadComp , widthFeedbackLoadCompHelperVar);
						Gecode::FloatVar lengthFeedbackLoadCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
						channel(getSpace(), lengthFeedbackLoadComp , lengthFeedbackLoadCompHelperVar);

						Gecode::FloatVar currentFeedbackLoadCompHelperVar = getSpace().createChanneledFloatVar(currentFeedbackLoadComp, -1000000000, 1000000000);

						for(auto & opAmpLoadComp : gateConnectedOpAmpLoadPartComps)
						{
							Gecode::IntVar currentOpAmpLoadComp = getTransistorToCurrentMap().find(*opAmpLoadComp);
							Gecode::IntVar widthOpAmpLoadComp = getTransistorToWidthMap().find(*opAmpLoadComp);
							Gecode::IntVar lengthOpAmpLoadComp = getTransistorToLengthMap().find(*opAmpLoadComp);

							Gecode::FloatVar widthOpAmpLoadCompHelperVar(getSpace(), 1 , getSpace().getWidthUpperBound());
							channel(getSpace(), widthOpAmpLoadComp , widthOpAmpLoadCompHelperVar);
							Gecode::FloatVar lengthOpAmpLoadCompHelperVar(getSpace(), 1 , getSpace().getLengthUpperBound());
							channel(getSpace(), lengthOpAmpLoadComp , lengthOpAmpLoadCompHelperVar);

							Gecode::FloatVar currentOpAmpLoadCompHelperVar = getSpace().createChanneledFloatVar(currentOpAmpLoadComp, -1000000000, 1000000000);


							rel(getSpace(), (currentOpAmpLoadCompHelperVar* lengthOpAmpLoadCompHelperVar * widthFeedbackLoadCompHelperVar) /
									(currentFeedbackLoadCompHelperVar * widthOpAmpLoadCompHelperVar * lengthFeedbackLoadCompHelperVar) > 0.9);

							rel(getSpace(), (currentOpAmpLoadCompHelperVar* lengthOpAmpLoadCompHelperVar * widthFeedbackLoadCompHelperVar) /
									(currentFeedbackLoadCompHelperVar * widthOpAmpLoadCompHelperVar * lengthFeedbackLoadCompHelperVar) < 1.1);
						}

					}

				}
			}
		}
	}


	void SizingRulesConstraints::addSymmetricalSecondStageConstraintToMap()
	{
		std::vector<Partitioning::TransconductancePart*> secondStage = getPartitioningResult().getSecondStages();
		assert(secondStage.size() == 1 || secondStage.size() ==2 , "Unknown, unsupported topology.");

		Partitioning::TransconductancePart * secondStage1 = *secondStage.begin();
		std::vector<Partitioning::Component*> compSecondStage1 = getPartitioningResult().getBelongingComponents(*secondStage1);

		Partitioning::TransconductancePart * secondStage2 = *std::next(secondStage.begin());
		std::vector<Partitioning::Component*>compSecondStage2 = getPartitioningResult().getBelongingComponents(*secondStage2);

		if(compSecondStage1.size() == 1 && compSecondStage2.size() == 1)
		{
			std::vector<Partitioning::Component*> equalTransComps;
			equalTransComps.insert(equalTransComps.end(), compSecondStage1.begin(), compSecondStage1.end());
			equalTransComps.insert(equalTransComps.end(), compSecondStage2.begin(), compSecondStage2.end());

			equalWidthMap_.add(equalTransComps, getPartitioningResult());
			equalLengthMap_.add(equalTransComps, getPartitioningResult());
		}
		else if(compSecondStage1.size() == 2 && compSecondStage2.size() == 2)
		{
			std::vector<Partitioning::Component*> equalTransCompsSupply;
			std::vector<Partitioning::Component*> equalTransCompsOutput;

			for(auto& it : compSecondStage1)
			{
				if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					equalTransCompsSupply.push_back(it);
				}
				else
				{
					equalTransCompsOutput.push_back(it);
				}
			}

			for(auto& it : compSecondStage2)
			{
				if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					equalTransCompsSupply.push_back(it);
				}
				else
				{
					equalTransCompsOutput.push_back(it);
				}
			}

			assert(equalTransCompsSupply.size() == 2 && equalTransCompsOutput.size() == 2, "There should be two output and supply comps in the two second stages!");

			equalWidthMap_.add(equalTransCompsSupply, getPartitioningResult());
			equalLengthMap_.add(equalTransCompsSupply, getPartitioningResult());

			equalWidthMap_.add(equalTransCompsOutput, getPartitioningResult());
			equalLengthMap_.add(equalTransCompsOutput, getPartitioningResult());
		}
		else
		{
			assert("The second stages should be symmetric!");
		}


		std::vector<Partitioning::Component*> equalBiasComps;

		Partitioning::BiasPart& biasSecondStage1 = **secondStage1->getBiasPart().begin();
		std::vector<Partitioning::Component*>compBiasSecondStage1 = getPartitioningResult().getBelongingComponents(biasSecondStage1);

		Partitioning::BiasPart& biasSecondStage2 = **secondStage2->getBiasPart().begin();
		std::vector<Partitioning::Component*> compBiasSecondStage2 = getPartitioningResult().getBelongingComponents(biasSecondStage2);

		for(auto& it : compBiasSecondStage1)
		{
			if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
				equalBiasComps.push_back(it);
			}
		}

		for(auto& it : compBiasSecondStage2)
		{
			if(it->getArray().findNet(StructRec::StructurePinType(it->getArray().getStructureName().toStr(), "Source")).isSupply())
			{
				equalBiasComps.push_back(it);
			}
		}

		equalWidthMap_.add(equalBiasComps, getPartitioningResult());
		equalLengthMap_.add(equalBiasComps, getPartitioningResult());

	}


	void SizingRulesConstraints::addSizingRulesdOfCMFBCircuitsToMap()
	{

		for(auto & transPart : getPartitioningResult().getAllTransconductanceParts())
		{
			if(transPart->isFeedBack())
			{

				std::vector<Partitioning::Component *> transPartComps = getPartitioningResult().getBelongingComponents(*transPart);

				if(transPartComps.size() == 4)
				{
					equalWidthMap_.add(transPartComps, getPartitioningResult());
					equalLengthMap_.add(transPartComps, getPartitioningResult());
				}
				else
				{
					std::vector<Partitioning::Component *> referenceVoltageInputComps;
					for(auto & transPartComp : transPartComps)
					{
						const StructRec::StructureNet & gateNet = transPartComp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
						if(getCircuitInformation().getCircuitParameter().isReferenceVoltageNet(gateNet.getIdentifier()))
						{
							referenceVoltageInputComps.push_back(transPartComp);
						}
					}
					if(referenceVoltageInputComps.size() > 1)
					{
						equalWidthMap_.add(referenceVoltageInputComps, getPartitioningResult());
						equalLengthMap_.add(referenceVoltageInputComps, getPartitioningResult());
					}

				}
				std::vector<Partitioning::BiasPart*> biasParts = transPart->getBiasPart();
				std::vector<Partitioning::Component*> equalBiasComps;
				for(auto & biasPart : biasParts )
				{
					std::vector<Partitioning::Component*> biasComps = getPartitioningResult().getBelongingComponents(*biasPart);
					equalBiasComps.insert(equalBiasComps.end(), biasComps.begin(), biasComps.end());
				}
				assert(!equalBiasComps.empty(), "A feedback circuits need to have at least one bias comp");
				if(equalBiasComps.size() > 1)
				{
					equalWidthMap_.add(equalBiasComps, getPartitioningResult());
					equalLengthMap_.add(equalBiasComps, getPartitioningResult());
				}

				std::vector<Partitioning::LoadPart*> loadParts = transPart->getLoadPart();
				std::vector<Partitioning::Component*> equalLoadComps;
				for(auto & loadPart : loadParts )
				{
					std::vector<Partitioning::Component*> loadComps = getPartitioningResult().getBelongingComponents(*loadPart);
					equalLoadComps.insert(equalLoadComps.end(), loadComps.begin(), loadComps.end());
				}

				if(equalLoadComps.size() > 1)
				{
					equalWidthMap_.add(equalLoadComps, getPartitioningResult());
					equalLengthMap_.add(equalLoadComps, getPartitioningResult());
				}

			}
		}

	}


	bool SizingRulesConstraints::isInitialized() const
	{
		return isInitialized_;
	}

	void SizingRulesConstraints::addEqualLengthVoltageCurrentBiasToMap()
	{
		for(auto &voltageBias : getPartitioningResult().getAllVoltageBiases())
		{
			std::vector<Partitioning::Part*> biasedParts = voltageBias->getBiasedParts();

			std::vector<Partitioning::Component*> compsVoltageBias = getPartitioningResult().getBelongingComponents(*voltageBias);

			for(auto & biasedPart : biasedParts)
			{
				std::vector<Partitioning::Component*> compsBiasedPart = getPartitioningResult().getBelongingComponents(*biasedPart);

				for(auto & compVoltageBias : compsVoltageBias)
				{
					std::vector<Partitioning::Component*> equalLengthComps;
					equalLengthComps.push_back(compVoltageBias);

					Core::PinType gatePinType = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName("Gate"));
					Core::NetId gateNetCompVoltageBias = (*compVoltageBias->getArray().getDevices().begin())->findNet(gatePinType).getIdentifier();

					for(auto & compBiasedPart : compsBiasedPart)
					{
						Core::NetId gateNetCompBiasedPart = (*compBiasedPart->getArray().getDevices().begin())->findNet(gatePinType).getIdentifier();

						if(gateNetCompVoltageBias == gateNetCompBiasedPart)
						{
							equalLengthComps.push_back(compBiasedPart);
						}
					}
					if(equalLengthComps.size() > 1)
						equalLengthMap_.add(equalLengthComps,getPartitioningResult());
				}
			}


		}
	}

	void SizingRulesConstraints::addSymmetryIn2TransistorCurrentMirrorPartsToMap(std::vector<Partitioning::BiasPart*> currentMirrorParts)
	{
		for(auto &it_currentMirrorPart : currentMirrorParts)
		{
//			assert(it_currentMirrorPart->getMainStructures().size() ==1, "A voltage bias should consist of only on structure");
			const StructRec::Structure & mainStructure =  **it_currentMirrorPart->getMainStructures().begin();
			bool isPartOfFourTransistorCurrentMirror = false;
			bool isPartOfCascodeCurrentMirror = false;
			bool isPartOfImprovedWilsonCurrentMirror = false;
			bool isWideSwingCascodeCurrentMirror = false;

			if(mainStructure.hasParent())
			{
				for(auto & it_parents : mainStructure.getTopmostParents())
				{
					if(it_parents->getStructureName() == StructRec::StructureName("MosfetFourTransistorCurrentMirror"))
					{
						isPartOfFourTransistorCurrentMirror = true;
					}
					else if(it_parents->getStructureName() == StructRec::StructureName("MosfetCascodeCurrentMirror"))
					{
						isPartOfCascodeCurrentMirror = true;
					}
					else if(it_parents->getStructureName() == StructRec::StructureName("MosfetImprovedWilsonCurrentMirror"))
					{
						isPartOfImprovedWilsonCurrentMirror = true;
					}
					else if(it_parents->getStructureName() == StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror"))
					{
						isWideSwingCascodeCurrentMirror = true;
					}
				}
			}

			if(isPartOfFourTransistorCurrentMirror)
			{
				equalWidthMap_.add(getPartitioningResult().findComponents(mainStructure), getPartitioningResult());
				equalLengthMap_.add(getPartitioningResult().findComponents(mainStructure), getPartitioningResult());
			}
			else if(isWideSwingCascodeCurrentMirror || isPartOfImprovedWilsonCurrentMirror || isPartOfCascodeCurrentMirror)
			{
				equalLengthMap_.add(getPartitioningResult().findComponents(mainStructure), getPartitioningResult());
			}

		}
	}

	std::vector<const StructRec::StructureNet*> SizingRulesConstraints::findOutputNets(Partitioning::TransconductancePart & stage) const
	{
		assert(stage.isFirstStage() || stage.isPrimarySecondStage() || stage.isThirdStage(), "Transconductance might only be a helper Structure");

		std::vector<const StructRec::StructureNet *> outputNets;

		if((stage.isPrimarySecondStage() || stage.isThirdStage()) && getPartitioningResult().getBelongingComponents(stage).size() > 1)
		{
			std::vector<Partitioning::Component *> compsStage = getPartitioningResult().getBelongingComponents(stage);
			for(auto & comp : compsStage)
			{
				if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply())
				{
					outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")));
					break;
				}
			}
			assert(!outputNets.empty(), "There should be at least one transistor in the second stage NOT connected to supply");
		}
		else if(stage.isFirstStage())
		{
			if(getPartitioningResult().hasSecondStage() && !getPartitioningResult().hasSecondarySecondStage())
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
			else if(getPartitioningResult().hasSecondarySecondStage())
			{
				std::vector<Partitioning::Component *> compsStage = getPartitioningResult().getBelongingComponents(stage);
				for(auto & comp : compsStage)
				{
					outputNets.push_back(&comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Drain")));
					break;
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

		}
		else
		{
			Partitioning::Component & compStage = **getPartitioningResult().getBelongingComponents(stage).begin();
			outputNets.push_back(&compStage.getArray().findNet(StructRec::StructurePinType(compStage.getArray().getStructureName(), "Drain")));
		}
		return outputNets;
	}

	bool SizingRulesConstraints::isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const
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



	void SizingRulesConstraints::setTwoPortToValueMap(
		ComponentToIntVarMap& twoPortMap)
	{
		twoPortToValueMap_  = & twoPortMap;
	}

	ComponentToIntVarMap& SizingRulesConstraints::getTwoPortToValueMap()
	{
		assert(twoPortToValueMap_ != nullptr);
		return * twoPortToValueMap_;
	}

	void SizingRulesConstraints::createSymmetricalCompensationCapaciticies()
	{
		if(getPartitioningResult().hasSecondStage() && getPartitioningResult().getSecondStages().size() == 2)
		{
			Partitioning::Component* compCap1 = nullptr;
			Partitioning::Component* compCap2 = nullptr;

			for(auto & cap : getPartitioningResult().getAllCapacitanceParts())
			{
				if(cap->isCompensationCapacitance())
				{
					Partitioning::Component * capComp = *getPartitioningResult().getBelongingComponents(*cap).begin();
					if(compCap1 == nullptr)
						compCap1 = capComp;
					else
						compCap2 = capComp;
				}
			}

			if(compCap1 != nullptr && compCap2 != nullptr)
			{
				Gecode::IntVar compCap1Value = getTwoPortToValueMap().find(*compCap1);
				Gecode::IntVar compCap2Value = getTwoPortToValueMap().find(*compCap2);

				rel(getSpace(), compCap1Value == compCap2Value);
			}
		}
	}

	void SizingRulesConstraints::createMinimalAreaConstraintForSimpleSlewRateEnhancer()
	{
		if(getPartitioningResult().hasPositiveFeedbackParts())
		{
			for(auto & posFeedbackPart : getPartitioningResult().getAllPositiveFeedbackParts())
			{
				if(posFeedbackPart->isSlewRateEnhancer() && posFeedbackPart->hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult()))
				{
					for(auto & comp : getPartitioningResult().getBelongingComponents(*posFeedbackPart))
					{
						int Amin = getCircuitInformation().getTechnologieSpecificationSHM(*comp).getMinArea();
						Gecode::IntVar width = getTransistorToWidthMap().find(*comp);
						Gecode::IntVar length = getTransistorToLengthMap().find(*comp);

						rel(getSpace(), width * length <= 2* Amin);
					}
				}
			}
		}
	}

	void SizingRulesConstraints::createCurrentConstraintForSimpleSlewRateEnhancer()
		{
			if(getPartitioningResult().hasPositiveFeedbackParts())
			{
				for(auto & posFeedbackPart : getPartitioningResult().getAllPositiveFeedbackParts())
				{
					if(posFeedbackPart->isSlewRateEnhancer() && posFeedbackPart->hasGateNetsConnectedToBiasOfFoldedPair(getPartitioningResult()))
					{
						Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();

						Partitioning::BiasPart & FB = **firstStage.getBiasPart().begin();
						Partitioning::Component & compFB = **getPartitioningResult().getBelongingComponents(FB).begin();

						Partitioning::Component * biasFL1B = nullptr;
						Partitioning::Component& FL1B = getPartitioningResult().getBiasOfFoldedPair();

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

						Gecode::IntVar idsFB = getTransistorToCurrentMap().find(compFB);
						Gecode::IntVar idsBiasFL1B = getTransistorToCurrentMap().find(*biasFL1B);
						rel(getSpace(), abs(idsFB) > 10 * abs(idsBiasFL1B));
					}
				}
			}
		}

		void SizingRulesConstraints::createThreeTransistorFeedbackCircuitConstraint()
		{
			if(getPartitioningResult().hasFeedbackStage())
			{
				for(auto & transPart : getPartitioningResult().getAllTransconductanceParts())
				{
					if(transPart->isFeedBack())
					{
						std::vector<Partitioning::Component* > feedbackComps = getPartitioningResult().getBelongingComponents(*transPart);
						if(feedbackComps.size() <= 3)
						{
							Partitioning::Component * vrefInputComp = nullptr;
							Partitioning::Component * otherComp = & transPart->findComponentNotConnectedToVref(getCircuitInformation().getCircuitParameter(), getPartitioningResult());
							if(getCircuitInformation().getCircuitParameter().hasReferenceVoltagePin())
							{
								for(auto & comp : feedbackComps)
								{
									const StructRec::StructureNet & gateNet = comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
									if(getCircuitInformation().getCircuitParameter().isReferenceVoltageNet(gateNet.getIdentifier()))
									{
										vrefInputComp = comp;
										break;
									}
								}

								assert(vrefInputComp != nullptr, "There should be one component connected to vref");

								Gecode::IntVar widthVrefComp = getTransistorToWidthMap().find(*vrefInputComp);
								Gecode::IntVar lengthVrefComp = getTransistorToLengthMap().find(*vrefInputComp);

								Gecode::IntVar widthOtherComp = getTransistorToWidthMap().find(*otherComp);
								Gecode::IntVar lengthOtherComp = getTransistorToLengthMap().find(*otherComp);

								rel(getSpace(), widthOtherComp == 2 * widthVrefComp);
								rel(getSpace(), lengthOtherComp == lengthVrefComp);

								Partitioning::TransconductancePart & firstStage = getPartitioningResult().getFirstStage();
								Partitioning::Component & firstStageComp = **getPartitioningResult().getBelongingComponents(firstStage).begin();

								Gecode::IntVar widthFirstStageComp = getTransistorToWidthMap().find(firstStageComp);
								Gecode::IntVar lengthFirstStageComp = getTransistorToLengthMap().find(firstStageComp);

								rel(getSpace(), widthOtherComp == widthFirstStageComp);
								rel(getSpace(), lengthOtherComp == lengthFirstStageComp);

							}
						}

					}
				}
			}
		}


		void SizingRulesConstraints::addSymmetricalCompensationResistorToMap()
		{
			if(getPartitioningResult().hasResistorParts())
			{
				if(getPartitioningResult().getSecondStages().size() == 2)
				{
					Partitioning::Component* resTran1 = nullptr;
					Partitioning::Component* resTran2 = nullptr;

					for(auto & res : getPartitioningResult().getAllResistorParts())
					{
						if(res->isCompensationResistor())
						{
							Partitioning::Component * resComp = *getPartitioningResult().getBelongingComponents(*res).begin();
							if(resComp->isTransistor())
							{
								if(resTran1 == nullptr)
									resTran1 = resComp;
								else
									resTran2 = resComp;
							}
						}
					}

					if(resTran1 != nullptr && resTran2 != nullptr)
					{
						equalWidthMap_.add({resTran1, resTran2}, getPartitioningResult());
						equalLengthMap_.add({resTran1, resTran2}, getPartitioningResult());
					}
				}
			}
		}

		void SizingRulesConstraints::addSymmetricalPositiveFeedbackPartConstraintsToMap()
		{
			if(getPartitioningResult().hasPositiveFeedbackParts())
			{
				for(auto posFeedbackPart : getPartitioningResult().getAllPositiveFeedbackParts())
				{
					if(getPartitioningResult().getBelongingComponents(*posFeedbackPart).size() == 2
							|| getPartitioningResult().getBelongingComponents(*posFeedbackPart).size() == 4 )
					{
						std::vector<Partitioning::Component *> comps = getPartitioningResult().getBelongingComponents(*posFeedbackPart);
						equalWidthMap_.add(comps, getPartitioningResult());
						equalLengthMap_.add(comps, getPartitioningResult());
					}
				}
			}
		}

		void SizingRulesConstraints::createSymmericalCommonModeSignalDetector()
		{
			if(getPartitioningResult().hasCommonModeSignalDetectorParts())
			{
				for(auto & detectPart : getPartitioningResult().getAllCommonModeSignalDetectorParts())
				{
					Partitioning::Component * res1 = nullptr;
					Partitioning::Component * res2 = nullptr;
					Partitioning::Component * cap1 = nullptr;
					Partitioning::Component * cap2 = nullptr;

					for(auto & comp : getPartitioningResult().getBelongingComponents(*detectPart))
					{
						if(comp->getArray().getStructureName().toStr() == "ResistorArray")
						{
							if(res1 == nullptr)
								res1 = comp;
							else
								res2 = comp;
						}
						else if (comp->getArray().getStructureName().toStr() == "CapacitorArray")
						{
							if(cap1 == nullptr)
								cap1 = comp;
							else
								cap2 = comp;
						}
					}

					assert(res1 != nullptr && res2 != nullptr, "A common mode signal detector should have at least two transistors!");

					Gecode::IntVar res1Value = getTwoPortToValueMap().find(*res1);
					Gecode::IntVar res2Value = getTwoPortToValueMap().find(*res2);
					rel(getSpace(), res1Value == res2Value);

					if(cap1 != nullptr && cap2 != nullptr)
					{
						Gecode::IntVar cap1Value = getTwoPortToValueMap().find(*cap1);
						Gecode::IntVar cap2Value = getTwoPortToValueMap().find(*cap2);
						rel(getSpace(), cap1Value == cap2Value);
					}


				}
			}
		}



	}
