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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_INVERTINGSTAGES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL4_INVERTINGSTAGES_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include "Synthesis/incl/Library/LibraryItem.h"


namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
  class StructuralLevel;

	class InvertingStages : public LibraryItem
	{
		public:
			InvertingStages(const StructuralLevel & structuralLevel);
			~InvertingStages();

			std::vector<const Core::Circuit *>  getInvertingStagesPmosTransconductance() const;
			std::vector<const Core::Circuit *>  getInvertingStagesNmosTransconductance() const;
			std::vector<const Core::Circuit *>  getInvertingStages() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName OUTPUT_TERMINAL_;

			static const Core::TerminalName SOURCEPMOS_TERMINAL_;
			static const Core::TerminalName SOURCENMOS_TERMINAL_;

			static const Core::TerminalName INTRANSCONDUCTANCE_TERMINAL_;
			static const Core::TerminalName INOUTPUTTRANSCONDUCTANCE_TERMINAL_;
			static const Core::TerminalName INSOURCETRANSCONDUCTANCE_TERMINAL_;
			static const Core::TerminalName INNERTRANSCONDUCTANCE_TERMINAL_;

			static const Core::TerminalName INSTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INOUTPUTSTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INSOURCESTAGEBIAS_TERMINAL_;
			static const Core::TerminalName INNERSTAGEBIAS_TERMINAL_;

			static const Core::InstanceName ANALOGINVERTER_;

		private:
			void initializeInvertingStages(const StructuralLevel & structuralLevel);

			std::vector<const Core::Circuit*> createInvertingStagesPmosTransconductance(std::vector<const Core::Circuit*> analogInverters);
			std::vector<const Core::Circuit*> createInvertingStagesNmosTransconductance(std::vector<const Core::Circuit*> analogInverters);
		
			const Core::Circuit & createInvertingStagePmosTransconductance(Core::Instance & analogInverter, int & index);
			const Core::Circuit & createInvertingStageNmosTransconductance(Core::Instance & analogInverter, int & index);
		
			void addTransconductanceNets(std::vector<Core::NetId> & netNames, 
							std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, const Core::Instance & transconductance);
        	void addStageBiasNets(std::vector<Core::NetId> & netNames, 
							std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, const Core::Instance & stageBias);

			void connectInstanceTerminalsPmosTransconductance(Core::Circuit & invertingStage, Core::Instance & analogInverter);
			void connectInstanceTerminalsNmosTransconductance(Core::Circuit & invertingStage, Core::Instance & analogInverter);

			void eraseInvertingStagesPmosTransconductance();
            void eraseInvertingStagesNmosTransconductance();
		private:
			static const Core::NetId OUTPUT_NET_;

			static const Core::NetId SOURCEPMOS_NET_;
			static const Core::NetId SOURCENMOS_NET_;

			static const Core::NetId INTRANSCONDUCTANCE_NET_;
			static const Core::NetId INOUTPUTTRANSCONDUCTANCE_NET_;
			static const Core::NetId INSOURCETRANSCONDUCTANCE_NET_;
			static const Core::NetId INNERTRANSCONDUCTANCE_NET_;

			static const Core::NetId INSTAGEBIAS_NET_;
			static const Core::NetId INOUTPUTSTAGEBIAS_NET_;
			static const Core::NetId INSOURCESTAGEBIAS_NET_;
			static const Core::NetId INNERSTAGEBIAS_NET_;


			std::vector<const Core::Circuit *> invertingStagesPmosTransconductance_;
            std::vector<const Core::Circuit *> invertingStagesNmosTransconductance_;
	};

    }

#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_InvertingStages_H_ */
