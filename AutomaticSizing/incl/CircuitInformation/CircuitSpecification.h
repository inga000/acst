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


#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITSPECIFICATION_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITSPECIFICATION_H_

#include <gecode/float.hh>

namespace AutomaticSizing {


	class CircuitSpecification {
	public:
		CircuitSpecification();
		~CircuitSpecification();

		CircuitSpecification & clone() const;

		void setGain(float gain);
		void setTransientFrequency(float transientFrequency);
		void setSlewRate(float slewRate);
		void setCMRR(float CMRR);

		void setNegPSRR(float negPSRR);
		void setPosPSRR(float posPSRR);

		void setMaxOutputVoltage(float maxOutputSwing);
		void setMinOutputVoltage(float minOutputSwing);
		void setMaxCommonModeInputVoltage(float vcmMax);
		void setMinCommonModeInputVoltage(float vcmMin);
		void setPowerConsumption(float powerConsumption);
		void setMaxArea(float area);
		void setPhaseMargin(float phaseMargin);

		void setGateOverDriveVoltage(float gateOverDriveVoltage);


		float getGain() const;
		float getTransientFrequency() const;
		float getSlewRate() const;
		float getCMRR() const;
		float getPhaseMargin() const;

		float getPosPSRR() const;
		float getNegPSRR() const;
		float getMaxCommonModeInputVoltage() const;
		float getMinCommonModeInputVoltage() const;

		float getMaxOutputVoltage() const;
		float getMinOutputVoltage() const;
		float getPowerConsumption() const;
		float getMaxArea() const;

		float getGateOverDriveVoltage() const;

		bool hasGateOverDriveVoltage() const;


	private:
		static const float NOT_INITIALIZED_;

		bool hasGain() const;
		bool hasTransientFrequency() const;
		bool hasSlewRate() const;
		bool hasCMRR() const;

		bool hasPosPSRR() const;
		bool hasNegPSRR() const;


		bool hasMaxCommonModeInputVoltage() const;
		bool hasMinCommonModeInputVoltage() const;
		bool hasMaxOutputVoltage() const;
		bool hasMinOutputVoltage() const;
		bool hasPowerConsumption() const;
		bool hasMaxArea() const;
		bool hasPhaseMargin() const;
		bool hasSettlingTime() const;



		float gain_;
		float transientFrequency_;
		float slewRate_;
		float CMRR_;

		float posPSRR_;
		float negPSRR_;
		float maxOutputVoltage_;
		float minOutputVoltage_;
		float vcmMin_;
		float vcmMax_;
		float maxPowerConsumption_;
		float maxArea_;
		float gateOverDriveVoltage_;
		float phaseMargin_;
	};


}



#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITSPECIFICATION_H_ */
