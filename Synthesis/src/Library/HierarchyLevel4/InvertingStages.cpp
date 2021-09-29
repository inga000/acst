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
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel4/InvertingStages.h"

#include "Synthesis/incl/Library/HierarchyLevel2/AnalogInverters.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Device/TechType.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include <sstream>



namespace Synthesis {

    const Core::TerminalName InvertingStages::OUTPUT_TERMINAL_ = Core::TerminalName("Output");

	const Core::TerminalName InvertingStages::SOURCEPMOS_TERMINAL_ = Core::TerminalName("SourcePmos");
	const Core::TerminalName InvertingStages::SOURCENMOS_TERMINAL_ = Core::TerminalName("SourceNmos");

	const Core::TerminalName InvertingStages::INTRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("InTransconductance");
	const Core::TerminalName InvertingStages::INOUTPUTTRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("InOutputTransconductance");
	const Core::TerminalName InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("InSourceTransconductance");
	const Core::TerminalName InvertingStages::INNERTRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("InnerTransconductance");

	const Core::TerminalName InvertingStages::INSTAGEBIAS_TERMINAL_ = Core::TerminalName("InStageBias");
	const Core::TerminalName InvertingStages::INOUTPUTSTAGEBIAS_TERMINAL_ = Core::TerminalName("InOutputStageBias");
	const Core::TerminalName InvertingStages::INSOURCESTAGEBIAS_TERMINAL_ = Core::TerminalName("InSourceStageBias");
	const Core::TerminalName InvertingStages::INNERSTAGEBIAS_TERMINAL_ = Core::TerminalName("InnerStageBias");

    const Core::InstanceName InvertingStages::ANALOGINVERTER_ = Core::InstanceName("AnalogInverter");

	const Core::NetId InvertingStages::OUTPUT_NET_ = Core::NetName("output").createRootIdentifier();

	const Core::NetId InvertingStages::SOURCEPMOS_NET_ = Core::NetName("sourcePmos").createRootIdentifier();
	const Core::NetId InvertingStages::SOURCENMOS_NET_ = Core::NetName("sourceNmos").createRootIdentifier();

	const Core::NetId InvertingStages::INTRANSCONDUCTANCE_NET_ = Core::NetName("inTransconductance").createRootIdentifier();
	const Core::NetId InvertingStages::INOUTPUTTRANSCONDUCTANCE_NET_ = Core::NetName("inOutputTransconductance").createRootIdentifier();
	const Core::NetId InvertingStages::INSOURCETRANSCONDUCTANCE_NET_ = Core::NetName("inSourceTransconductance").createRootIdentifier();
	const Core::NetId InvertingStages::INNERTRANSCONDUCTANCE_NET_ = Core::NetName("innerTransconductance").createRootIdentifier();

	const Core::NetId InvertingStages::INSTAGEBIAS_NET_ = Core::NetName("inStageBias").createRootIdentifier();
	const Core::NetId InvertingStages::INOUTPUTSTAGEBIAS_NET_ = Core::NetName("inOutputStageBias").createRootIdentifier();
	const Core::NetId InvertingStages::INSOURCESTAGEBIAS_NET_ = Core::NetName("inSourceStageBias").createRootIdentifier();
	const Core::NetId InvertingStages::INNERSTAGEBIAS_NET_ = Core::NetName("innerStageBias").createRootIdentifier();


	InvertingStages::InvertingStages(const StructuralLevel & structuralLevel)
    {

		initializeInvertingStages(structuralLevel);


    }
	
    InvertingStages::~InvertingStages()
    {
		eraseInvertingStagesPmosTransconductance();
        eraseInvertingStagesNmosTransconductance();
    }


	std::vector<const Core::Circuit *>  InvertingStages::getInvertingStagesPmosTransconductance() const
    {
        assert(!invertingStagesPmosTransconductance_.empty());
        return invertingStagesPmosTransconductance_;
    }

	std::vector<const Core::Circuit *>  InvertingStages::getInvertingStagesNmosTransconductance() const
    {
        assert(!invertingStagesNmosTransconductance_.empty());
        return invertingStagesNmosTransconductance_;
    }

	std::vector<const Core::Circuit *>  InvertingStages::getInvertingStages() const
    {
        assert(!invertingStagesNmosTransconductance_.empty() || !invertingStagesPmosTransconductance_.empty()); 
        std::vector<const Core::Circuit *> invertingStages;
        invertingStages.insert(invertingStages.end(), invertingStagesNmosTransconductance_.begin(), invertingStagesNmosTransconductance_.end());
        invertingStages.insert(invertingStages.end(), invertingStagesPmosTransconductance_.begin(), invertingStagesPmosTransconductance_.end());
    
        return invertingStages;
    }

	std::string InvertingStages::toStr() const
    {
        std::ostringstream oss;
        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< InvertingStages: " << std::endl;
        oss << "PMOS TRANSCONDUCTANCE" << std::endl;
        for(auto & stage : invertingStagesPmosTransconductance_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << stage->getCircuitIdentifier().toStr()<< std::endl;
            oss << stage->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FlatCircuit >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << stage->getCircuitIdentifier() << std::endl;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }

        oss << "NMOS TRANSCONDUCTANCE" << std::endl;
        for(auto & stage : invertingStagesNmosTransconductance_)
        {
            oss << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << stage->getCircuitIdentifier().toStr()<< std::endl;
            oss << stage->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FlatCircuit >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << stage->getCircuitIdentifier() << std::endl;
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }
        delete flatCircuitRecursion;
       return oss.str();
    }

	
	void InvertingStages::initializeInvertingStages(const StructuralLevel & structuralLevel)
    {
        std::vector<const Core::Circuit*> analogInverters = structuralLevel.getAnalogInverters().getAnalogInverters();

        invertingStagesPmosTransconductance_ = createInvertingStagesPmosTransconductance(analogInverters);
        invertingStagesNmosTransconductance_ = createInvertingStagesNmosTransconductance(analogInverters);

    }

    
	std::vector<const Core::Circuit*> InvertingStages::createInvertingStagesPmosTransconductance(std::vector<const Core::Circuit*> analogInverters)
    {
        int index = 1;
        std::vector<const Core::Circuit*> invertingStages;
        for(auto & analogInverter : analogInverters)
        {
            const Core::Instance & currentBiasPmos = analogInverter->findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_));
            if(!sourceTransistorIsDiodeTransistor(currentBiasPmos.getMaster()))
            {
                const Core::Circuit & invertingStage = createInvertingStagePmosTransconductance(createInstance(*analogInverter, ANALOGINVERTER_), index);
                if(invertingStage.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
                {
                    invertingStages.push_back(&invertingStage);
                    index++;
                }
                else
                {
                    delete &invertingStage;
                }
            }
        }
        return invertingStages;
    }

	std::vector<const Core::Circuit*> InvertingStages::createInvertingStagesNmosTransconductance(std::vector<const Core::Circuit*> analogInverters)
    {
        int index = 1;
        std::vector<const Core::Circuit*> invertingStages;
        for(auto & analogInverter : analogInverters)
        {
            const Core::Instance & currentBiasNmos = analogInverter->findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_));
            if(!sourceTransistorIsDiodeTransistor(currentBiasNmos.getMaster()))
            {
                const Core::Circuit & invertingStage = createInvertingStageNmosTransconductance(createInstance(*analogInverter, ANALOGINVERTER_), index);
                if(invertingStage.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
                {
                    invertingStages.push_back(&invertingStage);
                    index++;
                }
                else
                {
                    delete &invertingStage;
                }
            }
        }
        return invertingStages;
    }

    const Core::Circuit & InvertingStages::createInvertingStagePmosTransconductance(Core::Instance & analogInverter, int & index)
    {
        Core::Circuit * invertingStage = new Core::Circuit;

        Core::CircuitIds circuitIds;
        Core::CircuitId invertingStageId = circuitIds.invertingStage(index);
        invertingStageId.setTechType(Core::TechType::p());
		invertingStage->setCircuitIdentifier(invertingStageId);

        const Core::Instance & transconductance = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_));
        const Core::Instance & stageBias = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_));

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUTPUT_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT_TERMINAL_, OUTPUT_NET_));

        netNames.push_back(SOURCEPMOS_NET_);
        netNames.push_back(SOURCENMOS_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));

        addTransconductanceNets(netNames, terminalToNetMap, transconductance);
        addStageBiasNets(netNames, terminalToNetMap, stageBias);


		addNetsToCircuit(*invertingStage, netNames);
        addTerminalsToCircuit(*invertingStage, terminalToNetMap);

        invertingStage->addInstance(analogInverter);
        


        connectInstanceTerminalsPmosTransconductance(*invertingStage, analogInverter);
        analogInverter.setCircuit(*invertingStage);

        return *invertingStage;
    }

	const Core::Circuit & InvertingStages::createInvertingStageNmosTransconductance(Core::Instance & analogInverter, int & index)
    {
        Core::Circuit * invertingStage = new Core::Circuit;

        Core::CircuitIds circuitIds;
        Core::CircuitId invertingStageId = circuitIds.invertingStage(index);
        invertingStageId.setTechType(Core::TechType::n());
		invertingStage->setCircuitIdentifier(invertingStageId);

        const Core::Instance & transconductance = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_));
        const Core::Instance & stageBias = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_));

        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

        netNames.push_back(OUTPUT_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTPUT_TERMINAL_, OUTPUT_NET_));

        netNames.push_back(SOURCEPMOS_NET_);
        netNames.push_back(SOURCENMOS_NET_);
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));

        addTransconductanceNets(netNames, terminalToNetMap, transconductance);
        addStageBiasNets(netNames, terminalToNetMap, stageBias);


		addNetsToCircuit(*invertingStage, netNames);
        addTerminalsToCircuit(*invertingStage, terminalToNetMap);

        invertingStage->addInstance(analogInverter);
        


        connectInstanceTerminalsNmosTransconductance(*invertingStage, analogInverter);
        analogInverter.setCircuit(*invertingStage);

        return *invertingStage;
        
    }

	void InvertingStages::addTransconductanceNets(std::vector<Core::NetId> & netNames, 
							std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, const Core::Instance & transconductance)
    {
        if(getDeviceNamesOfFlatCircuit(transconductance.getMaster()).size() == 2)
        {
            netNames.push_back(INSOURCETRANSCONDUCTANCE_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCETRANSCONDUCTANCE_TERMINAL_, INSOURCETRANSCONDUCTANCE_NET_));
            netNames.push_back(INOUTPUTTRANSCONDUCTANCE_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUTTRANSCONDUCTANCE_TERMINAL_, INOUTPUTTRANSCONDUCTANCE_NET_));
            netNames.push_back(INNERTRANSCONDUCTANCE_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSCONDUCTANCE_TERMINAL_, INNERTRANSCONDUCTANCE_NET_));
        }
        else 
        {
            netNames.push_back(INTRANSCONDUCTANCE_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INTRANSCONDUCTANCE_TERMINAL_, INTRANSCONDUCTANCE_NET_));
        }
    }
    
    void InvertingStages::addStageBiasNets(std::vector<Core::NetId> & netNames, 
							std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, const Core::Instance & stageBias)
    {
        if(getDeviceNamesOfFlatCircuit(stageBias.getMaster()).size() == 2)
        {
            netNames.push_back(INSOURCESTAGEBIAS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCESTAGEBIAS_TERMINAL_, INSOURCESTAGEBIAS_NET_));
            netNames.push_back(INOUTPUTSTAGEBIAS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUTSTAGEBIAS_TERMINAL_, INOUTPUTSTAGEBIAS_NET_));
            netNames.push_back(INNERSTAGEBIAS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIAS_TERMINAL_, INNERSTAGEBIAS_NET_));
        }
        else 
        {
            netNames.push_back(INSTAGEBIAS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSTAGEBIAS_TERMINAL_, INSTAGEBIAS_NET_));
        }
    }

    void InvertingStages::connectInstanceTerminalsPmosTransconductance(Core::Circuit & invertingStage, Core::Instance & analogInverter)
    {
        const Core::Instance & transconductance = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_));
        const Core::Instance & stageBias = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_));
             
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::OUTPUT_TERMINAL_, OUTPUT_NET_);        
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::SOURCE_CURRENTBIASPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::SOURCE_CURRENTBIASNMOS_TERMINAL_, SOURCENMOS_NET_);

        if(getDeviceNamesOfFlatCircuit(transconductance.getMaster()).size() == 2)
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INSOURCE_CURRENTBIASPMOS_TERMINAL_, INSOURCETRANSCONDUCTANCE_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INOUTPUT_CURRENTBIASPMOS_TERMINAL_, INOUTPUTTRANSCONDUCTANCE_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INNER_CURRENTBIASPMOS_TERMINAL_, INNERTRANSCONDUCTANCE_NET_);
        }
        else 
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::IN_CURRENTBIASPMOS_TERMINAL_, INTRANSCONDUCTANCE_NET_);
        }

        if(getDeviceNamesOfFlatCircuit(stageBias.getMaster()).size() == 2)
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INSOURCE_CURRENTBIASNMOS_TERMINAL_, INSOURCESTAGEBIAS_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INOUTPUT_CURRENTBIASNMOS_TERMINAL_, INOUTPUTSTAGEBIAS_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INNER_CURRENTBIASNMOS_TERMINAL_, INNERSTAGEBIAS_NET_);
        }
        else 
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::IN_CURRENTBIASNMOS_TERMINAL_, INSTAGEBIAS_NET_);
        }
    }
    
    
    void InvertingStages::connectInstanceTerminalsNmosTransconductance(Core::Circuit & invertingStage, Core::Instance & analogInverter)
    {
        const Core::Instance & transconductance = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_));
        const Core::Instance & stageBias = analogInverter.getMaster().findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_));
             
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::OUTPUT_TERMINAL_, OUTPUT_NET_);        
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::SOURCE_CURRENTBIASPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::SOURCE_CURRENTBIASNMOS_TERMINAL_, SOURCENMOS_NET_);

        if(getDeviceNamesOfFlatCircuit(transconductance.getMaster()).size() == 2)
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INSOURCE_CURRENTBIASNMOS_TERMINAL_, INSOURCETRANSCONDUCTANCE_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INOUTPUT_CURRENTBIASNMOS_TERMINAL_, INOUTPUTTRANSCONDUCTANCE_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INNER_CURRENTBIASNMOS_TERMINAL_, INNERTRANSCONDUCTANCE_NET_);
        }
        else 
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::IN_CURRENTBIASNMOS_TERMINAL_, INTRANSCONDUCTANCE_NET_);
        }

        if(getDeviceNamesOfFlatCircuit(stageBias.getMaster()).size() == 2)
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INSOURCE_CURRENTBIASPMOS_TERMINAL_, INSOURCESTAGEBIAS_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INOUTPUT_CURRENTBIASPMOS_TERMINAL_, INOUTPUTSTAGEBIAS_NET_);
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::INNER_CURRENTBIASPMOS_TERMINAL_, INNERSTAGEBIAS_NET_);
        }
        else 
        {
            connectInstanceTerminal(invertingStage, analogInverter, AnalogInverters::IN_CURRENTBIASPMOS_TERMINAL_, INSTAGEBIAS_NET_);
        }
    }

	void InvertingStages::eraseInvertingStagesPmosTransconductance()
	{
		for(auto & invertingStage : invertingStagesPmosTransconductance_)
		{
			delete invertingStage;
		}
	}

    void InvertingStages::eraseInvertingStagesNmosTransconductance()
	{
		for(auto & invertingStage : invertingStagesNmosTransconductance_)
		{
			delete invertingStage;
		}
	}
}
