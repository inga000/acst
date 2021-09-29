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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_NONINVERTINGSTAGES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_NONINVERTINGSTAGES_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include "Synthesis/incl/Library/LibraryItem.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
  class AmplificationStagesSubBlockLevel;

	class NonInvertingStages : public LibraryItem
	{
		public:
			NonInvertingStages(const AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, const AutomaticSizing::CircuitInformation & circuitInformationS);
			~NonInvertingStages();
			void eraseTemporaryFunctionalblocks() const;

			const AmplificationStagesSubBlockLevel & getAmplificationStagesSubBlockLevel() const;
			std::vector<const Core::Circuit *> getTemporaryNonInvertingStages() const;

			std::vector<const Core::Circuit *> getFeedbackNonInvertingStagesPmosTransconductance() const;
			std::vector<const Core::Circuit *> getFeedbackNonInvertingStagesNmosTransconductance() const;

			std::vector<const Core::Circuit *> createSimpleNonInvertingStages(int caseNumber) const;
			std::vector<const Core::Circuit *> createFullyDifferentialNonInvertingStages(int caseNumber) const;
			std::vector<const Core::Circuit *> createComplementaryNonInvertingStages(int caseNumber) const;
			std::vector<const Core::Circuit *> createSymmetricalNonInvertingStages(int caseNumber) const;


			std::string toStr() const;
		public:
			static const Core::TerminalName IN1_TERMINAL_;
			static const Core::TerminalName IN2_TERMINAL_;
			static const Core::TerminalName OUT1_TERMINAL_;
			static const Core::TerminalName OUT2_TERMINAL_;

			static const Core::TerminalName SOURCETRANSCONDUCTANCE_TERMINAL_;
			static const Core::TerminalName SOURCETRANSCONDUCTANCE1_TERMINAL_;
			static const Core::TerminalName SOURCETRANSCONDUCTANCE2_TERMINAL_;
			static const Core::TerminalName INNERTRANSCONDUCTANCE_TERMINAL_;
			static const Core::TerminalName SOURCETRANSCONDUCTANCEPMOS_TERMINAL_;
			static const Core::TerminalName SOURCETRANSCONDUCTANCENMOS_TERMINAL_;

			static const Core::TerminalName SOURCENMOS_TERMINAL_;
			static const Core::TerminalName SOURCEPMOS_TERMINAL_;

			static const Core::TerminalName INPUTSTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INSOURCESTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INOUTPUTSTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIAS1_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIAS2_TERMINAL_;

			static const Core::TerminalName INPUTSTAGEBIASPMOS_TERMINAL_;
			static const Core::TerminalName INSOURCESTAGEBIASPMOS_TERMINAL_;
			static const Core::TerminalName INOUTPUTSTAGEBIASPMOS_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIASPMOS_TERMINAL_;

			static const Core::TerminalName INPUTSTAGEBIASNMOS_TERMINAL_;
			static const Core::TerminalName INSOURCESTAGEBIASNMOS_TERMINAL_;
			static const Core::TerminalName INOUTPUTSTAGEBIASNMOS_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIASNMOS_TERMINAL_;

			static const Core::TerminalName SOURCEGCC1_TERMINAL_;
			static const Core::TerminalName SOURCEGCC2_TERMINAL_;

			static const Core::TerminalName INNERLOAD1_TERMINAL_;
			static const Core::TerminalName INNERLOAD2_TERMINAL_;
			static const Core::TerminalName INNERGCC_TERMINAL_;
			static const Core::TerminalName INNERBIASGCC_TERMINAL_;
			static const Core::TerminalName INNERSOURCELOAD1_TERMINAL_;			
			static const Core::TerminalName INNEROUTPUTLOAD1_TERMINAL_;
			static const Core::TerminalName INNERSOURCELOAD2_TERMINAL_;
			static const Core::TerminalName INNEROUTPUTLOAD2_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK1LOAD1_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK2LOAD1_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK1LOAD2_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK2LOAD2_TERMINAL_;

			static const Core::TerminalName INNERSOURCELOADNMOS_TERMINAL_;			
			static const Core::TerminalName INNEROUTPUTLOADNMOS_TERMINAL_;
			static const Core::TerminalName INNERSOURCELOADPMOS_TERMINAL_;
			static const Core::TerminalName INNEROUTPUTLOADPMOS_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK1LOADNMOS_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK2LOADNMOS_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK1LOADPMOS_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK2LOADPMOS_TERMINAL_;

			static const Core::TerminalName OUTOUTPUT1LOAD1_TERMINAL_;
			static const Core::TerminalName OUTOUTPUT2LOAD1_TERMINAL_;
			static const Core::TerminalName OUTSOURCE1LOAD1_TERMINAL_;
    		static const Core::TerminalName OUTSOURCE2LOAD1_TERMINAL_;

    		static const Core::NetId INNERTRANSCONDUCTANCE_NET_;

			static const Core::InstanceName LOAD_;
			
		private:

			void initializeFeedbackNonInvertingStages(const AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, int & indexPmos, int & indexNmos);

			std::vector<const Core::Circuit*> createSimpleTransconductanceNonInvertingStages(const Core::Circuit & transconductance,
													std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiases, int & index) const;
			const Core::Circuit & getTransconductancePmos();
			std::vector<const Core::Circuit*> getSimpleMixedLoadsNmos();
			std::vector<const Core::Circuit*> getOneTransistorStageBiasPmos();



			std::vector<const Core::Circuit*> createFeedbackTransconductanceNonInvertingStages(const Core::Circuit & transconductance, 
													std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiases, int & index) const; ///
			std::vector<const Core::Circuit*> createComplementaryTransconductanceNonInvertingStages(const Core::Circuit & transconductance,
																std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiasesNmos,
																std::vector<const Core::Circuit*> stageBiasesPmos) const;
		
			const Core::Circuit& createSimpleTransconductanceNonInvertingStage(Core::Instance & transconductance,
											Core::Instance & load, Core::Instance & stageBias, int index) const;
			const Core::Circuit& createFeedbackTransconductanceNonInvertingStage(Core::Instance & transconductance, 
													Core::Instance & load, Core::Instance & stageBias1, Core::Instance & stageBias2, int index) const;
			const Core::Circuit& createComplementaryTransconductanceNonInvertingStage(Core::Instance & transconductance,
																Core::Instance & load, Core::Instance & stageBiasNmos, Core::Instance & stageBiasPmos, int index) const;
			
			void addLoadNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & load) const;
			void addLoadPart1Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & load) const;
			void addLoadPart2Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & load) const;
			void addStageBiasNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & stageBias) const;
			void addComplementaryLoadNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
														Core::Instance & load) const;
			void addStageBiasesNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
											Core::Instance & stageBiasNmos, Core::Instance& stageBiasPmos) const;


			void connectInstanceTerminalsOfSimpleTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const;
			void connectInstanceTerminalsOfFeedbackTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const;
			void connectInstanceTerminalsOfComplementaryTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const;
			void connectInstanceTerminalsOfLoad(Core::Circuit & nonInvertingStage, Core::Instance & load) const;
			void connectInstanceTerminalsOfLoadPart1(Core::Circuit & nonInvertingStage, Core::Instance & load) const;
			void connectInstanceTerminalsOfLoadPart2(Core::Circuit & nonInvertingStage, Core::Instance & load) const;
			void connectInstanceTerminalsOfComplementaryLoad(Core::Circuit & nonInvertingStage, Core::Instance & load) const;
			void connectInstanceTerminalsOfStageBias(Core::Circuit & nonInvertingStage, Core::Instance & stageBias) const;
			void connectInstanceTerminalsOfStageBiases(Core::Circuit & nonInvertingStage, Core::Instance & stageBias1, Core::Instance & stageBias2) const;
			void connectInstanceTerminalsOfComplementaryStageBiases(Core::Circuit & nonInvertingStage, Core::Instance & stageBiasNmos, Core::Instance & stageBiasPmos) const;

			void eraseFeedbackNonInvertingStagesPmosTransconductance();
			void eraseFeedbackNonInvertingStagesNmosTransconductance();


		private:
			static const Core::InstanceName TRANSCONDUCTANCE_;
			static const Core::InstanceName STAGEBIAS_;
			static const Core::InstanceName STAGEBIASNMOS_;
			static const Core::InstanceName STAGEBIASPMOS_;
			static const Core::InstanceName STAGEBIAS1_;
			static const Core::InstanceName STAGEBIAS2_;

			static const Core::NetId IN1_NET_;
			static const Core::NetId IN2_NET_;
			static const Core::NetId OUT1_NET_;
			static const Core::NetId OUT2_NET_;

			static const Core::NetId SOURCETRANSCONDUCTANCE_NET_;
			static const Core::NetId SOURCETRANSCONDUCTANCE1_NET_;
			static const Core::NetId SOURCETRANSCONDUCTANCE2_NET_;

			static const Core::NetId SOURCETRANSCONDUCTANCEPMOS_NET_;
			static const Core::NetId SOURCETRANSCONDUCTANCENMOS_NET_;

			static const Core::NetId SOURCENMOS_NET_;
			static const Core::NetId SOURCEPMOS_NET_;

			static const Core::NetId INPUTSTAGEBIAS_NET_;
			static const Core::NetId INSOURCESTAGEBIAS_NET_;
			static const Core::NetId INOUTPUTSTAGEBIAS_NET_;
			static const Core::NetId INNERSTAGEBIAS_NET_;
			static const Core::NetId INNERSTAGEBIAS1_NET_;
			static const Core::NetId INNERSTAGEBIAS2_NET_;

			static const Core::NetId INPUTSTAGEBIASPMOS_NET_;
			static const Core::NetId INSOURCESTAGEBIASPMOS_NET_;
			static const Core::NetId INOUTPUTSTAGEBIASPMOS_NET_;
			static const Core::NetId INNERSTAGEBIASPMOS_NET_;

			static const Core::NetId INPUTSTAGEBIASNMOS_NET_;
			static const Core::NetId INSOURCESTAGEBIASNMOS_NET_;
			static const Core::NetId INOUTPUTSTAGEBIASNMOS_NET_;
			static const Core::NetId INNERSTAGEBIASNMOS_NET_;

			static const Core::NetId SOURCEGCC1_NET_;
			static const Core::NetId SOURCEGCC2_NET_;

			static const Core::NetId INNERLOAD1_NET_;
			static const Core::NetId INNERLOAD2_NET_;
			static const Core::NetId INNERGCC_NET_;
			static const Core::NetId INNERBIASGCC_NET_;
			static const Core::NetId INNERSOURCELOAD1_NET_;			
			static const Core::NetId INNEROUTPUTLOAD1_NET_;
			static const Core::NetId INNERSOURCELOAD2_NET_;
			static const Core::NetId INNEROUTPUTLOAD2_NET_;
			static const Core::NetId INNERTRANSISTORSTACK1LOAD1_NET_;
			static const Core::NetId INNERTRANSISTORSTACK2LOAD1_NET_;
			static const Core::NetId INNERTRANSISTORSTACK1LOAD2_NET_;
			static const Core::NetId INNERTRANSISTORSTACK2LOAD2_NET_;

			static const Core::NetId INNERSOURCELOADNMOS_NET_;			
			static const Core::NetId INNEROUTPUTLOADNMOS_NET_;
			static const Core::NetId INNERSOURCELOADPMOS_NET_;
			static const Core::NetId INNEROUTPUTLOADPMOS_NET_;
			static const Core::NetId INNERTRANSISTORSTACK1LOADNMOS_NET_;
			static const Core::NetId INNERTRANSISTORSTACK2LOADNMOS_NET_;
			static const Core::NetId INNERTRANSISTORSTACK1LOADPMOS_NET_;
			static const Core::NetId INNERTRANSISTORSTACK2LOADPMOS_NET_;

			static const Core::NetId OUTOUTPUT1LOAD1_NET_;
			static const Core::NetId OUTOUTPUT2LOAD1_NET_;
			static const Core::NetId OUTSOURCE1LOAD1_NET_;
    		static const Core::NetId OUTSOURCE2LOAD1_NET_;

			std::vector<const Core::Circuit *> feedbackNonInvertingStagesPmosTransconductance_;
			std::vector<const Core::Circuit *> feedbackNonInvertingStagesNmosTransconductance_;

			const AmplificationStagesSubBlockLevel * amplificationStagesSubBlockLevel_;
			std::vector<const Core::Circuit*> temporaryNonInvertingStages_;
	};

    }

#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_NonInvertingStages_H_ */
