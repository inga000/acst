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

#include "../../incl/CircuitInformation/TechnologieSpecificationEKV.h"

namespace AutomaticSizing {

const int TechnologieSpecificationEKV::NOT_INITIALIZED_ = -100;

	TechnologieSpecificationEKV::TechnologieSpecificationEKV() :
				thresholdVoltage_(NOT_INITIALIZED_),
				muCox_(NOT_INITIALIZED_),
				earlyVoltage_(NOT_INITIALIZED_),
				overlapCapacity_(NOT_INITIALIZED_),
				gateOxideCapacity_(NOT_INITIALIZED_),
				slopeFactor_(NOT_INITIALIZED_),
				Amin_(NOT_INITIALIZED_),
				Lmin_(NOT_INITIALIZED_),
				Wmin_(NOT_INITIALIZED_),
				lambda_(NOT_INITIALIZED_),
				cj_(NOT_INITIALIZED_),
				cjsw_(NOT_INITIALIZED_),
				pb_(NOT_INITIALIZED_),
				ldiff_(NOT_INITIALIZED_)
	{
	}

	TechnologieSpecificationEKV & TechnologieSpecificationEKV::clone() const
	{
		TechnologieSpecificationEKV * techSpec = new TechnologieSpecificationEKV;

		techSpec->setBulkJunctionContactPotential(getBulkJunctionContactPotential());
		techSpec->setChannelLengthCoefficient(getChannelLengthCoefficient());
		techSpec->setEarlyVoltage(getEarlyVoltage());
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

	void TechnologieSpecificationEKV::setThresholdVoltage(
		float voltage)
	{
		thresholdVoltage_ = voltage;
	}

	void TechnologieSpecificationEKV::setMobilityOxideCapacityCoefficient(
		float muCox)
	{
		muCox_ = muCox;
	}

	void TechnologieSpecificationEKV::setEarlyVoltage(
		float earlyVoltage)
	{
		earlyVoltage_ = earlyVoltage;
	}

	float TechnologieSpecificationEKV::getThresholdVoltage() const
	{
		assert(hasThresholdVoltage());
		return thresholdVoltage_;
	}

	float TechnologieSpecificationEKV::getMobilityOxideCapacityCoefficient() const
	{
		assert(hasMobilityOxideCapacityCoefficient());
		return muCox_;
	}

	float TechnologieSpecificationEKV::getEarlyVoltage() const
	{
		assert(hasEarlyVoltage());
		return earlyVoltage_;
	}

	bool TechnologieSpecificationEKV::hasThresholdVoltage() const
	{
		return thresholdVoltage_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasMobilityOxideCapacityCoefficient() const
	{
		return muCox_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasEarlyVoltage() const
	{
		return earlyVoltage_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setOverlapCapacity(float overlapCapacity)
	{
		overlapCapacity_ = overlapCapacity;
	}

	float TechnologieSpecificationEKV::getOverlapCapacity() const
	{
		assert(hasOverlapCapacity());
		return overlapCapacity_;
	}

	bool TechnologieSpecificationEKV::hasOverlapCapacity() const
	{
		return overlapCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setGateOxideCapacity(float gateOxideCapacity)
	{
		gateOxideCapacity_ = gateOxideCapacity;
	}

	float TechnologieSpecificationEKV::getGateOxideCapacity() const
	{
		assert(hasGateOxideCapacity());
		return gateOxideCapacity_;
	}

	void TechnologieSpecificationEKV::setSlopeFactor(float slopeFactor)
	{
		slopeFactor_ = slopeFactor;
	}

	float TechnologieSpecificationEKV::getSlopeFactor() const
	{
		assert(hasSlopeFactor());
		return slopeFactor_;
	}

	bool TechnologieSpecificationEKV::hasGateOxideCapacity() const
	{
		return gateOxideCapacity_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setChannelLengthCoefficient(float lamda)
	{
		lambda_ = lamda;
	}

	float TechnologieSpecificationEKV::getChannelLengthCoefficient() const
	{
		assert(lambda_ != NOT_INITIALIZED_);
		return lambda_;
	}

	bool TechnologieSpecificationEKV::hasSlopeFactor() const
	{
		return slopeFactor_ != NOT_INITIALIZED_;
	}

	void TechnologieSpecificationEKV::setMinArea(float minArea)
	{
		Amin_ =minArea;
	}

	void TechnologieSpecificationEKV::setMinLength(float Lmin)
	{
		Lmin_ = Lmin;
	}

	void TechnologieSpecificationEKV::setMinWidth(float minWidth)
	{
		Wmin_ = minWidth;
	}

	float TechnologieSpecificationEKV::getMinArea() const
	{
		assert(Amin_ != NOT_INITIALIZED_);

		return Amin_;
	}

	float TechnologieSpecificationEKV::getMinWidth() const
	{
		assert(Wmin_ != NOT_INITIALIZED_);
		return Wmin_;
	}

	float TechnologieSpecificationEKV::getMinLength() const
	{
		assert(Lmin_ != NOT_INITIALIZED_);
		return Lmin_;
	}

	void TechnologieSpecificationEKV::setZeroBiasBulkJunctionCapacitance(float Cj)
	{
		cj_ = Cj;
	}

	void TechnologieSpecificationEKV::setZeroBiasSidewallBulkJunctionCapacitance(
		float Cjsw)
	{
		cjsw_ = Cjsw;
	}

	void TechnologieSpecificationEKV::setBulkJunctionContactPotential(float pb)
	{
		pb_ = pb;
	}

	void TechnologieSpecificationEKV::setLateralDiffusionLength(float Ldiff)
	{
		ldiff_ = Ldiff;
	}

	float TechnologieSpecificationEKV::getZeroBiasBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasBulkJunctionCapacitance());
		return cj_;
	}

	float TechnologieSpecificationEKV::getZeroBiasSidewallBulkJunctionCapacitance() const
	{
		assert(hasZeroBiasSidewallBulkJunctionCapacitance());
		return cjsw_;
	}

	float TechnologieSpecificationEKV::getBulkJunctionContactPotential() const
	{
		assert(hasBulkJunctionContactPotential());
		return pb_;
	}


	float TechnologieSpecificationEKV::getLateralDiffusionLength() const
	{
		assert(hasLateralDiffusionLength());
		return ldiff_;
	}

	bool TechnologieSpecificationEKV::hasZeroBiasBulkJunctionCapacitance() const
	{
		return cj_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasZeroBiasSidewallBulkJunctionCapacitance() const
	{
		return cjsw_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasBulkJunctionContactPotential() const
	{
		return pb_ != NOT_INITIALIZED_;
	}

	bool TechnologieSpecificationEKV::hasLateralDiffusionLength() const
	{
		return ldiff_ != NOT_INITIALIZED_;
	}

}

                                                                                                                                                                                                                                                                                                                                                                                                                         
