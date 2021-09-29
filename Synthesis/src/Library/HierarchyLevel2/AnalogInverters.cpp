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
#include "Synthesis/incl/Library/HierarchyLevel2/AnalogInverters.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Device/TechType.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"


#include <sstream>


namespace Synthesis {

    const Core::TerminalName AnalogInverters::OUTPUT_TERMINAL_ = Core::TerminalName("Output");
			
	const Core::TerminalName AnalogInverters::SOURCE_CURRENTBIASNMOS_TERMINAL_ = Core::TerminalName("SourceNmos");
	const Core::TerminalName AnalogInverters::SOURCE_CURRENTBIASPMOS_TERMINAL_ = Core::TerminalName("SourcePmos");
			
	const Core::TerminalName AnalogInverters::IN_CURRENTBIASNMOS_TERMINAL_ = Core::TerminalName("InCurrentBiasNmos");
	const Core::TerminalName AnalogInverters::IN_CURRENTBIASPMOS_TERMINAL_ = Core::TerminalName("InCurrentBiasPmos");

	const Core::TerminalName AnalogInverters::INSOURCE_CURRENTBIASNMOS_TERMINAL_ = Core::TerminalName("InSourceCurrentBiasNmos");
	const Core::TerminalName AnalogInverters::INOUTPUT_CURRENTBIASNMOS_TERMINAL_ = Core::TerminalName("InOutputCurrentBiasNmos");
	const Core::TerminalName AnalogInverters::INOUTPUT_CURRENTBIASPMOS_TERMINAL_ = Core::TerminalName("InOutputCurrentBiasPmos");
	const Core::TerminalName AnalogInverters::INSOURCE_CURRENTBIASPMOS_TERMINAL_ = Core::TerminalName("InSourceCurrentBiasPmos");

	const Core::TerminalName AnalogInverters::INNER_CURRENTBIASNMOS_TERMINAL_ = Core::TerminalName("InnerCurrentBiasNmos");
	const Core::TerminalName AnalogInverters::INNER_CURRENTBIASPMOS_TERMINAL_ = Core::TerminalName("InnerCurrentBiasPmos");

    const Core::InstanceName AnalogInverters::CURRENTBIASNMOS_ = Core::InstanceName("CurrentBiasNmos");
	const Core::InstanceName AnalogInverters::CURRENTBIASPMOS_ = Core::InstanceName("CurrentBiasPmos");
			
	const Core::NetId AnalogInverters::OUTPUT_NET_ = Core::NetName("output").createRootIdentifier();
			
	const Core::NetId AnalogInverters::SOURCE_CURRENTBIASNMOS_NET_ = Core::NetName("sourceCurrentBiasNmos").createRootIdentifier();
	const Core::NetId AnalogInverters::SOURCE_CURRENTBIASPMOS_NET_ = Core::NetName("sourceCurrentBiasPmos").createRootIdentifier();
			
	const Core::NetId AnalogInverters::IN_CURRENTBIASNMOS_NET_ = Core::NetName("inCurrentBiasNmos").createRootIdentifier();
	const Core::NetId AnalogInverters::IN_CURRENTBIASPMOS_NET_ = Core::NetName("inCurrentBiasPmos").createRootIdentifier();

	const Core::NetId AnalogInverters::INSOURCE_CURRENTBIASNMOS_NET_ = Core::NetName("inSourceCurrentBiasNmos").createRootIdentifier();
	const Core::NetId AnalogInverters::INOUTPUT_CURRENTBIASNMOS_NET_ = Core::NetName("inOutputCurrentBiasNmos").createRootIdentifier();
	const Core::NetId AnalogInverters::INOUTPUT_CURRENTBIASPMOS_NET_ = Core::NetName("inOutputCurrentBiasPmos").createRootIdentifier();
	const Core::NetId AnalogInverters::INSOURCE_CURRENTBIASPMOS_NET_ = Core::NetName("inSourceCurrentBiasPmos").createRootIdentifier();

	const Core::NetId AnalogInverters::INNER_CURRENTBIASNMOS_NET_ = Core::NetName("innerCurrentBiasNmos").createRootIdentifier();
	const Core::NetId AnalogInverters::INNER_CURRENTBIASPMOS_NET_= Core::NetName("innerCurrentBiasPmos").createRootIdentifier();


	AnalogInverters::AnalogInverters(StructuralLevel & structuralLevel)
    {
		initializeAnalogInverters(structuralLevel);
    }
	
    AnalogInverters::~AnalogInverters()
    {
        eraseAllAnalogInverters();
    }

	std::vector<const Core::Circuit*>  AnalogInverters::getAnalogInverters() const
    {
        assert(!analogInverters_.empty());
        return analogInverters_;
    }
			

	std::string AnalogInverters::toStr() const
    {
        std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Analog Inverters: " << std::endl;
        for(auto & inverter : analogInverters_)
        {
            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*inverter);
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << inverter->getCircuitIdentifier().toStr()<< std::endl;
            oss << inverter->toStr() << std::endl;
            delete flatCircuitRecursion;
            delete flatCircuit;
        }
    
       

        return oss.str();
    }

	
	void AnalogInverters::initializeAnalogInverters(StructuralLevel & structuralLevel)
    {
        std::vector<const Core::Circuit *> analogInverters;
        std::vector<const Core::Circuit *> currentBiasesPmos = structuralLevel.getCurrentBiases().getAllCurrentBiasesPmos();
        std::vector<const Core::Circuit *> currentBiasesNmos = structuralLevel.getCurrentBiases().getAllCurrentBiasesNmos();

        int  index = 1;
        for(auto & currentBiasPmos : currentBiasesPmos)
        {
            for(auto & currentBiasNmos : currentBiasesNmos)
            {
                if(currentBiasPmos->findInstances().size() == 2 && currentBiasNmos->findInstances().size() == 2)
                {
                    if(!(currentBiasPmos->getGateNetsNotConnectedToADrain().size() ==1 
                                && currentBiasNmos->getGateNetsNotConnectedToADrain().size() ==1 ))
                    {
                        const Core::Circuit & analogInverter = createNewAnalogInverter(*currentBiasPmos, *currentBiasNmos, index);
                        analogInverters.push_back(&analogInverter);
                    }
                }
                else
                {
                        const Core::Circuit & analogInverter = createNewAnalogInverter(*currentBiasPmos, *currentBiasNmos, index);
                        analogInverters.push_back(&analogInverter);
                }
            }
        }
        analogInverters_ = analogInverters;
    }
	
    const Core::Circuit & AnalogInverters::createNewAnalogInverter(const Core::Circuit & currentBiasPmos, const Core::Circuit & currentBiasNmos, int & index)
    {
        Core::Instance & currentBiasPmosInstance = createInstance(currentBiasPmos, CURRENTBIASPMOS_);
        Core::Instance & currentBiasNmosInstance = createInstance(currentBiasNmos, CURRENTBIASNMOS_);
        
        Core::Circuit * analogInverter = new Core::Circuit;

        Core::CircuitIds circuitIds;
        Core::CircuitId analogInverterId = circuitIds.analogInverter(index);
        analogInverterId.setTechType(Core::TechType::undefined());
		analogInverter->setCircuitIdentifier(analogInverterId);

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUTPUT_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT_TERMINAL_, OUTPUT_NET_));

        netNames.push_back(SOURCE_CURRENTBIASNMOS_NET_);
        netNames.push_back(SOURCE_CURRENTBIASPMOS_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_CURRENTBIASNMOS_TERMINAL_, SOURCE_CURRENTBIASNMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCE_CURRENTBIASPMOS_TERMINAL_, SOURCE_CURRENTBIASPMOS_NET_));


        if(currentBiasPmos.findInstances().size() == 2)
        {
            netNames.push_back(INSOURCE_CURRENTBIASPMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCE_CURRENTBIASPMOS_TERMINAL_, INSOURCE_CURRENTBIASPMOS_NET_));
            netNames.push_back(INOUTPUT_CURRENTBIASPMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUT_CURRENTBIASPMOS_TERMINAL_, INOUTPUT_CURRENTBIASPMOS_NET_));
            netNames.push_back(INNER_CURRENTBIASPMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_CURRENTBIASPMOS_TERMINAL_, INNER_CURRENTBIASPMOS_NET_));
        }
        else if(currentBiasPmos.findInstances().size() == 1)
        {
            netNames.push_back(IN_CURRENTBIASPMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_CURRENTBIASPMOS_TERMINAL_, IN_CURRENTBIASPMOS_NET_));
        }
        if(currentBiasNmos.findInstances().size() == 2)
        {
            netNames.push_back(INSOURCE_CURRENTBIASNMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCE_CURRENTBIASNMOS_TERMINAL_, INSOURCE_CURRENTBIASNMOS_NET_));
            netNames.push_back(INOUTPUT_CURRENTBIASNMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUT_CURRENTBIASNMOS_TERMINAL_, INOUTPUT_CURRENTBIASNMOS_NET_));
            netNames.push_back(INNER_CURRENTBIASNMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNER_CURRENTBIASNMOS_TERMINAL_, INNER_CURRENTBIASNMOS_NET_));
        }
        else if(currentBiasNmos.findInstances().size() == 1)
        {
            netNames.push_back(IN_CURRENTBIASNMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN_CURRENTBIASNMOS_TERMINAL_, IN_CURRENTBIASNMOS_NET_));
        }



		addNetsToCircuit(*analogInverter, netNames);
        addTerminalsToCircuit(*analogInverter, terminalToNetMap);

        analogInverter->addInstance(currentBiasPmosInstance);
        analogInverter->addInstance(currentBiasNmosInstance);


        connectInstanceTerminals(*analogInverter, currentBiasNmosInstance, currentBiasPmosInstance);
        currentBiasPmosInstance.setCircuit(*analogInverter);
        currentBiasNmosInstance.setCircuit(*analogInverter);

		
        index++;

        return *analogInverter;
    }

    void AnalogInverters::connectInstanceTerminals(Core::Circuit & circuit, Core::Instance & currentBiasNmosInstance, 
									Core::Instance & currentBiasPmosInstance)
    {
        Core::TerminalId terminalIdOut;
        terminalIdOut.setTerminalName(CurrentBiases::OUT_TERMINAL_);

        Core::InstanceTerminal & outTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdOut);
        outTerminalCurrentBiasPmos.connect(circuit.findNet(OUTPUT_NET_));

        Core::InstanceTerminal & outTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdOut);
        outTerminalCurrentBiasNmos.connect(circuit.findNet(OUTPUT_NET_));


        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(CurrentBiases::SOURCE_TERMINAL_);

        Core::InstanceTerminal & sourceTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdSource);
        sourceTerminalCurrentBiasPmos.connect(circuit.findNet(SOURCE_CURRENTBIASPMOS_NET_));

        Core::InstanceTerminal & sourceTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdSource);
        sourceTerminalCurrentBiasNmos.connect(circuit.findNet(SOURCE_CURRENTBIASNMOS_NET_));
       
       
        Core::TerminalId terminalIdIn;
        terminalIdIn.setTerminalName(CurrentBiases::IN_TERMINAL_);

        Core::TerminalId terminalIdInner;
        terminalIdInner.setTerminalName(CurrentBiases::INNER_TERMINAL_);

        Core::TerminalId terminalIdInOutput;
        terminalIdInOutput.setTerminalName(CurrentBiases::INOUTPUT_TERMINAL_);

        Core::TerminalId terminalIdInSource;
        terminalIdInSource.setTerminalName(CurrentBiases::INSOURCE_TERMINAL_);
       
        if(currentBiasPmosInstance.getMaster().findInstances().size() == 2)
        {
            Core::InstanceTerminal & inSourceTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdInSource);
            inSourceTerminalCurrentBiasPmos.connect(circuit.findNet(INSOURCE_CURRENTBIASPMOS_NET_));

            Core::InstanceTerminal & inOutputTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdInOutput);
            inOutputTerminalCurrentBiasPmos.connect(circuit.findNet(INOUTPUT_CURRENTBIASPMOS_NET_));

            Core::InstanceTerminal & innerTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdInner);
            innerTerminalCurrentBiasPmos.connect(circuit.findNet(INNER_CURRENTBIASPMOS_NET_));
        }
        else if(currentBiasPmosInstance.getMaster().findInstances().size() == 1)
        {
            Core::InstanceTerminal & inTerminalCurrentBiasPmos = currentBiasPmosInstance.findInstanceTerminal(terminalIdIn);
            inTerminalCurrentBiasPmos.connect(circuit.findNet(IN_CURRENTBIASPMOS_NET_));
        }

        if(currentBiasNmosInstance.getMaster().findInstances().size() == 2)
        {
            Core::InstanceTerminal & inSourceTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdInSource);
            inSourceTerminalCurrentBiasNmos.connect(circuit.findNet(INSOURCE_CURRENTBIASNMOS_NET_));

            Core::InstanceTerminal & inOutputTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdInOutput);
            inOutputTerminalCurrentBiasNmos.connect(circuit.findNet(INOUTPUT_CURRENTBIASNMOS_NET_));

            Core::InstanceTerminal & innerTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdInner);
            innerTerminalCurrentBiasNmos.connect(circuit.findNet(INNER_CURRENTBIASNMOS_NET_));
        }
        else if(currentBiasNmosInstance.getMaster().findInstances().size() == 1)
        {
            Core::InstanceTerminal & inTerminalCurrentBiasNmos = currentBiasNmosInstance.findInstanceTerminal(terminalIdIn);
            inTerminalCurrentBiasNmos.connect(circuit.findNet(IN_CURRENTBIASNMOS_NET_));
        }

    }

    void AnalogInverters::eraseAllAnalogInverters()
    {
        for(auto & analogInverter : analogInverters_)
        {
            delete analogInverter;
        }
    }

}
