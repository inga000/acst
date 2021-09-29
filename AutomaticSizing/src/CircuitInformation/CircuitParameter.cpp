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

#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/CircuitInformation/LoadCapacity.h"

#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include "Log/incl/LogMacros.h"

namespace AutomaticSizing {

const int CircuitParameter::NOT_INITIALIZED_ = -100;

	CircuitParameter::CircuitParameter()
	{
		supplyVoltagePin_ = NULL;
		groundPin_ = NULL;
		inputPinMinus_ = NULL;
		inputPinPlus_ = NULL;
		currentBiasPin_ =NULL;
		outputPin_ = NULL;
		outputPinPlus_ = NULL;
		outputPinMinus_ = NULL;
		referenceVoltagePin_ = nullptr;
		isFullyDifferential_ = false;
		isComplementary_ = false;
	}

	CircuitParameter & CircuitParameter::clone() const
	{
		CircuitParameter * newParameter = new CircuitParameter;

		newParameter->setFullyDifferential(isFullyDifferential());

		for(auto &loadCap : getLoadCapacities())
		{
			LoadCapacity & newLoadCap = loadCap->clone();
			newParameter->addLoadCapacity(newLoadCap);
		}

		newParameter->setSupplyVoltagePin(getSupplyVoltagePin().clone());
		newParameter->setGroundPin(getGroundPin().clone());

		newParameter->setCurrentBiasPin(getCurrentBiasPin().clone());
		newParameter->setInputPinMinus(getInputPinMinus().clone());
		newParameter->setInputPinPlus(getInputPinPlus().clone());

		if(hasOutputPin())
		{
			newParameter->setOutputPin(getOutputPin().clone());
		}
		if(hasOutputPinMinus())
		{
			newParameter->setOutputPinMinus(getOutputPinMinus().clone());
		}
		if(hasOutputPinPlus())
		{
			newParameter->setOutputPinPlus(getOutputPinPlus().clone());
		}
		if(hasReferenceVoltagePin())
		{
			newParameter->setReferenceVoltagePin(getReferenceVoltagePin().clone());
		}

		for(auto & voltageBiasPin : getVoltageBiasPins())
		{
			Pin & newVoltageBiasPin = voltageBiasPin->clone();
			newParameter->addVoltageBiasPin(newVoltageBiasPin);
		}

		return * newParameter;
	}

	CircuitParameter::~CircuitParameter()
	{
		delete supplyVoltagePin_;
		delete groundPin_;
		delete inputPinMinus_;
		delete inputPinPlus_;
		delete currentBiasPin_;
		if(isFullyDifferential())
		{
			delete outputPinPlus_;
			delete outputPinMinus_;
		}
		else
		{
			delete outputPin_;
		}
		if(hasReferenceVoltagePin())
			delete referenceVoltagePin_;
		eraseVoltageBiasPins();
		eraseLoadCapacities();
	}

	void CircuitParameter::addLoadCapacity(
		LoadCapacity & loadCapacity)
	{
		loadCapacities_.push_back(&loadCapacity);
	}


	std::vector<LoadCapacity*> CircuitParameter::getLoadCapacities() const
	{
		assert(hasLoadCapacities());
		return loadCapacities_;
	}

	int CircuitParameter::getLoadCapacity(Core::DeviceId deviceId) const
	{
		assert(hasLoadCapacity(deviceId));
		int value;
		for(auto & loadCap : loadCapacities_)
		{
			if(loadCap->getDeviceId() == deviceId)
			{
				value = loadCap->getValue();
				break;
			}
		}
		return value;
	}


	LoadCapacity &CircuitParameter::getMaxLoadCapacity() const
	{
		assert(hasLoadCapacities());
		LoadCapacity * maxLoadCapacity = nullptr;
		int maxLoadCapacityValue = 0;
		for(auto & loadCapacity : loadCapacities_)
		{
			if(loadCapacity->getValue() > maxLoadCapacityValue)
			{
				maxLoadCapacityValue = loadCapacity->getValue();
				maxLoadCapacity = loadCapacity;
			}
		}
		return *maxLoadCapacity;
	}

	LoadCapacity & CircuitParameter::getMinLoadCapacity() const
	{
		assert(hasLoadCapacities());
		LoadCapacity * minLoadCapacity = nullptr;
		int minLoadCapacityValue = 100000000;
		for(auto & loadCapacity : loadCapacities_)
		{
			if(loadCapacity->getValue() < minLoadCapacityValue)
			{
				minLoadCapacityValue = loadCapacity->getValue();
				minLoadCapacity = loadCapacity;
			}
		}
		return *minLoadCapacity;

	}

	float CircuitParameter::getSupplyVoltage() const
	{
		const Pin & supplyPin = getSupplyVoltagePin();
		return supplyPin.getValue();
	}

	float CircuitParameter::getGroundVoltage() const
	{
		const Pin & groundPin = getGroundPin();
		return groundPin.getValue();
	}

	bool CircuitParameter::hasLoadCapacities() const
	{
		return !loadCapacities_.empty();
	}

	void CircuitParameter::setSupplyVoltagePin(const Pin& pin)
	{
		supplyVoltagePin_  =  & pin;
	}

	void CircuitParameter::setCurrentBiasPin(Pin& pin)
	{
		currentBiasPin_ = & pin;
	}

	void CircuitParameter::setInputPinMinus(const Pin& pin)
	{
		inputPinMinus_ = & pin;
	}

	void CircuitParameter::setInputPinPlus(const Pin& pin)
	{
		inputPinPlus_ = & pin;
	}

	void CircuitParameter::setOutputPin(const Pin& pin)
	{
		outputPin_ = & pin;
	}

	void CircuitParameter::setOutputPinMinus(const Pin& pin)
	{
		outputPinMinus_ = & pin;
	}

	void CircuitParameter::setOutputPinPlus(const Pin& pin)
	{
		outputPinPlus_ = & pin;
	}

	void CircuitParameter::setGroundPin(const Pin& pin)
	{
		groundPin_ = & pin;
	}

	void CircuitParameter::setReferenceVoltagePin(const Pin& pin)
	{
		referenceVoltagePin_ = & pin;
	}

	void CircuitParameter::setFullyDifferential(bool isIt)
	{
		isFullyDifferential_ = isIt;
	}

	void CircuitParameter::setComplementary(bool isIt)
	{
		isComplementary_ = isIt;
	}

	const Pin & CircuitParameter::getSupplyVoltagePin() const
	{
		assert(hasSupplyVoltagePin());
		return *supplyVoltagePin_;
	}

	const Pin & CircuitParameter::getCurrentBiasPin() const
	{
		assert(hasCurrentBiasPin());
		return *currentBiasPin_;
	}

	Pin & CircuitParameter::getCurrentBiasPin()
	{
		assert(hasCurrentBiasPin());
		return *currentBiasPin_;
	}

	const Pin & CircuitParameter::getInputPinMinus() const
	{
		assert(hasInputPinMinus());
		return * inputPinMinus_;
	}

	const Pin & CircuitParameter::getInputPinPlus() const
	{
		assert(hasInputPinPlus());
		return * inputPinPlus_;
	}

	const Pin & CircuitParameter::getOutputPin() const
	{
		assert(hasOutputPin());
		return * outputPin_;
	}

	const Pin & CircuitParameter::getOutputPinPlus() const
	{
		assert(hasOutputPinPlus());
		return * outputPinPlus_;
	}

	const Pin & CircuitParameter::getOutputPinMinus() const
	{
		assert(hasOutputPinMinus());
		return * outputPinMinus_;
	}

	const Pin & CircuitParameter::getGroundPin() const
	{
		assert(hasGroundPin());
		return * groundPin_;
	}

	const Pin & CircuitParameter::getReferenceVoltagePin() const
	{
		assert(hasReferenceVoltagePin());
		return * referenceVoltagePin_;
	}

	bool CircuitParameter::hasSupplyVoltagePin() const
	{
		return supplyVoltagePin_ != NULL;
	}

	bool CircuitParameter::hasCurrentBiasPin() const
	{
		return currentBiasPin_ != NULL;
	}

	bool CircuitParameter::hasInputPinMinus() const
	{
		return inputPinMinus_ != NULL;
	}

	bool CircuitParameter::hasInputPinPlus() const
	{
		return inputPinPlus_ != NULL;
	}

	bool CircuitParameter::hasOutputPin() const
	{
		return  outputPin_ != NULL;
	}

	bool CircuitParameter::hasOutputPinPlus() const
	{
		return  outputPinPlus_ != NULL;
	}

	bool CircuitParameter::hasOutputPinMinus() const
	{
		return  outputPinMinus_ != NULL;
	}

	float CircuitParameter::getValue(Core::NetId netId) const
	{
		const Pin & pin = getCorrespondingPin(netId);
		return pin.getValue();
	}

	bool CircuitParameter::hasGroundPin() const
	{
		return groundPin_ != NULL;
	}

	bool CircuitParameter::hasReferenceVoltagePin() const
	{
		return referenceVoltagePin_ != nullptr;
	}


	bool CircuitParameter::isInputNet(Core::NetId netId) const
	{
		bool isInputNet = false;

		const Pin & inputPinMinus = getInputPinMinus();
		const Pin & inputPinPlus = getInputPinPlus();
		if(inputPinPlus.getNetId() == netId ||  inputPinMinus.getNetId() == netId)
		{
			isInputNet = true;

		}

		return isInputNet;
	}

	bool CircuitParameter::isOutputNet(Core::NetId netId) const
	{
		bool isOutputNet = false;

		if(isFullyDifferential_)
		{
			const Pin & outputPinMinus = getOutputPinMinus();
			const Pin & outputPinPlus = getOutputPinPlus();
			if(outputPinMinus.getNetId() == netId || outputPinPlus.getNetId() == netId)
			{
				isOutputNet = true;

			}
		}
		else
		{
			const Pin & pin = getOutputPin();
			if(pin.getNetId() == netId)
			{
				isOutputNet = true;

			}
		}
		return isOutputNet;
	}

	bool CircuitParameter::isSupplyNet(Core::NetId netId) const
	{
		bool isSupplyNet = false;

		const Pin & pin = getSupplyVoltagePin();
		if(pin.getNetId() == netId)
		{
			isSupplyNet = true;

		}

		return isSupplyNet;
	}

	bool CircuitParameter::isGroundNet(Core::NetId netId) const
	{
		bool isGroundNet = false;

		const Pin & pin = getGroundPin();
		if(pin.getNetId() == netId)
		{
			isGroundNet = true;

		}

		return isGroundNet;
	}

	bool CircuitParameter::isCurrentBiasNet(Core::NetId netId) const
	{
		bool isBiasNet = false;
		if(hasCurrentBiasPin())
		{
			const Pin & pin = getCurrentBiasPin();
			if(pin.getNetId() == netId)
			{
				isBiasNet = true;

			}
		}

		return isBiasNet;
	}

	bool CircuitParameter::isVoltageBiasNet(Core::NetId netId) const
	{
		bool isBiasNet = false;

		std::vector<const Pin *> voltageBiasPins = getVoltageBiasPins();
		for(auto & pin : voltageBiasPins)
		{
			if(pin->getNetId() == netId)
			{
				isBiasNet = true;
				break;
			}
		}
		return isBiasNet;
	}

	bool CircuitParameter::isInputNetMinus(Core::NetId netId) const
	{
		bool isInputNetMinus = false;
		const Pin & pin = getInputPinMinus();
		if(pin.getNetId() == netId)
		{
			isInputNetMinus = true;
		}

		return isInputNetMinus;
	}

	bool CircuitParameter::isInputNetPlus(Core::NetId netId) const
	{
		bool isInputNetPlus = false;
		const Pin & pin = getInputPinPlus();
		if(pin.getNetId() == netId)
		{
			isInputNetPlus = true;
		}

		return isInputNetPlus;
	}

	bool CircuitParameter::isReferenceVoltageNet(Core::NetId netId) const
	{
		bool isReferenceVoltageNet = false;
		const Pin & pin = getReferenceVoltagePin();
		if(pin.getNetId() == netId)
		{
			isReferenceVoltageNet = true;
		}

		return isReferenceVoltageNet;
	}

	void CircuitParameter::addVoltageBiasPin(const Pin& pin)
	{
		voltageBiasPins_.push_back(&pin);
	}

	std::vector<const Pin*> CircuitParameter::getVoltageBiasPins() const
	{
		return voltageBiasPins_;
	}

	const Pin& CircuitParameter::getCorrespondingPin(Core::NetId netId) const
	{
		const Pin * pin = NULL;
		if(isInputNetMinus(netId))
		{
			pin = & getInputPinMinus();
		}
		if(isInputNetPlus(netId))
		{
			pin = & getInputPinPlus();
		}
		else if(isOutputNet(netId))
		{
			if(getOutputPinMinus().getNetId() == netId)
			{
				pin = & getOutputPinMinus();
			}
			else if(getOutputPinPlus().getNetId() == netId)
			{
				pin = & getOutputPinPlus();
			}
			else if(getOutputPin().getNetId() == netId)
			{
				pin = & getOutputPin();
			}
		}
		else if(isGroundNet(netId))
		{
			pin = & getGroundPin();
		}
		else if(isCurrentBiasNet(netId))
		{
			pin = & getCurrentBiasPin();
		}
		else if(isSupplyNet(netId))
		{
			pin = & getSupplyVoltagePin();
		}
		else if(isReferenceVoltageNet(netId))
		{
			pin = & getReferenceVoltagePin();
		}

		assert(pin != NULL, "Net does not have a pin!");
		return  *pin;
	}

	bool CircuitParameter::hasLoadCapacity(Core::DeviceId deviceId) const
	{
		bool hasIt = false;
		for(auto & loadCap : loadCapacities_)
		{
			if(loadCap->getDeviceId() == deviceId)
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	bool CircuitParameter::isFullyDifferential() const
	{
		return isFullyDifferential_;
	}

	bool CircuitParameter::isComplementary()const
	{
		return isComplementary_;
	}

	void CircuitParameter::eraseVoltageBiasPins()
	{
		for(auto & pin : voltageBiasPins_)
		{
			delete pin;
		}
	}

	void CircuitParameter::eraseLoadCapacities()
	{
		for(auto & loadCapacity : loadCapacities_)
		{
			delete loadCapacity;
		}
	}

	const Pin & CircuitParameter::findOutputPinWithBiggestLoadCapacity(
			const Partitioning::Result & result) const
	{
		if(isFullyDifferential())
		{
			Core::DeviceId maxLoadCapacity = getMaxLoadCapacity().getDeviceId();
			Partitioning::Component & loadCapComp = **result.getBelongingComponents(result.getLoadCapacity(maxLoadCapacity)).begin();

			Core::NetId plusPin = loadCapComp.getArray().findNet(StructRec::StructurePinType(loadCapComp.getArray().getStructureName(), "Plus")).getIdentifier();
			Core::NetId minusPin = loadCapComp.getArray().findNet(StructRec::StructurePinType(loadCapComp.getArray().getStructureName(), "Minus")).getIdentifier();
			if(plusPin == getOutputPinMinus().getNetId() || minusPin == getOutputPinMinus().getNetId())
			{
				return getOutputPinMinus();
			}
			else
			{
				assert(plusPin == getOutputPinPlus().getNetId() || minusPin == getOutputPinPlus().getNetId(), "Something went wrong. The load Capacities must be connected to a output pin. The net name of the output pins might not be correct!");
				return getOutputPinPlus();
			}
		}
			else
		{
			return getOutputPin();
		}
	}

}
