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
 *************************************************************************************************/#include "Synthesis/incl/Library/HierarchyLevel5/OpAmps.h"
#include "Synthesis/incl/Library/HierarchyLevel4/AmplificationStageLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel4/NonInvertingStages.h"
#include "Synthesis/incl/Library/HierarchyLevel4/InvertingStages.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Loads.h"
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"
#include "Synthesis/incl/Library/HierarchyLevel2/AnalogInverters.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel2/VoltageBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel1/Capacitor.h"


#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {

    const Core::TerminalName OpAmps::IN1_TERMINAL_ = Core::TerminalName("In1");
	const Core::TerminalName OpAmps::IN2_TERMINAL_ = Core::TerminalName("In2");
    const Core::TerminalName OpAmps::OUT_TERMINAL_ = Core::TerminalName("Out");
	const Core::TerminalName OpAmps::OUT1_TERMINAL_ = Core::TerminalName("Out1");
	const Core::TerminalName OpAmps::OUT2_TERMINAL_ = Core::TerminalName("Out2");

	const Core::TerminalName OpAmps::IBIAS_TERMINAL_ = Core::TerminalName("Ibias");
    const Core::TerminalName OpAmps::VREF_TERMINAL_ = Core::TerminalName("Vref");

    const Core::TerminalName OpAmps::SOURCEPMOS_TERMINAL_ = Core::TerminalName("SourcePmos");
    const Core::TerminalName OpAmps::SOURCENMOS_TERMINAL_ = Core::TerminalName("SourceNmos");

    const Core::InstanceName OpAmps::FIRSTSTAGE_ = Core::InstanceName("FirstStage");
	const Core::InstanceName OpAmps::SECONDSTAGE_ = Core::InstanceName("SecondStage");
    const Core::InstanceName OpAmps::SECONDSTAGE1_ = Core::InstanceName("SecondStage1");
    const Core::InstanceName OpAmps::SECONDSTAGE2_ = Core::InstanceName("SecondStage2");

    const Core::InstanceName OpAmps::LOADCAPACITOR_ = Core::InstanceName("LoadCapacitor");
    const Core::InstanceName OpAmps::LOADCAPACITOR1_ = Core::InstanceName("LoadCapacitor1");
    const Core::InstanceName OpAmps::LOADCAPACITOR2_ = Core::InstanceName("LoadCapacitor2");
    const Core::InstanceName OpAmps::COMPENSATIONCAPACITOR_ = Core::InstanceName("CompensationCapacitor");
    const Core::InstanceName OpAmps::COMPENSATIONCAPACITOR1_ = Core::InstanceName("CompensationCapacitor1");
    const Core::InstanceName OpAmps::COMPENSATIONCAPACITOR2_ = Core::InstanceName("CompensationCapacitor2");

    const Core::InstanceName OpAmps::FEEDBACKSTAGE_ = Core::InstanceName("FeedbackStage");
			
    const Core::InstanceName OpAmps::TRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_ = Core::InstanceName("TransconductanceComplementarySecondStage");
	const Core::InstanceName OpAmps::STAGEBIASCOMPLEMENTARYSECONDSTAGE_ = Core::InstanceName("StageBiasComplementarySecondStage");

    const std::string OpAmps::VOLTAGEBIAS_STRING_ = "VoltageBias";
    const std::string OpAmps::CURRENTBIAS_STRING_ = "CurrentBias";

	const Core::NetId OpAmps::IN1_NET_ = Core::NetName("in1").createRootIdentifier();
	const Core::NetId OpAmps::IN2_NET_ = Core::NetName("in2").createRootIdentifier();
    const Core::NetId OpAmps::OUT_NET_ = Core::NetName("out").createRootIdentifier();
	const Core::NetId OpAmps::OUT1_NET_ = Core::NetName("out1").createRootIdentifier();
	const Core::NetId OpAmps::OUT2_NET_ = Core::NetName("out2").createRootIdentifier();

    const Core::NetId OpAmps::SOURCEPMOS_NET_ = Core::NetName("sourcePmos").createRootIdentifier();
    const Core::NetId OpAmps::SOURCENMOS_NET_ = Core::NetName("sourceNmos").createRootIdentifier();

    const Core::NetId OpAmps::OUTFIRSTSTAGE_NET_ = Core::NetName("outFirstStage").createRootIdentifier();
    const Core::NetId OpAmps::OUT1FIRSTSTAGE_NET_ = Core::NetName("out1FirstStage").createRootIdentifier();
    const Core::NetId OpAmps::OUT2FIRSTSTAGE_NET_ = Core::NetName("out2FirstStage").createRootIdentifier();

    const Core::NetId OpAmps::OUTFEEDBACKSTAGE_NET_ = Core::NetName("outFeedback").createRootIdentifier();
            
    const Core::NetId OpAmps::INNERCOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("innerComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inTransconductanceComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inSourceTransconductanceComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inOutputTransconductanceComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inStageBiasComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INSOURCESTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inSourceStageBiasComplementarySecondStage").createRootIdentifier();
    const Core::NetId OpAmps::INOUTPUTSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_ = Core::NetName("inOutputStageBiasComplementarySecondStage").createRootIdentifier();

	const Core::NetId OpAmps::IBIAS_NET_  = Core::NetName("ibias").createRootIdentifier();
    const Core::NetId OpAmps::VREF_NET_  = Core::NetName("vref").createRootIdentifier();



    OpAmps::OpAmps(const AmplificationStageLevel & amplificationStageLevel, const CurrentBiases & currentBiases, 
    const VoltageBiases &  voltageBiases, const Capacitor & capacitor) :
                        amplificationStageLevel_(&amplificationStageLevel),
                        currentBiases_(&currentBiases),
                        voltageBiases_(&voltageBiases),
                        capacitor_(&capacitor)
    {

    }

	OpAmps::~OpAmps()
    {

    }

    std::vector<const Core::Circuit*> OpAmps::createSimpleOneStageOpAmps(int caseNumber, int & index)
    {
        std::vector<const Core::Circuit*> simpleOpAmps;
        for(auto & firstStage : getAmplificationStageLevel().getNonInvertingStages().createSimpleNonInvertingStages(caseNumber))
        {
        	logDebug("Create op amp num " << index);
            const Core::Circuit & opAmp = createSimpleOpAmp(index, createInstance(*firstStage, FIRSTSTAGE_));
            logDebug("finishing op amp num " << index);
            simpleOpAmps.push_back(&opAmp);
            index++;
        }
        return simpleOpAmps;
    }

	std::vector<const Core::Circuit*> OpAmps::createFullyDifferentialOneStageOpAmps(int caseNumber, int & index)
    {
        std::vector<const Core::Circuit*> fullyDifferentialOpAmps;

         for(auto & firstStage : getAmplificationStageLevel().getNonInvertingStages().createFullyDifferentialNonInvertingStages(caseNumber))
         {

			if(firstStage->getCircuitIdentifier().getTechType() == Core::TechType::p())
 			{
				 for(auto & feedbackStage : getAmplificationStageLevel().getNonInvertingStages().getFeedbackNonInvertingStagesPmosTransconductance())
				 {
					 const Core::Circuit & opAmp = createFullyDifferentialOpAmp(index, createInstance(*firstStage, FIRSTSTAGE_), createInstance(*feedbackStage, FEEDBACKSTAGE_));
					 fullyDifferentialOpAmps.push_back(&opAmp);
					 index++;
				 }
 			}
 			else
 			{
				 for(auto & feedbackStage : getAmplificationStageLevel().getNonInvertingStages().getFeedbackNonInvertingStagesNmosTransconductance())
				 {
					 const Core::Circuit & opAmp = createFullyDifferentialOpAmp(index, createInstance(*firstStage, FIRSTSTAGE_), createInstance(*feedbackStage, FEEDBACKSTAGE_));
					 fullyDifferentialOpAmps.push_back(&opAmp);
					 index++;
				 }
 			}
         }

        return fullyDifferentialOpAmps;
    }
		    
    std::vector<const Core::Circuit*> OpAmps::createComplementaryOpAmps(int caseNumber, int & index)
    {
        std::vector<const Core::Circuit*> complementaryOpAmps;


        for(auto & complementaryStage : getAmplificationStageLevel().getNonInvertingStages().createComplementaryNonInvertingStages(caseNumber))
        {
            const Core::Circuit & opAmp = createComplementaryOpAmp(index, createInstance(*complementaryStage, FIRSTSTAGE_));
            complementaryOpAmps.push_back(&opAmp);
            index++;
        }

        return complementaryOpAmps;
    }
		    
    std::vector<const Core::Circuit*> OpAmps::createSymmetricalOpAmps(int caseNumber, int & index)
    {
        std::vector<const Core::Circuit*> symmetricalOpAmps;

        for(auto & firstStage : getAmplificationStageLevel().getNonInvertingStages().createSymmetricalNonInvertingStages(caseNumber))
        {

			if(firstStage->getCircuitIdentifier().getTechType() == Core::TechType::p())
			{
	            for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStagesNmosTransconductance())
	            {
	                const Core::Circuit & load = firstStage->findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster();
	                const Core::Circuit & transconductanceSecondStage = getSecondStageTransconductance(*secondStage);

	                if(getDeviceNamesOfFlatCircuit(transconductanceSecondStage).size() >= (0.5 * getDeviceNamesOfFlatCircuit(load).size()))
	                {
	                    std::vector<const Core::Circuit*> complementarySecondStageStageBiases = findComplementarySecondStageStageBiases(*secondStage);

	                    for(auto & complementarySecondStageBias :complementarySecondStageStageBiases)
	                    {
	                        const Core::Circuit & opAmp = createSymmetricalOpAmp(index, createInstance(*firstStage, FIRSTSTAGE_),
	                                        createInstance(*secondStage, SECONDSTAGE_), createInstance(transconductanceSecondStage, TRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_),
	                                        createInstance(*complementarySecondStageBias, STAGEBIASCOMPLEMENTARYSECONDSTAGE_));
	                        symmetricalOpAmps.push_back(&opAmp);
	                        index++;
	                    }
	                }
	            }
			}
			else
			{
	            for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStagesPmosTransconductance())
	            {
	                const Core::Circuit & load = firstStage->findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster();

	                const Core::Circuit & transconductanceSecondStage = getSecondStageTransconductance(*secondStage);

	                if(getDeviceNamesOfFlatCircuit(transconductanceSecondStage).size() >= (0.5 * getDeviceNamesOfFlatCircuit(load).size()))
	                {
	                    std::vector<const Core::Circuit*> complementarySecondStageStageBiases = findComplementarySecondStageStageBiases(*secondStage);

	                    for(auto & complementarySecondStageBias :complementarySecondStageStageBiases)
	                    {
	                        const Core::Circuit & opAmp = createSymmetricalOpAmp(index, createInstance(*firstStage, FIRSTSTAGE_),
	                                        createInstance(*secondStage, SECONDSTAGE_), createInstance(transconductanceSecondStage, TRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_),
	                                        createInstance(*complementarySecondStageBias, STAGEBIASCOMPLEMENTARYSECONDSTAGE_));
	                        symmetricalOpAmps.push_back(&opAmp);
	                        index++;
	                    }
	                }
	            }
			}
        }

        return symmetricalOpAmps;
    }
	
    std::vector<const Core::Circuit*> OpAmps::createSimpleTwoStageOpAmps(std::vector<const Core::Circuit*> oneStageOpAmps)
    {
        std::vector<const Core::Circuit*> twoStageOpAmps;
        int index=1;
        for(auto & oneStageOpAmp : oneStageOpAmps)
        {
            const Core::Circuit & firstStage = oneStageOpAmp->findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
            for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
            {
                const Core::Circuit & opAmp = createSimpleOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), &createInstance(*secondStage, SECONDSTAGE_));
                twoStageOpAmps.push_back(&opAmp);
                index++;
            }
        }
        return twoStageOpAmps;
    }

    std::vector<const Core::Circuit*> OpAmps::createSimpleTwoStageOpAmps(const Core::Circuit & oneStageOpAmp, std::mutex &myMutex)
	{
        std::vector<const Core::Circuit*> twoStageOpAmps;
        std::lock_guard<std::mutex> guard(myMutex);

        int index = 1;
        const Core::Circuit & firstStage = oneStageOpAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
        for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
        {
            const Core::Circuit & opAmp = createSimpleOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), &createInstance(*secondStage, SECONDSTAGE_));
            twoStageOpAmps.push_back(&opAmp);
            index++;
        }

        return twoStageOpAmps;
    }

    std::vector<const Core::Circuit*> OpAmps::createSimpleTwoStageOpAmps(const Core::Circuit & oneStageOpAmp)
	{
        std::vector<const Core::Circuit*> twoStageOpAmps;
        int index = 1;
        const Core::Circuit & firstStage = oneStageOpAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
        for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
        {
            const Core::Circuit & opAmp = createSimpleOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), &createInstance(*secondStage, SECONDSTAGE_));
            twoStageOpAmps.push_back(&opAmp);
            index++;
        }

        return twoStageOpAmps;
    }


    std::vector<const Core::Circuit*> OpAmps::createFullyDifferentialTwoStageOpAmps(std::vector<const Core::Circuit*> oneStageOpAmps)
    {
        std::vector<const Core::Circuit*> twoStageOpAmps;

        int index =1;
        for(auto & oneStageOpAmp : oneStageOpAmps)
        {
            const Core::Circuit & firstStage = oneStageOpAmp->findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
            const Core::Circuit & feedbackStage = oneStageOpAmp->findInstance(createInstanceId(FEEDBACKSTAGE_)).getMaster();
            for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
            {
                const Core::Circuit & opAmp = createFullyDifferentialOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), createInstance(feedbackStage, FEEDBACKSTAGE_), &createInstance(*secondStage, SECONDSTAGE1_), &createInstance(*secondStage, SECONDSTAGE2_));
                twoStageOpAmps.push_back(&opAmp);
                index++;
            }
        }
        return twoStageOpAmps;
    }

    std::vector<const Core::Circuit*> OpAmps::createFullyDifferentialTwoStageOpAmps(const Core::Circuit & oneStageOpAmp, std::mutex &myMutex)
    {
        std::vector<const Core::Circuit*> twoStageOpAmps;
        std::lock_guard<std::mutex> guard(myMutex);

        int index =1;

        const Core::Circuit & firstStage = oneStageOpAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
        const Core::Circuit & feedbackStage = oneStageOpAmp.findInstance(createInstanceId(FEEDBACKSTAGE_)).getMaster();
        for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
        {
           const Core::Circuit & opAmp = createFullyDifferentialOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), createInstance(feedbackStage, FEEDBACKSTAGE_), &createInstance(*secondStage, SECONDSTAGE1_), &createInstance(*secondStage, SECONDSTAGE2_));
           twoStageOpAmps.push_back(&opAmp);
           index++;
        }
        return twoStageOpAmps;
    }

    std::vector<const Core::Circuit*> OpAmps::createFullyDifferentialTwoStageOpAmps(const Core::Circuit & oneStageOpAmp)
    {
        std::vector<const Core::Circuit*> twoStageOpAmps;
        int index =1;

        const Core::Circuit & firstStage = oneStageOpAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster();
        const Core::Circuit & feedbackStage = oneStageOpAmp.findInstance(createInstanceId(FEEDBACKSTAGE_)).getMaster();
        for(auto & secondStage : getAmplificationStageLevel().getInvertingStages().getInvertingStages())
        {
           const Core::Circuit & opAmp = createFullyDifferentialOpAmp(index, createInstance(firstStage, FIRSTSTAGE_), createInstance(feedbackStage, FEEDBACKSTAGE_), &createInstance(*secondStage, SECONDSTAGE1_), &createInstance(*secondStage, SECONDSTAGE2_));
           twoStageOpAmps.push_back(&opAmp);
           index++;
        }
        return twoStageOpAmps;
    }

    std::string OpAmps::toStr() const
    {
        std::ostringstream oss;

        return oss.str();
    }

	const AmplificationStageLevel & OpAmps::getAmplificationStageLevel() const
    {
        assert(amplificationStageLevel_ != nullptr);
        return *amplificationStageLevel_;
    }
            
    const CurrentBiases & OpAmps::getCurrentBiases() const
    {
        assert(currentBiases_ != nullptr);
        return *currentBiases_;
    }
    
    const VoltageBiases & OpAmps::getVoltageBiases() const
    {
        assert(voltageBiases_ != nullptr);
        return *voltageBiases_;
    }

    const Capacitor & OpAmps::getCapacitor() const
    {
        assert(capacitor_ != nullptr);
        return *capacitor_;
    }


	const Core::Circuit& OpAmps::createSimpleOpAmp(int & index, Core::Instance & firstStage, 
									Core::Instance * secondStage)
    {
        Core::Circuit * opAmp = new Core::Circuit;
        
        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
		        
        opAmp->addInstance(firstStage);
        firstStage.setCircuit(*opAmp);

        Core::Instance & loadCapacitor = getCapacitor().createNewCapacitorInstance(LOADCAPACITOR_);
        Core::Instance * compensationCapacitor = nullptr;
        opAmp->addInstance(loadCapacitor);
        loadCapacitor.setCircuit(*opAmp);

        Core::CircuitIds circuitIds;


        if(secondStage != nullptr)
        {
            Core::CircuitId opAmpId = circuitIds.simpleTwoStageOpAmp(index);
            opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
            opAmp->setCircuitIdentifier(opAmpId);
            opAmp->addInstance(*secondStage);
            addFirstStageToSecondStageNets(netNames, *opAmp);
            secondStage->setCircuit(*opAmp);

            compensationCapacitor = &getCapacitor().createNewCapacitorInstance(COMPENSATIONCAPACITOR_);
            opAmp->addInstance(*compensationCapacitor);
            compensationCapacitor->setCircuit(*opAmp);
        }
        else
        {
        	Core::CircuitId opAmpId = circuitIds.simpleOpAmp(index);
            opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
            opAmp->setCircuitIdentifier(opAmpId);
        }



        addTerminalNets(netNames, terminalToNetMap,*opAmp);
		
		addNetsToCircuit(*opAmp, netNames);
        addTerminalsToCircuit(*opAmp, terminalToNetMap);
        setSupplyNets(*opAmp);


        connectInstanceTerminalsSimpleOpAmp(*opAmp, firstStage, secondStage);
        connectInstanceTerminalsCapacitors(*opAmp, loadCapacitor, compensationCapacitor);
    

        buildAndConnectedBias(*opAmp);


        return *opAmp;
    }
	
    const Core::Circuit& OpAmps::createFullyDifferentialOpAmp(int & index, Core::Instance & firstStage, Core::Instance & feedbackStage,
								Core::Instance * secondStage1, Core::Instance * secondStage2)
    {
        Core::Circuit * opAmp = new Core::Circuit;
        
        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
		        
        opAmp->addInstance(firstStage);
        firstStage.setCircuit(*opAmp);

        opAmp->addInstance(feedbackStage);
        feedbackStage.setCircuit(*opAmp);

        Core::Instance & loadCapacitor1 = getCapacitor().createNewCapacitorInstance(LOADCAPACITOR1_);
        opAmp->addInstance(loadCapacitor1);
        loadCapacitor1.setCircuit(*opAmp);

        Core::Instance & loadCapacitor2 = getCapacitor().createNewCapacitorInstance(LOADCAPACITOR2_);
        opAmp->addInstance(loadCapacitor2);
        loadCapacitor2.setCircuit(*opAmp);

        Core::Instance * compensationCapacitor1 = nullptr;
        Core::Instance * compensationCapacitor2 = nullptr;


        Core::CircuitIds circuitIds;

		
        if(secondStage1 != nullptr)
        {
            Core::CircuitId opAmpId = circuitIds.fullyDifferentialTwoStageOpAmp(index);
            opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
            opAmp->setCircuitIdentifier(opAmpId);

            opAmp->addInstance(*secondStage1);
            opAmp->addInstance(*secondStage2);
            secondStage1->setCircuit(*opAmp);
            secondStage2->setCircuit(*opAmp);
            addFirstStageToSecondStageNets(netNames, *opAmp);

            compensationCapacitor1 = &getCapacitor().createNewCapacitorInstance(COMPENSATIONCAPACITOR1_);
            opAmp->addInstance(*compensationCapacitor1);
            compensationCapacitor1->setCircuit(*opAmp);

            compensationCapacitor2 = &getCapacitor().createNewCapacitorInstance(COMPENSATIONCAPACITOR2_);
            opAmp->addInstance(*compensationCapacitor2);
            compensationCapacitor2->setCircuit(*opAmp);
            
        }
        else
        {
        	Core::CircuitId opAmpId = circuitIds.fullyDifferentialOpAmp(index);
            opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
            opAmp->setCircuitIdentifier(opAmpId);
        }
        
        addTerminalNets(netNames, terminalToNetMap,*opAmp);
        netNames.push_back(OUTFEEDBACKSTAGE_NET_);
		
		addNetsToCircuit(*opAmp, netNames);
        addTerminalsToCircuit(*opAmp, terminalToNetMap);

        setSupplyNets(*opAmp);

        connectInstanceTerminalsFullyDifferentialOpAmp(*opAmp, firstStage, feedbackStage, secondStage1, secondStage2);
        connectInstanceTerminalsCapacitors(*opAmp, loadCapacitor1, compensationCapacitor1);
        
        

        buildAndConnectedBias(*opAmp);

        return *opAmp;
    }

	const Core::Circuit& OpAmps::createComplementaryOpAmp(int & index, Core::Instance & firstStage)
    {
        Core::Circuit * opAmp = new Core::Circuit;
        
        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
		        
        opAmp->addInstance(firstStage);
        firstStage.setCircuit(*opAmp);

        Core::Instance & loadCapacitor = getCapacitor().createNewCapacitorInstance(LOADCAPACITOR_);
        opAmp->addInstance(loadCapacitor);
        loadCapacitor.setCircuit(*opAmp);

        Core::CircuitIds circuitIds;
        Core::CircuitId opAmpId = circuitIds.complementaryOpAmp(index);
        opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
        opAmp->setCircuitIdentifier(opAmpId);

        addTerminalNets(netNames, terminalToNetMap,*opAmp);
		
		addNetsToCircuit(*opAmp, netNames);
        addTerminalsToCircuit(*opAmp, terminalToNetMap);

        setSupplyNets(*opAmp);

        connectInstanceTerminalsComplementaryOpAmp(*opAmp, firstStage);
        connectInstanceTerminalsCapacitors(*opAmp, loadCapacitor);

        buildAndConnectedBias(*opAmp);

        return *opAmp;
    }

    const Core::Circuit & OpAmps::createSymmetricalOpAmp(int & index, Core::Instance & firstStage, Core::Instance & secondStage,
                                 Core::Instance & transconductanceComplementarySecondStage, Core::Instance & stageBiasComplementarySecondStage)
    {
        Core::Circuit * opAmp = new Core::Circuit;
        
        std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;
		        
        opAmp->addInstance(firstStage);
        firstStage.setCircuit(*opAmp);

        opAmp->addInstance(secondStage);
        secondStage.setCircuit(*opAmp);

        opAmp->addInstance(transconductanceComplementarySecondStage);
        transconductanceComplementarySecondStage.setCircuit(*opAmp);

        opAmp->addInstance(stageBiasComplementarySecondStage);
        stageBiasComplementarySecondStage.setCircuit(*opAmp);

        Core::Instance & loadCapacitor = getCapacitor().createNewCapacitorInstance(LOADCAPACITOR_);
        opAmp->addInstance(loadCapacitor);
        loadCapacitor.setCircuit(*opAmp);


        Core::CircuitIds circuitIds;
        Core::CircuitId opAmpId = circuitIds.symmetricalOpAmp(index);
        opAmpId.setTechType(firstStage.getMaster().getCircuitIdentifier().getTechType());
        opAmp->setCircuitIdentifier(opAmpId);

        addTerminalNets(netNames, terminalToNetMap,*opAmp);
        addComplementarySecondStageNets( netNames, * opAmp);

		addNetsToCircuit(*opAmp, netNames);
        addTerminalsToCircuit(*opAmp, terminalToNetMap);

        setSupplyNets(*opAmp);
		
		addNetsToCircuit(*opAmp, netNames);
        addTerminalsToCircuit(*opAmp, terminalToNetMap);

        connectInstanceTerminalsSymmetricalOpAmp(*opAmp, firstStage, secondStage, transconductanceComplementarySecondStage, stageBiasComplementarySecondStage);
        connectInstanceTerminalsCapacitors(*opAmp, loadCapacitor);

        buildAndConnectedBias(*opAmp);

        return *opAmp;
    }
			
	void OpAmps::addTerminalNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
								Core::Circuit & opAmp) const
    {
        netNames.push_back(IN1_NET_);
        netNames.push_back(IN2_NET_);
        netNames.push_back(IBIAS_NET_);
        netNames.push_back(SOURCEPMOS_NET_);
        netNames.push_back(SOURCENMOS_NET_);
           
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN1_TERMINAL_, IN1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN2_TERMINAL_, IN2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IBIAS_TERMINAL_, IBIAS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));

        if(opAmp.hasInstance(createInstanceId(FEEDBACKSTAGE_)))
        {
            netNames.push_back(OUT1_NET_);
            netNames.push_back(OUT2_NET_);
            netNames.push_back(VREF_NET_);

            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(VREF_TERMINAL_, VREF_NET_));
        }
        else
        {
            netNames.push_back(OUT_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT_TERMINAL_, OUT_NET_));
        }
    }	

	void OpAmps::addFirstStageToSecondStageNets(std::vector<Core::NetId> & netNames, const Core::Circuit & opAmp) const
    {
        if(opAmp.hasInstance(createInstanceId(SECONDSTAGE_)) || opAmp.hasInstance(createInstanceId(SECONDSTAGE1_)))
        {
            if(opAmp.hasInstance(createInstanceId(FEEDBACKSTAGE_)))
            {
                netNames.push_back(OUT1FIRSTSTAGE_NET_);
                netNames.push_back(OUT2FIRSTSTAGE_NET_);
            }
            else
            {
                netNames.push_back(OUTFIRSTSTAGE_NET_);
            }
        }
    }	

	void OpAmps::addComplementarySecondStageNets(std::vector<Core::NetId> & netNames, Core::Circuit & opAmp) const
    {
        const Core::Circuit & load = opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster();

        const Core::Circuit & transconductanceComplementarySecondStage = opAmp.findInstance(createInstanceId(TRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_)).getMaster();
        const Core::Circuit & stageBiasComplementarySecondStage = opAmp.findInstance(createInstanceId(STAGEBIASCOMPLEMENTARYSECONDSTAGE_)).getMaster();

        netNames.push_back(INNERCOMPLEMENTARYSECONDSTAGE_NET_);
        if(getDeviceNamesOfFlatCircuit(load).size() == 2)
        {
            netNames.push_back(OUTFIRSTSTAGE_NET_);
        }
        else
        {
            netNames.push_back(OUT1FIRSTSTAGE_NET_);
            netNames.push_back(OUT2FIRSTSTAGE_NET_);
        }
        if(getDeviceNamesOfFlatCircuit(transconductanceComplementarySecondStage).size() == 1)
        {
              netNames.push_back(INTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);  
        }
        else
        {
            netNames.push_back(INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_); 

            const Core::Circuit & transistorStack = load.findInstance(createInstanceId(Loads::LOADPART1_)).getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster();
            if(transistorStack.getGateNetsNotConnectedToADrain().empty())
            {
            	netNames.push_back(INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            }
        }
        if(getDeviceNamesOfFlatCircuit(stageBiasComplementarySecondStage).size() == 1)
        {
              netNames.push_back(INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);  
        }
        else
        {
            netNames.push_back(INSOURCESTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_); 
            netNames.push_back(INOUTPUTSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
        }

    }

		
	void OpAmps::connectInstanceTerminalsSimpleOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, Core::Instance * secondStage) const
    {
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN1_TERMINAL_, IN1_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN2_TERMINAL_, IN2_NET_);

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);

        if(secondStage == nullptr)
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, OUT_NET_);
        }
        else
        {
            connectInstanceTerminal(opAmp, *secondStage, InvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage, InvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage, InvertingStages::OUTPUT_TERMINAL_, OUT_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, OUTFIRSTSTAGE_NET_);
            
            const Core::Circuit & transconductanceSecondStage = getSecondStageTransconductance(secondStage->getMaster());
            if(getDeviceNamesOfFlatCircuit(transconductanceSecondStage).size() == 1)
            {
                connectInstanceTerminal(opAmp, *secondStage, InvertingStages::INTRANSCONDUCTANCE_TERMINAL_, OUTFIRSTSTAGE_NET_);
            }
            else
            {
                connectInstanceTerminal(opAmp, *secondStage, InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_, OUTFIRSTSTAGE_NET_);
            }

        }


    }

	void OpAmps::connectInstanceTerminalsFullyDifferentialOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, 
                            Core::Instance & feedbackStage, Core::Instance * secondStage1, Core::Instance * secondStage2) const
    {
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN1_TERMINAL_, IN1_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN2_TERMINAL_, IN2_NET_);

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);

        if(secondStage1 == nullptr)
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT1_TERMINAL_, OUT1_NET_);
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, OUT2_NET_);
        }
        else
        {
            connectInstanceTerminal(opAmp, *secondStage1, InvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage1, InvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage1, InvertingStages::OUTPUT_TERMINAL_, OUT1_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT1_TERMINAL_, OUT1FIRSTSTAGE_NET_);

            connectInstanceTerminal(opAmp, *secondStage2, InvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage2, InvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);
            connectInstanceTerminal(opAmp, *secondStage2, InvertingStages::OUTPUT_TERMINAL_, OUT2_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, OUT2FIRSTSTAGE_NET_);
            
            const Core::Circuit & transconductanceSecondStage = getSecondStageTransconductance(secondStage1->getMaster());
            if(getDeviceNamesOfFlatCircuit(transconductanceSecondStage).size() == 1)
            {
                connectInstanceTerminal(opAmp, *secondStage1, InvertingStages::INTRANSCONDUCTANCE_TERMINAL_, OUT1FIRSTSTAGE_NET_);
                connectInstanceTerminal(opAmp, *secondStage2, InvertingStages::INTRANSCONDUCTANCE_TERMINAL_, OUT2FIRSTSTAGE_NET_);
            }
            else
            {
                connectInstanceTerminal(opAmp, *secondStage1, InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_, OUT1FIRSTSTAGE_NET_);
                connectInstanceTerminal(opAmp, *secondStage2, InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_, OUT2FIRSTSTAGE_NET_);
            }   
        }
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::OUT2_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::IN1_TERMINAL_, OUT2_NET_);
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::IN2_TERMINAL_, OUT1_NET_);
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::INNERTRANSCONDUCTANCE_TERMINAL_, VREF_NET_);
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, feedbackStage, NonInvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);
        connectedLoadInstanceTerminalToFeedbackStage(opAmp, firstStage);

    }

    void OpAmps::connectInstanceTerminalsComplementaryOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage) const
    {
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN1_TERMINAL_, IN1_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN2_TERMINAL_, IN2_NET_);

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, OUT_NET_);

     
    }

    void OpAmps::connectInstanceTerminalsSymmetricalOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, 
                                            Core::Instance &  secondStage, Core::Instance & transconductanceComplementarySecondStage,
                                            Core::Instance stageBiasComplementarySecondStage) const
    {

        const Core::Circuit & load = firstStage.getMaster().findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster();

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN1_TERMINAL_, IN1_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::IN2_TERMINAL_, IN2_NET_);

        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);

        connectInstanceTerminal(opAmp, secondStage, InvertingStages::SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_);
        connectInstanceTerminal(opAmp, secondStage, InvertingStages::SOURCENMOS_TERMINAL_, SOURCENMOS_NET_);

        if(transconductanceComplementarySecondStage.getMaster().getCircuitIdentifier().getTechType().isN())
        {
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
            connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
        }
        else
        {
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
            connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
        }

        connectInstanceTerminal(opAmp, secondStage, InvertingStages::OUTPUT_TERMINAL_, OUT_NET_);

        connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::OUT_TERMINAL_, INNERCOMPLEMENTARYSECONDSTAGE_NET_);
        connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::IN_TERMINAL_, INNERCOMPLEMENTARYSECONDSTAGE_NET_);
        
        
        if(getDeviceNamesOfFlatCircuit(load).size() == 2  
                   && getDeviceNamesOfFlatCircuit(transconductanceComplementarySecondStage.getMaster()).size() == 1)
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT1_TERMINAL_, OUTFIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INTRANSCONDUCTANCE_TERMINAL_, OUTFIRSTSTAGE_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, INTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::IN_TERMINAL_, INTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
        }
        else if(getDeviceNamesOfFlatCircuit(load).size() == 2  
                   && getDeviceNamesOfFlatCircuit(transconductanceComplementarySecondStage.getMaster()).size() == 2)
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT1_TERMINAL_, OUTFIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_, OUTFIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INOUTPUTTRANSCONDUCTANCE_TERMINAL_, INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUT2_TERMINAL_, INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::INSOURCE_TERMINAL_, INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::INOUTPUT_TERMINAL_, INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
        }
        else
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUTSOURCE1LOAD1_TERMINAL_, OUT1FIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUTOUTPUT1LOAD1_TERMINAL_, OUT2FIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INSOURCETRANSCONDUCTANCE_TERMINAL_, OUT1FIRSTSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INOUTPUTTRANSCONDUCTANCE_TERMINAL_, OUT2FIRSTSTAGE_NET_);

            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUTSOURCE2LOAD1_TERMINAL_, INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::INSOURCE_TERMINAL_, INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);

            const Core::Circuit & transistorStackLoad = load.findInstance(createInstanceId(Loads::LOADPART1_)).getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster();
            if(transistorStackLoad.getGateNetsNotConnectedToADrain().size() == 1)
            {
            	 connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUTOUTPUT2LOAD1_TERMINAL_, OUT2FIRSTSTAGE_NET_);
            	 connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::INOUTPUT_TERMINAL_, OUT2FIRSTSTAGE_NET_);

            }
            else
            {
            	 connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::OUTOUTPUT2LOAD1_TERMINAL_, INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            	 connectInstanceTerminal(opAmp, transconductanceComplementarySecondStage, CurrentBiases::INOUTPUT_TERMINAL_, INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_);
            }
         }
      
        if(getDeviceNamesOfFlatCircuit(stageBiasComplementarySecondStage.getMaster()).size() == 1)
        {
            connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::OUT_TERMINAL_, INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
              
            const Core::Circuit & stageBiasSecondStage = getSecondStageStageBias(secondStage.getMaster());
            if(getDeviceNamesOfFlatCircuit(stageBiasSecondStage).size() == 1)
            {
                connectInstanceTerminal(opAmp, secondStage, InvertingStages::INSTAGEBIAS_TERMINAL_,  INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
            }
            else
            {
                connectInstanceTerminal(opAmp, secondStage, InvertingStages::INSOURCESTAGEBIAS_TERMINAL_,  INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
                if(!isSingleDiodeTransistor(stageBiasComplementarySecondStage.getMaster()))
                {
                   connectInstanceTerminal(opAmp, secondStage, InvertingStages::INOUTPUTSTAGEBIAS_TERMINAL_,  INNERCOMPLEMENTARYSECONDSTAGE_NET_); 
                }
            }
        }
        else
        {
            connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::OUTSOURCE_TERMINAL_, INSOURCESTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, stageBiasComplementarySecondStage, VoltageBiases::OUTINPUT_TERMINAL_, INOUTPUTSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INSOURCESTAGEBIAS_TERMINAL_,  INSOURCESTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);
            connectInstanceTerminal(opAmp, secondStage, InvertingStages::INOUTPUTSTAGEBIAS_TERMINAL_,  INOUTPUTSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_);    
        }
    }


    void OpAmps::connectedLoadInstanceTerminalToFeedbackStage(Core::Circuit & opAmp, Core::Instance & firstStage) const
    {
        const Core::Circuit & load = firstStage.getMaster().findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster();
        const Core::Circuit & loadPart1 = load.findInstance(createInstanceId(Loads::LOADPART1_)).getMaster();
        
        if(load.findInstances().size() == 1)
        {
            
            if(getDeviceNamesOfFlatCircuit(loadPart1).size() == 2)
            {
                connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::INNERLOAD1_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
            }
            else
            {
                connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::INNERSOURCELOAD1_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
            }

        }
        else if(getDeviceNamesOfFlatCircuit(loadPart1).size() == 2)
        {
            const Core::Circuit & loadPart2 = load.findInstance(createInstanceId(Loads::LOADPART2_)).getMaster();
            if(getDeviceNamesOfFlatCircuit(loadPart2).size() == 2)
            {
                connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::INNERLOAD2_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
            }
            else
            {
                connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::INNERSOURCELOAD2_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
            }

        }
        else
        {
            connectInstanceTerminal(opAmp, firstStage, NonInvertingStages::INNERBIASGCC_TERMINAL_, OUTFEEDBACKSTAGE_NET_);
        }
    }

    void OpAmps::connectInstanceTerminalsCapacitors(Core::Circuit & opAmp, Core::Instance & loadCapacitor, 
											Core::Instance * compensationCapacitor) const
    {
        if(opAmp.hasInstance(createInstanceId(FEEDBACKSTAGE_)))
        {
            Core::Instance & loadCapacitor2 = opAmp.findInstance(createInstanceId(LOADCAPACITOR2_));
            connectInstanceTerminal(opAmp, loadCapacitor, Capacitor::PLUS_TERMINAL_, OUT1_NET_);
            connectInstanceTerminal(opAmp, loadCapacitor2, Capacitor::PLUS_TERMINAL_, OUT2_NET_);
            connectInstanceTerminal(opAmp, loadCapacitor2, Capacitor::MINUS_TERMINAL_, SOURCENMOS_NET_);

        }
        else
        {
            connectInstanceTerminal(opAmp, loadCapacitor, Capacitor::PLUS_TERMINAL_, OUT_NET_);
        }
        connectInstanceTerminal(opAmp, loadCapacitor, Capacitor::MINUS_TERMINAL_, SOURCENMOS_NET_);

        if(compensationCapacitor != nullptr)
        {
            if(opAmp.hasInstance(createInstanceId(FEEDBACKSTAGE_)))
            {
                Core::Instance & compensationCapacitor2 = opAmp.findInstance(createInstanceId(COMPENSATIONCAPACITOR2_));
                connectInstanceTerminal(opAmp, *compensationCapacitor, Capacitor::PLUS_TERMINAL_, OUT1FIRSTSTAGE_NET_);
                connectInstanceTerminal(opAmp, *compensationCapacitor, Capacitor::MINUS_TERMINAL_, OUT1_NET_);
                connectInstanceTerminal(opAmp, compensationCapacitor2, Capacitor::PLUS_TERMINAL_, OUT2FIRSTSTAGE_NET_);
                connectInstanceTerminal(opAmp, compensationCapacitor2, Capacitor::MINUS_TERMINAL_, OUT2_NET_);
            }
            else
            {   
                connectInstanceTerminal(opAmp, *compensationCapacitor, Capacitor::PLUS_TERMINAL_, OUTFIRSTSTAGE_NET_);
                connectInstanceTerminal(opAmp, *compensationCapacitor, Capacitor::MINUS_TERMINAL_, OUT_NET_);
            }
        }
        
    }

		

    void OpAmps::buildAndConnectedBias(Core::Circuit & opAmp)
    {
    	gateInstanceTerminalsConnectedToADrain_.clear();
    	assert(gateInstanceTerminalsConnectedToADrain_.empty(), "For every new bias build the vector gor gete instance terminals connected to a drain must be empty!!!!");

        std::vector<Core::InstanceTerminal*> gateInstanceTerminalsPmosSource = getGateInstanceTerminalsNotConnectedToADrainSourcePmos(opAmp);
		std::vector<Core::InstanceTerminal*> gateInstanceTerminalsPmosOutput = getGateInstanceTerminalsNotConnectedToADrainOutputPmos(opAmp);
		std::vector<Core::InstanceTerminal*> gateInstanceTerminalsNmosSource = getGateInstanceTerminalsNotConnectedToADrainSourceNmos(opAmp);
		std::vector<Core::InstanceTerminal*> gateInstanceTerminalsNmosOutput = getGateInstanceTerminalsNotConnectedToADrainOutputNmos(opAmp);

		checkForNonInvertingStageInput(gateInstanceTerminalsPmosSource);
		checkForNonInvertingStageInput(gateInstanceTerminalsPmosOutput);
		checkForNonInvertingStageInput(gateInstanceTerminalsNmosSource);
		checkForNonInvertingStageInput(gateInstanceTerminalsNmosOutput);


		int indexNmosVoltageBiases = 1;
		int indexPmosVoltageBiases = 1;

		connectCurrentBiasOfImprovedWilsonCurrentMirror(opAmp, gateInstanceTerminalsPmosOutput, indexPmosVoltageBiases);
		connectCurrentBiasOfImprovedWilsonCurrentMirror(opAmp, gateInstanceTerminalsNmosOutput, indexNmosVoltageBiases);



		if(opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster().findInstances().size() == 2)
		{
			const Core::Instance & loadPart1 = opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().findInstance(createInstanceId(NonInvertingStages::LOAD_)).getMaster().findInstance(createInstanceId(Loads::LOADPART1_));
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() == 2 && opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).hasInstanceTerminal(createTerminalId(NonInvertingStages::INNERGCC_TERMINAL_)))
			{
				Core::InstanceTerminal & innerGCCTerminal = opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).findInstanceTerminal(createTerminalId(NonInvertingStages::INNERGCC_TERMINAL_));


				if(opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().getCircuitIdentifier().getTechType().isN())
				{
					connectCascodeGCC(opAmp, innerGCCTerminal, indexNmosVoltageBiases);
				}
				else
				{
					connectCascodeGCC(opAmp, innerGCCTerminal, indexPmosVoltageBiases);
				}


			}
		}

        connectRemainingGateTerminals(opAmp, getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsPmosSource), getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsPmosOutput), Core::TechType::p(), indexPmosVoltageBiases);
        connectRemainingGateTerminals(opAmp, getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsNmosSource), getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsNmosOutput), Core::TechType::n(), indexNmosVoltageBiases);



        assert(getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsPmosSource).empty()
        		&& getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsNmosSource).empty()
				&& getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsPmosOutput).empty()
				&& getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsNmosOutput).empty(),
				"All vector of gate instance terminals should be empty");

        if(indexPmosVoltageBiases == indexNmosVoltageBiases)
        {
        	Core::TechType techType = opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().getCircuitIdentifier().getTechType();
        	connectIbiasTerminal(opAmp, techType);
        }
        else if(indexPmosVoltageBiases< indexNmosVoltageBiases)//Pmos gets Ibias
        {
        	connectIbiasTerminal(opAmp, Core::TechType::p());
        }
        else
        {
        	connectIbiasTerminal(opAmp, Core::TechType::n());

        }
        addCurrentBiasesToCircuit(opAmp);
    }

	

    const Core::Circuit & OpAmps::getSecondStageTransconductance(const Core::Circuit & secondStage) const
    {
        const Core::Circuit & analogInverter = secondStage.findInstance(createInstanceId(InvertingStages::ANALOGINVERTER_)).getMaster();
        const Core::Circuit * transconductanceCircuit = nullptr;

        if(secondStage.getCircuitIdentifier().getTechType().isN())
        {
            transconductanceCircuit = &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_)).getMaster();
        }
        else
        {
            transconductanceCircuit = &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_)).getMaster();
        }
        return *transconductanceCircuit;
    }

    const Core::Circuit & OpAmps::getSecondStageStageBias(const Core::Circuit & secondStage) const
    {
        const Core::Circuit & analogInverter = secondStage.findInstance(createInstanceId(InvertingStages::ANALOGINVERTER_)).getMaster();
        const Core::Circuit * stageBiasCircuit = nullptr;

        if(secondStage.getCircuitIdentifier().getTechType().isN())
        {
            stageBiasCircuit = &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_)).getMaster();
        }
        else
        {
            stageBiasCircuit = &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_)).getMaster();
        }
        return *stageBiasCircuit;
    }

    std::vector<const Core::Circuit *>  OpAmps::findComplementarySecondStageStageBiases(const Core::Circuit & secondStage) const
    {
        std::vector<const Core::Circuit*> stageVoltageBiases;
        const Core::Circuit & analogInverter = secondStage.findInstance(createInstanceId(InvertingStages::ANALOGINVERTER_)).getMaster();

        const Core::Circuit * stageCurrentBias = nullptr;
        std::vector<const Core::Circuit*> oneTransistorVoltageBiases;
        std::vector<const Core::Circuit*> voltageBiases;
        
        if(secondStage.getCircuitIdentifier().getTechType().isN())
        {
             stageCurrentBias =  &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASPMOS_)).getMaster();
             oneTransistorVoltageBiases = getVoltageBiases().getOneTransistorVoltageBiasesPmos();
             voltageBiases = getVoltageBiases().getAllVoltageBiasesPmos();

        }
        else
        {
            stageCurrentBias = &analogInverter.findInstance(createInstanceId(AnalogInverters::CURRENTBIASNMOS_)).getMaster();
            oneTransistorVoltageBiases = getVoltageBiases().getOneTransistorVoltageBiasesNmos();
            voltageBiases = getVoltageBiases().getAllVoltageBiasesNmos();
        }


        if(getDeviceNamesOfFlatCircuit(*stageCurrentBias).size() == 1)
        {
            for(auto & voltageBias : oneTransistorVoltageBiases)
            {
               if(isSingleDiodeTransistor(*voltageBias))
                {
                        stageVoltageBiases.push_back(voltageBias);
                }
            }
            return stageVoltageBiases;
        }
        else if(sourceTransistorIsDiodeTransistor(*stageCurrentBias))
        {
            for(auto & voltageBias : voltageBiases)
            {
               if(outputTransistorIsDiodeTransistor(*voltageBias) && voltageBias->getGateNetsNotConnectedToADrain().size() == 1)
                {
                    stageVoltageBiases.push_back(voltageBias);
                }
                else if(getDeviceNamesOfFlatCircuit(*voltageBias).size() == 1 && !isSingleDiodeTransistor(*voltageBias))
                {
                    stageVoltageBiases.push_back(voltageBias);
                }
            }
            return stageVoltageBiases; 
        }
        else
        {
            for(auto & voltageBias : voltageBiases)
            {
               if(!(outputTransistorIsDiodeTransistor(*voltageBias) && voltageBias->getGateNetsNotConnectedToADrain().size() == 1)
            		   && getDeviceNamesOfFlatCircuit(*voltageBias).size() == 2)
                {
                    stageVoltageBiases.push_back(voltageBias);
                }
                else if(isSingleDiodeTransistor(*voltageBias))
                {
                    stageVoltageBiases.push_back(voltageBias);
                }
            }
            return stageVoltageBiases; 
        }

    }

    void OpAmps::setSupplyNets(Core::Circuit & opAmp) const
    {
        Core::Net & sourcePmos = opAmp.findNet(SOURCEPMOS_NET_);
        sourcePmos.setSupply(Core::Supply::vdd());

        Core::Net & sourceNmos = opAmp.findNet(SOURCENMOS_NET_);
        sourceNmos.setSupply(Core::Supply::gnd());
    }

    std::vector<Core::InstanceTerminal*> OpAmps::getGateInstanceTerminalsNotConnectedToADrainSourcePmos(Core::Circuit & opAmp) const
    {
        std::vector<Core::InstanceTerminal*> notGateConnectedInstanceTerminals;

        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        std::vector<Core::NetId> unconnectedTopLevelGateNets = opAmp.getGateNetsNotConnectedToADrain();
        std::vector<Core::InstanceTerminal* > instanceTerminalsOfUnconnectedTopLevelGateNets;
        for(auto & netId : unconnectedTopLevelGateNets)
        {
        	if(opAmp.hasNet(netId))
        	{
        		for(auto & terminal : opAmp.findNet(netId).getInstanceTerminals())
        		{
        			instanceTerminalsOfUnconnectedTopLevelGateNets.push_back(terminal);
        		}
        	}
        }

        for(auto & instance : opAmp.findInstances())
        {
            std::vector<Core::NetId> unConnectedGateNets = instance->getMaster().getGateNetsNotConnectedToADrain();
            Core::Circuit * flatInstanceCircuit = flatCircuitRecursion->createNewFlatCopy(instance->getMaster());

            for(auto & netId : unConnectedGateNets)
            {
                assert( flatInstanceCircuit->hasNet(netId), "Net must be in instance! Something is gone wrong!!");
                const Core::Net & flatNet = flatInstanceCircuit->findNet(netId);

                if(flatNet.isOnlyConnectedToPmosTransistors() && flatNet.connectedDevicesConnectedToASourceNet())
                {
                    for(auto & terminal : instance->getMaster().findTerminals())
                    {
                        if(terminal->getNet().getIdentifier() == netId)
                        {
                            Core::InstanceTerminal & instanceTerminal = instance->getInstanceTerminal(terminal->getIdentifier());
                            if(instanceTerminal.isConnected())
                            {
                                if(terminalIsInVector(instanceTerminal, instanceTerminalsOfUnconnectedTopLevelGateNets))
                                {
                                    notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                                }
                            }
                            else
                            {
                                notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                            }
                        }
                    }
                }
            }
            delete flatInstanceCircuit;
        }
        delete flatCircuitRecursion;
        return notGateConnectedInstanceTerminals;
    }

	std::vector<Core::InstanceTerminal*> OpAmps::getGateInstanceTerminalsNotConnectedToADrainOutputPmos(Core::Circuit & opAmp) const
    {
        std::vector<Core::InstanceTerminal*> notGateConnectedInstanceTerminals;

        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        std::vector<Core::NetId> unconnectedTopLevelGateNets = opAmp.getGateNetsNotConnectedToADrain();
        std::vector<Core::InstanceTerminal* > instanceTerminalsOfUnconnectedTopLevelGateNets;
        for(auto & netId : unconnectedTopLevelGateNets)
        {
        	if(opAmp.hasNet(netId))
        	{
        		for(auto & terminal : opAmp.findNet(netId).getInstanceTerminals())
        		{
        			instanceTerminalsOfUnconnectedTopLevelGateNets.push_back(terminal);
        		}
        	}
        }

        for(auto & instance : opAmp.findInstances())
        {
            std::vector<Core::NetId> unConnectedGateNets = instance->getMaster().getGateNetsNotConnectedToADrain();
            Core::Circuit * flatInstanceCircuit = flatCircuitRecursion->createNewFlatCopy(instance->getMaster());
            for(auto & netId : unConnectedGateNets)
            {
                assert(instance->getMaster().hasNet(netId), "Net must be in instance! Something is gone wrong!!");
                const Core::Net & flatNet = flatInstanceCircuit->findNet(netId);
                
                if(flatNet.isOnlyConnectedToPmosTransistors() && !flatNet.connectedDevicesConnectedToASourceNet())
                {
                    for(auto & terminal : instance->getMaster().findTerminals())
                    {
                        if(terminal->getNet().getIdentifier() == netId)
                        {
                            Core::InstanceTerminal & instanceTerminal = instance->getInstanceTerminal(terminal->getIdentifier());
                            if(instanceTerminal.isConnected())
                            {
                                if(terminalIsInVector(instanceTerminal, instanceTerminalsOfUnconnectedTopLevelGateNets))
                                {

                                    notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                                }
                            }
                            else
                            {
                                notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                            }   
                        }
                    }
                }
            }
            delete flatInstanceCircuit;
        }
        delete flatCircuitRecursion;
        return notGateConnectedInstanceTerminals;
    }

	std::vector<Core::InstanceTerminal*> OpAmps::getGateInstanceTerminalsNotConnectedToADrainSourceNmos(Core::Circuit & opAmp) const
    {
        std::vector<Core::InstanceTerminal*> notGateConnectedInstanceTerminals;

        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        std::vector<Core::NetId> unconnectedTopLevelGateNets = opAmp.getGateNetsNotConnectedToADrain();
        std::vector<Core::InstanceTerminal* > instanceTerminalsOfUnconnectedTopLevelGateNets;
        for(auto & netId : unconnectedTopLevelGateNets)
        {
        	if(opAmp.hasNet(netId))
        	{
        		for(auto & terminal : opAmp.findNet(netId).getInstanceTerminals())
        		{
        			instanceTerminalsOfUnconnectedTopLevelGateNets.push_back(terminal);
        		}
        	}
        }

        for(auto & instance : opAmp.findInstances())
        {
            std::vector<Core::NetId> unConnectedGateNets = instance->getMaster().getGateNetsNotConnectedToADrain();
            Core::Circuit * flatInstanceCircuit = flatCircuitRecursion->createNewFlatCopy(instance->getMaster());
            for(auto & netId : unConnectedGateNets)
            {
                assert(instance->getMaster().hasNet(netId), "Net must be in instance! Something is gone wrong!!");
                const Core::Net & flatNet = flatInstanceCircuit->findNet(netId);

                if(flatNet.isOnlyConnectedToNmosTransistors() && flatNet.connectedDevicesConnectedToASourceNet())
                {
                    for(auto & terminal : instance->getMaster().findTerminals())
                    {
                        if(terminal->getNet().getIdentifier() == netId)
                        {
                            Core::InstanceTerminal & instanceTerminal = instance->getInstanceTerminal(terminal->getIdentifier());
                            if(instanceTerminal.isConnected())
                            {
                                if(terminalIsInVector(instanceTerminal, instanceTerminalsOfUnconnectedTopLevelGateNets))
                                {
                                    notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                                }
                            }
                            else
                            {
                                notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                            }
                        }
                    }
                }
            }
            delete flatInstanceCircuit;
        }
        delete flatCircuitRecursion;
        return notGateConnectedInstanceTerminals;
    }

	std::vector<Core::InstanceTerminal*> OpAmps::getGateInstanceTerminalsNotConnectedToADrainOutputNmos(Core::Circuit & opAmp) const
    {
        std::vector<Core::InstanceTerminal*> notGateConnectedInstanceTerminals;

        Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

        std::vector<Core::NetId> unconnectedTopLevelGateNets = opAmp.getGateNetsNotConnectedToADrain();
        std::vector<Core::InstanceTerminal* > instanceTerminalsOfUnconnectedTopLevelGateNets;
        for(auto & netId : unconnectedTopLevelGateNets)
        {
        	if(opAmp.hasNet(netId))
        	{
        		for(auto & terminal : opAmp.findNet(netId).getInstanceTerminals())
        		{
        			instanceTerminalsOfUnconnectedTopLevelGateNets.push_back(terminal);
        		}
        	}
        }

        for(auto & instance : opAmp.findInstances())
        {
            std::vector<Core::NetId> unConnectedGateNets = instance->getMaster().getGateNetsNotConnectedToADrain();
            Core::Circuit * flatInstanceCircuit = flatCircuitRecursion->createNewFlatCopy(instance->getMaster());
            for(auto & netId : unConnectedGateNets)
            {
                assert(instance->getMaster().hasNet(netId), "Net must be in instance! Something is gone wrong!!");
                const Core::Net & flatNet = flatInstanceCircuit->findNet(netId);

                if(flatNet.isOnlyConnectedToNmosTransistors() && !flatNet.connectedDevicesConnectedToASourceNet())
                {
                    for(auto & terminal : instance->getMaster().findTerminals())
                    {
                        if(terminal->getNet().getIdentifier() == netId)
                        {
                            Core::InstanceTerminal & instanceTerminal = instance->getInstanceTerminal(terminal->getIdentifier());
                            if(instanceTerminal.isConnected())
                            {
                                if(terminalIsInVector(instanceTerminal, instanceTerminalsOfUnconnectedTopLevelGateNets))
                                {
                                    notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                                }
                            }
                            else
                            {
                                notGateConnectedInstanceTerminals.push_back(&instanceTerminal);
                            }
                        }
                    }
                }
            }
            delete flatInstanceCircuit;
        }
        delete flatCircuitRecursion;
        return notGateConnectedInstanceTerminals;
    }

	std::vector<Core::InstanceTerminal*> OpAmps::getGateInstanceTerminalsNotConnectedToADrain(std::vector<Core::InstanceTerminal*> instanceTerminals) const
	{
		std::vector<Core::InstanceTerminal *> unconnectedInstanceTerminals;
		for(auto & instanceTerminal : instanceTerminals)
		{
			if(!isGateInstanceTerminalConnectedToADrain(*instanceTerminal))
			{
				unconnectedInstanceTerminals.push_back(instanceTerminal);
			}
		}
		return unconnectedInstanceTerminals;
	}

	std::vector<Core::Instance* > OpAmps::getVoltageBiasesOfBias(Core::Circuit & opAmp, Core::TechType techType)
	{
		Core::CircuitIds circuitIds;
		std::vector<Core::Instance *> voltageBiases;

		for(auto & instance : opAmp.findInstances())
		{
			if(instance->getMaster().getCircuitIdentifier() == circuitIds.voltageBias()
					&& instance->getMaster().getCircuitIdentifier().getTechType() == techType
					&& instance->getIdentifier().getInstanceName() != STAGEBIASCOMPLEMENTARYSECONDSTAGE_)
			{
				voltageBiases.push_back(instance);
			}
		}
		return voltageBiases;
	}

	Core::Instance & OpAmps::findReferenceVoltageBias(Core::Circuit & opAmp, std::vector<Core::Instance* > voltageBiases, Core::TechType techType)
	{
		Core::Instance * referenceVoltageBias = nullptr;
		for(auto voltageBias : voltageBiases)
		{
			if(isVoltageBiasConnectedToIbias(*voltageBias))
			{
				referenceVoltageBias = voltageBias;
			}
		}
		if(referenceVoltageBias == nullptr)
		{
			for(auto & voltageBias : voltageBiases)
			{
				if(isSingleDiodeTransistor(voltageBias->getMaster())
						&& isConnectedToTerminalOfSourceTransistors(voltageBias->getInstanceTerminal(createTerminalId(VoltageBiases::OUT_TERMINAL_)).getNet()))
				{
					referenceVoltageBias = voltageBias;
				}
			}
		}
		if(referenceVoltageBias == nullptr)
		{
			for(auto & voltageBias : voltageBiases)
			{
				if(sourceTransistorIsDiodeTransistor(voltageBias->getMaster()) && outputTransistorIsDiodeTransistor(voltageBias->getMaster())
						&& getDeviceNamesOfFlatCircuit(voltageBias->getMaster()).size() == 2)
				{
					referenceVoltageBias = voltageBias;
				}
			}
		}
		if(referenceVoltageBias == nullptr)
		{
			if(techType.isN())
			{
				const Core::Circuit & newVoltageBias = getVoltageBiases().getDiodeTransistorVoltageBiasNmos();
				Core::InstanceName voltageBiasName =createVoltageBiasInstanceName(0, techType);
				referenceVoltageBias = &createInstance(newVoltageBias, voltageBiasName);
			}
			else
			{
				const Core::Circuit & newVoltageBias = getVoltageBiases().getDiodeTransistorVoltageBiasPmos();
				Core::InstanceName voltageBiasName =createVoltageBiasInstanceName(0, techType);
				referenceVoltageBias = &createInstance(newVoltageBias, voltageBiasName);
			}
			opAmp.addInstance(*referenceVoltageBias);
			referenceVoltageBias->setCircuit(opAmp);

			std::vector<Core::NetId> netNames = createNetNamesOfBiasInstance(*referenceVoltageBias);
			addNetsToCircuit(opAmp, netNames);

			Core::InstanceName instanceName = referenceVoltageBias->getIdentifier().getInstanceName();
			Core::NetId outId = createNetIdByInstanceName("out", instanceName);
			connectInstanceTerminal(opAmp, *referenceVoltageBias, VoltageBiases::OUT_TERMINAL_, outId);

			if(techType.isN())
			{
				connectInstanceTerminal(opAmp, *referenceVoltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
			}
			else
			{
				connectInstanceTerminal(opAmp, *referenceVoltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
			}
		}

		return *referenceVoltageBias;
	}

	Core::Net & OpAmps::getReferenceVoltageBiasOutputNet(Core::Instance & referenceVoltageBias)
	{
		if(getDeviceNamesOfFlatCircuit(referenceVoltageBias.getMaster()).size() == 1)
		{
			return referenceVoltageBias.findInstanceTerminal(createTerminalId(VoltageBiases::OUT_TERMINAL_)).getNet();
		}
		else
		{
			return referenceVoltageBias.findInstanceTerminal(createTerminalId(VoltageBiases::OUTSOURCE_TERMINAL_)).getNet();
		}
	}

    bool OpAmps::netIsInVector(Core::NetId netId, std::vector<Core::NetId> unconnectedTopLevelGateNets) const
    {
    	bool hasIt = false;
        for(auto & netIdInVector : unconnectedTopLevelGateNets)
        {
        	if(netIdInVector == netId)
        	{
        		hasIt = true;
        		break;
        	}
        }
        return hasIt;
    }

    bool OpAmps::terminalIsInVector(Core::InstanceTerminal & instanceTerminal,
    		std::vector<Core::InstanceTerminal*>  instanceTerminals) const
    {
    	bool hasIt = false;
        for(auto & instanceTerminalInVector : instanceTerminals)
        {
        	if(*instanceTerminalInVector == instanceTerminal)
        	{
        		hasIt = true;
        		break;
        	}
        }
        return hasIt;
    }

    void OpAmps::checkForNonInvertingStageInput(std::vector<Core::InstanceTerminal*> & gateInstanceTerminals)
    {
    	for(auto & instanceTerminal : gateInstanceTerminals)
    	{
    		if(instanceTerminal->isConnected())
    		{
    			Core::NetId instanceNetId = instanceTerminal->getNet().getIdentifier();
    			if(instanceNetId == IN1_NET_ || instanceNetId == IN2_NET_ || instanceNetId == VREF_NET_)
    			{
    				addGateInstanceTerminalConnectedToADrain(*instanceTerminal);
    			}
    		}
    	}
    }

    void OpAmps::connectCurrentBiasOfImprovedWilsonCurrentMirror(Core::Circuit & opAmp, std::vector<Core::InstanceTerminal*> gateInstanceTerminalsOutput, int & index)
    {
    	Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
    	for(auto & instanceTerminal : gateInstanceTerminalsOutput)
    	{
    		if(!isGateInstanceTerminalConnectedToADrain(*instanceTerminal))
    		{
    			Core::Instance & instance = instanceTerminal->getInstance();
    			const Core::Net& terminalNet = instanceTerminal->getTerminal().getNet();
    			Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(instance.getMaster());
    			if(isGateNetOfCurrentBiasesOfImprovedWilsonCurrentMirror(terminalNet.getIdentifier(), *flatCircuit))
    			{
    				std::vector<const Core::Circuit *> voltageBiases;
    				Core::TechType techType;
    				if(isGateNetOfNmosTransistors(flatCircuit->findNet(terminalNet.getIdentifier())))
    				{
    					voltageBiases = getVoltageBiases().getTwoTransistorVoltageBiasesNmos();
    					techType = Core::TechType::n();
    				}
    				else
    				{
    					voltageBiases = getVoltageBiases().getTwoTransistorVoltageBiasesPmos();
    					techType = Core::TechType::p();
    				}
    				const Core::Circuit & voltageBiasImprovedWilsonCurrentMirror = findVoltageBiasOfImprovedWilsonCurrentMirror(voltageBiases);
    				Core::InstanceTerminal & sourceTerminalVoltageBias = getSourceTermimalImprovedWilsonCurrentMirror(instance, techType);

    				Core::InstanceName voltageBiasName =createVoltageBiasInstanceName(index, techType);
    				addTwoTransistorVoltageBiasToCircuit(opAmp, createInstance(voltageBiasImprovedWilsonCurrentMirror, voltageBiasName), *instanceTerminal, sourceTerminalVoltageBias);
    				addGateInstanceTerminalConnectedToADrain(*instanceTerminal);
    				index++;
    			}

    		}
    	}

    }

    void OpAmps::connectCascodeGCC(Core::Circuit & opAmp, Core::InstanceTerminal & innerGCCTerminal,int &  index)
    {
    	const Core::Circuit * voltageBias = nullptr;
        Core::TechType techType;
        if(opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).getMaster().getCircuitIdentifier().getTechType().isN())
        {
        	voltageBias = &getVoltageBiases().getDiodeTransistorVoltageBiasNmos();
        	techType = Core::TechType::n();
        }
        else
        {
        	voltageBias = &getVoltageBiases().getDiodeTransistorVoltageBiasPmos();
        	techType = Core::TechType::p();
        }

        Core::InstanceName voltageBiasName =createVoltageBiasInstanceName(index, techType);
        addOneTransistorVoltageBiasToCircuit(opAmp, createInstance(*voltageBias, voltageBiasName), innerGCCTerminal);
        addGateInstanceTerminalConnectedToADrain(innerGCCTerminal);
        index++;
    }

	void OpAmps::connectRemainingGateTerminals(Core::Circuit & opAmp, std::vector<Core::InstanceTerminal *> gateInstanceTerminalsSource,
			std::vector<Core::InstanceTerminal *>gateInstanceTerminalsOutput, Core::TechType techType,  int & index)
	{
		if(gateInstanceTerminalsSource.size() == gateInstanceTerminalsOutput.size() && !gateInstanceTerminalsSource.empty()
				&& allInstanceTerminalsArePartOfNotGateDrainConnectedTwoTransistorStacks(gateInstanceTerminalsSource,gateInstanceTerminalsOutput))
		{

	    	const Core::Circuit * voltageBias = nullptr;
	        if(techType.isN())
	        {
	        	voltageBias = &getVoltageBiases().getTwoDiodeTransistorVoltageBiasNmos();
	        }
	        else
	        {
	        	voltageBias = &getVoltageBiases().getTwoDiodeTransistorVoltageBiasPmos();

	        }

	        Core::InstanceName voltageBiasName = createVoltageBiasInstanceName(index, techType);
	        addTwoTransistorVoltageBiasToCircuit(opAmp, createInstance(*voltageBias, voltageBiasName), gateInstanceTerminalsOutput, gateInstanceTerminalsSource);

	        index++;


		}
		else
		{
	    	const Core::Circuit * voltageBias = nullptr;
	        if(techType.isN())
	        {
	        	voltageBias = &getVoltageBiases().getDiodeTransistorVoltageBiasNmos();
	        }
	        else
	        {
	        	voltageBias = &getVoltageBiases().getDiodeTransistorVoltageBiasPmos();

	        }
	        if(!gateInstanceTerminalsOutput.empty())
	        {
	        	Core::InstanceName voltageBiasOutputName = createVoltageBiasInstanceName(index, techType);
	        	addOneTransistorVoltageBiasToCircuit(opAmp, createInstance(*voltageBias, voltageBiasOutputName), gateInstanceTerminalsOutput);
	        	index++;
	        }

	        if(!gateInstanceTerminalsSource.empty())
	        {
	        	Core::InstanceName voltageBiasSourceName = createVoltageBiasInstanceName(index, techType);
	        	addOneTransistorVoltageBiasToCircuit(opAmp, createInstance(*voltageBias, voltageBiasSourceName), gateInstanceTerminalsSource);
	        	index++;
	        }
		}
	}

	void OpAmps::connectIbiasTerminal(Core::Circuit & opAmp, Core::TechType techType)
	{
		std::vector<Core::Instance * > voltageBiases = getVoltageBiasesOfBias(opAmp, techType);
		Core::Instance * ibiasVoltageBias = nullptr;
		for(auto & voltageBias : voltageBiases)
		{
			if(isSingleDiodeTransistor(voltageBias->getMaster())
					&& isConnectedToTerminalOfSourceTransistors(voltageBias->getInstanceTerminal(createTerminalId(VoltageBiases::OUT_TERMINAL_)).getNet()))
			{
				ibiasVoltageBias = voltageBias;
			}
		}
		if(ibiasVoltageBias == nullptr)
		{
			for(auto & voltageBias : voltageBiases)
			{
				if(sourceTransistorIsDiodeTransistor(voltageBias->getMaster()) && outputTransistorIsDiodeTransistor(voltageBias->getMaster())
						&& getDeviceNamesOfFlatCircuit(voltageBias->getMaster()).size() == 2)
				{
					ibiasVoltageBias = voltageBias;
				}
			}
		}
		if(ibiasVoltageBias == nullptr)
		{
			for(auto & voltageBias : voltageBiases)
			{
				if(getDeviceNamesOfFlatCircuit(voltageBias->getMaster()).size() == 2)
				{
					ibiasVoltageBias = voltageBias;
				}
			}
		}
		if(ibiasVoltageBias == nullptr)
		{
			if(techType.isP())
			{
				connectIbiasTerminal(opAmp, Core::TechType::n());
			}
			else
			{
				connectIbiasTerminal(opAmp, Core::TechType::p());
			}
		}
		else
		{
			connectInstanceTerminal(opAmp, *ibiasVoltageBias, VoltageBiases::IN_TERMINAL_, IBIAS_NET_);
		}
	}

	void OpAmps::addCurrentBiasesToCircuit(Core::Circuit & opAmp)
	{
		std::vector<Core::Instance* > voltageBiasesNmos = getVoltageBiasesOfBias(opAmp, Core::TechType::n());
		std::vector<Core::Instance* > voltageBiasesPmos = getVoltageBiasesOfBias(opAmp, Core::TechType::p());

		std::vector<Core::Instance *> vectorWithVoltageBiasConnectedToIbias;
		std::vector<Core::Instance *> vectorWithoutVoltageBiasConnectedToIbias;
		Core::TechType techTypeOfVectorWithoutVoltageBiasConnectedToIbias;

		if(hasVoltageBiasConnectedToIbias(voltageBiasesNmos))
		{
			vectorWithVoltageBiasConnectedToIbias = voltageBiasesNmos;
			vectorWithoutVoltageBiasConnectedToIbias = voltageBiasesPmos;
			techTypeOfVectorWithoutVoltageBiasConnectedToIbias = Core::TechType::p();
		}
		else
		{
			vectorWithVoltageBiasConnectedToIbias = voltageBiasesPmos;
			vectorWithoutVoltageBiasConnectedToIbias = voltageBiasesNmos;
			techTypeOfVectorWithoutVoltageBiasConnectedToIbias = Core::TechType::n();
		}

		if(vectorWithVoltageBiasConnectedToIbias.size() >1)
		{
			Core::Instance & referenceVoltageBias = findReferenceVoltageBias(opAmp, vectorWithoutVoltageBiasConnectedToIbias, techTypeOfVectorWithoutVoltageBiasConnectedToIbias);
			Core::Net & referenceVoltageBiasOutputNet = getReferenceVoltageBiasOutputNet(referenceVoltageBias);
			addCurrentBiasesToCircuit(opAmp, referenceVoltageBiasOutputNet, vectorWithVoltageBiasConnectedToIbias);
			vectorWithoutVoltageBiasConnectedToIbias = getVoltageBiasesOfBias(opAmp, techTypeOfVectorWithoutVoltageBiasConnectedToIbias);
		}
		if(!vectorWithoutVoltageBiasConnectedToIbias.empty())
		{
			Core::TechType techType = (**vectorWithVoltageBiasConnectedToIbias.begin()).getMaster().getCircuitIdentifier().getTechType();
			Core::Instance & referenceVoltageBias = findReferenceVoltageBias(opAmp, vectorWithVoltageBiasConnectedToIbias, techType);
			Core::Net & referenceVoltageBiasOutputNet = getReferenceVoltageBiasOutputNet(referenceVoltageBias);
			addCurrentBiasesToCircuit(opAmp, referenceVoltageBiasOutputNet, vectorWithoutVoltageBiasConnectedToIbias);
		}

	}

	void OpAmps::addCurrentBiasesToCircuit(Core::Circuit & opAmp, Core::Net & referenceVoltageBiasOutputNet,
			std::vector<Core::Instance*> voltageBiases)
	{
		Core::TechType techTypeVoltageBiases = (**voltageBiases.begin()).getMaster().getCircuitIdentifier().getTechType();
		Core::TechType techTypeCurrentBiases;
		int index = 1;
		if(techTypeVoltageBiases.isN())
		{
			techTypeCurrentBiases = Core::TechType::p();
		}
		else
		{
			techTypeCurrentBiases = Core::TechType::n();
		}

		for(auto & voltageBias : voltageBiases)
		{
			if(!isVoltageBiasConnectedToIbias(*voltageBias))
			{
				const Core::Circuit & currentBias = getCurrentBiases().getNormalTransistorCurrentBias(techTypeCurrentBiases);
				Core::InstanceName currentBiasName = createCurrentBiasInstanceName(index, techTypeCurrentBiases);
				Core::Instance & currentBiasInstance = createInstance(currentBias, currentBiasName);

				opAmp.addInstance(currentBiasInstance);
				currentBiasInstance.setCircuit(opAmp);

				Core::NetId inputId = createNetIdByInstanceName("input", voltageBias->getIdentifier().getInstanceName());
				opAmp.addNet(inputId);

				connectInstanceTerminal(opAmp, *voltageBias, VoltageBiases::IN_TERMINAL_, inputId);
				connectInstanceTerminal(opAmp, currentBiasInstance, VoltageBiases::IN_TERMINAL_, referenceVoltageBiasOutputNet.getIdentifier());
				connectInstanceTerminal(opAmp, currentBiasInstance, VoltageBiases::OUT_TERMINAL_, inputId);

				if(techTypeCurrentBiases.isN())
				{
					connectInstanceTerminal(opAmp, currentBiasInstance, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
				}
				else
				{
					connectInstanceTerminal(opAmp, currentBiasInstance, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
				}
				index++;
			}
		}
	}

	void OpAmps::addOneTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
			Core::InstanceTerminal & terminalCurrentBias)
	{
		opAmp.addInstance(voltageBias);
		voltageBias.setCircuit(opAmp);

		std::vector<Core::NetId> netNames = createNetNamesOfBiasInstance(voltageBias);
		addNetsToCircuit(opAmp, netNames);


		Core::InstanceName instanceName = voltageBias.getIdentifier().getInstanceName();
		Core::NetId outId = createNetIdByInstanceName("out", instanceName);


		terminalCurrentBias.connect(opAmp.findNet(outId));

		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUT_TERMINAL_, outId);

		Core::TechType techType = voltageBias.getMaster().getCircuitIdentifier().getTechType();
		if(terminalCurrentBias.getIdentifier() == createTerminalId(NonInvertingStages::INNERGCC_TERMINAL_))
		{
			Core::NetId sourceTransId = Core::NetName("sourceTransconductance").createRootIdentifier();
			opAmp.addNet(sourceTransId);
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, sourceTransId);
			Core::InstanceTerminal & sourceTransTerminal = opAmp.findInstance(createInstanceId(FIRSTSTAGE_)).findInstanceTerminal(createTerminalId(NonInvertingStages::SOURCETRANSCONDUCTANCE_TERMINAL_));
			sourceTransTerminal.connect(opAmp.findNet(sourceTransId));
		}
		else if(techType.isN())
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
		}
	}

	void OpAmps::addTwoTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
			Core::InstanceTerminal& terminalOutputCurrentBias, Core::InstanceTerminal& terminalSourceCurrentBias)
	{
		opAmp.addInstance(voltageBias);
		voltageBias.setCircuit(opAmp);

		std::vector<Core::NetId> netNames = createNetNamesOfBiasInstance(voltageBias);
		addNetsToCircuit(opAmp, netNames);


		Core::InstanceName instanceName = voltageBias.getIdentifier().getInstanceName();
		Core::NetId outInputId = createNetIdByInstanceName("outInput", instanceName);
		Core::NetId outSourceId = createNetIdByInstanceName("outSource", instanceName);

		terminalOutputCurrentBias.connect(opAmp.findNet(outInputId));
		terminalSourceCurrentBias.connect(opAmp.findNet(outSourceId));

		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUTINPUT_TERMINAL_, outInputId);
		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUTSOURCE_TERMINAL_, outSourceId);

		Core::TechType techType = voltageBias.getMaster().getCircuitIdentifier().getTechType();
		if(techType.isN())
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
		}

	}

	void OpAmps::addOneTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
						std::vector<Core::InstanceTerminal *> terminalsCurrentBias)
	{
		opAmp.addInstance(voltageBias);
		voltageBias.setCircuit(opAmp);

		bool gateNetAlreadyExists = false;
		Core::NetId gateNetId;

		for(auto & terminalCurrentBias : terminalsCurrentBias)
		{
			if(!isGateInstanceTerminalConnectedToADrain(*terminalCurrentBias)
					&& terminalCurrentBias->isConnected())
			{
				gateNetAlreadyExists = true;
				gateNetId = terminalCurrentBias->getNet().getIdentifier();
			}
		}

		std::vector<Core::NetId> netNames;
		Core::InstanceName instanceName = voltageBias.getIdentifier().getInstanceName();
		if(gateNetAlreadyExists)
		{
			Core::NetId inId = createNetIdByInstanceName("input", instanceName);
			netNames.push_back(inId);
		}
		else
		{
			netNames = createNetNamesOfBiasInstance(voltageBias);
			Core::NetId outId = createNetIdByInstanceName("out", instanceName);
			gateNetId = outId;
		}

		addNetsToCircuit(opAmp, netNames);

		for(auto & terminalCurrentBias : terminalsCurrentBias)
		{
			if(!isGateInstanceTerminalConnectedToADrain(*terminalCurrentBias))
			{
				if(!terminalCurrentBias->isConnected())
				{
					terminalCurrentBias->connect(opAmp.findNet(gateNetId));
				}

				addGateInstanceTerminalConnectedToADrain(*terminalCurrentBias);
			}
		}

		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUT_TERMINAL_, gateNetId);

		Core::TechType techType = voltageBias.getMaster().getCircuitIdentifier().getTechType();
		if(techType.isN())
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
		}
	}

	void OpAmps::addTwoTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
						std::vector<Core::InstanceTerminal*> terminalsOutputCurrentBias, std::vector<Core::InstanceTerminal*> terminalsSourceCurrentBias)
	{
		opAmp.addInstance(voltageBias);
		voltageBias.setCircuit(opAmp);

		std::vector<Core::NetId> netNames = createNetNamesOfBiasInstance(voltageBias);
		addNetsToCircuit(opAmp, netNames);


		Core::InstanceName instanceName = voltageBias.getIdentifier().getInstanceName();
		Core::NetId outInputId = createNetIdByInstanceName("outInput", instanceName);
		Core::NetId outSourceId = createNetIdByInstanceName("outSource", instanceName);

		for(auto & terminalOutputCurrentBias : terminalsOutputCurrentBias)
		{
			if(!isGateInstanceTerminalConnectedToADrain(*terminalOutputCurrentBias))
			{
				terminalOutputCurrentBias->connect(opAmp.findNet(outInputId));
				addGateInstanceTerminalConnectedToADrain(*terminalOutputCurrentBias);
			}

		}

		for(auto & terminalSourceCurrentBias : terminalsSourceCurrentBias)
		{
			if(!isGateInstanceTerminalConnectedToADrain(*terminalSourceCurrentBias))
			{
				terminalSourceCurrentBias->connect(opAmp.findNet(outSourceId));
				addGateInstanceTerminalConnectedToADrain(*terminalSourceCurrentBias);
			}
		}


		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUTINPUT_TERMINAL_, outInputId);
		connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::OUTSOURCE_TERMINAL_, outSourceId);

		Core::TechType techType = voltageBias.getMaster().getCircuitIdentifier().getTechType();
		if(techType.isN())
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(opAmp, voltageBias, VoltageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
		}
	}

    const Core::Circuit & OpAmps::findVoltageBiasOfImprovedWilsonCurrentMirror(std::vector<const Core::Circuit * > twoTransistorVoltageBiases) const
    {
    	const Core::Circuit * voltageBiasImprovedWilson = nullptr;
    	for(auto & voltageBias : twoTransistorVoltageBiases )
    	{
    		 if(outputTransistorIsDiodeTransistor(*voltageBias) && voltageBias->getGateNetsNotConnectedToADrain().size() == 1)
    	     {
    	          voltageBiasImprovedWilson = voltageBias;
    	     }
    	}

    	assert(voltageBiasImprovedWilson != nullptr, "There must be a voltageBias which is part of the improved wilson current mirror!");
    	return * voltageBiasImprovedWilson;

    }

	bool OpAmps::allInstanceTerminalsArePartOfNotGateDrainConnectedTwoTransistorStacks(
			std::vector<Core::InstanceTerminal *> gateInstanceTerminalsSource,std::vector<Core::InstanceTerminal *> gateInstanceTerminalsOutput) const
	{
		if(getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsSource).size() == getGateInstanceTerminalsNotConnectedToADrain(gateInstanceTerminalsOutput).size())
		{

			bool hasFreeOutputTerminal = true;
			for(auto & sourceInstanceTerminal : gateInstanceTerminalsSource)
			{
				if(!isGateInstanceTerminalConnectedToADrain(*sourceInstanceTerminal))
				{
					if(isGateInstanceTerminalSourceTransistorOfTranistorStack(*sourceInstanceTerminal))
					{
						Core::InstanceTerminal & belongingOutputInstanceTerminal = getBelongingGateOutputInstanceTerminal(*sourceInstanceTerminal);
						if(!terminalIsInVector(belongingOutputInstanceTerminal, gateInstanceTerminalsOutput))
						{
							hasFreeOutputTerminal = false;
							break;
						}
					}

				}
			}

            bool hasFreeSourceTerminal = true;
			for(auto & outputInstanceTerminal : gateInstanceTerminalsOutput)
			{
				if(!isGateInstanceTerminalConnectedToADrain(*outputInstanceTerminal))
				{
					Core::InstanceTerminal & belongingSourceInstanceTerminal = getBelongingGateSourceInstanceTerminal(*outputInstanceTerminal);
					if(!terminalIsInVector(belongingSourceInstanceTerminal, gateInstanceTerminalsSource))
					{
							hasFreeSourceTerminal = false;
							break;
					}
				}
			}

            return hasFreeOutputTerminal && hasFreeSourceTerminal;
		}
		else
		{
			return false;
		}
	}

    bool OpAmps::isGateNetOfCurrentBiasesOfImprovedWilsonCurrentMirror(Core::NetId netId, const Core::Circuit & flatCircuit) const
    {
    	Core::MosfetDeviceType mosfet;
    	const Core::Net & flatNet = flatCircuit.findNet(netId);
    	std::vector<const Core::Pin * > gatePins = flatNet.getPins(mosfet.gate());
    	if(gatePins.size() > 1)
    	{
    		return false; //ThecurrentBiases of a improved Wilson Current Mirror cannot exist in current mirror benches
    	}
    	else
    	{
    		const Core::Device &  outputTransistor = (**gatePins.begin()).getDevice();
    		const Core::Net & sourceNet = outputTransistor.findNet(mosfet.source());

    		if(sourceNet.hasPin(mosfet.gate()) && sourceNet.hasPin(mosfet.drain()))
    		{
    			bool hasDiodeSourceTransistor = false;
        		std::vector<const Core::Pin * > gatePinsAtSourceNet = sourceNet.getPins(mosfet.gate());
        		std::vector<const Core::Pin * > drainPinsAtSourceNet = sourceNet.getPins(mosfet.drain());

        		for(auto & gatePin : gatePinsAtSourceNet)
        		{
        			const Core::Device & gateTransistor = gatePin->getDevice();

        			for(auto & drainPin : drainPinsAtSourceNet)
        			{
        				const Core::Device & drainTransistor = drainPin->getDevice();
        				if(gateTransistor.getIdentifier() == drainTransistor.getIdentifier())
        				{
        					hasDiodeSourceTransistor = true;
        					break;
        				}
        			}
        			if(hasDiodeSourceTransistor)
        				break;
        		}

        		return hasDiodeSourceTransistor;
    		}
    		else
    		{
    			return false;
    		}

    	}
    }

    bool OpAmps::isVoltageBiasConnectedToIbias(Core::Instance & voltageBias) const
    {
    	if(voltageBias.findInstanceTerminal(createTerminalId(VoltageBiases::IN_TERMINAL_)).isConnected())
    	{
    		Core::Net & inputNet = voltageBias.findInstanceTerminal(createTerminalId(VoltageBiases::IN_TERMINAL_)).getNet();
    		return inputNet.getIdentifier() == IBIAS_NET_;
    	}
    	else
    	{
    		return false;
    	}

    }

    bool OpAmps::hasVoltageBiasConnectedToIbias(std::vector<Core::Instance*> voltageBiases) const
    {
    	bool isConnectedToIbias = false;

    	for(auto voltageBias : voltageBiases)
    	{
    		if(isVoltageBiasConnectedToIbias(*voltageBias))
    		{
    			isConnectedToIbias = true;
    			break;
    		}
    	}
    	return isConnectedToIbias;
    }

    bool OpAmps::isConnectedToTerminalOfSourceTransistors(const Core::Net & net)
    {
    	Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;

    	std::vector<Core::InstanceTerminal*> instanceTerminals = net.getInstanceTerminals();
    	bool allTransistorsConnectedToSourceNet = true;

    	for(auto & instanceTerminal : instanceTerminals)
    	{
    		Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(instanceTerminal->getInstance().getMaster());
    		const Core::Net & net = flatCircuit->findTerminal(instanceTerminal->getIdentifier()).getNet();

    		if(!net.connectedDevicesConnectedToASourceNet())
    		{
    			allTransistorsConnectedToSourceNet = false;
    			break;
    		}
    		delete flatCircuit;
    	}

    	delete flatCircuitRecursion;
    	return allTransistorsConnectedToSourceNet;
    }

	bool OpAmps::isGateInstanceTerminalSourceTransistorOfTranistorStack(Core::InstanceTerminal & sourceInstanceTerminal) const
	{
		Core::MosfetDeviceType mosfet;
		const Core::Circuit & instanceCircuit = sourceInstanceTerminal.getInstance().getMaster();
		Core::FlatCircuitRecursion* flatCircuitRecursion = new Core::FlatCircuitRecursion;
		const Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(instanceCircuit);
		assert(flatCircuit->hasNet(sourceInstanceTerminal.getTerminal().getNet().getIdentifier()), "Gate Instance Terminal ofsource transistor must be in circuit as it is not connected to any net");

		Core::NetId gateNet = sourceInstanceTerminal.getTerminal().getNet().getIdentifier();
		std::vector<const Core::Pin * > gatePins = flatCircuit->findNet(gateNet).getPins(mosfet.gate());

		bool isIt = true;

		for(auto & gatePin : gatePins)
		{
			const Core::Net & drainNet = gatePin->getDevice().findPin(mosfet.drain()).getNet();
			if(!drainNet.hasPin(mosfet.source()))
			{
				isIt = false;
				break;
			}
			else
			{
				bool onlyFirstStageInput = true;
				std::vector<const Core::Pin*> sourcePins = drainNet.getPins(mosfet.source());
				for(auto & sourcePin : sourcePins)
				{
					const Core::Device & device = sourcePin->getDevice();
					const Core::Net & outputNet = device.findNet(mosfet.gate());
					if(outputNet.getIdentifier() != IN1_NET_ && outputNet.getIdentifier() != IN2_NET_ && outputNet.getIdentifier() != NonInvertingStages::INNERTRANSCONDUCTANCE_NET_)
					{
						onlyFirstStageInput = false;
						break;
					}
				}
				if(onlyFirstStageInput)
				{
					isIt = false;
					break;
				}
			}

		}

		delete flatCircuitRecursion;
		delete flatCircuit;
		return isIt;
	}
    
	Core::InstanceTerminal & OpAmps::getBelongingGateOutputInstanceTerminal(Core::InstanceTerminal & sourceInstanceTerminal) const
	{
		Core::MosfetDeviceType mosfet;
		const Core::Circuit & instanceCircuit = sourceInstanceTerminal.getInstance().getMaster();
		Core::FlatCircuitRecursion* flatCircuitRecursion = new Core::FlatCircuitRecursion;
		const Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(instanceCircuit);
		assert(flatCircuit->hasNet(sourceInstanceTerminal.getTerminal().getNet().getIdentifier()), "Gate Instance Terminal ofsource transistor must be in circuit as it is not connected to any net");

		Core::NetId gateNet = sourceInstanceTerminal.getTerminal().getNet().getIdentifier();
		std::vector<const Core::Pin * > gatePins = flatCircuit->findNet(gateNet).getPins(mosfet.gate());

		Core::InstanceTerminal * outputInstanceTerminal = nullptr;

		for(auto & gatePin : gatePins)
		{
			const Core::Net & drainNet = gatePin->getDevice().findPin(mosfet.drain()).getNet();
			assert(drainNet.hasPin(mosfet.source()), "Instance termininal must be connected to a source transistor of a transistor stack");

			std::vector<const Core::Pin * > sourcePins =  drainNet.getPins(mosfet.source());
			for(auto & sourcePin : sourcePins)
			{
				const Core::Net & gateNet = sourcePin->getDevice().findPin(mosfet.gate()).getNet();
				Core::InstanceTerminal * potentialOutputTerminal = &sourceInstanceTerminal.getInstance().findInstanceTerminal(instanceCircuit.findTerminal(gateNet.getIdentifier()).getIdentifier());
                if(outputInstanceTerminal == nullptr)
                {
                    outputInstanceTerminal = potentialOutputTerminal;
                }
                else if(outputInstanceTerminal->getIdentifier() != potentialOutputTerminal->getIdentifier())
                {
                    assert(false, "Something went wrong the gate instance terminal of the output transistor should be equal");
                }
			}
		}

		delete flatCircuitRecursion;
		delete flatCircuit;
		return *outputInstanceTerminal;
	}

	Core::InstanceTerminal & OpAmps::getBelongingGateSourceInstanceTerminal(Core::InstanceTerminal & outputInstanceTerminal) const
	{
		Core::MosfetDeviceType mosfet;
		const Core::Circuit & instanceCircuit = outputInstanceTerminal.getInstance().getMaster();
		Core::FlatCircuitRecursion* flatCircuitRecursion = new Core::FlatCircuitRecursion;
		const Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(instanceCircuit);
		assert(flatCircuit->hasNet(outputInstanceTerminal.getTerminal().getNet().getIdentifier()), "Gate Instance Terminal of output transistor must be in circuit as it is not connected to any net");

		Core::NetId gateNet = outputInstanceTerminal.getTerminal().getNet().getIdentifier();
		std::vector<const Core::Pin * > gatePins = flatCircuit->findNet(gateNet).getPins(mosfet.gate());
		Core::InstanceTerminal * sourceInstanceTerminal = nullptr;

		for(auto & gatePin : gatePins)
		{
			const Core::Net & sourceNet = gatePin->getDevice().findPin(mosfet.source()).getNet();
			assert(sourceNet.hasPin(mosfet.drain()), "Instance termininal must be connected to a output transistor of a transistor stack");

			std::vector<const Core::Pin * > drainPins =  sourceNet.getPins(mosfet.drain());
			for(auto & drainPin : drainPins)
			{
				const Core::Net & gateNet = drainPin->getDevice().findPin(mosfet.gate()).getNet();
				Core::InstanceTerminal * potentialSourceTerminal = &outputInstanceTerminal.getInstance().findInstanceTerminal(instanceCircuit.findTerminal(gateNet.getIdentifier()).getIdentifier());
                if(sourceInstanceTerminal == nullptr && potentialSourceTerminal->getIdentifier() != createTerminalId(NonInvertingStages::IN1_TERMINAL_)
                		&& potentialSourceTerminal->getIdentifier() != createTerminalId(NonInvertingStages::IN2_TERMINAL_))
                {
                    sourceInstanceTerminal = potentialSourceTerminal;
                }
                else if(sourceInstanceTerminal->getIdentifier() != potentialSourceTerminal->getIdentifier()
                		&& potentialSourceTerminal->getIdentifier() != createTerminalId(NonInvertingStages::IN1_TERMINAL_)
                		&& potentialSourceTerminal->getIdentifier() != createTerminalId(NonInvertingStages::IN2_TERMINAL_))
                {
                    assert(false, "Something went wrong the gate instance terminal of the source transistor should be equal");
                }
			}
		}

		delete flatCircuitRecursion;
		delete flatCircuit;
		return *sourceInstanceTerminal;
	}

    Core::InstanceTerminal & OpAmps::getSourceTermimalImprovedWilsonCurrentMirror(Core::Instance & instance, Core::TechType techType) const
    {
    	Core::CircuitIds circuitIds;
    	assert(instance.getMaster().getCircuitIdentifier() == circuitIds.firstStage() || instance.getMaster().getCircuitIdentifier() == circuitIds.feedbackStage()
    			|| instance.getMaster().getCircuitIdentifier() == circuitIds.invertingStage(), "Instance must be either non inverting stage or inverting stage" );
    	assert(instance.hasInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIAS_TERMINAL_)) ||
    			instance.hasInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIASNMOS_TERMINAL_))
				|| instance.hasInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIASPMOS_TERMINAL_)), "Wilson Current mirror can only be part of stage bias");
    	if(instance.hasInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIAS_TERMINAL_)))
    	{
    		return instance.getInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIAS_TERMINAL_));
    	}
    	else if(techType.isN())
    	{
    		return instance.getInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIASNMOS_TERMINAL_));
    	}
    	else
    	{
    		return instance.getInstanceTerminal(createTerminalId(NonInvertingStages::INSOURCESTAGEBIASPMOS_TERMINAL_));
    	}
    }

    void OpAmps::addGateInstanceTerminalConnectedToADrain(const Core::InstanceTerminal & terminal)
    {
    	gateInstanceTerminalsConnectedToADrain_.push_back(&terminal);
    }

    bool OpAmps::isGateInstanceTerminalConnectedToADrain(const Core::InstanceTerminal & terminal) const
    {
    	bool isConnected = false;
    	for(auto & connectedTerminal : gateInstanceTerminalsConnectedToADrain_)
    	{
    		if(*connectedTerminal == terminal)
    		{
    			isConnected = true;
    			break;
    		}
    	}
    	return isConnected;
    }

    bool OpAmps::isGateNetOfNmosTransistors(const Core::Net & flatNet) const
    {
    	Core::MosfetDeviceType mosfet;
    	std::vector<const Core::Pin * > gatePins = flatNet.getPins(mosfet.gate());

    	if(gatePins.empty())
    	{
    		return false;
    	}
    	else
    	{
    		bool isNmos = true;
    		for(auto & gatePin : gatePins)
    		{
    			const Core::Device &  device = gatePin->getDevice();
    			if(!device.getTechType().isN())
    			{
    				isNmos = false;
    				break;
    			}
    		}
    		return isNmos;
    	}
    }

    bool OpAmps::isGateNetOfPmosTransistors(const Core::Net & flatNet) const
    {
    	Core::MosfetDeviceType mosfet;
    	std::vector<const Core::Pin * > gatePins = flatNet.getPins(mosfet.gate());

    	if(gatePins.empty())
    	{
    		return false;
    	}
    	else
    	{
    		bool isPmos = true;
    		for(auto & gatePin : gatePins)
    		{
    			const Core::Device &  device = gatePin->getDevice();
    			if(!device.getTechType().isP())
    			{
    				isPmos = false;
    				break;
    			}
    		}
    		return isPmos;
    	}
    }

	Core::InstanceName OpAmps::createVoltageBiasInstanceName(int index, Core::TechType techType) const
	{
		std::ostringstream oss;
		oss << VOLTAGEBIAS_STRING_ << "XX" << techType.toStr() << "XX" << index;
		Core::InstanceName name = Core::InstanceName(oss.str());
		return name;

	}

	Core::InstanceName OpAmps::createCurrentBiasInstanceName(int index, Core::TechType techType) const
	{
		std::ostringstream oss;
		oss << CURRENTBIAS_STRING_ << "XX" << techType.toStr() << "XX" << index;
		Core::InstanceName name = Core::InstanceName(oss.str());
		return name;
	}

	std::vector<Core::NetId>  OpAmps::createNetNamesOfBiasInstance(Core::Instance & biasInstance) const
	{
		std::vector<Core::NetId> netNames;
		Core::InstanceName instanceName = biasInstance.getIdentifier().getInstanceName();

		Core::NetId inputId = createNetIdByInstanceName("input", instanceName);
		netNames.push_back(inputId);

		if(getDeviceNamesOfFlatCircuit(biasInstance.getMaster()).size() == 1)
		{
			Core::NetId outId = createNetIdByInstanceName("out", instanceName);
			netNames.push_back(outId);
		}
		else
		{
			Core::NetId outInputId = createNetIdByInstanceName("outInput", instanceName);
			netNames.push_back(outInputId);

			Core::NetId outSourceId = createNetIdByInstanceName("outSource", instanceName);
			netNames.push_back(outSourceId);
		}

        return netNames;
	}

	Core::NetId OpAmps::createNetIdByInstanceName(std::string netName, Core::InstanceName instanceName) const
	{
		std::ostringstream oss;
		oss << netName << instanceName.toStr();

		return Core::NetName(oss.str()).createRootIdentifier();
	}

}
