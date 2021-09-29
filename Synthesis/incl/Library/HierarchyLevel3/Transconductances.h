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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_TRANSCONDUCTANCES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_TRANSCONDUCTANCES_H_

#include "Synthesis/incl/Library/LibraryItem.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
  class StructuralLevel;

	class Transconductances : public LibraryItem
	{
		public:
	  	  	Transconductances(const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation);
			~Transconductances();

			const Core::Circuit &  getSimpleTransconductancePmos() const;
			const Core::Circuit & getSimpleTransconductancePmos2();
			const Core::Circuit &  getSimpleTransconductanceNmos() const;

			const Core::Circuit &  getFeedbackTransconductancePmos() const;
			const Core::Circuit &  getFeedbackTransconductanceNmos() const;

			const Core::Circuit &  getComplementaryTransconductance() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName INPUT1_TERMINAL_;
			static const Core::TerminalName INPUT2_TERMINAL_;

			static const Core::TerminalName INNER_TERMINAL_;
			
			static const Core::TerminalName OUT1_TERMINAL_;
			static const Core::TerminalName OUT2_TERMINAL_;
			static const Core::TerminalName OUT1NMOS_TERMINAL_;
			static const Core::TerminalName OUT2NMOS_TERMINAL_;
			static const Core::TerminalName OUT1PMOS_TERMINAL_;
			static const Core::TerminalName OUT2PMOS_TERMINAL_;

			static const Core::TerminalName SOURCE_TERMINAL_;
			static const Core::TerminalName SOURCE1_TERMINAL_;
			static const Core::TerminalName SOURCE2_TERMINAL_;
			static const Core::TerminalName SOURCENMOS_TERMINAL_;
			static const Core::TerminalName SOURCEPMOS_TERMINAL_;

		private:
			void initializeTransconductances(const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation);
			
			const Core::Circuit & createSimpleTransconductance(const Core::Circuit & differentialPair, int & index);
			const Core::Circuit& createFeedbackTransconductance(const Core::Circuit & differentialPair, int & index);
       		const Core::Circuit & createComplementaryTransconductance(const Core::Circuit & differentialPairPmos,
			   						const Core::Circuit & differentialPairNmos, int & index);

			void connectInstanceTerminalsOfSimpleTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPair);
			void connectInstanceTerminalsOfFeedbackTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPair1, Core::Instance & differentialPair2);
			void connectInstanceTerminalsOfComplementaryTransconductance(Core::Circuit & transconductance, 
									Core::Instance & differentialPairNmos, Core::Instance & differentialPairPmos);

		private:
			static const Core::InstanceName DIFFERENTIALPAIR_;
			static const Core::InstanceName DIFFERENTIALPAIR1_;
			static const Core::InstanceName DIFFERENTIALPAIR2_;
			static const Core::InstanceName DIFFERENTIALPAIRNMOS_;
			static const Core::InstanceName DIFFERENTIALPAIRPMOS_;

			static const Core::NetId INPUT1_NET_;
			static const Core::NetId INPUT2_NET_;

			static const Core::NetId INNER_NET_;
			
			static const Core::NetId OUT1_NET_;
			static const Core::NetId OUT2_NET_;
			static const Core::NetId OUT1NMOS_NET_;
			static const Core::NetId OUT2NMOS_NET_;
			static const Core::NetId OUT1PMOS_NET_;
			static const Core::NetId OUT2PMOS_NET_;

			static const Core::NetId SOURCE_NET_;
			static const Core::NetId SOURCE1_NET_;
			static const Core::NetId SOURCE2_NET_;
			static const Core::NetId SOURCENMOS_NET_;
			static const Core::NetId SOURCEPMOS_NET_;


			const Core::Circuit * simpleTransconductanceNmos_;
            const Core::Circuit* feedbackTransconductanceNmos_;

			const Core::Circuit * simpleTransconductancePmos_;
            const Core::Circuit* feedbackTransconductancePmos_;

			const Core::Circuit * complementaryTransconductance_;
	};

    }
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_Transconductances_H_ */
