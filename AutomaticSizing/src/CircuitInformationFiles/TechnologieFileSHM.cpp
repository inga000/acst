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


#include "../../../AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "../../../AutomaticSizing/incl/CircuitInformation/TechnologieSpecificationSHM.h"
#include "../../incl/CircuitInformationFiles/TechnologieFileSHM.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

	const std::string TechnologieFileSHM::GENERAL_NODE_ = "general";
	const std::string TechnologieFileSHM::THERMAL_VOLTAGE_NODE_ = "thermalVoltage";
	const std::string TechnologieFileSHM::THERMAL_VOLTAGE_ATTRIBUTE_ = "Vt";

	const std::string TechnologieFileSHM::PMOS_NODE_ = "pmos";
	const std::string TechnologieFileSHM::NMOS_NODE_ = "nmos";
	const std::string TechnologieFileSHM::THRESHOLD_VOLTAGE_NODE_ = "thresholdVoltage";
	const std::string TechnologieFileSHM::MOBILITY_OXIDE_CAPACITY_NODE_ = "mobilityOxideCapacity";
	const std::string TechnologieFileSHM::EARLY_VOLTAGE_NODE_ = "earlyVoltage";
	const std::string TechnologieFileSHM::THRESHOLD_VOLTAGE_ATTRIBUTE_ = "vth";
	const std::string TechnologieFileSHM::MOBILITY_OXIDE_CAPACITY_ATTRIBUTE_ = "muCox";
	const std::string TechnologieFileSHM::EARLY_VOLTAGE_ATTRIBUTE_ = "earlyVoltage";
	const std::string TechnologieFileSHM::OVERLAP_CAPACITY_NODE_ = "overlapCapacity";
	const std::string TechnologieFileSHM::OVERLAP_CAPACITY_ATTRIBUTE_ = "Cgdov";
	const std::string TechnologieFileSHM::SLOPE_FACTOR_NODE_ = "slopeFactor";
	const std::string TechnologieFileSHM::SLOPE_FACTOR_ATTRIBUTE_ = "n";
	const std::string TechnologieFileSHM::GATE_OXIDE_CAPACITY_NODE_ = "gateOxideCapacity";
	const std::string TechnologieFileSHM::GATE_OXIDE_CAPACITY_ATTRIBUTE_ = "Cox";
	const std::string TechnologieFileSHM::MIN_AREA_NODE_ = "minArea";
	const std::string TechnologieFileSHM::MIN_AREA_ATTRIBUTE_ = "Amin";
	const std::string TechnologieFileSHM::MIN_LENGTH_NODE_ = "minLength";
	const std::string TechnologieFileSHM::MIN_LENGTH_ATTRIBUTE_ = "Lmin";
	const std::string TechnologieFileSHM::MIN_WIDTH_NODE_ = "minWidth";
	const std::string TechnologieFileSHM::MIN_WIDTH_ATTRIBUTE_ = "Wmin";
	const std::string TechnologieFileSHM::CHANNEL_LENGTH_COEFFICIENT_STRONG_INVERSION_NODE_ = "channelLengthCoefficientStrongInversion";
	const std::string TechnologieFileSHM::CHANNEL_LENGTH_COEFFICIENT_WEAK_INVERSION_NODE_ = "channelLengthCoefficientWeakInversion";
	const std::string TechnologieFileSHM::CHANNEL_LENGTH_COEFFICIENT_ATTRIBUTE_ = "lamda";
	const std::string TechnologieFileSHM::ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_NODE_ = "zeroBiasBulkJunctionCapacitance";
	const std::string TechnologieFileSHM::ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_ = "Cj";
	const std::string TechnologieFileSHM::ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_NODE_ = "zeroBiasSidewallBulkJunctionCapacitance";
	const std::string TechnologieFileSHM::ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_ = "Cjsw";
	const std::string TechnologieFileSHM::BULK_JUNCTION_CONTACT_POTENTIAL_NODE_ = "bulkJunctionContactPotential";
	const std::string TechnologieFileSHM::BULK_JUNCTION_CONTACT_POTENTIAL_ATTRIBUTE_ = "pb";
	const std::string TechnologieFileSHM::LATERAL_DIFFUSION_LENGTH_NODE_ = "lateralDiffusionLength";
	const std::string TechnologieFileSHM::LATERAL_DIFFUSION_LENGTH_ATTRIBUTE_ = "Ldiff";



	TechnologieFileSHM::TechnologieFileSHM()
	{

	}

	void TechnologieFileSHM::parse(CircuitInformation & circuitInformation)
	{
		try
		{
			Control::XmlInputFile::parse();

			circuitInformation.setTechnologieSpecificationSHMNmos(parseNmosTechnologieSpecificationSHM());
			circuitInformation.setTechnologieSpecificationSHMPmos(parsePmosTechnologieSpecificationSHM());
		}
		catch(const ::Core::BacktraceException& ex)
		{
			logDebug(ex.what());
		}
		catch(...)
		{
			logDebug("Error pasing Technologie file!");
		}

	}

	TechnologieSpecificationSHM & TechnologieFileSHM::parsePmosTechnologieSpecificationSHM()
	{
		rapidxml::xml_node<>* pmosNode = getDocumentNode().first_node(PMOS_NODE_.c_str());
		TechnologieSpecificationSHM * pmosSpecification = new TechnologieSpecificationSHM;
		parseSpecification(*pmosNode, * pmosSpecification);
		parseGeneralSpecification(*pmosSpecification);
		return * pmosSpecification;
	}

	TechnologieSpecificationSHM & TechnologieFileSHM::parseNmosTechnologieSpecificationSHM()
	{
		rapidxml::xml_node<>* nmosNode = getDocumentNode().first_node(NMOS_NODE_.c_str());
		TechnologieSpecificationSHM * nmosSpecification = new TechnologieSpecificationSHM;
		parseSpecification(*nmosNode, * nmosSpecification);
		parseGeneralSpecification(*nmosSpecification);
		return * nmosSpecification;
	}

	void TechnologieFileSHM::parseSpecification(rapidxml::xml_node<>& typeNode, TechnologieSpecificationSHM & technologieSpecification)
	{
		rapidxml::xml_node<>* thresholdVoltageNode = typeNode.first_node(THRESHOLD_VOLTAGE_NODE_.c_str());
		if(thresholdVoltageNode != NULL)
		{
			rapidxml::xml_attribute<>* voltageAttribute = thresholdVoltageNode->first_attribute(THRESHOLD_VOLTAGE_ATTRIBUTE_.c_str());
			technologieSpecification.setThresholdVoltage(std::atof(voltageAttribute->value()));
		}
		else
		{
			logDebug("ThresholdVoltage needs to be specified.");
		}
		rapidxml::xml_node<>* mobilityOxideCapacityNode = typeNode.first_node(MOBILITY_OXIDE_CAPACITY_NODE_.c_str());
		if(mobilityOxideCapacityNode != NULL)
		{
			rapidxml::xml_attribute<>* muCoxAttribute = mobilityOxideCapacityNode->first_attribute(MOBILITY_OXIDE_CAPACITY_ATTRIBUTE_.c_str());
			technologieSpecification.setMobilityOxideCapacityCoefficient(std::atof(muCoxAttribute->value()));
		}
		else
		{
			logDebug("Mobility times oxide capacity needs to be specified.");
		}

		rapidxml::xml_node<>* earlyVoltageNode = typeNode.first_node(EARLY_VOLTAGE_NODE_.c_str());

		if(earlyVoltageNode != NULL)
		{
			rapidxml::xml_attribute<>* earlyVoltageAttribute = earlyVoltageNode->first_attribute(EARLY_VOLTAGE_ATTRIBUTE_.c_str());
			technologieSpecification.setEarlyVoltage(std::atof(earlyVoltageAttribute->value()));
		}
		else
		{
			logDebug("Early voltage needs to be specified.");
		}
		rapidxml::xml_node<>* overlapCapacityNode = typeNode.first_node(OVERLAP_CAPACITY_NODE_.c_str());
		if(overlapCapacityNode != NULL)
		{
			rapidxml::xml_attribute<>* overlabCapacityAttribute = overlapCapacityNode->first_attribute(OVERLAP_CAPACITY_ATTRIBUTE_.c_str());
			technologieSpecification.setOverlapCapacity(std::atof(overlabCapacityAttribute->value()));
		}
		else
		{
			logDebug("Overlap capacity needs to be specified.");
		}
		rapidxml::xml_node<>* slopeFactorNode = typeNode.first_node(SLOPE_FACTOR_NODE_.c_str());
		if(slopeFactorNode != NULL)
		{
			rapidxml::xml_attribute<>* slopeFactorAttribute = slopeFactorNode->first_attribute(SLOPE_FACTOR_ATTRIBUTE_.c_str());
			technologieSpecification.setSlopeFactor(std::atof(slopeFactorAttribute->value()));
		}
		else
		{
			logDebug("SlopeFactor needs to be specified.");
		}
		rapidxml::xml_node<>* gateOxideCapacityNode = typeNode.first_node(GATE_OXIDE_CAPACITY_NODE_.c_str());
		if(gateOxideCapacityNode != NULL)
		{
			rapidxml::xml_attribute<>* gateOxideCapacityAttribute = gateOxideCapacityNode->first_attribute(GATE_OXIDE_CAPACITY_ATTRIBUTE_.c_str());
			technologieSpecification.setGateOxideCapacity(std::atof(gateOxideCapacityAttribute->value()));
		}
		else
		{
			logDebug("Gate oxide capacity needs to be specified.");
		}
		rapidxml::xml_node<>* minAreaNode = typeNode.first_node(MIN_AREA_NODE_.c_str());
		if(minAreaNode != NULL)
		{
			rapidxml::xml_attribute<>* minAreaAttribute = minAreaNode->first_attribute(MIN_AREA_ATTRIBUTE_.c_str());
			technologieSpecification.setMinArea(std::atof(minAreaAttribute->value()));
		}
		else
		{
			logDebug("Minimal area needs to be specified.");
		}
		rapidxml::xml_node<>* minLengthNode = typeNode.first_node(MIN_LENGTH_NODE_.c_str());
		if(minLengthNode != NULL)
		{
			rapidxml::xml_attribute<>* minLengthAttribute = minLengthNode->first_attribute(MIN_LENGTH_ATTRIBUTE_.c_str());
			technologieSpecification.setMinLength(std::atof(minLengthAttribute->value()));
		}
		else
		{
			logDebug("Minimal length needs to be specified.");
		}
		rapidxml::xml_node<>* minWidthNode = typeNode.first_node(MIN_WIDTH_NODE_.c_str());
		if(minWidthNode != NULL)
		{
			rapidxml::xml_attribute<>* minWidthAttribute = minWidthNode->first_attribute(MIN_WIDTH_ATTRIBUTE_.c_str());
			technologieSpecification.setMinWidth(std::atof(minWidthAttribute->value()));
		}
		else
		{
			logDebug("Minimal Width needs to be specified.");
		}

		rapidxml::xml_node<>* channelLengthCoefficientWeakInversionNode = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_WEAK_INVERSION_NODE_.c_str());
		assert(channelLengthCoefficientWeakInversionNode != NULL, "Channel length coefficient needs to be specified.");
		rapidxml::xml_attribute<>* channelLengthCoefficientWeakInversionAttribute = channelLengthCoefficientWeakInversionNode->first_attribute(CHANNEL_LENGTH_COEFFICIENT_ATTRIBUTE_.c_str());
		technologieSpecification.setChannelLengthCoefficientWeakInversion(std::atof(channelLengthCoefficientWeakInversionAttribute->value()));

		rapidxml::xml_node<>* channelLengthCoefficientStrongInversionNode = typeNode.first_node(CHANNEL_LENGTH_COEFFICIENT_STRONG_INVERSION_NODE_.c_str());
		assert(channelLengthCoefficientStrongInversionNode != NULL, "Channel length coefficient needs to be specified.");
		rapidxml::xml_attribute<>* channelLengthCoefficientStrongInversionAttribute = channelLengthCoefficientStrongInversionNode->first_attribute(CHANNEL_LENGTH_COEFFICIENT_ATTRIBUTE_.c_str());
		technologieSpecification.setChannelLengthCoefficientStrongInversion(std::atof(channelLengthCoefficientStrongInversionAttribute->value()));

		rapidxml::xml_node<>* zeroBiasBulkJunctionCapacitanceNode = typeNode.first_node(ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_NODE_.c_str());
		if(zeroBiasBulkJunctionCapacitanceNode != NULL)
		{
			rapidxml::xml_attribute<>* zeroBiasBulkJunctionCapacitanceAttribute = zeroBiasBulkJunctionCapacitanceNode->first_attribute(ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_.c_str());
			technologieSpecification.setZeroBiasBulkJunctionCapacitance(std::atof(zeroBiasBulkJunctionCapacitanceAttribute->value()));
		}
		else
		{
			logDebug("Zero-bias bulk junction capacitance needs to be specified.");
		}
		rapidxml::xml_node<>* zeroBiasSidewallBulkJunctionCapacitanceNode = typeNode.first_node(ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_NODE_.c_str());
		if(zeroBiasSidewallBulkJunctionCapacitanceNode != NULL)
		{
			rapidxml::xml_attribute<>* zeroBiasSidewallBulkJunctionCapacitanceAttribute = zeroBiasSidewallBulkJunctionCapacitanceNode->first_attribute(ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_.c_str());
			technologieSpecification.setZeroBiasSidewallBulkJunctionCapacitance(std::atof(zeroBiasSidewallBulkJunctionCapacitanceAttribute->value()));
		}
		else
		{
			logDebug("Zero-bias sidewall  bulk junction capacitance needs to be specified.");
		}
		rapidxml::xml_node<>* bulkJunctionContactPotentialNode = typeNode.first_node(BULK_JUNCTION_CONTACT_POTENTIAL_NODE_.c_str());
		if(bulkJunctionContactPotentialNode != NULL)
		{
			rapidxml::xml_attribute<>* bulkJunctionContactPotentialAttribute = bulkJunctionContactPotentialNode->first_attribute(BULK_JUNCTION_CONTACT_POTENTIAL_ATTRIBUTE_.c_str());
			technologieSpecification.setBulkJunctionContactPotential(std::atof(bulkJunctionContactPotentialAttribute->value()));
		}
		else
		{
			logDebug("Bulk junction contact potential needs to be specified.");
		}
		rapidxml::xml_node<>* lateralDiffusionLengthNode = typeNode.first_node(LATERAL_DIFFUSION_LENGTH_NODE_.c_str());
		if(lateralDiffusionLengthNode != NULL)
		{
			rapidxml::xml_attribute<>* lateralDiffusionLengthAttribute =lateralDiffusionLengthNode->first_attribute(LATERAL_DIFFUSION_LENGTH_ATTRIBUTE_.c_str());
			float lateralDiffusionLengthTemp = std::atof(lateralDiffusionLengthAttribute->value());
			technologieSpecification.setLateralDiffusionLength(lateralDiffusionLengthTemp/1000000);
		}
		else
		{
			logDebug("Channel length coefficient needs to be specified.");
		}

	}

	void TechnologieFileSHM::parseGeneralSpecification(TechnologieSpecificationSHM & specification)
	{
		rapidxml::xml_node<>* generalNode = getDocumentNode().first_node(GENERAL_NODE_.c_str());

		rapidxml::xml_node<>* thermalVoltageNode = generalNode->first_node(THERMAL_VOLTAGE_NODE_.c_str());
		assert(thermalVoltageNode != NULL, "Thermal voltage needs to be specified.");
		rapidxml::xml_attribute<>* thermalVoltageAttribute = thermalVoltageNode->first_attribute(THERMAL_VOLTAGE_ATTRIBUTE_.c_str());
		specification.setThermalVoltage(std::atof(thermalVoltageAttribute->value()));
	}
}
