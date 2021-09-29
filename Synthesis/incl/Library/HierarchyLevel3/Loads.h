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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_LOADS_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_LOADS_H_

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

	class Loads : public LibraryItem
	{
		public:
			Loads(AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel);
			~Loads();
			void eraseTemporaryFunctionalblocks() const;

			const AmplificationStagesSubBlockLevel & getAmplificationStagesSubBlockLevel() const;
			const AutomaticSizing::CircuitInformation & getCircuitInformation();
			std::vector<const Core::Circuit *> getTemporaryLoads() const;

			std::vector<const Core::Circuit*>  createSimpleMixedLoadPmos() const;
			std::vector<const Core::Circuit*>  createSimpleMixedLoadNmos() const;
			std::vector<const Core::Circuit*>  createSimpleTwoLoadPartsFoldedGCCMixedLoadPmos() const;
			std::vector<const Core::Circuit*>  createSimpleTwoLoadPartsFoldedGCCMixedLoadNmos() const;
			std::vector<const Core::Circuit*>  createLoadsTwoLoadPartsCascodeGCCMixedPmos() const;
			std::vector<const Core::Circuit*>  createLoadsTwoLoadPartsCascodeGCCMixedNmos() const;
			std::vector<const Core::Circuit*>  createLoadsTwoLoadPartsMixedCurrentBiasesPmos() const;
			std::vector<const Core::Circuit*>  createLoadsTwoLoadPartsMixedCurrentBiasesNmos() const;
			std::vector<const Core::Circuit*>  createLoadsPmosForFullyDifferentialNonInvertingStage() const;
			std::vector<const Core::Circuit*>  createLoadsNmosForFullyDifferentialNonInvertingStage() const;

			std::vector<const Core::Circuit*>  createLoadsForComplementaryNonInvertingStage() const;

			std::vector<const Core::Circuit*>  createLoadsPmosTwoForSymmetricalOpAmpNonInvertingStage() const;
			std::vector<const Core::Circuit*>  createLoadsPmosFourForSymmetricalOpAmpNonInvertingStage() const;
			std::vector<const Core::Circuit*>  createLoadsNmosTwoForSymmetricalOpAmpNonInvertingStage() const;
			std::vector<const Core::Circuit*>  createLoadsNmosFourForSymmetricalOpAmpNonInvertingStage() const;

			std::vector<const Core::Circuit*>  getLoadsPmosForFeedbackNonInvertingStage() const;
			std::vector<const Core::Circuit*>  getLoadsNmosForFeedbackNonInvertingStage() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName OUT1_TERMINAL_;
			static const Core::TerminalName OUT2_TERMINAL_;

			static const Core::TerminalName SOURCELOAD1_TERMINAL_;
			static const Core::TerminalName SOURCELOAD2_TERMINAL_;

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

			static const Core::TerminalName OUTOUTPUT1LOAD1_TERMINAL_;
			static const Core::TerminalName OUTOUTPUT2LOAD1_TERMINAL_;
			static const Core::TerminalName OUTSOURCE1LOAD1_TERMINAL_;
    		static const Core::TerminalName OUTSOURCE2LOAD1_TERMINAL_;

			static const Core::InstanceName LOADPART1_;
			static const Core::InstanceName LOADPART2_;


		private:
			void initializeLoadsVoltageBiasesLoadPart(AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, int & nmosIndex, int & pmosIndex);

			std::vector<const Core::Circuit *> createOneLoadPartLoads(std::vector<const Core::Circuit *> loadParts, int & nmos) const;
			std::vector<const Core::Circuit *> createTwoLoadPartLoadsWithGCC(std::vector<const Core::Circuit *> loadPartsGCC,
												std::vector<const Core::Circuit *> secondLoadParts, int & index) const;
			std::vector<const Core::Circuit *> createTwoLoadPartLoadsWithoutGCC(std::vector<const Core::Circuit *> mixedLoadParts,
												std::vector<const Core::Circuit *> currentBiasLoadParts, int & index) const;
			std::vector<const Core::Circuit *> createSymmetricalLoadsFourTransistorMixedLoadParts(std::vector<const Core::Circuit *> pmosLoadParts,
												std::vector<const Core::Circuit *> nmosLoadParts) const;

			const Core::Circuit & createOneLoadPartLoad(Core::Instance & loadPart, int index) const;
			const Core::Circuit & createTwoLoadPartLoadWithGCC(Core::Instance & loadPartWithGCC, Core::Instance & secondLoadPart, int index) const;
			const Core::Circuit & createTwoLoadPartLoadWithoutGCC(Core::Instance & mixedLoadPart, Core::Instance & currentBiasLoadPart, int index) const;
			const Core::Circuit & createSymmetricalLoadFourTransistorMixedLoadParts(Core::Instance & pmosLoadPart,
												Core::Instance & nmosLoadPart, int index) const;

			void connectInstanceTerminalsOfLoadPart1WithoutGCC(Core::Circuit & load, Core::Instance & loadPart1) const;
			void connectInstanceTerminalsOfLoadPart1WithGCC(Core::Circuit & load, Core::Instance & loadPartWithGCC) const;
			void connectInstanceTerminalsOfLoadPart2(Core::Circuit & load, Core::Instance & loadPart2) const;


			void addLoad1WithoutGCCNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, 
									Core::Instance & loadPart1) const;
			void addLoad1WithGCCNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
												Core::Instance & loadPart1) const;
			void addLoad2Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
												Core::Instance & loadPart2) const;
		
			void eraseOneLoadPartLoadsVoltageBiasesNmos();
			void eraseOneLoadPartLoadsVoltageBiasesPmos();

		private:
			static const Core::NetId OUT1_NET_;
			static const Core::NetId OUT2_NET_;

			static const Core::NetId SOURCELOAD1_NET_;
			static const Core::NetId SOURCELOAD2_NET_;

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

			static const Core::NetId OUTOUTPUT1LOAD1_NET_;
			static const Core::NetId OUTOUTPUT2LOAD1_NET_;
			static const Core::NetId OUTSOURCE1LOAD1_NET_;
    		static const Core::NetId OUTSOURCE2LOAD1_NET_;

	

			std::vector<const Core::Circuit *> oneLoadPartLoadsVoltageBiasesNmos_;
			std::vector<const Core::Circuit *> oneLoadPartLoadsVoltageBiasesPmos_;

			const AmplificationStagesSubBlockLevel * amplificationStagesSubBlockLevel_;
			std::vector<const Core::Circuit*> temporaryLoads_;


	};

    }
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_Loads_H_ */
