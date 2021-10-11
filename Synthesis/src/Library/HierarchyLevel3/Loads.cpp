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
#include "Synthesis/incl/Library/HierarchyLevel3/Loads.h"
#include "Synthesis/incl/Library/HierarchyLevel3/AmplificationStagesSubBlockLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"


#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Core/incl/Common/BacktraceAssert.h"


#include <sstream>


namespace Synthesis {

    const Core::TerminalName Loads::OUT1_TERMINAL_ = Core::TerminalName("Out1");
	const Core::TerminalName Loads::OUT2_TERMINAL_ = Core::TerminalName("Out2");

	const Core::TerminalName Loads::SOURCELOAD1_TERMINAL_ = Core::TerminalName("SourceLoad1");
	const Core::TerminalName Loads::SOURCELOAD2_TERMINAL_ = Core::TerminalName("SourceLoad2");

	const Core::TerminalName Loads::SOURCEGCC1_TERMINAL_ = Core::TerminalName("SourceGCC1");
	const Core::TerminalName Loads::SOURCEGCC2_TERMINAL_ = Core::TerminalName("SourceGCC2");

	const Core::TerminalName Loads::INNERLOAD1_TERMINAL_ = Core::TerminalName("innerLoad1");
	const Core::TerminalName Loads::INNERLOAD2_TERMINAL_ = Core::TerminalName("InnerLoad2");
	const Core::TerminalName Loads::INNERGCC_TERMINAL_ = Core::TerminalName("InnerGCC");
	const Core::TerminalName Loads::INNERBIASGCC_TERMINAL_ = Core::TerminalName("InnerBiasGCC");
	const Core::TerminalName Loads::INNERSOURCELOAD1_TERMINAL_ = Core::TerminalName("InnerSourceLoad1");			
	const Core::TerminalName Loads::INNEROUTPUTLOAD1_TERMINAL_ = Core::TerminalName("InnerOutputLoad1");
	const Core::TerminalName Loads::INNERSOURCELOAD2_TERMINAL_ = Core::TerminalName("InnerSourceLoad2");
	const Core::TerminalName Loads::INNEROUTPUTLOAD2_TERMINAL_ = Core::TerminalName("InnerOutputLoad2");
	const Core::TerminalName Loads::INNERTRANSISTORSTACK1LOAD1_TERMINAL_ = Core::TerminalName("InnerTransistorStack1Load1");
	const Core::TerminalName Loads::INNERTRANSISTORSTACK2LOAD1_TERMINAL_ = Core::TerminalName("InnerTransistorStack2Load1");
	const Core::TerminalName Loads::INNERTRANSISTORSTACK1LOAD2_TERMINAL_ = Core::TerminalName("InnerTransistorStack1Load2");
	const Core::TerminalName Loads::INNERTRANSISTORSTACK2LOAD2_TERMINAL_ = Core::TerminalName("InnerTransistorStack2Load2");

	const Core::TerminalName Loads::OUTOUTPUT1LOAD1_TERMINAL_ = Core::TerminalName("OutOutput1Load1");
	const Core::TerminalName Loads::OUTOUTPUT2LOAD1_TERMINAL_ = Core::TerminalName("OutOutput2Load1");
	const Core::TerminalName Loads::OUTSOURCE1LOAD1_TERMINAL_ = Core::TerminalName("OutSourceLoad1");
    const Core::TerminalName Loads::OUTSOURCE2LOAD1_TERMINAL_ = Core::TerminalName("OutSourceLoad2");

    const Core::InstanceName Loads::LOADPART1_ = Core::InstanceName("LoadPart1");
	const Core::InstanceName Loads::LOADPART2_ = Core::InstanceName("LoadPart2");
			
	const Core::NetId Loads::OUT1_NET_ = Core::NetName("out1").createRootIdentifier();
	const Core::NetId Loads::OUT2_NET_ = Core::NetName("out2").createRootIdentifier();

	const Core::NetId Loads::SOURCELOAD1_NET_ = Core::NetName("sourceLoad1").createRootIdentifier();
	const Core::NetId Loads::SOURCELOAD2_NET_ = Core::NetName("sourceLoad2").createRootIdentifier();

	const Core::NetId Loads::SOURCEGCC1_NET_ = Core::NetName("sourceGCC1").createRootIdentifier();
	const Core::NetId Loads::SOURCEGCC2_NET_ = Core::NetName("sourceGCC2").createRootIdentifier();

	const Core::NetId Loads::INNERLOAD1_NET_ = Core::NetName("innerLoad1").createRootIdentifier();
	const Core::NetId Loads::INNERLOAD2_NET_ = Core::NetName("innerLoad2").createRootIdentifier();
	const Core::NetId Loads::INNERGCC_NET_ = Core::NetName("innerGCC").createRootIdentifier();
	const Core::NetId Loads::INNERBIASGCC_NET_ = Core::NetName("innerBiasGCC").createRootIdentifier();
	const Core::NetId Loads::INNERSOURCELOAD1_NET_ = Core::NetName("innerSourceLoad1").createRootIdentifier();			
	const Core::NetId Loads::INNEROUTPUTLOAD1_NET_ = Core::NetName("innerOutputLoad1").createRootIdentifier();
	const Core::NetId Loads::INNERSOURCELOAD2_NET_ = Core::NetName("innerSourceLoad2").createRootIdentifier();
	const Core::NetId Loads::INNEROUTPUTLOAD2_NET_ = Core::NetName("innerOutputLoad2").createRootIdentifier();
	const Core::NetId Loads::INNERTRANSISTORSTACK1LOAD1_NET_ = Core::NetName("innerTransistorStack1Load1").createRootIdentifier();
	const Core::NetId Loads::INNERTRANSISTORSTACK2LOAD1_NET_ = Core::NetName("innerTransistorStack2Load1").createRootIdentifier();
	const Core::NetId Loads::INNERTRANSISTORSTACK1LOAD2_NET_ = Core::NetName("innerTransistorStack1Load2").createRootIdentifier();
	const Core::NetId Loads::INNERTRANSISTORSTACK2LOAD2_NET_ = Core::NetName("innerTransistorStack2Load2").createRootIdentifier();

	const Core::NetId Loads::OUTOUTPUT1LOAD1_NET_ = Core::NetName("outOutput1Load1").createRootIdentifier();
	const Core::NetId Loads::OUTOUTPUT2LOAD1_NET_ = Core::NetName("outOutput2Load1").createRootIdentifier();
	const Core::NetId Loads::OUTSOURCE1LOAD1_NET_ = Core::NetName("outSource1Load1").createRootIdentifier();
    const Core::NetId Loads::OUTSOURCE2LOAD1_NET_ = Core::NetName("outSource2Load1").createRootIdentifier();

    Loads::Loads(AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel)
    {
		amplificationStagesSubBlockLevel_ = & amplificationStagesSubBlockLevel;

        int pmosIndex = 1;
        int nmosIndex = 1;
		initializeLoadsVoltageBiasesLoadPart(amplificationStagesSubBlockLevel, nmosIndex, pmosIndex);
    }
	
    Loads::~Loads()
    {
		eraseOneLoadPartLoadsVoltageBiasesNmos();
		eraseOneLoadPartLoadsVoltageBiasesPmos();
    }

    void Loads::eraseTemporaryFunctionalblocks() const
    {
		for (const Core::Circuit* temporaryElement: temporaryLoads_)
		{
			if(temporaryElement)
			{
				delete temporaryElement;
			}
		}
    }

	const AmplificationStagesSubBlockLevel & Loads::getAmplificationStagesSubBlockLevel() const
	{
		assert(amplificationStagesSubBlockLevel_ != nullptr);
		return * amplificationStagesSubBlockLevel_;
	}

	std::vector<const Core::Circuit *> Loads::getTemporaryLoads() const
	{
		return temporaryLoads_;
	}

	std::vector<const Core::Circuit*>  Loads::createSimpleMixedLoadPmos() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosMixed();
		loads = createOneLoadPartLoads(pmosLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createSimpleMixedLoadNmos() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> nmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosMixed();
		loads = createOneLoadPartLoads(nmosLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createSimpleTwoLoadPartsFoldedGCCMixedLoadPmos() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosGCCLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosFourTransistorCurrentBiases();
		std::vector<const Core::Circuit *> nmosSecondLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosMixed();
		loads = createTwoLoadPartLoadsWithGCC(pmosGCCLoadParts, nmosSecondLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createSimpleTwoLoadPartsFoldedGCCMixedLoadNmos() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> nmosGCCLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosFourTransistorCurrentBiases();
		std::vector<const Core::Circuit *> pmosSecondLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosMixed();
		loads = createTwoLoadPartLoadsWithGCC(nmosGCCLoadParts, pmosSecondLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsTwoLoadPartsCascodeGCCMixedPmos() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *>  pmosGCCLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosTwoTransistorCurrentBiasesDifferentSources();
		std::vector<const Core::Circuit *>  nmosSecondLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosMixed();
		loads = createTwoLoadPartLoadsWithGCC(pmosGCCLoadParts, nmosSecondLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsTwoLoadPartsCascodeGCCMixedNmos() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *>  nmosGCCLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosTwoTransistorCurrentBiasesDifferentSources();
		std::vector<const Core::Circuit *>  pmosSecondLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosMixed();
		loads = createTwoLoadPartLoadsWithGCC(nmosGCCLoadParts, pmosSecondLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsTwoLoadPartsMixedCurrentBiasesPmos() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> nmosCurrentBiasLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosCurrentBiases();
		std::vector<const Core::Circuit *> pmosMixedLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosMixed();
		loads = createTwoLoadPartLoadsWithoutGCC(pmosMixedLoadParts, nmosCurrentBiasLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsTwoLoadPartsMixedCurrentBiasesNmos() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosCurrentBiasLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosCurrentBiases();
		std::vector<const Core::Circuit *> nmosMixedLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosMixed();
		loads = createTwoLoadPartLoadsWithoutGCC(nmosMixedLoadParts, pmosCurrentBiasLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsPmosForFullyDifferentialNonInvertingStage() const
	{
		int index = 1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit*> oneLoadPartLoadsCurrentBiasesPmos = createOneLoadPartLoads(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosCurrentBiases(), index);
		std::vector<const Core::Circuit*> twoLoadPartLoadsOnlyCurrentBiasesFoldedPmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosFourTransistorCurrentBiases(),
						getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosCurrentBiases(), index);
		std::vector<const Core::Circuit*> twoLoadPartLoadsOnlyCurrentBiasesCascodeNmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosTwoTransistorCurrentBiasesDifferentSources(),
				getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosCurrentBiases(), index);

		loads.insert(loads.end(), oneLoadPartLoadsCurrentBiasesPmos.begin(), oneLoadPartLoadsCurrentBiasesPmos.end());
		loads.insert(loads.end(), twoLoadPartLoadsOnlyCurrentBiasesFoldedPmosGCC.begin(), twoLoadPartLoadsOnlyCurrentBiasesFoldedPmosGCC.end());
		loads.insert(loads.end(), twoLoadPartLoadsOnlyCurrentBiasesCascodeNmosGCC.begin(), twoLoadPartLoadsOnlyCurrentBiasesCascodeNmosGCC.end());
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsNmosForFullyDifferentialNonInvertingStage() const
	{
		int index = 1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit*> oneLoadPartLoadsCurrentBiasesNmos = createOneLoadPartLoads(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosCurrentBiases(), index);
		std::vector<const Core::Circuit*> twoLoadPartLoadsOnlyCurrentBiasesFoldedNmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosFourTransistorCurrentBiases(),
				getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosCurrentBiases(), index);
		std::vector<const Core::Circuit*> twoLoadPartLoadsOnlyCurrentBiasesCascodePmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosTwoTransistorCurrentBiasesDifferentSources(),
				getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosCurrentBiases(), index);

		loads.insert(loads.end(), oneLoadPartLoadsCurrentBiasesNmos.begin(), oneLoadPartLoadsCurrentBiasesNmos.end());
		loads.insert(loads.end(), twoLoadPartLoadsOnlyCurrentBiasesFoldedNmosGCC.begin(), twoLoadPartLoadsOnlyCurrentBiasesFoldedNmosGCC.end());
		loads.insert(loads.end(), twoLoadPartLoadsOnlyCurrentBiasesCascodePmosGCC.begin(), twoLoadPartLoadsOnlyCurrentBiasesCascodePmosGCC.end());
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsForComplementaryNonInvertingStage() const
	{
		int index =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosFourTransistorMixed();
		std::vector<const Core::Circuit *> nmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosFourTransistorMixed();
		loads = createSymmetricalLoadsFourTransistorMixedLoadParts(pmosLoadParts, nmosLoadParts);

		std::vector<const Core::Circuit *> twoLoadPartLoadsMixedFoldedPmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosFourTransistorCurrentBiases(),
				getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosMixed(), index);
		std::vector<const Core::Circuit *> twoLoadPartLoadsMixedFoldedNmosGCC = createTwoLoadPartLoadsWithGCC(getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsNmosFourTransistorCurrentBiases(),
				getAmplificationStagesSubBlockLevel().getLoadParts().createLoadPartsPmosMixed(), index);

		for(auto & load : twoLoadPartLoadsMixedFoldedNmosGCC)
		{
			if(getDeviceNamesOfFlatCircuit(*load).size() == 8)
			{
				loads.push_back(load);
			}
		}

		for(auto & load : twoLoadPartLoadsMixedFoldedPmosGCC)
		{
			if(getDeviceNamesOfFlatCircuit(*load).size() == 8)
			{
				loads.push_back(load);
			}
		}
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsPmosTwoForSymmetricalOpAmpNonInvertingStage() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createTwoTransistorsLoadPartsLoadPartsPmosVoltageBiases();
		loads = createOneLoadPartLoads(pmosLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsPmosFourForSymmetricalOpAmpNonInvertingStage() const
	{
		int pmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> pmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createFourTransistorsLoadPartsLoadPartsPmosVoltageBiases();
		loads = createOneLoadPartLoads(pmosLoadParts, pmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsNmosTwoForSymmetricalOpAmpNonInvertingStage() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> nmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createTwoTransistorsLoadPartsLoadPartsNmosVoltageBiases();
		loads = createOneLoadPartLoads(nmosLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::createLoadsNmosFourForSymmetricalOpAmpNonInvertingStage() const
	{
		int nmosIndex =1;
		std::vector<const Core::Circuit*> loads;
		std::vector<const Core::Circuit *> nmosLoadParts = getAmplificationStagesSubBlockLevel().getLoadParts().createFourTransistorsLoadPartsLoadPartsNmosVoltageBiases();
		loads = createOneLoadPartLoads(nmosLoadParts, nmosIndex);
		getTemporaryLoads() = loads;
		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::getLoadsPmosForFeedbackNonInvertingStage() const
	{
		std::vector<const Core::Circuit *> loads;
		for(auto & voltageBiasLoad : oneLoadPartLoadsVoltageBiasesPmos_)
		{
			std::vector<Core::DeviceId> devicesInCircuit = getDeviceNamesOfFlatCircuit(*voltageBiasLoad);
			if(devicesInCircuit.size() == 2)
			{
				loads.push_back(voltageBiasLoad);
				break;
			}
		}
		assert(loads.size() == 1, "There must be a voltage bias load with only two diode transustors");

		return loads;
	}

	std::vector<const Core::Circuit*>  Loads::getLoadsNmosForFeedbackNonInvertingStage() const
	{
		std::vector<const Core::Circuit *> loads;
		for(auto & voltageBiasLoad : oneLoadPartLoadsVoltageBiasesNmos_)
		{
			std::vector<Core::DeviceId> devicesInCircuit = getDeviceNamesOfFlatCircuit(*voltageBiasLoad);
			if(devicesInCircuit.size() == 2)
			{
				loads.push_back(voltageBiasLoad);
				break;
			}
		}
		assert(loads.size() == 1, "There must be a voltage bias load with only two diode transustors");

		return loads;
	}

	std::string Loads::toStr() const
    {
        std::ostringstream oss;
		Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Loads:" << std::endl;

		int caseNumber = 1;
		std::vector<const Core::Circuit*> loads;
		do
		{
			switch(caseNumber)
			{
			case 1: {
				loads = createSimpleMixedLoadPmos();
				break;
			}
			case 2: {
				loads = createSimpleMixedLoadNmos();
				break;
			}
			case 3: {
				loads = createSimpleTwoLoadPartsFoldedGCCMixedLoadPmos();
				break;
			}
			case 4: {
				loads = createSimpleTwoLoadPartsFoldedGCCMixedLoadNmos();
				break;
			}
			case 5: {
				loads = createLoadsTwoLoadPartsCascodeGCCMixedPmos();
				break;
			}
			case 6: {
				loads = createLoadsTwoLoadPartsCascodeGCCMixedNmos();
				break;
			}
			case 7: {
				loads = createLoadsTwoLoadPartsMixedCurrentBiasesPmos();
				break;
			}
			case 8: {
				loads = createLoadsTwoLoadPartsMixedCurrentBiasesNmos();
				break;
			}
			case 9: {
				loads = createLoadsPmosForFullyDifferentialNonInvertingStage();
				break;
			}
			case 10: {
				loads = createLoadsNmosForFullyDifferentialNonInvertingStage();
				break;
			}
			case 11: {
				loads = createLoadsForComplementaryNonInvertingStage();
				break;
			}
			case 12: {
				loads = createLoadsPmosTwoForSymmetricalOpAmpNonInvertingStage();
				break;
			}
			case 13: {
				loads = createLoadsPmosFourForSymmetricalOpAmpNonInvertingStage();
				break;
			}
			case 14: {
				loads = createLoadsNmosTwoForSymmetricalOpAmpNonInvertingStage();
				break;
			}
			case 15: {
				loads = createLoadsNmosFourForSymmetricalOpAmpNonInvertingStage();
				break;
			}
			case 16: {
				loads.clear();
				break;
			}
			default: {
				break;
			}
			}

			for(auto & load : loads)
			{
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << load->getCircuitIdentifier().toStr() << ": " << std::endl;
	            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*load);
	            oss << load->toStr() << std::endl;
	            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	            oss << flatCircuit->toStr() << std::endl;
	            delete flatCircuit;
			}
        	caseNumber++;
		} while (!loads.empty());

		oss << ">>>>>>>>>>>>>>>>>>>>>> VOLTAGE BIAS LOADS:" << std::endl;
		oss << "NMOS VOLTAGE BIASES" << std::endl;

		for(auto & load : oneLoadPartLoadsVoltageBiasesNmos_) // feedback
		{
			oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << load->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*load);
            oss << load->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
			
		}

		oss << "PMOS VOLTAGE BIASES" << std::endl;

		for(auto & load : oneLoadPartLoadsVoltageBiasesPmos_) // feedback
		{
			oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << load->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*load);
            oss << load->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
			
		}

		delete flatCircuitRecursion;
        return oss.str();
    }
	
    void Loads::initializeLoadsVoltageBiasesLoadPart(AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, int & nmosIndex, int & pmosIndex) // feedback
    {
		std::vector<const Core::Circuit *> pmosLoadParts = amplificationStagesSubBlockLevel.getLoadParts().getLoadPartsPmosVoltageBiases();
		std::vector<const Core::Circuit *> nmosLoadParts = amplificationStagesSubBlockLevel.getLoadParts().getLoadPartsNmosVoltageBiases();

        oneLoadPartLoadsVoltageBiasesNmos_ = createOneLoadPartLoads(nmosLoadParts, nmosIndex);
	    oneLoadPartLoadsVoltageBiasesPmos_ = createOneLoadPartLoads(pmosLoadParts, pmosIndex);
		
    }

	std::vector<const Core::Circuit *> Loads::createOneLoadPartLoads(std::vector<const Core::Circuit *> loadParts, int & index) const
	{
		std::vector<const Core::Circuit *> loads;

		for(auto & loadPart : loadParts)
		{

			const Core::Circuit & load = createOneLoadPartLoad(createInstance(*loadPart, LOADPART1_), index);

			if(load.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
			{
				loads.push_back(&load);
				index++;
			}
			else
			{
				delete &load;
			}
		}
		return loads;
	}

	std::vector<const Core::Circuit *> Loads::createTwoLoadPartLoadsWithGCC(std::vector<const Core::Circuit *> loadPartsGCC,
									std::vector<const Core::Circuit *> secondLoadParts, int & index) const
	{
		std::vector<const Core::Circuit *> loads;

		for(auto & loadPartWithGCC : loadPartsGCC)
		{
			for(auto & secondLoadPart : secondLoadParts)
			{
				const Core::Circuit & load = createTwoLoadPartLoadWithGCC(createInstance(*loadPartWithGCC, LOADPART1_), createInstance(*secondLoadPart, LOADPART2_), index);
				if(load.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
				{
					loads.push_back(&load);
					index++;
				}
				else
				{
					delete &load;
				}
			}
		}
		return loads;
	}
	
	std::vector<const Core::Circuit *> Loads::createTwoLoadPartLoadsWithoutGCC(std::vector<const Core::Circuit *> mixedLoadParts,
									std::vector<const Core::Circuit *> currentBiasLoadParts, int & index) const
	{
		std::vector<const Core::Circuit *> loads;

		for(auto & mixedLoadPart : mixedLoadParts)
		{
			for(auto & currentBiasLoadPart : currentBiasLoadParts)
			{
				const Core::Circuit & load = createTwoLoadPartLoadWithoutGCC(createInstance(*mixedLoadPart, LOADPART1_), createInstance(*currentBiasLoadPart, LOADPART2_), index);
				if(load.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
				{
					loads.push_back(&load);
					index++;
				}
				else
				{
					delete &load;
				}
			}
		}
		return loads;
	}

	std::vector<const Core::Circuit *> Loads::createSymmetricalLoadsFourTransistorMixedLoadParts(std::vector<const Core::Circuit *> pmosLoadParts,
									std::vector<const Core::Circuit *> nmosLoadParts) const
	{

		std::vector<const Core::Circuit* > symmetricalLoadParts;

		std::vector<const Core::Circuit*>::const_iterator pmosIterator = pmosLoadParts.begin();
		std::vector<const Core::Circuit*>::const_iterator nmosIterator = nmosLoadParts.begin();

		int index = 1;

		while(pmosIterator != pmosLoadParts.end())
		{
			const Core::Circuit & pmosLoadPart = **pmosIterator;
			const Core::Circuit & nmosLoadPart = **nmosIterator;

			const Core::Circuit & symmetricalLoadPart = createSymmetricalLoadFourTransistorMixedLoadParts(
									createInstance(pmosLoadPart, LOADPART1_), createInstance(nmosLoadPart, LOADPART2_), index);
			if(symmetricalLoadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
			{
				symmetricalLoadParts.push_back(&symmetricalLoadPart);
				index++;
			}
			pmosIterator++;
			nmosIterator++;
		}

		return symmetricalLoadParts;
	}

	const Core::Circuit & Loads::createOneLoadPartLoad(Core::Instance & loadPart, int index) const
	{
		
		Core::Circuit * load = new Core::Circuit;
		
        Core::CircuitIds circuitIds;

        if (loadPart.getMaster().getCircuitIdentifier() == circuitIds.voltageBiasLoadPart())
        {
        	Core::CircuitId loadId = circuitIds.voltageBiasLoad(index);
        	loadId.setTechType(loadPart.getMaster().getCircuitIdentifier().getTechType());
            load->setCircuitIdentifier(loadId);
        }
        else {
        	Core::CircuitId loadId = circuitIds.load(index);
        	loadId.setTechType(loadPart.getMaster().getCircuitIdentifier().getTechType());
        	load->setCircuitIdentifier(loadId);
        }

		std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

		netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
		
		addLoad1WithoutGCCNets(netNames, terminalToNetMap, loadPart);
		

		addNetsToCircuit(*load, netNames);
        addTerminalsToCircuit(*load, terminalToNetMap);
        load->addInstance(loadPart);
        
        connectInstanceTerminalsOfLoadPart1WithoutGCC(*load, loadPart);
        loadPart.setCircuit(*load);

        return *load;
	}

	const Core::Circuit & Loads::createTwoLoadPartLoadWithGCC(Core::Instance & loadPartWithGCC, Core::Instance & secondLoadPart, int index) const
		{
			Core::Circuit * load = new Core::Circuit;

	        Core::CircuitIds circuitIds;
	        Core::CircuitId loadId = circuitIds.load(index);
	        loadId.setTechType(loadPartWithGCC.getMaster().getCircuitIdentifier().getTechType());
			load->setCircuitIdentifier(loadId);

			std::vector<Core::NetId> netNames;
	        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

			netNames.push_back(OUT1_NET_);
	        netNames.push_back(OUT2_NET_);

	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));

			addLoad1WithGCCNets(netNames, terminalToNetMap, loadPartWithGCC);
			addLoad2Nets(netNames, terminalToNetMap, secondLoadPart);


			addNetsToCircuit(*load, netNames);
	        addTerminalsToCircuit(*load, terminalToNetMap);
	        load->addInstance(loadPartWithGCC);
			load->addInstance(secondLoadPart);

	        connectInstanceTerminalsOfLoadPart1WithGCC(*load, loadPartWithGCC);
			connectInstanceTerminalsOfLoadPart2(*load, secondLoadPart);
	        loadPartWithGCC.setCircuit(*load);
			secondLoadPart.setCircuit(*load);

	        return *load;
		}

	const Core::Circuit & Loads::createTwoLoadPartLoadWithoutGCC(Core::Instance & mixedLoadPart, Core::Instance & currentBiasLoadPart, int index) const
		{
			Core::Circuit * load = new Core::Circuit;

	        Core::CircuitIds circuitIds;
	        Core::CircuitId loadId = circuitIds.load(index);
	        loadId.setTechType(mixedLoadPart.getMaster().getCircuitIdentifier().getTechType());
			load->setCircuitIdentifier(loadId);

			std::vector<Core::NetId> netNames;
	        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

			netNames.push_back(OUT1_NET_);
	        netNames.push_back(OUT2_NET_);

	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));

			addLoad1WithoutGCCNets(netNames, terminalToNetMap, mixedLoadPart);
			addLoad2Nets(netNames, terminalToNetMap, currentBiasLoadPart);


			addNetsToCircuit(*load, netNames);
	        addTerminalsToCircuit(*load, terminalToNetMap);
	        load->addInstance(mixedLoadPart);
			load->addInstance(currentBiasLoadPart);

	        connectInstanceTerminalsOfLoadPart1WithoutGCC(*load, mixedLoadPart);
			connectInstanceTerminalsOfLoadPart2(*load, currentBiasLoadPart);
	        mixedLoadPart.setCircuit(*load);
			currentBiasLoadPart.setCircuit(*load);

	        return *load;
		}

	const Core::Circuit & Loads::createSymmetricalLoadFourTransistorMixedLoadParts(Core::Instance & pmosLoadPart,
								Core::Instance & nmosLoadPart, int index) const
	{
		Core::Circuit * load = new Core::Circuit;

        Core::CircuitIds circuitIds;
        Core::CircuitId loadId = circuitIds.load(index);
        loadId.setTechType(Core::TechType::undefined());
		load->setCircuitIdentifier(loadId);

		std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

		netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);

        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));

		addLoad1WithoutGCCNets(netNames, terminalToNetMap, pmosLoadPart);
		addLoad2Nets(netNames, terminalToNetMap, nmosLoadPart);


		addNetsToCircuit(*load, netNames);
        addTerminalsToCircuit(*load, terminalToNetMap);
        load->addInstance(nmosLoadPart);
		load->addInstance(pmosLoadPart);

        connectInstanceTerminalsOfLoadPart1WithoutGCC(*load, pmosLoadPart);
		connectInstanceTerminalsOfLoadPart2(*load, nmosLoadPart);
        pmosLoadPart.setCircuit(*load);
		nmosLoadPart.setCircuit(*load);

        return *load;
	}

	void Loads::addLoad1WithoutGCCNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, // feedback
											Core::Instance & loadPart1) const
	{
		netNames.push_back(SOURCELOAD1_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCELOAD1_TERMINAL_, SOURCELOAD1_NET_));

		if(bothTransistorStacksAreVoltageBiases(loadPart1))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				netNames.push_back(OUTOUTPUT1LOAD1_NET_);
            	netNames.push_back(OUTOUTPUT2LOAD1_NET_);
            	netNames.push_back(OUTSOURCE1LOAD1_NET_);
            	netNames.push_back(OUTSOURCE2LOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT1LOAD1_TERMINAL_, OUTOUTPUT1LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT2LOAD1_TERMINAL_, OUTOUTPUT2LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE1LOAD1_TERMINAL_, OUTSOURCE1LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE2LOAD1_TERMINAL_, OUTSOURCE2LOAD1_NET_));
			}
		}
		else
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() == 2)
			{
				netNames.push_back(INNERLOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERLOAD1_TERMINAL_, INNERLOAD1_NET_));
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
        		netNames.push_back(INNERSOURCELOAD1_NET_);
        		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOAD1_TERMINAL_, INNERSOURCELOAD1_NET_));

        		if(isSingleDiodeTransistor(loadPart1.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       			{
            		netNames.push_back(INNEROUTPUTLOAD1_NET_);
            		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_));
        		}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				netNames.push_back(INNEROUTPUTLOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_));
			}
		}
		if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
		{
			netNames.push_back(INNERTRANSISTORSTACK2LOAD1_NET_);
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOAD1_TERMINAL_, INNERTRANSISTORSTACK2LOAD1_NET_));
		}
		if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
		{
			netNames.push_back(INNERTRANSISTORSTACK1LOAD1_NET_);
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOAD1_TERMINAL_, INNERTRANSISTORSTACK1LOAD1_NET_));
		}
	}

	void Loads::addLoad1WithGCCNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
										Core::Instance & loadPart1) const
	{
		netNames.push_back(SOURCEGCC1_NET_);
        netNames.push_back(SOURCEGCC2_NET_);
		netNames.push_back(INNERGCC_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEGCC1_TERMINAL_, SOURCEGCC1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEGCC2_TERMINAL_, SOURCEGCC2_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERGCC_TERMINAL_, INNERGCC_NET_));

		if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
		{
       		netNames.push_back(SOURCELOAD1_NET_);
			netNames.push_back(INNERBIASGCC_NET_);
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCELOAD1_TERMINAL_, SOURCELOAD1_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERBIASGCC_TERMINAL_, INNERBIASGCC_NET_));
		}

	}

	void Loads::addLoad2Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & loadPart2) const
		{
			netNames.push_back(SOURCELOAD2_NET_);
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCELOAD2_TERMINAL_, SOURCELOAD2_NET_));

			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() == 2)
			{
				netNames.push_back(INNERLOAD2_NET_);
	            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERLOAD2_TERMINAL_, INNERLOAD2_NET_));
			}
			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 2)
			{
	        	netNames.push_back(INNERSOURCELOAD2_NET_);
				netNames.push_back(INNERTRANSISTORSTACK2LOAD2_NET_);
	        	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOAD2_NET_));
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOAD2_TERMINAL_, INNERTRANSISTORSTACK2LOAD2_NET_));

	        	if(isSingleDiodeTransistor(loadPart2.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
	       		{
	            		netNames.push_back(INNEROUTPUTLOAD2_NET_);
	            		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_));
	        	}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 3)
			{
				netNames.push_back(INNEROUTPUTLOAD2_NET_);
	            netNames.push_back(INNERSOURCELOAD2_NET_);
				netNames.push_back(INNERTRANSISTORSTACK1LOAD2_NET_);
	            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_));
	            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOAD2_NET_));
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOAD2_TERMINAL_, INNERTRANSISTORSTACK1LOAD2_NET_));
			}
		}

	void Loads::connectInstanceTerminalsOfLoadPart1WithoutGCC(Core::Circuit & load, Core::Instance & loadPart1) const
	{
		connectInstanceTerminal(load, loadPart1, LoadParts::OUT1_TERMINAL_, OUT1_NET_);
		connectInstanceTerminal(load, loadPart1, LoadParts::OUT2_TERMINAL_, OUT2_NET_);
		connectInstanceTerminal(load, loadPart1, LoadParts::SOURCE_TERMINAL_, SOURCELOAD1_NET_);

		if(bothTransistorStacksAreVoltageBiases(loadPart1))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(load, loadPart1, LoadParts::OUTOUTPUT1_TERMINAL_, OUTOUTPUT1LOAD1_NET_);
				connectInstanceTerminal(load, loadPart1, LoadParts::OUTOUTPUT2_TERMINAL_, OUTOUTPUT2LOAD1_NET_);
				connectInstanceTerminal(load, loadPart1, LoadParts::OUTSOURCE1_TERMINAL_, OUTSOURCE1LOAD1_NET_);
				connectInstanceTerminal(load, loadPart1, LoadParts::OUTSOURCE2_TERMINAL_, OUTSOURCE2LOAD1_NET_);
			}
		}
		else
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() == 2)
			{
				connectInstanceTerminal(load, loadPart1, LoadParts::INNER_TERMINAL_, INNERLOAD1_NET_);
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(load, loadPart1, LoadParts::INNERSOURCE_TERMINAL_, INNERSOURCELOAD1_NET_);
        		
        		if(isSingleDiodeTransistor(loadPart1.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       			{
					connectInstanceTerminal(load, loadPart1, LoadParts::INNEROUTPUT_TERMINAL_, INNEROUTPUTLOAD1_NET_);
        		}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				connectInstanceTerminal(load, loadPart1, LoadParts::INNEROUTPUT_TERMINAL_, INNEROUTPUTLOAD1_NET_);
			}
		}
		if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
		{
			connectInstanceTerminal(load, loadPart1, LoadParts::INNERTRANSISTORSTACK2_TERMINAL_, INNERTRANSISTORSTACK2LOAD1_NET_);
		}
		if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
		{
			connectInstanceTerminal(load, loadPart1, LoadParts::INNERTRANSISTORSTACK1_TERMINAL_, INNERTRANSISTORSTACK1LOAD1_NET_);
		}
	}

	void Loads::connectInstanceTerminalsOfLoadPart1WithGCC(Core::Circuit & load, Core::Instance & loadPartWithGCC) const
		{
			connectInstanceTerminal(load, loadPartWithGCC, LoadParts::OUT1_TERMINAL_, OUT1_NET_);
			connectInstanceTerminal(load, loadPartWithGCC, LoadParts::OUT2_TERMINAL_, OUT2_NET_);

			if(getDeviceNamesOfFlatCircuit(loadPartWithGCC.getMaster()).size()== 2)
			{
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::SOURCE1_TERMINAL_, SOURCEGCC1_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::SOURCE2_TERMINAL_, SOURCEGCC2_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::INNER_TERMINAL_, INNERGCC_NET_);
			}
			else
			{
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::INNERTRANSISTORSTACK1_TERMINAL_, SOURCEGCC1_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::INNERTRANSISTORSTACK2_TERMINAL_, SOURCEGCC2_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::INNEROUTPUT_TERMINAL_, INNERGCC_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::SOURCE_TERMINAL_, SOURCELOAD1_NET_);
				connectInstanceTerminal(load, loadPartWithGCC, LoadParts::INNERSOURCE_TERMINAL_, INNERBIASGCC_NET_);
			}
		}

	void Loads::connectInstanceTerminalsOfLoadPart2(Core::Circuit & load, Core::Instance & loadPart2) const
		{
			connectInstanceTerminal(load, loadPart2, LoadParts::OUT1_TERMINAL_, OUT1_NET_);
			connectInstanceTerminal(load, loadPart2, LoadParts::OUT2_TERMINAL_, OUT2_NET_);
			connectInstanceTerminal(load, loadPart2, LoadParts::SOURCE_TERMINAL_, SOURCELOAD2_NET_);

			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() == 2)
			{
				connectInstanceTerminal(load, loadPart2, LoadParts::INNER_TERMINAL_, INNERLOAD2_NET_);
			}
			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 2)
			{
				connectInstanceTerminal(load, loadPart2, LoadParts::INNERSOURCE_TERMINAL_, INNERSOURCELOAD2_NET_);
	        	connectInstanceTerminal(load, loadPart2, LoadParts::INNERTRANSISTORSTACK2_TERMINAL_, INNERTRANSISTORSTACK2LOAD2_NET_);

				if(isSingleDiodeTransistor(loadPart2.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
	       		{
					connectInstanceTerminal(load, loadPart2, LoadParts::INNEROUTPUT_TERMINAL_, INNEROUTPUTLOAD2_NET_);
	        	}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 3)
			{
					connectInstanceTerminal(load, loadPart2, LoadParts::INNEROUTPUT_TERMINAL_, INNEROUTPUTLOAD2_NET_);
					connectInstanceTerminal(load, loadPart2, LoadParts::INNERTRANSISTORSTACK1_TERMINAL_, INNERTRANSISTORSTACK1LOAD2_NET_);
			}

		}

	void Loads::eraseOneLoadPartLoadsVoltageBiasesNmos()
	{
    	for(auto & load : oneLoadPartLoadsVoltageBiasesNmos_)
    	{
    		delete load;
    	}
	}

	void Loads::eraseOneLoadPartLoadsVoltageBiasesPmos()
	{
    	for(auto & load : oneLoadPartLoadsVoltageBiasesPmos_)
    	{
    		delete load;
    	}
	}

}
