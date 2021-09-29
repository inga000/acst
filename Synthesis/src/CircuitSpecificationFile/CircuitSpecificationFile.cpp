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

#include "Synthesis/incl/CircuitSpecificationFile/CircuitSpecificationFile.h"


#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/CircuitInformation/LoadCapacity.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"

#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {

	const std::string CircuitSpecificationFile::CIRCUIT_SPECIFICATIONS_NODE_ = "Specifications";
	const std::string CircuitSpecificationFile::LOAD_CAPACITY_NODE_ = "LoadCapacity";
	const std::string CircuitSpecificationFile::LOAD_CAPACITY_ATTRIBUTE_ = "Cl";
	const std::string CircuitSpecificationFile::SUPPLY_VOLTAGE_NODE_ = "SupplyVoltage";
	const std::string CircuitSpecificationFile::GROUND_VOLTAGE_NODE_ = "GroundVoltage";
	const std::string CircuitSpecificationFile::GROUND_VOLTAGE_ATTRIBUTE_ = "Gnd";
	const std::string CircuitSpecificationFile::BIAS_CURRENT_NODE_ = "BiasCurrent";
	const std::string CircuitSpecificationFile::BIAS_CURRENT_ATTRIBUTE_ = "Ibias";
	const std::string CircuitSpecificationFile::INPUT_VOLTAGE_NODE_ = "InputVoltage";
	const std::string CircuitSpecificationFile::REFERENCE_VOLTAGE_NODE_ = "ReferenceVoltage";
	const std::string CircuitSpecificationFile::REFERENCE_VOLTAGE_ATTRIBUTE_ = "Vref";
	const std::string CircuitSpecificationFile::SUPPLY_VOLTAGE_ATTRIBUTE_ = "Vdd";
	const std::string CircuitSpecificationFile::INPUT_VOLTAGE_ATTRIBUTE_ = "Vin";

	const std::string CircuitSpecificationFile::GAIN_NODE_ = "minimumGain";
	const std::string CircuitSpecificationFile::TRANSIENT_FREQUENCY_NODE_ = "minimumTransientFrequency";
	const std::string CircuitSpecificationFile::SLEW_RATE_NODE_ = "maximumSlewRate";
	const std::string CircuitSpecificationFile::CMRR_NODE_ = "minimumCMRR";
	const std::string CircuitSpecificationFile::NEG_PSRR_NODE_ = "minimumNegPSRR";
	const std::string CircuitSpecificationFile::POS_PSRR_NODE_ = "minimumPosPSRR";
	const std::string CircuitSpecificationFile::OUTPUT_VOLTAGE_SWING_NODE_ = "OutputVoltageSwing";
	const std::string CircuitSpecificationFile::OFFSET_ERROR_NODE_ = "OffsetError";
	const std::string CircuitSpecificationFile::POWER_CONSUMPTION_NODE_ = "maximumPowerConsumption";
	const std::string CircuitSpecificationFile::POWER_CONSUMPTION_ATTRIBUTE_ = "P";
	const std::string CircuitSpecificationFile::GAIN_ATTRIBUTE_ = "A";
	const std::string CircuitSpecificationFile::TRANSIENT_FREQUENCY_ATTRIBUTE_ = "ft";
	const std::string CircuitSpecificationFile::SLEW_RATE_ATTRIBUTE_ = "SR";
	const std::string CircuitSpecificationFile::CMRR_ATTRIBUTE_ = "CMRR";
	const std::string CircuitSpecificationFile::NEG_PSRR_ATTRIBUTE_ = "negPSRR";
	const std::string CircuitSpecificationFile::POS_PSRR_ATTRIBUTE_ = "posPSRR";
	const std::string CircuitSpecificationFile::COMMON_MODE_INPUT_VOLTAGE_NODE_ = "CommonModeInputVoltage";
	const std::string CircuitSpecificationFile::MAX_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_ = "Vcmmax";
	const std::string CircuitSpecificationFile::MIN_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_ = "Vcmmin";
	const std::string CircuitSpecificationFile::MAX_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_ = "Voutmax";
	const std::string CircuitSpecificationFile::MAX_OFFSET_ERROR_ATTRIBUTE_ = "Vmax";
	const std::string CircuitSpecificationFile::MIN_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_ = "Voutmin";
	const std::string CircuitSpecificationFile::MIN_OFFSET_ERROR_ATTRIBUTE_ = "Vmin";
	const std::string CircuitSpecificationFile::AREA_ATTRIBUTE_ = "Area";
	const std::string CircuitSpecificationFile::AREA_NODE_ ="maximumArea";
	const std::string CircuitSpecificationFile::GATE_OVER_DRIVE_VOLTAGE_NODE_ = "GateOverDriveVoltage";
	const std::string CircuitSpecificationFile::GATE_OVER_DRIVE_VOLTAGE_ATTRIBUTE_ = "Vover";
	const std::string CircuitSpecificationFile::SETTLING_TIME_NODE_  = "settlingTime";
	const std::string CircuitSpecificationFile::SETTLING_TIME_ATTRIBUTE_="ts";
	const std::string CircuitSpecificationFile::PHASE_MARGIN_NODE_ = "phaseMargin";
	const std::string CircuitSpecificationFile::PHASE_MARGIN_ATTRIBUTE_ = "PM";
	const std::string CircuitSpecificationFile::COMPLEMENTARY_NODE_ = "complementary";
	const std::string CircuitSpecificationFile::FULLYDIFFERENTIAL_NODE_ = "fullyDifferential";

	//DesignAttributs
	const std::string CircuitSpecificationFile::DESIGN_ATTRIBUTES_NODE_ = "DesignAttributes";
	const std::string CircuitSpecificationFile::TRANSCONDUCTANCE_IN_WEAK_INVERSION_NODE_ = "TransconductanceInWeakInversion";
	const std::string CircuitSpecificationFile::ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_ = "AllTransistorsInWeakInversion";
	const std::string CircuitSpecificationFile::WEAK_INVERSION_NODE_ = "WeakInversion";
	const std::string CircuitSpecificationFile::MODERATE_INVERSION_NODE_ = "ModerateInversion";



	CircuitSpecificationFile::CircuitSpecificationFile() :
			circuitParameter_(NULL),
			useHSpiceLibrary_(false)
	{
	}

	void CircuitSpecificationFile::parse(AutomaticSizing::CircuitInformation & circuitInformation)
	{
		logDebug("Start Parsing circuit Informations");
		try
		{
			Control::XmlInputFile::parse();

			AutomaticSizing::CircuitParameter * parameter = new AutomaticSizing::CircuitParameter;
			circuitParameter_ = parameter;

			parseCircuitAttributes();
			parseCircuitVoltages();
			parseLoadCapacity();
			parseBiasCurrent();

			if(!useHSpiceLibrary())
			{
				createOutputPins();
			}



			circuitInformation.setCircuitParameter(getCircuitParameter());
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

	void CircuitSpecificationFile::parseCircuitVoltages()
	{
		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());

		rapidxml::xml_node<>* supplyVoltageNode = specificationNode->first_node(SUPPLY_VOLTAGE_NODE_.c_str());
		parseSupplyVoltage(*supplyVoltageNode);

		rapidxml::xml_node<>* groundVoltageNode = specificationNode->first_node(GROUND_VOLTAGE_NODE_.c_str());
		parseGroundVoltage(*groundVoltageNode);

		rapidxml::xml_node<>* inputVoltageNode = specificationNode->first_node(INPUT_VOLTAGE_NODE_.c_str());
		parseInputVoltage(*inputVoltageNode);

		rapidxml::xml_node<>* referenceVoltageNode =specificationNode->first_node(REFERENCE_VOLTAGE_NODE_.c_str());
		if(referenceVoltageNode != nullptr)
		{
			parseReferenceVoltage(*referenceVoltageNode);
		}



	}



	AutomaticSizing::DesignAttributes & CircuitSpecificationFile::parseDesignAttributes()
	{
		AutomaticSizing::DesignAttributes * attributes = new AutomaticSizing::DesignAttributes;

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
		}

		return * attributes;
	}

	AutomaticSizing::CircuitSpecification & CircuitSpecificationFile::parseCircuitSpecification()
	{
		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());
		AutomaticSizing::CircuitSpecification * specification = new AutomaticSizing::CircuitSpecification;

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


		rapidxml::xml_node<>* outputVoltageSwingNode = specificationNode->first_node(OUTPUT_VOLTAGE_SWING_NODE_.c_str());
		rapidxml::xml_attribute<>* maxSwingAttribute = outputVoltageSwingNode->first_attribute(MAX_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_.c_str());
		specification->setMaxOutputVoltage(std::atof(maxSwingAttribute->value()));
		rapidxml::xml_attribute<>* minSwingAttribute = outputVoltageSwingNode->first_attribute(MIN_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_.c_str());
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


	void CircuitSpecificationFile::parseLoadCapacity()
	{
		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());
		rapidxml::xml_node<>* loadCapacityNode = specificationNode->first_node(LOAD_CAPACITY_NODE_.c_str());
		rapidxml::xml_attribute<>* value = loadCapacityNode->first_attribute(LOAD_CAPACITY_ATTRIBUTE_.c_str());


		if(useHSpiceLibrary())
		{
			AutomaticSizing::LoadCapacity * loadCapacity = new AutomaticSizing::LoadCapacity;
			loadCapacity->setValue(std::atoi(value->value()));
			loadCapacity->setDeviceId(createCapacitorId(Core::DeviceName("cl")));
			getCircuitParameter().addLoadCapacity(*loadCapacity);
		}
		else
		{
			if(getCircuitParameter().isFullyDifferential())
			{
				AutomaticSizing::LoadCapacity * loadCapacity1 = new AutomaticSizing::LoadCapacity;
				loadCapacity1->setValue(std::atoi(value->value()));
				loadCapacity1->setDeviceId(createCapacitorId(Core::DeviceName("Capacitor"), "LoadCapacitor1"));
				getCircuitParameter().addLoadCapacity(*loadCapacity1);

				AutomaticSizing::LoadCapacity * loadCapacity2 = new AutomaticSizing::LoadCapacity;
				loadCapacity2->setValue(std::atoi(value->value()));
				loadCapacity2->setDeviceId(createCapacitorId(Core::DeviceName("Capacitor"), "LoadCapacitor2"));
				getCircuitParameter().addLoadCapacity(*loadCapacity2);
			}
			else
			{
				AutomaticSizing::LoadCapacity * loadCapacity = new AutomaticSizing::LoadCapacity;
				loadCapacity->setValue(std::atoi(value->value()));
				loadCapacity->setDeviceId(createCapacitorId(Core::DeviceName("Capacitor"), "LoadCapacitor"));
				getCircuitParameter().addLoadCapacity(*loadCapacity);
			}

		}

	}

	void CircuitSpecificationFile::parseBiasCurrent()
	{
		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());
		rapidxml::xml_node<>* biasCurrentNode = specificationNode->first_node(BIAS_CURRENT_NODE_.c_str());
		rapidxml::xml_attribute<>* biasCurrentAttribute = biasCurrentNode->first_attribute(BIAS_CURRENT_ATTRIBUTE_.c_str());
		AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
		float biasCurrent = std::atof(biasCurrentAttribute->value()) *pow(10,-6);
		pin->setValue(biasCurrent);
		if(useHSpiceLibrary())
		{
			pin->setNetId(createNetId("vbBias"));
		}
		else
		{
			pin->setNetId(createNetId("ibias"));
		}
		getCircuitParameter().setCurrentBiasPin(*pin);
	}

	void CircuitSpecificationFile::parseCircuitAttributes()
	{


		rapidxml::xml_node<>* specificationNode = getDocumentNode().first_node(CIRCUIT_SPECIFICATIONS_NODE_.c_str());
		rapidxml::xml_node<>* fullyDifferentialNode = specificationNode->first_node(FULLYDIFFERENTIAL_NODE_.c_str());
		rapidxml::xml_node<>* complementaryNode = specificationNode->first_node(COMPLEMENTARY_NODE_.c_str());

		if(fullyDifferentialNode != nullptr && !useHSpiceLibrary())
		{
			std::string nodeValue = fullyDifferentialNode->value();
			if(nodeValue == "yes")
			{
				getCircuitParameter().setFullyDifferential(true);
			}
		}
		if(complementaryNode != nullptr && !useHSpiceLibrary())
		{
			std::string nodeValue = complementaryNode->value();
			if(nodeValue == "yes")
			{
				getCircuitParameter().setComplementary(true);
			}
		}


	}


	void CircuitSpecificationFile::parseSupplyVoltage(
		rapidxml::xml_node<>& supplyVoltageNode)
	{
		rapidxml::xml_attribute<>* supplyVoltageAttribute = supplyVoltageNode.first_attribute(SUPPLY_VOLTAGE_ATTRIBUTE_.c_str());
		AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
		pin->setValue(std::atof(supplyVoltageAttribute->value()));
		if(useHSpiceLibrary())
		{
			pin->setNetId(createNetId("vdd!"));
		}
		else
		{
			pin->setNetId(createNetId("sourcePmos"));
		}

		getCircuitParameter().setSupplyVoltagePin(*pin);

	}



	void CircuitSpecificationFile::parseInputVoltage(
		rapidxml::xml_node<>& inputVoltageNode)
	{
		rapidxml::xml_attribute<>* inputVoltageAttribute = inputVoltageNode.first_attribute(INPUT_VOLTAGE_ATTRIBUTE_.c_str());

		AutomaticSizing::Pin * plusPin = new AutomaticSizing::Pin;
		plusPin->setValue(std::atof(inputVoltageAttribute->value()));
		plusPin->setNetId(createNetId("in1"));
		getCircuitParameter().setInputPinPlus(*plusPin);

		AutomaticSizing::Pin * minusPin = new AutomaticSizing::Pin;
		minusPin->setValue(std::atof(inputVoltageAttribute->value()));
		minusPin->setNetId(createNetId("in2"));

		getCircuitParameter().setInputPinMinus(*minusPin);

	}

	void CircuitSpecificationFile::parseGroundVoltage(
		rapidxml::xml_node<>& groundVoltageNode)
	{

		rapidxml::xml_attribute<>* groundVoltageAttribute = groundVoltageNode.first_attribute(GROUND_VOLTAGE_ATTRIBUTE_.c_str());
		AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
		pin->setValue(std::atof(groundVoltageAttribute->value()));
		if(useHSpiceLibrary())
		{
			pin->setNetId(createNetId("gnd!"));
		}
		else
		{
			pin->setNetId(createNetId("sourceNmos"));
		}

		getCircuitParameter().setGroundPin(*pin);
	}

	void CircuitSpecificationFile::parseReferenceVoltage(
		rapidxml::xml_node<>& referenceVoltageNode)
	{

		rapidxml::xml_attribute<>* referenceVoltageAttribute = referenceVoltageNode.first_attribute(REFERENCE_VOLTAGE_ATTRIBUTE_.c_str());
		AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
		pin->setValue(std::atof(referenceVoltageAttribute->value()));
		pin->setNetId(createNetId("vref"));
		getCircuitParameter().setReferenceVoltagePin(*pin);


	}

	void CircuitSpecificationFile::createOutputPins()
	{
		if(getCircuitParameter().isFullyDifferential())
		{
			AutomaticSizing::Pin * pin1 = new AutomaticSizing::Pin;
			pin1->setNetId(createNetId("out1"));
			getCircuitParameter().setOutputPinPlus(*pin1);

			AutomaticSizing::Pin * pin2 = new AutomaticSizing::Pin;
			pin2->setNetId(createNetId("out2"));
			getCircuitParameter().setOutputPinMinus(*pin2);
		}
		else
		{
			AutomaticSizing::Pin * pin = new AutomaticSizing::Pin;
			pin->setNetId(createNetId("out"));
			getCircuitParameter().setOutputPin(*pin);
		}

	}

	void CircuitSpecificationFile::setCircuitParameter(AutomaticSizing::CircuitParameter& parameter)
	{
		circuitParameter_ = & parameter;
	}

	AutomaticSizing::CircuitParameter& CircuitSpecificationFile::getCircuitParameter()
	{
		assert(circuitParameter_ != NULL);
		return * circuitParameter_;
	}

	Core::NetId CircuitSpecificationFile::createNetId(std::string netName)
	{
		Core::NetName net(netName);
		return net.createRootIdentifier();
	}

	Core::DeviceId CircuitSpecificationFile::createCapacitorId(Core::DeviceName capacitorName, std::string instanceName)
	{
		Core::DeviceId capacitorId = capacitorName.createRootIdentifier();
		Core::InstanceNamePath * instanceNamePath = new Core::InstanceNamePath;
		instanceNamePath->setIsRoot();
		if(instanceName != "empty")
		{
			instanceNamePath->append(Core::InstanceName(instanceName));
			capacitorId.setPath(*instanceNamePath);
		}
		else
		{
			delete instanceNamePath;
		}

		return capacitorId;
	}

	bool CircuitSpecificationFile::useHSpiceLibrary() const
	{
		return useHSpiceLibrary_;
	}

	void CircuitSpecificationFile::setUseHSpiceLibrary(bool useIt)
	{
		useHSpiceLibrary_ = useIt;
	}
}


