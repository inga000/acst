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

#ifndef AUTOMATICSIZING_CIRCUITINFORMATIONFILES_CIRCUITINFORMATIONFILE_H_
#define AUTOMATICSIZING_CIRCUITINFORMATIONFILES_CIRCUITINFORMATIONFILE_H_

#include "Control/incl/File/XmlInputFile.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"


namespace AutomaticSizing {

	class CircuitInformation;
	class CircuitParameter;
	class CircuitSpecification;
	class DesignAttributes;

	class CircuitInformationFile : public Control::XmlInputFile
	{
	public:
		CircuitInformationFile();
		void parse(CircuitInformation & circuitInformation );

	private:
		static const std::string CIRCUIT_PARAMETER_NODE_;
		static const std::string CIRCUIT_SPECIFICATIONS_NODE_;
		static const std::string LOAD_CAPACITIES_NODE_;
		static const std::string LOAD_CAPACITY_NODE_;
		static const std::string SUPPLY_VOLTAGE_PIN_NODE_;
		static const std::string SUPPLY_VOLTAGE_NODE_;
		static const std::string GROUND_PIN_NODE_;
		static const std::string GROUND_VOLTAGE_NODE_;
		static const std::string REFERENCEVOLTAGE_PIN_NODE_;
		static const std::string REFERENCE_VOLTAGE_NODE_;
		static const std::string REFERENCE_VOLTAGE_ATTRIBUTE_;
		static const std::string BIAS_CURRENT_NODE_;
		static const std::string CURRENT_BIAS_PIN_NODE_;
		static const std::string BIAS_VOLTAGE_NODE_;
		static const std::string VOLTAGE_BIAS_PIN_NODE_;
		static const std::string VOLTAGE_BIAS_PINS_NODE_;
		static const std::string INPUT_PIN_MINUS_NODE_;
		static const std::string INPUT_PIN_PLUS_NODE_;
		static const std::string OUTPUT_PIN_NODE_;
		static const std::string OUTPUT_PIN_MINUS_NODE_;
		static const std::string OUTPUT_PIN_PLUS_NODE_;
		static const std::string INPUT_VOLTAGE_NODE_;
		static const std::string SUPPLY_VOLTAGE_ATTRIBUTE_;
		static const std::string GROUND_VOLTAGE_ATTRIBUTE_;
		static const std::string BIAS_CURRENT_ATTRIBUTE_;
		static const std::string BIAS_VOLTAGE_ATTRIBUTE_;
		static const std::string INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string NET_NAME_NODE_;
		static const std::string DEVICE_NAME_NODE_;
		static const std::string VALUE_NODE_;
		static const std::string GAIN_NODE_;
		static const std::string TRANSIENT_FREQUENCY_NODE_;
		static const std::string SLEW_RATE_NODE_;
		static const std::string CMRR_NODE_;
		static const std::string POS_PSRR_NODE_;
		static const std::string NEG_PSRR_NODE_;
		static const std::string OUTPUT_VOLTAGE_NODE_;
		static const std::string POWER_CONSUMPTION_NODE_;
		static const std::string GAIN_ATTRIBUTE_;
		static const std::string TRANSIENT_FREQUENCY_ATTRIBUTE_;
		static const std::string SLEW_RATE_ATTRIBUTE_;
		static const std::string CMRR_ATTRIBUTE_;
		static const std::string POS_PSRR_ATTRIBUTE_;
		static const std::string NEG_PSRR_ATTRIBUTE_;
		static const std::string MAX_OUTPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string MIN_OUTPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string POWER_CONSUMPTION_ATTRIBUTE_;
		static const std::string AREA_NODE_;
		static const std::string AREA_ATTRIBUTE_;
		static const std::string COMMON_MODE_INPUT_VOLTAGE_NODE_;
		static const std::string MAX_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string MIN_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string GATE_OVER_DRIVE_VOLTAGE_NODE_;
		static const std::string GATE_OVER_DRIVE_VOLTAGE_ATTRIBUTE_;
		static const std::string PHASE_MARGIN_NODE_;
		static const std::string PHASE_MARGIN_ATTRIBUTE_;


		//DesignAttributs
		static const std::string DESIGN_ATTRIBUTES_NODE_;
		static const std::string TRANSCONDUCTANCE_IN_WEAK_INVERSION_NODE_;
		static const std::string ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_;
		static const std::string WEAK_INVERSION_NODE_;
		static const std::string MODERATE_INVERSION_NODE_;


		CircuitSpecification & parseCircuitSpecification();
		CircuitParameter & parseCircuitParameter();
		DesignAttributes & parseDesignAttributes();


		void parseLoadCapacities(rapidxml::xml_node<> & loadCapacitiesNode);
		void parseLoadCapacity(rapidxml::xml_node<> & loadCapacityNode);
		void parseSupplyVoltagePin(rapidxml::xml_node<> & supplyVoltagePinNode);
		void parseCurrentBiasPin(rapidxml::xml_node<>& biasPinNode);
		void parseInputPinMinus(rapidxml::xml_node<> & inputPinsNodeMinus);
		void parseInputPinPlus(rapidxml::xml_node<> & inputPinNodePlus);
		void parseOutputPins(rapidxml::xml_node<> &  outputPinNode);
		void parseGroundPin(rapidxml::xml_node<> & groundPinNode);
		void parseVoltageBiasPins(rapidxml::xml_node<> & voltageBiasPinsNode);
		void parseReferenceVoltagePin(rapidxml::xml_node<> & voltageReferencePinNode);

		Core::NetId parseNetId(rapidxml::xml_node<> & netIdNode);
		Core::DeviceId parseDeviceId(rapidxml::xml_node<> & deviceIdNode);

		void setCircuitParameter(CircuitParameter & parameter);
		CircuitParameter & getCircuitParameter();

	private:

		CircuitParameter * circuitParameter_;
	};


}



#endif /* AUTOMATICSIZING_CIRCUITINFORMATIONFILES_CIRCUITINFORMATIONFILE_H_ */
