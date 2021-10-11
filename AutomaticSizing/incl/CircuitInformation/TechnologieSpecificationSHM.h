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


#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONSHM_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONSHM_H_


#include <gecode/float.hh>


namespace AutomaticSizing {

	class TechnologieSpecificationSHM
	{
	public:
		TechnologieSpecificationSHM();

		TechnologieSpecificationSHM & clone() const;

		void setThermalVoltage(float voltage);

		void setThresholdVoltage(float voltage);
		void setMobilityOxideCapacityCoefficient(float muCox);
		void setOverlapCapacity(float overlapCapacity);
		void setGateOxideCapacity(float gateOxideCapacity);
		void setSlopeFactor(float slopeFactor);
		void setMinArea(int minArea );
		void setMinLength(int Lmin);
		void setMinWidth(int minWidth);
		void setChannelLengthCoefficientStrongInversion(float lamda);
		void setChannelLengthCoefficientWeakInversion(float lamda);

		void setZeroBiasBulkJunctionCapacitance(float Cj);
		void setZeroBiasSidewallBulkJunctionCapacitance(float Cjsw);
		void setBulkJunctionContactPotential(float pb);
		void setLateralDiffusionLength(float Ldiff);

		float getThermalVoltage() const;

		float getOverlapCapacity() const;
		float getThresholdVoltage() const;
		float getMobilityOxideCapacityCoefficient() const;
		float getGateOxideCapacity() const;
		float getSlopeFactor() const;
		float getChannelLengthCoefficientStrongInversion() const;
		float getChannelLengthCoefficientWeakInversion() const;
		int getMinArea() const;
		int getMinWidth() const;
		int getMinLength() const;

		float getZeroBiasBulkJunctionCapacitance() const;
		float getZeroBiasSidewallBulkJunctionCapacitance() const;
		float getBulkJunctionContactPotential() const;
		float getLateralDiffusionLength() const;


	private:
		static const int NOT_INITIALIZED_;

		bool hasOverlapCapacity() const;
		bool hasThresholdVoltage() const;
		bool hasMobilityOxideCapacityCoefficient() const;
		bool hasGateOxideCapacity() const;
		bool hasSlopeFactor() const;

		bool hasZeroBiasBulkJunctionCapacitance() const;
		bool hasZeroBiasSidewallBulkJunctionCapacitance() const;
		bool hasBulkJunctionContactPotential() const;
		bool hasLateralDiffusionLength() const;

	private:
		float thermalVoltage_;

		float thresholdVoltage_;
		float muCox_;
		float overlapCapacity_;
		float gateOxideCapacity_;
		float slopeFactor_;
		int Amin_;
		int Lmin_;
		int Wmin_;
		float lambdaWeakInversion_;
		float lambdaStrongInversion_;
		float pb_;
		float cj_;
		float cjsw_;
		float ldiff_;

	};


}


#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONSHM_H_ */
