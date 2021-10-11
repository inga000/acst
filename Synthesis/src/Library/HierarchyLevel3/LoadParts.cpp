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
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/VoltageBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"




#include <sstream>


namespace Synthesis {

    const Core::TerminalName LoadParts::OUT1_TERMINAL_ = Core::TerminalName("Out1");
	const Core::TerminalName LoadParts::OUT2_TERMINAL_ = Core::TerminalName("Out2");
	const Core::TerminalName LoadParts::SOURCE_TERMINAL_ = Core::TerminalName("Source");

    const Core::TerminalName LoadParts::SOURCE1_TERMINAL_ = Core::TerminalName("Source1");
    const Core::TerminalName LoadParts::SOURCE2_TERMINAL_ = Core::TerminalName("Source2");

	const Core::TerminalName LoadParts::INNER_TERMINAL_ = Core::TerminalName("Inner");

	const Core::TerminalName LoadParts::INNEROUTPUT_TERMINAL_ = Core::TerminalName("InnerOutput");
	const Core::TerminalName LoadParts::INNERSOURCE_TERMINAL_ = Core::TerminalName("InnerSource");

    const Core::TerminalName LoadParts::OUTOUTPUT1_TERMINAL_ = Core::TerminalName("OutOutput1");
    const Core::TerminalName LoadParts::OUTOUTPUT2_TERMINAL_ = Core::TerminalName("OutOutput2");
	const Core::TerminalName LoadParts::OUTSOURCE1_TERMINAL_ = Core::TerminalName("OutSource1");
	const Core::TerminalName LoadParts::OUTSOURCE2_TERMINAL_ = Core::TerminalName("OutSource2");

	const Core::TerminalName LoadParts::INNERTRANSISTORSTACK1_TERMINAL_ = Core::TerminalName("InnerTransistorStack1");
	const Core::TerminalName LoadParts::INNERTRANSISTORSTACK2_TERMINAL_ = Core::TerminalName("InnerTransistorStack2");

    const Core::InstanceName LoadParts::TRANSISTORSTACK1_ = Core::InstanceName("TransistorStack1");
	const Core::InstanceName LoadParts::TRANSISTORSTACK2_ = Core::InstanceName("TransistorStack2");
			
	const Core::NetId LoadParts::OUT1_NET_ = Core::NetName("out1").createRootIdentifier();
	const Core::NetId LoadParts::OUT2_NET_ = Core::NetName("out2").createRootIdentifier();
	const Core::NetId LoadParts::SOURCE_NET_ = Core::NetName("source").createRootIdentifier();

    const Core::NetId LoadParts::SOURCE1_NET_ = Core::NetName("source1").createRootIdentifier();
    const Core::NetId LoadParts::SOURCE2_NET_ = Core::NetName("source2").createRootIdentifier();

	const Core::NetId LoadParts::INNER_NET_ = Core::NetName("inner").createRootIdentifier();

	const Core::NetId LoadParts::INNEROUTPUT_NET_ = Core::NetName("innerOutput").createRootIdentifier();
	const Core::NetId LoadParts::INNERSOURCE_NET_ = Core::NetName("innerSource").createRootIdentifier();

    const Core::NetId LoadParts::OUTOUTPUT1_NET_ = Core::NetName("outOutput1").createRootIdentifier();
	const Core::NetId LoadParts::OUTOUTPUT2_NET_ = Core::NetName("outOutput2").createRootIdentifier();
	const Core::NetId LoadParts::OUTSOURCE1_NET_ = Core::NetName("outSource1").createRootIdentifier();
    const Core::NetId LoadParts::OUTSOURCE2_NET_ = Core::NetName("outSource2").createRootIdentifier();

	const Core::NetId LoadParts::INNERTRANSISTORSTACK1_NET_ = Core::NetName("innerTransistorStack1").createRootIdentifier();
	const Core::NetId LoadParts::INNERTRANSISTORSTACK2_NET_ = Core::NetName("innerTransistorStack2").createRootIdentifier();


	LoadParts::LoadParts(const StructuralLevel & structuralLevel)
    {
		structuralLevel_ = & structuralLevel;

        initializeLoadPartsPmos(structuralLevel);
        initializeLoadPartsNmos(structuralLevel);

    }
	
    LoadParts::~LoadParts()
    {
        eraseTwoTransistorsLoadPartsPmosVoltageBiases();
        eraseFourTransistorsLoadPartsPmosVoltageBiases();
        eraseTwoTransistorsLoadPartsNmosVoltageBiases();
        eraseFourTransistorsLoadPartsNmosVoltageBiases();
    }

    void LoadParts::eraseTemporaryFunctionalblocks() const
    {
		for (const Core::Circuit* temporaryElement: temporaryLoadParts_)
		{
			if(temporaryElement)
			{
				delete temporaryElement;
			}
		}
    }

    const StructuralLevel & LoadParts::getStructuralLevel() const
	{
		assert(structuralLevel_ != nullptr);
		return * structuralLevel_;
	}

	std::vector<const Core::Circuit *> LoadParts::getTemporaryLoadParts() const
	{
		return temporaryLoadParts_;
	}

	std::vector<const Core::Circuit *> LoadParts::createTwoTransistorsLoadPartsLoadPartsPmosVoltageBiases() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesPmos();

        loadParts = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createFourTransistorsLoadPartsLoadPartsPmosVoltageBiases() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesPmos();

        loadParts = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createTwoTransistorsLoadPartsLoadPartsNmosVoltageBiases() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesNmos();

        loadParts = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createFourTransistorsLoadPartsLoadPartsNmosVoltageBiases() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesNmos();

        loadParts = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }


    std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosTwoTransistorCurrentBiasesDifferentSources() const
    {
    	int index = 1;
    	std::vector<const Core::Circuit*> loadParts;
    	std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesPmos();

    	loadParts = createTwoTransistorLoadPartsCurrentBiasesDifferentSources(oneTransistorCurrentBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosTwoTransistorCurrentBiasesDifferentSources() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> loadParts;
		std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesNmos();

		loadParts = createTwoTransistorLoadPartsCurrentBiasesDifferentSources(oneTransistorCurrentBiases, index);
        getTemporaryLoadParts() = loadParts;
        return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosFourTransistorCurrentBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> loadParts;
		std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesPmos();

		loadParts = createFourTransistorLoadPartsCurrentBiases(twoTransistorCurrentBiases, index);
		getTemporaryLoadParts() = loadParts;
		return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosFourTransistorCurrentBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> loadParts;
		std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesNmos();

		loadParts =  createFourTransistorLoadPartsCurrentBiases(twoTransistorCurrentBiases, index);
		getTemporaryLoadParts() = loadParts;
		return loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosCurrentBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesPmos();
		std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesPmos();
		std::vector<const Core::Circuit*> twoTransistorsLoadPartsPmosCurrentBiases = createTwoTransistorLoadPartsCurrentBiases(oneTransistorCurrentBiases, index);
		std::vector<const Core::Circuit*> fourTransistorsLoadPartsPmosCurrentBiases = createFourTransistorLoadPartsCurrentBiases(twoTransistorCurrentBiases, index);

        std::vector<const Core::Circuit*> loadPartsPmosCurrentBiases;
        loadPartsPmosCurrentBiases.insert(loadPartsPmosCurrentBiases.end(), twoTransistorsLoadPartsPmosCurrentBiases.begin(), twoTransistorsLoadPartsPmosCurrentBiases.end());
        loadPartsPmosCurrentBiases.insert(loadPartsPmosCurrentBiases.end(), fourTransistorsLoadPartsPmosCurrentBiases.begin(), fourTransistorsLoadPartsPmosCurrentBiases.end());
        getTemporaryLoadParts() = loadPartsPmosCurrentBiases;
        return loadPartsPmosCurrentBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosCurrentBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesNmos();
		std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesNmos();
		std::vector<const Core::Circuit*> twoTransistorsLoadPartsNmosCurrentBiases = createTwoTransistorLoadPartsCurrentBiases(oneTransistorCurrentBiases, index);
		std::vector<const Core::Circuit*> fourTransistorsLoadPartsNmosCurrentBiases = createFourTransistorLoadPartsCurrentBiases(twoTransistorCurrentBiases, index);

        std::vector<const Core::Circuit*> loadPartsNmosCurrentBiases;
        loadPartsNmosCurrentBiases.insert(loadPartsNmosCurrentBiases.end(), twoTransistorsLoadPartsNmosCurrentBiases.begin(), twoTransistorsLoadPartsNmosCurrentBiases.end());
        loadPartsNmosCurrentBiases.insert(loadPartsNmosCurrentBiases.end(), fourTransistorsLoadPartsNmosCurrentBiases.begin(), fourTransistorsLoadPartsNmosCurrentBiases.end());
        getTemporaryLoadParts() = loadPartsNmosCurrentBiases;
        return loadPartsNmosCurrentBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosVoltageBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesPmos();
		std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesPmos();
		std::vector<const Core::Circuit*> twoTransistorsLoadPartsPmosVoltageBiases = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
		std::vector<const Core::Circuit*> fourTransistorsLoadPartsPmosVoltageBiases = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);

        std::vector<const Core::Circuit*> loadPartsPmosVoltageBiases;
        loadPartsPmosVoltageBiases.insert(loadPartsPmosVoltageBiases.end(), twoTransistorsLoadPartsPmosVoltageBiases.begin(), twoTransistorsLoadPartsPmosVoltageBiases.end());
        loadPartsPmosVoltageBiases.insert(loadPartsPmosVoltageBiases.end(), fourTransistorsLoadPartsPmosVoltageBiases.begin(), fourTransistorsLoadPartsPmosVoltageBiases.end());
        getTemporaryLoadParts() = loadPartsPmosVoltageBiases;
        return loadPartsPmosVoltageBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosVoltageBiases() const
    {
		int index = 1;
		std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesNmos();
		std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesNmos();
		std::vector<const Core::Circuit*> twoTransistorsLoadPartsNmosVoltageBiases = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
		std::vector<const Core::Circuit*> fourTransistorsLoadPartsNmosVoltageBiases = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);

        std::vector<const Core::Circuit*> loadPartsNmosVoltageBiases;
        loadPartsNmosVoltageBiases.insert(loadPartsNmosVoltageBiases.end(), twoTransistorsLoadPartsNmosVoltageBiases.begin(), twoTransistorsLoadPartsNmosVoltageBiases.end());
        loadPartsNmosVoltageBiases.insert(loadPartsNmosVoltageBiases.end(), fourTransistorsLoadPartsNmosVoltageBiases.begin(), fourTransistorsLoadPartsNmosVoltageBiases.end());
        getTemporaryLoadParts() = loadPartsNmosVoltageBiases;
        return loadPartsNmosVoltageBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosMixed() const
    {
		int index = 1;
	    std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesPmos();
	    std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesPmos();
	    std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesPmos();
	    std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesPmos();
		std::vector<const Core::Circuit*> twoTransistorsLoadPartsPmosMixed = createTwoTransistorLoadPartsMixed(oneTransistorVoltageBiases, oneTransistorCurrentBiases, index);
		std::vector<const Core::Circuit*> threeTransistorsLoadPartsPmosMixed = createThreeTransistorLoadPartsMixed(oneTransistorVoltageBiases, twoTransistorCurrentBiases, index);
		std::vector<const Core::Circuit*> fourTransistorsLoadPartsPmosMixed = createFourTransistorLoadPartsMixed(twoTransistorVoltageBiases, twoTransistorCurrentBiases, index);

        std::vector<const Core::Circuit*> loadPartsPmosMixed;
        loadPartsPmosMixed.insert(loadPartsPmosMixed.end(), twoTransistorsLoadPartsPmosMixed.begin(), twoTransistorsLoadPartsPmosMixed.end());
        loadPartsPmosMixed.insert(loadPartsPmosMixed.end(), threeTransistorsLoadPartsPmosMixed.begin(), threeTransistorsLoadPartsPmosMixed.end());
        loadPartsPmosMixed.insert(loadPartsPmosMixed.end(), fourTransistorsLoadPartsPmosMixed.begin(), fourTransistorsLoadPartsPmosMixed.end());
        getTemporaryLoadParts() = loadPartsPmosMixed;
        return loadPartsPmosMixed;
    }

	std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosMixed() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getOneTransistorVoltageBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesNmos();
        std::vector<const Core::Circuit*> oneTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getOneTransistorCurrentBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorsLoadPartsNmosMixed = createTwoTransistorLoadPartsMixed(oneTransistorVoltageBiases, oneTransistorCurrentBiases, index);
        std::vector<const Core::Circuit*> threeTransistorsLoadPartsNmosMixed = createThreeTransistorLoadPartsMixed(oneTransistorVoltageBiases, twoTransistorCurrentBiases, index);;
        std::vector<const Core::Circuit*> fourTransistorsLoadPartsNmosMixed = createFourTransistorLoadPartsMixed(twoTransistorVoltageBiases, twoTransistorCurrentBiases, index);

        std::vector<const Core::Circuit*> loadPartsNmosMixed;
        loadPartsNmosMixed.insert(loadPartsNmosMixed.end(), twoTransistorsLoadPartsNmosMixed.begin(), twoTransistorsLoadPartsNmosMixed.end());
        loadPartsNmosMixed.insert(loadPartsNmosMixed.end(), threeTransistorsLoadPartsNmosMixed.begin(), threeTransistorsLoadPartsNmosMixed.end());
        loadPartsNmosMixed.insert(loadPartsNmosMixed.end(), fourTransistorsLoadPartsNmosMixed.begin(), fourTransistorsLoadPartsNmosMixed.end());
        getTemporaryLoadParts() = loadPartsNmosMixed;
        return loadPartsNmosMixed;
    }

    std::vector<const Core::Circuit *> LoadParts::createLoadPartsPmosFourTransistorMixed() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesPmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesPmos();

        loadParts = createFourTransistorLoadPartsMixed(twoTransistorVoltageBiases, twoTransistorCurrentBiases, index);
        getTemporaryLoadParts() = loadParts;
        return  loadParts;
    }

    std::vector<const Core::Circuit *> LoadParts::createLoadPartsNmosFourTransistorMixed() const
    {
        int index = 1;
        std::vector<const Core::Circuit*> loadParts;
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = getStructuralLevel().getVoltageBiases().getTwoTransistorVoltageBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = getStructuralLevel().getCurrentBiases().getTwoTransistorCurrentBiasesNmos();

        loadParts = createFourTransistorLoadPartsMixed(twoTransistorVoltageBiases, twoTransistorCurrentBiases, index);
        getTemporaryLoadParts() = loadParts;
        return  loadParts;
    }

	std::vector<const Core::Circuit *> LoadParts::getLoadPartsPmosVoltageBiases() const // feedback
    {
        assert(!twoTransistorsLoadPartsPmosVoltageBiases_.empty() || !fourTransistorsLoadPartsPmosVoltageBiases_.empty());
       
        std::vector<const Core::Circuit*> loadPartsPmosVoltageBiases;
        loadPartsPmosVoltageBiases.insert(loadPartsPmosVoltageBiases.end(), twoTransistorsLoadPartsPmosVoltageBiases_.begin(), twoTransistorsLoadPartsPmosVoltageBiases_.end());
        loadPartsPmosVoltageBiases.insert(loadPartsPmosVoltageBiases.end(), fourTransistorsLoadPartsPmosVoltageBiases_.begin(), fourTransistorsLoadPartsPmosVoltageBiases_.end());

        return loadPartsPmosVoltageBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::getLoadPartsNmosVoltageBiases() const // feedback
    {
        assert(!twoTransistorsLoadPartsNmosVoltageBiases_.empty() || !fourTransistorsLoadPartsNmosVoltageBiases_.empty());
       
        std::vector<const Core::Circuit*> loadPartsNmosVoltageBiases;
        loadPartsNmosVoltageBiases.insert(loadPartsNmosVoltageBiases.end(), twoTransistorsLoadPartsNmosVoltageBiases_.begin(), twoTransistorsLoadPartsNmosVoltageBiases_.end());
        loadPartsNmosVoltageBiases.insert(loadPartsNmosVoltageBiases.end(), fourTransistorsLoadPartsNmosVoltageBiases_.begin(), fourTransistorsLoadPartsNmosVoltageBiases_.end());

        return loadPartsNmosVoltageBiases;
    }

	std::string LoadParts::toStr() const
    {
        std::ostringstream oss;
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Load Parts: " << std::endl;

		int caseNumber = 1;
		std::vector<const Core::Circuit*> loadParts;
		do
		{
			switch(caseNumber)
			{
			case 1: {
				loadParts = createTwoTransistorsLoadPartsLoadPartsPmosVoltageBiases();
				break;
			}
			case 2: {
				loadParts = createFourTransistorsLoadPartsLoadPartsPmosVoltageBiases();
				break;
			}
			case 3: {
				loadParts = createTwoTransistorsLoadPartsLoadPartsNmosVoltageBiases();
				break;
			}
			case 4: {
				loadParts = createFourTransistorsLoadPartsLoadPartsNmosVoltageBiases();
				break;
			}
			case 5: {
				loadParts = createLoadPartsPmosTwoTransistorCurrentBiasesDifferentSources();
				break;
			}
			case 6: {
				loadParts = createLoadPartsNmosTwoTransistorCurrentBiasesDifferentSources();
				break;
			}
			case 7: {
				loadParts = createLoadPartsPmosFourTransistorCurrentBiases();
				break;
			}
			case 8: {
				loadParts = createLoadPartsNmosFourTransistorCurrentBiases();
				break;
			}
			case 9: {
				loadParts = createLoadPartsPmosCurrentBiases();
				break;
			}
			case 10: {
				loadParts = createLoadPartsNmosCurrentBiases();
				break;
			}
			case 11: {
				loadParts = createLoadPartsPmosVoltageBiases();
				break;
			}
			case 12: {
				loadParts = createLoadPartsNmosVoltageBiases();
				break;
			}
			case 13: {
				loadParts = createLoadPartsPmosMixed();
				break;
			}
			case 14: {
				loadParts = createLoadPartsNmosMixed();
				break;
			}
			case 15: {
				loadParts = createLoadPartsPmosFourTransistorMixed();
				break;
			}
			case 16: {
				loadParts = createLoadPartsNmosFourTransistorMixed();
				break;
			}
			case 17: {
				loadParts.clear();
				break;
			}
			default: {
				logDebug("default");
				break;
			}
			}

	        for(auto & loadPart : loadParts)
	        {
	             oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << loadPart->getCircuitIdentifier().toStr() << ": " << std::endl;
	            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*loadPart);
	            oss << loadPart->toStr() << std::endl;
	            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	            oss << flatCircuit->toStr() << std::endl;
	            delete flatCircuit;
	        }
        	caseNumber++;
		} while (!loadParts.empty());

        oss << "Pmos:" << std::endl;

        oss << "TWO TRANSISTORS LOAD PARTS VOLTAGE BIASES" << std::endl;
        
        for(auto & loadPart : twoTransistorsLoadPartsPmosVoltageBiases_) // feedback
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << loadPart->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*loadPart);
            oss << loadPart->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
            
        }

        oss << "FOUR TRANSISTORS LOAD PARTS VOLTAGE BIASES" << std::endl;
        for(auto & loadPart : fourTransistorsLoadPartsPmosVoltageBiases_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << loadPart->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*loadPart);
            oss << loadPart->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }

        oss << "Nmos:" << std::endl;

        oss << "TWO TRANSISTORS LOAD PARTS VOLTAGE BIASES" << std::endl;
        for(auto & loadPart : twoTransistorsLoadPartsNmosVoltageBiases_) // feedback
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << loadPart->getCircuitIdentifier().toStr() << ": " << std::endl;  
            oss << loadPart->toStr() << std::endl;
        }

        oss << "FOUR TRANSISTORS LOAD PARTS VOLTAGE BIASES" << std::endl;
        for(auto & loadPart : fourTransistorsLoadPartsNmosVoltageBiases_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << loadPart->getCircuitIdentifier().toStr() << ": " << std::endl;  
            oss << loadPart->toStr() << std::endl;
        }

        delete flatCircuitRecursion;
        return oss.str();
    }

	
	void LoadParts::initializeLoadPartsPmos(const StructuralLevel & structuralLevel)
    {
        int index = 1;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases = structuralLevel.getVoltageBiases().getOneTransistorVoltageBiasesPmos();
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = structuralLevel.getVoltageBiases().getTwoTransistorVoltageBiasesPmos();
        std::vector<const Core::Circuit*> oneTransistorCurrentBiases = structuralLevel.getCurrentBiases().getOneTransistorCurrentBiasesPmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = structuralLevel.getCurrentBiases().getTwoTransistorCurrentBiasesPmos();

        twoTransistorsLoadPartsPmosVoltageBiases_ = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
        fourTransistorsLoadPartsPmosVoltageBiases_ = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);
    }
	
    void LoadParts::initializeLoadPartsNmos(const StructuralLevel & structuralLevel)
    {
        int index = 1;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases = structuralLevel.getVoltageBiases().getOneTransistorVoltageBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases = structuralLevel.getVoltageBiases().getTwoTransistorVoltageBiasesNmos();
        std::vector<const Core::Circuit*> oneTransistorCurrentBiases = structuralLevel.getCurrentBiases().getOneTransistorCurrentBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = structuralLevel.getCurrentBiases().getTwoTransistorCurrentBiasesNmos();

        twoTransistorsLoadPartsNmosVoltageBiases_ = createTwoTransistorLoadPartsVoltageBiases(oneTransistorVoltageBiases, index);
        fourTransistorsLoadPartsNmosVoltageBiases_ = createFourTransistorLoadPartsVoltageBiases(twoTransistorVoltageBiases, index);
    }

    std::vector<const Core::Circuit *> LoadParts::createTwoTransistorLoadPartsCurrentBiases(
						std::vector<const Core::Circuit* > oneTransistorCurrentBiases, int & index) const
    {
        std::vector<const Core::Circuit*> twoTransistorLoadPartsCurrentBiases;

        for(auto & currentBias : oneTransistorCurrentBiases )
        {
            Core::Instance & currentBiasInstance1 = createInstance(*currentBias, TRANSISTORSTACK1_);
            Core::Instance & currentBiasInstance2 = createInstance(*currentBias, TRANSISTORSTACK2_);
           const Core::Circuit & loadPart  = createTwoTransistorLoadPart(currentBiasInstance1, currentBiasInstance2, index);
           twoTransistorLoadPartsCurrentBiases.push_back(&loadPart);
           index++;
        }

        return twoTransistorLoadPartsCurrentBiases;
    }

    std::vector<const Core::Circuit *> LoadParts::createTwoTransistorLoadPartsCurrentBiasesDifferentSources(
						std::vector<const Core::Circuit *> oneTransistorCurrentBiases, int & index) const
    {
         std::vector<const Core::Circuit*> twoTransistorLoadPartsCurrentBiases;

        for(auto & currentBias : oneTransistorCurrentBiases )
        {
            Core::Instance & currentBiasInstance1 = createInstance(*currentBias, TRANSISTORSTACK1_);
            Core::Instance & currentBiasInstance2 = createInstance(*currentBias, TRANSISTORSTACK2_);
           const Core::Circuit & loadPart  = createTwoTransistorLoadPartDifferentSources(currentBiasInstance1, currentBiasInstance2, index);
           twoTransistorLoadPartsCurrentBiases.push_back(&loadPart);
           index++;
        }

        return twoTransistorLoadPartsCurrentBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createTwoTransistorLoadPartsVoltageBiases(
						std::vector<const Core::Circuit* > oneTransistorVoltageBiases,  int & index) const
    {
        std::vector<const Core::Circuit*> twoTransistorLoadPartsVoltageBiases;

        for(auto & voltageBias : oneTransistorVoltageBiases )
        {
            if(isSingleDiodeTransistor(*voltageBias))
            {
                Core::Instance & voltageBiasInstance1 = createInstance(*voltageBias, TRANSISTORSTACK1_);
                Core::Instance & voltageBiasInstance2 = createInstance(*voltageBias, TRANSISTORSTACK2_);
                const Core::Circuit & loadPart  = createTwoTransistorLoadPart(voltageBiasInstance1, voltageBiasInstance2, index);
                if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType() 
                     &&  !loadPart.hasGateNetsNotConnectedToADrain())
                {
                 twoTransistorLoadPartsVoltageBiases.push_back(&loadPart);
                 index++;
                }
                else
                {
                    delete &loadPart;
                }
            }
        }
        return twoTransistorLoadPartsVoltageBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createTwoTransistorLoadPartsMixed(
							std::vector<const Core::Circuit* > oneTransistorVoltageBiases,
							std::vector<const Core::Circuit* > oneTransistorCurrentBiases, int & index) const
	    {
	        std::vector<const Core::Circuit*> twoTransistorLoadPartsMixed;
	        for(auto & voltageBias : oneTransistorVoltageBiases )
	        {
	            for(auto & currentBias : oneTransistorCurrentBiases)
	            {
	                Core::Instance & voltageBiasInstance = createInstance(*voltageBias, TRANSISTORSTACK1_);
	                Core::Instance & currentBiasInstance = createInstance(*currentBias, TRANSISTORSTACK2_);
	                const Core::Circuit & loadPart = createTwoTransistorLoadPart(voltageBiasInstance, currentBiasInstance, index);

	                if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType()
	                          &&  !loadPart.hasGateNetsNotConnectedToADrain())
	                {
	                    twoTransistorLoadPartsMixed.push_back(&loadPart);
	                    index++;
	                }
	                else
	                {
	                    delete &loadPart;
	                }
	            }
	        }
	        return twoTransistorLoadPartsMixed;
	    }

	std::vector<const Core::Circuit *> LoadParts::createThreeTransistorLoadPartsMixed(
								std::vector<const Core::Circuit* > oneTransistorVoltageBiases,
								std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const
		    {
		        std::vector<const Core::Circuit*> threeTransistorLoadParts;

		        for(auto & voltageBias : oneTransistorVoltageBiases )
		        {
		            for(auto & currentBias : twoTransistorCurrentBiases)
		            {
		                Core::Instance & voltageBiasInstance = createInstance(*voltageBias, TRANSISTORSTACK1_);
		                Core::Instance & currentBiasInstance = createInstance(*currentBias, TRANSISTORSTACK2_);
		                const Core::Circuit & loadPart = createThreeTransistorLoadPart(voltageBiasInstance, currentBiasInstance, index);

		                if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType()
		                          &&  !loadPart.hasGateNetsNotConnectedToADrain() && !loadPart.GatesOfInstanceTransistorsConnectedToSameNet(currentBiasInstance))
		                {
		                    threeTransistorLoadParts.push_back(&loadPart);
		                    index++;
		                }
		                else if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
		                {
		                    std::vector<Core::NetId> gateNets = loadPart.getGateNetsNotConnectedToADrain();
		                    if(transistorsWithSameGateNetAreNotSourceConnected(loadPart, gateNets))
		                    {
		                        threeTransistorLoadParts.push_back(&loadPart);
		                        index++;
		                    }
		                    else
		                    {
		                        delete &loadPart;
		                    }

		                }
		                else
		                {
		                    delete &loadPart;
		                }

		            }
		        }

		        return threeTransistorLoadParts;
		    }

	std::vector<const Core::Circuit *> LoadParts::createFourTransistorLoadPartsCurrentBiases(
						std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const
    {
        std::vector<const Core::Circuit*> fourTransistorLoadPartsCurrentBiases;

        for(auto & currentBias : twoTransistorCurrentBiases )
        {
            Core::Instance & currentBiasInstance1 = createInstance(*currentBias, TRANSISTORSTACK1_);
            Core::Instance & currentBiasInstance2 = createInstance(*currentBias, TRANSISTORSTACK2_);
           const Core::Circuit & loadPart  = createFourTransistorLoadPart(currentBiasInstance1, currentBiasInstance2, index);
           if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
           {
                fourTransistorLoadPartsCurrentBiases.push_back(&loadPart);
                index++;
           }

        }

        return fourTransistorLoadPartsCurrentBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createFourTransistorLoadPartsVoltageBiases(
						std::vector<const Core::Circuit* > twoTransistorVoltageBiases, int & index) const
    {
        std::vector<const Core::Circuit*> fourTransistorLoadPartsVoltageBiases;

        for(auto & voltageBias : twoTransistorVoltageBiases )
        {
            Core::Instance & voltageBiasInstance1 = createInstance(*voltageBias, TRANSISTORSTACK1_);
            Core::Instance & voltageBiasInstance2 = createInstance(*voltageBias, TRANSISTORSTACK2_);
            const Core::Circuit & loadPart  = createFourTransistorLoadPart(voltageBiasInstance1, voltageBiasInstance2, index);

            if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType() 
                    &&  !loadPart.hasGateNetsNotConnectedToADrain())
            {
                fourTransistorLoadPartsVoltageBiases.push_back(&loadPart);
                index++;
            }
            else if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
            {
                std::vector<Core::NetId> gateNets = loadPart.getGateNetsNotConnectedToADrain();
                std::vector<Core::DeviceId> devices = getGateConnectedFlatDevices(loadPart, gateNets);
                if(!transistorsAreSourceConnected(loadPart, devices))
                {
                    fourTransistorLoadPartsVoltageBiases.push_back(&loadPart);
                    index++;
                }
                else
                {
                    delete &loadPart;
                }
                
            }
            else
            {
                delete &loadPart;
            }
        }
        
        return fourTransistorLoadPartsVoltageBiases;
    }

	std::vector<const Core::Circuit *> LoadParts::createFourTransistorLoadPartsMixed(
								std::vector<const Core::Circuit* > twoTransistorVoltageBiases,
								std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const
		    {
		        std::vector<const Core::Circuit*> fourTransistorLoadParts;

		        for(auto & voltageBias : twoTransistorVoltageBiases )
		        {
		            for(auto & currentBias : twoTransistorCurrentBiases)
		            {
		                Core::Instance & voltageBiasInstance = createInstance(*voltageBias, TRANSISTORSTACK1_);
		                Core::Instance & currentBiasInstance = createInstance(*currentBias, TRANSISTORSTACK2_);
		                const Core::Circuit & loadPart = createFourTransistorLoadPart(voltageBiasInstance, currentBiasInstance, index);

		                if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType()
		                          &&  !loadPart.hasGateNetsNotConnectedToADrain())
		                {
		                    fourTransistorLoadParts.push_back(&loadPart);
		                    index++;
		                }
		                else if(loadPart.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
		                {
		                    std::vector<Core::NetId> gateNets = loadPart.getGateNetsNotConnectedToADrain();
		                    if(transistorsWithSameGateNetAreNotSourceConnected(loadPart, gateNets))
		                    {
		                        fourTransistorLoadParts.push_back(&loadPart);
		                        index++;
		                    }
		                    else
		                    {
		                        delete &loadPart;
		                    }
		                }
		                else
		                {
		                    delete &loadPart;
		                }

		            }
		        }

		        return fourTransistorLoadParts;
		    }

	const Core::Circuit & LoadParts::createTwoTransistorLoadPart(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const
    {
        Core::Circuit * loadPart = new Core::Circuit;
        
        Core::CircuitIds circuitIds;

        if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias() && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
        {
        	Core::CircuitId loadPartId = circuitIds.voltageBiasLoadPart(index);
        	loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
    		loadPart->setCircuitIdentifier(loadPartId);
        }
        else {
        	Core::CircuitId loadPartId = circuitIds.loadPart(index);
            loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
    		loadPart->setCircuitIdentifier(loadPartId);
        }

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        netNames.push_back(SOURCE_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        
        if(transistorStack1.getMaster().getCircuitIdentifier() != circuitIds.voltageBias()
            || transistorStack2.getMaster().getCircuitIdentifier() != circuitIds.voltageBias() )
        {
            netNames.push_back(INNER_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
        }

		addNetsToCircuit(*loadPart, netNames);
        addTerminalsToCircuit(*loadPart, terminalToNetMap);

        loadPart->addInstance(transistorStack1);
        loadPart->addInstance(transistorStack2);


        connectInstanceTerminalsOfTwoTransistorLoadPart(*loadPart, transistorStack1, transistorStack2);
        transistorStack1.setCircuit(*loadPart);
        transistorStack2.setCircuit(*loadPart);

        return *loadPart;
    }

    	const Core::Circuit & LoadParts::createTwoTransistorLoadPartDifferentSources(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const
    {
        Core::Circuit * loadPart = new Core::Circuit;
        
        Core::CircuitIds circuitIds;
        Core::CircuitId loadPartId = circuitIds.loadPart(index);
        loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
		loadPart->setCircuitIdentifier(loadPartId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        netNames.push_back(SOURCE1_NET_);
        netNames.push_back(SOURCE2_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE1_TERMINAL_, SOURCE1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE2_TERMINAL_, SOURCE2_NET_));
        
        if(transistorStack1.getMaster().getCircuitIdentifier() != circuitIds.voltageBias()
            || transistorStack2.getMaster().getCircuitIdentifier() != circuitIds.voltageBias() )
        {
            netNames.push_back(INNER_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
        }

		addNetsToCircuit(*loadPart, netNames);
        addTerminalsToCircuit(*loadPart, terminalToNetMap);

        loadPart->addInstance(transistorStack1);
        loadPart->addInstance(transistorStack2);

        connectInstanceTerminalsOfTwoTransistorLoadPartDifferentSources(*loadPart, transistorStack1, transistorStack2);
        transistorStack1.setCircuit(*loadPart);
        transistorStack2.setCircuit(*loadPart);

        return *loadPart;
    }

	const Core::Circuit & LoadParts::createThreeTransistorLoadPart(Core::Instance & transistorStack1,
								Core::Instance & transistorStack2, int & index) const
		    {
		        Core::Circuit * loadPart = new Core::Circuit;

		        Core::CircuitIds circuitIds;
		        Core::CircuitId loadPartId = circuitIds.loadPart(index);
		        loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
				loadPart->setCircuitIdentifier(loadPartId);

		        std::vector<Core::NetId> netNames;
		        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

		        netNames.push_back(OUT1_NET_);
		        netNames.push_back(OUT2_NET_);
		        netNames.push_back(SOURCE_NET_);
		        netNames.push_back(INNERTRANSISTORSTACK2_NET_);
		        netNames.push_back(INNERSOURCE_NET_);

		        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
		        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
		        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
		        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2_TERMINAL_, INNERTRANSISTORSTACK2_NET_));
		        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCE_TERMINAL_, INNERSOURCE_NET_));

		        if(isSingleDiodeTransistor(transistorStack1.getMaster()))
		        {
		            netNames.push_back(INNEROUTPUT_NET_);
		            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUT_TERMINAL_, INNEROUTPUT_NET_));
		        }

				addNetsToCircuit(*loadPart, netNames);
		        addTerminalsToCircuit(*loadPart, terminalToNetMap);

		        loadPart->addInstance(transistorStack1);
		        loadPart->addInstance(transistorStack2);


		        connectInstanceTerminalsOfThreeTransistorLoadPart(*loadPart, transistorStack1, transistorStack2);
		        transistorStack1.setCircuit(*loadPart);
		        transistorStack2.setCircuit(*loadPart);

		        return *loadPart;
		    }

	const Core::Circuit & LoadParts::createFourTransistorLoadPart(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const
    {
        Core::Circuit * loadPart = new Core::Circuit;
        
        Core::CircuitIds circuitIds;

        if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias() && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
        {
        	Core::CircuitId loadPartId = circuitIds.voltageBiasLoadPart(index);
        	loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
        	loadPart->setCircuitIdentifier(loadPartId);
        }
        else {
        	Core::CircuitId loadPartId = circuitIds.loadPart(index);
        	loadPartId.setTechType(transistorStack1.getMaster().getCircuitIdentifier().getTechType());
        	loadPart->setCircuitIdentifier(loadPartId);
        }

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        netNames.push_back(INNERTRANSISTORSTACK1_NET_);
        netNames.push_back(INNERTRANSISTORSTACK2_NET_);
        netNames.push_back(SOURCE_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1_TERMINAL_, INNERTRANSISTORSTACK1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2_TERMINAL_, INNERTRANSISTORSTACK2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        
        if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
            && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias() )
        {
            netNames.push_back(OUTOUTPUT1_NET_);
            netNames.push_back(OUTOUTPUT2_NET_);
            netNames.push_back(OUTSOURCE1_NET_);
            netNames.push_back(OUTSOURCE2_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT1_TERMINAL_, OUTOUTPUT1_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT2_TERMINAL_, OUTOUTPUT2_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE1_TERMINAL_, OUTSOURCE1_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE2_TERMINAL_, OUTSOURCE2_NET_));
        }
        else
        {
            netNames.push_back(INNEROUTPUT_NET_);
            netNames.push_back(INNERSOURCE_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUT_TERMINAL_, INNEROUTPUT_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCE_TERMINAL_, INNERSOURCE_NET_));
        }

		addNetsToCircuit(*loadPart, netNames);
        addTerminalsToCircuit(*loadPart, terminalToNetMap);

        loadPart->addInstance(transistorStack1);
        loadPart->addInstance(transistorStack2);


        connectInstanceTerminalsOfFourTransistorLoadPart(*loadPart, transistorStack1, transistorStack2);
        transistorStack1.setCircuit(*loadPart);
        transistorStack2.setCircuit(*loadPart);

        return *loadPart;
    }

	void LoadParts::connectInstanceTerminalsOfTwoTransistorLoadPart(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const
    {
        std::vector<Core::Instance *> instanceVector;
        instanceVector.push_back(&transistorStack1);
        instanceVector.push_back(&transistorStack2);

        Core::CircuitIds circuitIds;
        int num = 1;

        for(auto & transistorStack : instanceVector)
        {
            if(transistorStack->getMaster().getCircuitIdentifier() == circuitIds.currentBias())
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT2_NET_);
                }
                
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::IN_TERMINAL_, INNER_NET_);
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
            }
            else
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT2_NET_);
                }

                if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
                        && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
                {
                    if(num ==1)
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, OUT1_NET_);
                    }
                    else
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, OUT2_NET_);
                    }
                }
                else
                {
                   connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, INNER_NET_); 
                }
                
                connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::SOURCE_TERMINAL_, SOURCE_NET_);
            }

            num++;
        }

    }

    void LoadParts::connectInstanceTerminalsOfTwoTransistorLoadPartDifferentSources(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const
    {
        std::vector<Core::Instance *> instanceVector;
        instanceVector.push_back(&transistorStack1);
        instanceVector.push_back(&transistorStack2);

        Core::CircuitIds circuitIds;
        int num = 1;

        for(auto & transistorStack : instanceVector)
        {
            if(transistorStack->getMaster().getCircuitIdentifier() == circuitIds.currentBias())
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT1_NET_); 
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::SOURCE_TERMINAL_, SOURCE1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT2_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::SOURCE_TERMINAL_, SOURCE2_NET_);
                }
                
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::IN_TERMINAL_, INNER_NET_);
               
            }
            else
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT1_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::SOURCE_TERMINAL_, SOURCE1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT2_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::SOURCE_TERMINAL_, SOURCE2_NET_);
                }

                if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
                        && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
                {
                    if(num ==1)
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, OUT1_NET_);
                    }
                    else
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, OUT2_NET_);
                    }
                }
                else
                {
                   connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUT_TERMINAL_, INNER_NET_); 
                }
                
                
            }

            num++;
        }

    }

	void LoadParts::connectInstanceTerminalsOfThreeTransistorLoadPart(Core::Circuit & loadPart,
								Core::Instance & transistorStack1, Core::Instance & transistorStack2) const
	{
		connectInstanceTerminal(loadPart, transistorStack1, VoltageBiases::IN_TERMINAL_, OUT1_NET_);
		connectInstanceTerminal(loadPart, transistorStack1, VoltageBiases::OUT_TERMINAL_, INNERSOURCE_NET_);
		connectInstanceTerminal(loadPart, transistorStack1, VoltageBiases::SOURCE_TERMINAL_, SOURCE_NET_);

		connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::OUT_TERMINAL_, OUT2_NET_);
		if(isSingleDiodeTransistor(transistorStack1.getMaster()))
		{
		    connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::INOUTPUT_TERMINAL_, INNEROUTPUT_NET_);
		}
		else
		{
		   connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::INOUTPUT_TERMINAL_, OUT1_NET_);
		}

		connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::INSOURCE_TERMINAL_, INNERSOURCE_NET_);
		connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::INNER_TERMINAL_, INNERTRANSISTORSTACK2_NET_);
		connectInstanceTerminal(loadPart, transistorStack2, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
	}

	void LoadParts::connectInstanceTerminalsOfFourTransistorLoadPart(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const
    {
        std::vector<Core::Instance *> instanceVector;
        instanceVector.push_back(&transistorStack1);
        instanceVector.push_back(&transistorStack2);

        Core::CircuitIds circuitIds;
        int num = 1;

        for(auto & transistorStack : instanceVector)
        {
            if(transistorStack->getMaster().getCircuitIdentifier() == circuitIds.currentBias())
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT1_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::INNER_TERMINAL_, INNERTRANSISTORSTACK1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::OUT_TERMINAL_, OUT2_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::INNER_TERMINAL_, INNERTRANSISTORSTACK2_NET_);
                }
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::INOUTPUT_TERMINAL_, INNEROUTPUT_NET_);
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::INSOURCE_TERMINAL_, INNERSOURCE_NET_);
                connectInstanceTerminal(loadPart, *transistorStack, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
            }
            else
            {
                if(num ==1)
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT1_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::INNER_TERMINAL_, INNERTRANSISTORSTACK1_NET_);
                }
                else
                {
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::IN_TERMINAL_, OUT2_NET_);
                    connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::INNER_TERMINAL_, INNERTRANSISTORSTACK2_NET_);
                }

                if(transistorStack1.getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
                        && transistorStack2.getMaster().getCircuitIdentifier() == circuitIds.voltageBias())
                {
                    if(num ==1)
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTINPUT_TERMINAL_, OUTOUTPUT1_NET_);
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTSOURCE_TERMINAL_, OUTSOURCE1_NET_);
                    }
                    else
                    {
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTINPUT_TERMINAL_, OUTOUTPUT2_NET_);
                        connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTSOURCE_TERMINAL_, OUTSOURCE2_NET_);
                    }
                }
                else
                {
                   connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTINPUT_TERMINAL_, INNEROUTPUT_NET_); 
                   connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::OUTSOURCE_TERMINAL_, INNERSOURCE_NET_); 
                }
                
                connectInstanceTerminal(loadPart, *transistorStack, VoltageBiases::SOURCE_TERMINAL_, SOURCE_NET_);
            }

            num++;
        }
    }

    void LoadParts::eraseTwoTransistorsLoadPartsPmosVoltageBiases()
    {
    	for(auto & loadPart : twoTransistorsLoadPartsPmosVoltageBiases_)
    	{
    		delete loadPart;
    	}
    }

    void LoadParts::eraseFourTransistorsLoadPartsPmosVoltageBiases()
    {
    	for(auto & loadPart : fourTransistorsLoadPartsPmosVoltageBiases_)
    	{
    		delete loadPart;
    	}
    }

    void LoadParts::eraseTwoTransistorsLoadPartsNmosVoltageBiases()
    {
    	for(auto & loadPart : twoTransistorsLoadPartsNmosVoltageBiases_)
    	{
    		delete loadPart;
    	}
    }

    void LoadParts::eraseFourTransistorsLoadPartsNmosVoltageBiases()
    {
    	for(auto & loadPart : fourTransistorsLoadPartsNmosVoltageBiases_)
    	{
    		delete loadPart;
    	}

    }

}
