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

#ifndef SYNTHESIS_CIRCUITSPECIFICATIONFILE_CIRCUITSPECIFICATIONFILE_H_
#define SYNTHESIS_CIRCUITSPECIFICATIONFILE_CIRCUITSPECIFICATIONFILE_H_

#include "Control/incl/File/XmlInputFile.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"


namespace AutomaticSizing {

	class CircuitInformation;
	class CircuitParameter;
	class CircuitSpecification;
	class DesignAttributes;

}

namespace Synthesis {

	class CircuitSpecificationFile : public Control::XmlInputFile
	{
	public:
		CircuitSpecificationFile();
		void parse(AutomaticSizing::CircuitInformation & circuitInformation);

		void setUseHSpiceLibrary(bool useIt);

	private:
		static const std::string CIRCUIT_SPECIFICATIONS_NODE_;
		static const std::string LOAD_CAPACITY_NODE_;
		static const std::string BIAS_CURRENT_NODE_;
		static const std::string BIAS_CURRENT_ATTRIBUTE_;
		static const std::string LOAD_CAPACITY_ATTRIBUTE_;
		static const std::string SUPPLY_VOLTAGE_NODE_;
		static const std::string GROUND_VOLTAGE_NODE_;
		static const std::string REFERENCE_VOLTAGE_NODE_;
		static const std::string REFERENCE_VOLTAGE_ATTRIBUTE_;
		static const std::string INPUT_VOLTAGE_NODE_;
		static const std::string SUPPLY_VOLTAGE_ATTRIBUTE_;
		static const std::string GROUND_VOLTAGE_ATTRIBUTE_;
		static const std::string INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string DEVICE_NAME_NODE_;
		static const std::string GAIN_NODE_;
		static const std::string TRANSIENT_FREQUENCY_NODE_;
		static const std::string SLEW_RATE_NODE_;
		static const std::string CMRR_NODE_;
		static const std::string POS_PSRR_NODE_;
		static const std::string NEG_PSRR_NODE_;
		static const std::string OUTPUT_VOLTAGE_SWING_NODE_;
		static const std::string OFFSET_ERROR_NODE_;
		static const std::string POWER_CONSUMPTION_NODE_;
		static const std::string GAIN_ATTRIBUTE_;
		static const std::string TRANSIENT_FREQUENCY_ATTRIBUTE_;
		static const std::string SLEW_RATE_ATTRIBUTE_;
		static const std::string CMRR_ATTRIBUTE_;
		static const std::string POS_PSRR_ATTRIBUTE_;
		static const std::string NEG_PSRR_ATTRIBUTE_;
		static const std::string MAX_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_;
		static const std::string MAX_OFFSET_ERROR_ATTRIBUTE_;
		static const std::string MIN_OUTPUT_VOLTAGE_SWING_ATTRIBUTE_;
		static const std::string MIN_OFFSET_ERROR_ATTRIBUTE_;
		static const std::string POWER_CONSUMPTION_ATTRIBUTE_;
		static const std::string AREA_NODE_;
		static const std::string PHASE_MARGIN_NODE_;
		static const std::string PHASE_MARGIN_ATTRIBUTE_;
		static const std::string COMPLEMENTARY_NODE_;
		static const std::string FULLYDIFFERENTIAL_NODE_;

		void parseLoadCapacity();
		void parseBiasCurrent();
		void parseCircuitAttributes();
		void parseSupplyVoltage(rapidxml::xml_node<> & supplyVoltageNode);
		void parseInputVoltage(rapidxml::xml_node<> & inputVoltage);
		void parseGroundVoltage(rapidxml::xml_node<> & groundVoltageNode);
		void parseReferenceVoltage(rapidxml::xml_node<> & voltageReferencePinNode);
		void createOutputPins();

		Core::NetId createNetId(std::string netName);
		Core::DeviceId createCapacitorId(Core::DeviceName deviceName, std::string instanceName = "empty");
		static const std::string AREA_ATTRIBUTE_;
		static const std::string COMMON_MODE_INPUT_VOLTAGE_NODE_;
		static const std::string MAX_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string MIN_COMMON_MODE_INPUT_VOLTAGE_ATTRIBUTE_;
		static const std::string GATE_OVER_DRIVE_VOLTAGE_NODE_;
		static const std::string GATE_OVER_DRIVE_VOLTAGE_ATTRIBUTE_;
		static const std::string SETTLING_TIME_NODE_;
		static const std::string SETTLING_TIME_ATTRIBUTE_;

		//DesignAttributs
		static const std::string DESIGN_ATTRIBUTES_NODE_;
		static const std::string TRANSCONDUCTANCE_IN_WEAK_INVERSION_NODE_;
		static const std::string ALL_TRANSISTORS_IN_WEAK_INVERSION_NODE_;
		static const std::string WEAK_INVERSION_NODE_;
		static const std::string MODERATE_INVERSION_NODE_;


		AutomaticSizing::CircuitSpecification & parseCircuitSpecification();
		void parseCircuitVoltages();
		AutomaticSizing::DesignAttributes & parseDesignAttributes();


		void setCircuitParameter(AutomaticSizing::CircuitParameter & parameter);
		AutomaticSizing::CircuitParameter & getCircuitParameter();

		bool useHSpiceLibrary() const;

	private:

		AutomaticSizing::CircuitParameter * circuitParameter_;
		bool useHSpiceLibrary_;
	};


}



#endif /* SYNTHESIS_CIRCUITINFORMATIONFILES_CIRCUITINFORMATIONFILE_H_ */
