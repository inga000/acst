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

#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"

#include "AutomaticSizing/incl/ConstraintProgram/SearchSpace.h"

#include <vector>

#include "Partitioning/incl/Results/Component.h"
#include "AutomaticSizing/incl/ConstraintProgram/Constraints/KCLConstraints.h"
#include "AutomaticSizing/incl/ConstraintProgram/Constraints/TransistorConstraintsEKV.h"
#include "AutomaticSizing/incl/ConstraintProgram/Constraints/TransistorConstraintsSHM.h"
#include "AutomaticSizing/incl/ConstraintProgram/Constraints/PolesAndZeros.h"
#include "AutomaticSizing/incl/ConstraintProgram/Constraints/SizingRulesConstraints.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Graph.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Node.h"
#include "AutomaticSizing/incl/ConstraintProgram/Graph/Edge.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Other.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/TwoPort.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/PoleOrZero.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Specification.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Voltage.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/CircuitInformation/LoadCapacity.h"

#include "AutomaticSizing/incl/Results/Result.h"

#include "GecodeExtensions/incl/Brancher/TransistorBrancher.h"
#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancher.h"
#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancherSupportingTransistorDomain.h"
#include "GecodeExtensions/incl/Brancher/TransistorBrancherIntervalSplitting.h"
#include "GecodeExtensions/incl/Brancher/TransistorBrancherOnlyVariableDomain.h"

#include "Partitioning/incl/Results/Result.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"

namespace AutomaticSizing {

	const float SearchSpace::NOT_INITIALIZED_ = -1;
	const std::string SearchSpace::GAIN_ = "gain";
	const std::string SearchSpace::FEEDBACK_GAIN_ = "feedbackGain";
	const std::string SearchSpace::POWER_ = "power";
	const std::string SearchSpace::POWER_OP_AMP_ = "powerOpAmp";
	const std::string SearchSpace::POWER_FEEDBACK_ = "powerFeedback";
	const std::string SearchSpace::TRANSIT_FREQUENCY_ = "transitFrequency";
	const std::string SearchSpace::FEEDBACK_TRANSIT_FREQUENCY_ = "feedbackTransitFrequency";
	const std::string SearchSpace::TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_ = "transitFrequencyWithErrorFactor";
	const std::string SearchSpace::SLEW_RATE_ = "slewRate";
	const std::string SearchSpace::SETTLING_TIME_ = "settlingTime";
	const std::string SearchSpace::CMRR_ = "CMRR";
	const std::string SearchSpace::FEEDBACK_CMRR_ = "feedbackCMRR";
	const std::string SearchSpace::NEG_PSRR_ = "negPSRR";
	const std::string SearchSpace::POS_PSRR_ = "posPSRR";
	const std::string SearchSpace::VOUT_MAX_ = "voutMax";
	const std::string SearchSpace::VOUT_MIN_ = "voutMin";
	const std::string SearchSpace::VCM_MIN_ = "vcmMin";
	const std::string SearchSpace::VCM_MAX_ = "vcmMax";
	const std::string SearchSpace::FEEDBACK_VCM_MIN_ = "feedbackVcmMin";
	const std::string SearchSpace::FEEDBACK_VCM_MAX_ = "feedbackVcmMax";
	const std::string SearchSpace::AREA_ = "area";
	const std::string SearchSpace::PHASE_MARGIN_ = "phaseMargin";
	const std::string SearchSpace::FEEDBACK_PHASE_MARGIN_ = "feedbackPhaseMargin";

	SearchSpace::~SearchSpace()
	{

	}

	SearchSpace::SearchSpace() :
				result_(nullptr),
				widthUpperBound_(NOT_INITIALIZED_),
				lengthUpperBound_(NOT_INITIALIZED_),
				scalingFactorMUM_(NOT_INITIALIZED_),
				circuitInformation_(nullptr),
				graph_(nullptr),
				structRecResult_(nullptr),
				polesAndZeros_(nullptr),
				variablesMap_(nullptr),
				usesHSpiceLibrary_(false)
	{
	}

	SearchSpace::SearchSpace(SearchSpace& other) : Space(other)
	{
		result_ = other.result_;
		circuitInformation_ = other.circuitInformation_;
		graph_ = other.graph_;
		structRecResult_ = other.structRecResult_;
		sizingRulesConstraints_ = other.sizingRulesConstraints_;
		transistorModel_ = other.transistorModel_;
		definition_ = other.definition_;
		usesHSpiceLibrary_ = other.usesHSpiceLibrary_;
		polesAndZeros_ = other.polesAndZeros_;
		variablesMap_ = other.variablesMap_;

		lengthUpperBound_ = other.lengthUpperBound_;
		widthUpperBound_ = other.widthUpperBound_;
		scalingFactorMUM_ = other.scalingFactorMUM_;
		indexToVariableMap_ = other.indexToVariableMap_;

		variables_.update(*this, other.variables_);
		cost_.update(*this, other.cost_);
		helper_.update(*this, other.helper_);

//		logDebug("transistorVariable: " << other.transistorVariables_);

//		logDebug("Cost " << other.cost_);
		createMaps();
	}


	std::string SearchSpace::toStr() const
	{
		std::ostringstream oss;
		oss << "Width: " << transistorToWidthMap_.toStr() << std::endl;
		oss << "Length: " << transistorToLengthMap_.toStr() << std::endl;
		oss << "TwoPorts: " << twoPortToValueMap_.toStr() << std::endl;
		oss << "Voltages: " << netToVoltageMap_.toStr() << std::endl;
		oss << "Currents: " << transistorToCurrentMap_.toStr() << std::endl;
		int gainIndex = getVariables().findOther(GAIN_).getIndex();
		oss << "Gain: " << variables_[gainIndex] << std::endl;
		int powerIndex = getVariables().findOther(POWER_).getIndex();
		oss << "Power: " << variables_[powerIndex] << std::endl;
		int transitFrequencyIndex = getVariables().findOther(TRANSIT_FREQUENCY_).getIndex();
		oss << "TransitFrequency: " << variables_[transitFrequencyIndex] << std::endl;
		int transitFrequencyIndexWithErrorFactor = getVariables().findOther(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_).getIndex();
		oss << "TransitFrequencyWithErrorFactor: " << variables_[transitFrequencyIndexWithErrorFactor] << std::endl;
		int slewRateIndex = getVariables().findOther(SLEW_RATE_).getIndex();
		oss << "SlewRate: " << variables_[slewRateIndex] << std::endl;
		int settlingTimeIndex = getVariables().findOther(SETTLING_TIME_).getIndex();
		oss << "SettlingTime: " << variables_[settlingTimeIndex] << std::endl;
		int CMRRIndex = getVariables().findOther(CMRR_).getIndex();
		oss << "CMRR: " << variables_[CMRRIndex] << std::endl;
		int negPSRRIndex = getVariables().findOther(NEG_PSRR_).getIndex();
		oss << "NegPSRR: " << variables_[negPSRRIndex] << std::endl;
		int posPSRRIndex = getVariables().findOther(POS_PSRR_).getIndex();
		oss << "PosPSRR: " << variables_[posPSRRIndex] << std::endl;
		int voutMaxIndex = getVariables().findOther(VOUT_MAX_).getIndex();
		oss << "VoutMax: " << variables_[voutMaxIndex] << std::endl;
		int voutMinIndex = getVariables().findOther(VOUT_MIN_).getIndex();
		oss << "VoutMin: " << variables_[voutMinIndex] << std::endl;
		int vcmMinIndex = getVariables().findOther(VCM_MIN_).getIndex();
		oss << "VcmMin: " << variables_[vcmMinIndex] << std::endl;
		int vcmMaxIndex = getVariables().findOther(VCM_MAX_).getIndex();
		oss << "VcmMax: " << variables_[vcmMaxIndex] << std::endl;
		int areaIndex = getVariables().findOther(AREA_).getIndex();
		oss << "Area: " << variables_[areaIndex] << std::endl;
		int phaseMarginIndex = getVariables().findOther(PHASE_MARGIN_).getIndex();
		oss << "PhaseMargin: " << variables_[phaseMarginIndex] << std::endl;
		oss << std::endl;


		if(getPartitioningResult().hasFeedbackStage())
		{
			oss << "Feedback circuit" << std::endl;
			int feedbackGainIndex = getVariables().findOther(FEEDBACK_GAIN_).getIndex();
			oss << "FeedbackGain: " << variables_[feedbackGainIndex] << std::endl;
			int powerOpAmpIndex = getVariables().findOther(POWER_OP_AMP_).getIndex();
			oss << "PowerOpAmp: " << variables_[powerOpAmpIndex] << std::endl;
			int powerFeedbackIndex = getVariables().findOther(POWER_FEEDBACK_).getIndex();
			oss << "PowerFeedback: " << variables_[powerFeedbackIndex] << std::endl;
			int feedbackTransitFrequencyIndex = getVariables().findOther(FEEDBACK_TRANSIT_FREQUENCY_).getIndex();
			oss << "FeedbackTransitFrequency: " << variables_[feedbackTransitFrequencyIndex] << std::endl;
			int feedbackPhaseMarginIndex = getVariables().findOther(FEEDBACK_PHASE_MARGIN_).getIndex();
			oss << "FeedbackPhaseMargin: " << variables_[feedbackPhaseMarginIndex] << std::endl;
			int feedbackCMRRIndex = getVariables().findOther(FEEDBACK_CMRR_).getIndex();
			oss << "FeedbackCMRR: " << variables_[feedbackCMRRIndex] << std::endl;
			int feedbackVcmMinIndex = getVariables().findOther(FEEDBACK_VCM_MIN_).getIndex();
			int feedbackVcmMaxIndex = getVariables().findOther(FEEDBACK_VCM_MAX_).getIndex();
			oss << "<Feedback input voltage range: " << "VinMin: " << variables_[feedbackVcmMinIndex] <<  " VinMax: " << variables_[feedbackVcmMaxIndex] << std::endl;
		}

		oss << "Poles: " << std::endl;
		oss << "DominantPole " << variables_[getPolesAndZeros().getDominantPoleIndex()] << std::endl;// / 100
		oss << "PositiveZero " << variables_[getPolesAndZeros().getPositiveZeroIndex()] << std::endl;
		oss << "ImportantNonDominantPoles: ";
		int i = 0;
		for(auto & index : getPolesAndZeros().getImportantNonDominantPolesIndexVector())
		{
			oss << variables_[index]  << ", ";
			i++;
		}
		oss << std::endl;
		oss << "UnimportantNonDominantPoles: ";
		for(auto & index : getPolesAndZeros().getUnimportantNonDominantPolesIndexVector())
		{
			 oss << variables_[index] << ", ";
		}
		oss << std::endl;
		oss << "ImportantPositiveZeros: ";
		for(auto & index : getPolesAndZeros().getImportantZerosIndexVector())
		{
			 oss << variables_[index] << ", ";
		}
		oss << std::endl;
		oss << "UnimportantPositiveZeros: ";
		for(auto & index : getPolesAndZeros().getUnimportantZerosIndexVector())
		{
			 oss << variables_[index]  << ", ";
		}
		oss << std::endl;


		oss << "Helper: " << helper_ << std::endl;
		return oss.str();
	}


	std::string SearchSpace::printIndexToVariableMap() const
	{
		std::ostringstream oss;

		for(auto & it : indexToVariableMap_)
		{
			int index = it.first;
			std::string variable = it.second;

			oss << "Index: " << index;
			oss << "        Variable: " << variable << std::endl;
		}

		return oss.str();
	}

	bool SearchSpace::master(const Gecode::MetaInfo & mi)
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

	float SearchSpace::getGain() const
	{
		int gainIndex = getVariables().findOther(GAIN_).getIndex();
		return Gecode::FloatNum(variables_[gainIndex].min());
	}


	float SearchSpace::getBestCircuitPerformance() const
	{
		return Gecode::FloatNum(cost_.med());
	}

	float SearchSpace::getPowerConsumption() const
	{
		int powerIndex = getVariables().findOther(POWER_).getIndex();

		return Gecode::FloatNum(variables_[powerIndex].min())* pow(10,-6);
	}

	float SearchSpace::getTransitFrequency() const
	{
		int transitFrequencyIndex = getVariables().findOther(TRANSIT_FREQUENCY_).getIndex();
		return Gecode::FloatNum(variables_[transitFrequencyIndex].min())* pow(10,3);
	}

	float SearchSpace::getTransitFrequencyWithErrorFactor() const
	{
		int transitFrequencyIndex = getVariables().findOther(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_).getIndex();
		return Gecode::FloatNum(variables_[transitFrequencyIndex].min());
	}

	float SearchSpace::getPhaseMargin() const
	{
		int phaseMarginIndex = getVariables().findOther(PHASE_MARGIN_).getIndex();
		return Gecode::FloatNum(variables_[phaseMarginIndex].min())/100;
	}

	float SearchSpace::getArea() const
	{
		int areaIndex = getVariables().findOther(AREA_).getIndex();
		return Gecode::FloatNum(variables_[areaIndex].min());
	}

	float SearchSpace::getSlewRate() const
	{
		int slewRateIndex = getVariables().findOther(SLEW_RATE_).getIndex();
		return Gecode::FloatNum(variables_[slewRateIndex].min());
	}

	float SearchSpace::getSettlingTime() const
	{
		int settlingTimeIndex = getVariables().findOther(SETTLING_TIME_).getIndex();
		return Gecode::FloatNum(variables_[settlingTimeIndex].min());
	}

	float SearchSpace::getMaxOutputVoltage() const
	{
		int voutMaxIndex = getVariables().findOther(VOUT_MAX_).getIndex();
		return Gecode::FloatNum(variables_[voutMaxIndex].min())/100;
	}

	float SearchSpace::getMinOutputVoltage() const
	{
		int voutMinIndex = getVariables().findOther(VOUT_MIN_).getIndex();
		return Gecode::FloatNum(variables_[voutMinIndex].min())/100;
	}

	float SearchSpace::getMaxCommonModeInputVoltage() const
	{
		int vcmMaxIndex = getVariables().findOther(VCM_MAX_).getIndex();
		return Gecode::FloatNum(variables_[vcmMaxIndex].max())/100;
	}

	float SearchSpace::getMinCommonModeInputVoltage() const
	{
		int vcmMinIndex = getVariables().findOther(VCM_MIN_).getIndex();
		return Gecode::FloatNum(variables_[vcmMinIndex].max())/100;
	}


	void SearchSpace::setResult(const Partitioning::Result& result)
	{
		result_ = & result;
	}

	void SearchSpace::setGain(Gecode::FloatVar gain)
	{
		Gecode::IntVar intGain(*this, 0, 200);
		Gecode::FloatVar floatGain(*this, 0, 200);
		channel(*this, intGain, floatGain);
		rel(*this, floatGain >= gain -0.5);
		rel(*this, floatGain <= gain+0.5);

		rel(*this, floatGain <= getCircuitInformation().getCircuitSpecification().getGain() + 150);
		rel(*this, variables_[getVariables().findOther(GAIN_).getIndex()] == intGain);
	}

	void SearchSpace::setFeedbackGain(Gecode::FloatVar feedbackGain)
	{
		Gecode::IntVar intGain(*this, 0, 200);
		Gecode::FloatVar floatGain(*this, 0, 200);
		channel(*this, intGain, floatGain);
		rel(*this, floatGain >= feedbackGain -0.5);
		rel(*this, floatGain <= feedbackGain+0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_GAIN_).getIndex()] == intGain);
	}

	void SearchSpace::setPowerConsumption(Gecode::FloatVar power)
	{
		Gecode::IntVar intPower(*this, 0, pow(10,9));
		Gecode::FloatVar floatPower(*this, 0, 100000000000000);
		channel(*this, intPower, floatPower);
		rel(*this, floatPower >= power * pow(10,6) -0.5 );
		rel(*this, floatPower <= power * pow(10,6) +0.5);

		rel(*this, variables_[getVariables().findOther(POWER_).getIndex()] == intPower);
	}

	void SearchSpace::setPowerConsumptionOpAmp(Gecode::FloatVar power)
	{
		Gecode::IntVar intPower(*this, 0, pow(10,9));
		Gecode::FloatVar floatPower(*this, 0, 10000000000);
		channel(*this, intPower, floatPower);
		rel(*this, floatPower >= power * pow(10,6) -0.5);
		rel(*this, floatPower <= power * pow(10,6) +0.5);

		rel(*this, variables_[getVariables().findOther(POWER_OP_AMP_).getIndex()] == intPower);
	}

	void SearchSpace::setPowerConsumptionFeedback(Gecode::FloatVar power)
	{
		Gecode::IntVar intPower(*this, 0, pow(10,9));
		Gecode::FloatVar floatPower(*this, 0, 1000000000000);
		channel(*this, intPower, floatPower);
		rel(*this, floatPower >= power * pow(10,6) -0.5);
		rel(*this, floatPower <= power * pow(10,6) +0.5);

		rel(*this, variables_[getVariables().findOther(POWER_FEEDBACK_).getIndex()] == intPower);
	}

	void SearchSpace::setTransitFrequency(Gecode::FloatVar ft)
	{
		dom(*this, ft, 0, pow(10,9));
		Gecode::IntVar intFt(*this, 0, pow(10,9));
		Gecode::FloatVar floatFt(*this, 0, pow(10,9));
		channel(*this, intFt, floatFt);
		rel(*this, floatFt >= ft * pow(10, -3)  -0.5);
		rel(*this, floatFt <= ft * pow(10,-3) +0.5);
		rel(*this, variables_[getVariables().findOther(TRANSIT_FREQUENCY_).getIndex()] == intFt);

	}

	void SearchSpace::setFeedbackTransitFrequency(Gecode::FloatVar ft)
	{
		Gecode::IntVar intFt(*this, 0, 100000000000);
		Gecode::FloatVar floatFt(*this, 0, 100000000000000);
		channel(*this, intFt, floatFt);
		rel(*this, floatFt >= ft * pow(10, -3) -0.5);
		rel(*this, floatFt <= ft * pow(10, -3) +0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_TRANSIT_FREQUENCY_).getIndex()] == intFt);
	}

	void SearchSpace::setTransitFrequencyWithErrorFactor(Gecode::FloatVar ft)
	{

		dom(*this, ft, 0, 1000000000000000);
		Gecode::IntVar intFt(*this, 0, 10000000000000);
		Gecode::FloatVar floatFt(*this, 0, 1000000000000);
		channel(*this, intFt, floatFt);
		rel(*this, floatFt >= ft  -0.5);
		rel(*this, floatFt <= ft  +0.5);

		rel(*this, variables_[getVariables().findOther(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_).getIndex()] == intFt);
	}


	void SearchSpace::setSlewRate(Gecode::FloatVar slewRate)
	{
		Gecode::IntVar intSlewRate(*this, 0, 100000000000);
		Gecode::FloatVar floatSlewRate(*this, 0, 10000000000000000);
		channel(*this, intSlewRate, floatSlewRate);
		rel(*this, floatSlewRate >= slewRate  -0.5);
		rel(*this, floatSlewRate <= slewRate  +0.5);

		rel(*this, variables_[getVariables().findOther(SLEW_RATE_).getIndex()] == intSlewRate);
	}

	void SearchSpace::setSettlingTime(Gecode::FloatVar settlingTime)
	{

		Gecode::IntVar intSettlingTime(*this, -1000000000, 1000000000);
		Gecode::FloatVar floatSettlingTime(*this, -1000000000, 1000000000);
		channel(*this, intSettlingTime, floatSettlingTime);
		rel(*this, (floatSettlingTime < 0 && settlingTime <0) && (floatSettlingTime > 0 && settlingTime > 0));
		rel(*this, abs(floatSettlingTime) >=  abs(settlingTime) * pow(10, 6)  -0.5);
		rel(*this, abs(floatSettlingTime) <= abs(settlingTime) * pow(10,6)  +0.5);

		rel(*this, variables_[getVariables().findOther(SETTLING_TIME_).getIndex()] == intSettlingTime);

	}

	void SearchSpace::setCMRR(Gecode::FloatVar CMRR)
	{
		Gecode::IntVar intCMRR(*this, 0, 300);
		Gecode::FloatVar floatCMRR(*this, 0, 300);
		channel(*this, intCMRR, floatCMRR);
		rel(*this, floatCMRR >= CMRR  -0.5);
		rel(*this, floatCMRR <= CMRR  +0.5);

		rel(*this, variables_[getVariables().findOther(CMRR_).getIndex()] == intCMRR);
	}

	void SearchSpace::setFeedbackCMRR(Gecode::FloatVar CMRR)
	{
		Gecode::IntVar intCMRR(*this, 0, 300);
		Gecode::FloatVar floatCMRR(*this, 0, 300);
		channel(*this, intCMRR, floatCMRR);
		rel(*this, floatCMRR >= CMRR  -0.5);
		rel(*this, floatCMRR <= CMRR  +0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_CMRR_).getIndex()] == intCMRR);
	}

	void SearchSpace::setNegPSRR(Gecode::FloatVar negPSRR)
	{
		Gecode::IntVar intPSRR(*this, 0, 300);
		Gecode::FloatVar floatPSRR(*this, 0, 300);
		channel(*this, intPSRR, floatPSRR);
		rel(*this, floatPSRR >= negPSRR  -0.5);
		rel(*this, floatPSRR <= negPSRR  +0.5);

		rel(*this, variables_[getVariables().findOther(NEG_PSRR_).getIndex()] == intPSRR);
	}

	void SearchSpace::setPosPSRR(Gecode::FloatVar posPSRR)
	{
		Gecode::IntVar intPSRR(*this, 0, 300);
		Gecode::FloatVar floatPSRR(*this, 0, 300);
		channel(*this, intPSRR, floatPSRR);
		rel(*this, floatPSRR >= posPSRR  -0.5);
		rel(*this, floatPSRR <= posPSRR  +0.5);

		rel( *this, variables_[getVariables().findOther(POS_PSRR_).getIndex()] == intPSRR);
	}


	void SearchSpace::setMaximumOutputVoltage(Gecode::FloatVar voutMax)
	{
		Gecode::IntVar intVout(*this, -10*100,10*100);
		Gecode::FloatVar floatVout(*this, -10*100, 10*100);


		channel(*this, intVout, floatVout);
		rel(*this, (voutMax > 0 && floatVout > 0) || (voutMax < 0 && floatVout < 0));
		rel(*this, abs(floatVout) >= abs(voutMax) *100  -0.5);
		rel(*this, abs(floatVout) <= abs(voutMax) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(VOUT_MAX_).getIndex()] == intVout);
	}

	void SearchSpace::setMinimumOutputVoltage(Gecode::FloatVar voutMin)
	{
		Gecode::IntVar intVout(*this, -10*100,10*100 );
		Gecode::FloatVar floatVout(*this, -10*100, 10*100);
		channel(*this, intVout, floatVout);
		rel(*this, (voutMin > 0 && floatVout > 0) || (voutMin < 0 && floatVout < 0));
		rel(*this, abs(floatVout) >= abs(voutMin) *100  -0.5);
		rel(*this, abs(floatVout) <= abs(voutMin) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(VOUT_MIN_).getIndex()] == intVout);
	}

	void SearchSpace::setMinimumCommonModeInputVoltage(Gecode::FloatVar vcmMin)
	{
		Gecode::IntVar intVcm(*this, -10*100,10 *100);
		Gecode::FloatVar floatVcm(*this, -10*100, 10*100);


		channel(*this, intVcm, floatVcm);
		rel(*this, (vcmMin > 0 && floatVcm > 0) || (vcmMin < 0 && floatVcm < 0));
		rel(*this, abs(floatVcm) >= abs(vcmMin) *100  -0.5);
		rel(*this, abs(floatVcm) <= abs(vcmMin) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(VCM_MIN_).getIndex()] == intVcm);
	}

	void SearchSpace::setMaximumCommonModeInputVoltage(Gecode::FloatVar vcmMax)
	{
		Gecode::IntVar intVcm(*this, -10*100,10 *100);
		Gecode::FloatVar floatVcm(*this, -10*100, 10*100);

		channel(*this, intVcm, floatVcm);
		rel(*this, (vcmMax > 0 && floatVcm > 0) || (vcmMax < 0 && floatVcm < 0));
		rel(*this, abs(floatVcm) >= abs(vcmMax) *100  -0.5);
		rel(*this, abs(floatVcm) <= abs(vcmMax) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(VCM_MAX_).getIndex()] == intVcm);
	}

	void SearchSpace::setFeedbackMinimumInputVoltage(Gecode::FloatVar vcmMin)
	{
		Gecode::IntVar intVcm(*this, -10*100,10 *100);
		Gecode::FloatVar floatVcm(*this, -10*100, 10*100);

		channel(*this, intVcm, floatVcm);
		rel(*this, (vcmMin > 0 && floatVcm > 0) || (vcmMin < 0 && floatVcm < 0));
		rel(*this, abs(floatVcm) >= abs(vcmMin) *100  -0.5);
		rel(*this, abs(floatVcm) <= abs(vcmMin) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_VCM_MIN_).getIndex()] == intVcm);
	}

	void SearchSpace::setFeedbackMaximumInputVoltage(Gecode::FloatVar vcmMax)
	{
		Gecode::IntVar intVcm(*this, -10*100,10 *100);
		Gecode::FloatVar floatVcm(*this, -10*100, 10*100);

		channel(*this, intVcm, floatVcm);
		rel(*this, (vcmMax > 0 && floatVcm > 0) || (vcmMax < 0 && floatVcm < 0));
		rel(*this, abs(floatVcm) >= abs(vcmMax) *100  -0.5);
		rel(*this, abs(floatVcm) <= abs(vcmMax) * 100  +0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_VCM_MAX_).getIndex()] == intVcm);
	}

	void SearchSpace::setArea(Gecode::FloatVar area)
	{
		dom(*this, area, 0, 10000000000);
		Gecode::IntVar intArea(*this, 0,1000000000 );
		Gecode::FloatVar floatArea(*this, 0, 1000000000);
		channel(*this, intArea, floatArea);

		rel(*this, floatArea>= area  -0.5);
		rel(*this, floatArea <= area  +0.5);

		rel(*this, variables_[getVariables().findOther(AREA_).getIndex()] == intArea);
	}

	void SearchSpace::setPhaseMargin(Gecode::FloatVar phaseMargin)
	{
		Gecode::IntVar intPhaseMargin(*this, 0,400 );
		Gecode::FloatVar floatPhaseMargin(*this, 0, 400);
		channel(*this, intPhaseMargin, floatPhaseMargin);

		rel(*this, floatPhaseMargin>= phaseMargin * 100  -0.5);
		rel(*this, floatPhaseMargin <= phaseMargin * 100 +0.5);

		rel(*this, variables_[getVariables().findOther(PHASE_MARGIN_).getIndex()] == intPhaseMargin);
	}

	void SearchSpace::setFeedbackPhaseMargin(Gecode::FloatVar phaseMargin)
	{
		Gecode::IntVar intPhaseMargin(*this, 0,400 );
		Gecode::FloatVar floatPhaseMargin(*this, 0, 400);
		channel(*this, intPhaseMargin, floatPhaseMargin);

		rel(*this, floatPhaseMargin>= phaseMargin * 100  -0.5);
		rel(*this, floatPhaseMargin <= phaseMargin * 100 +0.5);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_PHASE_MARGIN_).getIndex()] == intPhaseMargin);
	}

	void SearchSpace::setHelper(Gecode::FloatVar helper)
	{
		helper_ = helper;
	}

	Gecode::FloatVar SearchSpace::getTransitFrequencyVariable()
	{
		Gecode::IntVar intFt(*this, 0,pow(10,9));
		Gecode::FloatVar floatFt(*this, 0, pow(10,9));
		channel(*this, intFt, floatFt);

		rel(*this, variables_[getVariables().findOther(TRANSIT_FREQUENCY_).getIndex()] * pow(10,3) == intFt);
		return floatFt;
	}

	Gecode::FloatVar SearchSpace::getFeedbackTransitFrequencyVariable()
	{
		Gecode::IntVar intFt(*this, 0,pow(10,9));
		Gecode::FloatVar floatFt(*this, 0, 100000000000);
		channel(*this, intFt, floatFt);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_TRANSIT_FREQUENCY_).getIndex()]  * pow(10,3)== intFt);
		return floatFt;
	}

	Gecode::FloatVar SearchSpace::getFeedbackGainVariable()
	{
		Gecode::IntVar intGain(*this, 0,1000 );
		Gecode::FloatVar floatGain(*this, 0, 1000);
		channel(*this, intGain, floatGain);

		rel(*this, variables_[getVariables().findOther(FEEDBACK_GAIN_).getIndex()] == intGain);
		return floatGain;
	}

	float SearchSpace::getCMRR() const
	{
		int CMRRIndex = getVariables().findOther(CMRR_).getIndex();
		return Gecode::FloatNum(variables_[CMRRIndex].min());
	}

	float SearchSpace::getNegPSRR() const
	{
		int negPSRRIndex = getVariables().findOther(NEG_PSRR_).getIndex();
		return Gecode::FloatNum(variables_[negPSRRIndex].max());
	}

	float SearchSpace::getPosPSRR() const
	{
		int posPSRRIndex = getVariables().findOther(POS_PSRR_).getIndex();
		return Gecode::FloatNum(variables_[posPSRRIndex].max());
	}

	void SearchSpace::setGraph(const Graph& graph)
	{
		graph_ = & graph;
	}

	void SearchSpace::setCircuitInformation(
		const CircuitInformation& information)
	{
		circuitInformation_ = & information;
	}

	const Partitioning::Result& SearchSpace::getPartitioningResult() const
	{
		assert(hasResult());
		return * result_;
	}

	void SearchSpace::initialize()
	{

		initializeScalingValues();


		logDebug("create Sizing Rule Constraints Maps ");
		createSizingRulesConstraints();
		logDebug("create Variables ");
		initializePolesAndZeros();
		createVariables();
		initializeVariableMap();
		createIndexes();

		logDebug(printIndexToVariableMap());

		logDebug("create Maps ");
		createMaps();
		logDebug("create Constraints ");
		createConstraints();
		initializeCost();
		initializeBranching();
	}

	void SearchSpace::initializePolesAndZeros()
	{
		PolesAndZeros * polesAndZeros = new PolesAndZeros(getPartitioningResult(), getCircuitInformation());
		setPolesAndZeros(*polesAndZeros);

	}

	void SearchSpace::initializeIntegerPolesAndZeros()
	{
		Gecode::FloatVar dominantPole = expr(*this, getPolesAndZeros().getDominantPole() * 100);
		Gecode::FloatVar dominantPoleHelper(*this, 0 , pow(10,10));
		rel(*this, dominantPoleHelper <= dominantPole + 0.5);
		rel(*this, dominantPoleHelper >= dominantPole - 0.5);

		Gecode::IntVar dominantPoleInteger(*this, 0, pow(10,9));
		channel(*this, dominantPoleInteger, dominantPoleHelper);
		rel(*this, variables_[getPolesAndZeros().getDominantPoleIndex()] == dominantPoleInteger);

		Gecode::FloatVar positiveZero = expr(*this, getPolesAndZeros().getPositiveZero());
		Gecode::FloatVar positiveZeroHelper(*this, 0 , pow(10,10));
		rel(*this, positiveZeroHelper <= positiveZero * pow(10,-3) + 0.5);
		rel(*this, positiveZeroHelper >= positiveZero * pow(10,-3) - 0.5);

		Gecode::IntVar positiveZeroInteger(*this, 0, pow(10,9));
		channel(*this, positiveZeroInteger, positiveZeroHelper);
		rel(*this, variables_[getPolesAndZeros().getPositiveZeroIndex()] == positiveZeroInteger);


		std::vector<int> importantNonDominantPolesIndexes  = getPolesAndZeros().getImportantNonDominantPolesIndexVector();
		std::vector<int>::const_iterator  importantNonDominantPoleIndex = importantNonDominantPolesIndexes.begin();
		for(auto & poles : getPolesAndZeros().getImportantNonDominantPoles())
		{
			int index =  *importantNonDominantPoleIndex;

			Gecode::FloatVar poleHelper(*this, 0, pow(10,10));
			dom(*this, poles, 0, pow(10,10));
			rel(*this, poleHelper  <= poles * pow(10,-3)+0.5);
			rel(*this, poleHelper  >= poles * pow(10,-3) -0.5);

			Gecode::IntVar intPole(*this, 0, 10000000000);
			channel(*this, poleHelper, intPole);
			rel(*this, variables_[index] == intPole);
			importantNonDominantPoleIndex++;
		}

		std::vector<int> importantPositiveZerosIndexes  = getPolesAndZeros().getImportantZerosIndexVector();
		std::vector<int>::iterator  importantPositiveZerosIndex = importantPositiveZerosIndexes.begin();
		for(auto & zeros : getPolesAndZeros().getImportantZeros())
		{
			int index = * importantPositiveZerosIndex;

			dom(*this, zeros, 0, pow(10,10));
			Gecode::FloatVar zeroHelper(*this, 0, pow(10,12));
			rel(*this, zeroHelper <= zeros *pow(10,-3) +0.5);
			rel(*this, zeroHelper  >= zeros* pow(10, -3) -0.5);

			Gecode::IntVar intZero(*this, 0, 10000000000);
			channel(*this, zeroHelper, intZero);
			rel(*this, variables_[index] == intZero);
			importantPositiveZerosIndex++;
		}

		std::vector<int> unimportantNonDominantPoleIndexes  = getPolesAndZeros().getUnimportantNonDominantPolesIndexVector();
		std::vector<int>::iterator  unimportantNonDominantPoleIndex = unimportantNonDominantPoleIndexes.begin();
		for(auto & poles : getPolesAndZeros().getUnimportantNonDominantPoles())
		{
			int index = * unimportantNonDominantPoleIndex;
			dom(*this, poles, 0, pow(10,10));
			Gecode::FloatVar poleHelper(*this, 0, pow(10,12));
			rel(*this, poleHelper <= poles * pow(10,-3) +0.5);
			rel(*this, poleHelper >= poles * pow(10,-3) -0.5);

			Gecode::IntVar intPole(*this, 0, 10000000000);
			channel(*this, poleHelper, intPole);
			rel(*this, variables_[index] == intPole);
			unimportantNonDominantPoleIndex++;
		}

		std::vector<int> unimportantPositiveZerosIndexes  = getPolesAndZeros().getUnimportantZerosIndexVector();
		std::vector<int>::iterator  unimportantPositiveZerosIndex = unimportantPositiveZerosIndexes.begin();
		for(auto & zeros : getPolesAndZeros().getUnimportantZeros())
		{
			int index = * unimportantPositiveZerosIndex;

			dom(*this, zeros, 0, pow(10,10));
			Gecode::FloatVar zeroHelper(*this, 0, pow(10,12));
			rel(*this, zeroHelper <= zeros * pow(10,-3)+0.5);
			rel(*this, zeroHelper >= zeros * pow(10,-3)-0.5);

			Gecode::IntVar intZero(*this, 0, 10000000000);
			channel(*this, zeroHelper, intZero);
			rel(*this, variables_[index] == intZero);
			unimportantPositiveZerosIndex++;
		}
	}

	void SearchSpace::print() const
	{

	}

	void SearchSpace::eraseVariables() const
	{
		if(variablesMap_->isInitialized())
			delete variablesMap_;
	}

	void SearchSpace::initializeBranching()
	{
		Gecode::Rnd r(1U);
		Gecode::Rnd u(1U);

		logDebug("Starting Branching");

		branchBiasInputTransistor();
//		Gecode::branchTransistorsIntervalSplitting(*this, variables_, getVariables().getTransistors(), getResult());
//		Gecode::branchIntegerVariables(*this, variables_, getVariables());
		Gecode::branchIntegerVariablesWithSupportOfTransistorDomain(*this, variables_, getVariables());
//		Gecode::branchTransistorsOnlyVariableDomain(*this, variables_, transistors);
//		Gecode::branch(*this, variables_, Gecode::INT_VAR_SIZE_MIN(), Gecode::INT_VAL_MED());


		Gecode::branch(*this, cost_, Gecode::FLOAT_VAL_SPLIT_MAX());
	}


	void SearchSpace::branchBiasInputTransistor()
	{
		if(getCircuitInformation().getCircuitParameter().hasCurrentBiasPin())
		{

			const Core::NetId & biasNet = getCircuitInformation().getCircuitParameter().getCurrentBiasPin().getNetId();
			const StructRec::StructureCircuit & arrayCircuit = structRecResult_->findStructureCircuit(0);

			std::vector<const StructRec::Structure*> connectedStructures = arrayCircuit.findConnectedStructures(biasNet);
			std::vector<Partitioning::Component*> connectedComponents;
			for(auto& i : connectedStructures)
			{
				const StructRec::Structure & array = *i;
				if(array.getStructureName() == StructRec::StructureName("MosfetDiodeArray"))
				{

					Partitioning::Component & comp = getPartitioningResult().findComponent(array.getIdentifier());
					branchComponent(comp);
				}
			}
		}

	}

	void SearchSpace::branchComponent(Partitioning::Component & component)
	{
		Gecode::Rnd r(1U);
		Gecode::Rnd u(1U);
		Gecode::IntVar length = transistorToLengthMap_.find(component);
		Gecode::IntVar width = transistorToWidthMap_.find(component);
		Gecode::IntVar current = transistorToCurrentMap_.find(component);
		branch(*this, length, Gecode::INT_VAL_RND(r));
		branch(*this, width, Gecode::INT_VAL_RND(u));
		branch(*this, current, Gecode::INT_VAL_SPLIT_MIN());


		if(component.getArray().getIdentifier().getName() == StructRec::StructureName("MosfetNormalArray"))
		{
			Gecode::IntVar sourceVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetNormalArray", "Source")).getNet().getIdentifier());
			//Drain Voltage in the moment not enough specified for branching
			Gecode::IntVar drainVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getNet().getIdentifier());
			Gecode::IntVar gateVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getNet().getIdentifier());


//			branch(*this, drainVoltage, Gecode::INT_VAL_SPLIT_MIN());
			branch(*this, gateVoltage, Gecode::INT_VAL_SPLIT_MIN());
			branch(*this, sourceVoltage, Gecode::INT_VAL_SPLIT_MIN());

		}
		else if(component.getArray().getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray"))
		{
			Gecode::IntVar sourceVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetDiodeArray", "Source")).getNet().getIdentifier());
			Gecode::IntVar drainVoltage = netToVoltageMap_.find(component.getArray().findPin(StructRec::StructurePinType("MosfetDiodeArray", "Drain")).getNet().getIdentifier());

			branch(*this, sourceVoltage, Gecode::INT_VAL_SPLIT_MIN());
			branch(*this, drainVoltage, Gecode::INT_VAL_SPLIT_MIN());
		}
	}


	void SearchSpace::initializeCost()
	{
		int gainIndex = getVariables().findOther(GAIN_).getIndex();
		dom(*this, variables_[gainIndex],0, 1000);
		int transitFrequencyWithErrorFactorIndex = getVariables().findOther(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_).getIndex();
		dom(*this, variables_[transitFrequencyWithErrorFactorIndex],0, 100000000000);

		Gecode::FloatVar floatGain(*this, 0, 280);
		channel(*this, floatGain, variables_[gainIndex]);
		Gecode::FloatVar normedGain (*this, 0,1);
		Gecode::rel(*this, ( floatGain- floatGain.min()) == normedGain *  floatGain.size());

		Gecode::FloatVar normedArea (*this,-1,1);
		int areaIndex = getVariables().findOther(AREA_).getIndex();
		Gecode::FloatVar floatArea(*this, 0, getCircuitInformation().getCircuitSpecification().getMaxArea());
		channel(*this, floatArea, variables_[areaIndex]);
		Gecode::rel(*this, (floatArea.max() - floatArea) == normedArea * floatArea.size());

		Gecode::FloatVar normedPower (*this,0, 1);
		int powerIndex = getVariables().findOther(POWER_).getIndex();
		Gecode::FloatVar floatPower(*this, 0, getCircuitInformation().getCircuitSpecification().getPowerConsumption() * pow(10,6));
		channel(*this, floatPower, variables_[powerIndex]);
		Gecode::rel(*this, (floatPower.max() - floatPower) == normedPower *  floatPower.size());

		Gecode::FloatVar normedTransitFrequency (*this, 0, 1);
		Gecode::FloatVar floatTransitFrequency(*this, 0, pow(10, 10));
		channel(*this, floatTransitFrequency, variables_[transitFrequencyWithErrorFactorIndex]);
		Gecode::rel(*this, ( floatTransitFrequency - floatTransitFrequency.min()) == normedTransitFrequency *  floatTransitFrequency.size());

		Gecode::FloatVar normedSlewRate (*this, 0,1);
		int slewRateIndex = getVariables().findOther(SLEW_RATE_).getIndex();
		Gecode::FloatVar floatSlewRate(*this, 0, pow(10,10));
		channel(*this, floatSlewRate, variables_[slewRateIndex]);
		Gecode::rel(*this, (floatSlewRate - floatSlewRate.min()) == normedSlewRate *  floatSlewRate.size());

		cost_ = Gecode::expr(*this,
				normedGain
				+ normedPower
				+ normedArea
				+	normedTransitFrequency
				+ normedSlewRate
			);
	}

	void SearchSpace::createVariables()
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		std::vector<Partitioning::TwoPort*> twoPort =  getPartitioningResult().getAllTwoPorts();
		const std::vector<Node*> nodes = getGraph().getAllNodes();
		int numOfWidths = getSizingRulesConstraints().getEqualWidthMap().computeNumberOfNeededParameter(transistors);
		int numOfLengths = getSizingRulesConstraints().getEqualLengthMap().computeNumberOfNeededParameter(transistors);

		int numOfTransistorVariables = numOfWidths + numOfLengths + nodes.size() + transistors.size();
		int numOfTwoPortVariables =  twoPort.size();

		int numOfPolesAndZeros = 2 + getPolesAndZeros().getNumberImportantNonDominantPoles() + getPolesAndZeros().getNumberImportantZeros()
				+getPolesAndZeros().getNumberUnimportantNonDominantPoles() + getPolesAndZeros().getNumberUnimportantZeros();

		int numOfSpecifications = 23;

		int numOfVariables = numOfTransistorVariables + numOfTwoPortVariables + numOfPolesAndZeros + numOfSpecifications;
		variables_ = Gecode::IntVarArray(*this, numOfVariables, -pow(10,9), pow(10,9));
		cost_ = Gecode::FloatVar(*this, -10000000, 100000000000000000);

		helper_ =Gecode::FloatVar(*this, - pow(10,16), pow(10,16));
	}

	void SearchSpace::createMaps()
	{
//		logDebug("createTransistorVariableMaps");
		createTransistorVariableMaps();
//		logDebug("CreateTowPortMap");
		createTwoPortValueMap();


	}

	void SearchSpace::createIndexes()
	{
		int index = 0;
		createTransistorVariableIndexMaps(index);

		createTwoPortIndexMap(index);
		createPolesAndZerosIndexVectors(index);
		createSpecificationIndexMap(index);
	}

	void  SearchSpace::createTransistorVariableMaps()
	{
//		logDebug("create Length Map");
		createLengthMap();
//		logDebug("create Width Map");
		createWidthMap();
//		logDebug("create Current Map");
		createCurrentMap();
//		logDebug("create Voltage Map");
		createVoltageMap();
	}

	void  SearchSpace::createTransistorVariableIndexMaps(int & index)
	{


//		logDebug("create Length Map");
		createLengthIndexMap(index);
//		logDebug("create Width Map");
		createWidthIndexMap(index);
//		logDebug("create Current Map");
		createCurrentIndexMap(index);
//		logDebug("create Voltage Map");
		createVoltageIndexMap(index);
	}

	void SearchSpace::initializeVariableMap()
	{
		Variables * variablesMap = new Variables();
		variablesMap_ = variablesMap;
		variablesMap_->initializeRndSeedMap(variables_.size());
		initializeTransistorMap();
	}


	void SearchSpace::initializeTransistorMap()
	{
		for(auto & transistor : getPartitioningResult().getAllTransistors())
		{
			Transistor * tran = new Transistor;
			tran->setName(transistor->getArray().getIdentifier().toStr());
			if(getPartitioningResult().isInOpAmp(*transistor)){
				tran->setType("opAmp");
			}
			else {
				tran->setType("bias");
			}
			getVariables().add(*tran);
		}
	}

	void SearchSpace::createWidthMap()
	{
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::map<const StructRec::StructureId, std::vector<Partitioning::Component*>> equalWidthMap = getSizingRulesConstraints().getEqualWidthMap().getMap();
		for(std::map<const StructRec::StructureId, std::vector<Partitioning::Component*> >::const_iterator it = equalWidthMap.begin(); it != equalWidthMap.end(); it++)
		{
			const StructRec::StructureId main = it->first;
			int index = getVariables().findTransistor(main).getWidthIndex();
			Gecode::IntVar intVar = variables_[index];
			transistorToWidthMap_.add(getPartitioningResult().findComponent(main), intVar);


			std::vector<Partitioning::Component*> belongingComps = it->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = belongingComps.begin(); it_comps != belongingComps.end(); it_comps++)
			{
				Partitioning::Component* comp = *it_comps;
				transistorToWidthMap_.add(*comp,intVar);
			}
		}
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			if(!getSizingRulesConstraints().getEqualWidthMap().hasComponentAsMain(*tran)
					&& !getSizingRulesConstraints().getEqualWidthMap().hasComponentAsSecondary(*tran))
			{
				int index = getVariables().findTransistor(tran->getArray().getIdentifier()).getWidthIndex();
				Gecode::IntVar intVar = variables_[index];
				transistorToWidthMap_.add(*tran, intVar);

			}
		}

		transistorToWidthMap_.setDefinition(getDefinition());
	}

	void SearchSpace::createWidthIndexMap(int & index)
	{
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::map<const StructRec::StructureId, std::vector<Partitioning::Component*>> equalWidthMap = getSizingRulesConstraints().getEqualWidthMap().getMap();
		for(std::map<const StructRec::StructureId, std::vector<Partitioning::Component*> >::const_iterator it = equalWidthMap.begin(); it != equalWidthMap.end(); it++)
		{
			const StructRec::StructureId main = it->first;
			getVariables().findTransistor(main).setWidthIndex(index);

			std::ostringstream mainString;
			mainString << "Width: " << main.toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			std::vector<Partitioning::Component*> belongingComps = it->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = belongingComps.begin(); it_comps != belongingComps.end(); it_comps++)
			{
				Partitioning::Component* comp = *it_comps;
				getVariables().findTransistor(comp->getArray().getIdentifier()).setWidthIndex(index);

				std::ostringstream oss;
				std::string oldValue = indexToVariableMap_.at(index);
				oss << oldValue << ", " << comp->getArray().getIdentifier().toStr();
				indexToVariableMap_.at(index) = oss.str();

			}
			index++;
		}
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			if(!getSizingRulesConstraints().getEqualWidthMap().hasComponentAsMain(*tran)
					&& !getSizingRulesConstraints().getEqualWidthMap().hasComponentAsSecondary(*tran))
			{
				getVariables().findTransistor(tran->getArray().getIdentifier()).setWidthIndex(index);

				std::ostringstream mainString;
				mainString << "Width: " << tran->getArray().getIdentifier().toStr();

				indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));



				index++;
			}
		}

		transistorToWidthMap_.setDefinition(getDefinition());
	}

	void SearchSpace::createLengthMap()
	{
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::map<const StructRec::StructureId, std::vector<Partitioning::Component*>> equalLengthMap = getSizingRulesConstraints().getEqualLengthMap().getMap();
		for(std::map<const StructRec::StructureId, std::vector<Partitioning::Component*> >::const_iterator it = equalLengthMap.begin(); it != equalLengthMap.end(); it++)
		{
			const StructRec::StructureId main = it->first;
			int index = getVariables().findTransistor(main).getLengthIndex();
			Gecode::IntVar intVar = variables_[index];
			transistorToLengthMap_.add(getPartitioningResult().findComponent(main), intVar);


			std::vector<Partitioning::Component*> belongingComps = it->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = belongingComps.begin(); it_comps != belongingComps.end(); it_comps++)
			{
				Partitioning::Component* comp = *it_comps;
				transistorToLengthMap_.add(*comp,intVar);

			}
			index++;
		}
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;

			if(!getSizingRulesConstraints().getEqualLengthMap().hasComponentAsMain(*tran)
				&& !getSizingRulesConstraints().getEqualLengthMap().hasComponentAsSecondary(*tran))
			{
				int index = getVariables().findTransistor(tran->getArray().getIdentifier()).getLengthIndex();
				Gecode::IntVar intVar = variables_[index];
				transistorToLengthMap_.add(*tran, intVar);

				index++;
			}
		}
		transistorToLengthMap_.setDefinition(getDefinition());
	}

	void SearchSpace::createLengthIndexMap(int & index)
	{
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::map<const StructRec::StructureId, std::vector<Partitioning::Component*>> equalLengthMap = getSizingRulesConstraints().getEqualLengthMap().getMap();
		for(std::map<const StructRec::StructureId, std::vector<Partitioning::Component*> >::const_iterator it = equalLengthMap.begin(); it != equalLengthMap.end(); it++)
		{
			const StructRec::StructureId main = it->first;
			getVariables().findTransistor(main).setLengthIndex(index);

			std::ostringstream mainString;
			mainString << "Length: " << main.toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			std::vector<Partitioning::Component*> belongingComps = it->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = belongingComps.begin(); it_comps != belongingComps.end(); it_comps++)
			{
				Partitioning::Component* comp = *it_comps;
				getVariables().findTransistor(comp->getArray().getIdentifier()).setLengthIndex(index);

				std::ostringstream oss;
				std::string oldValue = indexToVariableMap_.at(index);
				oss << oldValue << ", " << comp->getArray().getIdentifier().toStr();
				indexToVariableMap_.at(index) = oss.str();
			}
			index++;
		}
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;

			if(!getSizingRulesConstraints().getEqualLengthMap().hasComponentAsMain(*tran)
				&& !getSizingRulesConstraints().getEqualLengthMap().hasComponentAsSecondary(*tran))
			{
				getVariables().findTransistor(tran->getArray().getIdentifier()).setLengthIndex(index);

				std::ostringstream mainString;
				mainString << "Length: " << tran->getArray().getIdentifier().toStr();

				indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

				index++;
			}
		}
		transistorToLengthMap_.setDefinition(getDefinition());
	}

	void SearchSpace::createTwoPortValueMap()
	{
		assert(getVariables().isInitialized(), "Variables need to be initialized");
		for(auto & it : getPartitioningResult().getAllTwoPorts())
		{
			StructRec::StructureId  twoPortId = it->getArray().getIdentifier();
			int index = getVariables().findOther(twoPortId).getIndex();
			Gecode::IntVar intVar = variables_[index];
			twoPortToValueMap_.add(twoPortId, intVar);
		}
	}

	void SearchSpace::createTwoPortIndexMap(int & index)
	{
		std::vector<Partitioning::TwoPort*> twoPorts = getPartitioningResult().getAllTwoPorts();
		for(std::vector<Partitioning::TwoPort*>::const_iterator it = twoPorts.begin(); it != twoPorts.end(); it++)
		{
			Partitioning::TwoPort * tran = *it;

			TwoPort * twoPort = new TwoPort;
			twoPort->setName(tran->getArray().getIdentifier().toStr());
			twoPort->setIndex(index);
			getVariables().add(*twoPort);


			std::ostringstream mainString;
			mainString << tran->getArray().getIdentifier().toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			index++;
		}
	}

	void SearchSpace::createVoltageMap()
	{
		assert(getVariables().isInitialized(), "NetToVoltageIndexMap needs to be initialized");
		for(auto & node : getGraph().getAllNodes())
		{
			Core::NetId netId =node->getNetId();
			int index = getVariables().findOther(netId.toStr()).getIndex();


			Gecode::IntVar intVar = variables_[index];
			netToVoltageMap_.add(netId, intVar);
			index++;
		}
	}

	void SearchSpace::createVoltageIndexMap(int & index)
	{
		std::vector<Node*> nodes = getGraph().getAllNodes();
		for(std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
		{
			Node * node = *it;

			Voltage * net = new Voltage;
			net->setName(node->getNetId().toStr());
			net->setIndex(index);
			getVariables().add(*net);
			setNetIndexOfTransistors(node->getNetId(), index);

			std::ostringstream mainString;
			mainString << "Voltage: " << node->getNetId().toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			index++;
		}
	}


	void SearchSpace::setNetIndexOfTransistors(Core::NetId netId, int &index)
	{
		const StructRec::StructureNet & net = getStructureRecognitionResult().findStructureCircuit(0).findStructureNet(netId);

		std::vector<const StructRec::Structure *> gateDTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
		for(auto & gateDTA : gateDTAs)
		{
			getVariables().findTransistor(gateDTA->getIdentifier()).setDrainVoltageIndex(index);
			getVariables().findTransistor(gateDTA->getIdentifier()).setGateVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> sourceDTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Source"));
		for(auto & sourceDTA : sourceDTAs)
		{
			getVariables().findTransistor(sourceDTA->getIdentifier()).setSourceVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> gateNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
		for(auto & gateNTA : gateNTAs)
		{
			getVariables().findTransistor(gateNTA->getIdentifier()).setGateVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> drainNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		for(auto & drainNTA : drainNTAs)
		{
			getVariables().findTransistor(drainNTA->getIdentifier()).setDrainVoltageIndex(index);
		}

		std::vector<const StructRec::Structure *> sourceNTAs = net.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Source"));
		for(auto & sourceNTA : sourceNTAs)
		{
			getVariables().findTransistor(sourceNTA->getIdentifier()).setSourceVoltageIndex(index);
		}


	}

	void SearchSpace::createCurrentMap()
	{
		assert(getVariables().isInitialized(), "Transistors need to be initialized");
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			int index = getVariables().findTransistor(tran->getArray().getIdentifier()).getCurrentIndex();
			Gecode::IntVar intVar = variables_[index];
			transistorToCurrentMap_.add(*tran, intVar);
		}
	}

	void SearchSpace::createCurrentIndexMap(int & index)
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor * tran = *it;
			getVariables().findTransistor(tran->getArray().getIdentifier()).setCurrentIndex(index);

			std::ostringstream mainString;
			mainString << "Current: " << tran->getArray().getIdentifier().toStr();

			indexToVariableMap_.insert(std::pair<int, std::string>(index, mainString.str()));

			index++;
		}
	}

	void SearchSpace::createPolesAndZerosIndexVectors(int & index)
	{

		getPolesAndZeros().setPolesAndZerosIndexes(index, indexToVariableMap_, getVariables());
	}

	void SearchSpace::createSpecificationIndexMap(int & index)
	{
		Specification * gain = new Specification;
		gain->setName(GAIN_);
		gain->setIndex(index);
		getVariables().add(*gain);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, GAIN_));
		index++;

		Specification * feedbackGain = new Specification;
		feedbackGain->setName(FEEDBACK_GAIN_);
		feedbackGain->setIndex(index);
		getVariables().add(*feedbackGain);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_GAIN_));
		index++;

		Specification * power = new Specification;
		power->setName(POWER_);
		power->setIndex(index);
		getVariables().add(*power);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, POWER_));
		index++;

		Specification * powerOpAmp = new Specification;
		powerOpAmp->setName(POWER_OP_AMP_);
		powerOpAmp->setIndex(index);
		getVariables().add(*powerOpAmp);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, POWER_OP_AMP_));
		index++;

		Specification * powerFeedback = new Specification;
		powerFeedback->setName(POWER_FEEDBACK_);
		powerFeedback->setIndex(index);
		getVariables().add(*powerFeedback);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, POWER_FEEDBACK_));
		index++;

		Specification * ft = new Specification;
		ft->setName(TRANSIT_FREQUENCY_);
		ft->setIndex(index);
		getVariables().add(*ft);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, TRANSIT_FREQUENCY_));
		index++;

		Specification * feedbackFt = new Specification;
		feedbackFt->setName(FEEDBACK_TRANSIT_FREQUENCY_);
		feedbackFt->setIndex(index);
		getVariables().add(*feedbackFt);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_TRANSIT_FREQUENCY_));
		index++;

		Specification * ftErrorFactor = new Specification;
		ftErrorFactor->setName(TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_);
		ftErrorFactor->setIndex(index);
		getVariables().add(*ftErrorFactor);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, TRANSIT_FREQUENCY_WITH_ERROR_FACTOR_));
		index++;

		Specification * slewRate = new Specification;
		slewRate->setName(SLEW_RATE_);
		slewRate->setIndex(index);
		getVariables().add(*slewRate);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, SLEW_RATE_));
		index++;

		Specification * settlingTime = new Specification;
		settlingTime->setName(SETTLING_TIME_);
		settlingTime->setIndex(index);
		getVariables().add(*settlingTime);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, SETTLING_TIME_));
		index++;

		Specification * CMRR = new Specification;
		CMRR->setName(CMRR_);
		CMRR->setIndex(index);
		getVariables().add(*CMRR);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, CMRR_));
		index++;

		Specification * feedbackCMRR = new Specification;
		feedbackCMRR->setName(FEEDBACK_CMRR_);
		feedbackCMRR->setIndex(index);
		getVariables().add(*feedbackCMRR);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_CMRR_));
		index++;

		Specification * negPSRR = new Specification;
		negPSRR->setName(NEG_PSRR_);
		negPSRR->setIndex(index);
		getVariables().add(*negPSRR);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, NEG_PSRR_));
		index++;

		Specification * posPSRR = new Specification;
		posPSRR->setName(POS_PSRR_);
		posPSRR->setIndex(index);
		getVariables().add(*posPSRR);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, POS_PSRR_));
		index++;

		Specification * voutMax = new Specification;
		voutMax->setName(VOUT_MAX_);
		voutMax->setIndex(index);
		getVariables().add(*voutMax);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, VOUT_MAX_));
		index++;

		Specification * voutMin = new Specification;
		voutMin->setName(VOUT_MIN_);
		voutMin->setIndex(index);
		getVariables().add(*voutMin);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, VOUT_MIN_));
		index++;

		Specification * vcmMin = new Specification;
		vcmMin->setName(VCM_MIN_);
		vcmMin->setIndex(index);
		getVariables().add(*vcmMin);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, VCM_MIN_));
		index++;

		Specification * vcmMax = new Specification;
		vcmMax->setName(VCM_MAX_);
		vcmMax->setIndex(index);
		getVariables().add(*vcmMax);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, VCM_MAX_));
		index++;

		Specification * feedbackVcmMin = new Specification;
		feedbackVcmMin->setName(FEEDBACK_VCM_MIN_);
		feedbackVcmMin->setIndex(index);
		getVariables().add(*feedbackVcmMin);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_VCM_MIN_));
		index++;

		Specification * feedbackVcmMax = new Specification;
		feedbackVcmMax->setName(FEEDBACK_VCM_MAX_);
		feedbackVcmMax->setIndex(index);
		getVariables().add(*feedbackVcmMax);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_VCM_MAX_));
		index++;

		Specification * area = new Specification;
		area->setName(AREA_);
		area->setIndex(index);
		getVariables().add(*area);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, AREA_));
		index++;

		Specification * phaseMargin = new Specification;
		phaseMargin->setName(PHASE_MARGIN_);
		phaseMargin->setIndex(index);
		getVariables().add(*phaseMargin);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, PHASE_MARGIN_));
		index++;

		Specification * feedbackPhaseMargin = new Specification;
		feedbackPhaseMargin->setName(FEEDBACK_PHASE_MARGIN_);
		feedbackPhaseMargin->setIndex(index);
		getVariables().add(*feedbackPhaseMargin);
		indexToVariableMap_.insert(std::pair<int, std::string>(index, FEEDBACK_PHASE_MARGIN_));
		index++;
	}

	void SearchSpace::createConstraints()
	{
		//Only for Debugging
//		setWidthAndLength();

		logDebug("Creating Constraints");
		logDebug("Create sizing constraints");
		createSizingConstraints();
		logDebug("Create Domain Constraints");
		createDomainConstraints();
		logDebug("Create Voltage Constraints");
		createVoltageConstraints();
		logDebug("Create Load Capacity Constraints");
		createLoadCapacityConstraint();
		logDebug("Creating KCL constraints");
		createKCLConstraints();
		logDebug("Creating circuit specification constraints");
		createCircuitSpecificationConstraints();
		logDebug("Creating sizing rules constraints");
		getSizingRulesConstraints().createConstraints();
		logDebug("Creating transistor constraints");
		createTransistorConstraints();
	}

	void SearchSpace::createDomainConstraints()
	{
		for(auto & tran : getPartitioningResult().getAllTransistors())
		{
			dom(*this, transistorToWidthMap_.find(*tran), 1, widthUpperBound_);
			dom(*this, transistorToLengthMap_.find(*tran), 1, lengthUpperBound_);
			dom(*this, transistorToCurrentMap_.find(*tran), -1000000000, 1000000000);
		}

		int vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage() * 1000;
		int vss = getCircuitInformation().getCircuitParameter().getGroundVoltage() *1000;

		for(auto & net : graph_->getAllNodes())
		{
			dom(*this, netToVoltageMap_.find(net->getNetId()), vss, vdd);
		}


	}

	void SearchSpace::createSizingConstraints()
	{
		std::vector<Partitioning::Transistor*> transistors = getPartitioningResult().getAllTransistors();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = transistors.begin(); it != transistors.end(); it++)
		{
			Partitioning::Transistor*  tran = * it;
			if(!(getPartitioningResult().getPart(tran->getArray()).isPositiveFeedbackPart() &&getPartitioningResult().getPositiveFeedbackPart(tran->getArray()).isGainEnhancer()))
			{
				Gecode::IntVar length = transistorToLengthMap_.find(*tran);
				Gecode::IntVar width = transistorToWidthMap_.find(*tran);
				rel(*this, width >= length);
			}
		}
	}

	void SearchSpace::createLoadCapacityConstraint()
	{
		if(getPartitioningResult().hasLoadCapacitance())
		{
			std::vector<Partitioning::CapacitancePart *>  loadCaps = getPartitioningResult().getLoadCapacities();
			for(auto & loadCap : loadCaps)
			{
				Partitioning::Component * loadCapComp = *getPartitioningResult().getBelongingComponents(*loadCap).begin();
				Core::DeviceId loadCapId = (**loadCapComp->getArray().getDevices().begin()).getIdentifier();
				Gecode::IntVar loadCapValue = twoPortToValueMap_.find(*loadCapComp);
				Gecode::rel(*this, loadCapValue ==  10*getCircuitInformation().getCircuitParameter().getLoadCapacity(loadCapId));
			}
		}
	}

	void SearchSpace::createVoltageConstraints()
	{

		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getInputPinMinus());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getInputPinPlus());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getGroundPin());
		createVoltageConstraints(getCircuitInformation().getCircuitParameter().getSupplyVoltagePin());

		if(getCircuitInformation().getCircuitParameter().hasReferenceVoltagePin())
		{
			createVoltageConstraints(getCircuitInformation().getCircuitParameter().getReferenceVoltagePin());
		}

		for(auto & biasVoltagePin : getCircuitInformation().getCircuitParameter().getVoltageBiasPins())
		{
			createVoltageConstraints(*biasVoltagePin);
		}

		createOutputVoltageConstraint();

		if(usesHSpiceLibrary())
		{
			logDebug("Create stage bias voltage pin constraint");
			createStageBiasVoltagePinsConstraints();
		}

	}

	void SearchSpace::createVoltageConstraints(const Pin & pin)
	{
		Gecode::rel(*this, pin.getValue() * 1000 == netToVoltageMap_.find(pin.getNetId()));

	}

	void SearchSpace::createStageBiasVoltagePinsConstraints()
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
				Core::PinType gatePin = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName( "Gate"));
				const StructRec::StructureCircuit & arrayCircuit = getStructureRecognitionResult().findStructureCircuit(0);

				Partitioning::Component & biasCompFirstStage = **supplyFirstStageBiasComps.begin();
				const Core::Device & deviceFirstStageBias = **biasCompFirstStage.getArray().getDevices().begin();
				Core::NetId gateNetBiasCompFirstStage = deviceFirstStageBias.findNet(gatePin).getIdentifier();

				for(auto & biasCompSecondStage : supplySecondStageBiasComps)
				{
					const Core::Device & deviceSecondStageBias = **biasCompSecondStage->getArray().getDevices().begin();
					Core::NetId gateNetBiasCompSecondStage = deviceSecondStageBias.findNet(gatePin).getIdentifier();
					if(arrayCircuit.findStructureNet(gateNetBiasCompSecondStage).getAllConnectedStructures().size() ==1)
					{
						Gecode::rel(*this, netToVoltageMap_.find(gateNetBiasCompSecondStage) == 1 * netToVoltageMap_.find(gateNetBiasCompFirstStage));
					}
				}
			}

		}
	}

	void SearchSpace::createOutputVoltageConstraint()
	{
		if(getCircuitInformation().getCircuitParameter().isFullyDifferential())
		{
			const Pin & outputPinMinus = getCircuitInformation().getCircuitParameter().getOutputPinMinus();
			const Pin & inputPinPlus = getCircuitInformation().getCircuitParameter().getInputPinPlus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPinMinus.getNetId()) == 1 * netToVoltageMap_.find(inputPinPlus.getNetId()));

			const Pin & outputPinPlus = getCircuitInformation().getCircuitParameter().getOutputPinPlus();
			const Pin & inputPinMinus = getCircuitInformation().getCircuitParameter().getInputPinMinus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPinPlus.getNetId()) == 1 * netToVoltageMap_.find(inputPinMinus.getNetId()));

		}
		else
		{
			const Pin & outputPin = getCircuitInformation().getCircuitParameter().getOutputPin();
			const Pin & inputPinMinus = getCircuitInformation().getCircuitParameter().getInputPinMinus();
			Gecode::rel(*this, netToVoltageMap_.find(outputPin.getNetId()) == 1 * netToVoltageMap_.find(inputPinMinus.getNetId()));
		}
	}

	void SearchSpace::createKCLConstraints()
	{
		KCLConstraints kclConstraints;
		kclConstraints.setCircuitParameter(getCircuitInformation().getCircuitParameter());
		kclConstraints.setGraph(getGraph());
		kclConstraints.setSpace(*this);
		kclConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
		kclConstraints.setUsesHspiceLibrary(usesHSpiceLibrary());
		kclConstraints.createConstraints();
	}




	void SearchSpace::createTransistorConstraints()
	{
		if(transistorModel_ == "SHM")
		{
			TransistorConstraintsSHM transistorConstraints;
			transistorConstraints.setCircuitInformation(getCircuitInformation());
			transistorConstraints.setGraph(getGraph());
			transistorConstraints.setNetToVoltageMap(netToVoltageMap_);
			transistorConstraints.setPartitioningResult(getPartitioningResult());
			transistorConstraints.setSpace(*this);
			transistorConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
			transistorConstraints.setTransistorToLengthMap(transistorToLengthMap_);
			transistorConstraints.setTransistorToWidthMap(transistorToWidthMap_);
			transistorConstraints.createConstraints();
		}
		else
		{
			TransistorConstraintsEKV transistorConstraints;
			transistorConstraints.setCircuitInformation(getCircuitInformation());
			transistorConstraints.setGraph(getGraph());
			transistorConstraints.setNetToVoltageMap(netToVoltageMap_);
			transistorConstraints.setPartitioningResult(getPartitioningResult());
			transistorConstraints.setSpace(*this);
			transistorConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
			transistorConstraints.setTransistorToLengthMap(transistorToLengthMap_);
			transistorConstraints.setTransistorToWidthMap(transistorToWidthMap_);
			transistorConstraints.createConstraints();
		}
	}


	void SearchSpace::createSizingRulesConstraints()
	{
		SizingRulesConstraints sizingRulesConstraints;
		sizingRulesConstraints.initialize();
		sizingRulesConstraints.setPartitioningResult(getPartitioningResult());
		sizingRulesConstraints.setCircuitInformation(getCircuitInformation());
		sizingRulesConstraints.setSpace(*this);
		sizingRulesConstraints.setStructureRecognitionResult(getStructureRecognitionResult());
		sizingRulesConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
		sizingRulesConstraints.setTransistorToLengthMap(transistorToLengthMap_);
		sizingRulesConstraints.setTransistorToWidthMap(transistorToWidthMap_);
		sizingRulesConstraints.setTwoPortToValueMap(twoPortToValueMap_);
		sizingRulesConstraints.createMaps();
		sizingRulesConstraints_ = sizingRulesConstraints;
	}

	bool SearchSpace::hasCircuitInformation() const
	{
		return circuitInformation_ != NULL;
	}

	bool SearchSpace::hasGraph() const
	{
		return graph_ != NULL;
	}

	bool SearchSpace::hasResult() const
	{
		return result_ != NULL;
	}

	const CircuitInformation& SearchSpace::getCircuitInformation() const
	{
		assert(hasCircuitInformation());
		return * circuitInformation_;
	}

	const Graph& SearchSpace::getGraph() const
	{
		assert(hasGraph());
		return * graph_;
	}

	void SearchSpace::setStructureRecognitionResult(
			const StructRec::StructureCircuits& structRecResult)
	{
		structRecResult_ = & structRecResult;
	}

	const StructRec::StructureCircuits& SearchSpace::getStructureRecognitionResult() const
	{
		assert(structRecResult_ != NULL);
		return * structRecResult_;
	}

	SizingRulesConstraints& SearchSpace::getSizingRulesConstraints()
	{
		assert(sizingRulesConstraints_.isInitialized());
		return sizingRulesConstraints_;
	}

	const Result & SearchSpace::computeResult()
	{
		Result * result = new Result;
		result->initializeTransistorMap(getPartitioningResult());
		result->initializeTwoPortMap(getPartitioningResult());

		transistorToWidthMap_.writeWidthToTransistor(*result);
		transistorToLengthMap_.writeLengthToTransistor(*result);
		twoPortToValueMap_.writeValueToCapacitance(*result);

		netToVoltageMap_.write(*result);
		transistorToCurrentMap_.writeCurrent(*result);
		result->setGain(getGain());
		result->setPower(getPowerConsumption());
		result->setTransitFrequency(getTransitFrequency());
		result->setTransitFrequencyWithErrorFactor(getTransitFrequencyWithErrorFactor());
		result->setArea(getArea());
		result->setPhaseMargin(getPhaseMargin());
		result->setSlewRate(getSlewRate());
		result->setCMRR(getCMRR());
		result->setNegPSRR(getNegPSRR());
		result->setPosPSRR(getPosPSRR());
		result->setMaxOutputVoltage(getMaxOutputVoltage());
		result->setMinOutputVoltage(getMinOutputVoltage());
		result->setMinCommonModeInputVoltage(getMinCommonModeInputVoltage());
		result->setMaxCommonModeInputVoltage(getMaxCommonModeInputVoltage());

		return * result;
	}

	void SearchSpace::setWidthAndLength()
	{
		transistorToLengthMap_.setLength(*this);

		transistorToWidthMap_.setWidth(*this);

		//For further help during debugging
//		twoPortToValueMap_.setCapacitance(*this);
//		transistorToCurrentMap_.setCurrents(*this);
//		netToVoltageMap_.setVoltages(*this);
	}



	void SearchSpace::setTransistorModel(std::string transistorModel)
	{
		transistorModel_ = transistorModel;
	}

	void SearchSpace::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	void SearchSpace::setUsesHSpiceLibrary(bool usesIt)
	{
		usesHSpiceLibrary_ = usesIt;
	}

	void SearchSpace::setPolesAndZeros(PolesAndZeros & polesAndZeros)
	{
		polesAndZeros_ = &polesAndZeros;
	}

	PolesAndZeros & SearchSpace::getPolesAndZeros()
	{
		assert(polesAndZeros_ != NULL);
		return *polesAndZeros_;
	}

	const PolesAndZeros & SearchSpace::getPolesAndZeros() const
	{
		assert(polesAndZeros_ != NULL);
		return *polesAndZeros_;
	}

	std::string SearchSpace::getTransistorModel() const
	{
		assert(transistorModel_ == "SHM" || transistorModel_ == "EKV", "Transistor model not supported");
		return transistorModel_;
	}

	Variables & SearchSpace::getVariables()
	{
		assert(variablesMap_ != nullptr);
		return * variablesMap_;
	}
	const Variables & SearchSpace::getVariables() const
	{
		assert(variablesMap_ != nullptr);
		return * variablesMap_;
	}

	int SearchSpace::getLengthUpperBound() const
	{
		assert(lengthUpperBound_ != NOT_INITIALIZED_);
		return lengthUpperBound_;
	}

	int SearchSpace::getWidthUpperBound() const
	{
		assert(widthUpperBound_ != NOT_INITIALIZED_);
		return widthUpperBound_;
	}

	float SearchSpace::getScalingFactorMUM() const
	{
		assert(scalingFactorMUM_ != NOT_INITIALIZED_);
		return scalingFactorMUM_;
	}

	void SearchSpace::initializeScalingValues()
	{
		if(definition_ == "1mum")
		{
			widthUpperBound_ = 600;
			lengthUpperBound_ = 10;
			scalingFactorMUM_ = pow(10,-6);
		}
		if(definition_ == "0.1mum")
		{
			widthUpperBound_ = 6000;
			lengthUpperBound_ = 100;
			scalingFactorMUM_ = pow(10,-7);
		}
	}

	std::string SearchSpace::getDefinition() const
	{
		assert(definition_ == "1mum" || definition_ == "0.1mum",  "Definition not supported");
		return definition_;
	}

	bool SearchSpace::isOutputNetStage(const StructRec::StructureNet & net, Partitioning::TransconductancePart & stage) const
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

	std::vector<const StructRec::StructureNet *> SearchSpace::findOutputNets(Partitioning::TransconductancePart & stage) const
	{
		assert(stage.isFirstStage() || stage.isPrimarySecondStage() || stage.isSecondarySecondStage()|| stage.isThirdStage() , "Transconductance might only be a helper Structure");

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
		else
		{
			Partitioning::Component & compStage = **getPartitioningResult().getBelongingComponents(stage).begin();
			outputNets.push_back(&compStage.getArray().findNet(StructRec::StructurePinType(compStage.getArray().getStructureName(), "Drain")));
		}
		return outputNets;
	}




	Gecode::FloatVar SearchSpace::createFloatWidth(Gecode::IntVar intWidth)
	{
		Gecode::FloatVar channeledWidth = createChanneledFloatVar(intWidth, 1, widthUpperBound_);
		return expr(*this, channeledWidth * getScalingFactorMUM());
	}

	Gecode::FloatVar SearchSpace::createFloatLength(Gecode::IntVar intLength)
	{
		Gecode::FloatVar channeledLength = createChanneledFloatVar(intLength, 1, lengthUpperBound_);
		return expr(*this, channeledLength * getScalingFactorMUM());
	}

	Gecode::FloatVar SearchSpace::createFloatCurrent(Gecode::IntVar intCurrent)
	{
		Gecode::FloatVar channeledCurrent = createChanneledFloatVar(intCurrent, -10000000000, 10000000000);
		return expr(*this, channeledCurrent * pow(10,-9));
	}

	Gecode::FloatVar SearchSpace::createFloatVoltage(Gecode::IntVar intVoltage)
	{
		int vdd = getCircuitInformation().getCircuitParameter().getSupplyVoltage() * 1000;
		int vss = getCircuitInformation().getCircuitParameter().getGroundVoltage() *1000;
		Gecode::FloatVar channeledVoltage = createChanneledFloatVar(intVoltage, vss, vdd);
		return expr(*this, channeledVoltage * pow(10,-3));
	}

	Gecode::FloatVar SearchSpace::createChanneledFloatVar(Gecode::IntVar integer, long int lb, long int ub)
	{
		Gecode::FloatVar helperVar(*this, lb, ub);
		channel(*this, integer, helperVar);

		return helperVar;

	}

	bool SearchSpace::fulfillsFirstStageSpecifications() const
	{
		bool fulfills = true;
		if(getPhaseMargin() < getCircuitInformation().getCircuitSpecification().getPhaseMargin())
		{
//			logDebug("PhaseMargn!");
			fulfills = false;
		}
		if(getMinCommonModeInputVoltage() >
		getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() + getCircuitInformation().getCircuitSpecification().getMinCommonModeInputVoltage())
		{

//			logDebug("MinCommonModeInputVoltage!");
			fulfills = false;
		}
		if(getMaxCommonModeInputVoltage() <
	 			getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() + getCircuitInformation().getCircuitSpecification().getMaxCommonModeInputVoltage()
				&& getMaxCommonModeInputVoltage() > -100000 )
		{
//			logDebug("MaxCommonModeInputVoltage!");
			fulfills = false;
		}
		if(getCMRR()< getCircuitInformation().getCircuitSpecification().getCMRR())
		{
//			logDebug("CMRR!");
			fulfills = false;
		}
		if(getArea() > getCircuitInformation().getCircuitSpecification().getMaxArea())
		{
//			logDebug("Area!");
			fulfills = false;
		}
		if(getPowerConsumption() > getCircuitInformation().getCircuitSpecification().getPowerConsumption())
		{
//			logDebug("PowerConsumption!");
			fulfills = false;
		}
		return fulfills;

	}

	bool SearchSpace::fulfillsSpecifications() const
	{
		bool fulfills = true;
		if(!fulfillsFirstStageSpecifications())
		{
//			logDebug("FirstStageSpecification!!!!");
			fulfills = false;
		}
		if(getGain() < getCircuitInformation().getCircuitSpecification().getGain())
		{
//			logDebug("Gain!");
			fulfills = false;
		}
		if(getMinOutputVoltage() > getCircuitInformation().getCircuitSpecification().getMinOutputVoltage())
		{
//			logDebug("MinOutputVoltageSwing!!");
			fulfills = false;

		}
		if(getMaxOutputVoltage() < getCircuitInformation().getCircuitSpecification().getMaxOutputVoltage())
		{
//			logDebug("MaxOutputVoltageSwing!");
			fulfills = false;
		}
		if(getTransitFrequency() < getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
//			logDebug("TransitFrequency!");
			fulfills = false;
		}
		if(getTransitFrequencyWithErrorFactor() < getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
//			logDebug("TransitFrequencyWithErrorFactor!");
			fulfills = false;
		}
		if(getSlewRate() < getCircuitInformation().getCircuitSpecification().getSlewRate())
		{
//			logDebug("SlewRate!");
			fulfills = false;
		}
		if(getNegPSRR() < getCircuitInformation().getCircuitSpecification().getNegPSRR() && getNegPSRR()>0)
		{
//			logDebug("negPSRR!");
			fulfills = false;
		}
		if(getPosPSRR()< getCircuitInformation().getCircuitSpecification().getPosPSRR() && getPosPSRR() >0)
		{
//			logDebug("posPSRR!");
			fulfills = false;
		}

		return fulfills;
	}


	bool SearchSpace::usesHSpiceLibrary() const
	{
		return usesHSpiceLibrary_;
	}

}

