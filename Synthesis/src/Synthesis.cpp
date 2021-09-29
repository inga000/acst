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

#include "Synthesis/incl/Synthesis.h"
#include "Synthesis/incl/Results/Result.h"
#include "Synthesis/incl/Library/Library.h"
#include "Synthesis/incl/LibraryFile/LibraryFile.h"
#include "Synthesis/incl/SearchSpaceExploration/SearchSpaceExploration.h"
#include "Synthesis/incl/SearchSpaceExploration/OpAmpSizing.h"
#include "Synthesis/incl/CircuitSpecificationFile/CircuitSpecificationFile.h"
#include "Synthesis/incl/Library/FunctionalBlockLibrary.h"

#include "HSpice/incl/OutputFile/OutputFile.h"

#include "Control/incl/FileFormat/OutputFileFormat.h"

#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"

#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/LibraryFile/LibraryFile.h"

#include "AutomaticSizing/incl/CircuitInformationFiles/TechnologieFileEKV.h"
#include "AutomaticSizing/incl/CircuitInformationFiles/TechnologieFileSHM.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"

#include <time.h>

namespace Synthesis {

    Synthesis::Synthesis() :
		            Control::Analysis<LocalOptions>(),
					result_(nullptr),
					deviceTypeRegister_(nullptr)
    {
    }

    Synthesis::~Synthesis()
    {
    	delete result_;
    	delete deviceTypeRegister_;
    }

    void Synthesis::initialize()
    {
    	deviceTypeRegister_ = getLocalOptions().readDeviceTypeRegister();
    }

    void Synthesis::compute()
    {

    	logDebug("Parsing Circuit Structure Library");
        StructRec::Library* structureLibrary = parseStructRecLibrary();
        logDebug("Done parsing structure library");

        if(getLocalOptions().useHSpiceLibrary())
        {
        	logDebug("Parsing Circuit Library");
        	Library* circuitLibrary = parseCircuitLibrary();
        	logDebug("Done parsing circuit library");
        	circuitLibrary->setStructRecLibrary(*structureLibrary);

            SearchSpaceExploration * searchSpaceExploration = new SearchSpaceExploration;

            searchSpaceExploration->setLibrary(*circuitLibrary);
            searchSpaceExploration->setDefinition(getLocalOptions().getScaling());
            searchSpaceExploration->setTransistorModel(getLocalOptions().getTransistorModel());
            searchSpaceExploration->setCircuitInformation(*parseCircuitInformation());
            logDebug("Start search space exploration");
            result_ = &searchSpaceExploration->compute();

             logDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RESULTS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
             logDebug(result_);

             delete searchSpaceExploration;
             delete circuitLibrary;
        }
        else
        {
        	OpAmpSizing * opAmpSizing = new OpAmpSizing;

            opAmpSizing->setDefinition(getLocalOptions().getScaling());
            opAmpSizing->setTransistorModel(getLocalOptions().getTransistorModel());
            opAmpSizing->setHSpiceNetlistDirectoryPath(getLocalOptions().getHSPICENetlistDirectoryPath());

            AutomaticSizing::CircuitInformation * circuitInformation = parseCircuitInformation();
            opAmpSizing->setCircuitInformation(*circuitInformation);


			auto start = std::chrono::high_resolution_clock::now();

			logDebug("Create Functional Block Library");
			FunctionalBlockLibrary *functionalBlockLibrary = new FunctionalBlockLibrary(*circuitInformation);


			functionalBlockLibrary->setStructRecLibrary(*structureLibrary);

			auto end = std::chrono::high_resolution_clock::now();
			auto diff = end - start;
			int timeParaMinutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
			int timeParaSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count() - timeParaMinutes * 60;
			logDebug("\n Runtime Creation of the Functional Block Library: " << timeParaMinutes <<"min " << timeParaSeconds << "s");

			opAmpSizing->setLibrary(*functionalBlockLibrary);

			logDebug("Start search space exploration");
			opAmpSizing->compute();

			delete opAmpSizing;
			delete functionalBlockLibrary;
        }

        delete structureLibrary;
    }

    void Synthesis::write()
    {
    	if(getLocalOptions().useHSpiceLibrary())
    	{
    		getResult().write(getLocalOptions().getHSPICENetlistDirectoryPath(), getLocalOptions().useHSpiceLibrary());
    	}

    }


    Library * Synthesis::parseCircuitLibrary() const
    {
    	LibraryFile libraryFile = getLocalOptions().getCircuitLibraryFile();
    	libraryFile.setDeviceTypeRegister(getDeviceTypeRegister());
    	libraryFile.setSupplyNetIdentifiers(getLocalOptions().readSupplyNetIdentifiers());
    	libraryFile.setDeviceLineMapper(*getLocalOptions().readDeviceLineMapper());
        return libraryFile.parse();
    }

    StructRec::Library * Synthesis::parseStructRecLibrary() const
    {
   		StructRec::LibraryFile structRecLibraryFile = getLocalOptions().getStructRecLibraryFile();
   		structRecLibraryFile.setDeviceTypeRegister(getDeviceTypeRegister());
   		StructRec::Library* structRecLibrary = structRecLibraryFile.parse();
       	return structRecLibrary;
    }

    AutomaticSizing::CircuitInformation * Synthesis::parseCircuitInformation() const
    {
    	logDebug("Parse circuit Information");
    	AutomaticSizing::CircuitInformation * circuitInformation = new AutomaticSizing::CircuitInformation;
    	CircuitSpecificationFile specificationFile = getLocalOptions().getCircuitSpecificationsFile();
    	specificationFile.parse(*circuitInformation);

    	if(getLocalOptions().getTransistorModel() == "SHM")
    	{
    		AutomaticSizing::TechnologieFileSHM technologieFile = getLocalOptions().getTechnologieFileSHM();
    		technologieFile.parse(*circuitInformation);
    	}
    	else if(getLocalOptions().getTransistorModel() == "EKV")
		{
    		AutomaticSizing::TechnologieFileEKV technologieFile = getLocalOptions().getTechnologieFileEKV();
    		technologieFile.parse(*circuitInformation);
		}

    	return circuitInformation;
    }

    const Result & Synthesis::getResult() const
    {
    	assert(result_ != nullptr);
    	return *result_;
    }

    const Core::DeviceTypeRegister & Synthesis::getDeviceTypeRegister() const
    {
    	assert(deviceTypeRegister_ != nullptr);
    	return *deviceTypeRegister_;
    }

}




