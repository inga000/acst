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

#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITPARAMETER_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITPARAMETER_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"



namespace Partitioning {

	class Result;

}

namespace AutomaticSizing {

	class Pin;
	class LoadCapacity;

	class CircuitParameter {
	public:
		CircuitParameter();
		~CircuitParameter();

		CircuitParameter & clone() const;

		void addLoadCapacity(LoadCapacity & loadCapacity);
		void setSupplyVoltagePin(const Pin & pin);
		void setCurrentBiasPin(Pin & pin);
		void addVoltageBiasPin(const Pin & pin);
		void setInputPinMinus(const Pin & pin);
		void setInputPinPlus(const Pin & pin);
		void setOutputPin(const Pin & pin);
		void setOutputPinPlus(const Pin & pin);
		void setOutputPinMinus(const Pin & pin);
		void setGroundPin(const Pin & pin);
		void setReferenceVoltagePin(const Pin & pin);
		void setFullyDifferential(bool isIt);
		void setComplementary(bool isIt);

		std::vector<LoadCapacity *> getLoadCapacities() const;
		int getLoadCapacity(Core::DeviceId deviceId) const;
		LoadCapacity & getMaxLoadCapacity() const;
		LoadCapacity & getMinLoadCapacity() const;


		float getSupplyVoltage() const;
		float getGroundVoltage() const;
		float getValue(Core::NetId netId) const;
		const Pin & getSupplyVoltagePin() const;
		const Pin & getCurrentBiasPin() const;
		Pin & getCurrentBiasPin();
		const Pin & getInputPinMinus() const;
		const Pin & getInputPinPlus() const;
		const Pin & getOutputPin() const;
		const Pin & getOutputPinMinus() const;
		const Pin & getOutputPinPlus() const;
		const Pin & getGroundPin() const;
		const Pin & getReferenceVoltagePin() const;
		const Pin & findOutputPinWithBiggestLoadCapacity(const Partitioning::Result & result) const;
		std::vector<const Pin *> getVoltageBiasPins() const;

		bool isInputNet(Core::NetId netId) const;
		bool isOutputNet(Core::NetId netId) const;
		bool isSupplyNet(Core::NetId netId) const;
		bool isGroundNet(Core::NetId netId) const;
		bool isCurrentBiasNet(Core::NetId netId) const;
		bool isVoltageBiasNet(Core::NetId netId) const;
		bool isReferenceVoltageNet(Core::NetId netId) const;

		bool hasReferenceVoltagePin() const;
		bool hasCurrentBiasPin() const;

		bool isFullyDifferential() const;
		bool isComplementary()const;
	private:

		static const int NOT_INITIALIZED_;

		bool hasLoadCapacities() const;
		bool hasLoadCapacity(Core::DeviceId deviceId) const;
		bool hasSupplyVoltagePin() const;
		bool hasInputPinMinus() const;
		bool hasInputPinPlus() const;
		bool hasOutputPin() const;
		bool hasOutputPinMinus() const;
		bool hasOutputPinPlus() const;
		bool hasGroundPin() const;


		bool isInputNetMinus(Core::NetId netId) const;
		bool isInputNetPlus(Core::NetId netId) const;

		const Pin& getCorrespondingPin(Core::NetId netId) const;

	private:

		void eraseVoltageBiasPins();
		void eraseLoadCapacities();

		std::vector<LoadCapacity*> loadCapacities_;
		const Pin* supplyVoltagePin_;
		Pin*  currentBiasPin_;
		const Pin* inputPinMinus_;
		const Pin* inputPinPlus_;
		const Pin* outputPin_;
		const Pin* outputPinMinus_;
		const Pin* outputPinPlus_;
		const Pin* groundPin_;
		const Pin * referenceVoltagePin_;
		std::vector<const Pin *> voltageBiasPins_;
		bool isFullyDifferential_;
		bool isComplementary_;
	};


}




#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITPARAMETER_H_ */
