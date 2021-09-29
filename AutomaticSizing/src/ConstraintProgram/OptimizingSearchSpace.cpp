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

#include "AutomaticSizing/incl/ConstraintProgram/OptimizingSearchSpace.h"

#include "AutomaticSizing/incl/ConstraintProgram/Constraints/CircuitSpecificationsConstraints.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Other.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"

#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"

#include "AutomaticSizing/incl/ConstraintProgram/NetToIntVarMap.h"

#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

	const float OptimizingSearchSpace::NOT_INITIALIZED_ = -1;

	OptimizingSearchSpace::OptimizingSearchSpace()
	{
	}

	OptimizingSearchSpace::~OptimizingSearchSpace()
	{
	}

	OptimizingSearchSpace::OptimizingSearchSpace(OptimizingSearchSpace& other) : SearchSpace(other)
	{
	}

	 OptimizingSearchSpace* OptimizingSearchSpace::copy()
	{
		return new OptimizingSearchSpace(*this);
	}



	void OptimizingSearchSpace::constrain(const Gecode::Space& other)
	{
		const OptimizingSearchSpace& otherSpace = static_cast<const OptimizingSearchSpace&>(other);

		if(otherSpace.fulfillsSpecifications())
		Gecode::rel(*this, cost_ >= otherSpace.cost_ * 1.01);

//		logDebug("Fulfills First stage spec ? " << fulfillsFirstStageSpecifications());
//		logDebug("Fullfils all specs? " << fulfillsSpecifications());

		//Get OpAmp to fullfilled Specs relevant for first stage
		if(!fulfillsFirstStageSpecifications())
		{
			logDebug("Optimizing first stage specs!");
		if(getPhaseMargin() < getCircuitInformation().getCircuitSpecification().getPhaseMargin())
		{
			int phaseMarginIndex = getVariables().findOther(PHASE_MARGIN_).getIndex();
			Gecode::IntVar phaseMargin = variables_[phaseMarginIndex];
			Gecode::IntVar phaseMarginOther = otherSpace.variables_[phaseMarginIndex];
			Gecode::rel(*this, phaseMargin >= phaseMarginOther);
		}
		if(getMinCommonModeInputVoltage()>
		getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() +getCircuitInformation().getCircuitSpecification().getMinCommonModeInputVoltage())
		{
			int vcmMinIndex = getVariables().findOther(VCM_MIN_).getIndex();
			Gecode::IntVar vcmMin = variables_[vcmMinIndex];
			Gecode::IntVar vcmMinOther = otherSpace.variables_[vcmMinIndex];
			Gecode::rel(*this, vcmMin <= vcmMinOther);
		}
		if(getMaxCommonModeInputVoltage()<
				getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() + getCircuitInformation().getCircuitSpecification().getMaxCommonModeInputVoltage())
		{
			int vcmMaxIndex = getVariables().findOther(VCM_MAX_).getIndex();
			Gecode::IntVar vcmMax = variables_[vcmMaxIndex];
			Gecode::IntVar vcmMaxOther = otherSpace.variables_[vcmMaxIndex];
			Gecode::rel(*this, vcmMax >= vcmMaxOther);
		}
		if(getCMRR()< getCircuitInformation().getCircuitSpecification().getCMRR())
		{
			int CMRRIndex = getVariables().findOther(CMRR_).getIndex();
			Gecode::IntVar CMRR = variables_[CMRRIndex];
			Gecode::IntVar CMRROther = otherSpace.variables_[CMRRIndex];
			Gecode::rel(*this, CMRR >= CMRROther);
		}
		if(getArea() > getCircuitInformation().getCircuitSpecification().getMaxArea())
		{
			int areaIndex = getVariables().findOther(AREA_).getIndex();
			Gecode::IntVar area = variables_[areaIndex];
			Gecode::IntVar areaOther = otherSpace.variables_[areaIndex];
			Gecode::rel(*this, area <= areaOther);
		}
		if(getPowerConsumption() > getCircuitInformation().getCircuitSpecification().getPowerConsumption())
		{
			int powerIndex = getVariables().findOther(POWER_).getIndex();
			Gecode::IntVar power = variables_[powerIndex];
			Gecode::IntVar powerOther = otherSpace.variables_[powerIndex];
			Gecode::rel(*this, power <= powerOther);
		}
		}

		//Get the opAmp to fullfill All Specifications
		if(otherSpace.fulfillsFirstStageSpecifications())
		{
//			logDebug("optimizing second stage spec!!!")
		if(getGain()< getCircuitInformation().getCircuitSpecification().getGain())
		{
			int gainIndex = getVariables().findOther(GAIN_).getIndex();
			Gecode::IntVar gain = variables_[gainIndex];
			Gecode::IntVar gainOther = otherSpace.variables_[gainIndex];
			Gecode::rel(*this, gain >= gainOther);
		}
		if(getMinOutputVoltage()> getCircuitInformation().getCircuitSpecification().getMinOutputVoltage())
		{
			int voutMinIndex = getVariables().findOther(VOUT_MIN_).getIndex();
			Gecode::IntVar voutMin = variables_[voutMinIndex];
			Gecode::IntVar voutMinOther = otherSpace.variables_[voutMinIndex];
			Gecode::rel(*this, voutMin <= voutMinOther);
		}
		if(getMaxOutputVoltage()< getCircuitInformation().getCircuitSpecification().getMaxOutputVoltage())
		{
			int voutMaxIndex = getVariables().findOther(VOUT_MAX_).getIndex();
			Gecode::IntVar voutMax = variables_[voutMaxIndex];
			Gecode::IntVar voutMaxOther = otherSpace.variables_[voutMaxIndex];
			Gecode::rel(*this, voutMax >= voutMaxOther);
		}
		if(getTransitFrequency()< getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
			int transitFrequencyIndex = getVariables().findOther(TRANSIT_FREQUENCY_).getIndex();
			Gecode::IntVar transitFrequency = variables_[transitFrequencyIndex];
			Gecode::IntVar transitFrequencyOther = otherSpace.variables_[transitFrequencyIndex];
			Gecode::rel(*this, transitFrequency >= transitFrequencyOther);
		}
		if(getTransitFrequencyWithErrorFactor()< getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
			int transitFrequencyIndex = getVariables().findOther(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_).getIndex();
			Gecode::IntVar transitFrequency = variables_[transitFrequencyIndex];
			Gecode::IntVar transitFrequencyOther = otherSpace.variables_[transitFrequencyIndex];
			Gecode::rel(*this, transitFrequency >= transitFrequencyOther);
		}

		if(getSlewRate()< getCircuitInformation().getCircuitSpecification().getSlewRate())
		{
			int slewRateIndex = getVariables().findOther(SLEW_RATE_).getIndex();
			Gecode::IntVar slewRate = variables_[slewRateIndex];
			Gecode::IntVar slewRateOther = otherSpace.variables_[slewRateIndex];
			Gecode::rel(*this, slewRate > slewRateOther);
		}
		if(getNegPSRR()< getCircuitInformation().getCircuitSpecification().getNegPSRR())
		{
			int negPSRRIndex = getVariables().findOther(NEG_PSRR_).getIndex();
			Gecode::IntVar negPSRR = variables_[negPSRRIndex];
			Gecode::IntVar negPSRROther = otherSpace.variables_[negPSRRIndex];
			Gecode::rel(*this, negPSRR >= negPSRROther);
		}
		if(getPosPSRR()< getCircuitInformation().getCircuitSpecification().getPosPSRR())
		{
			int posPSRRIndex = getVariables().findOther(POS_PSRR_).getIndex();
			Gecode::IntVar posPSRR = variables_[posPSRRIndex];
			Gecode::IntVar posPSRROther = otherSpace.variables_[posPSRRIndex];
			Gecode::rel(*this, posPSRR >= posPSRROther);
		}

		}

		//Trying to get the output voltage of first stage as equal as possible
		if(otherSpace.fulfillsSpecifications())
		{
		Partitioning::Component * compN1 = nullptr;
		Partitioning::Component * compN2 = nullptr;
		Partitioning::Component * compP1 = nullptr;
		Partitioning::Component * compP2 = nullptr;


		std::vector<Partitioning::Component* > comps = getPartitioningResult().getBelongingComponents(getPartitioningResult().getFirstStage());
		for(auto & comp : comps)
		{
			if(compN1 == nullptr && comp->getArray().getTechType().isN())
			{
				compN1 = comp;
			}
			else if(compN2 == nullptr && comp->getArray().getTechType().isN())
			{
				compN2 = comp;
			}
			else if(compP1 == nullptr && comp->getArray().getTechType().isP())
			{
				compP1 = comp;
			}
			else if(compP2 == nullptr && comp->getArray().getTechType().isP())
			{
				compP2 = comp;
			}
			else
			{
				assert(false, " There should be two nmos components and/or two pmos components  in the first stage but nothing more!");
			}
		}

		assert((compN1 != nullptr && compN2 != nullptr)
				|| (compP1 != nullptr && compP2 != nullptr), " There should be two nmos components and/or two pmos components  in the first stage!");

		if((compN1 != nullptr && compN2 != nullptr))
		{
			Core::NetId outputNetFirstStage1 = compN1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();
			Core::NetId outputNetFirstStage2 = compN2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();


		 	Gecode::IntVar outputNetFirstStage1Voltage = netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2Voltage = netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::IntVar outputNetFirstStage1VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::rel(*this, abs(outputNetFirstStage1Voltage - outputNetFirstStage2Voltage) <= abs(outputNetFirstStage1VoltageOther - outputNetFirstStage2VoltageOther));
		}

		if((compP1 != nullptr && compP2 != nullptr))
		{
			Core::NetId outputNetFirstStage1 = compP1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();
			Core::NetId outputNetFirstStage2 = compP2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();


		 	Gecode::IntVar outputNetFirstStage1Voltage = netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2Voltage = netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::IntVar outputNetFirstStage1VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::rel(*this, abs(outputNetFirstStage1Voltage - outputNetFirstStage2Voltage) <= abs(outputNetFirstStage1VoltageOther - outputNetFirstStage2VoltageOther));
		}
		}


	}

	bool OptimizingSearchSpace::master(const Gecode::MetaInfo & mi)
	{
		if(mi.last() != NULL)
		{
			constrain(*mi.last());
		}
		if(mi.solution() == 0)
		{
			mi.nogoods().post(*this);
		}
		return true;
	}


	void OptimizingSearchSpace::createCircuitSpecificationConstraints()
	{
		CircuitSpecificationsConstraints specificationsConstraints;
		specificationsConstraints.setTwoPortToValueMap(twoPortToValueMap_);
		specificationsConstraints.setCircuitInformation(getCircuitInformation());
		specificationsConstraints.setGraph(getGraph());
		specificationsConstraints.setNetToVoltageMap(netToVoltageMap_);
		specificationsConstraints.setPartitioningResult(getPartitioningResult());
		specificationsConstraints.setSpace(*this);
		specificationsConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
		specificationsConstraints.setTransistorToLengthMap(transistorToLengthMap_);
		specificationsConstraints.setTransistorToWidthMap(transistorToWidthMap_);
		specificationsConstraints.setTransistorModel(getTransistorModel());
		specificationsConstraints.setStructureRecognitionResult(getStructureRecognitionResult());
		specificationsConstraints.createOptimizingConstraints();


	}



}

