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


#include "AutomaticSizing/incl/LocalOptions.h"

#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Circuit/Circuit.h"

#include "HSpice/incl/InputFile/InputFile.h"

#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"



namespace AutomaticSizing {

	const std::string LocalOptions::XML_TECHNOLOGIE_FILE_OPTION_ = "xml-technologie-file";
	const std::string LocalOptions::XML_TECHNOLOGIE_FILE_DESCRIPTION_ = "path to the technologie file of the used components";
	const std::string LocalOptions::XML_CIRCUIT_INFORMATION_FILE_OPTION_ = "xml-circuit-information-file";
	const std::string LocalOptions::XML_CIRCUIT_INFORMATION_FILE_DESCRIPTION_ = "path to the circuit information file with the circuit parameters and specifications";
	const std::string LocalOptions::TRANSISTOR_MODEL_DESCRIPTION_ = "declares which transistor model is used. Possible parameters: SHM = Shichman-Hodge-Model, EKV";
	const std::string LocalOptions::TRANSISTOR_MODEL_OPTION_ = "transistor-model";
	const std::string LocalOptions::SCALING_OPTION_ = "scaling";
    const std::string LocalOptions::SCALING_DESCRIPTION_ = "specifies the resolution of the transistor sizes. Possible parameters: 0.1mum, 1mum ";
    const std::string LocalOptions::RUNTIME_OPTION_ = "runtime";
    const std::string LocalOptions::RUNTIME_DESCRIPTION_ = "runtime of the gecode search in sec.";

    LocalOptions::LocalOptions(const std::string& description) :
		Partitioning::LocalOptions(description)
    {
        initialize();
    }

    void LocalOptions::initialize()
    {
    	Control::OptionsBase::addStringOption(XML_CIRCUIT_INFORMATION_FILE_OPTION_, XML_CIRCUIT_INFORMATION_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(XML_TECHNOLOGIE_FILE_OPTION_, XML_TECHNOLOGIE_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(TRANSISTOR_MODEL_OPTION_, TRANSISTOR_MODEL_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(SCALING_OPTION_, SCALING_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(RUNTIME_OPTION_, RUNTIME_DESCRIPTION_);
    }


    std::string LocalOptions::getTransistorModel() const
    {
    	std::string transistorModel = findStringOptionValue(TRANSISTOR_MODEL_OPTION_);
    	assert(transistorModel == "EKV" || transistorModel == "SHM", "Transistor model not supported.");
    	return transistorModel;
    }

    std::string LocalOptions::getScaling() const
    {
       	std::string scaling = findStringOptionValue(SCALING_OPTION_);
       	assert(scaling == "1mum" || scaling == "0.1mum", "Definition not allowed ");
       	return scaling;
     }

    CircuitInformationFile LocalOptions::getCircuitInformationFile() const
    {
    	 std::string optionValue = findStringOptionValue(XML_CIRCUIT_INFORMATION_FILE_OPTION_);
    	 CircuitInformationFile xmlCircuitInformationFile;
    	 xmlCircuitInformationFile.setPath(optionValue);
    	 return xmlCircuitInformationFile;
    }

    TechnologieFileSHM LocalOptions::getTechnologieFileSHM() const
    {
    	std::string optionValue = findStringOptionValue(XML_TECHNOLOGIE_FILE_OPTION_);
    	TechnologieFileSHM xmlTechnologieFile;
    	xmlTechnologieFile.setPath(optionValue);
    	return xmlTechnologieFile;

    }

    TechnologieFileEKV LocalOptions::getTechnologieFileEKV() const
    {
    	std::string optionValue = findStringOptionValue(XML_TECHNOLOGIE_FILE_OPTION_);
    	TechnologieFileEKV xmlTechnologieFile;
    	xmlTechnologieFile.setPath(optionValue);
    	return xmlTechnologieFile;

    }

    int LocalOptions::getRuntime() const
    {
       	int runtime = stoi(findStringOptionValue(RUNTIME_OPTION_));
       	return runtime;
     }
}
