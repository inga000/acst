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


#ifndef AUTOMATICSIZING_CIRCUITINFORMATIONFILES_TECHNOLOGIEFILESHM_H_
#define AUTOMATICSIZING_CIRCUITINFORMATIONFILES_TECHNOLOGIEFILESHM_H_



#include "Control/incl/File/XmlInputFile.h"
#include <rapidxml.hpp>
#include <string>

namespace AutomaticSizing {

	class CircuitInformation;
	class TechnologieSpecificationSHM;

	class TechnologieFileSHM : public Control::XmlInputFile
	{
	public:
		TechnologieFileSHM();

		void parse(CircuitInformation & CircuitInformation);

	private:
		static const std::string GENERAL_NODE_;
		static const std::string THERMAL_VOLTAGE_NODE_;
		static const std::string THERMAL_VOLTAGE_ATTRIBUTE_;

		static const std::string PMOS_NODE_;
		static const std::string NMOS_NODE_;
		static const std::string THRESHOLD_VOLTAGE_NODE_;
		static const std::string MOBILITY_OXIDE_CAPACITY_NODE_;
		static const std::string EARLY_VOLTAGE_NODE_;
		static const std::string THRESHOLD_VOLTAGE_ATTRIBUTE_;
		static const std::string MOBILITY_OXIDE_CAPACITY_ATTRIBUTE_;
		static const std::string EARLY_VOLTAGE_ATTRIBUTE_;
		static const std::string OVERLAP_CAPACITY_NODE_;
		static const std::string OVERLAP_CAPACITY_ATTRIBUTE_;
		static const std::string SLOPE_FACTOR_NODE_;
		static const std::string SLOPE_FACTOR_ATTRIBUTE_;
		static const std::string GATE_OXIDE_CAPACITY_NODE_;
		static const std::string GATE_OXIDE_CAPACITY_ATTRIBUTE_;
		static const std::string MIN_AREA_NODE_;
		static const std::string MIN_AREA_ATTRIBUTE_;
		static const std::string MIN_LENGTH_NODE_;
		static const std::string MIN_LENGTH_ATTRIBUTE_;
		static const std::string MIN_WIDTH_NODE_;
		static const std::string MIN_WIDTH_ATTRIBUTE_;
		static const std::string CHANNEL_LENGTH_COEFFICIENT_WEAK_INVERSION_NODE_;
		static const std::string CHANNEL_LENGTH_COEFFICIENT_STRONG_INVERSION_NODE_;
		static const std::string CHANNEL_LENGTH_COEFFICIENT_ATTRIBUTE_;
		static const std::string ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_NODE_;
		static const std::string ZERO_BIAS_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_;
		static const std::string ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_NODE_;
		static const std::string ZERO_BIAS_SIDEWALL_BULK_JUNCTION_CAPACITANCE_ATTRIBUTE_;
		static const std::string BULK_JUNCTION_CONTACT_POTENTIAL_NODE_;
		static const std::string BULK_JUNCTION_CONTACT_POTENTIAL_ATTRIBUTE_;
		static const std::string LATERAL_DIFFUSION_LENGTH_NODE_;
		static const std::string LATERAL_DIFFUSION_LENGTH_ATTRIBUTE_;


		TechnologieSpecificationSHM & parsePmosTechnologieSpecificationSHM();
		TechnologieSpecificationSHM & parseNmosTechnologieSpecificationSHM();

		void parseSpecification(rapidxml::xml_node<>& typeNode, TechnologieSpecificationSHM  & technologieSpecification);
		void parseGeneralSpecification(TechnologieSpecificationSHM & specification);

	};


}



#endif /* AUTOMATICSIZING_CIRCUITINFORMATIONFILES_TECHNOLOGIEFILE_H_ */
