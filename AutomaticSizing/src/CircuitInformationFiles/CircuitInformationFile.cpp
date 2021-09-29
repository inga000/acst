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


#include "AutomaticSizing/incl/CircuitInformationFiles/CircuitInformationFile.h"
#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/CircuitInformation/LoadCapacity.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"

#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

	const std::string CircuitInformationFile::CIRCUIT_PARAMETER_NODE_ = "CircuitParameter";
	const std::string CircuitInformationFile::CIRCUIT_SPECIFICATIONS_NODE_ = "Specifications";
	const std::string CircuitInformationFile::LOAD_CAPACITY_NODE_ = "LoadCapacity";
	const std::string CircuitInformationFile::LOAD_CAPACITIES_NODE_ = "LoadCapacities";
	const std::string CircuitInformationFile::SUPPLY_VOLTAGE_NODE_ = "SupplyVoltage";
	const std::string CircuitInformationFile::GROUND_VOLTAGE_NODE_ = "GroundVoltage";
	const std::string CircuitInformationFile::GROUND_PIN_NODE_ = "GroundPin";
	const std::string CircuitInformationFile::GROUND_VOLTAGE_ATTRIBUTE_ = "Gnd";
	const std::string CircuitInformationFile::BIAS_CURRENT_NODE_ = "BiasCurrent";
	const std::string CircuitInformationFile::BIAS_VOLTAGE_NODE_ = "BiasVoltage";
	const std::string CircuitInformationFile::INPUT_VOLTAGE_NODE_ = "InputVoltage";
	const std::string CircuitInformationFile::SUPPLY_VOLTAGE_PIN_NODE_ = "SupplyVoltagePin";
	const std::string CircuitInformationFile::CURRENT_BIAS_PIN_NODE_ = "CurrentBiasPin";
	const std::string CircuitInformationFile::VOLTAGE_BIAS_PIN_NODE_ = "VoltageBiasPin";
	const std::string CircuitInformationFile::VOLTAGE_BIAS_PINS_NODE_ = "VoltageBiasPins";
	const std::string CircuitInformationFile::INPUT_PIN_MINUS_NODE_ = "InputPinMinus";
	const std::string CircuitInformationFile::INPUT_PIN_PLUS_NODE_ = "InputPinPlus";
	const std::string CircuitInformationFile::OUTPUT_PIN_MINUS_NODE_ = "OutputPinMinus";
	const std::string CircuitInformationFile::OUTPUT_PIN_PLUS_NODE_ = "OutputPinPlus";
	const std::string CircuitInformationFile::OUTPUT_PIN_NODE_ = "OutputPin";
	const std::string CircuitInformationFile::REFERENCEVOLTAGE_PIN_NODE_ = "ReferenceVoltagePin";
	const std::string CircuitInformationFile::REFERENCE_VOLTAGE_NODE_ = "ReferenceVoltage";
	const std::string CircuitInformationFile::REFERENCE_VOLTAGE_ATTRIBUTE_ = "Vref";
	const std::string CircuitInformationFile::SUPPLY_VOLTAGE_ATTRIBUTE_ = "Vdd";
	const std::string CircuitInformationFile::BIAS_CURRENT_ATTRIBUTE_ = "Ibias";
	const std::string CircuitInformationFile::BIAS_VOLTAGE_ATTRIBUTE_ = "Vbias";
	const std::string CircuitInformationFile::INPUT_VOLTAGE_ATTRIBUTE_ = "Vin";
	const std::string CircuitInformationFile::VALUE_NODE_ = "Value";
	const std::string CircuitInformationFile::NET_NAME_NODE_ = "NetName";
	const std::string CircuitInformationFile::DEVICE_NAME_NODE_ = "DeviceName";

	const std::string CircuitInformationFile::GAIN_NODE_ = "minimumGain";
	const std::string CircuitInformationFile::TRANSIENT_FREQUENCY_NODE_ = "minimumTransientFrequency";
	const std::string CircuitInformationFile::SLEW_RATE_NODE_ = "maximumSlewRate";
	const std::string CircuitInformationFile::CMRR_NODE_ = "minimumCMRR";
	const std::string CircuitInformationFile::NEG_PSRR_NODE_ = "minimumNegPSRR";
	const std::string CircuitInformationFile::POS_PSRR_NODE_ = "minimumPosPSRR";

	const std::string CircuitInformationFile::OUTPUT_VOLTAGE_NODE_ = "OutputVoltageSwing";
	const std::string CircuitInformationFile::POWER_CONSUMPTION_NODE_ = "maximumPowerConsumption";
	const std::string CircuitInformationFile::POWER_CONSUMPTION_ATTRIBUTE_ = "P";
	const std::string CircuitInformationFile::GAIN_ATTRIBUTE_ = "A";
	const std::string CircuitInformationFile::TRANSIENT_FREQUENCY_ATTRIBUTE_ = "ft";
	const std::string CircuitInformationFile::SLEW_RATE_ATTRIBUTE_ = "SR";
	const std::string CircuitInformationFile::CMRR_ATTRIBUTE_ = "CMRR";
	const std::string CircuitInformationFile::NEG_PSRR_ATTRIBUTE_ = "negPSRR";
	const std::string CircuitInformationFile::POS_PSRR_ATTRIBUTE_ = "posPSRR";
	const std::string CircuitInformationFile::COMMON_MODE_INPUT_VOLTAGE_NODE_ = "CommonModeInputVoltage";
	const std::string CircuitInformationFile::MAX_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_ = "Vcmmax";
	const std::string CircuitInformationFile::MIN_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_ = "Vcmmin";
	const std::string CircuitInformationFile::MAX_OUTPUT_VOLTAGE_ATTRIBUTE_ = "Voutmax";
	const std::string CircuitInformationFile::MIN_OUTPUT_VOLTAGE_ATTRIBUTE_ = "Voutmin";
	const std::string CircuitInformationFile::AREA_ATTRIBUTE_ = "Area";
	const std::string CircuitInformationFile::AREA_NODE_ ="maximumArea";
	const std::string CircuitInformationFile::GATE_OVER_DRIVE_VOLTAGE_NODE_ = "GateOverDriveVoltage";
	const std::string CircuitInformationFile::GATE_OVER_DRIVE_VOLTAGE_ATTRIBUTE_ = "Vover";
	const std::string CircuitInformationFile::PHASE_MARGIN_NODE_ = "phaseMargin";
	const std::string CircuitInformationFile::PHASE_MARGIN_ATTRIBUTE_ = "PM";


	//DesignAttributs
	const std::string CircuitInformationFile::DESIGN_ATTRIBUTES_NODE_ = "DesignAttributes";
	const std::string CircuitInformationFile::TRANSCONDUCTANCE_IN_WEAK_INVERSION_NODE_ = "TransconductanceInWeakInversion";
	const std::string CircuitInformationFile::ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_ = "AllTransistorsInWeakInversion";
	const std::string CircuitInformationFile::WEAK_INVERSION_NODE_ = "WeakInversion";
	const std::string CircuitInformationFile::MODERATE_INVERSION_NODE_ = "ModerateInversion";



	CircuitInformationFile::CircuitInformationFile() :
			circuitParameter_(NULL)
	{

	}

	void CircuitInformationFile::parse(CircuitInformation & circuitInformation)
	{
		logDebug("Start Parsing circuit Informations");
		try
		{
			Control::XmlInputFile::parse();

			circuitInformation.setCircuitParameter(parseCircuitParameter());
			circuitInformation.setCircuitSpecification(parseCircuitSpecification());
			circuitInformation.setDesignAttributes(parseDesignAttributes());

		}
		catch(const ::Core::BacktraceException& ex)
		{
			logDebug(ex.what());
		}
		catch(...)
		{
			logDebug("Error parsing circuit information  file!");
		}

	}

	CircuitParameter & CircuitInformationFile::parseCircuitParameter()
	{
		rapidxml::xml_node<>* parameterNode = getDocumentNode().first_node(CIRCUIT_PARAMETER_NODE_.c_str());
		CircuitParameter * parameter = new CircuitParameter;
		circuitParameter_ = parameter;

		rapidxml::xml_node<>* loadCapacitiesNode = parameterNode->first_node(LOAD_CAPACITIES_NODE_.c_str());
		parseLoadCapacities(*loadCapacitiesNode);

		rapidxml::xml_node<>* supplyVoltagePinNode = parameterNode->first_node(SUPPLY_VOLTAGE_PIN_NODE_.c_str());
		parseSupplyVoltagePin(*supplyVoltagePinNode);


		rapidxml::xml_node<>* currentBiasPinNode = parameterNode->first_node(CURRENT_BIAS_PIN_NODE_.c_str());
		parseCurrentBiasPin(*currentBiasPinNode);

		rapidxml::xml_node<>* voltageBiasPinsNode = parameterNode->first_node(VOLTAGE_BIAS_PINS_NODE_.c_str());
		if(voltageBiasPinsNode != nullptr)
		{
			parseVoltageBiasPins(*voltageBiasPinsNode);
		}

		rapidxml::xml_node<>* inputPinNodeMinus = parameterNode->first_node(INPUT_PIN_MINUS_NODE_.c_str());
		parseInputPinMinus(*inputPinNodeMinus);

		rapidxml::xml_node<>* inputPinNodePlus = parameterNode->first_node(INPUT_PIN_PLUS_NODE_.c_str());
		parseInputPinPlus(*inputPinNodePlus);


		parseOutputPins(*parameterNode);

		rapidxml::xml_node<>* groundPinNode = parameterNode->first_node(GROUND_PIN_NODE_.c_str());
		parseGroundPin(*groundPinNode);

		rapidxml::xml_node<>* referenceVoltagePinNode = parameterNode->first_node(REFERENCEVOLTAGE_PIN_NODE_.c_str());
		if(referenceVoltagePinNode != nullptr)
		{static const std::string ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_;
			parseReferenceVoltagePin(*referenceVoltagePinNode);
		}

		return * parameter;

	}

	DesignAttributes & CircuitInformationFile::parseDesignAttributes()
	{
		DesignAttributes * attributes = new DesignAttributes;

		rapidxml::xml_node<>* attributesNode = getDocumentNode().first_node(DESIGN_ATTRIBUTES_NODE_.c_str());
		if(attributesNode != nullptr)
		{
			rapidxml::xml_node<>* transconductanceNode = attributesNode->first_node(TRANSCONDUCTANCE_IN_WEAK_INVERSION_NODE_.c_str());
			if(transconductanceNode != nullptr)
			{
				bool isWeakInversion;
				std::istringstream(transconductanceNode->value()) >> std::boolalpha >> isWeakInversion;
				attributes->setTransconductancesInWeakInversion(isWeakInversion);
			}

			rapidxml::xml_node<>* allTransistorsInWeakInversionNode = attributesNode->first_node(ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_.c_str());
			if(allTransistorsInWeakInversionNode != nullptr)
			{
				bool isWeakInversion;
				std::istringstream(allTransistorsInWeakInversionNode->value()) >> std::boolalpha >> isWeakInversion;
				attributes->setAllTransistorsInWeakInversion(isWeakInversion);
			}

			rapidxml::xml_node<>* moderateInversionNode = attributesNode->first_node(MODERATE_INVERSION_NODE_.c_str());
			if(moderateInversionNode != nullptr)
			{
				rapidxml::xml_node<>* deviceNode = moderateInversionNode->first_node(DEVICE_NAME_NODE_.c_str());
				while(deviceNode != nullptr)
				{
					Core::DeviceId deviceId = parseDeviceId(*deviceNode);
					attributes->addModerateInversionTransistor(deviceId);
					deviceNode = deviceNode->next_sibling(DEVICE_NAME_NODE_.c_str());
				}

			}

			rapidxml::xml_node<>* weakInversionNode = attributesNode->first_node(WEAK_INVERSION_NODE_.c_str());
			if(weakInversionNode != nullptr)
			{
				rapidxml::xml_node<>* deviceNode = weakInversionNode->first_node(DEVICE_NAME_NODE_.c_str());
				while(deviceNode != nullptr)
				{
					Core::DeviceId deviceId = parseDeviceId(*deviceNode);
					attributes->addWeakInversionTransistor(deviceId);
					deviceNode = deviceNode->next_sibling(DEVICE_NAME_NODE_.c_str());
				}
			}
		}



		return * attributes;
	}

	CircuitSpecification & CircuitInformationFile::parseCircuitSpecification()
	{
		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());
		CircuitSpecification * specification = new CircuitSpecification;

		rapidxml::xml_node<>* gainNode = specificationNode->first_node(GAIN_NODE_.c_str());
		rapidxml::xml_attribute<>* gainAttribute = gainNode->first_attribute(GAIN_ATTRIBUTE_.c_str());
		specification->setGain(std::atof(gainAttribute->value()));

		rapidxml::xml_node<>* transientFrequencyNode = specificationNode->first_node(TRANSIENT_FREQUENCY_NODE_.c_str());
		rapidxml::xml_attribute<>* frequencyAttribute = transientFrequencyNode->first_attribute(TRANSIENT_FREQUENCY_ATTRIBUTE_.c_str());
		float temp = std::atof(frequencyAttribute->value());
		specification->setTransientFrequency(temp * 1000000);

		rapidxml::xml_node<>* slewRateNode = specificationNode->first_node(SLEW_RATE_NODE_.c_str());
		rapidxml::xml_attribute<>* slewRateAttribute = slewRateNode->first_attribute(SLEW_RATE_ATTRIBUTE_.c_str());
		float temp1 = std::atof(slewRateAttribute->value());
		specification->setSlewRate(temp1 * 1000000);

		rapidxml::xml_node<>* cMRRNode = specificationNode->first_node(CMRR_NODE_.c_str());
		rapidxml::xml_attribute<>* cMRRAttribute = cMRRNode->first_attribute(CMRR_ATTRIBUTE_.c_str());
		specification->setCMRR(std::atof(cMRRAttribute->value()));

		rapidxml::xml_node<>* negPSRRNode = specificationNode->first_node(NEG_PSRR_NODE_.c_str());
		rapidxml::xml_attribute<>* negPSRRAttribute = negPSRRNode->first_attribute(NEG_PSRR_ATTRIBUTE_.c_str());
		specification->setNegPSRR(std::atof(negPSRRAttribute->value()));

		rapidxml::xml_node<>* posPSRRNode = specificationNode->first_node(POS_PSRR_NODE_.c_str());
		rapidxml::xml_attribute<>* posPSRRAttribute = posPSRRNode->first_attribute(POS_PSRR_ATTRIBUTE_.c_str());
		specification->setPosPSRR(std::atof(posPSRRAttribute->value()));


		rapidxml::xml_node<>* outputVoltageSwingNode = specificationNode->first_node(OUTPUT_VOLTAGE_NODE_.c_str());
		rapidxml::xml_attribute<>* maxSwingAttribute = outputVoltageSwingNode->first_attribute(MAX_OUTPUT_VOLTAGE_ATTRIBUTE_.c_str());
		specification->setMaxOutputVoltage(std::atof(maxSwingAttribute->value()));
		rapidxml::xml_attribute<>* minSwingAttribute = outputVoltageSwingNode->first_attribute(MIN_OUTPUT_VOLTAGE_ATTRIBUTE_.c_str());
		specification->setMinOutputVoltage(std::atof(minSwingAttribute->value()));

		rapidxml::xml_node<>* powerNode = specificationNode->first_node(POWER_CONSUMPTION_NODE_.c_str());
		rapidxml::xml_attribute<>* powerAttribute = powerNode->first_attribute(POWER_CONSUMPTION_ATTRIBUTE_.c_str());
		float temp3 = std::atof(powerAttribute->value());
		specification->setPowerConsumption(temp3 /1000);

		rapidxml::xml_node<>* areaNode = specificationNode->first_node(AREA_NODE_.c_str());
		rapidxml::xml_attribute<>* areaAttribute = areaNode->first_attribute(AREA_ATTRIBUTE_.c_str());
		specification->setMaxArea(std::atof(areaAttribute->value()));


		rapidxml::xml_node<>* gateOverDriveNode = specificationNode->first_node(GATE_OVER_DRIVE_VOLTAGE_NODE_.c_str());
		rapidxml::xml_attribute<>* gateOverDriveAttribute = gateOverDriveNode->first_attribute(GATE_OVER_DRIVE_VOLTAGE_ATTRIBUTE_.c_str());
		specification->setGateOverDriveVoltage(std::atof(gateOverDriveAttribute->value()));

		rapidxml::xml_node<>* commonModeInputVoltageNode = specificationNode->first_node(COMMON_MODE_INPUT_VOLTAGE_NODE_.c_str());
		rapidxml::xml_attribute<>* maxCommonModeInputVoltageAttribute = commonModeInputVoltageNode->first_attribute(MAX_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_.c_str());
		specification->setMaxCommonModeInputVoltage(std::atof(maxCommonModeInputVoltageAttribute->value()));
		rapidxml::xml_attribute<>* minCommonModeInputVoltageAttribute = commonModeInputVoltageNode->first_attribute(MIN_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_.c_str());
		specification->setMinCommonModeInputVoltage(std::atof(minCommonModeInputVoltageAttribute->value()));

		rapidxml::xml_node<>* phaseMarginNode = specificationNode->first_node(PHASE_MARGIN_NODE_.c_str());
		rapidxml::xml_attribute<>* phaseMarginAttribute = phaseMarginNode->first_attribute(PHASE_MARGIN_ATTRIBUTE_.c_str());
		float temp5 = std::atof(phaseMarginAttribute->value());
		specification->setPhaseMargin(temp5 * 3.141582654/ (180));

		return * specification;

	}

	void CircuitInformationFile::parseLoadCapacities(rapidxml::xml_node<> & loadCapacitiesNode)
	{
		rapidxml::xml_node<>* loadCapacityNode = loadCapacitiesNode.first_node(LOAD_CAPACITY_NODE_.c_str());
		while(loadCapacityNode != NULL) {
			parseLoadCapacity(*loadCapacityNode);
			loadCapacityNode = loadCapacityNode->next_sibling(LOAD_CAPACITY_NODE_.c_str());
		}

	}

	void CircuitInformationFile::parseLoadCapacity(rapidxml::xml_node<> & loadCapacityNode)
	{
		rapidxml::xml_node<>* value = loadCapacityNode.first_node(VALUE_NODE_.c_str());
		LoadCapacity * loadCapacity = new LoadCapacity;
		loadCapacity->setValue(std::atoi(value->value()));

		rapidxml::xml_node<>* deviceNameNode = loadCapacityNode.first_node(DEVICE_NAME_NODE_.c_str());
		loadCapacity->setDeviceId(parseDeviceId(*deviceNameNode));

		getCircuitParameter().addLoadCapacity(*loadCapacity);
	}

	void CircuitInformationFile::parseSupplyVoltagePin(
		rapidxml::xml_node<>& supplyVoltagePinNode)
	{
		rapidxml::xml_node<>* supplyVoltageNode = supplyVoltagePinNode.first_node(SUPPLY_VOLTAGE_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = supplyVoltagePinNode.first_node(NET_NAME_NODE_.c_str());

		rapidxml::xml_attribute<>* supplyVoltageAttribute = supplyVoltageNode->first_attribute(SUPPLY_VOLTAGE_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		pin->setValue(std::atof(supplyVoltageAttribute->value()));
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setSupplyVoltagePin(*pin);

	}

	void CircuitInformationFile::parseCurrentBiasPin(rapidxml::xml_node<>& biasPinNode)
	{
		rapidxml::xml_node<>* biasCurrentNode = biasPinNode.first_node(BIAS_CURRENT_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = biasPinNode.first_node(NET_NAME_NODE_.c_str());

		rapidxml::xml_attribute<>* biasCurrentAttribute = biasCurrentNode->first_attribute(BIAS_CURRENT_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		float temp = std::atof(biasCurrentAttribute->value());
		pin->setValue(temp/1000000);
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setCurrentBiasPin(*pin);

	}

	void CircuitInformationFile:: parseVoltageBiasPins(rapidxml::xml_node<> & voltageBiasPinsNode)
	{
		Core::XmlNode* voltageBiasPinNode = voltageBiasPinsNode.first_node(VOLTAGE_BIAS_PIN_NODE_.c_str());
		while(voltageBiasPinNode != NULL) {

			rapidxml::xml_node<>* biasVoltageNode = voltageBiasPinNode->first_node(BIAS_VOLTAGE_NODE_.c_str());

			rapidxml::xml_node<>* netIdNode = voltageBiasPinNode->first_node(NET_NAME_NODE_.c_str());

			rapidxml::xml_attribute<>* biasVoltageAttribute = biasVoltageNode->first_attribute(BIAS_VOLTAGE_ATTRIBUTE_.c_str());
			Pin * pin = new Pin;

			pin->setValue(std::atof(biasVoltageAttribute->value()));
			pin->setNetId(parseNetId(*netIdNode));
			getCircuitParameter().addVoltageBiasPin(*pin);
			voltageBiasPinNode = voltageBiasPinNode->next_sibling(VOLTAGE_BIAS_PIN_NODE_.c_str());
		}

	}


	void CircuitInformationFile::parseInputPinMinus(
		rapidxml::xml_node<>& inputPinMinusNode)
	{
		rapidxml::xml_node<>* inputVoltageNode = inputPinMinusNode.first_node(INPUT_VOLTAGE_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = inputPinMinusNode.first_node(NET_NAME_NODE_.c_str());
		rapidxml::xml_attribute<>* inputVoltageAttribute = inputVoltageNode->first_attribute(INPUT_VOLTAGE_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		pin->setValue(std::atof(inputVoltageAttribute->value()));
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setInputPinMinus(*pin);

	}

	void CircuitInformationFile::parseInputPinPlus(
		rapidxml::xml_node<>& inputPinPlusNode)
	{
		rapidxml::xml_node<>* inputVoltageNode = inputPinPlusNode.first_node(INPUT_VOLTAGE_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = inputPinPlusNode.first_node(NET_NAME_NODE_.c_str());
		rapidxml::xml_attribute<>* inputVoltageAttribute = inputVoltageNode->first_attribute(INPUT_VOLTAGE_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		pin->setValue(std::atof(inputVoltageAttribute->value()));
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setInputPinPlus(*pin);

	}

	void CircuitInformationFile::parseOutputPins(
		rapidxml::xml_node<>& parameterNode)
	{
		rapidxml::xml_node<>* outputPinNode = parameterNode.first_node(OUTPUT_PIN_NODE_.c_str());
		rapidxml::xml_node<>* outputPinMinusNode = parameterNode.first_node(OUTPUT_PIN_MINUS_NODE_.c_str());
		rapidxml::xml_node<>* outputPinPlusNode = parameterNode.first_node(OUTPUT_PIN_PLUS_NODE_.c_str());
		assert(((outputPinNode != NULL) || (outputPinMinusNode != NULL && outputPinPlusNode != NULL))
				&& (!(outputPinNode != NULL) || !(outputPinMinusNode != NULL && outputPinPlusNode != NULL)), "Either the op amp has only one output pin or it has a negative and a positive output pin!");

		if(outputPinNode != NULL)
		{
			rapidxml::xml_node<>* netIdNode = outputPinNode->first_node(NET_NAME_NODE_.c_str());
			Pin * pin = new Pin;
			pin->setNetId(parseNetId(*netIdNode));
			getCircuitParameter().setOutputPin(*pin);

			getCircuitParameter().setFullyDifferential(false);
		}
		else
		{
			rapidxml::xml_node<>* netIdMinusNode = outputPinMinusNode->first_node(NET_NAME_NODE_.c_str());
			Pin * pinMinus = new Pin;
			pinMinus->setNetId(parseNetId(*netIdMinusNode));
			getCircuitParameter().setOutputPinMinus(*pinMinus);

			rapidxml::xml_node<>* netIdPlusNode = outputPinPlusNode->first_node(NET_NAME_NODE_.c_str());
			Pin * pinPlus = new Pin;
			pinPlus->setNetId(parseNetId(*netIdPlusNode));
			getCircuitParameter().setOutputPinPlus(*pinPlus);

			getCircuitParameter().setFullyDifferential(true);
		}


	}


	void CircuitInformationFile::parseGroundPin(
		rapidxml::xml_node<>& groundPinNode)
	{

		rapidxml::xml_node<>* groundVoltageNode = groundPinNode.first_node(GROUND_VOLTAGE_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = groundPinNode.first_node(NET_NAME_NODE_.c_str());

		rapidxml::xml_attribute<>* groundVoltageAttribute = groundVoltageNode->first_attribute(GROUND_VOLTAGE_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		pin->setValue(std::atof(groundVoltageAttribute->value()));
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setGroundPin(*pin);


	}

	void CircuitInformationFile::parseReferenceVoltagePin(
		rapidxml::xml_node<>& referenceVoltagePinNode)
	{

		rapidxml::xml_node<>* referenceVoltageNode = referenceVoltagePinNode.first_node(REFERENCE_VOLTAGE_NODE_.c_str());

		rapidxml::xml_node<>* netIdNode = referenceVoltagePinNode.first_node(NET_NAME_NODE_.c_str());

		rapidxml::xml_attribute<>* referenceVoltageAttribute = referenceVoltageNode->first_attribute(REFERENCE_VOLTAGE_ATTRIBUTE_.c_str());
		Pin * pin = new Pin;
		pin->setValue(std::atof(referenceVoltageAttribute->value()));
		pin->setNetId(parseNetId(*netIdNode));
		getCircuitParameter().setReferenceVoltagePin(*pin);


	}

	void CircuitInformationFile::setCircuitParameter(CircuitParameter& parameter)
	{
		circuitParameter_ = & parameter;
	}

	CircuitParameter& CircuitInformationFile::getCircuitParameter()
	{
		assert(circuitParameter_ != NULL);
		return * circuitParameter_;
	}

	Core::NetId CircuitInformationFile::parseNetId(rapidxml::xml_node<> & netIdNode)
	{
		Core::NetName netName(netIdNode.value());
		return netName.createRootIdentifier();
	}

	Core::DeviceId CircuitInformationFile::parseDeviceId(rapidxml::xml_node<> & deviceIdNode)
	{
		Core::DeviceName deviceName(deviceIdNode.value());
		return deviceName.createRootIdentifier();
	}
}


