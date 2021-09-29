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

#ifndef PARTITIONING_INCL_PARTITIONING_H_
#define PARTITIONING_INCL_PARTITIONING_H_

#include "Partitioning/incl/Results/Result.h"
#include "Partitioning/incl/Parts/BiasPart.h"
#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "StructRec/incl/Results/StructureCircuits.h"

namespace Partitioning {

	class Partitioning
	{
	public:

		Partitioning();
		~Partitioning();

		Result & compute(const StructRec::StructureCircuits & circuits);

		void setResult(Result & result);

	private:
		int & getIdTransPart();
		int & getIdLoadPart();
		int & getIdBiasPart();
		int & getIdCapacitancePart();
		int & getIdUndefinedPart();
		int & getIdCommonModeSignalDetectorPart();
		int & getIdResistorPart();
		int & getIdPositiveFeedbackPart();
		Result & getResult() const;

		void partitioningDifferentialPairs(const StructRec::StructureCircuits & circuits);
		void partitioningBiasPartsOfDifferentialPairs(const StructRec::StructureCircuits & circuits);
		void partitioningLoadPartsOfDifferentialPairs(const StructRec::StructureCircuits & circuits);
		void partitioningSecondStage(const StructRec::StructureCircuits &circuits);
		void partitioningThirdStage(const StructRec::StructureCircuits & circuit);
		void findCommonModeSignalDetector(const StructRec::StructureCircuits &circuits);
		void findPositiveFeedbackParts(const StructRec::StructureCircuits & circuits);

		void classifyMosfetAnalogInverter(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedAnalogInverter(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterNmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterPmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterNmosDiodeTransistor(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterPmosDiodeTransistor(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterNmosDiodeTransistorPmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetAnalogInverterPmosDiodeTransistorNmosCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedPMOSAnalogInverter(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedNMOSAnalogInverter(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedPMOSAnalogInverterCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedNMOSAnalogInverterCurrentMirrorLoad(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedPMOSAnalogInverterOneDiodeTransistor(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedNMOSAnalogInverterOneDiodeTransistor(const StructRec::StructureCircuits & circuits);
		void classifyMosfetCascodedAnalogInverterTwoCurrentMirrorLoads(const StructRec::StructureCircuits & circuits);
		void classifyInverter(const StructRec::StructureCircuits & circuits, const StructRec::StructureName inverterName,
				const StructRec::StructurePinType gatePin1, const StructRec::StructurePinType gatePin2);
		void recognizeSecondSecondStage(const StructRec::StructureCircuits & circuits);

		const StructRec::Structure * findBiasStructToSecondSecondStageTransconductance(std::vector<const StructRec::Structure*>potentialBiasStruct);
		void partitioningRemainingCurrentMirrors(const StructRec::StructureCircuits & circuits);
		void partitioningVoltageReference1(const StructRec::StructureCircuits & circuits);
		void classifyDiodeArrays(const StructRec::StructureCircuits & circuits);
		void partitioningCapacitances(const StructRec::StructureCircuits & circuits);
		void createUndefinedParts(const StructRec::StructureCircuits & circuits);

		void initializeInverterStage(std::vector<const StructRec::Structure *> transStructs, const StructRec::Structure & biasStruc, std::string type);
		void initializeLoadPartsOfDifferentialPair(const StructRec::StructureCircuits & circuits, const StructRec::Structure & dP);
		void initializeBiasPartOfDifferentialPair(const StructRec::StructureCircuits & circuits, const StructRec::Structure & diffPair);

		void findBiasOfLoadPart(LoadPart& loadPart);

		void addBiasedPartTypeVoltageBias(const StructRec::Pair & currentMirror, const StructRec::StructureCircuits  & circuits);
		void addBiasedPartToCurrentBias(const StructRec::Pair & currentMirror, const StructRec::StructureCircuits  & circuits );


		void findWithDrainConnectedDevices(const StructRec::StructureNet & net, const StructRec::Structure & refStruct,
				Part * partSameTechType, Part * partOppositeTechType);

		bool hasResult() const;
		bool parentHasDevicesInOtherStructure(const StructRec::Structure & structure,const StructRec::Structure & other ) const;

		bool isVoltageBias(const StructRec::Structure& structure ) const;
		bool isCurrentBias(const StructRec::Structure & structure) const;

		bool hasVoltageBiasesWithSameType(const std::vector<const StructRec::Structure*>& structures1,
				const std::vector<const StructRec::Structure*>& structures2 ) const;
		bool hasCurrentBiasesWithSameType(const std::vector<const StructRec::Structure*> & structures1,
				const std::vector<const StructRec::Structure*>& structures2) const;

		bool hasVoltageBiasOutputConnection(const StructRec::StructureNet& net) const;
		bool hasVoltageBiasInputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const;
		bool hasCurrentBiasOutputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const;
		bool hasCurrentBiasInputConnectionOnNet(const StructRec::Structure & structure, const StructRec::StructureNet& net) const;
		bool hasSecondStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits & circuits) const;
		bool hasSecondStageInputConnection(const StructRec::StructureNet & net, const StructRec::StructureCircuits & circuits) const;
		bool hasFirstStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits & circuits) const ;
		bool hasThirdStageInputConnection(const TransconductancePart & part) const;
		bool hasThirdStageOutputConnection(const StructRec::StructureNet& net, const StructRec::StructureCircuits & circuits) const;
		bool hasGroundConnection(const StructRec::StructureNet & net ) const;
		bool hasStageOutputNetConnection(const StructRec::StructureNet& net, const Part & stage,
				const StructRec::StructureCircuits & circuits) const;
		bool onlyOneTransistorConnectedToNet(const StructRec::StructureNet & net) const;
		bool isConnectedAtDrainToCapacitance(const TransconductancePart & part) const;
		bool isSymmetricalOpAmpLoadPart(const LoadPart & loadPart) const;

		bool oneParentIsCurrentMirror(const StructRec::Structure & structure ) const;

		const StructRec::Structure & getCurrentMirrorParent(const StructRec::Structure & structure);
		const StructRec::Structure & getRecursiveCurrentMirrorParent(const StructRec::Structure & structure);
		Part & initializeVoltageBiasToCurrentBias(const StructRec::Structure & currentBias);

		std::vector<const StructRec::Structure *>  getVoltageBiasesWithSameType(
				const std::vector<const StructRec::Structure*>& structures1, const std::vector<const StructRec::Structure*>& structures2,
				const StructRec::StructureNet  & net1, const StructRec::StructureNet & net2 );
		std::vector<const StructRec::Structure *>  getCurrentBiasesWithSameType(
				const std::vector<const StructRec::Structure*> & structures1, const std::vector<const StructRec::Structure*>& structures2,
			const StructRec::StructureNet  & net1, const StructRec::StructureNet & net2 );

		bool isCurrentMirror(const StructRec::Structure & structure) const;
		bool hasOtherCommonStructures(const std::vector<const StructRec::Structure*>& structures1,
				const std::vector<const StructRec::Structure*>& structures2, const StructRec::Structure &  oldStructure) const;
		const StructRec::Structure& getOtherCommonStructure(const std::vector<const StructRec::Structure*>& structures1,
				const std::vector<const StructRec::Structure*>& structures2, const StructRec::Structure & oldStructure);

		const StructRec::Structure & findOtherDifferentialPair(std::vector<const StructRec::Structure *> dPVector1, std::vector<const StructRec::Structure*> dPVector2,
				const StructRec::Structure & dP);

		void findWithDrainConnectedLoadStructures(const StructRec::StructureNet & net, const StructRec::Structure & structure,
				LoadPart & loadPartSameTechType, LoadPart * loadPartOppositeTechType = nullptr);
		void findStructureBetweenDifferentialPairAndOutputFirstStage(const StructRec::Structure & dP, LoadPart & loadPartOppositeTechType);

		bool isInverterName(const StructRec::StructureName structureName) const;
		bool isConnectedToInverterOutput(const StructRec::StructureNet & net, const StructRec::Structure & inverter) const;
		bool inputOfInverterIsConnectedToFirstStageOutput(const StructRec::Structure & inverter, const StructRec::StructureCircuits &circuits) const;

	private:
		int idUndefinedPart_;
		int idTransPart_;
		int idBiasPart_;
		int idLoadPart_;
		int idCapacitancePart_;
		int idCommonModeSignalDetectorPart_;
		int idResistorPart_;
		int idPositiveFeedbackPart_;
		Result * result_;
	};

}



#endif /* STRUCTUREDANALOGDESIGN_INCL_PARTITIONING_PARTITIONING_H_ */
