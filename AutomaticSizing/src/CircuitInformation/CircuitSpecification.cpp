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


#include "AutomaticSizing/incl/CircuitInformation/CircuitSpecification.h"

#include "Log/incl/LogMacros.h"


namespace AutomaticSizing {

	const float CircuitSpecification::NOT_INITIALIZED_ = -100;

	CircuitSpecification::CircuitSpecification() :
			gain_(NOT_INITIALIZED_),
			transientFrequency_(NOT_INITIALIZED_),
			slewRate_(NOT_INITIALIZED_),
			CMRR_(NOT_INITIALIZED_),
			posPSRR_(NOT_INITIALIZED_),
			negPSRR_(NOT_INITIALIZED_),
			vcmMin_(NOT_INITIALIZED_),
			vcmMax_(NOT_INITIALIZED_),
			maxOutputVoltage_(NOT_INITIALIZED_),
			minOutputVoltage_(NOT_INITIALIZED_),
			maxPowerConsumption_(NOT_INITIALIZED_),
			maxArea_(NOT_INITIALIZED_),
			gateOverDriveVoltage_(NOT_INITIALIZED_),
			phaseMargin_(NOT_INITIALIZED_)
	{
	}

	CircuitSpecification & CircuitSpecification::clone() const
	{
		CircuitSpecification* newSpec = new CircuitSpecification;

		newSpec->setGain(gain_);
		newSpec->setTransientFrequency(transientFrequency_);
		newSpec->setSlewRate(slewRate_);
		newSpec->setCMRR(CMRR_);

		newSpec->setPosPSRR(posPSRR_);
		newSpec->setNegPSRR(negPSRR_);
		newSpec->setMaxOutputVoltage(maxOutputVoltage_);
		newSpec->setMinOutputVoltage(minOutputVoltage_);
		newSpec->setMinCommonModeInputVoltage(vcmMin_);
		newSpec->setMaxCommonModeInputVoltage(vcmMax_);
		newSpec->setPowerConsumption(maxPowerConsumption_);
		newSpec->setMaxArea(maxArea_);
		newSpec->setGateOverDriveVoltage(gateOverDriveVoltage_);
		newSpec->setPhaseMargin(phaseMargin_);

		return * newSpec;

	}

	CircuitSpecification::~CircuitSpecification()
	{
	}

	void CircuitSpecification::setGain(float gain)
	{
		gain_ = gain;
	}

	void CircuitSpecification::setTransientFrequency(
		float transientFrequency)
	{
		transientFrequency_ = transientFrequency;
	}

	void CircuitSpecification::setSlewRate(float slewRate)
	{
		slewRate_ = slewRate;
	}

	void CircuitSpecification::setCMRR(float CMRR)
	{
		CMRR_ = CMRR;
	}


	void CircuitSpecification::setPosPSRR(float posPSRR)
	{
		posPSRR_ = posPSRR;
	}

	void CircuitSpecification::setNegPSRR(float negPSRR)
	{
		negPSRR_ = negPSRR;
	}

	void CircuitSpecification::setMaxOutputVoltage(
		float maxOutputSwing)
	{
		maxOutputVoltage_ = maxOutputSwing;
	}

	void CircuitSpecification::setMinOutputVoltage(
		float minOutputSwing)
	{
		minOutputVoltage_ = minOutputSwing;
	}


	void CircuitSpecification::setPowerConsumption(
		float powerConsumption)
	{
		maxPowerConsumption_ = powerConsumption;
	}

	float CircuitSpecification::getGain() const
	{
		assert(hasGain());
		return gain_;
	}

	float CircuitSpecification::getTransientFrequency() const
	{
		assert(hasTransientFrequency());
		return transientFrequency_;
	}

	float CircuitSpecification::getSlewRate() const
	{
		assert(hasSlewRate());
		return slewRate_;
	}

	float CircuitSpecification::getCMRR() const
	{
		assert(hasCMRR());
		return CMRR_;
	}


	float CircuitSpecification::getPosPSRR() const
	{
		assert(hasPosPSRR());
		return posPSRR_;
	}

	float CircuitSpecification::getNegPSRR() const
	{
		assert(hasNegPSRR());
		return negPSRR_;
	}

	float CircuitSpecification::getMaxOutputVoltage() const
	{
		assert(hasMaxOutputVoltage());
		return maxOutputVoltage_;
	}

	float CircuitSpecification::getMinOutputVoltage() const
	{
		assert(hasMinOutputVoltage());
		return minOutputVoltage_;
	}


	float CircuitSpecification::getPowerConsumption() const
	{
		assert(hasPowerConsumption());
		return maxPowerConsumption_;
	}

	bool CircuitSpecification::hasGain() const
	{
		return gain_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasTransientFrequency() const
	{
		return transientFrequency_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasSlewRate() const
	{
		return slewRate_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasCMRR() const
	{
		return CMRR_ != NOT_INITIALIZED_;
	}


	bool CircuitSpecification::hasPosPSRR() const
	{
		return posPSRR_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasNegPSRR() const
	{
		return negPSRR_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasMaxOutputVoltage() const
	{
		return maxOutputVoltage_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasMinOutputVoltage() const
	{
		return minOutputVoltage_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasPowerConsumption() const
	{
		return maxPowerConsumption_ != NOT_INITIALIZED_;
	}

	void CircuitSpecification::setMaxArea(float area)
	{
		maxArea_ = area;
	}

	float CircuitSpecification::getMaxArea() const
	{
		assert(hasMaxArea());
		return maxArea_;
	}


	void CircuitSpecification::setGateOverDriveVoltage(float gateOverDriveVoltage)
	{
		gateOverDriveVoltage_ = gateOverDriveVoltage;
	}

	float CircuitSpecification::getGateOverDriveVoltage() const
	{
		assert(hasGateOverDriveVoltage());
		return gateOverDriveVoltage_;
	}


	bool CircuitSpecification::hasMaxArea() const
	{
		return maxArea_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasGateOverDriveVoltage() const
	{
		return gateOverDriveVoltage_ != NOT_INITIALIZED_;
	}


	void CircuitSpecification::setMaxCommonModeInputVoltage(float vcmMax)
	{
		vcmMax_ = vcmMax;
	}

	void CircuitSpecification::setMinCommonModeInputVoltage(float vcmMin)
	{
		vcmMin_ = vcmMin;
	}

	void CircuitSpecification::setPhaseMargin(float phaseMargin)
	{

		phaseMargin_ = phaseMargin;
	}

	float CircuitSpecification::getMaxCommonModeInputVoltage() const
	{
		assert(hasMaxCommonModeInputVoltage());
		return vcmMax_;
	}

	float CircuitSpecification::getMinCommonModeInputVoltage() const
	{
		assert(hasMinCommonModeInputVoltage());
		return vcmMin_;
	}

	float CircuitSpecification::getPhaseMargin() const
	{
		assert(hasPhaseMargin());
		return phaseMargin_;
	}

	bool CircuitSpecification::hasPhaseMargin() const
	{
		return phaseMargin_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasMaxCommonModeInputVoltage() const
	{
		return vcmMax_ != NOT_INITIALIZED_;
	}

	bool CircuitSpecification::hasMinCommonModeInputVoltage() const
	{
		return vcmMin_ != NOT_INITIALIZED_;
	}



}


