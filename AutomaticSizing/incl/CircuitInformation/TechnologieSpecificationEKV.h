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


#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONEKV_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATIONEKV_H_


#include <gecode/float.hh>


namespace AutomaticSizing {

	class TechnologieSpecificationEKV
	{
	public:
		TechnologieSpecificationEKV();

		TechnologieSpecificationEKV & clone() const;

		void setThresholdVoltage(float voltage);
		void setMobilityOxideCapacityCoefficient(float muCox);
		void setEarlyVoltage(float earlyVoltage);
		void setOverlapCapacity(float overlapCapacity);
		void setGateOxideCapacity(float gateOxideCapacity);
		void setSlopeFactor(float slopeFactor);
		void setMinArea(float minArea );
		void setMinLength(float Lmin);
		void setMinWidth(float minWidth);
		void setChannelLengthCoefficient(float lamda);

		void setZeroBiasBulkJunctionCapacitance(float Cj);
		void setZeroBiasSidewallBulkJunctionCapacitance(float Cjsw);
		void setBulkJunctionContactPotential(float pb);
		void setLateralDiffusionLength(float Ldiff);


		float getOverlapCapacity() const;
		float getThresholdVoltage() const;
		float getMobilityOxideCapacityCoefficient() const;
		float getEarlyVoltage() const;
		float getGateOxideCapacity() const;
		float getSlopeFactor() const;
		float getChannelLengthCoefficient() const;
		float getMinArea() const;
		float getMinWidth() const;
		float getMinLength() const;

		float getZeroBiasBulkJunctionCapacitance() const;
		float getZeroBiasSidewallBulkJunctionCapacitance() const;
		float getBulkJunctionContactPotential() const;
		float getLateralDiffusionLength() const;


	private:
		static const int NOT_INITIALIZED_;

		bool hasOverlapCapacity() const;
		bool hasThresholdVoltage() const;
		bool hasMobilityOxideCapacityCoefficient() const;
		bool hasEarlyVoltage() const;
		bool hasGateOxideCapacity() const;
		bool hasSlopeFactor() const;

		bool hasZeroBiasBulkJunctionCapacitance() const;
		bool hasZeroBiasSidewallBulkJunctionCapacitance() const;
		bool hasBulkJunctionContactPotential() const;
		bool hasLateralDiffusionLength() const;

	private:
		float thresholdVoltage_;
		float muCox_;
		float earlyVoltage_;
		float overlapCapacity_;
		float gateOxideCapacity_;
		float slopeFactor_;
		float Amin_;
		float Lmin_;
		float Wmin_;
		float lambda_;
		float pb_;
		float cj_;
		float cjsw_;
		float ldiff_;

	};


}


#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_TECHNOLOGIESPECIFICATION_H_ */
