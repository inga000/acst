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

#include "Partitioning/incl/Partitioning.h"
#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "Partitioning/incl/Parts/CapacitancePart.h"
#include "Partitioning/incl/Parts/ResistorPart.h"
#include "Partitioning/incl/Parts/CommonModeSignalDetectorPart.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"

#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/CapacitorDeviceType.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

	Partitioning::Partitioning() :
			result_(NULL)
	{
		idLoadPart_ = 1;
		idTransPart_ = 1;
		idBiasPart_ = 1;
		idCapacitancePart_ = 1;
		idUndefinedPart_ = 1;
		idCommonModeSignalDetectorPart_ = 1;
		idResistorPart_ = 1;
		idPositiveFeedbackPart_ = 1;
	}

	Partitioning::~Partitioning()
	{
	}

	Result & Partitioning::compute(const StructRec::StructureCircuits & circuits)
	{
		Result * result = new Result;

		setResult(*result);
		partitioningDifferentialPairs(circuits);
		if(result->hasTransconductanceParts())
		{
			partitioningBiasPartsOfDifferentialPairs(circuits);
			partitioningLoadPartsOfDifferentialPairs(circuits);
		}
		partitioningSecondStage(circuits);
		partitioningThirdStage(circuits);
		recognizeSecondSecondStage(circuits);

		if(result->hasSecondarySecondStage())
		{
			result->getFirstStage().setFirstStageType("symmetrical");
		}
		else if(!result->getFirstStage().hasFirstStageType())
		{
			result->getFirstStage().setFirstStageType("simple");
		}

		partitioningRemainingCurrentMirrors(circuits);
		partitioningVoltageReference1(circuits);
		classifyDiodeArrays(circuits);
		findPositiveFeedbackParts(circuits);
		partitioningCapacitances(circuits);
		findCommonModeSignalDetector(circuits);

		createUndefinedParts(circuits);
		return * result;

	}

	void Partitioning::partitioningDifferentialPairs(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName dPName =  StructRec::StructureName("MosfetDifferentialPair");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetDifferentialPair", "Input1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetDifferentialPair", "Input2");

		std::vector<const StructRec::Structure*> differentialPairs = circuits.findStructures(dPName);
		for(auto& it : differentialPairs)
		{
			const StructRec::Structure & dP = * it;
			if(!getResult().structureAlreadyClassified(dP))
			{
				TransconductancePart * transPart = new TransconductancePart(getIdTransPart());
				transPart->addMainStructure(dP, getResult());
				getResult().addTransconductancePart(*transPart);

				const StructRec::StructureNet & netGatePin1 = dP.findNet(gatePin1);
				std::vector<const StructRec::Structure*> dPsWithGatePin1OnGatePin1Net = netGatePin1.findConnectedStructures(gatePin1);
				std::vector<const StructRec::Structure*> dPsWithGatePin2OnGatePin1Net = netGatePin1.findConnectedStructures(gatePin2);
				const StructRec::StructureNet & netGatePin2 = dP.findNet(gatePin2);
				std::vector<const StructRec::Structure*> dPsWithGatePin1OnGatePin2Net = netGatePin2.findConnectedStructures(gatePin1);
				std::vector<const StructRec::Structure*> dPsWithGatePin2OnGatePin2Net = netGatePin2.findConnectedStructures(gatePin2);
				if((dPsWithGatePin1OnGatePin1Net.size() + dPsWithGatePin2OnGatePin1Net.size() + dPsWithGatePin1OnGatePin2Net.size() + dPsWithGatePin2OnGatePin2Net.size() )>2)
				{

					if(dPsWithGatePin1OnGatePin1Net ==  dPsWithGatePin2OnGatePin2Net && dPsWithGatePin2OnGatePin1Net == dPsWithGatePin1OnGatePin2Net )
					{
						const StructRec::Structure & otherDP = findOtherDifferentialPair(dPsWithGatePin1OnGatePin1Net, dPsWithGatePin2OnGatePin1Net, dP);
						if(dP.getTechType() != otherDP.getTechType())
						{
							transPart->setType("firstStage");
							transPart->setFirstStageType("complementary");
						}
						else
						{
							transPart->setType("feedBack");
						}
					}
					else
					{
						transPart->setType("feedBack");
					}
					transPart->addBelongingMainStructure(dPsWithGatePin1OnGatePin1Net,getResult());
					transPart->addBelongingMainStructure(dPsWithGatePin2OnGatePin1Net, getResult());
					transPart->addBelongingMainStructure(dPsWithGatePin1OnGatePin2Net, getResult());
					transPart->addBelongingMainStructure(dPsWithGatePin2OnGatePin2Net, getResult());
				}
				else
				{
					transPart->setType("firstStage");
					if(dP.hasParent())
					{
						const StructRec::Structure * parent = & dP.getParent();
						const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(parent);
						transPart->setHelperStructure(parentPair->getChild2());
						if(parentPair->getChild2().getTechType() == dP.getTechType())
						{
							transPart->setFirstStageType("telescopic");
						}
						else
						{
								transPart->setFirstStageType("foldedCascode");
						}

					}
				}


			}

		}
	}

	void Partitioning::partitioningBiasPartsOfDifferentialPairs(const StructRec::StructureCircuits & circuits)
	{
		std::vector<TransconductancePart*> transParts = getResult().getAllTransconductanceParts();
		for(std::vector<TransconductancePart*>::iterator it_transParts = transParts.begin(); it_transParts != transParts.end(); it_transParts++)
		{
			TransconductancePart* transPart = * it_transParts;
			std::vector<const StructRec::Structure*> & mainStructures = transPart->getMainStructures();
			for(std::vector<const StructRec::Structure*>::iterator it_mainStruc = mainStructures.begin();
				it_mainStruc != mainStructures.end(); it_mainStruc++)
			{
				const StructRec::Structure* mainStruc = *it_mainStruc;
				initializeBiasPartOfDifferentialPair(circuits, *mainStruc);

			}

		}
	}

	void Partitioning::partitioningLoadPartsOfDifferentialPairs(const StructRec::StructureCircuits & circuits)
	{
		std::vector<TransconductancePart*> transParts = getResult().getAllTransconductanceParts();
		std::vector<TransconductancePart*> laterInitialization;
		for(std::vector<TransconductancePart*>::iterator it_transParts = transParts.begin(); it_transParts != transParts.end(); it_transParts++)
		{
			TransconductancePart* transPart = * it_transParts;
			if(transPart->isFeedBack())
			{
				std::vector<const StructRec::Structure*> & mainStructures = transPart->getMainStructures();
				for(std::vector<const StructRec::Structure*>::iterator it_mainStruc = mainStructures.begin();
						it_mainStruc != mainStructures.end(); it_mainStruc++)
				{
					const StructRec::Structure* mainStruc = *it_mainStruc;
					initializeLoadPartsOfDifferentialPair(circuits, *mainStruc);

				}
			}
			else
			{
				laterInitialization.push_back(transPart);
			}
		}

		for(std::vector<TransconductancePart*>::iterator it_transParts = laterInitialization.begin(); it_transParts != laterInitialization.end(); it_transParts++)
		{
			TransconductancePart* transPart = * it_transParts;
			std::vector<const StructRec::Structure*> & mainStructures = transPart->getMainStructures();
			for(std::vector<const StructRec::Structure*>::iterator it_mainStruc = mainStructures.begin();
								it_mainStruc != mainStructures.end(); it_mainStruc++)
			{
				const StructRec::Structure* mainStruc = *it_mainStruc;
				initializeLoadPartsOfDifferentialPair(circuits, *mainStruc);
			}
		}

	}

	void Partitioning::partitioningSecondStage(const StructRec::StructureCircuits &circuits)
	{
		classifyMosfetAnalogInverter(circuits);
		classifyMosfetCascodedAnalogInverter(circuits);
		classifyMosfetAnalogInverterNmosCurrentMirrorLoad(circuits);
		classifyMosfetAnalogInverterPmosCurrentMirrorLoad(circuits);
		classifyMosfetAnalogInverterNmosDiodeTransistor(circuits);
		classifyMosfetAnalogInverterPmosDiodeTransistor(circuits);
		classifyMosfetAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad(circuits);
		classifyMosfetAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad(circuits);
		classifyMosfetCascodedPMOSAnalogInverter(circuits);
		classifyMosfetCascodedNMOSAnalogInverter(circuits);
		classifyMosfetCascodedPMOSAnalogInverterCurrentMirrorLoad(circuits);
		classifyMosfetCascodedNMOSAnalogInverterCurrentMirrorLoad(circuits);
		classifyMosfetCascodedPMOSAnalogInverterOneDiodeTransistor(circuits);
		classifyMosfetCascodedNMOSAnalogInverterOneDiodeTransistor(circuits);
		classifyMosfetCascodedAnalogInverterTwoCurrentMirrorLoads(circuits);

	}

	void Partitioning::partitioningThirdStage(const StructRec::StructureCircuits &circuits)
	{

		classifyMosfetAnalogInverter(circuits);
		classifyMosfetCascodedAnalogInverter(circuits);
		classifyMosfetCascodedPMOSAnalogInverter(circuits);
		classifyMosfetCascodedNMOSAnalogInverter(circuits);
		//By symmetrical OTAs it can happen that the third stage is connected to first and second stage

		if(getResult().hasSecondStage() && getResult().getSecondStages().size() > 1)
		{
			for(auto & secondStage: getResult().getSecondStages())
			{
				const StructRec::StructureNet * netGatePin = nullptr ;
				if(secondStage->isPrimarySecondStage())
				{

					for(auto & component : getResult().getBelongingComponents(*secondStage))
					{
						if(component->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
						{
							netGatePin = &component->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
						}
					}
					assert(netGatePin != nullptr, "There should be one transconductance component connected to  supply!");

					for(auto & comparisonSecondStage : getResult().getSecondStages())
					{
						if(comparisonSecondStage!= secondStage &&
							hasStageOutputNetConnection(*netGatePin, *comparisonSecondStage, circuits))
						{

							secondStage->setType("thirdStage");
						}
					}
				}

			}

		}


	}

	void Partitioning::classifyMosfetAnalogInverter(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetAnalogInverter");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetAnalogInverter", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedAnalogInverter(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodedAnalogInverter");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterNmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterPmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterNmosDiodeTransistor(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistor");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosDiodeTransistor", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosDiodeTransistor", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterPmosDiodeTransistor(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistor");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosDiodeTransistor", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosDiodeTransistor", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedPMOSAnalogInverter(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodedPMOSAnalogInverter");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedNMOSAnalogInverter(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodedNMOSAnalogInverter");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedPMOSAnalogInverterCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedNMOSAnalogInverterCurrentMirrorLoad(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedPMOSAnalogInverterOneDiodeTransistor(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodePMOSAnalogInverterOneDiodeTransistor");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodePMOSAnalogInverterOneDiodeTransistor", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodePMOSAnalogInverterOneDiodeTransistor", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedNMOSAnalogInverterOneDiodeTransistor(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyMosfetCascodedAnalogInverterTwoCurrentMirrorLoads(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName inverterName =  StructRec::StructureName("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads");
		const StructRec::StructurePinType gatePin1 = StructRec::StructurePinType("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads", "InputPMOS1");
		const StructRec::StructurePinType gatePin2 = StructRec::StructurePinType("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads", "InputNMOS1");

		classifyInverter(circuits, inverterName, gatePin1,  gatePin2);
	}

	void Partitioning::classifyInverter(const StructRec::StructureCircuits & circuits, const StructRec::StructureName inverterName, const StructRec::StructurePinType gatePin1, const StructRec::StructurePinType gatePin2)
	{

		std::vector<const StructRec::Structure*> inverters = circuits.findStructures(inverterName);
		int childNumberTransStruc = 0;
		int childNumberBiasStruc = 0;
		for(auto& it : inverters)
		{

			if(!getResult().structureAlreadyClassified(*it))
			{
			const StructRec::Structure * inverter = & *it;
			const StructRec::Pair * inverterPair =static_cast<const StructRec::Pair*>(inverter);
			const StructRec::StructureNet & netGatePin1 = inverter->findNet(gatePin1);

			std::string type;
			std::vector<const StructRec::Structure*> structuresGatePin1 = circuits.findConnectedStructures(netGatePin1.getIdentifier());
			for(auto& it : structuresGatePin1)
			{

				const StructRec::Structure & structure = * it;


				if(getResult().structureAlreadyClassified(structure) && (getResult().getPart(structure).isTransconductancePart() || getResult().getPart(structure).isLoadPart()))
				{

					if(hasStageOutputNetConnection(netGatePin1, getResult().getPart(structure), circuits)
							&& (hasVoltageBiasOutputConnection(inverter->findNet(gatePin2)) || onlyOneTransistorConnectedToNet(inverter->findNet(gatePin2)) ))
					{


						if(inverterPair->getChild(1).getStructureName() != StructRec::StructureName("MosfetMixedCascodePair2"))
						{
							childNumberTransStruc = 1;
							childNumberBiasStruc = 2;
							if(getResult().getPart(structure).isLoadPart() ||
									getResult().getTransconductancePart(structure).isFirstStage())
								type = "primarySecondStage";
							else if(getResult().getTransconductancePart(structure).isPrimarySecondStage())
								type = "thirdStage";
						}


					}

				}
			}

			const StructRec::StructureNet & netGatePin2 = inverter->findNet(gatePin2);

			std::vector<const StructRec::Structure*> structuresGatePin2 = circuits.findConnectedStructures(netGatePin2.getIdentifier());
			for(auto& it : structuresGatePin2)
			{
				const StructRec::Structure & structure = * it;
				if(getResult().structureAlreadyClassified(structure)
						&& (getResult().getPart(structure).isTransconductancePart() ||
								getResult().getPart(structure).isLoadPart()))
				{
					if(hasStageOutputNetConnection(netGatePin2, getResult().getPart(structure), circuits)
							&& (hasVoltageBiasOutputConnection(inverter->findNet(gatePin1))
									|| onlyOneTransistorConnectedToNet(inverter->findNet(gatePin1)) ))
					{
							if(inverterPair->getChild(2).getStructureName() != StructRec::StructureName("MosfetMixedCascodePair2"))
							{
								childNumberTransStruc = 2;
								childNumberBiasStruc = 1;
								if(getResult().getPart(structure).isLoadPart() ||
									getResult().getTransconductancePart(structure).isFirstStage())
									type = "primarySecondStage";
								else if(getResult().getTransconductancePart(structure).isPrimarySecondStage())
									type = "thirdStage";
							}


					}
				}
			}

				if(childNumberTransStruc != 0)
				{

					initializeInverterStage({&inverterPair->getChild(childNumberTransStruc)}, inverterPair->getChild(childNumberBiasStruc), type);
				}
			}
		}
	}

	void Partitioning::recognizeSecondSecondStage(const StructRec::StructureCircuits & circuits)
	{
		if(getResult().hasSecondStage() && getResult().getFirstStage().getLoadPart().size() ==1
				&& isSymmetricalOpAmpLoadPart(**getResult().getFirstStage().getLoadPart().begin()))
		{
			std::vector<const StructRec::Structure*> mainStructures = (*getResult().getAllLoadParts().begin())->getMainStructures();
			for(auto & mainStruct: mainStructures)
			{
				if(mainStruct->isPartOfCurrentMirror())
				{
				const StructRec::Structure & parentCurrentMirror = getRecursiveCurrentMirrorParent(*mainStruct);
				const StructRec::Pair * parentCurrentMirrorPair =static_cast<const StructRec::Pair*>(&parentCurrentMirror);
				const StructRec::Structure & potentialSecondStage2Transconductance = parentCurrentMirrorPair->getChild2();

				std::vector<const StructRec::Structure *> arrayChildren = potentialSecondStage2Transconductance.getArrayChildren();

				for(auto & arrayChild : arrayChildren)
				{
					if(arrayChild->getStructureName() == StructRec::StructureName("MosfetNormalArray") && arrayChild->findNet(StructRec::StructurePinType("MosfetNormalArray", "Source")).isSupply())
					{
						std::vector<const StructRec::Structure *> drainConnectedStructures = arrayChild->findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getAllConnectedStructures();

						for(auto & it_connectedStruct: drainConnectedStructures)
						{
							if(it_connectedStruct->getIdentifier() != arrayChild->getIdentifier() &&
									potentialSecondStage2Transconductance.getTechType() == it_connectedStruct->getTechType() &&
									it_connectedStruct->getIdentifier().getName().toStr() =="MosfetNormalArray")
							{
								std::vector<const StructRec::Structure *> potentialBiasStruct = it_connectedStruct->findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getAllConnectedStructures();

								const StructRec::Structure * biasStruct = findBiasStructToSecondSecondStageTransconductance(potentialBiasStruct);

								if(biasStruct != nullptr)
								{
									if(!getResult().structureAlreadyClassified(potentialSecondStage2Transconductance)	)
									{
										std::vector<const StructRec::Structure *> transStructs = {arrayChild, it_connectedStruct};
										initializeInverterStage(transStructs, *biasStruct, "secondarySecondStage");
									}
									else if(getResult().getPart(potentialSecondStage2Transconductance).isTransconductancePart()
											&& getResult().getTransconductancePart(potentialSecondStage2Transconductance).isPrimarySecondStage())
									{
										if(!isConnectedAtDrainToCapacitance(getResult().getTransconductancePart(potentialSecondStage2Transconductance)) && !hasThirdStageInputConnection(getResult().getTransconductancePart(potentialSecondStage2Transconductance)))
										{

											getResult().getTransconductancePart(potentialSecondStage2Transconductance).setType("secondarySecondStage");
										}
									}
								}
							}

						}
						if(!getResult().hasSecondarySecondStage())
						{
							const StructRec::Structure * biasStruct = findBiasStructToSecondSecondStageTransconductance(drainConnectedStructures);
							if(biasStruct != nullptr)
							{
								if(!getResult().structureAlreadyClassified(potentialSecondStage2Transconductance)	)
								{
									std::vector<const StructRec::Structure *> transStructs = {&potentialSecondStage2Transconductance};
									initializeInverterStage(transStructs, *biasStruct, "secondarySecondStage");
								}
								else if(getResult().getPart(potentialSecondStage2Transconductance).isTransconductancePart()
										&& getResult().getTransconductancePart(potentialSecondStage2Transconductance).isPrimarySecondStage())
								{
									if(!isConnectedAtDrainToCapacitance(getResult().getTransconductancePart(potentialSecondStage2Transconductance)) && !hasThirdStageInputConnection(getResult().getTransconductancePart(potentialSecondStage2Transconductance)))
									{
										getResult().getTransconductancePart(potentialSecondStage2Transconductance).setType("secondarySecondStage");
										}
								}
							}
						}

					}
				}
			}
		}
		}
	}

	const StructRec::Structure * Partitioning::findBiasStructToSecondSecondStageTransconductance(std::vector<const StructRec::Structure*>potentialBiasStruct)
	{
		const StructRec::Structure * voltageBias = nullptr;
		for(auto & it_potentialBiasStruct :potentialBiasStruct)
		{
			const std::vector<StructRec::Structure *> parents = it_potentialBiasStruct->getParents();
			for(auto & it_parents : parents)
			{

				if(isVoltageBias(*it_potentialBiasStruct))
				{
					voltageBias = it_potentialBiasStruct;
					break;
				}
				else if(isVoltageBias(*it_parents))
				{
					voltageBias = it_parents;
					break;
				}

			}
		}
		if(voltageBias == nullptr)
		{
			return nullptr;
		}
		else
		{
			std::vector<const StructRec::Structure *> parentsCMConnectedStruct = voltageBias->getTopmostParents();

			const StructRec::Pair * parentCMConnectedStructPair = nullptr;
			for(auto & parent : parentsCMConnectedStruct)
			{
				if(parent->isCurrentMirror())
				{
					parentCMConnectedStructPair = static_cast<const StructRec::Pair*>(parent);
				}
			}

			std::vector<const StructRec::Structure*> potentialBiasSecondStageArrays = parentCMConnectedStructPair->getChild2().getArrayChildren();

			for(auto & secondStage : getResult().getSecondStages())
			{
				BiasPart & biasPartSecondStage = **secondStage->getBiasPart().begin();
				if(biasPartSecondStage.getPartId() != getResult().getPart(*voltageBias).getPartId())
				{
					for(auto & it_biasComps :getResult().getBelongingComponents(biasPartSecondStage) )
					{
						if((**potentialBiasSecondStageArrays.begin()).getIdentifier() == it_biasComps->getArray().getIdentifier())
						{
							return &parentCMConnectedStructPair->getChild1();
						}
					}
				}

			}

			return nullptr;
		}
	}
	void Partitioning::partitioningRemainingCurrentMirrors(const StructRec::StructureCircuits & circuits)
	{
		std::vector<const StructRec::Structure*> structures = circuits.findAllStructures();
		for(auto& it : structures)
		{
			const StructRec::Structure * structure = & *it;
			if(isCurrentMirror(*structure))
			{
				const StructRec::Pair * pair =static_cast<const StructRec::Pair*>(structure);
				if(!getResult().structureAlreadyClassified(pair->getChild1()) && !getResult().structureAlreadyClassified(pair->getChild2()))
				{
					BiasPart * voltageBias = new BiasPart(getIdBiasPart());
					voltageBias->setType("voltageBias");
					voltageBias->addMainStructure(pair->getChild1(), getResult());
					getResult().addBiasPart(*voltageBias);

					BiasPart * currentBias = new BiasPart(getIdBiasPart());
					currentBias->setType("currentBias");
					currentBias->addMainStructure(pair->getChild2(), getResult());
					getResult().addBiasPart(*currentBias);
					voltageBias->addBiasedPart(*currentBias);

					addBiasedPartTypeVoltageBias(*pair, circuits);
				}
				else if(!getResult().structureAlreadyClassified(pair->getChild2()))
				{
					BiasPart * currentBias = new BiasPart(getIdBiasPart());
					currentBias->setType("currentBias");
					currentBias->addMainStructure(pair->getChild2(), getResult());
					getResult().addBiasPart(*currentBias);
					if(getResult().getPart(pair->getChild1()).isBiasPart())
					{
						getResult().getBiasPart(pair->getChild1()).addBiasedPart(*currentBias);
					}

					addBiasedPartTypeVoltageBias(*pair, circuits);
				}
				else if(!getResult().structureAlreadyClassified(pair->getChild1()))
				{
					BiasPart * voltageBias = new BiasPart(getIdBiasPart());
					voltageBias->setType("voltageBias");
					voltageBias->addMainStructure(pair->getChild1(), getResult());
					getResult().addBiasPart(*voltageBias);
					voltageBias->addBiasedPart(getResult().getPart(pair->getChild2()));

					addBiasedPartToCurrentBias(*pair,circuits);
				}
			}
		}
	}

	void Partitioning::partitioningVoltageReference1(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName voltRefName = StructRec::StructureName("MosfetVoltageReference1");
		std::vector<const StructRec::Structure*> voltRefs = circuits.findStructures(voltRefName);

		bool isLoadPart = false;
		bool isCurrentBias = false;

		for(auto& it_voltRefs : voltRefs)
		{
			const StructRec::Structure & voltRef = * it_voltRefs;
			if(!getResult().structureAlreadyClassified(voltRef))
			{
				const StructRec::StructurePinType input = StructRec::StructurePinType("MosfetVoltageReference1", "Input");
				const StructRec::StructureNet & inputNet = voltRef.findNet(input);
				std::vector<const StructRec::Structure*> connectedStructs = circuits.findConnectedStructures(inputNet.getIdentifier());

				for(auto& it_connectedStructs : connectedStructs)
				{
					const StructRec::Structure & connectedStruct = * it_connectedStructs;
					if(getResult().structureAlreadyClassified(connectedStruct) && hasCurrentBiasInputConnectionOnNet(connectedStruct, inputNet))
					{
						isLoadPart = getResult().getPart(connectedStruct).isLoadPart();
						if (getResult().getPart(connectedStruct).isBiasPart())
						{
							if(getResult().getBiasPart(connectedStruct).isCurrentBias())
								isCurrentBias = true;
						}
						if(isLoadPart)
							break;
					}
				}
				if(isLoadPart)
				{
					BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
					biasPartVoltage->addMainStructure(voltRef, getResult());
					biasPartVoltage->setType("voltageBias");
					getResult().addBiasPart(*biasPartVoltage);
					for(auto& it_connectedStructs : connectedStructs)
					{
						const StructRec::Structure & connectedStruct = * it_connectedStructs;
						if(getResult().structureAlreadyClassified(connectedStruct) && hasCurrentBiasInputConnectionOnNet(connectedStruct,inputNet))
						{
							if(getResult().getPart(connectedStruct).isLoadPart())
							{
								if(!biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)) && !connectedStruct.hasCommonDevices(voltRef))
									biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
								if(!getResult().getLoadPart(connectedStruct).hasBiasPart(biasPartVoltage->getPartId()))
									getResult().getLoadPart(connectedStruct).addBiasPart(*biasPartVoltage);
							}
							else if(getResult().getPart(connectedStruct).isBiasPart())
							{
								if(getResult().getBiasPart(connectedStruct).isCurrentBias() && !biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)))
								{
									biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
								}
							}
						}
						if(getResult().structureAlreadyClassified(connectedStruct) && hasCurrentBiasOutputConnectionOnNet(connectedStruct,inputNet) )
						{
							if(!getResult().getBiasPart(connectedStruct).hasBiasedPart(*biasPartVoltage)  && !connectedStruct.hasCommonDevices(voltRef))
							{

								getResult().getBiasPart(connectedStruct).addBiasedPart(*biasPartVoltage);
							}

						}
					}

				}
				else if(isCurrentBias)
				{
					BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
					biasPartVoltage->addMainStructure(voltRef, getResult());
					biasPartVoltage->setType("voltageBias");
					getResult().addBiasPart(*biasPartVoltage);

					for(auto& it_connectedStructs : connectedStructs)
					{
						const StructRec::Structure & connectedStruct = * it_connectedStructs;
						if(getResult().structureAlreadyClassified(connectedStruct))
						{
							if(getResult().getPart(connectedStruct).isBiasPart() && hasCurrentBiasInputConnectionOnNet(connectedStruct, inputNet))
							{
								if(getResult().getBiasPart(connectedStruct).isCurrentBias() && !biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)) )
								{
									biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
								}
							}
							if(getResult().getPart(connectedStruct).isBiasPart() && getResult().getBiasPart(connectedStruct).isCurrentBias() && hasCurrentBiasOutputConnectionOnNet(connectedStruct,inputNet))
							{
								if(!getResult().getBiasPart(connectedStruct).hasBiasedPart(*biasPartVoltage))
								{
									getResult().getBiasPart(connectedStruct).addBiasedPart(*biasPartVoltage);
								}
							}
						}

					}
				}
			}
		}
	}


	void Partitioning::classifyDiodeArrays(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName diodeArrayName = StructRec::StructureName("MosfetDiodeArray");
		std::vector<const StructRec::Structure*> diodeArrays = circuits.findStructures(diodeArrayName);

		bool isLoadPart = false;
		bool isCurrentBias = false;
		bool isVoltageBias = false;
		bool isTransconductancePart = false;

		for(auto& it_diodeArrays : diodeArrays)
		{

			const StructRec::Structure & diodeArray = * it_diodeArrays;
			if(!getResult().structureAlreadyClassified(diodeArray))
			{

				const StructRec::StructurePinType gatePin = StructRec::StructurePinType("MosfetDiodeArray", "Drain");
				const StructRec::StructurePinType sourcePin = StructRec::StructurePinType("MosfetDiodeArray", "Source");
				if(diodeArray.findNet(sourcePin).isSupply())
				{
					const StructRec::StructureNet & gateNet = diodeArray.findNet(gatePin);
					std::vector<const StructRec::Structure*> connectedStructs = gateNet.getAllConnectedStructures();

					for(auto& it_connectedStructs : connectedStructs)
					{
						const StructRec::Structure & connectedStruct = * it_connectedStructs;

						if(getResult().structureAlreadyClassified(connectedStruct)
								&& hasCurrentBiasInputConnectionOnNet(connectedStruct, gateNet)
								&& diodeArray.getTechType() == connectedStruct.getTechType())
						{
							isLoadPart = getResult().getPart(connectedStruct).isLoadPart();
							isTransconductancePart = getResult().getPart(connectedStruct).isTransconductancePart();
							if (getResult().getPart(connectedStruct).isBiasPart())
							{

								if(getResult().getBiasPart(connectedStruct).isCurrentBias())
									isCurrentBias = true;
								else if(getResult().getBiasPart(connectedStruct).isVoltageBias())
									isVoltageBias = true;
							}
							if(isLoadPart || isTransconductancePart)
								break;

						}
					}

					if(isLoadPart || isTransconductancePart)
					{
						BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
						biasPartVoltage->addMainStructure(diodeArray, getResult());
						biasPartVoltage->setType("voltageBias");
						getResult().addBiasPart(*biasPartVoltage);
						for(auto& it_connectedStructs : connectedStructs)
						{
							const StructRec::Structure & connectedStruct = * it_connectedStructs;
							if(getResult().structureAlreadyClassified(connectedStruct) &&
									hasCurrentBiasInputConnectionOnNet(connectedStruct,gateNet))
							{
								if(getResult().getPart(connectedStruct).isLoadPart()
										&& diodeArray.getTechType() == connectedStruct.getTechType())
								{
									if(!biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)) &&
											!connectedStruct.hasCommonDevices(diodeArray))
										biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
									if(!getResult().getLoadPart(connectedStruct).hasBiasPart(biasPartVoltage->getPartId()))
										getResult().getLoadPart(connectedStruct).addBiasPart(*biasPartVoltage);
								}
								else if(getResult().getPart(connectedStruct).isBiasPart()
										&& diodeArray.getTechType() == connectedStruct.getTechType())
								{
									if(!biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)))
									{
										biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
									}
								}
								else if(getResult().getPart(connectedStruct).isTransconductancePart()
										&& diodeArray.getTechType() == connectedStruct.getTechType())
								{
									if(!biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct)) &&
											!connectedStruct.hasCommonDevices(diodeArray))
										biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
									if(!getResult().getTransconductancePart(connectedStruct).hasBiasPart(biasPartVoltage->getPartId()))
										getResult().getTransconductancePart(connectedStruct).addBiasPart(*biasPartVoltage);
								}
							}
							if(getResult().structureAlreadyClassified(connectedStruct) && hasCurrentBiasOutputConnectionOnNet(connectedStruct,gateNet) )
							{

								if(!getResult().getBiasPart(connectedStruct).hasBiasedPart(*biasPartVoltage)  && !connectedStruct.hasCommonDevices(diodeArray))
								{
									getResult().getBiasPart(connectedStruct).addBiasedPart(*biasPartVoltage);
								}
							}
						}
					}
					else if(isCurrentBias || isVoltageBias)
					{

						BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
						biasPartVoltage->addMainStructure(diodeArray, getResult());
						biasPartVoltage->setType("voltageBias");
						getResult().addBiasPart(*biasPartVoltage);

						for(auto& it_connectedStructs : connectedStructs)
						{
							const StructRec::Structure & connectedStruct = * it_connectedStructs;
							if(getResult().structureAlreadyClassified(connectedStruct))
							{
								if(getResult().getPart(connectedStruct).isBiasPart() && hasCurrentBiasInputConnectionOnNet(connectedStruct, gateNet))
								{
									if(!biasPartVoltage->hasBiasedPart(getResult().getPart(connectedStruct))
											&& diodeArray.getTechType() == connectedStruct.getTechType())
									{
										biasPartVoltage->addBiasedPart(getResult().getPart(connectedStruct));
									}
								}
								if(getResult().getPart(connectedStruct).isBiasPart() && getResult().getBiasPart(connectedStruct).isCurrentBias() && hasCurrentBiasOutputConnectionOnNet(connectedStruct,gateNet))
								{
									if(!getResult().getBiasPart(connectedStruct).hasBiasedPart(*biasPartVoltage))
									{
										getResult().getBiasPart(connectedStruct).addBiasedPart(*biasPartVoltage);
									}
								}
							}
						}
					}
				}
			}
			else if(getResult().findComponent(diodeArray.getIdentifier()).getPart().isBiasPart())
			{

				const StructRec::StructureNet & drainNet = diodeArray.findNet(StructRec::StructurePinType(diodeArray.getStructureName().toStr(), "Drain" ));
				std::vector<const StructRec::Structure*> normalArrays =  drainNet.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				for(auto & normalArray : normalArrays)
				{
					if(getResult().structureAlreadyClassified(*normalArray))
					{
						if(getResult().findComponent(normalArray->getIdentifier()).getPart().getPartId() != getResult().findComponent(diodeArray.getIdentifier()).getPart().getPartId()
								&&normalArray->getTechType() == diodeArray.getTechType())
						{
							if(!getResult().getBiasPart(diodeArray).hasBiasedPart(getResult().findComponent(normalArray->getIdentifier()).getPart()))
							{
								getResult().getBiasPart(diodeArray).addBiasedPart(getResult().findComponent(normalArray->getIdentifier()).getPart());
							}
						}
					}
					else
					{
						BiasPart * biasPartCurrent = new BiasPart(getIdBiasPart());
						biasPartCurrent->addMainStructure(*normalArray, getResult());
						biasPartCurrent->setType("currentBias");
						getResult().addBiasPart(*biasPartCurrent);
						getResult().getBiasPart(diodeArray).addBiasedPart(getResult().findComponent(normalArray->getIdentifier()).getPart());
					}
				}
			}
		}

	}

	void Partitioning::findPositiveFeedbackParts(const StructRec::StructureCircuits & circuits)
	{

		assert(getResult().hasFirstStage(), "A first stage needs to be recognized before the positive feedback can be found!");

		TransconductancePart & firstStage = getResult().getFirstStage();

		//Complementary First Stage are not yet supported
		if(!firstStage.isComplementary())
		{
			assert(getResult().getBelongingComponents(firstStage).size() == 2, "The first stage should have only two components!");
			const StructRec::StructureNet * diffPairOutputNet1 = nullptr;
			const StructRec::StructureNet * diffPairOutputNet2 = nullptr;

			for(auto& comp: getResult().getBelongingComponents(firstStage))
			{
				if(diffPairOutputNet1 == nullptr)
				{
					diffPairOutputNet1 = &comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				}
				else
				{
					diffPairOutputNet2 = &comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
				}

			}

			const StructRec::Structure* possiblePosFeedbackStruc1 = nullptr;
			const StructRec::Structure* possiblePosFeedbackStruc2 = nullptr;

			std::vector<const StructRec::Structure * >  ntas1 = diffPairOutputNet1->findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Source"));
			std::vector<const StructRec::Structure * >  ntas2 = diffPairOutputNet2->findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Source"));

			std::vector<const StructRec::Structure * >  dtas1 = diffPairOutputNet1->findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Source"));
			std::vector<const StructRec::Structure * >  dtas2 = diffPairOutputNet2->findConnectedStructures(StructRec::StructurePinType("MosfetDiodeArray", "Source"));

			for(auto & nta : ntas1)
			{
				if(!getResult().structureAlreadyClassified(*nta))
				{
					possiblePosFeedbackStruc1 = nta;
				}
			}

			for(auto & nta : ntas2)
			{
				if(!getResult().structureAlreadyClassified(*nta))
				{
					possiblePosFeedbackStruc2 = nta;
				}
			}

			const StructRec::StructureNet * inputNet1 = nullptr;
			const StructRec::StructureNet * inputNet2 = nullptr;

			if(possiblePosFeedbackStruc1 != nullptr && possiblePosFeedbackStruc2 != nullptr)
			{
				inputNet1 = &possiblePosFeedbackStruc1->findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				inputNet2 = &possiblePosFeedbackStruc1->findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
			}
			else
			{
				for(auto & dta : dtas1)
				{
					if(!getResult().structureAlreadyClassified(*dta))
					{
						possiblePosFeedbackStruc1 = dta;
					}
				}

				for(auto & dta : dtas2)
				{
					if(!getResult().structureAlreadyClassified(*dta))
					{
						possiblePosFeedbackStruc2 = dta;
					}
				}
				if(possiblePosFeedbackStruc1 != nullptr && possiblePosFeedbackStruc2 != nullptr)
				{
					inputNet1 = &possiblePosFeedbackStruc1->findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
					inputNet2 = &possiblePosFeedbackStruc1->findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));

				}
			}
			if(inputNet1 != nullptr &&  inputNet2 != nullptr)
			{
				if((hasFirstStageOutputConnection(*inputNet1, circuits) && hasFirstStageOutputConnection(*inputNet2, circuits)) ||
						(hasSecondStageInputConnection(*inputNet1, circuits) && hasSecondStageInputConnection(*inputNet2, circuits)))
				{
					PositiveFeedbackPart * positiveFeedbackPart = new PositiveFeedbackPart(getIdPositiveFeedbackPart());
					positiveFeedbackPart->addBelongingMainStructure({possiblePosFeedbackStruc1, possiblePosFeedbackStruc2}, getResult());
					positiveFeedbackPart->setIsGainEnhancer(true);
					positiveFeedbackPart->setStage(firstStage);

					getResult().addPositiveFeedbackPart(*positiveFeedbackPart);
				}
				else if(inputNet1->getIdentifier() == inputNet2->getIdentifier())
				{
					PositiveFeedbackPart * positiveFeedbackPart = new PositiveFeedbackPart(getIdPositiveFeedbackPart());
					positiveFeedbackPart->addBelongingMainStructure({possiblePosFeedbackStruc1, possiblePosFeedbackStruc2}, getResult());
					positiveFeedbackPart->setIsSlewRateEnhancer(true);
					positiveFeedbackPart->setStage(firstStage);
					getResult().addPositiveFeedbackPart(*positiveFeedbackPart);
				}
			}
		}

	}

	void Partitioning::findCommonModeSignalDetector(const StructRec::StructureCircuits &circuits)
	{
		const StructRec::StructureName commonModeSignalDetectorName = StructRec::StructureName("CommonModeSignalDetector");
		std::vector<const StructRec::Structure*> commonModeSignalDetectors = circuits.findStructures(commonModeSignalDetectorName);
		const StructRec::StructurePinType output = StructRec::StructurePinType("CommonModeSignalDetector", "Output");
		const StructRec::StructurePinType input = StructRec::StructurePinType("CommonModeSignalDetector", "Input");

		for(auto & commonModeSignalDetector : commonModeSignalDetectors )
		{
			const StructRec::StructureNet &netInput = commonModeSignalDetector->findNet(input);
			const StructRec::StructureNet& netOutput = commonModeSignalDetector->findNet(output);

			if((hasSecondStageOutputConnection(netInput, circuits) && hasSecondStageOutputConnection(netOutput, circuits))
					|| (hasFirstStageOutputConnection(netInput, circuits) && hasFirstStageOutputConnection(netOutput, circuits)))
			{
				if(!getResult().structureAlreadyClassified(*commonModeSignalDetector))
				{
					CommonModeSignalDetectorPart * detectPart = new CommonModeSignalDetectorPart(getIdCommonModeSignalDetectorPart());
					detectPart->addMainStructure(*commonModeSignalDetector, getResult());
					getResult().addCommonModeSignalDetectorPart(*detectPart);
				}
			}
		}


		const StructRec::StructureName resistorRowName = StructRec::StructureName("ResistorRow");
		std::vector<const StructRec::Structure*> resistorRows = circuits.findStructures(resistorRowName);
		const StructRec::StructurePinType outputPin = StructRec::StructurePinType("ResistorRow", "Output");
		const StructRec::StructurePinType inputPin = StructRec::StructurePinType("ResistorRow", "Input");

		for(auto & resistorRow : resistorRows )
		{
			const StructRec::StructureNet &netInput = resistorRow->findNet(inputPin);
			const StructRec::StructureNet& netOutput = resistorRow->findNet(outputPin);

			if((hasSecondStageOutputConnection(netInput, circuits) && hasSecondStageOutputConnection(netOutput, circuits))
					|| (hasFirstStageOutputConnection(netInput, circuits) && hasFirstStageOutputConnection(netOutput, circuits)))
			{
				if(!getResult().structureAlreadyClassified(*resistorRow))
				{
				CommonModeSignalDetectorPart * detectPart = new CommonModeSignalDetectorPart(getIdCommonModeSignalDetectorPart());
				detectPart->addMainStructure(*resistorRow, getResult());
				getResult().addCommonModeSignalDetectorPart(*detectPart);
				}
			}
		}
	}


	void Partitioning::partitioningCapacitances(const StructRec::StructureCircuits & circuits)
	{
		const StructRec::StructureName capacitorName = StructRec::StructureName("CapacitorArray");
		std::vector<const StructRec::Structure*> capacitors = circuits.findStructures(capacitorName);

		const StructRec::StructurePinType minus = StructRec::StructurePinType("CapacitorArray", "Minus");
		const StructRec::StructurePinType plus = StructRec::StructurePinType("CapacitorArray", "Plus");

		for(auto& it_capacitors : capacitors)
		{


			const StructRec::Structure & capacitor = *it_capacitors;
			const StructRec::StructureNet &netMinus = capacitor.findNet(minus);
			const StructRec::StructureNet& netPlus = capacitor.findNet(plus);

			if( (hasSecondStageOutputConnection(netMinus, circuits) && hasGroundConnection(netPlus) ) || (hasSecondStageOutputConnection(netPlus, circuits) && hasGroundConnection(netMinus) )
					||(hasFirstStageOutputConnection(netPlus, circuits) && hasGroundConnection(netMinus)) || (hasFirstStageOutputConnection(netMinus, circuits) && hasGroundConnection(netPlus))
					||(hasThirdStageOutputConnection(netPlus, circuits) && hasGroundConnection(netMinus)) || (hasThirdStageOutputConnection(netMinus, circuits) && hasGroundConnection(netPlus)))
			{
				CapacitancePart * capPart = new CapacitancePart(getIdCapacitancePart());
				capPart->setType("load");
				capPart->addMainStructure(capacitor, getResult());
				getResult().addCapacitancePart(*capPart);
			}
			else if((hasSecondStageOutputConnection(netMinus, circuits) && hasFirstStageOutputConnection(netPlus, circuits))
					|| (hasSecondStageOutputConnection(netPlus, circuits) && hasFirstStageOutputConnection(netMinus, circuits))
					|| (hasThirdStageOutputConnection(netMinus, circuits) && hasSecondStageOutputConnection(netPlus, circuits))
					|| (hasThirdStageOutputConnection(netPlus, circuits) && hasSecondStageOutputConnection(netMinus, circuits)))
			{
				CapacitancePart * capPart = new CapacitancePart(getIdCapacitancePart());
				capPart->setType("compensation");
				capPart->addMainStructure(capacitor, getResult());
				getResult().addCapacitancePart(*capPart);
			}
			else if((hasSecondStageOutputConnection(netMinus, circuits) && netPlus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() ==1)
					|| (hasSecondStageOutputConnection(netPlus, circuits) && netMinus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() ==1)
					|| (hasThirdStageOutputConnection(netMinus, circuits) && netPlus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() ==1)
					|| (hasThirdStageOutputConnection(netPlus, circuits) && netMinus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() ==1))
			{
				const StructRec::Structure * nta = nullptr;
				if(netPlus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).size() ==1)
				{
					nta = *netPlus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).begin();
				}
				else
				{
					nta = *netMinus.findConnectedStructures(StructRec::StructurePinType("MosfetNormalArray", "Drain")).begin();
				}
				assert(nta != nullptr, "Something is wrong with the if case!");

				const StructRec::StructureNet & sourceNet = nta->findNet(StructRec::StructurePinType("MosfetNormalArray", "Source"));

				if(!getResult().structureAlreadyClassified(*nta) && (hasFirstStageOutputConnection(sourceNet, circuits) || hasSecondStageOutputConnection(sourceNet, circuits)))
				{
					CapacitancePart * capPart = new CapacitancePart(getIdCapacitancePart());
					capPart->setType("compensation");
					capPart->addMainStructure(capacitor, getResult());
					getResult().addCapacitancePart(*capPart);

					ResistorPart * resPart = new ResistorPart(getIdResistorPart());
					resPart->setType("compensation");
					resPart->setCompensationCapacitor(*capPart);
					resPart->addMainStructure(*nta, getResult());
					getResult().addResistorPart(*resPart);
				}
			}

		}
	}

	void Partitioning::createUndefinedParts(const StructRec::StructureCircuits & circuits)
	{
		std::vector<const StructRec::Structure*> structures = circuits.findAllStructures();
		for(auto& it : structures)
		{
			const StructRec::Structure & structure = *it;
			if(!getResult().structureAlreadyClassified(structure))
			{
				UndefinedPart* part = new UndefinedPart(getIdUndefinedPart());
				part->addMainStructure(structure, getResult());
				getResult().addUndefinedPart(*part);
			}
		}

	}


	void Partitioning::initializeInverterStage(std::vector<const StructRec::Structure *> transStructs, const StructRec::Structure & biasStruc, std::string type)
	{

		bool transStructsNotClassified = true;
		for(auto & it_transStructs: transStructs)
		{
			if(getResult().structureAlreadyClassified(*it_transStructs))
			{
				 transStructsNotClassified= false;
			}
		}
		if(transStructsNotClassified)
		{
			TransconductancePart* transPart = new TransconductancePart(getIdTransPart());
			for(auto & it_transStructs : transStructs)
			{
				transPart->addMainStructure(*it_transStructs, getResult());
			}
			transPart->setType(type);
			transPart->setHelperStructure(biasStruc);
			getResult().addTransconductancePart(*transPart);
			if(oneParentIsCurrentMirror(biasStruc))
			{
				const StructRec::Structure * currentMirror = &getCurrentMirrorParent(biasStruc);
				const StructRec::Pair * currentMirrorPair =static_cast<const StructRec::Pair*>(currentMirror);
				if(!getResult().structureAlreadyClassified(biasStruc))
				{

					BiasPart * biasPartCurrent = new BiasPart(getIdBiasPart());
					biasPartCurrent->addMainStructure(biasStruc, getResult());
					biasPartCurrent->addBiasedPart(*transPart);
					biasPartCurrent->setType("currentBias");
					transPart->addBiasPart(*biasPartCurrent);
					getResult().addBiasPart(*biasPartCurrent);

					if(!getResult().structureAlreadyClassified(currentMirrorPair->getChild1()))
					{
						BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
						biasPartVoltage->addMainStructure(currentMirrorPair->getChild1(), getResult());
						biasPartVoltage->addBiasedPart(*biasPartCurrent);
						biasPartVoltage->setType("voltageBias");
						getResult().addBiasPart(*biasPartVoltage);
					}
					else
					{
						Part * part = & getResult().getPart(currentMirrorPair->getChild1());
						if(part->isBiasPart())
						{
							BiasPart * biasPart = static_cast<BiasPart*>(part);
							if(!biasPart->hasBiasedPart(*biasPartCurrent))
							{
								biasPart->addBiasedPart(*biasPartCurrent);
							}
						}
					}
				}
				else
				{
					Part * part = & getResult().getPart(biasStruc);
					if(part->isBiasPart())
					{
						BiasPart * biasPart = static_cast<BiasPart*>(part);
						if(!biasPart->hasBiasedPart(*transPart))
						{
							biasPart->addBiasedPart(*transPart);
							transPart->addBiasPart(*biasPart);
						}
					}
				}
			}
			else if(biasStruc.isPair())
			{
				std::vector<const StructRec::Structure*> arrayChildren =  biasStruc.getArrayChildren();



				if(!getResult().structureAlreadyClassified(biasStruc))
				{
					BiasPart * biasPartCurrent = new BiasPart(getIdBiasPart());
					biasPartCurrent->addMainStructure(biasStruc, getResult());
					biasPartCurrent->addBiasedPart(*transPart);
					biasPartCurrent->setType("currentBias");
					transPart->addBiasPart(*biasPartCurrent);
					getResult().addBiasPart(*biasPartCurrent);

					for(auto & arrayChild : arrayChildren)
					{

						if(oneParentIsCurrentMirror(*arrayChild))
						{
							const StructRec::Structure * currentMirror = &getCurrentMirrorParent(*arrayChild);
							const StructRec::Pair * currentMirrorPair =static_cast<const StructRec::Pair*>(currentMirror);

							if(!getResult().structureAlreadyClassified(currentMirrorPair->getChild1()))
							{
								BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
								biasPartVoltage->addMainStructure(currentMirrorPair->getChild1(), getResult());
								biasPartVoltage->addBiasedPart(*biasPartCurrent);
								biasPartVoltage->setType("voltageBias");
								getResult().addBiasPart(*biasPartVoltage);
							}
							else
							{
								Part * part = & getResult().getPart(currentMirrorPair->getChild1());
								if(part->isBiasPart())
								{
									BiasPart * biasPart = static_cast<BiasPart*>(part);
									if(!biasPart->hasBiasedPart(*biasPartCurrent))
									{
										biasPart->addBiasedPart(*biasPartCurrent);
									}
								}
							}
						}
					}
				}
				else
				{
					Part * part = & getResult().getPart(biasStruc);
					if(part->isBiasPart())
					{
						BiasPart * biasPart = static_cast<BiasPart*>(part);
						if(!biasPart->hasBiasedPart(*transPart))
						{
							biasPart->addBiasedPart(*transPart);
							transPart->addBiasPart(*biasPart);
						}
					}
				}
			}
			else if(onlyOneTransistorConnectedToNet((**biasStruc.getArrayChildren().begin()).findNet(StructRec::StructurePinType("MosfetNormalArray","Gate"))))
				//Cecking that this is a circuit without a bias for stage bias of second stage. CAREFUL By Symmerical OP Amps should never get here otherwise something is wrong..
			{
				if(!getResult().structureAlreadyClassified(biasStruc))
				{
					BiasPart * biasPart = new BiasPart(getIdBiasPart());
					biasPart->addMainStructure(biasStruc, getResult());
					biasPart->addBiasedPart(*transPart);
					biasPart->setType("currentBias");
					transPart->addBiasPart(*biasPart);
					getResult().addBiasPart(*biasPart);
				}
			}
		}
	}

	void Partitioning::initializeBiasPartOfDifferentialPair(const StructRec::StructureCircuits & circuits, const StructRec::Structure & diffPair)
	{
		const StructRec::StructurePinType sourcePin = StructRec::StructurePinType("MosfetDifferentialPair", "Source");
		Core::NetId netIdSourcePin = diffPair.findNet(sourcePin).getIdentifier();

		const StructRec::StructureCircuit & arrayCircuit = circuits.findStructureCircuit(0);

		const StructRec::StructureNet & sourceNet = arrayCircuit.findStructureNet(netIdSourcePin);

		BiasPart * biasPart = new BiasPart(getIdBiasPart());

		findWithDrainConnectedDevices(sourceNet, diffPair, biasPart, nullptr);

		if(biasPart->hasMainStructures())
		{

			biasPart->addBiasedPart(getResult().getTransconductancePart(diffPair));
			biasPart->setType("currentBias");
			getResult().getTransconductancePart(diffPair).addBiasPart(*biasPart);
			getResult().addBiasPart(*biasPart);
		}
		else
		{
			delete biasPart;
		}


	}

	void Partitioning::initializeLoadPartsOfDifferentialPair(const StructRec::StructureCircuits & circuits, const StructRec::Structure & dP)
	{
		std::vector< const StructRec::Structure *> dpArrays = dP.getArrayChildren();

		const StructRec::Structure & dpArray1 = **dpArrays.begin();
		const StructRec::Structure & dpArray2 = **std::next(dpArrays.begin());

		//Finding load Parrts only of array level:
		const StructRec::StructureNet & drainNet1 = dpArray1.findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		const StructRec::StructureNet & drainNet2 = dpArray2.findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));

		LoadPart * loadPartSameTechType = new LoadPart;
		LoadPart * loadPartOppositeTechType = new LoadPart;

		findWithDrainConnectedLoadStructures(drainNet1, dpArray1, *loadPartSameTechType, loadPartOppositeTechType);
		findWithDrainConnectedLoadStructures(drainNet2, dpArray2, *loadPartSameTechType, loadPartOppositeTechType);

		if(dP.getStructureName().toStr() == "MosfetDifferentialPair" && dP.hasParent() && !getResult().getTransconductancePart(dP).isComplementary())
		{
			const StructRec::Structure & cascodedPair = getResult().getTransconductancePart(dP).getHelperStructure();

			const StructRec::Structure & cascodedPairArray1 = **cascodedPair.getArrayChildren().begin();
			const StructRec::Structure & cascodedPairArray2 = **std::next(cascodedPair.getArrayChildren().begin());


			LoadPart * loadPartWithCascodedPair;
			if(cascodedPair.getTechType() == dP.getTechType())
			{
				loadPartWithCascodedPair = loadPartSameTechType;
			}
			else
			{
				loadPartWithCascodedPair = loadPartOppositeTechType;
			}

			loadPartWithCascodedPair->setCascodedPair(cascodedPair);
			if(!loadPartWithCascodedPair->hasAsMainStructure(cascodedPairArray1))
			{
				loadPartWithCascodedPair->addMainStructure(cascodedPairArray1, getResult());
			}
			if(!loadPartWithCascodedPair->hasAsMainStructure(cascodedPairArray2))
			{
				loadPartWithCascodedPair->addMainStructure(cascodedPairArray2, getResult());
			}

			if(dP.getParent().getStructureName().toStr() == "MosfetFoldedCascodeDifferentialPair")
			{
				for(auto& it_mainStruc : loadPartWithCascodedPair->getMainStructures())
				{

					const StructRec::Structure & mainStruc = * it_mainStruc;
					if(mainStruc != cascodedPairArray1 && mainStruc != cascodedPairArray2)
					{
						loadPartWithCascodedPair->addCurrentBiasOfFoldedPair(mainStruc);
					}

				}
			}

			initializeLoadPartsOfDifferentialPair(circuits, cascodedPair);
		}
		else if(getResult().structureAlreadyClassified(dP)
				&& getResult().getPart(dP).isTransconductancePart() && getResult().getTransconductancePart(dP).isComplementary())
		{
			findStructureBetweenDifferentialPairAndOutputFirstStage(dP, *loadPartOppositeTechType);
		}

		if(loadPartSameTechType->hasMainStructures())
		{
			loadPartSameTechType->createPartId(getIdLoadPart());
			if(dP.hasParent())
			{
				getResult().getTransconductancePart(dP.getParent()).addLoadPart(*loadPartSameTechType);
			}
			else
			{
				getResult().getTransconductancePart(dP).addLoadPart(*loadPartSameTechType);
			}
			getResult().addLoadPart(*loadPartSameTechType);
			findBiasOfLoadPart( *loadPartSameTechType);
		}
		else
		{
			delete loadPartSameTechType;
		}

		if(loadPartOppositeTechType->hasMainStructures())
		{
			loadPartOppositeTechType->createPartId(getIdLoadPart());
			if(dP.hasParent())
			{
				getResult().getTransconductancePart(dP.getParent()).addLoadPart(*loadPartOppositeTechType);
			}
			else
			{
				getResult().getTransconductancePart(dP).addLoadPart(*loadPartOppositeTechType);
			}
			getResult().addLoadPart(*loadPartOppositeTechType);
			findBiasOfLoadPart( *loadPartOppositeTechType);
		}
		else
		{
			delete loadPartOppositeTechType;
		}

	}


	void Partitioning::findBiasOfLoadPart(LoadPart& loadPart)
	{
		for(auto & mainStruct : loadPart.getMainStructures())
		{
			if(mainStruct->getStructureName().toStr() == "MosfetNormalArray")
			{
				const StructRec::StructureNet & gateArrayNet = mainStruct->findNet(StructRec::StructurePinType(mainStruct->getStructureName(), "Gate")).findArrayNet();
				std::vector<const StructRec::Structure*> gateConnectedArrays = gateArrayNet.getAllConnectedStructures();
				for(auto& gateConnectedArray : gateConnectedArrays)
				{
					if(gateConnectedArray != mainStruct && gateConnectedArray->getTechType() == mainStruct->getTechType())
					{
						if(gateConnectedArray->hasParent())
						{
							for(auto & parent : gateConnectedArray->getParents() )
							{
								if(isVoltageBias(*gateConnectedArray)
										&& gateConnectedArray->findNet(StructRec::StructurePinType(gateConnectedArray->getStructureName(), "Source")) != gateArrayNet)
								{
									if(getResult().structureAlreadyClassified(*gateConnectedArray))
									{
										if(getResult().getPart(*gateConnectedArray).isBiasPart())
										{
											BiasPart & voltageBias =  getResult().getBiasPart(*gateConnectedArray);
											if(!loadPart.hasBiasPart(voltageBias.getPartId()))
											{
												loadPart.addBiasPart(voltageBias);
											}
											if(!voltageBias.hasBiasedPart(loadPart))
											{
												voltageBias.addBiasedPart(loadPart);
											}
										}
										else if(getResult().getPart(*gateConnectedArray).isLoadPart())
										{
											LoadPart & voltageBias =  getResult().getLoadPart(*gateConnectedArray);

											if(loadPart.getPartId() != voltageBias.getPartId()
													&& !loadPart.hasBiasPart(voltageBias.getPartId()))
											{
												loadPart.addBiasPart(voltageBias);
											}
										}

									}
									else
									{

										BiasPart * voltageBias = new BiasPart(getIdBiasPart());
										voltageBias->setType("voltageBias");
										if(isVoltageBias(*parent))
											voltageBias->addMainStructure(*parent, getResult());
										else
											voltageBias->addMainStructure(*gateConnectedArray, getResult());
										voltageBias->addBiasedPart(loadPart);
										loadPart.addBiasPart(*voltageBias);
										getResult().addBiasPart(*voltageBias);
									}
								}

							}
						}
						if(isVoltageBias(*gateConnectedArray) && gateConnectedArray->findNet(StructRec::StructurePinType(gateConnectedArray->getStructureName(), "Drain")) == gateArrayNet)
						{
							if(getResult().structureAlreadyClassified(*gateConnectedArray))
							{
								if(getResult().getPart(*gateConnectedArray).isBiasPart())
								{
									BiasPart & voltageBias =  getResult().getBiasPart(*gateConnectedArray);
									if(!loadPart.hasBiasPart(voltageBias.getPartId()))
									{
										loadPart.addBiasPart(voltageBias);
									}
									if(!voltageBias.hasBiasedPart(loadPart))
									{
										voltageBias.addBiasedPart(loadPart);
									}
								}
							}
							else
							{
								BiasPart * voltageBias = new BiasPart(getIdBiasPart());
								voltageBias->setType("voltageBias");
								voltageBias->addMainStructure(*gateConnectedArray, getResult());
								voltageBias->addBiasedPart(loadPart);
								loadPart.addBiasPart(*voltageBias);
								getResult().addBiasPart(*voltageBias);
							}
						}
					}
				}
			}
		}
	}

	void Partitioning::setResult(Result& result)
	{
		result_=& result;
	}

	Result& Partitioning::getResult() const
	{
		assert(hasResult());
		return *result_;
	}

	bool Partitioning::hasResult() const
	{
		return result_ != NULL;
	}

	bool Partitioning::isCurrentMirror(const StructRec::Structure & structure) const
	{
	   const StructRec::StructureName& simpleCurrentMirror = StructRec::StructureName("MosfetSimpleCurrentMirror");
	   const StructRec::StructureName& cascodeCurrentMirror = StructRec::StructureName("MosfetCascodeCurrentMirror");
	   const StructRec::StructureName& improvedWilsonCurrentMirror = StructRec::StructureName("MosfetImprovedWilsonCurrentMirror");
	   const StructRec::StructureName& wideSwingCascodeCurrentMirror = StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror");
	   const StructRec::StructureName& wideSwingCurrentMirror = StructRec::StructureName("MosfetWideSwingCurrentMirror");
	   const StructRec::StructureName& fourTransistorCurrentMirror = StructRec::StructureName("MosfetFourTransistorCurrentMirror");
	   const StructRec::StructureName& wilsonCurrentMirror = StructRec::StructureName("MosfetWilsonCurrentMirror");


	        return (structure.getStructureName() == simpleCurrentMirror) ||
	                (structure.getStructureName() == cascodeCurrentMirror) ||
	                (structure.getStructureName() == improvedWilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCascodeCurrentMirror) ||
	                (structure.getStructureName() == fourTransistorCurrentMirror) ||
	                (structure.getStructureName() == wilsonCurrentMirror) ||
	                (structure.getStructureName() == wideSwingCurrentMirror);
	    }

	bool Partitioning::hasOtherCommonStructures(const std::vector<const StructRec::Structure*>& structures1,
			const std::vector<const StructRec::Structure*> & structures2, const StructRec::Structure &  oldStructure) const
	{
		bool commonStructures = false;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			for(auto& it_structures2 : structures2)
			{
				const StructRec::Structure & structure2 = * it_structures2;
				if(structure1 == structure2 && !parentHasDevicesInOtherStructure(oldStructure,structure1) && structure1.getStructureName() != oldStructure.getStructureName())
				{
					commonStructures = true;
					break;
				}
			}
		}
		return commonStructures;
	}

	const StructRec::Structure & Partitioning::getOtherCommonStructure(
			const std::vector<const StructRec::Structure*>& structures1,
			const std::vector<const StructRec::Structure*>& structures2, const StructRec::Structure & oldStructure)
	{
		const StructRec::Structure* otherStructure = NULL;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			for(auto& it_structures2 : structures2)
			{
				const StructRec::Structure & structure2 = * it_structures2;
				if(structure1 == structure2 && !parentHasDevicesInOtherStructure(oldStructure,structure1) &&
				   structure1.getStructureName() != oldStructure.getStructureName())
				{
					if(structure1.hasParent())
					{
						std::vector<const StructRec::Structure *> parents = structure1.getTopmostParents();
						for(auto & it_parents: parents)
						{
							if(isCurrentMirror(*it_parents))
							{
								otherStructure = it_parents;
								break;
							}
						}
						assert(otherStructure != NULL, "Should have current mirror as parent!");
					}
					else
					{
						otherStructure = & structure1;
					}
				}
			}
		}

		return * otherStructure;
	}


	const StructRec::Structure & Partitioning::findOtherDifferentialPair(std::vector<const StructRec::Structure *> dPVector1, std::vector<const StructRec::Structure * > dPVector2,
			const StructRec::Structure & dP)
	{
		assert(dPVector1.size() + dPVector2.size() == 2, "There should be  exactly two differential pairs in the two vectors combined!");
		const StructRec::Structure * otherDP = nullptr;

		for(auto & dPIterator : dPVector1)
		{
			if(dPIterator->getIdentifier() != dP.getIdentifier())
			{
				otherDP = dPIterator;
				break;
			}
		}
		for(auto & dPIterator : dPVector2)
		{
			if(dPIterator->getIdentifier() != dP.getIdentifier())
			{
				otherDP = dPIterator;
				break;
			}
		}
		assert(otherDP != nullptr, "Another Differential Pair should have benn found!" );
		return *otherDP;

	}


	bool Partitioning::isVoltageBias(const StructRec::Structure& structure ) const
	{
		bool isIt = false;
		const StructRec::StructureName& diodeArray = StructRec::StructureName("MosfetDiodeArray");
		const StructRec::StructureName& voltageReference1 = StructRec::StructureName("MosfetVoltageReference1");
		if(structure.getStructureName() == diodeArray || structure.getStructureName() == voltageReference1
				|| (structure.hasExactlyOneParent() && structure.getParentStructureName() == voltageReference1))
		{
			isIt = true;
		}
		else if(structure.hasParent())
		{
			const std::vector<StructRec::Structure*>  parents = structure.getParents();
			for(auto & parent  :parents)
			{
				if(isCurrentMirror(*parent))
				{
					const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(parent);
					if(parentPair->getChild1() == structure)
					{
						isIt = true;
						break;
					}
				}
				else if(isVoltageBias(*parent))
				{
					isIt = true;
					break;
				}
			}
		}
		return isIt;
	}

	bool Partitioning::isCurrentBias(const StructRec::Structure & structure) const
	{
		bool isCurrentBias = false;

		if(structure.hasParent())
		{
			std::vector<const StructRec::Structure*> parents = structure.getTopmostParents();
			for(auto& it_parents: parents)
			{
				if(isCurrentMirror(*it_parents))
				{
					const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(it_parents);

					if(parentPair->getChild2().hasCommonDevices(structure))
					{
						isCurrentBias = true;
					}
				}
			}

		}


		return isCurrentBias;
	}

	bool Partitioning::hasVoltageBiasesWithSameType(const std::vector<const StructRec::Structure*>& structures1,
			const std::vector<const StructRec::Structure*>& structures2 ) const
	{
		bool voltageBiasesWithSameType = false;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			if(isVoltageBias(structure1))
			{
				for(auto& it_structures2 : structures2)
				{
					const StructRec::Structure & structure2 = *it_structures2;
					if(isVoltageBias(structure2) && structure1.getStructureName() == structure2.getStructureName())
					{
						voltageBiasesWithSameType = true;
					}
				}
			}
		}
		return voltageBiasesWithSameType;
	}

	bool Partitioning::hasCurrentBiasesWithSameType(const std::vector<const StructRec::Structure*>& structures1,
			const std::vector<const StructRec::Structure*>& structures2) const
	{
		bool currentBiasesWithSameType = false;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			if(isCurrentBias(structure1))
			{
				for(auto& it_structures2 : structures2)
				{
					const StructRec::Structure & structure2 = *it_structures2;
					if(isCurrentBias(structure2) && structure1.getStructureName() == structure2.getStructureName())
					{
						currentBiasesWithSameType = true;
					}
				}
			}
		}
		return currentBiasesWithSameType;
	}

	std::vector<const StructRec::Structure *> Partitioning::getVoltageBiasesWithSameType(
			const std::vector<const StructRec::Structure*>& structures1, const std::vector<const StructRec::Structure*>& structures2,
			const StructRec::StructureNet  & net1, const StructRec::StructureNet & net2 )
	{
		std::vector<const StructRec::Structure*> voltageBiasesWithSameType;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			if(isVoltageBias(structure1) && hasVoltageBiasInputConnectionOnNet(structure1, net1))
			{
				for(auto& it_structures2 : structures2)
				{
					const StructRec::Structure & structure2 = *it_structures2;
					if(isVoltageBias(structure2) && hasVoltageBiasInputConnectionOnNet(structure2, net2) && structure1.getStructureName() == structure2.getStructureName())
					{
						voltageBiasesWithSameType.push_back(&structure1);
						voltageBiasesWithSameType.push_back(&structure2);
					}
				}
			}
		}
		return voltageBiasesWithSameType;
	}

	std::vector<const StructRec::Structure *> Partitioning::getCurrentBiasesWithSameType(
			const std::vector<const StructRec::Structure*>& structures1, const std::vector<const StructRec::Structure*>& structures2,
			const StructRec::StructureNet& net1, const StructRec::StructureNet& net2)
	{
		std::vector<const StructRec::Structure *>  currentBiasesWithSameType;
		for(auto& it_structures1 : structures1)
		{
			const StructRec::Structure & structure1 = * it_structures1;
			if(isCurrentBias(structure1) && hasCurrentBiasOutputConnectionOnNet(structure1, net1))
			{
				for(auto& it_structures2 : structures2)
				{
					const StructRec::Structure & structure2 = *it_structures2;
					if(isCurrentBias(structure2) && hasCurrentBiasOutputConnectionOnNet(structure2, net2)&& structure1.getStructureName() == structure2.getStructureName())
					{
						currentBiasesWithSameType.push_back(&structure1);
						currentBiasesWithSameType.push_back(&structure2);
					}
				}
			}
		}
		return currentBiasesWithSameType;
	}

	bool Partitioning::hasVoltageBiasInputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const
	{
		const StructRec::StructurePinType drain = StructRec::StructurePinType(structure.getStructureName().toStr(), "Drain");
		const StructRec::StructurePinType input = StructRec::StructurePinType(structure.getStructureName().toStr(), "Input");
		if(structure.hasPin(drain))
		{
			return structure.findPin(drain).getNet().getIdentifier() == net.getIdentifier();
		}
		else if(structure.hasPin(input))
		{
			return structure.findPin(input).getNet().getIdentifier() == net.getIdentifier();
		}
		else
		{
			assert(!isVoltageBias(structure), "Invalid Pin Name!");
			return false;
		}
	}


	bool Partitioning::hasVoltageBiasOutputConnection(const StructRec::StructureNet& net) const
	{

		const StructRec::StructureNet & arrayNet = net.findArrayNet();

		std::vector<const StructRec::Structure*> potentialVoltageBiases = arrayNet.getAllConnectedStructures();

		for(auto & struc : potentialVoltageBiases)
		{
			if(isVoltageBias(*struc))
			{
				return true;
			}
		}

		return false;
	}

	bool Partitioning::hasCurrentBiasOutputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const
	{
		const StructRec::StructurePinType drain = StructRec::StructurePinType(structure.getStructureName().toStr(), "Drain");
		const StructRec::StructurePinType output = StructRec::StructurePinType(structure.getStructureName().toStr(), "Output");
		if(structure.hasPin(drain))
		{
			return structure.findPin(drain).getNet().getIdentifier() == net.getIdentifier();
		}
		else if(structure.hasPin(output))
		{
			return structure.findPin(output).getNet().getIdentifier() == net.getIdentifier();
		}
		else
		{
			assert(!isCurrentBias(structure), "Invalid Pin Name!");
			return false;
		}
	}

	bool Partitioning::hasCurrentBiasInputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const
	{
		const StructRec::StructurePinType gate = StructRec::StructurePinType(structure.getStructureName().toStr(), "Gate");
		const StructRec::StructurePinType gate1 = StructRec::StructurePinType(structure.getStructureName().toStr(), "Gate1");

		if(structure.hasPin(gate))
		{
			return structure.findPin(gate).getNet().getIdentifier() == net.getIdentifier();
		}
		else if(structure.hasPin(gate1))
		{
			return structure.findPin(gate1).getNet().getIdentifier() == net.getIdentifier();
		}
		else
		{
			assert(!isCurrentBias(structure) || hasCurrentBiasOutputConnectionOnNet(structure, net), "Invalid Pin Name!");
			return false;
		}
	}


	bool Partitioning::hasStageOutputNetConnection(const StructRec::StructureNet& net, const Part & stage,
			const StructRec::StructureCircuits & circuits) const
	{
		bool hasConnection = false;

		if(stage.isTransconductancePart())
		{
			TransconductancePart & transconductance = getResult().getTransconductancePart(**stage.getMainStructures().begin());
			if(transconductance.isFirstStage())
			{
				hasConnection = hasFirstStageOutputConnection(net, circuits);
			}
			else if(transconductance.isPrimarySecondStage() || transconductance.isSecondarySecondStage())
			{
				hasConnection = hasSecondStageOutputConnection(net, circuits);
			}
		}
		else if(stage.isLoadPart())
		{
			LoadPart & loadPart = getResult().getLoadPart(**stage.getMainStructures().begin());
			if(getResult().getFirstStage().getLoadPart().size() ==1
				&& isSymmetricalOpAmpLoadPart(loadPart))
			{
				for(auto & connectedStructure : circuits.findConnectedStructures(net.getIdentifier()))
				{
					const StructRec::StructureName normalTransistorName = StructRec::StructureName("MosfetNormalArray");
					const StructRec::StructureName diodeTransistorName = StructRec::StructureName("MosfetDiodeArray");
					if(connectedStructure->getStructureName() == diodeTransistorName)
					{
						const StructRec::StructureNet & gateNet = connectedStructure->findNet(StructRec::StructurePinType(diodeTransistorName.toStr(), "Drain"));
						const StructRec::StructureNet & sourceNet = connectedStructure->findNet(StructRec::StructurePinType(diodeTransistorName.toStr(), "Source"));
						if(gateNet.getIdentifier() == net.getIdentifier() && sourceNet.isSupply())
						{
							hasConnection = true;
						}
					}
					else if(connectedStructure->getStructureName() == normalTransistorName)
					{
						const StructRec::StructureNet & gateNet = connectedStructure->findNet(StructRec::StructurePinType(normalTransistorName.toStr(), "Gate"));
						const StructRec::StructureNet & sourceNet = connectedStructure->findNet(StructRec::StructurePinType(normalTransistorName.toStr(), "Source"));
						if(gateNet.getIdentifier() == net.getIdentifier() && sourceNet.isSupply())
						{
							hasConnection = true;
						}
					}

				}
			}
		}


		return hasConnection;
	}

	bool Partitioning::onlyOneTransistorConnectedToNet(const StructRec::StructureNet & net) const
	{
		const StructRec::StructureNet & arrayNet = net.findArrayNet();

		return arrayNet.getAllConnectedStructures().size() == 1;
	}

	bool Partitioning::isConnectedAtDrainToCapacitance(const TransconductancePart & part) const
	{
		Core::CapacitorDeviceType capacitor;
		Core::MosfetDeviceType mosfet;

		bool isConnected = false;

		std::vector<const StructRec::Structure* > mainStructures = part.getMainStructures();

		for(auto & mainStructure : mainStructures)
		{
			std::vector<const Core::Device *> devices = mainStructure->getDevices();
			assert(devices.size() <= 2, "Transconductance should never have more than 2 devices");

			for(auto & device : devices)
			{
				if(device->findNet(mosfet.drain()).hasPin(capacitor.minus()) || device->findNet(mosfet.drain()).hasPin(capacitor.plus()))
				{
					isConnected = true;
				}
			}
			if(isConnected)
				break;
		}

		return isConnected;
	}

	bool Partitioning::isSymmetricalOpAmpLoadPart(const LoadPart & loadPart) const
	{
		std::vector<const StructRec::Structure *> mainStructures = loadPart.getMainStructures();

		if(mainStructures.size() == 2)
		{
			bool isDiodeTransistor = true;
			bool equalNames = false;
			const StructRec::Structure * refStructure = nullptr;
			for(auto mainStruct : mainStructures)
			{
				if(refStructure == nullptr)
				{
					refStructure = mainStruct;
				}
				else if(refStructure->getStructureName() == mainStruct->getStructureName())
				{
					equalNames = true;
				}
				if(!isVoltageBias(*mainStruct))
				{
					isDiodeTransistor = false;
				}
			}
			return isDiodeTransistor && equalNames;
		}
		else if(mainStructures.size() == 4)
		{
			const StructRec::Structure * dta1 = nullptr;
			const StructRec::Structure * dta2 = nullptr;
			const StructRec::Structure * nta1 = nullptr;
			const StructRec::Structure * nta2 = nullptr;
			for(auto & struc : mainStructures)
			{
				if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray")
					&& dta1 == nullptr)
				{
						dta1 = struc;
				}
				else if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray"))
				{
					dta2 = struc;
				}
				else if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetNormalArray")
					&& nta1 == nullptr)
				{
						nta1 = struc;
				}
				else if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetNormalArray"))
				{
					nta2 = struc;
				}
			}
			if(dta1 != nullptr && dta2 != nullptr && nta1 != nullptr && nta2 != nullptr)
			{
				if(dta1->hasDirectParent(StructRec::StructureName("MosfetSimpleCurrentMirror"))
						&& dta2->hasDirectParent(StructRec::StructureName("MosfetSimpleCurrentMirror"))
						&& nta1->hasDirectParent(StructRec::StructureName("MosfetCrossCoupledPair"))
						&& nta2->hasDirectParent(StructRec::StructureName("MosfetCrossCoupledPair")))
				{
					return true;
				}
			}

			const StructRec::Structure * voltageBias1 = nullptr;
			const StructRec::Structure * voltageBias2 = nullptr;
			bool thirdStructureInCircuit = false;
			for(auto & mainStruct : mainStructures)
			{
				if(mainStruct->hasExactlyOneParent())
				{
					const StructRec::Structure & parent = mainStruct->getParent();

					if(voltageBias1 == nullptr)
					{
						voltageBias1 = &parent;
					}
					else if(voltageBias1->getIdentifier() != parent.getIdentifier() && voltageBias2 == nullptr)
					{
						voltageBias2 = &parent;
					}
					else if(parent.getIdentifier() != voltageBias1->getIdentifier() && parent.getIdentifier() != voltageBias2->getIdentifier())
					{
						thirdStructureInCircuit = true;
					}
				}
				else
				{
					return false;
				}
			}

			if(!thirdStructureInCircuit && isVoltageBias(*voltageBias1) && isVoltageBias(*voltageBias2)
					&& voltageBias1->getStructureName() == voltageBias2->getStructureName())
				return true;
			else
			{
				return false;
			}
		}

		return false;

	}

	bool Partitioning::hasThirdStageInputConnection(const TransconductancePart & part) const
	{
		assert(!part.isThirdStage(), "The part should not be a third stage");

		Core::MosfetDeviceType mosfet;

		for(auto & mainStructure : part.getMainStructures())
		{
			for(auto & device : mainStructure->getDevices())
			{
				const Core::Net & drainNet = device->findNet(mosfet.drain());

				if(drainNet.hasPin(mosfet.gate()))
				{
					std::vector<const Core::Pin *> gatePins = drainNet.getPins(mosfet.gate());
					for(auto & pin : gatePins)
					{
						const Core::Device & gateDevice = pin->getDevice();
						if(getResult().deviceAlreadyClassified(gateDevice))
						{
							const Part & part = getResult().getPart(gateDevice);
							if(part.isTransconductancePart())
							{
								const TransconductancePart & transPart = getResult().getTransconductancePart(**part.getMainStructures().begin());
								if(transPart.isThirdStage())
									return true;
							}
						}
					}
				}

			}
		}
		return false;
	}

	bool Partitioning::hasThirdStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits &circuits) const
	{
		bool hasConnection = false;
		const StructRec::StructurePinType output1 = StructRec::StructurePinType("MosfetAnalogInverter", "Output");
		const StructRec::StructurePinType gate1_1 = StructRec::StructurePinType("MosfetAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate1_2 = StructRec::StructurePinType("MosfetAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName1 = StructRec::StructureName("MosfetAnalogInverter");
		const StructRec::StructurePinType output2 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "Output");
		const StructRec::StructurePinType gate2_1 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate2_2 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName2 = StructRec::StructureName("MosfetCascodedAnalogInverter");
		const StructRec::StructurePinType output3 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "Output");
		const StructRec::StructurePinType gate3_1 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate3_2 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName3 = StructRec::StructureName("MosfetCascodedPMOSAnalogInverter");
		const StructRec::StructurePinType output4 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "Output");
		const StructRec::StructurePinType gate4_1 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate4_2 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName4 = StructRec::StructureName("MosfetCascodedNMOSAnalogInverter");

		std::vector<const StructRec::Structure*> connectedStructures = circuits.findConnectedStructures(net.getIdentifier());
		for(auto& it : connectedStructures)
		{
			const StructRec::Structure & connectedStructure = * it;
			if((connectedStructure.getStructureName() == inverterName1 && connectedStructure.findNet(output1).getIdentifier() == net.getIdentifier()
					&& (hasSecondStageOutputConnection(connectedStructure.findNet(gate1_1), circuits) ||hasSecondStageOutputConnection(connectedStructure.findNet(gate1_2), circuits )) )
					|| (connectedStructure.getStructureName() == inverterName2 && connectedStructure.findNet(output2).getIdentifier() == net.getIdentifier()
					&& (hasSecondStageOutputConnection(connectedStructure.findNet(gate2_1), circuits) ||hasSecondStageOutputConnection(connectedStructure.findNet(gate2_2), circuits )) )
					|| (connectedStructure.getStructureName() == inverterName3 && connectedStructure.findNet(output3).getIdentifier() == net.getIdentifier()
					&& (hasSecondStageOutputConnection(connectedStructure.findNet(gate3_1), circuits) ||hasSecondStageOutputConnection(connectedStructure.findNet(gate3_2), circuits )) )
					|| (connectedStructure.getStructureName() == inverterName4 && connectedStructure.findNet(output4).getIdentifier() == net.getIdentifier()
					&& (hasSecondStageOutputConnection(connectedStructure.findNet(gate4_1), circuits) ||hasSecondStageOutputConnection(connectedStructure.findNet(gate4_2), circuits )) ))
			{
				hasConnection = true;
				break;
			}
		}
		return hasConnection;

	}

	bool Partitioning::hasSecondStageInputConnection(const StructRec::StructureNet & net, const StructRec::StructureCircuits & circuits) const
	{
		bool hasConnection = false;
		const StructRec::StructurePinType gate1_1 = StructRec::StructurePinType("MosfetAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate1_2 = StructRec::StructurePinType("MosfetAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName1 = StructRec::StructureName("MosfetAnalogInverter");
		const StructRec::StructurePinType gate2_1 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate2_2 = StructRec::StructurePinType("MosfetCascodedAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName2 = StructRec::StructureName("MosfetCascodedAnalogInverter");
		const StructRec::StructurePinType gate3_1 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate3_2 = StructRec::StructurePinType("MosfetCascodedPMOSAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName3 = StructRec::StructureName("MosfetCascodedPMOSAnalogInverter");
		const StructRec::StructurePinType gate4_1 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputPMOS1");
		const StructRec::StructurePinType gate4_2 = StructRec::StructurePinType("MosfetCascodedNMOSAnalogInverter", "InputNMOS1");
		const StructRec::StructureName inverterName4 = StructRec::StructureName("MosfetCascodedNMOSAnalogInverter");

		std::vector<const StructRec::Structure*> connectedStructures = circuits.findConnectedStructures(net.getIdentifier());
		for(auto & connectedStruc : connectedStructures)
		{
			if((!getResult().structureAlreadyClassified(*connectedStruc) || !getResult().isLoadPartOfFirstStage(getResult().getPart(*connectedStruc))) &&
					( (connectedStruc->getStructureName() == inverterName1 && (connectedStruc->findNet(gate1_1).getIdentifier() == net.getIdentifier()
							|| connectedStruc->findNet(gate1_2).getIdentifier() == net.getIdentifier()) )
					||	(connectedStruc->getStructureName() == inverterName2 && (connectedStruc->findNet(gate2_1).getIdentifier() == net.getIdentifier()
							|| connectedStruc->findNet(gate2_2).getIdentifier() == net.getIdentifier()) )
					||	(connectedStruc->getStructureName() == inverterName3 && (connectedStruc->findNet(gate3_1).getIdentifier() == net.getIdentifier()
							|| connectedStruc->findNet(gate3_2).getIdentifier() == net.getIdentifier()) )
					||	(connectedStruc->getStructureName() == inverterName4 && (connectedStruc->findNet(gate4_1).getIdentifier() == net.getIdentifier()
									|| connectedStruc->findNet(gate4_2).getIdentifier() == net.getIdentifier()) ) 		)
					)
			{
				hasConnection = true;
				break;
			}
		}
		return hasConnection;
	}

	bool Partitioning::isInverterName(const StructRec::StructureName structureName) const
	{
		std::vector<StructRec::StructureName> inverterNames;
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedPMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedNMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads"));

		bool isIt = false;
		for(auto & inverterName : inverterNames)
		{
			if(structureName == inverterName)
			{
				isIt = true;
				break;
			}
		}

		return isIt;
	}

	bool Partitioning::isConnectedToInverterOutput(const StructRec::StructureNet & net, const StructRec::Structure & inverter) const
	{
		assert(isInverterName(inverter.getStructureName()), "Structure must be a inverter");
		std::vector<StructRec::StructureName> inverterNames;
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedPMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedNMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads"));

		bool isConnected = false;
		for(auto & inverterName : inverterNames)
		{
			const StructRec::StructurePinType output = StructRec::StructurePinType(inverterName.toStr(), "Output");
			const StructRec::StructurePinType inner1 = StructRec::StructurePinType(inverterName.toStr(), "Inner1");
			const StructRec::StructurePinType inner2= StructRec::StructurePinType(inverterName.toStr(), "Inner2");
			const StructRec::StructurePinType inner = StructRec::StructurePinType(inverterName.toStr(), "Inner");
			if(inverter.hasPin(output))
			{
				if(inverter.findNet(output).getIdentifier() == net.getIdentifier())
				{
					isConnected = true;
					break;
				}
			}
			if(inverter.hasPin(inner1))
			{
				if(inverter.findNet(inner1).getIdentifier() == net.getIdentifier())
				{
					isConnected = true;
					break;
				}
			}
			if(inverter.hasPin(inner2))
			{
				if(inverter.findNet(inner2).getIdentifier() == net.getIdentifier())
				{
					isConnected = true;
					break;
				}
			}
			if(inverter.hasPin(inner))
			{
				if(inverter.findNet(inner).getIdentifier() == net.getIdentifier())
				{
					isConnected = true;
					break;
				}
			}

		}
		return isConnected;
	}

	bool Partitioning::inputOfInverterIsConnectedToFirstStageOutput(const StructRec::Structure & inverter, const StructRec::StructureCircuits &circuits) const
	{
		assert(isInverterName(inverter.getStructureName()), "Structure must be a inverter");
		std::vector<StructRec::StructureName> inverterNames;
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedPMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodedNMOSAnalogInverter"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterOneDiodeTransistor"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodePMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeNMOSAnalogInverterCurrentMirrorLoad"));
		inverterNames.push_back(StructRec::StructureName("MosfetCascodeAnalogInverterTwoCurrentMirrorLoads"));

		bool isConnected = false;
		for(auto & inverterName : inverterNames)
		{
			const StructRec::StructurePinType inputPmos = StructRec::StructurePinType(inverterName.toStr(), "InputPMOS1");
			const StructRec::StructurePinType inputNmos = StructRec::StructurePinType(inverterName.toStr(), "InputNMOS1");

			if(inverter.hasPin(inputNmos))
			{
				if(hasFirstStageOutputConnection(inverter.findNet(inputNmos), circuits))
				{
					isConnected = true;
					break;
				}
			}
			if(inverter.hasPin(inputPmos))
			{
				if(hasFirstStageOutputConnection(inverter.findNet(inputPmos), circuits))
				{
					isConnected = true;
					break;
				}
			}


		}
		return isConnected;
	}


	bool Partitioning::hasSecondStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits &circuits) const
	{
		bool hasConnection = false;

		std::vector<const StructRec::Structure*> connectedStructures = circuits.findConnectedStructures(net.getIdentifier());
		for(auto& it : connectedStructures)
		{
			const StructRec::Structure & connectedStructure = * it;

			if((getResult().structureAlreadyClassified(connectedStructure) && !getResult().isLoadPartOfFirstStage(getResult().getPart(connectedStructure))))
			{
				if(isInverterName(connectedStructure.getStructureName()))
				{
					if(isConnectedToInverterOutput(net, connectedStructure) && inputOfInverterIsConnectedToFirstStageOutput(connectedStructure, circuits))
					{
						hasConnection = true;
						break;
					}
				}
			}
		}
		return hasConnection;

	}

	bool Partitioning::hasFirstStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits & circuits) const
	{
		bool hasConnection = false;

		if(getResult().getFirstStage().getLoadPart().size() == 1 && isSymmetricalOpAmpLoadPart(**getResult().getFirstStage().getLoadPart().begin()))
		{
			LoadPart & loadPart = **getResult().getFirstStage().getLoadPart().begin();
			if(hasVoltageBiasOutputConnection(net))
			{
				for(auto & mainStruct : loadPart.getMainStructures())
				{
					if(isVoltageBias(*mainStruct) && hasVoltageBiasInputConnectionOnNet(*mainStruct, net))
					{
						hasConnection = true;
					}
				}
			}
			return hasConnection;
		}
		else if(!getResult().getFirstStage().isComplementary())
		{
			const StructRec::StructureName diffPairName = StructRec::StructureName("MosfetDifferentialPair");
			const StructRec::StructureName gateConnectedCoupleName = StructRec::StructureName("MosfetGateConnectedCouple");

			std::vector<const StructRec::Structure*> connectedStructures = circuits.findConnectedStructures(net.getIdentifier());
			for(auto& it : connectedStructures)
			{
				const StructRec::Structure & connectedStructure = * it;
				if(connectedStructure.getStructureName() == diffPairName || connectedStructure.getStructureName() == gateConnectedCoupleName)
				{

					const StructRec::StructurePinType output1 = StructRec::StructurePinType(connectedStructure.getStructureName().toStr(), "Output1");
					const StructRec::StructurePinType output2 = StructRec::StructurePinType(connectedStructure.getStructureName().toStr(), "Output2");

					if(connectedStructure.findNet(output1).getIdentifier() == net.getIdentifier() || connectedStructure.findNet(output2).getIdentifier() == net.getIdentifier())
					{
						hasConnection = true;
						break;
					}

				}
			}
			return hasConnection;
		}
		else
		{
			std::vector<LoadPart*> loadParts = getResult().getFirstStage().getLoadPart();
			assert(loadParts.size() >= 2, "Complementary First Stage should always have to load parts");

			LoadPart & loadPart = **loadParts.begin();

			std::vector<Component *> outputComps;


			for(auto & comp : getResult().getBelongingComponents(loadPart))
			{
				if(!comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(),"Source")).isSupply())
				{
					outputComps.push_back(comp);
				}
			}

			assert(outputComps.size() == 2, "Every load of a complementary first stage should have two components NOT connected to supply");

			for(auto & outputComp : outputComps)
			{
				if(outputComp->getArray().findNet(StructRec::StructurePinType(outputComp->getArray().getStructureName(), "Drain")).getIdentifier()
						== net.getIdentifier() && !hasConnection)
				{
					hasConnection = true;
				}
				else if(outputComp->getArray().findNet(StructRec::StructurePinType(outputComp->getArray().getStructureName(), "Drain")).getIdentifier()
						== net.getIdentifier() && hasConnection)
				{
					assert(false, "Something went wrong! Only one transistor should be connected to the output net of the first stage");
				}
			}

			return hasConnection;
		}
	}


	bool Partitioning::hasGroundConnection(const StructRec::StructureNet & net ) const
	{
		return net.isSupply() && net.getSupply().isGnd();
	}

	Part & Partitioning::initializeVoltageBiasToCurrentBias(const StructRec::Structure & currentBias)
	{
		std::vector<const StructRec::Structure*> parents = currentBias.getTopmostParents();
		const StructRec::Structure * currentMirror = nullptr;
		for(auto& it_parents: parents)
		{
			if(isCurrentMirror(*it_parents))
			{
				currentMirror =it_parents;
				break;
			}
		}
		assert(currentMirror != nullptr, "Supposed current bias does not have a current mirror as parent!");

		const StructRec::Pair * parentPair =static_cast<const StructRec::Pair*>(currentMirror);

		if(!getResult().structureAlreadyClassified(parentPair->getChild1()))
		{
			BiasPart * biasPartVoltage = new BiasPart(getIdBiasPart());
			biasPartVoltage->addMainStructure(parentPair->getChild1(), getResult());
			biasPartVoltage->addBiasedPart(getResult().getPart(currentBias));
			biasPartVoltage->setType("voltageBias");
			getResult().addBiasPart(*biasPartVoltage);
			return *biasPartVoltage;
		}
		else
		{
			Part * part = & getResult().getPart(parentPair->getChild1());
			if(part->isBiasPart())
			{
				BiasPart * biasPart = static_cast<BiasPart*>(part);
				if(!biasPart->hasBiasedPart(getResult().getPart(currentBias)))
				{
					biasPart->addBiasedPart(getResult().getPart(currentBias));
				}
			}
			return *part;
		}

	}

	int & Partitioning::getIdTransPart()
	{
		return idTransPart_;
	}

	int & Partitioning::getIdCapacitancePart()
	{
		return idCapacitancePart_;
	}

	int & Partitioning::getIdUndefinedPart()
	{
		return idUndefinedPart_;
	}

	int & Partitioning::getIdCommonModeSignalDetectorPart()
	{
		return idCommonModeSignalDetectorPart_;
	}

	int & Partitioning::getIdResistorPart()
	{
		return idResistorPart_;
	}

	int & Partitioning::getIdPositiveFeedbackPart()
	{
		return idPositiveFeedbackPart_;
	}

	int & Partitioning::getIdLoadPart()
	{
		return idLoadPart_;
	}

	int & Partitioning::getIdBiasPart()
	{
		return idBiasPart_;
	}

	bool Partitioning::parentHasDevicesInOtherStructure(const StructRec::Structure & structure,const StructRec::Structure & other ) const
	{
		bool deviceInOtherStructure = false;
		if(structure.hasParent() && structure.getParent().hasCommonDevices(other))
		{
			deviceInOtherStructure = true;
		}

		return deviceInOtherStructure;
	}

	bool Partitioning::oneParentIsCurrentMirror(const StructRec::Structure & structure ) const
	{
		bool parentIsCurrentMirror = false;
		const std::vector<StructRec::Structure*> parents = structure.getParents();

		for(std::vector<StructRec::Structure*>::const_iterator it_parents = parents.begin(); it_parents != parents.end(); it_parents++ )
		{
			StructRec::Structure * parent = * it_parents;
			if(isCurrentMirror(*parent))
			{
				parentIsCurrentMirror = true;
				break;
			}
		}
		return parentIsCurrentMirror;
	}

	const StructRec::Structure & Partitioning::getCurrentMirrorParent(const StructRec::Structure & structure)
	{
		StructRec::Structure * currentMirror = NULL;
		const std::vector<StructRec::Structure*> parents = structure.getParents();

		for(std::vector<StructRec::Structure*>::const_iterator it_parents = parents.begin(); it_parents != parents.end(); it_parents++ )
		{
			StructRec::Structure * parent = * it_parents;
			if(isCurrentMirror(*parent))
			{
				currentMirror = parent;
				break;
			}
		}
		assert(currentMirror != NULL , "Has no current mirror as parent.");
		return * currentMirror;
	}

	const StructRec::Structure & Partitioning::getRecursiveCurrentMirrorParent(const StructRec::Structure & structure)
	{
		assert(structure.isPartOfCurrentMirror(), "Structure must be part of current mirror");
		const StructRec::Structure * currentMirror = NULL;
		const std::vector<StructRec::Structure*> parents = structure.getParents();

		for(std::vector<StructRec::Structure*>::const_iterator it_parents = parents.begin(); it_parents != parents.end(); it_parents++ )
		{
				StructRec::Structure * parent = * it_parents;
				if(isCurrentMirror(*parent))
				{
					currentMirror = parent;
					break;
				}
				else if(parent->isPartOfCurrentMirror())
				{
					currentMirror = &getRecursiveCurrentMirrorParent(*parent);
				}
			}
			assert(currentMirror != NULL , "Has no current mirror as parent.");
			return * currentMirror;
	}

	void Partitioning::addBiasedPartTypeVoltageBias( const StructRec::Pair & currentMirror, const StructRec::StructureCircuits & circuits)
	{
		StructRec::StructurePinType output = StructRec::StructurePinType(currentMirror.getStructureName().toStr(),"Output");
		const StructRec::StructureNet& netOutput = currentMirror.findNet(output);
		std::vector<const StructRec::Structure*> posVoltageBias = circuits.findConnectedStructures(netOutput.getIdentifier());

		for(auto& it_posVoltageBias : posVoltageBias)
		{
			const StructRec::Structure& posVoltBias = *it_posVoltageBias;
			if(posVoltBias.getTechType() != currentMirror.getTechType())
			{
				if((hasVoltageBiasInputConnectionOnNet(posVoltBias, netOutput) && getResult().isClassifiedVoltageBias(posVoltBias)) && !getResult().getBiasPart(currentMirror.getChild2()).hasBiasedPart(getResult().getPart(posVoltBias)))
				{
					getResult().getBiasPart(currentMirror.getChild2()).addBiasedPart(getResult().getPart(posVoltBias));
				}
			}

		}
	}

	void Partitioning::addBiasedPartToCurrentBias(const StructRec::Pair & currentMirror, const StructRec::StructureCircuits  & circuits )
	{
		StructRec::StructurePinType input = StructRec::StructurePinType(currentMirror.getStructureName().toStr(),"Input");
		const StructRec::StructureNet& netInput = currentMirror.findNet(input);
		std::vector<const StructRec::Structure*> posCurrentBias = circuits.findConnectedStructures(netInput.getIdentifier());

		for(auto& it_posCurrentBias : posCurrentBias)
		{
			const StructRec::Structure & posCurrBias = *it_posCurrentBias;
			if(posCurrBias.getTechType() != currentMirror.getTechType())
			{
				if(hasCurrentBiasOutputConnectionOnNet(posCurrBias, netInput) && getResult().isClassifiedCurrentBias(posCurrBias) && !getResult().getBiasPart(posCurrBias).hasBiasedPart(getResult().getPart(currentMirror.getChild1())) )
				{
					getResult().getBiasPart(posCurrBias).addBiasedPart(getResult().getPart(currentMirror.getChild1()));
				}
			}

		}
	}

	void Partitioning::findWithDrainConnectedLoadStructures(const StructRec::StructureNet & net, const StructRec::Structure & structure,
			LoadPart & loadPartSameTechType, LoadPart * loadPartOppositeTechType)
	{

		std::vector<const StructRec::Structure *> connectedStructures  = net.getAllConnectedStructures();
		for(auto & connectedStructure : connectedStructures)
		{
			if(!getResult().structureAlreadyClassified(*connectedStructure) &&
					*connectedStructure != structure && connectedStructure->hasPin(StructRec::StructurePinType(connectedStructure->getStructureName(), "Drain"))
					&& connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Drain"))  == net  )
			{


				 const StructRec::StructureNet & sourceNet =connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Source"));
				 if(connectedStructure->getTechType() == structure.getTechType())
				 {
					 loadPartSameTechType.addMainStructure( * connectedStructure, getResult());

				 	 if(!sourceNet.isSupply())
				 	 {
				 		 findWithDrainConnectedLoadStructures(sourceNet, *connectedStructure, loadPartSameTechType);
				 	 }
				 }
				 else if(loadPartOppositeTechType != nullptr)
				 {
					 loadPartOppositeTechType->addMainStructure( * connectedStructure, getResult());

					 if(!sourceNet.isSupply())
					 {
						 findWithDrainConnectedLoadStructures(sourceNet, *connectedStructure, *loadPartOppositeTechType);
					 }
				 }

			}
		}

	}


	void Partitioning::findWithDrainConnectedDevices(const StructRec::StructureNet & net, const StructRec::Structure & refStruct,
			Part * partSameTechType, Part * partOppositeTechType)
	{
		std::vector<const StructRec::Structure *> connectedStructures  = net.getAllConnectedStructures();
		for(auto & connectedStructure : connectedStructures)
		{
			if(!getResult().structureAlreadyClassified(*connectedStructure) &&
					*connectedStructure != refStruct && connectedStructure->hasPin(StructRec::StructurePinType(connectedStructure->getStructureName(), "Drain"))
					&& connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Drain"))  == net  )
			{

				 const StructRec::StructureNet & sourceNet =connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Source"));
				 if( partSameTechType != nullptr &&  connectedStructure->getTechType() == refStruct.getTechType())
				 {
					 partSameTechType->addMainStructure( * connectedStructure, getResult());

				 	 if(!sourceNet.isSupply())
				 	 {
				 		 findWithDrainConnectedDevices(sourceNet, *connectedStructure, partSameTechType, nullptr);
				 	 }
				 }
				 else if(partOppositeTechType != nullptr)
				 {
					 partOppositeTechType->addMainStructure( * connectedStructure, getResult());

					 if(!sourceNet.isSupply())
					 {
						 findWithDrainConnectedDevices(sourceNet, *connectedStructure, partOppositeTechType, nullptr);
					 }
				 }

			}
		}
	}

	void Partitioning::findStructureBetweenDifferentialPairAndOutputFirstStage(const StructRec::Structure & dP, LoadPart & loadPartOppositeTechType)
	{

		std::vector< const StructRec::Structure *> dpArrays = dP.getArrayChildren();

		const StructRec::Structure & dpArray1 = **dpArrays.begin();
		const StructRec::Structure & dpArray2 = **std::next(dpArrays.begin());

		//Finding load Parts only of array level:
		const StructRec::StructureNet & drainNet1 = dpArray1.findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));
		const StructRec::StructureNet & drainNet2 = dpArray2.findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain"));

		std::vector<const StructRec::Structure *> connectedStructures1  = drainNet1.getAllConnectedStructures();
		for(auto & connectedStructure : connectedStructures1)
		{
			if(connectedStructure->getTechType() != dP.getTechType()
				&& !connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Source")).isSupply())
			{
				loadPartOppositeTechType.addMainStructure(*connectedStructure, getResult());
			}
		}

		std::vector<const StructRec::Structure *> connectedStructures2  = drainNet2.getAllConnectedStructures();
		for(auto & connectedStructure : connectedStructures2)
		{
			if(connectedStructure->getTechType() != dP.getTechType()
				&& !connectedStructure->findNet(StructRec::StructurePinType(connectedStructure->getStructureName(), "Source")).isSupply())
			{
				loadPartOppositeTechType.addMainStructure(*connectedStructure, getResult());
			}
		}

	}
}
