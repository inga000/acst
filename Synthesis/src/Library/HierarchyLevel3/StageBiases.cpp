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
#include "Synthesis/incl/Library/HierarchyLevel3/StageBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"




#include <sstream>


namespace Synthesis {

    const Core::TerminalName StageBiases::IN_TERMINAL_ = Core::TerminalName("In");
    const Core::TerminalName StageBiases::SOURCE_TERMINAL_ = Core::TerminalName("Source");
    const Core::TerminalName StageBiases::OUT_TERMINAL_ = Core::TerminalName("Out");

    const Core::TerminalName StageBiases::INNER_TERMINAL_ = Core::TerminalName("Inner");
    const Core::TerminalName StageBiases::INOUTPUT_TERMINAL_ = Core::TerminalName("InOutput");
    const Core::TerminalName StageBiases::INSOURCE_TERMINAL_ = Core::TerminalName("InSource");

    const Core::InstanceName StageBiases::ONETRANSISTORCURRENTBIAS_ = Core::InstanceName("OneTransistorCurrentBias");
	const Core::InstanceName StageBiases::TWOTRANSISTORCURRENTBIAS_ = Core::InstanceName("TwoTransistorCurrentBias");

	const Core::NetId StageBiases::IN_NET_ = Core::NetName("in").createRootIdentifier();
	const Core::NetId StageBiases::SOURCE_NET_ = Core::NetName("source").createRootIdentifier();
	const Core::NetId StageBiases::OUT_NET_ = Core::NetName("out").createRootIdentifier();

	const Core::NetId StageBiases::INNER_NET_ = Core::NetName("inner").createRootIdentifier();
	const Core::NetId StageBiases::INOUTPUT_NET_ = Core::NetName("inOutput").createRootIdentifier();
	const Core::NetId StageBiases::INSOURCE_NET_ = Core::NetName("inSource").createRootIdentifier();


	StageBiases::StageBiases(const StructuralLevel & structuralLevel)
    {
        initializeStageBiasesPmos(structuralLevel);
        initializeStageBiasesNmos(structuralLevel);
    }
	
    StageBiases::~StageBiases()
    {
		eraseOneTransistorBiasesNmos();
        eraseTwoTransistorBiasesNmos();

		eraseOneTransistorBiasesPmos();
        eraseTwoTransistorBiasesPmos();
    }

	std::vector<const Core::Circuit*>  StageBiases::getAllStageBiasesPmos() const
    {
        std::vector<const Core::Circuit*> stageBiases;

        assert(!oneTransistorBiasesPmos_.empty() || !twoTransistorBiasesPmos_.empty());

        stageBiases.insert(stageBiases.end(), oneTransistorBiasesPmos_.begin(), oneTransistorBiasesPmos_.end());
        stageBiases.insert(stageBiases.end(), twoTransistorBiasesPmos_.begin(), twoTransistorBiasesPmos_.end());

        return stageBiases;
    }
	
    std::vector<const Core::Circuit*>  StageBiases::getAllStageBiasesNmos() const
    {
        std::vector<const Core::Circuit*> stageBiases;

        assert(!oneTransistorBiasesNmos_.empty() || !twoTransistorBiasesNmos_.empty());

        stageBiases.insert(stageBiases.end(), oneTransistorBiasesNmos_.begin(), oneTransistorBiasesNmos_.end());
        stageBiases.insert(stageBiases.end(), twoTransistorBiasesNmos_.begin(), twoTransistorBiasesNmos_.end());

        return stageBiases;
    }

	std::vector<const Core::Circuit*>  StageBiases::getOneTransistorStageBiasesPmos() const
    {
        assert(!oneTransistorBiasesPmos_.empty());
        return oneTransistorBiasesPmos_;
    }
	
    std::vector<const Core::Circuit*>  StageBiases::getOneTransistorStageBiasesNmos() const
    {
        assert(!oneTransistorBiasesNmos_.empty());
        return oneTransistorBiasesNmos_;
    }

	std::vector<const Core::Circuit*>  StageBiases::getTwoTransistorStageBiasesPmos() const
    {
        assert(!twoTransistorBiasesPmos_.empty());
        return twoTransistorBiasesPmos_;
    }

	std::vector<const Core::Circuit*>  StageBiases::getTwoTransistorStageBiasesNmos() const
    {
        assert(!twoTransistorBiasesNmos_.empty());
        return twoTransistorBiasesNmos_;
    }

	std::string StageBiases::toStr() const
    {
        std::ostringstream oss;

         Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< StageBiases: " << std::endl;
        oss << "Pmos:" << std::endl;
        oss << "ONE TRANSISTOR STAGE BIAS" << std::endl;
        for(auto & stageBias : oneTransistorBiasesPmos_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stageBias->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stageBias);
            oss << stageBias->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }
        oss << "TWO TRANSISTOR STAGE BIAS" << std::endl;
        for(auto & stageBias : twoTransistorBiasesPmos_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stageBias->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stageBias);
            oss << stageBias->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }
         oss << "Nmos:" << std::endl;
        oss << "ONE TRANSISTOR STAGE BIAS" << std::endl;
        for(auto & stageBias : oneTransistorBiasesNmos_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stageBias->getCircuitIdentifier().toStr() << ": " << std::endl;  
            oss << stageBias->toStr() << std::endl;
        }
        oss << "TWO TRANSISTOR STAGE BIAS" << std::endl;
        for(auto & stageBias : twoTransistorBiasesNmos_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stageBias->getCircuitIdentifier().toStr() << ": " << std::endl;  
            oss << stageBias->toStr() << std::endl;
        }
        return oss.str();
    }

	
	void StageBiases::initializeStageBiasesPmos(const StructuralLevel & structuralLevel)
    {
        int index = 1;

        std::vector<const Core::Circuit*> oneTransistorCurrentBiases = structuralLevel.getCurrentBiases().getOneTransistorCurrentBiasesPmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = structuralLevel.getCurrentBiases().getTwoTransistorCurrentBiasesPmos();

        oneTransistorBiasesPmos_ = createOneTransistorStageBiases(oneTransistorCurrentBiases, index);
        twoTransistorBiasesPmos_ = createTwoTransistorStageBiases(twoTransistorCurrentBiases, index);

    }
	
    void StageBiases::initializeStageBiasesNmos(const StructuralLevel & structuralLevel)
    {
        int index = 1;
        std::vector<const Core::Circuit*> oneTransistorCurrentBiases = structuralLevel.getCurrentBiases().getOneTransistorCurrentBiasesNmos();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases = structuralLevel.getCurrentBiases().getTwoTransistorCurrentBiasesNmos();

        oneTransistorBiasesNmos_ = createOneTransistorStageBiases(oneTransistorCurrentBiases, index);
        twoTransistorBiasesNmos_ = createTwoTransistorStageBiases(twoTransistorCurrentBiases, index);
    }

    std::vector<const Core::Circuit*> StageBiases::createOneTransistorStageBiases(
						std::vector<const Core::Circuit*> oneTransistorCurrentBiases, int & index)
    {
        std::vector<const Core::Circuit *> oneTransistorStageBiases;

        for(auto & currentBias : oneTransistorCurrentBiases)
        {
            Core::Instance & currentBiasInstance = createInstance(*currentBias, ONETRANSISTORCURRENTBIAS_);
            const Core::Circuit & stageBias  = createOneTransistorStageBias(currentBiasInstance, index);
            oneTransistorStageBiases.push_back(&stageBias);
            index++; 
        }

        return oneTransistorStageBiases;
    }
        	
    std::vector<const Core::Circuit*> StageBiases::createTwoTransistorStageBiases(
							std::vector<const Core::Circuit*> twoTransistorCurrentBiases, int & index)
    {
       std::vector<const Core::Circuit *> twoTransistorStageBiases;

        for(auto & currentBias : twoTransistorCurrentBiases)
        {
            Core::Instance & currentBiasInstance = createInstance(*currentBias, TWOTRANSISTORCURRENTBIAS_);
            const Core::Circuit & stageBias  = createTwoTransistorStageBias(currentBiasInstance, index);
            twoTransistorStageBiases.push_back(&stageBias);
            index++; 
        }

        return twoTransistorStageBiases;
    }

    const Core::Circuit & StageBiases::createOneTransistorStageBias(Core::Instance & currentBias, int & index)
    {
        Core::Circuit * stageBias = new Core::Circuit;
        
        Core::CircuitIds circuitIds;
        Core::CircuitId stageBiasId = circuitIds.stageBias(index);
        stageBiasId.setTechType(currentBias.getMaster().getCircuitIdentifier().getTechType());
		stageBias->setCircuitIdentifier(stageBiasId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT_NET_);
        netNames.push_back(IN_NET_);
        netNames.push_back(SOURCE_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_TERMINAL_, IN_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        
		addNetsToCircuit(*stageBias, netNames);
        addTerminalsToCircuit(*stageBias, terminalToNetMap);

        stageBias->addInstance(currentBias);

        connectInstanceTerminalsOfOneTransistorStageBias(*stageBias, currentBias);
        currentBias.setCircuit(*stageBias);

        return *stageBias;
    }

	const Core::Circuit & StageBiases::createTwoTransistorStageBias(Core::Instance & currentBias, int & index)
    {
        Core::Circuit * stageBias = new Core::Circuit;
        
        Core::CircuitIds circuitIds;
        Core::CircuitId stageBiasId = circuitIds.stageBias(index);
        stageBiasId.setTechType(currentBias.getMaster().getCircuitIdentifier().getTechType());
		stageBias->setCircuitIdentifier(stageBiasId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT_NET_);
        netNames.push_back(INOUTPUT_NET_);
        netNames.push_back(INNER_NET_);
        netNames.push_back(INSOURCE_NET_);
        netNames.push_back(SOURCE_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUT_TERMINAL_, INOUTPUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCE_TERMINAL_, INSOURCE_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        
		addNetsToCircuit(*stageBias, netNames);
        addTerminalsToCircuit(*stageBias, terminalToNetMap);

        stageBias->addInstance(currentBias);

        connectInstanceTerminalsOfTwoTransistorStageBias(*stageBias, currentBias);
        currentBias.setCircuit(*stageBias);

        return *stageBias;
    }

    void StageBiases::connectInstanceTerminalsOfOneTransistorStageBias(Core::Circuit & stageBias, Core::Instance & currentBias)
    {
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::IN_TERMINAL_, IN_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::OUT_TERMINAL_, OUT_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
    }

	void StageBiases::connectInstanceTerminalsOfTwoTransistorStageBias(Core::Circuit & stageBias, Core::Instance & currentBias)
    {
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::INOUTPUT_TERMINAL_, INOUTPUT_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::INSOURCE_TERMINAL_, INSOURCE_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::INNER_TERMINAL_, INNER_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::OUT_TERMINAL_, OUT_NET_);
        connectInstanceTerminal(stageBias, currentBias, CurrentBiases::SOURCE_TERMINAL_, SOURCE_NET_);
    }

	void StageBiases::eraseOneTransistorBiasesNmos()
	{
		for(auto & stageBias : oneTransistorBiasesNmos_)
		{
			delete stageBias;
		}
	}

    void StageBiases::eraseTwoTransistorBiasesNmos()
	{
		for(auto & stageBias : twoTransistorBiasesNmos_)
		{
			delete stageBias;
		}
	}

	void StageBiases::eraseOneTransistorBiasesPmos()
	{
		for(auto & stageBias : oneTransistorBiasesPmos_)
		{
			delete stageBias;
		}
	}

    void StageBiases::eraseTwoTransistorBiasesPmos()
	{
		for(auto & stageBias : twoTransistorBiasesPmos_)
		{
			delete stageBias;
		}
	}

}
