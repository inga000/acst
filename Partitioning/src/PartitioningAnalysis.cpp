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
#include "Partitioning/incl/PartitioningAnalysis.h"
#include "Partitioning/incl/Partitioning.h"
#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/Results/Result.h"
#include "HSpice/incl/OutputFile/OutputFile.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include <time.h>

namespace Partitioning {

    PartitioningAnalysis::PartitioningAnalysis() :
		            Control::FlatCircuitAnalysis<LocalOptions>(),

					structRecResult_(nullptr),
					result_(nullptr)
    {
    }

    PartitioningAnalysis::~PartitioningAnalysis()
    {
    	if(result_ != nullptr) {
    		delete result_;
    	}

    }

    void PartitioningAnalysis::compute()
    {
    	result_ = new Result();

		runStructureRecognition();
		logDebug("<<<<<<<<<<<<<< Structure Recognition Result >>>>>>>>>>>>>>>");
		logDebug(getStructRecResult());

		Partitioning * partioning = new Partitioning;
		result_ = &partioning->compute(getStructRecResult().getTopLevelResults());
		logDebug("<<<<<<<<<<<<<< Partitioning Result >>>>>>>>>>>>>>>");
		logDebug(*result_);

		delete partioning;
    }

    void PartitioningAnalysis::write()
    {
    	if(hasOutputFile())
    	{
    		getResult().writeXmlPartitioningResult(getOutputFile());
        }
    }

    void PartitioningAnalysis::runStructureRecognition()
    {
    	StructRec::LibraryFile libraryFile = getLocalOptions().getLibraryFile();
    	libraryFile.setDeviceTypeRegister(getDeviceTypeRegister());
    	StructRec::Library & structureLibrary = *libraryFile.parse();
    	structRecResult_ = structureLibrary.recognize(getFlatCircuit());
    	delete & structureLibrary;
    }

    const Result& PartitioningAnalysis::getResult() const
    {
    	assert(result_ != nullptr);
    	return *result_;
    }


    const StructRec::Result& PartitioningAnalysis::getStructRecResult() const
    {
    	assert(structRecResult_ != nullptr);
    	return *structRecResult_;
    }

}




