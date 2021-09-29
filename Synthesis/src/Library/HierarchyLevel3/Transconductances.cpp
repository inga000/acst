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
#include "Synthesis/incl/Library/HierarchyLevel3/Transconductances.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/DifferentialPair.h"


#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"


#include <sstream>


namespace Synthesis {

    const Core::TerminalName Transconductances::INPUT1_TERMINAL_ = Core::TerminalName("Input1");
	const Core::TerminalName Transconductances::INPUT2_TERMINAL_ = Core::TerminalName("Input2");

	const Core::TerminalName Transconductances::INNER_TERMINAL_ = Core::TerminalName("Inner");
	
    const Core::TerminalName Transconductances::OUT1_TERMINAL_ = Core::TerminalName("Out1");
	const Core::TerminalName Transconductances::OUT2_TERMINAL_ = Core::TerminalName("Out2");
	const Core::TerminalName Transconductances::OUT1NMOS_TERMINAL_ = Core::TerminalName("Out1Nmos");
	const Core::TerminalName Transconductances::OUT2NMOS_TERMINAL_ = Core::TerminalName("Out2Nmos");
	const Core::TerminalName Transconductances::OUT1PMOS_TERMINAL_ = Core::TerminalName("Out1Pmos");
	const Core::TerminalName Transconductances::OUT2PMOS_TERMINAL_ = Core::TerminalName("Out2Pmos");

	const Core::TerminalName Transconductances::SOURCE_TERMINAL_ = Core::TerminalName("Source");
    const Core::TerminalName Transconductances::SOURCE1_TERMINAL_ = Core::TerminalName("Source1");
    const Core::TerminalName Transconductances::SOURCE2_TERMINAL_ = Core::TerminalName("Source2");
	const Core::TerminalName Transconductances::SOURCENMOS_TERMINAL_ = Core::TerminalName("SourceNmos");
	const Core::TerminalName Transconductances::SOURCEPMOS_TERMINAL_ = Core::TerminalName("SourcePmos");

    const Core::InstanceName Transconductances::DIFFERENTIALPAIR_ = Core::InstanceName("DifferentialPair");
	const Core::InstanceName Transconductances::DIFFERENTIALPAIR1_ = Core::InstanceName("DifferentialPair1");
	const Core::InstanceName Transconductances::DIFFERENTIALPAIR2_ = Core::InstanceName("DifferentialPair2");
	const Core::InstanceName Transconductances::DIFFERENTIALPAIRNMOS_ = Core::InstanceName("DifferentialPairNmos");
	const Core::InstanceName Transconductances::DIFFERENTIALPAIRPMOS_ = Core::InstanceName("DifferentialPairPmos");

	const Core::NetId Transconductances::INPUT1_NET_ = Core::NetName("Input1").createRootIdentifier();
	const Core::NetId Transconductances::INPUT2_NET_ = Core::NetName("Input2").createRootIdentifier();

	const Core::NetId Transconductances::INNER_NET_ = Core::NetName("Inner").createRootIdentifier();
			
	const Core::NetId Transconductances::OUT1_NET_ = Core::NetName("Out1").createRootIdentifier();
	const Core::NetId Transconductances::OUT2_NET_ = Core::NetName("Out2").createRootIdentifier();
	const Core::NetId Transconductances::OUT1NMOS_NET_ = Core::NetName("Out1Nmos").createRootIdentifier();
	const Core::NetId Transconductances::OUT2NMOS_NET_ = Core::NetName("Out2Nmos").createRootIdentifier();
	const Core::NetId Transconductances::OUT1PMOS_NET_ = Core::NetName("Out1Pmos").createRootIdentifier();
	const Core::NetId Transconductances::OUT2PMOS_NET_ = Core::NetName("Out2Pmos").createRootIdentifier();

	const Core::NetId Transconductances::SOURCE_NET_ = Core::NetName("Source").createRootIdentifier();
    const Core::NetId Transconductances::SOURCE1_NET_ = Core::NetName("Source1").createRootIdentifier();
    const Core::NetId Transconductances::SOURCE2_NET_ = Core::NetName("Source2").createRootIdentifier();
	const Core::NetId Transconductances::SOURCENMOS_NET_ = Core::NetName("SourceNmos").createRootIdentifier();
	const Core::NetId Transconductances::SOURCEPMOS_NET_ = Core::NetName("SourcePmos").createRootIdentifier();

	Transconductances::Transconductances(const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation) :
	            simpleTransconductanceNmos_(nullptr),
                simpleTransconductancePmos_(nullptr),
                feedbackTransconductanceNmos_(nullptr),
                feedbackTransconductancePmos_(nullptr),
                complementaryTransconductance_(nullptr)
    {
		initializeTransconductances(structuralLevel, circuitInformation);
    }
	
    Transconductances::~Transconductances()
    {
		delete simpleTransconductanceNmos_;
        delete feedbackTransconductanceNmos_;

		delete simpleTransconductancePmos_;
        delete feedbackTransconductancePmos_;

		delete complementaryTransconductance_;
    }

    const Core::Circuit & Transconductances::getSimpleTransconductancePmos() const
    {
        assert(simpleTransconductancePmos_ != nullptr);
        return * simpleTransconductancePmos_;
    }

    const Core::Circuit & Transconductances::getSimpleTransconductanceNmos() const
    {
        assert(simpleTransconductanceNmos_ != nullptr);
        return * simpleTransconductanceNmos_;
    }

	const Core::Circuit & Transconductances::getFeedbackTransconductancePmos() const
    {
        assert(feedbackTransconductancePmos_ != nullptr);
        return * feedbackTransconductancePmos_;
    }
	
    const Core::Circuit &  Transconductances::getFeedbackTransconductanceNmos() const
    {
        assert(feedbackTransconductanceNmos_ != nullptr);
        return * feedbackTransconductanceNmos_;
    }

	const Core::Circuit &  Transconductances::getComplementaryTransconductance() const
    {
        assert(complementaryTransconductance_ != nullptr);
        return * complementaryTransconductance_;
    }

	std::string Transconductances::toStr() const
    {
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Transconductances: " << std::endl;
        oss << "Simple Transconductances:" << std::endl;
        
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << simpleTransconductancePmos_->getCircuitIdentifier().toStr() << ": " << std::endl;      
        Core::Circuit * flatCircuitSimpleTransconductancePmos = flatCircuitRecursion->createNewFlatCopy(*simpleTransconductancePmos_);
        oss << simpleTransconductancePmos_->toStr() << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitSimpleTransconductancePmos->toStr() << std::endl;
        delete flatCircuitSimpleTransconductancePmos;

        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << simpleTransconductanceNmos_->getCircuitIdentifier().toStr() << ": " << std::endl;      
        Core::Circuit * flatCircuitSimpleTransconductanceNmos = flatCircuitRecursion->createNewFlatCopy(*simpleTransconductanceNmos_);
        oss << simpleTransconductanceNmos_->toStr() << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitSimpleTransconductanceNmos->toStr() << std::endl;
        delete flatCircuitSimpleTransconductanceNmos;

        oss << "Feedback: " << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << feedbackTransconductancePmos_->getCircuitIdentifier().toStr() << ": " << std::endl;      
        Core::Circuit * flatCircuitFeedbackTransconductancePmos = flatCircuitRecursion->createNewFlatCopy(*feedbackTransconductancePmos_);
        oss << simpleTransconductancePmos_->toStr() << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitFeedbackTransconductancePmos->toStr() << std::endl;
        delete flatCircuitFeedbackTransconductancePmos;

        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << feedbackTransconductanceNmos_->getCircuitIdentifier().toStr() << ": " << std::endl;      
        Core::Circuit * flatCircuitFeedbackTransconductanceNmos = flatCircuitRecursion->createNewFlatCopy(*feedbackTransconductanceNmos_);
        oss << simpleTransconductanceNmos_->toStr() << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitFeedbackTransconductanceNmos->toStr() << std::endl;
        delete flatCircuitFeedbackTransconductanceNmos;


        oss << "Complementary: " << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << complementaryTransconductance_->getCircuitIdentifier().toStr() << ": " << std::endl;      
        Core::Circuit * flatCircuitComplementaryTransconductance = flatCircuitRecursion->createNewFlatCopy(*complementaryTransconductance_);
        oss << complementaryTransconductance_->toStr() << std::endl;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitComplementaryTransconductance->toStr() << std::endl;
        delete flatCircuitComplementaryTransconductance;
      

        return oss.str();
    }

	void Transconductances::initializeTransconductances(const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation)
    {
        int index = 1;
        const Core::Circuit & differentialPairPmos = structuralLevel.getDifferentialPair().getDifferentialPairPmos();
        const Core::Circuit & differentialPairNmos = structuralLevel.getDifferentialPair().getDifferentialPairNmos();

        if(!circuitInformation.getCircuitParameter().isComplementary())
        {
        	simpleTransconductancePmos_ = &createSimpleTransconductance(differentialPairPmos, index);
        	simpleTransconductanceNmos_ = &createSimpleTransconductance(differentialPairNmos, index);

        	if(circuitInformation.getCircuitParameter().isFullyDifferential())
        	{
        		feedbackTransconductancePmos_ = &createFeedbackTransconductance(differentialPairPmos, index);
        		feedbackTransconductanceNmos_ = &createFeedbackTransconductance(differentialPairNmos, index);
        	}


        }
        
        if(circuitInformation.getCircuitParameter().isComplementary())
        {
			complementaryTransconductance_ = &createComplementaryTransconductance(differentialPairPmos, differentialPairNmos, index);
        }
    }
	
    const Core::Circuit & Transconductances::createSimpleTransconductance(const Core::Circuit & differentialPair, int & index)
    {
        Core::Circuit * transconductance = new Core::Circuit;
        
        Core::Instance & differentialPairInstance = createInstance(differentialPair, DIFFERENTIALPAIR_);

        Core::CircuitIds circuitIds;
        Core::CircuitId transconductanceId = circuitIds.transconductance(index);
        transconductanceId.setTechType(differentialPair.getCircuitIdentifier().getTechType());
		transconductance->setCircuitIdentifier(transconductanceId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        netNames.push_back(INPUT1_NET_);
        netNames.push_back(INPUT2_NET_);
        netNames.push_back(SOURCE_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT1_TERMINAL_, INPUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT2_TERMINAL_, INPUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        
		addNetsToCircuit(*transconductance, netNames);
        addTerminalsToCircuit(*transconductance, terminalToNetMap);

        transconductance->addInstance(differentialPairInstance);

        connectInstanceTerminalsOfSimpleTransconductance(*transconductance, differentialPairInstance);
        differentialPairInstance.setCircuit(*transconductance);
        index ++;
        return *transconductance;
    }

	
    const Core::Circuit& Transconductances::createFeedbackTransconductance(const Core::Circuit & differentialPair, int & index)
    {
        Core::Circuit * transconductance = new Core::Circuit;
        
        Core::Instance & differentialPair1Instance = createInstance(differentialPair, DIFFERENTIALPAIR1_);
        Core::Instance & differentialPair2Instance = createInstance(differentialPair, DIFFERENTIALPAIR2_);

        Core::CircuitIds circuitIds;
        Core::CircuitId transconductanceId = circuitIds.transconductance(index);
        transconductanceId.setTechType(differentialPair.getCircuitIdentifier().getTechType());
		transconductance->setCircuitIdentifier(transconductanceId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
        netNames.push_back(INPUT1_NET_);
        netNames.push_back(INPUT2_NET_);
        netNames.push_back(INNER_NET_);
        netNames.push_back(SOURCE1_NET_);
        netNames.push_back(SOURCE2_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT1_TERMINAL_, INPUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT2_TERMINAL_, INPUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE1_TERMINAL_, SOURCE1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE2_TERMINAL_, SOURCE2_NET_));

		addNetsToCircuit(*transconductance, netNames);
        addTerminalsToCircuit(*transconductance, terminalToNetMap);

        transconductance->addInstance(differentialPair1Instance);
        transconductance->addInstance(differentialPair2Instance);

        connectInstanceTerminalsOfFeedbackTransconductance(*transconductance, differentialPair1Instance, differentialPair2Instance);
        differentialPair1Instance.setCircuit(*transconductance);
        differentialPair2Instance.setCircuit(*transconductance);
        
        index ++;
        return *transconductance;
    }
       		
    const Core::Circuit & Transconductances::createComplementaryTransconductance(const Core::Circuit & differentialPairPmos,
			   						const Core::Circuit & differentialPairNmos, int & index)
    {
        Core::Circuit * transconductance = new Core::Circuit;
        
        Core::Instance & differentialPairNmosInstance = createInstance(differentialPairNmos, DIFFERENTIALPAIRNMOS_);
        Core::Instance & differentialPairPmosInstance = createInstance(differentialPairPmos, DIFFERENTIALPAIRPMOS_);

        Core::CircuitIds circuitIds;
        Core::CircuitId transconductanceId = circuitIds.transconductance(index);
        transconductanceId.setTechType(Core::TechType::undefined());
		transconductance->setCircuitIdentifier(transconductanceId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUT1NMOS_NET_);
        netNames.push_back(OUT2NMOS_NET_);
        netNames.push_back(OUT1PMOS_NET_);
        netNames.push_back(OUT2PMOS_NET_);
        netNames.push_back(INPUT1_NET_);
        netNames.push_back(INPUT2_NET_);
        netNames.push_back(SOURCENMOS_NET_);
        netNames.push_back(SOURCEPMOS_NET_);
        
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1NMOS_TERMINAL_, OUT1NMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2NMOS_TERMINAL_, OUT2NMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1PMOS_TERMINAL_, OUT1PMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2PMOS_TERMINAL_, OUT2PMOS_NET_)); 
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT1_TERMINAL_, INPUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT2_TERMINAL_, INPUT2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));

		addNetsToCircuit(*transconductance, netNames);
        addTerminalsToCircuit(*transconductance, terminalToNetMap);

        transconductance->addInstance(differentialPairPmosInstance);
        transconductance->addInstance(differentialPairNmosInstance);

        connectInstanceTerminalsOfComplementaryTransconductance(*transconductance, differentialPairNmosInstance, differentialPairPmosInstance);
        differentialPairPmosInstance.setCircuit(*transconductance);
        differentialPairNmosInstance.setCircuit(*transconductance);
        
        index ++;
        return *transconductance;
    }

    void Transconductances::connectInstanceTerminalsOfSimpleTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPair)
    {
        connectInstanceTerminal(transconductance, differentialPair, DifferentialPair::INPUT1_TERMINAL_, INPUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPair, DifferentialPair::INPUT2_TERMINAL_, INPUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPair, DifferentialPair::OUTPUT1_TERMINAL_, OUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPair, DifferentialPair::OUTPUT2_TERMINAL_, OUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPair, DifferentialPair::SOURCE_TERMINAL_, SOURCE_NET_);
    }

	void Transconductances::connectInstanceTerminalsOfFeedbackTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPair1, Core::Instance & differentialPair2)
    {
        connectInstanceTerminal(transconductance, differentialPair1, DifferentialPair::INPUT1_TERMINAL_, INPUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPair1, DifferentialPair::INPUT2_TERMINAL_, INNER_NET_);
        connectInstanceTerminal(transconductance, differentialPair1, DifferentialPair::OUTPUT1_TERMINAL_, OUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPair1, DifferentialPair::OUTPUT2_TERMINAL_, OUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPair1, DifferentialPair::SOURCE_TERMINAL_, SOURCE1_NET_);

        connectInstanceTerminal(transconductance, differentialPair2, DifferentialPair::INPUT1_TERMINAL_, INPUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPair2, DifferentialPair::INPUT2_TERMINAL_, INNER_NET_);
        connectInstanceTerminal(transconductance, differentialPair2, DifferentialPair::OUTPUT1_TERMINAL_, OUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPair2, DifferentialPair::OUTPUT2_TERMINAL_, OUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPair2, DifferentialPair::SOURCE_TERMINAL_, SOURCE2_NET_);
    }
	
    void Transconductances::connectInstanceTerminalsOfComplementaryTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPairNmos, Core::Instance & differentialPairPmos)
    {
        connectInstanceTerminal(transconductance, differentialPairNmos, DifferentialPair::INPUT1_TERMINAL_, INPUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPairNmos, DifferentialPair::INPUT2_TERMINAL_, INPUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPairNmos, DifferentialPair::OUTPUT1_TERMINAL_, OUT1NMOS_NET_);
        connectInstanceTerminal(transconductance, differentialPairNmos, DifferentialPair::OUTPUT2_TERMINAL_, OUT2NMOS_NET_);
        connectInstanceTerminal(transconductance, differentialPairNmos, DifferentialPair::SOURCE_TERMINAL_, SOURCENMOS_NET_);

        connectInstanceTerminal(transconductance, differentialPairPmos, DifferentialPair::INPUT1_TERMINAL_, INPUT1_NET_);
        connectInstanceTerminal(transconductance, differentialPairPmos, DifferentialPair::INPUT2_TERMINAL_, INPUT2_NET_);
        connectInstanceTerminal(transconductance, differentialPairPmos, DifferentialPair::OUTPUT1_TERMINAL_, OUT1PMOS_NET_);
        connectInstanceTerminal(transconductance, differentialPairPmos, DifferentialPair::OUTPUT2_TERMINAL_, OUT2PMOS_NET_);
        connectInstanceTerminal(transconductance, differentialPairPmos, DifferentialPair::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
    }

    


}
