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
#include "Synthesis/incl/Library/HierarchyLevel2/DifferentialPair.h"
#include "Synthesis/incl/Library/HierarchyLevel1/DeviceLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel1/NormalTransistor.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"


#include <sstream>


namespace Synthesis {

    const Core::TerminalName DifferentialPair::OUTPUT1_TERMINAL_ = Core::TerminalName("Output1");
	const Core::TerminalName DifferentialPair::OUTPUT2_TERMINAL_ = Core::TerminalName("Output2");

	const Core::TerminalName DifferentialPair::INPUT1_TERMINAL_ = Core::TerminalName("Input1");
	const Core::TerminalName DifferentialPair::INPUT2_TERMINAL_ = Core::TerminalName("Input2");

	const Core::TerminalName DifferentialPair::SOURCE_TERMINAL_ = Core::TerminalName("Source");

    const Core::InstanceName DifferentialPair::NORMALTRANSISTOR1_ = Core::InstanceName("NormalTransistor1");
    const Core::InstanceName DifferentialPair::NORMALTRANSISTOR2_ = Core::InstanceName("NormalTransistor2");
			
	const Core::NetId DifferentialPair::OUTPUT1_NET_ = Core::NetName("output1").createRootIdentifier();
	const Core::NetId DifferentialPair::OUTPUT2_NET_ = Core::NetName("output2").createRootIdentifier();

	const Core::NetId DifferentialPair::INPUT1_NET_= Core::NetName("input1").createRootIdentifier();
	const Core::NetId DifferentialPair::INPUT2_NET_= Core::NetName("input2").createRootIdentifier();

	const Core::NetId DifferentialPair::SOURCE_NET_ = Core::NetName("source").createRootIdentifier();


	DifferentialPair::DifferentialPair(const DeviceLevel & deviceLevel) :
                differentialPairPmos_(nullptr),
                differentialPairNmos_(nullptr)
    {
        initializeDifferentialPairPmos(deviceLevel);
        initializeDifferentialPairNmos( deviceLevel);
    }
	
    DifferentialPair::~DifferentialPair()
    {
    	delete differentialPairPmos_;
    	delete differentialPairNmos_;
    }

	const Core::Circuit &  DifferentialPair::getDifferentialPairPmos() const
    {
        assert(differentialPairPmos_ != nullptr);
        return *differentialPairPmos_;
    }
			
    const Core::Circuit &  DifferentialPair::getDifferentialPairNmos() const
    {
        assert(differentialPairNmos_ != nullptr);
        return *differentialPairNmos_;
    }

	std::string DifferentialPair::toStr() const
    {
		std::ostringstream oss;
		oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Differential Pairs: " << std::endl;
        oss << differentialPairPmos_->getCircuitIdentifier().toStr() << ": " << std::endl;
		oss << differentialPairPmos_->toStr() << std::endl;

        Core::FlatCircuitRecursion * flatCircuitRecursionPmos = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuitPmos = flatCircuitRecursionPmos->createNewFlatCopy(*differentialPairPmos_);
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitPmos->toStr() << std::endl;

        oss << differentialPairNmos_->getCircuitIdentifier().toStr() << ": " << std::endl;
		oss << differentialPairNmos_->toStr() << std::endl;

        Core::FlatCircuitRecursion * flatCircuitRecursionNmos = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuitNmos = flatCircuitRecursionNmos->createNewFlatCopy(*differentialPairNmos_);
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuitNmos->toStr() << std::endl;

        delete flatCircuitRecursionPmos;
        delete flatCircuitPmos;

        delete flatCircuitRecursionNmos;
        delete flatCircuitNmos;
		return oss.str();
    }

	
	void DifferentialPair::initializeDifferentialPairPmos(const DeviceLevel & deviceLevel) 
    {
        const Core::Circuit & normalTransistorPmos = deviceLevel.getNormalTransistor().getNormalTransistorPmosCircuit();
        int index = 1;
        const Core::Circuit & pmosDifferentialPair = createDifferentialPair(normalTransistorPmos, index);
        differentialPairPmos_ = & pmosDifferentialPair;
    }
	
    void DifferentialPair::initializeDifferentialPairNmos(const DeviceLevel & deviceLevel)
    {
        const Core::Circuit & normalTransistorPmos = deviceLevel.getNormalTransistor().getNormalTransistorNmosCircuit();
        int index = 1;
        const Core::Circuit & nmosDifferentialPair = createDifferentialPair(normalTransistorPmos, index);
        differentialPairNmos_ = & nmosDifferentialPair;
    }

    
    const Core::Circuit & DifferentialPair::createDifferentialPair(const Core::Circuit & normalTransistor, int & index)
    {
            Core::Instance & normalTransistor1 = createInstance(normalTransistor, NORMALTRANSISTOR1_);
            Core::Instance & normalTransistor2 = createInstance(normalTransistor, NORMALTRANSISTOR2_);

            Core::Circuit * differentialPair = new Core::Circuit;

           	Core::CircuitIds circuitIds; 
            Core::CircuitId differentialPairId = circuitIds.differentialPair(index);
            differentialPairId.setTechType(normalTransistor.getCircuitIdentifier().getTechType());
		    differentialPair->setCircuitIdentifier(differentialPairId);

            std::vector<Core::NetId> netNames;
            netNames.push_back(INPUT1_NET_);
            netNames.push_back(INPUT2_NET_);
            netNames.push_back(SOURCE_NET_);
            netNames.push_back(OUTPUT1_NET_);
            netNames.push_back(OUTPUT2_NET_);

		    addNetsToCircuit(*differentialPair, netNames);

            std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT1_TERMINAL_, INPUT1_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUT2_TERMINAL_, INPUT2_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT1_TERMINAL_, OUTPUT1_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT2_TERMINAL_, OUTPUT2_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
            addTerminalsToCircuit(*differentialPair, terminalToNetMap);

            differentialPair->addInstance(normalTransistor1);
            normalTransistor1.setCircuit(*differentialPair);
                        
            differentialPair->addInstance(normalTransistor2);
            normalTransistor2.setCircuit(*differentialPair);

            connectInstanceTerminals(*differentialPair, normalTransistor1, normalTransistor2);

            index++;
            return *differentialPair;
    }

    void DifferentialPair::connectInstanceTerminals(Core::Circuit & differentialPair, Core::Instance & normalTransistor1,
											Core::Instance &  normalTransistor2)
    {
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(NormalTransistor::GATE_TERMINAL_);

        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(NormalTransistor::DRAIN_TERMINAL_);

        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(NormalTransistor::SOURCE_TERMINAL_);
       
        //NormalTransistor1
        Core::InstanceTerminal & gateTerminalNormalTransistor1 = normalTransistor1.findInstanceTerminal(terminalIdGate);
        gateTerminalNormalTransistor1.connect(differentialPair.findNet(INPUT1_NET_));

        Core::InstanceTerminal & drainTerminalNormalTransistor1 = normalTransistor1.findInstanceTerminal(terminalIdDrain);
        drainTerminalNormalTransistor1.connect(differentialPair.findNet(OUTPUT1_NET_));

        Core::InstanceTerminal & sourceTerminalNormalTransistor1 = normalTransistor1.findInstanceTerminal(terminalIdSource);
        sourceTerminalNormalTransistor1.connect(differentialPair.findNet(SOURCE_NET_));
       

        //NormalTransistor2
        Core::InstanceTerminal & gateTerminalNormalTransistor2 = normalTransistor2.findInstanceTerminal(terminalIdGate);
        gateTerminalNormalTransistor2.connect(differentialPair.findNet(INPUT2_NET_));

        Core::InstanceTerminal & drainTerminalNormalTransistor2 = normalTransistor2.findInstanceTerminal(terminalIdDrain);
        drainTerminalNormalTransistor2.connect(differentialPair.findNet(OUTPUT2_NET_));

        Core::InstanceTerminal & sourceTerminalNormalTransistor2 = normalTransistor2.findInstanceTerminal(terminalIdSource);
        sourceTerminalNormalTransistor2.connect(differentialPair.findNet(SOURCE_NET_));
    }


}
