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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_OPAMPS_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_OPAMPS_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Device/TechType.h"

#include "Synthesis/incl/Library/LibraryItem.h"
#include <mutex>



namespace Core
{
	class Circuit;
	class Instance;
	class Device;
	class InstanceTerminal;
	class Net;
}



namespace Synthesis
{
  class AmplificationStageLevel;
  class CurrentBiases;
  class VoltageBiases;
  class Capacitor;

	class OpAmps : public LibraryItem
	{
		public:
			OpAmps(const AmplificationStageLevel & amplificationStageLevel, 
                            const CurrentBiases & currentBiases, const VoltageBiases &  voltageBiases, const Capacitor & capacitor);
			~OpAmps();

            std::vector<const Core::Circuit*> createSimpleOneStageOpAmps(int caseNumber, int & index);
		    std::vector<const Core::Circuit*> createFullyDifferentialOneStageOpAmps(int caseNumber, int & index);
		    std::vector<const Core::Circuit*> createComplementaryOpAmps(int caseNumber, int & index);
		    std::vector<const Core::Circuit*> createSymmetricalOpAmps(int caseNumber, int & index);

		    std::vector<const Core::Circuit*> createSimpleTwoStageOpAmps(std::vector<const Core::Circuit*> oneStageOpAmps);
		    std::vector<const Core::Circuit*> createSimpleTwoStageOpAmps(const Core::Circuit& oneStageOpAmp, std::mutex &myMutex);
		    std::vector<const Core::Circuit*> createSimpleTwoStageOpAmps(const Core::Circuit& oneStageOpAmp);

		    std::vector<const Core::Circuit*> createFullyDifferentialTwoStageOpAmps(std::vector<const Core::Circuit*> oneStageOpAmps);
		    std::vector<const Core::Circuit*> createFullyDifferentialTwoStageOpAmps(const Core::Circuit & oneStageOpAmp, std::mutex &myMutex);
		    std::vector<const Core::Circuit*> createFullyDifferentialTwoStageOpAmps(const Core::Circuit & oneStageOpAmp);

			std::string toStr() const;
		public:
			static const Core::TerminalName IN1_TERMINAL_;
			static const Core::TerminalName IN2_TERMINAL_;
            static const Core::TerminalName OUT_TERMINAL_;
			static const Core::TerminalName OUT1_TERMINAL_;
			static const Core::TerminalName OUT2_TERMINAL_;

			static const Core::TerminalName IBIAS_TERMINAL_;
            static const Core::TerminalName VREF_TERMINAL_;

			static const Core::TerminalName SOURCEPMOS_TERMINAL_;
			static const Core::TerminalName SOURCENMOS_TERMINAL_;

			static const Core::InstanceName FIRSTSTAGE_;

		private:
            const AmplificationStageLevel & getAmplificationStageLevel() const;
            const CurrentBiases & getCurrentBiases() const;
            const VoltageBiases & getVoltageBiases() const;
            const Capacitor & getCapacitor() const;

            void addGateInstanceTerminalConnectedToADrain(const Core::InstanceTerminal & terminal);
            bool isGateInstanceTerminalConnectedToADrain(const Core::InstanceTerminal & terminal) const;

			const Core::Circuit& createSimpleOpAmp(int & index, Core::Instance & firstStage, 
													Core::Instance * secondStage = nullptr);
			const Core::Circuit& createFullyDifferentialOpAmp(int & index, Core::Instance & firstStage, Core::Instance & feedbackStage,
													Core::Instance * secondStage1 = nullptr, Core::Instance * secondStage2 = nullptr);
			const Core::Circuit& createComplementaryOpAmp(int & index, Core::Instance & firstStage);
            const Core::Circuit & createSymmetricalOpAmp(int & index, Core::Instance & firstStage, Core::Instance & secondStage,
													Core::Instance & transconductanceComplementarySecondStage, Core::Instance & stageBiasComplementarySecondStage);
			
			void addTerminalNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Circuit & opAmp) const;	
			void addFirstStageToSecondStageNets(std::vector<Core::NetId> & netNames, const Core::Circuit & opAmp) const;									
			void addComplementarySecondStageNets(std::vector<Core::NetId> & netNames, Core::Circuit & opAmp) const;
			
			void connectInstanceTerminalsSimpleOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, Core::Instance * secondStage = nullptr) const;
			void connectInstanceTerminalsFullyDifferentialOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, 
                                            Core::Instance & feedbackStage, Core::Instance * secondStage1 = nullptr, Core::Instance * secondStage2 = nullptr) const;
            void connectInstanceTerminalsComplementaryOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage) const;
            void connectInstanceTerminalsSymmetricalOpAmp(Core::Circuit & opAmp, Core::Instance & firstStage, 
                                            Core::Instance &  secondStage, Core::Instance & transconductanceComplementarySecondStage,
                                            Core::Instance stageBiasComplementarySecondStage) const;
			void connectInstanceTerminalsCapacitors(Core::Circuit & opAmp, Core::Instance & loadCapacitor, 
											Core::Instance * compensationCapacitor = nullptr) const;

			void connectedLoadInstanceTerminalToFeedbackStage(Core::Circuit & opAmp, Core::Instance & firstStage) const;
		
			const Core::Circuit & getSecondStageTransconductance(const Core::Circuit & secondStage) const;
			const Core::Circuit & getSecondStageStageBias(const Core::Circuit & secondStage) const;
            std::vector<const Core::Circuit *> findComplementarySecondStageStageBiases(const Core::Circuit & secondStage) const;

			void setSupplyNets(Core::Circuit & opAmp) const;

            void buildAndConnectedBias(Core::Circuit & opAmp);

			std::vector<Core::InstanceTerminal*> getGateInstanceTerminalsNotConnectedToADrainSourcePmos(Core::Circuit & opAmp) const;
			std::vector<Core::InstanceTerminal*> getGateInstanceTerminalsNotConnectedToADrainOutputPmos(Core::Circuit & opAmp) const;
			std::vector<Core::InstanceTerminal*> getGateInstanceTerminalsNotConnectedToADrainSourceNmos(Core::Circuit & opAmp) const;
			std::vector<Core::InstanceTerminal*> getGateInstanceTerminalsNotConnectedToADrainOutputNmos(Core::Circuit & opAmp) const;
			std::vector<Core::InstanceTerminal*> getGateInstanceTerminalsNotConnectedToADrain(std::vector<Core::InstanceTerminal*> instanceTerminals) const;

			std::vector<Core::Instance* > getVoltageBiasesOfBias(Core::Circuit & opAmp, Core::TechType techType);
			Core::Instance & findReferenceVoltageBias(Core::Circuit & opAmp, std::vector<Core::Instance* > voltageBiases, Core::TechType techType);
			Core::Net & getReferenceVoltageBiasOutputNet(Core::Instance & referenceVoltageBias);

			bool netIsInVector(Core::NetId netId, std::vector<Core::NetId> unconnectedTopLevelGateNets) const;
			bool terminalIsInVector(Core::InstanceTerminal & instanceTerminal, std::vector<Core::InstanceTerminal*> instanceTerminals) const;
			
			void checkForNonInvertingStageInput(std::vector<Core::InstanceTerminal*> & gateInstanceTerminals);
			void connectCurrentBiasOfImprovedWilsonCurrentMirror(Core::Circuit & opAmp, std::vector<Core::InstanceTerminal*> gateInstanceTerminalsOutput, int & index);
			void connectCascodeGCC(Core::Circuit & opAmp, Core::InstanceTerminal & innerGCCTerminal,int &  index);
			void connectRemainingGateTerminals(Core::Circuit & opAmp, std::vector<Core::InstanceTerminal *> gateInstanceTerminalsSource,
					std::vector<Core::InstanceTerminal *>gateInstanceTerminalsOutput, Core::TechType techType, int & index);
			void connectIbiasTerminal(Core::Circuit & opAmp, Core::TechType techType);
			void addCurrentBiasesToCircuit(Core::Circuit & opAmp);
			void addCurrentBiasesToCircuit(Core::Circuit & opAmp, Core::Net & referenceVoltageBiasOutputNet,
					std::vector<Core::Instance*> voltageBiases);

			const Core::Circuit & findVoltageBiasOfImprovedWilsonCurrentMirror(std::vector<const Core::Circuit * > twoTransistorVoltageBiases) const;
			Core::InstanceTerminal & getSourceTermimalImprovedWilsonCurrentMirror(Core::Instance & instance, Core::TechType techType) const;

			void addOneTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias, Core::InstanceTerminal & terminalCurrentBias);
			void addTwoTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
					Core::InstanceTerminal& terminalOutputCurrentBias, Core::InstanceTerminal& terminalSourceCurrentBias);
			void addOneTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
					std::vector<Core::InstanceTerminal *> terminalsCurrentBias);
			void addTwoTransistorVoltageBiasToCircuit(Core::Circuit & opAmp, Core::Instance & voltageBias,
					std::vector<Core::InstanceTerminal*> terminalsOutputCurrentBias, std::vector<Core::InstanceTerminal*> terminalsSourceCurrentBias);

			bool allInstanceTerminalsArePartOfNotGateDrainConnectedTwoTransistorStacks(
					std::vector<Core::InstanceTerminal *> gateInstanceTerminalsSource,std::vector<Core::InstanceTerminal *> gateInstanceTerminalsOutput) const;
			bool isGateNetOfCurrentBiasesOfImprovedWilsonCurrentMirror(Core::NetId netId, const Core::Circuit & flatCircuit) const;
			bool isVoltageBiasConnectedToIbias(Core::Instance & voltageBias) const;
			bool hasVoltageBiasConnectedToIbias(std::vector<Core::Instance*> voltageBiases) const;

			bool isConnectedToTerminalOfSourceTransistors(const Core::Net & net);
			bool isGateInstanceTerminalSourceTransistorOfTranistorStack(Core::InstanceTerminal & sourceInstanceTerminal) const;
			Core::InstanceTerminal & getBelongingGateOutputInstanceTerminal(Core::InstanceTerminal & sourceInstanceTerminal) const;
			Core::InstanceTerminal & getBelongingGateSourceInstanceTerminal(Core::InstanceTerminal & outputInstanceTerminal) const;

			bool isGateNetOfNmosTransistors(const Core::Net & flatNet) const;
			bool isGateNetOfPmosTransistors(const Core::Net & flatNet) const;
		private:
			Core::InstanceName createVoltageBiasInstanceName(int index, Core::TechType techType) const;
			Core::InstanceName createCurrentBiasInstanceName(int index, Core::TechType techType) const;

			std::vector<Core::NetId>  createNetNamesOfBiasInstance(Core::Instance & biasInstance) const;
			Core::NetId createNetIdByInstanceName(std::string netName, Core::InstanceName instanceName) const;

			static const Core::InstanceName SECONDSTAGE_;
			static const Core::InstanceName SECONDSTAGE1_;
			static const Core::InstanceName SECONDSTAGE2_;

			static const Core::InstanceName LOADCAPACITOR_;
			static const Core::InstanceName LOADCAPACITOR1_;
			static const Core::InstanceName LOADCAPACITOR2_;
			static const Core::InstanceName COMPENSATIONCAPACITOR_;
			static const Core::InstanceName COMPENSATIONCAPACITOR1_;
			static const Core::InstanceName COMPENSATIONCAPACITOR2_;

            static const Core::InstanceName FEEDBACKSTAGE_;

            static const std::string VOLTAGEBIAS_STRING_;
            static const std::string CURRENTBIAS_STRING_;
			
            static const Core::InstanceName TRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_;
			static const Core::InstanceName STAGEBIASCOMPLEMENTARYSECONDSTAGE_;


			static const Core::NetId IN1_NET_;
			static const Core::NetId IN2_NET_;
            static const Core::NetId OUT_NET_;
			static const Core::NetId OUT1_NET_;
			static const Core::NetId OUT2_NET_;

			static const Core::NetId SOURCEPMOS_NET_;
			static const Core::NetId SOURCENMOS_NET_;

            static const Core::NetId OUTFIRSTSTAGE_NET_;
			static const Core::NetId OUT1FIRSTSTAGE_NET_;
			static const Core::NetId OUT2FIRSTSTAGE_NET_;

			static const Core::NetId OUTFEEDBACKSTAGE_NET_;


            static const Core::NetId INNERCOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INSOURCETRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INOUTPUTTRANSCONDUCTANCECOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INSOURCESTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_;
            static const Core::NetId INOUTPUTSTAGEBIASCOMPLEMENTARYSECONDSTAGE_NET_;

			static const Core::NetId IBIAS_NET_;
            static const Core::NetId VREF_NET_;


            const AmplificationStageLevel * amplificationStageLevel_;
            const CurrentBiases * currentBiases_; 
            const VoltageBiases *  voltageBiases_;
            const Capacitor * capacitor_;

            std::vector<const Core::InstanceTerminal*> gateInstanceTerminalsConnectedToADrain_;

	};

    }

#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_OpAmps_H_ */
