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


#include "../../incl/CircuitInformation/TechnologieSpecificationSHM.h"

namespace AutomaticSizing {

const int TechnologieSpecificationSHM::NOT_INITIALIZED_ = -100;

	TechnologieSpecificationSHM::TechnologieSpecificationSHM() :
				thermalVoltage_(NOT_INITIALIZED_),
				thresholdVoltage_(NOT_INITIALIZED_),
				muCox_(NOT_INITIALIZED_),
				overlapCapacity_(NOT_INITIALIZED_),
				gateOxideCapacity_(NOT_INITIALIZED_),
				slopeFactor_(NOT_INITIALIZED_),
				Amin_(NOT_INITIALIZED_),
				Lmin_(NOT_INITIALIZED_),
				Wmin_(NOT_INITIALIZED_),
				lambdaWeakInversion_(NOT_INITIALIZED_),
				lambdaStrongInversion_(NOT_INITIALIZED_),
				cj_(NOT_INITIALIZED_),
				cjsw_(NOT_INITIALIZED_),
				pb_(NOT_INITIALIZED_),
				ldiff_(NOT_INITIALIZED_)
	{
	}

	TechnologieSpecificationSHM & TechnologieSpecificationSHM::clone() const
	{
		TechnologieSpecificationSHM * techSpec = new TechnologieSpecificationSHM;

		techSpec->setBulkJunctionContactPotential(getBulkJunctionContactPotential());
		techSpec->setChannelLengthCoefficientStrongInversion(getChannelLengthCoefficientStrongInversion());
		techSpec->setChannelLengthCoefficientWeakInversion(getChannelLengthCoefficientWeakInversion());
		techSpec->setThermalVoltage(getThermalVoltage());
		techSpec->setGateOxideCapacity(getGateOxideCapacity());
		techSpec->setLateralDiffusionLength(getLateralDiffusionLength());
		techSpec->setMinArea(getMinArea());
		techSpec->setMinLength(getMinLength());
		techSpec->setMinWidth(getMinWidth());
		techSpec->setMobilityOxideCapacityCoefficient(getMobilityOxideCapacityCoefficient());
		techSpec->setOverlapCapacity(getOverlapCapacity());
		techSpec->setSlopeFactor(getSlopeFactor());
		techSpec->setThresholdVoltage(getThresholdVoltage());
		techSpec->setZeroBiasBulkJunctionCapacitance(getZeroBiasBulkJunctionCapacitance());
		techSpec->setZeroBiasSidewallBulkJunctionCapacitance(getZeroBiasSidewallBulkJunctionCapacitance());

		return *techSpec;
	}


	void TechnologieSpecificationSHM::setThermalVoltage(
		float voltage)
	{
		thermalVoltage_ = voltage;
	}

	float TechnologieSpecificationSHM::getThermalVoltage() const
	{
		assert(thermalVoltage_ != NOT_INITIALIZED_);
		return thermalVoltage_;
	}

	void TechnologieSpecificationSHM::setThresholdVoltage(
		float voltage)
	{
		thresholdVoltage_ = voltage;
	}

	void TechnologieSpecificationSHM::setMobilityOxideCapacityCoefficient(
		float muCox)
	{
		muCox_ = muCox;
	}

	float TechnologieSpecificationSHM::getThresholdVoltage() const
	{
		assert(hasThresholdVoltage());
		return thresholdVoltage_;
	}

	float TechnologieSpecificationSHM::getMobilityOxideCapacityCoefficient() const
	{
		assert(hasMobilityOxideCapacityCoefficient());
		return muCox_;
	}

	bool TechnologieSpecificationSHM::hasThresholdVoltage() const
	{
		return thresholdVoltage_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationSHM::hasMobilityOxideCapacityCoefficient() const
	{
		return muCox_ != NOT_INITIALIZED_;
	}


	void TechnologieSpecificationSHM::setOverlapCapacity(float overlapCapacity)
	{
		overlapCapacity_ = overlapCapacity;
	}

	float TechnologieSpecificationSHM::getOverlapCapacity() const
	{
		assert(hasOverlapCapacity());
		return overlapCapacity_;
	}

	bool TechnologieSpecificationSHM::hasOverlapCapacity() const
	{
		return overlapCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationSHM::setGateOxideCapacity(float gateOxideCapacity)
	{
		gateOxideCapacity_ = gateOxideCapacity;
	}

	float TechnologieSpecificationSHM::getGateOxideCapacity() const
	{
		assert(hasGateOxideCapacity());
		return gateOxideCapacity_;
	}

	void TechnologieSpecificationSHM::setSlopeFactor(float slopeFactor)
	{
		slopeFactor_ = slopeFactor;
	}

	float TechnologieSpecificationSHM::getSlopeFactor() const
	{
		assert(hasSlopeFactor());
		return slopeFactor_;
	}

	bool TechnologieSpecificationSHM::hasGateOxideCapacity() const
	{
		return gateOxideCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationSHM::setChannelLengthCoefficientStrongInversion(float lamda)
	{
		lambdaStrongInversion_ = lamda;
	}

	void TechnologieSpecificationSHM::setChannelLengthCoefficientWeakInversion(float lamda)
	{
		lambdaWeakInversion_ = lamda;
	}

	float TechnologieSpecificationSHM::getChannelLengthCoefficientStrongInversion() const
	{
		assert(lambdaStrongInversion_ != NOT_INITIALIZED_);
		return lambdaStrongInversion_;
	}

	float TechnologieSpecificationSHM::getChannelLengthCoefficientWeakInversion() const
	{
		assert(lambdaWeakInversion_ != NOT_INITIALIZED_);
		return lambdaWeakInversion_;
	}

	bool TechnologieSpecificationSHM::hasSlopeFactor() const
	{
		return slopeFactor_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationSHM::setMinArea(int minArea)
	{
		Amin_ =minArea;
	}

	void TechnologieSpecificationSHM::setMinLength(int Lmin)
	{
		Lmin_ = Lmin;
	}

	void TechnologieSpecificationSHM::setMinWidth(int minWidth)
	{
		Wmin_ = minWidth;
	}

	int TechnologieSpecificationSHM::getMinArea() const
	{
		assert(Amin_ != NOT_INITIALIZED_);
		return Amin_;
	}

	int TechnologieSpecificationSHM::getMinWidth() const
	{
		assert(Wmin_ != NOT_INITIALIZED_);
		return Wmin_;
	}

	int TechnologieSpecificationSHM::getMinLength() const
	{
		assert(Lmin_ != NOT_INITIALIZED_);
		return Lmin_;
	}

	void TechnologieSpecificationSHM::setZeroBiasBulkJunctionCapacitance(float Cj)
	{
		cj_ = Cj;
	}

	void TechnologieSpecificationSHM::setZeroBiasSidewallBulkJunctionCapacitance(
		float Cjsw)
	{
		cjsw_ = Cjsw;
	}

	void TechnologieSpecificationSHM::setBulkJunctionContactPotential(float pb)
	{
		pb_ = pb;
	}

	void TechnologieSpecificationSHM::setLateralDiffusionLength(float Ldiff)
	{
		ldiff_ = Ldiff;
	}

	float TechnologieSpecificationSHM::getZeroBiasBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasBulkJunctionCapacitance());
		return cj_;
	}

	float TechnologieSpecificationSHM::getZeroBiasSidewallBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasSidewallBulkJunctionCapacitance());
		return cjsw_;
	}

	float TechnologieSpecificationSHM::getBulkJunctionContactPotential() const
	{
		assert(hasBulkJunctionContactPotential());
		return pb_;
	}

	float TechnologieSpecificationSHM::getLateralDiffusionLength() const
	{
		assert(hasLateralDiffusionLength());
		return ldiff_;
	}

	bool TechnologieSpecificationSHM::hasZeroBiasBulkJunctionCapacitance() const
	{
		return cj_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationSHM::hasZeroBiasSidewallBulkJunctionCapacitance() const
	{
		return cjsw_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationSHM::hasBulkJunctionContactPotential() const
	{
		return pb_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationSHM::hasLateralDiffusionLength() const
	{
		return ldiff_ != NOT_INITIALIZED_;
	}

}

                                                                                                                                                                                                                                                                                                                                                                                                                         
