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

#include "AutomaticSizing/incl/AutomaticSizingAnalysis.h"
#include "AutomaticSizing/incl/CircuitInformationFiles/TechnologieFileEKV.h"
#include "AutomaticSizing/incl/ConstraintProgram/AutomaticSizing.h"
#include "AutomaticSizing/incl/Results/Result.h"
#include "Partitioning/incl/Partitioning.h"
#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Results/Result.h"
#include "HSpice/incl/OutputFile/OutputFile.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <time.h>


namespace AutomaticSizing {

    AutomaticSizingAnalysis::AutomaticSizingAnalysis() :
					Control::CircuitAnalysis<LocalOptions>(),
					structRecResult_(NULL),
					result_(NULL),
					partitioningResult_(nullptr)
    {
    }

    AutomaticSizingAnalysis::~AutomaticSizingAnalysis()
    {
    	delete structRecResult_;
    	delete result_;
    	delete partitioningResult_;
    }

    void AutomaticSizingAnalysis::compute()
    {
       runAutomaticSizing();
    }

    void AutomaticSizingAnalysis::write()
   {
    	if(hasOutputFile())
    	{
    	  	getResult().writeXmlAutomaticSizingResult(getOutputFile());
    	}

    }

    void AutomaticSizingAnalysis::runAutomaticSizing()
    {

    	StructRec::Library* structRecLibrary = parseStructRecLibrary();
		runStructureRecognition(*structRecLibrary);
		logDebug("<<<<<<<<<<<<<<<< Structure Recognition Results >>>>>>>>>>>>>>>>>>>>");
		logDebug(getStructureRecognitionResult());


		CircuitInformation & circuitInformation = parseCircuitInformation();

		Partitioning::Partitioning * partioning = new Partitioning::Partitioning;
		partitioningResult_ = &partioning->compute(getStructureRecognitionResult().getTopLevelResults());

		logDebug("<<<<<<<<<<<<<<<< Partitioning Results >>>>>>>>>>>>>>>>>>>>");
		logDebug(partitioningResult_);

		delete partioning;
		delete structRecLibrary;

		AutomaticSizing * automaticSizing = new AutomaticSizing;
		automaticSizing->setPartitioningResult(getPartitioningResult());
		automaticSizing->setCircuitInformation(circuitInformation);
		automaticSizing->setStructureRecognitionResult(getStructureRecognitionResult().getTopLevelResults());
		automaticSizing->setTransistorModel(getLocalOptions().getTransistorModel());
		automaticSizing->setDefinition(getLocalOptions().getScaling());
		automaticSizing->setRuntime(getLocalOptions().getRuntime());
		result_ = &automaticSizing->compute();
		logDebug("<<<<<<<<<<<<<<<< Final Result After Sizing >>>>>>>>>>>>>>>>>>>>");
		logDebug(result_);

		delete automaticSizing;
		delete &circuitInformation;
    }

    const StructRec::Result& AutomaticSizingAnalysis::getStructureRecognitionResult()
    {
    	assert(hasStructureRecognitionResult());
    	return * structRecResult_;
    }

    bool AutomaticSizingAnalysis::hasStructureRecognitionResult() const
    {
    	return structRecResult_ != NULL;
    }

    StructRec::Library* AutomaticSizingAnalysis::parseStructRecLibrary() const
    {
   		StructRec::LibraryFile structRecLibraryFile = getLocalOptions().getLibraryFile();
   		structRecLibraryFile.setDeviceTypeRegister(getDeviceTypeRegister());
   		StructRec::Library* structRecLibrary = structRecLibraryFile.parse();
       	return structRecLibrary;
    }

    CircuitInformation& AutomaticSizingAnalysis::parseCircuitInformation() const
    {
    	CircuitInformation * circuitInformation = new CircuitInformation;
    	if(getLocalOptions().getTransistorModel() == "SHM")
    	{
    		TechnologieFileSHM technologieFile = getLocalOptions().getTechnologieFileSHM();
    		technologieFile.parse(*circuitInformation);
    		assert(circuitInformation->hasTechnologieSpecificationSHMNmos() && circuitInformation->hasTechnologieSpecificationSHMPmos(), "Error while  passing Technology file. Wrong file path or wrong syntax!");
    	}
    	else if(getLocalOptions().getTransistorModel() == "EKV")
		{
    		TechnologieFileEKV technologieFile = getLocalOptions().getTechnologieFileEKV();
    		technologieFile.parse(*circuitInformation);
    		assert(circuitInformation->hasTechnologieSpecificationEKVNmos() && circuitInformation->hasTechnologieSpecificationEKVPmos(), "Error while  passing Technology file. Wrong file path or wrong syntax!");

		}
    	CircuitInformationFile circuitInformationFile = getLocalOptions().getCircuitInformationFile();
    	circuitInformationFile.parse(*circuitInformation);
    	assert(circuitInformation->hasCircuitParameter() && circuitInformation->hasCircuitSpecification() && circuitInformation->hasDesignAttributes(), "Error while  passing circuit parameter and specification file. Wrong file path or wrong syntax!");


    	return * circuitInformation;
    }

    void AutomaticSizingAnalysis::runStructureRecognition(StructRec::Library & library)
    {
    	structRecResult_ = library.recognize(getCircuit());
    }

    const Result& AutomaticSizingAnalysis::getResult() const
    {
    	assert(hasResult());
    	return *result_;
    }

    const Partitioning::Result& AutomaticSizingAnalysis::getPartitioningResult() const
    {
    	assert(hasPartitioningResult());
    	return *partitioningResult_;
    }


    bool AutomaticSizingAnalysis::hasResult() const
    {
    	return result_ != NULL;
    }

    bool AutomaticSizingAnalysis::hasPartitioningResult() const
    {
    	return partitioningResult_ != NULL;
    }


}




