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


#include "../../incl/Options/CircuitReadInOptions.h"
#include "Control/incl/FactoryMap/AnalysesFactoryMap.h"
#include "HSpice/incl/InputFile/InputFile.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetFile.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Common/BacktraceAssert.h"


namespace Control {

    const std::string CircuitReadInOptions::OUTPUT_FILE_OPTION_ = "output-file";
    const std::string CircuitReadInOptions::OUTPUT_FILE_DESCRIPTION_ = "path of the output file";
    const std::string CircuitReadInOptions::CIRCUIT_NETLIST_OPTION_ = "circuit-netlist";
    const std::string CircuitReadInOptions::CIRCUIT_NETLIST_DESCRIPTION_ = "path to input circuit netlist to be analysized";





    CircuitReadInOptions::CircuitReadInOptions(const std::string& description) :
            HspiceOptions(description)
    {
    	initialize();
    }



    bool CircuitReadInOptions::hasOutputFile() const
    {
        return hasOptionValue(OUTPUT_FILE_OPTION_);
    }

    OutputFile CircuitReadInOptions::findOutputFile() const
    {
        assert(hasOutputFile());

        std::string outputFilePath =  findStringOptionValue(OUTPUT_FILE_OPTION_);
        OutputFile outputFile;
        outputFile.setPath(outputFilePath);
        return outputFile;
    }


    void CircuitReadInOptions::initialize()
    {
    	Control::OptionsBase::addStringOption(CIRCUIT_NETLIST_OPTION_, CIRCUIT_NETLIST_DESCRIPTION_);
    	Control::OptionsBase::addStringOption(OUTPUT_FILE_OPTION_, OUTPUT_FILE_DESCRIPTION_);
    }

	Core::Circuit & CircuitReadInOptions::readInCircuit(const Core::DeviceTypeRegister & deviceTypeRegister) const
	{
		 std::string filePath = findStringOptionValue(CIRCUIT_NETLIST_OPTION_);

		 HSpice::InputFile::InputFile* hspiceInputFile = new HSpice::InputFile::InputFile();

	     hspiceInputFile->setPath(filePath);
	     hspiceInputFile->setSupplyNetIds(readSupplyNetIdentifiers());

	     hspiceInputFile->setDeviceTypeRegister(deviceTypeRegister);
	     hspiceInputFile->setDeviceLineMapper(*readDeviceLineMapper());

	     Core::Circuit & circuit = *hspiceInputFile->readNewCircuit();
	     delete hspiceInputFile;
	     return circuit;

	}

	bool CircuitReadInOptions::isCircuitReadInOption() const
	{
		return true;
	}

}


