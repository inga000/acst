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
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel1/DeviceLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel1/NormalTransistor.h"
#include "Synthesis/incl/Library/HierarchyLevel1/DiodeTransistor.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include <sstream>
#include <vector>

namespace Synthesis {

    const Core::TerminalName CurrentBiases::IN_TERMINAL_ = Core::TerminalName("In");
	const Core::TerminalName CurrentBiases::SOURCE_TERMINAL_ = Core::TerminalName("Source");
	const Core::TerminalName CurrentBiases::OUT_TERMINAL_ = Core::TerminalName("Out");

	const Core::TerminalName CurrentBiases::INNER_TERMINAL_ = Core::TerminalName("inner");
	const Core::TerminalName CurrentBiases::INOUTPUT_TERMINAL_ = Core::TerminalName("inOutput");
	const Core::TerminalName CurrentBiases::INSOURCE_TERMINAL_ = Core::TerminalName("inSource");

	const Core::InstanceName CurrentBiases::TRANSISTOR_ = Core::InstanceName("Transistor");
	const Core::InstanceName CurrentBiases::SOURCETRANSISTOR_ = Core::InstanceName("SourceTransistor");;
	const Core::InstanceName CurrentBiases::OUTPUTTRANSISTOR_ = Core::InstanceName("OutputTransistor");;

    const Core::NetId CurrentBiases::IN_NET_ = Core::NetName("in").createRootIdentifier();
	const Core::NetId CurrentBiases::SOURCE_NET_ = Core::NetName("source").createRootIdentifier();
	const Core::NetId CurrentBiases::OUT_NET_ = Core::NetName("out").createRootIdentifier();

	const Core::NetId CurrentBiases::INNER_NET_ = Core::NetName("inner").createRootIdentifier();
	const Core::NetId CurrentBiases::INOUTPUT_NET_ = Core::NetName("inOutput").createRootIdentifier();
	const Core::NetId CurrentBiases::INSOURCE_NET_ = Core::NetName("inSource").createRootIdentifier();


	CurrentBiases::CurrentBiases(const DeviceLevel & deviceLevel)
    {
        initializeCurrentBiasesPmos(deviceLevel);
        initializeCurrentBiasesNmos( deviceLevel);
    }
	
    CurrentBiases::~CurrentBiases()
    {
		eraseOneTransistorCurrentBiasesPmos();
		eraseTwoTransistorCurrentBiasesPmos();
		eraseOneTransistorCurrentBiasesNmos();
		eraseTwoTransistorCurrentBiasesNmos();
    }

	std::vector<const Core::Circuit*>  CurrentBiases::getAllCurrentBiasesPmos() const
    {
        std::vector<const Core::Circuit*> allCurrentBiases;
        allCurrentBiases.insert(allCurrentBiases.end(), oneTransistorCurrentBiasesPmos_.begin(), oneTransistorCurrentBiasesPmos_.end());
        allCurrentBiases.insert(allCurrentBiases.end(), twoTransistorCurrentBiasesPmos_.begin(), twoTransistorCurrentBiasesPmos_.end());
        return allCurrentBiases;
   }
			
    std::vector<const Core::Circuit*>  CurrentBiases::getAllCurrentBiasesNmos() const
    {
        std::vector<const Core::Circuit*> allCurrentBiases;
        allCurrentBiases.insert(allCurrentBiases.end(), oneTransistorCurrentBiasesNmos_.begin(), oneTransistorCurrentBiasesNmos_.end());
        allCurrentBiases.insert(allCurrentBiases.end(), twoTransistorCurrentBiasesNmos_.begin(), twoTransistorCurrentBiasesNmos_.end());
        return allCurrentBiases;
    }

    std::vector<const Core::Circuit*>   CurrentBiases::getOneTransistorCurrentBiasesPmos() const
    {
        assert(!oneTransistorCurrentBiasesPmos_.empty());
        return oneTransistorCurrentBiasesPmos_;
    }
	
    std::vector<const Core::Circuit*>  CurrentBiases::getTwoTransistorCurrentBiasesPmos() const
    {
        assert(!twoTransistorCurrentBiasesPmos_.empty());
        return twoTransistorCurrentBiasesPmos_;
    }
	
    std::vector<const Core::Circuit*>  CurrentBiases::getOneTransistorCurrentBiasesNmos() const
    {
        assert(!oneTransistorCurrentBiasesNmos_.empty());
        return oneTransistorCurrentBiasesNmos_;
    }
			
    std::vector<const Core::Circuit*>  CurrentBiases::getTwoTransistorCurrentBiasesNmos() const
    {
        assert(!twoTransistorCurrentBiasesNmos_.empty());
        return twoTransistorCurrentBiasesNmos_;
    }

    const Core::Circuit & CurrentBiases::getNormalTransistorCurrentBias(Core::TechType techType) const
    {
    	const Core::Circuit * normalTransistorCurrentBias = nullptr;
    	if(techType.isN())
    	{
    		for(auto & currentBias : oneTransistorCurrentBiasesNmos_ )
    		{
    			if(!isSingleDiodeTransistor(*currentBias))
    			{
    				normalTransistorCurrentBias = currentBias;
    				break;
    			}
    		}
    	}
    	else
    	{
    		for(auto & currentBias : oneTransistorCurrentBiasesPmos_ )
    		{
    			if(!isSingleDiodeTransistor(*currentBias))
    			{
    				normalTransistorCurrentBias = currentBias;
    				break;
    			}
    		}
    	}
    	assert(normalTransistorCurrentBias != nullptr);
    	return * normalTransistorCurrentBias;
    }
   

	std::string CurrentBiases::toStr() const
    {
        std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Current Biases: " << std::endl;
        oss << "Pmos:" << std::endl;
        oss << "ONE TRANSISTOR CURRENT BIASES" << std::endl;
        for(auto & currentBias : oneTransistorCurrentBiasesPmos_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << currentBias->getCircuitIdentifier().toStr() << std::endl;
            oss << currentBias->toStr() << std::endl;
        }
        oss << "TWO TRANSISTORS CURRENT BIASES" << std::endl;
        for(auto & currentBias : twoTransistorCurrentBiasesPmos_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << currentBias->getCircuitIdentifier().toStr() << std::endl;
            oss << currentBias->toStr() << std::endl;
           
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*currentBias);

            delete flatCircuit;
            delete flatCircuitRecursion;
        }
    
        oss << "Nmos:" << std::endl;
        oss << "ONE TRANSISTOR CURRENT BIASES" << std::endl;
        for(auto & currentBias : oneTransistorCurrentBiasesNmos_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << currentBias->getCircuitIdentifier().toStr() << std::endl;
            oss << currentBias->toStr() << std::endl;
        }
        oss << "TWO TRANSISTORS CURRENT BIASES" << std::endl;
        for(auto & currentBias : twoTransistorCurrentBiasesNmos_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << currentBias->getCircuitIdentifier().toStr() << std::endl;
            oss << currentBias->toStr() << std::endl;
        }

        return oss.str();
    }

	
	void CurrentBiases::initializeCurrentBiasesPmos(const DeviceLevel & deviceLevel)
    {
        int num = 1;
        initializeOneTransistorCurrentBiasesPmos(deviceLevel, num);
        initializeTwoTransistorCurrentBiasesPmos(deviceLevel, num);
    }
	
    void CurrentBiases::initializeCurrentBiasesNmos(const DeviceLevel & deviceLevel)
    {
        int num =1;
        initializeOneTransistorCurrentBiasesNmos(deviceLevel, num);
        initializeTwoTransistorCurrentBiasesNmos(deviceLevel, num);
    }

    void CurrentBiases::initializeOneTransistorCurrentBiasesPmos(const DeviceLevel & deviceLevel, int & num)
    {
        Core::Instance & normalTransistor = deviceLevel.getNormalTransistor().createNewNormalTransistorInstancePmos(TRANSISTOR_);
        std::vector<const Core::Circuit*> oneTransistorCurrentBiasesPmos = createOneTransistorCurrentBias(normalTransistor, num);
        oneTransistorCurrentBiasesPmos_ = oneTransistorCurrentBiasesPmos;
    }
        	
    void CurrentBiases::initializeTwoTransistorCurrentBiasesPmos(const DeviceLevel & deviceLevel, int & num)
    {
        const Core::Circuit & normalTransistorPmos = deviceLevel.getNormalTransistor().getNormalTransistorPmosCircuit();
        const Core::Circuit & diodeTransistorPmos = deviceLevel.getDiodeTransistor().getDiodeTransistorPmosCircuit();
        
        std::vector<const Core::Circuit*> twoTransistorCurrentBiasesPmos = createTwoTransistorCurrentBias(normalTransistorPmos, diodeTransistorPmos, num);
        twoTransistorCurrentBiasesPmos_ = twoTransistorCurrentBiasesPmos;
    }

	void CurrentBiases::initializeOneTransistorCurrentBiasesNmos(const DeviceLevel & deviceLevel, int & num)
    {
        Core::Instance & normalTransistor = deviceLevel.getNormalTransistor().createNewNormalTransistorInstanceNmos(TRANSISTOR_);
        std::vector<const Core::Circuit*> oneTransistorCurrentBiasesNmos = createOneTransistorCurrentBias(normalTransistor, num);
        oneTransistorCurrentBiasesNmos_ = oneTransistorCurrentBiasesNmos;
    }
        	
    void CurrentBiases::initializeTwoTransistorCurrentBiasesNmos(const DeviceLevel & deviceLevel, int & num)
    {
        const Core::Circuit & normalTransistorNmos = deviceLevel.getNormalTransistor().getNormalTransistorNmosCircuit();
        const Core::Circuit & diodeTransistorNmos = deviceLevel.getDiodeTransistor().getDiodeTransistorNmosCircuit();
        std::vector<const Core::Circuit*> twoTransistorCurrentBiasesNmos = createTwoTransistorCurrentBias(normalTransistorNmos, diodeTransistorNmos, num);
        twoTransistorCurrentBiasesNmos_ = twoTransistorCurrentBiasesNmos;
    }

    std::vector<const Core::Circuit*> CurrentBiases::createOneTransistorCurrentBias(Core::Instance & normalTransistor, int & num)
    {
        std::vector<const Core::Circuit*> oneTransistorCurrentBiases;
       
        Core::Circuit * circuit = new Core::Circuit;

		Core::CircuitIds circuitIds;
        Core::CircuitId currentBiasId = circuitIds.currentBias(num);
        currentBiasId.setTechType(normalTransistor.getMaster().getCircuitIdentifier().getTechType());
		circuit->setCircuitIdentifier(currentBiasId);

        std::vector<Core::NetId> netNames;
        netNames.push_back(IN_NET_);
        netNames.push_back(OUT_NET_);
        netNames.push_back(SOURCE_NET_);
		addNetsToCircuit(*circuit, netNames);

        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_TERMINAL_, IN_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        addTerminalsToCircuit(*circuit, terminalToNetMap);

        circuit->addInstance(normalTransistor);
        connectInstanceTerminalsOneTransistorCurrentBias(*circuit, normalTransistor);
        normalTransistor.setCircuit(*circuit);

		oneTransistorCurrentBiases.push_back(circuit);
        num ++;

        return oneTransistorCurrentBiases;
    }


	std::vector<const Core::Circuit*> CurrentBiases::createTwoTransistorCurrentBias(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & num)
    {
        std::vector<const Core::Circuit*> twoTransistorCurrentBiases;

        const Core::Circuit & firstTwoTransistorCurrentBias = createTwoTransistorCircuit(createInstance(normalTransistor, SOURCETRANSISTOR_), createInstance(normalTransistor, OUTPUTTRANSISTOR_),num);
        const Core::Circuit & secondTwoTransistorCurrentBias = createTwoTransistorCircuit(createInstance(diodeTransistor, SOURCETRANSISTOR_), createInstance(normalTransistor, OUTPUTTRANSISTOR_), num);

        twoTransistorCurrentBiases.push_back(&firstTwoTransistorCurrentBias);
        twoTransistorCurrentBiases.push_back(&secondTwoTransistorCurrentBias);
        return twoTransistorCurrentBiases;
    }

    const Core::Circuit& CurrentBiases::createTwoTransistorCircuit(Core::Instance & sourceTransistor, Core::Instance & outputTransistor, int  & index)
    {
        Core::Circuit * circuit = new Core::Circuit;

        std::ostringstream name;
		
		Core::CircuitIds circuitIds; 
        Core::CircuitId currentBiasId = circuitIds.currentBias(index);
        currentBiasId.setTechType(sourceTransistor.getMaster().getCircuitIdentifier().getTechType());
		circuit->setCircuitIdentifier(currentBiasId);

        std::vector<Core::NetId> netNames;
        netNames.push_back(INSOURCE_NET_);
        netNames.push_back(INOUTPUT_NET_);
        netNames.push_back(INNER_NET_);
        netNames.push_back(OUT_NET_);
        netNames.push_back(SOURCE_NET_);
		addNetsToCircuit(*circuit, netNames);

        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCE_TERMINAL_, INSOURCE_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUT_TERMINAL_, INOUTPUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        addTerminalsToCircuit(*circuit, terminalToNetMap);

        circuit->addInstance(sourceTransistor);
        circuit->addInstance(outputTransistor);
        connectInstanceTerminalsTwoTransistorCurrentBias(*circuit, sourceTransistor, outputTransistor);

        sourceTransistor.setCircuit(*circuit);
        outputTransistor.setCircuit(*circuit);
        index ++;
		return *circuit;
    }

    void CurrentBiases::connectInstanceTerminalsOneTransistorCurrentBias(Core::Circuit & circuit, 
                Core::Instance & transistor)
    {
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(NormalTransistor::GATE_TERMINAL_);

        Core::InstanceTerminal & gateTerminal = transistor.findInstanceTerminal(terminalIdGate);
        gateTerminal.connect(circuit.findNet(IN_NET_));

  
        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(NormalTransistor::DRAIN_TERMINAL_);

        Core::InstanceTerminal & drainTerminal = transistor.findInstanceTerminal(terminalIdDrain);
        drainTerminal.connect(circuit.findNet(OUT_NET_));


        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(NormalTransistor::SOURCE_TERMINAL_);

        Core::InstanceTerminal & sourceTerminal = transistor.findInstanceTerminal(terminalIdSource);
        sourceTerminal.connect(circuit.findNet(SOURCE_NET_));

    }

    void CurrentBiases::connectInstanceTerminalsTwoTransistorCurrentBias(Core::Circuit & circuit, 
                    Core::Instance & sourceTransistor, Core::Instance & outputTransistor)
    {
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(NormalTransistor::GATE_TERMINAL_);

        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(NormalTransistor::DRAIN_TERMINAL_);

        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(NormalTransistor::SOURCE_TERMINAL_);

        Core::InstanceTerminal & gateTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdGate);
        gateTerminalSourceTransistor.connect(circuit.findNet(INSOURCE_NET_));

        Core::InstanceTerminal & drainTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdDrain);
        drainTerminalSourceTransistor.connect(circuit.findNet(INNER_NET_));

        Core::InstanceTerminal & sourceTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdSource);
        sourceTerminalSourceTransistor.connect(circuit.findNet(SOURCE_NET_));

        Core::InstanceTerminal & gateTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdGate);
        gateTerminalOutputTransistor.connect(circuit.findNet(INOUTPUT_NET_));

        Core::InstanceTerminal & drainTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdDrain);
        drainTerminalOutputTransistor.connect(circuit.findNet(OUT_NET_));

        Core::InstanceTerminal & sourceTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdSource);
        sourceTerminalOutputTransistor.connect(circuit.findNet(INNER_NET_));
    }

	void CurrentBiases::eraseOneTransistorCurrentBiasesPmos()
	{
		for(auto & currentBias : oneTransistorCurrentBiasesPmos_)
			delete currentBias;
	}

	void CurrentBiases::eraseTwoTransistorCurrentBiasesPmos()
	{
		for(auto & currentBias : twoTransistorCurrentBiasesPmos_)
			delete currentBias;
	}

	void CurrentBiases::eraseOneTransistorCurrentBiasesNmos()
	{
		for(auto & currentBias : oneTransistorCurrentBiasesNmos_)
			delete currentBias;
	}

	void CurrentBiases::eraseTwoTransistorCurrentBiasesNmos()
	{
		for(auto & currentBias : twoTransistorCurrentBiasesNmos_)
			delete currentBias;
	}

}
