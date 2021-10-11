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
 *************************************************************************************************/#include "Synthesis/incl/Library/FunctionalBlockLibrary.h"
#include "Synthesis/incl/Library/Circuit.h"
#include "Synthesis/incl/Library/HierarchyLevel1/DeviceLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel3/AmplificationStagesSubBlockLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel4/AmplificationStageLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel5/OpAmps.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Log/incl/LogMacros.h"

#include <sstream> 

namespace Synthesis {

	FunctionalBlockLibrary::FunctionalBlockLibrary(const AutomaticSizing::CircuitInformation & circuitInformation):
            deviceLevel_(nullptr),
            structuralLevel_(nullptr),
            amplificationStagesSubBlockLevel_(nullptr),
            amplificationStageLevel_(nullptr),
            opAmps_(nullptr),
            structRecLibrary_(nullptr)
    {
        logDebug("Creating device level");
        deviceLevel_ = new DeviceLevel();
        logDebug("Creating Structural level");
        structuralLevel_ = new StructuralLevel(getDeviceLevel(), circuitInformation);
        logDebug("Creating amplification stages sub block level");
        amplificationStagesSubBlockLevel_ = new AmplificationStagesSubBlockLevel(getStructuralLevel(), circuitInformation);
        logDebug("Creating amplification stage level");
        amplificationStageLevel_ = new AmplificationStageLevel(getAmplificationStagesSubBlockLevel(), getStructuralLevel(), circuitInformation);
        logDebug("Creating Op Amps");
        opAmps_ = new OpAmps(getAmplificationStageLevel(), getStructuralLevel().getCurrentBiases(), getStructuralLevel().getVoltageBiases(), getDeviceLevel().getCapacitor());

    }
	FunctionalBlockLibrary::~FunctionalBlockLibrary()
    {
		logDebug("Delete device level");
        delete deviceLevel_;
        logDebug("Delete structural level");
        delete structuralLevel_;
        logDebug("Delete amplification stage subblock level");
        delete amplificationStagesSubBlockLevel_;
        logDebug("Delete amplification stage level");
        delete amplificationStageLevel_;
        logDebug("Delete op amps");
        delete opAmps_;
    }

    void FunctionalBlockLibrary::setStructRecLibrary(StructRec::Library& library)
    {
        structRecLibrary_ = & library;
    }


	std::string FunctionalBlockLibrary::toStr() const
    {
        std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< FUNCTIONAL BLOCK LIBRARY >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
//        oss << getDeviceLevel().toStr() << std::endl;
//        oss << getStructuralLevel().toStr() << std::endl;
//        oss << getAmplificationStagesSubBlockLevel().toStr() << std::endl;
//        oss << getAmplificationStageLevel().toStr() << std::endl;
        oss << printOpAmps() << std::endl;

        return oss.str();
    }

	std::string FunctionalBlockLibrary::printOpAmps() const
	{
		//Writting all op-amps needs lots of space as especially many two stage op-amp exist (Printing all simple two stage op-amps needs more than 8 GB RAM or a different algoritm which must be written...)
		std::ostringstream oss;

		int caseNumber = 1; //neu
		int amountOfOpAmps = 1; //neu
		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SIMPLE OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createSimpleOneStageOpAmps(caseNumber, amountOfOpAmps))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss << std::endl;

		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FULLY DIFFERENTIAL OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createFullyDifferentialOneStageOpAmps(caseNumber, amountOfOpAmps))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss << std::endl;

		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> COMPLEMENTARY OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createComplementaryOpAmps(caseNumber, amountOfOpAmps))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss <<std::endl;

		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SYMMETRICAL OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createSymmetricalOpAmps(caseNumber, amountOfOpAmps))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss <<std::endl;

		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SIMPLE TWO STAGE OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createSimpleTwoStageOpAmps(createSimpleOneStageOpAmps(caseNumber, amountOfOpAmps)))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss <<std::endl;

		oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FULLY DIFFERENTIAL TWO STAGE OP AMPS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		for(auto & opAmp : createFullyDifferentialTwoStageOpAmps(createFullyDifferentialOneStageOpAmps(caseNumber, amountOfOpAmps)))
		{
			oss << ">>>>>>>>>>>>> " << opAmp->getFlatCircuit().getCircuitIdentifier().toStr() << ">>>>>>>>>>>>>>>>>>" << std::endl;
			oss << opAmp->toStr() << std::endl;
		}
		oss <<std::endl;

		return oss.str();
	}

	std::vector<Circuit*> FunctionalBlockLibrary::createSimpleOneStageOpAmps(int caseNumber, int & index) const
    {
    	auto start = std::chrono::high_resolution_clock::now();

		std::vector<Circuit*> oneStageOpAmps;
		logDebug("Create simple op amps;")
		std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createSimpleOneStageOpAmps(caseNumber, index);
		logDebug("Finishing creating simple op amps");

		auto end = std::chrono::high_resolution_clock::now();
		auto diff = end - start;
		int timeParaMinutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
		int timeParaSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count() - timeParaMinutes * 60;
		logDebug("\nRuntime Create simple op amps: " << timeParaMinutes <<"min " << timeParaSeconds << "s");
        
		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());

				oneStageOpAmps.push_back(opAmp);
		}

		return oneStageOpAmps;
    }
		
    std::vector<Circuit*> FunctionalBlockLibrary::createFullyDifferentialOneStageOpAmps(int caseNumber, int & index) const
    {
    	auto start = std::chrono::high_resolution_clock::now();
		std::vector<Circuit*> oneStageOpAmps;
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createFullyDifferentialOneStageOpAmps(caseNumber, index);
        
		auto end = std::chrono::high_resolution_clock::now();
		auto diff = end - start;
		int timeParaMinutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
		int timeParaSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count() - timeParaMinutes * 60;
		logDebug("\nRuntime Create FullyDifferential: " << timeParaMinutes <<"min " << timeParaSeconds << "s");

		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				oneStageOpAmps.push_back(opAmp);
		}
		return oneStageOpAmps;
    }
		
    std::vector<Circuit*> FunctionalBlockLibrary::createComplementaryOpAmps(int caseNumber, int & index) const
    {
    	auto start = std::chrono::high_resolution_clock::now();

		std::vector<Circuit*> oneStageOpAmps;

        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createComplementaryOpAmps(caseNumber,index);

        auto end = std::chrono::high_resolution_clock::now();
		auto diff = end - start;
		int timeParaMinutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
		int timeParaSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count() - timeParaMinutes * 60;
		logDebug("\nRuntime Create Complementary: " << timeParaMinutes <<"min " << timeParaSeconds << "s");

		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				oneStageOpAmps.push_back(opAmp);
		}

		return oneStageOpAmps;
    }
		
    std::vector<Circuit*> FunctionalBlockLibrary::createSymmetricalOpAmps(int caseNumber, int & index) const
    {
        std::vector<Circuit*> oneStageOpAmps;
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createSymmetricalOpAmps(caseNumber, index);

		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				oneStageOpAmps.push_back(opAmp);
		}

		return oneStageOpAmps;
    }
		
    std::vector<Circuit*> FunctionalBlockLibrary::createSimpleTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const
    {
        std::vector<Circuit*> twoStageOpAmps;
        std::vector<const Core::Circuit *> hierarchicalOneStageOpAmpCircuits = extractHierachicalOneStageOpAmpCircuits(oneStageOpAmps);
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createSimpleTwoStageOpAmps(hierarchicalOneStageOpAmpCircuits);
        
		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				twoStageOpAmps.push_back(opAmp);
		}

		return twoStageOpAmps;
    }

    std::vector<Circuit*> FunctionalBlockLibrary::createSimpleTwoStageOpAmps(const Circuit & oneStageOpAmp, std::mutex &myMutex) const
	{
        std::vector<Circuit*> twoStageOpAmps;
        const Core::Circuit & hierarchicalOneStageOpAmp = oneStageOpAmp.getHierarchicalCircuit();
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createSimpleTwoStageOpAmps(hierarchicalOneStageOpAmp, std::ref(myMutex));

		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				twoStageOpAmps.push_back(opAmp);
		}

		return twoStageOpAmps;
	}
		
    std::vector<Circuit*> FunctionalBlockLibrary::createFullyDifferentialTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const
    {
        std::vector<Circuit*> twoStageOpAmps;
        std::vector<const Core::Circuit *> hierarchicalOneStageOpAmpCircuits = extractHierachicalOneStageOpAmpCircuits(oneStageOpAmps);
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createFullyDifferentialTwoStageOpAmps(hierarchicalOneStageOpAmpCircuits);
        
		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				twoStageOpAmps.push_back(opAmp);
		}
		return twoStageOpAmps;
    }


	std::vector<Circuit*> FunctionalBlockLibrary::createFullyDifferentialTwoStageOpAmps(const Circuit &  oneStageOpAmp, std::mutex &myMutex) const
    {
        std::vector<Circuit*> twoStageOpAmps;
        const Core::Circuit & hierarchicalOneStageOpAmpCircuit = oneStageOpAmp.getHierarchicalCircuit();
        std::vector<const Core::Circuit *> opAmpCircuits = getOpAmps().createFullyDifferentialTwoStageOpAmps(hierarchicalOneStageOpAmpCircuit, std::ref(myMutex));

		for(auto & circuit : opAmpCircuits)
		{
				Circuit * opAmp = new Circuit;
				opAmp->initialize(*circuit, getStructRecLibrary());
				twoStageOpAmps.push_back(opAmp);
		}
		return twoStageOpAmps;
    }

    const DeviceLevel & FunctionalBlockLibrary::getDeviceLevel() const
    {
        assert(deviceLevel_ != nullptr);
        return * deviceLevel_;
    }

	const StructuralLevel & FunctionalBlockLibrary::getStructuralLevel() const
    {
        assert(structuralLevel_ != nullptr);
        return *structuralLevel_;
    }

	const AmplificationStagesSubBlockLevel & FunctionalBlockLibrary::getAmplificationStagesSubBlockLevel() const
    {
        assert(amplificationStagesSubBlockLevel_ != nullptr);
        return * amplificationStagesSubBlockLevel_;
    }
	const AmplificationStageLevel & FunctionalBlockLibrary::getAmplificationStageLevel() const
    {
        assert(amplificationStageLevel_ != nullptr);
        return * amplificationStageLevel_;
    }



    OpAmps & FunctionalBlockLibrary::getOpAmps() const
    {
        assert(opAmps_ != nullptr);
        return * opAmps_;
    }

    StructRec::Library & FunctionalBlockLibrary::getStructRecLibrary() const
    {
        assert(structRecLibrary_ != nullptr);
        return * structRecLibrary_;

    }

    std::vector<const Core::Circuit *> FunctionalBlockLibrary::extractHierachicalOneStageOpAmpCircuits(std::vector<Circuit*> oneStageOpAmps) const
    {
        std::vector<const Core::Circuit *> firstStages;
        for(auto & oneStageOpAmp: oneStageOpAmps)
        {
            const Core::Circuit & circuit = oneStageOpAmp->getHierarchicalCircuit();
            firstStages.push_back(&circuit);
        }
        return firstStages;
    }



}
