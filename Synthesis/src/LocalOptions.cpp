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

#include "Synthesis/incl/LocalOptions.h"
#include "Synthesis/incl/LibraryFile/LibraryFile.h"

#include "StructRec/incl/LibraryFile/LibraryFile.h"

#include "AutomaticSizing/incl/CircuitInformationFiles/TechnologieFileSHM.h"
#include "AutomaticSizing/incl/CircuitInformationFiles/TechnologieFileEKV.h"

#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {

	const std::string LocalOptions::XML_STRUCTREC_LIBRARY_FILE_OPTION_ = "xml-structrec-library-file";
	const std::string LocalOptions::XML_STRUCTREC_LIBRARY_FILE_DESCRIPTION_ = "path to the xml-library-file for structure recognition";
	const std::string LocalOptions::XML_CIRCUIT_LIBRARY_FILE_OPTION_ = "xml-circuit-library-file";
	const std::string LocalOptions::XML_CIRCUIT_LIBRARY_FILE_DESCRIPTION_ = "path to the xml-circuit-library-file with different amplification stages";
	const std::string LocalOptions::XML_TECHNOLOGIE_FILE_OPTION_ = "xml-technologie-file";
	const std::string LocalOptions::XML_TECHNOLOGIE_FILE_DESCRIPTION_ = "path to the technologie file of the used components";
	const std::string LocalOptions::XML_CIRCUITSPECIFICATIONS_FILE_OPTION_ = "xml-circuit-information-file";
	const std::string LocalOptions::XML_CIRCUITSPECIFICATIONS_FILE_DESCRIPTION_ = "path to the circuit information file with the circuit parameters and specifications";
	const std::string LocalOptions::TRANSISTOR_MODEL_DESCRIPTION_ = "declares which transistor model is used. Possible parameters: SHM = Shichman-Hodge-Model, EKV";
	const std::string LocalOptions::TRANSISTOR_MODEL_OPTION_ = "transistor-model";
	const std::string LocalOptions::SCALING_OPTION_ = "scaling";
    const std::string LocalOptions::SCALING_DESCRIPTION_ = "specifies the resolution of the transistor sizes. Possible parameters: 0.1mum, 1mum ";
    const std::string LocalOptions::HSPICE_NETLIST_DIRECTORY_OPTION_ = "HSPICE-netlist-dir";
    const std::string LocalOptions::HSPICE_NETLIST_DIRECTORY_DESCRIPTION_ = "Path to the directory to which the generated netlists are written.";
    const std::string LocalOptions::USE_HSPICE_LIBRARY_OPTION_ = "use-hspice-library";
    const std::string LocalOptions::USE_HSPICE_LIBRARY_DESCRIPTION_ = "Set true if you want to use the hspice library for synthese";


    LocalOptions::LocalOptions(const std::string& description) :
            Control::HspiceOptions(description)
    {
        initialize();
    }

    StructRec::LibraryFile LocalOptions::getStructRecLibraryFile() const
    {
    	 std::string optionValue = findStringOptionValue(XML_STRUCTREC_LIBRARY_FILE_OPTION_);
    	 StructRec::LibraryFile xmlLibraryFile;
    	 xmlLibraryFile.setPath(optionValue);
    	 return xmlLibraryFile;
    }

    LibraryFile LocalOptions::getCircuitLibraryFile() const
    {
    	 std::string optionValue = findStringOptionValue(XML_CIRCUIT_LIBRARY_FILE_OPTION_);
    	 LibraryFile xmlLibraryFile;
    	 xmlLibraryFile.setPath(optionValue);
    	 return xmlLibraryFile;
    }

    void LocalOptions::initialize()
    {
    	Control::OptionsBase::addStringOption(XML_STRUCTREC_LIBRARY_FILE_OPTION_, XML_STRUCTREC_LIBRARY_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(XML_CIRCUIT_LIBRARY_FILE_OPTION_, XML_CIRCUIT_LIBRARY_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(XML_CIRCUITSPECIFICATIONS_FILE_OPTION_, XML_CIRCUITSPECIFICATIONS_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(XML_TECHNOLOGIE_FILE_OPTION_, XML_TECHNOLOGIE_FILE_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(TRANSISTOR_MODEL_OPTION_, TRANSISTOR_MODEL_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(SCALING_OPTION_, SCALING_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(HSPICE_NETLIST_DIRECTORY_OPTION_, HSPICE_NETLIST_DIRECTORY_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(USE_HSPICE_LIBRARY_OPTION_, USE_HSPICE_LIBRARY_DESCRIPTION_);
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

    CircuitSpecificationFile LocalOptions::getCircuitSpecificationsFile() const
    {
    	 std::string optionValue = findStringOptionValue(XML_CIRCUITSPECIFICATIONS_FILE_OPTION_);
    	 CircuitSpecificationFile xmlCircuitInformationFile;
    	 xmlCircuitInformationFile.setPath(optionValue);
    	 xmlCircuitInformationFile.setUseHSpiceLibrary(useHSpiceLibrary());
    	 return xmlCircuitInformationFile;
    }

    AutomaticSizing::TechnologieFileSHM LocalOptions::getTechnologieFileSHM() const
    {
    	std::string optionValue = findStringOptionValue(XML_TECHNOLOGIE_FILE_OPTION_);
    	AutomaticSizing::TechnologieFileSHM xmlTechnologieFile;
    	xmlTechnologieFile.setPath(optionValue);
    	return xmlTechnologieFile;

    }

    AutomaticSizing::TechnologieFileEKV LocalOptions::getTechnologieFileEKV() const
    {
    	std::string optionValue = findStringOptionValue(XML_TECHNOLOGIE_FILE_OPTION_);
    	AutomaticSizing::TechnologieFileEKV xmlTechnologieFile;
    	xmlTechnologieFile.setPath(optionValue);
    	return xmlTechnologieFile;

    }

    std::string LocalOptions::getHSPICENetlistDirectoryPath() const
    {
    	return findStringOptionValue(HSPICE_NETLIST_DIRECTORY_OPTION_);
    }

    bool LocalOptions::useHSpiceLibrary() const
    {
    	if(hasOptionValue(USE_HSPICE_LIBRARY_OPTION_))
    	{
    		 std::string optionValue = findStringOptionValue(USE_HSPICE_LIBRARY_OPTION_);
    		 if(optionValue == "true")
    			 return true;
    		 else
    			 return false;
    	}
    	else
    		return false;

    }

    bool LocalOptions::isCircuitReadInOption() const
    {
    	return false;
    }

}
