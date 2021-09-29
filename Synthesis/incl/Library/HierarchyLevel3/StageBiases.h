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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_STAGEBIASES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_STAGEBIASES_H_

#include "Synthesis/incl/Library/LibraryItem.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"


namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
  class StructuralLevel;

	class StageBiases : public LibraryItem
	{
		public:
			StageBiases(const StructuralLevel & structuralLevel);
			~StageBiases();

			std::vector<const Core::Circuit*>  getAllStageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getAllStageBiasesNmos() const;

			std::vector<const Core::Circuit*>  getOneTransistorStageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getOneTransistorStageBiasesNmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorStageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorStageBiasesNmos() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName IN_TERMINAL_;
			static const Core::TerminalName SOURCE_TERMINAL_;
			static const Core::TerminalName OUT_TERMINAL_;

			static const Core::TerminalName INNER_TERMINAL_;
			static const Core::TerminalName INOUTPUT_TERMINAL_;
			static const Core::TerminalName INSOURCE_TERMINAL_;

		private:
			void initializeStageBiasesPmos(const StructuralLevel & structuralLevel);
			void initializeStageBiasesNmos(const StructuralLevel & structuralLevel);

			
        	std::vector<const Core::Circuit*> createOneTransistorStageBiases(
								std::vector<const Core::Circuit*> oneTransistorCurrentBiases, int & index);
        	std::vector<const Core::Circuit*> createTwoTransistorStageBiases(
								std::vector<const Core::Circuit*> twoTransistorCurrentBiases, int & index);

			const Core::Circuit & createOneTransistorStageBias(Core::Instance & currentBias, int & index);
			const Core::Circuit & createTwoTransistorStageBias(Core::Instance & currentBias, int & index);

			void connectInstanceTerminalsOfOneTransistorStageBias(Core::Circuit & stageBias, Core::Instance & currentBias);
			void connectInstanceTerminalsOfTwoTransistorStageBias(Core::Circuit & stageBias, Core::Instance & currentBias);

			void eraseOneTransistorBiasesNmos();
            void eraseTwoTransistorBiasesNmos();

			void eraseOneTransistorBiasesPmos();
            void eraseTwoTransistorBiasesPmos();

		private:
			static const Core::InstanceName ONETRANSISTORCURRENTBIAS_;
			static const Core::InstanceName TWOTRANSISTORCURRENTBIAS_;

			static const Core::NetId IN_NET_;
			static const Core::NetId SOURCE_NET_;
			static const Core::NetId OUT_NET_;

			static const Core::NetId INNER_NET_;
			static const Core::NetId INOUTPUT_NET_;
			static const Core::NetId INSOURCE_NET_;


			std::vector<const Core::Circuit *> oneTransistorBiasesNmos_;
            std::vector<const Core::Circuit *> twoTransistorBiasesNmos_;
			
            std::vector<const Core::Circuit *> oneTransistorBiasesPmos_;
            std::vector<const Core::Circuit *> twoTransistorBiasesPmos_;
	};

    }
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_StageBiases_H_ */
