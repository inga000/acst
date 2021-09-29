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
#include "Synthesis/incl/Library/HierarchyLevel2/VoltageBiases.h"
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


namespace Synthesis {

    const Core::TerminalName VoltageBiases::IN_TERMINAL_ = Core::TerminalName("In");
	const Core::TerminalName VoltageBiases::SOURCE_TERMINAL_ = Core::TerminalName("Source");
	const Core::TerminalName VoltageBiases::OUT_TERMINAL_ = Core::TerminalName("Out");

	const Core::TerminalName VoltageBiases::INNER_TERMINAL_ = Core::TerminalName("Inner");
	const Core::TerminalName VoltageBiases::OUTINPUT_TERMINAL_ = Core::TerminalName("OutInput");
	const Core::TerminalName VoltageBiases::OUTSOURCE_TERMINAL_ = Core::TerminalName("OutSource");

	const Core::NetId VoltageBiases::IN_NET_ = Core::NetName("in").createRootIdentifier();
	const Core::NetId VoltageBiases::SOURCE_NET_ = Core::NetName("source").createRootIdentifier();
	const Core::NetId VoltageBiases::OUT_NET_ = Core::NetName("out").createRootIdentifier();

	const Core::NetId VoltageBiases::INNER_NET_ = Core::NetName("inner").createRootIdentifier();
	const Core::NetId VoltageBiases::OUTINPUT_NET_ = Core::NetName("outInput").createRootIdentifier();
	const Core::NetId VoltageBiases::OUTSOURCE_NET_ = Core::NetName("outSource").createRootIdentifier();

    const Core::InstanceName VoltageBiases::TRANSISTOR_ = Core::InstanceName("Transistor");
	 const Core::InstanceName VoltageBiases::SOURCETRANSISTOR_ = Core::InstanceName("SourceTransistor");
	const Core::InstanceName VoltageBiases::OUTPUTTRANSISTOR_ = Core::InstanceName("OutputTransistor");

	VoltageBiases::VoltageBiases(const DeviceLevel & deviceLevel)
    {
        initializeVoltageBiasesPmos(deviceLevel);
        initializeVoltageBiasesNmos(deviceLevel);
    }
	
    VoltageBiases::~VoltageBiases()
    {
		eraseOneTransistorVoltageBiasesPmos();
		eraseTwoTransistorVoltageBiasesPmos();
		eraseOneTransistorVoltageBiasesNmos();
		eraseTwoTransistorVoltageBiasesNmos();
    }

	std::vector<const Core::Circuit*>  VoltageBiases::getAllVoltageBiasesPmos() const
    {
        std::vector<const Core::Circuit*> allVoltageBiases;
        allVoltageBiases.insert(allVoltageBiases.end(), oneTransistorVoltageBiasesPmos_.begin(), oneTransistorVoltageBiasesPmos_.end());
        allVoltageBiases.insert(allVoltageBiases.end(), twoTransistorVoltageBiasesPmos_.begin(), twoTransistorVoltageBiasesPmos_.end());
        return allVoltageBiases;
   }
			
    std::vector<const Core::Circuit*>  VoltageBiases::getAllVoltageBiasesNmos() const
    {
        std::vector<const Core::Circuit*> allVoltageBiases;
        allVoltageBiases.insert(allVoltageBiases.end(), oneTransistorVoltageBiasesNmos_.begin(), oneTransistorVoltageBiasesNmos_.end());
        allVoltageBiases.insert(allVoltageBiases.end(), twoTransistorVoltageBiasesNmos_.begin(), twoTransistorVoltageBiasesNmos_.end());
        return allVoltageBiases;
    }

    std::vector<const Core::Circuit*>   VoltageBiases::getOneTransistorVoltageBiasesPmos() const
    {
        assert(!oneTransistorVoltageBiasesPmos_.empty());
        return oneTransistorVoltageBiasesPmos_;
    }
	
    std::vector<const Core::Circuit*>  VoltageBiases::getTwoTransistorVoltageBiasesPmos() const
    {
        assert(!twoTransistorVoltageBiasesPmos_.empty());
        return twoTransistorVoltageBiasesPmos_;
    }
	
    std::vector<const Core::Circuit*>  VoltageBiases::getOneTransistorVoltageBiasesNmos() const
    {
        assert(!oneTransistorVoltageBiasesNmos_.empty());
        return oneTransistorVoltageBiasesNmos_;
    }
			
    std::vector<const Core::Circuit*>  VoltageBiases::getTwoTransistorVoltageBiasesNmos() const
    {
        assert(!twoTransistorVoltageBiasesNmos_.empty());
        return twoTransistorVoltageBiasesNmos_;
    }

	const Core::Circuit& VoltageBiases::getDiodeTransistorVoltageBiasNmos() const
	{
		const Core::Circuit * diodeTransistorVoltageBias = nullptr;

		for(auto & voltageBias : oneTransistorVoltageBiasesNmos_)
		{
			if(isSingleDiodeTransistor(*voltageBias))
			{
				diodeTransistorVoltageBias = voltageBias;
				break;
			}
		}

		assert(diodeTransistorVoltageBias != nullptr);
		return *diodeTransistorVoltageBias;
	}

	const Core::Circuit& VoltageBiases::getDiodeTransistorVoltageBiasPmos() const
	{
		const Core::Circuit * diodeTransistorVoltageBias = nullptr;

		for(auto & voltageBias : oneTransistorVoltageBiasesPmos_)
		{
			if(isSingleDiodeTransistor(*voltageBias))
			{
				diodeTransistorVoltageBias = voltageBias;
				break;
			}
		}

		assert(diodeTransistorVoltageBias != nullptr);
		return *diodeTransistorVoltageBias;
	}

	const Core::Circuit& VoltageBiases::getTwoDiodeTransistorVoltageBiasNmos() const
	{
		const Core::Circuit * twoDiodeTransistorVoltageBias = nullptr;

		for(auto & voltageBias : getTwoTransistorVoltageBiasesNmos())
		{
			if(sourceTransistorIsDiodeTransistor(*voltageBias)
					&& outputTransistorIsDiodeTransistor(*voltageBias))
			{
				twoDiodeTransistorVoltageBias = voltageBias;
				break;
			}
		}

		assert(twoDiodeTransistorVoltageBias != nullptr);
		return *twoDiodeTransistorVoltageBias;
	}
	const Core::Circuit& VoltageBiases::getTwoDiodeTransistorVoltageBiasPmos() const
	{
		const Core::Circuit * twoDiodeTransistorVoltageBias = nullptr;

		for(auto & voltageBias : getTwoTransistorVoltageBiasesPmos())
		{
			if(sourceTransistorIsDiodeTransistor(*voltageBias)
					&& outputTransistorIsDiodeTransistor(*voltageBias))
			{
				twoDiodeTransistorVoltageBias = voltageBias;
				break;
			}
		}

		assert(twoDiodeTransistorVoltageBias != nullptr);
		return *twoDiodeTransistorVoltageBias;
	}

	std::string VoltageBiases::toStr() const
    {
        std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Voltage Biases: " << std::endl;
        oss << "Pmos:" << std::endl;
        oss << "ONE TRANSISTOR Voltage BIASES" << std::endl;
        for(auto & voltageBias : oneTransistorVoltageBiasesPmos_)
        {
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*voltageBias);
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << voltageBias->getCircuitIdentifier().toStr() << std::endl;
            oss << voltageBias->toStr() << std::endl;
        }
        oss << "TWO TRANSISTORS Voltage BIASES" << std::endl;
        for(auto & voltageBias : twoTransistorVoltageBiasesPmos_)
        {
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*voltageBias);
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << voltageBias->getCircuitIdentifier().toStr() << std::endl;
            oss << voltageBias->toStr() << std::endl;
        }
    
        oss << "Nmos:" << std::endl;
        oss << "ONE TRANSISTOR Voltage BIASES" << std::endl;
        for(auto & voltageBias : oneTransistorVoltageBiasesNmos_)
        {
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*voltageBias);
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << voltageBias->getCircuitIdentifier().toStr() << std::endl;
            oss << voltageBias->toStr() << std::endl;
        }
        oss << "TWO TRANSISTORS Voltage BIASES" << std::endl;
        for(auto & voltageBias : twoTransistorVoltageBiasesNmos_)
        {
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*voltageBias);
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << voltageBias->getCircuitIdentifier().toStr() << std::endl;
            oss << voltageBias->toStr() << std::endl;
        }

        return oss.str();
    }

	
	void VoltageBiases::initializeVoltageBiasesPmos(const DeviceLevel & deviceLevel)
    {
        int index = 1;
        initializeOneTransistorVoltageBiasesPmos(deviceLevel, index);
        initializeTwoTransistorVoltageBiasesPmos(deviceLevel, index);
    }
	
    void VoltageBiases::initializeVoltageBiasesNmos(const DeviceLevel & deviceLevel)
    {
        int index = 1;
        initializeOneTransistorVoltageBiasesNmos(deviceLevel, index);
        initializeTwoTransistorVoltageBiasesNmos(deviceLevel, index);
    }

    void  VoltageBiases::initializeOneTransistorVoltageBiasesPmos(const DeviceLevel &deviceLevel, int & index)
    {
        const Core::Circuit & normalTransistorPmos = deviceLevel.getNormalTransistor().getNormalTransistorPmosCircuit();
        const Core::Circuit & diodeTransistorPmos = deviceLevel.getDiodeTransistor().getDiodeTransistorPmosCircuit();
        
        std::vector<const Core::Circuit*> oneTransistorVoltageBiasesPmos = createOneTransistorVoltageBiases(normalTransistorPmos, diodeTransistorPmos, index);
        oneTransistorVoltageBiasesPmos_ = oneTransistorVoltageBiasesPmos;
    }
    
    void  VoltageBiases::initializeTwoTransistorVoltageBiasesPmos(const DeviceLevel &deviceLevel, int & index)
    {
        const Core::Circuit & normalTransistorPmos = deviceLevel.getNormalTransistor().getNormalTransistorPmosCircuit();
        const Core::Circuit & diodeTransistorPmos = deviceLevel.getDiodeTransistor().getDiodeTransistorPmosCircuit();
        
        std::vector<const Core::Circuit*> twoTransistorVoltageBiasesPmos = createTwoTransistorVoltageBiases(normalTransistorPmos, diodeTransistorPmos, index);
        twoTransistorVoltageBiasesPmos_ = twoTransistorVoltageBiasesPmos;
    }

	void  VoltageBiases::initializeOneTransistorVoltageBiasesNmos(const DeviceLevel &deviceLevel, int & index)
    {
        const Core::Circuit & normalTransistorNmos = deviceLevel.getNormalTransistor().getNormalTransistorNmosCircuit();
        const Core::Circuit & diodeTransistorNmos = deviceLevel.getDiodeTransistor().getDiodeTransistorNmosCircuit();
        
        std::vector<const Core::Circuit*> oneTransistorVoltageBiasesNmos = createOneTransistorVoltageBiases(normalTransistorNmos, diodeTransistorNmos, index);
        oneTransistorVoltageBiasesNmos_ = oneTransistorVoltageBiasesNmos;
    }
        	
    void  VoltageBiases::initializeTwoTransistorVoltageBiasesNmos(const DeviceLevel &deviceLevel, int & index)
    {
        const Core::Circuit & normalTransistorNmos = deviceLevel.getNormalTransistor().getNormalTransistorNmosCircuit();
        const Core::Circuit & diodeTransistorNmos = deviceLevel.getDiodeTransistor().getDiodeTransistorNmosCircuit();
        
        std::vector<const Core::Circuit*> twoTransistorVoltageBiasesNmos = createTwoTransistorVoltageBiases(normalTransistorNmos, diodeTransistorNmos, index);
        twoTransistorVoltageBiasesNmos_ = twoTransistorVoltageBiasesNmos;
    }
	std::vector<const Core::Circuit*> VoltageBiases::createOneTransistorVoltageBiases(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & index)
    {
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases;

        Core::Circuit & firstOneTransistorVoltageBias = createOneTransistorCircuit(createInstance(normalTransistor, TRANSISTOR_), index);
        Core::Circuit & secondOneTransistorVoltageBias = createOneTransistorCircuit(createInstance(diodeTransistor, TRANSISTOR_), index);

        oneTransistorVoltageBiases.push_back(&firstOneTransistorVoltageBias);
        oneTransistorVoltageBiases.push_back(&secondOneTransistorVoltageBias);
        return oneTransistorVoltageBiases;
    }

	std::vector<const Core::Circuit*> VoltageBiases::createTwoTransistorVoltageBiases(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & num)
    {
        std::vector<const Core::Circuit*> twoTransistorVoltageBiases;

        std::vector<const Core::Circuit *> twoDiodeTransistorCircuit = createTwoTransistorCircuit(diodeTransistor, diodeTransistor, num);
        std::vector<const Core::Circuit *> twoNormalTransistorCircuit = createTwoTransistorCircuit(normalTransistor, normalTransistor, num);
        std::vector<const Core::Circuit *> mixedCircuits = createTwoTransistorCircuit(normalTransistor, diodeTransistor, num);

        twoTransistorVoltageBiases.insert(twoTransistorVoltageBiases.end(), twoDiodeTransistorCircuit.begin(), twoDiodeTransistorCircuit.end());
        twoTransistorVoltageBiases.insert(twoTransistorVoltageBiases.end(), twoNormalTransistorCircuit.begin(), twoNormalTransistorCircuit.end());
        twoTransistorVoltageBiases.insert(twoTransistorVoltageBiases.end(), mixedCircuits.begin(), mixedCircuits.end());

        return twoTransistorVoltageBiases;
    }

    Core::Circuit & VoltageBiases::createOneTransistorCircuit(Core::Instance & instance, int  &  num)
    {
        Core::Circuit * circuit = new Core::Circuit;


		
		Core::CircuitIds circuitIds; 
        Core::CircuitId voltageBiasId = circuitIds.voltageBias(num);
        voltageBiasId.setTechType(instance.getMaster().getCircuitIdentifier().getTechType());
		circuit->setCircuitIdentifier(voltageBiasId);

        std::vector<Core::NetId> netNames;
        netNames.push_back(IN_NET_);
        netNames.push_back(SOURCE_NET_);
        netNames.push_back(OUT_NET_);

		addNetsToCircuit(*circuit, netNames);

        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_TERMINAL_, IN_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
        addTerminalsToCircuit(*circuit, terminalToNetMap);
      
        circuit->addInstance(instance);
        connectInstanceTerminalsOneTransistorVoltageBias(*circuit, instance);

        instance.setCircuit(*circuit);
        num ++;
		return *circuit;
    }

	std::vector<const Core::Circuit *> VoltageBiases::createTwoTransistorCircuit(const Core::Circuit & sourceTransistor, const Core::Circuit & outputTransistor, int & num)
    {
        std::vector<const Core::Circuit*> voltageBiases;
        Core::CircuitIds circuitIds;

        if(outputTransistor.getCircuitIdentifier() == circuitIds.diodeTransistor())
        {
            Core::Instance & sourceInstance = createInstance(sourceTransistor, SOURCETRANSISTOR_);
            Core::Instance & outputInstance = createInstance(outputTransistor, OUTPUTTRANSISTOR_);

            Core::Circuit * circuit = new Core::Circuit;

           	Core::CircuitIds circuitIds; 
            Core::CircuitId voltageBiasId = circuitIds.voltageBias(num);
            voltageBiasId.setTechType(sourceTransistor.getCircuitIdentifier().getTechType());
		    circuit->setCircuitIdentifier(voltageBiasId);

            std::vector<Core::NetId> netNames;
            netNames.push_back(IN_NET_);
            netNames.push_back(SOURCE_NET_);
            netNames.push_back(INNER_NET_);
            netNames.push_back(OUTSOURCE_NET_);
            netNames.push_back(OUTINPUT_NET_);

		    addNetsToCircuit(*circuit, netNames);

            std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_TERMINAL_, IN_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE_TERMINAL_, OUTSOURCE_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTINPUT_TERMINAL_, OUTINPUT_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
            addTerminalsToCircuit(*circuit, terminalToNetMap);

            circuit->addInstance(sourceInstance);
            sourceInstance.setCircuit(*circuit);
                        
            circuit->addInstance(outputInstance);
            outputInstance.setCircuit(*circuit);

            connectInstanceTerminalsTwoTransistorVoltageBias(*circuit, sourceInstance, outputInstance);
            
		    voltageBiases.push_back(circuit);
            num++;

        }
        
        if(sourceTransistor.getCircuitIdentifier() == circuitIds.normalTransistor())
        {
            Core::Instance & sourceInstance = createInstance(sourceTransistor, SOURCETRANSISTOR_);
            Core::Instance & outputInstance = createInstance(outputTransistor, OUTPUTTRANSISTOR_);

            Core::Circuit * circuit = new Core::Circuit;

            Core::CircuitIds circuitIds; 
            Core::CircuitId voltageBiasId = circuitIds.voltageBias(num);
            voltageBiasId.setTechType(sourceTransistor.getCircuitIdentifier().getTechType());
		    circuit->setCircuitIdentifier(voltageBiasId);

            std::vector<Core::NetId> netNames;
            netNames.push_back(IN_NET_);
            netNames.push_back(SOURCE_NET_);
            netNames.push_back(INNER_NET_);
            netNames.push_back(OUTINPUT_NET_);
		    addNetsToCircuit(*circuit, netNames);
            
            std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_TERMINAL_, IN_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE_TERMINAL_, IN_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTINPUT_TERMINAL_, OUTINPUT_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_TERMINAL_, INNER_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_TERMINAL_, SOURCE_NET_));
            addTerminalsToCircuit(*circuit, terminalToNetMap);

            circuit->addInstance(sourceInstance);
            sourceInstance.setCircuit(*circuit);
                        
            circuit->addInstance(outputInstance);
            outputInstance.setCircuit(*circuit);

            connectInstanceTerminalsTwoTransistorVoltageBias(*circuit, sourceInstance, outputInstance);
            
		    voltageBiases.push_back(circuit);
            num++;
        }

        return voltageBiases;

    }

    void VoltageBiases::connectInstanceTerminalsOneTransistorVoltageBias(Core::Circuit & circuit, Core::Instance & transistor)
    {
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(NormalTransistor::GATE_TERMINAL_);

        Core::InstanceTerminal & gateTerminal = transistor.findInstanceTerminal(terminalIdGate);
        gateTerminal.connect(circuit.findNet(OUT_NET_));

        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(NormalTransistor::DRAIN_TERMINAL_);

        Core::InstanceTerminal & drainTerminal = transistor.findInstanceTerminal(terminalIdDrain);
        drainTerminal.connect(circuit.findNet(IN_NET_));


        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(NormalTransistor::SOURCE_TERMINAL_);

        Core::InstanceTerminal & sourceTerminal = transistor.findInstanceTerminal(terminalIdSource);
        sourceTerminal.connect(circuit.findNet(SOURCE_NET_));
    }
			
    void VoltageBiases::connectInstanceTerminalsTwoTransistorVoltageBias(Core::Circuit & circuit, 
			Core::Instance & sourceTransistor, Core::Instance & outputTransistor)
    {
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(NormalTransistor::GATE_TERMINAL_);

        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(NormalTransistor::DRAIN_TERMINAL_);

        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(NormalTransistor::SOURCE_TERMINAL_);
       
        //SourceTransistor
        Core::InstanceTerminal & gateTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdGate);
        if(circuit.hasNet(OUTSOURCE_NET_))
        {
            gateTerminalSourceTransistor.connect(circuit.findNet(OUTSOURCE_NET_));
        }
        else
        {
            gateTerminalSourceTransistor.connect(circuit.findNet(IN_NET_));
        }

        Core::InstanceTerminal & drainTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdDrain);
        drainTerminalSourceTransistor.connect(circuit.findNet(INNER_NET_));

        Core::InstanceTerminal & sourceTerminalSourceTransistor = sourceTransistor.findInstanceTerminal(terminalIdSource);
        sourceTerminalSourceTransistor.connect(circuit.findNet(SOURCE_NET_));

        // OutputTransistor
        Core::InstanceTerminal & gateTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdGate);
        gateTerminalOutputTransistor.connect(circuit.findNet(OUTINPUT_NET_));

        Core::InstanceTerminal & drainTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdDrain);
        drainTerminalOutputTransistor.connect(circuit.findNet(IN_NET_));

        Core::InstanceTerminal & sourceTerminalOutputTransistor = outputTransistor.findInstanceTerminal(terminalIdSource);
        sourceTerminalOutputTransistor.connect(circuit.findNet(INNER_NET_));
    }

	void VoltageBiases::eraseOneTransistorVoltageBiasesPmos()
	{
		for(auto & voltageBias : oneTransistorVoltageBiasesPmos_)
		{
			delete voltageBias;
		}
	}

	void VoltageBiases::eraseTwoTransistorVoltageBiasesPmos()
	{
		for(auto & voltageBias : twoTransistorVoltageBiasesPmos_)
		{
			delete voltageBias;
		}
	}

	void VoltageBiases::eraseOneTransistorVoltageBiasesNmos()
	{
		for(auto & voltageBias : oneTransistorVoltageBiasesNmos_)
		{
			delete voltageBias;
		}
	}

	void VoltageBiases::eraseTwoTransistorVoltageBiasesNmos()
	{
		for(auto & voltageBias : twoTransistorVoltageBiasesNmos_)
		{
			delete voltageBias;
		}
	}
}
