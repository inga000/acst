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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_CURRENTBIASES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_CURRENTBIASES_H_

#include "Synthesis/incl/Library/LibraryItem.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Device/TechType.h"

namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
	class DeviceLevel;

	class CurrentBiases : public LibraryItem
	{
		public:
			CurrentBiases(const DeviceLevel & deviceLevel);
			~CurrentBiases();

			std::vector<const Core::Circuit*>  getAllCurrentBiasesPmos() const;
			std::vector<const Core::Circuit*>  getAllCurrentBiasesNmos() const;

			std::vector<const Core::Circuit*>  getOneTransistorCurrentBiasesPmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorCurrentBiasesPmos() const;
			std::vector<const Core::Circuit*>  getOneTransistorCurrentBiasesNmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorCurrentBiasesNmos() const;
			const Core::Circuit & getNormalTransistorCurrentBias(Core::TechType techType) const;

			std::string toStr() const;

		public:
			static const Core::TerminalName IN_TERMINAL_;
			static const Core::TerminalName SOURCE_TERMINAL_;
			static const Core::TerminalName OUT_TERMINAL_;

			static const Core::TerminalName INNER_TERMINAL_;
			static const Core::TerminalName INOUTPUT_TERMINAL_;
			static const Core::TerminalName INSOURCE_TERMINAL_;

			static const Core::InstanceName SOURCETRANSISTOR_;
			static const Core::InstanceName OUTPUTTRANSISTOR_;

		private:
			void initializeCurrentBiasesPmos(const DeviceLevel & DeviceLevel);
			void initializeCurrentBiasesNmos(const DeviceLevel & DeviceLevel);

			void initializeOneTransistorCurrentBiasesPmos(const DeviceLevel & DeviceLevel, int & num);
        	void initializeTwoTransistorCurrentBiasesPmos(const DeviceLevel & DeviceLevel, int & num);

			void initializeOneTransistorCurrentBiasesNmos(const DeviceLevel & DeviceLevel, int & num);
        	void initializeTwoTransistorCurrentBiasesNmos(const DeviceLevel & DeviceLevel, int & num);

			std::vector<const Core::Circuit*> createOneTransistorCurrentBias(Core::Instance & normalTransistor, int & num);
			std::vector<const Core::Circuit*> createTwoTransistorCurrentBias(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & num);

			const Core::Circuit& createTwoTransistorCircuit(Core::Instance & sourceTransistor, 
							Core::Instance & outputTransistor, int & index);

			void connectInstanceTerminalsOneTransistorCurrentBias(Core::Circuit & circuit, Core::Instance & transistor);
			void connectInstanceTerminalsTwoTransistorCurrentBias(Core::Circuit & circuit, 
							Core::Instance & sourceTransistor, Core::Instance & outputTransistor);

			void eraseOneTransistorCurrentBiasesPmos();
			void eraseTwoTransistorCurrentBiasesPmos();
			void eraseOneTransistorCurrentBiasesNmos();
			void eraseTwoTransistorCurrentBiasesNmos();
			
		private:
			static const Core::InstanceName TRANSISTOR_;

			static const Core::NetId IN_NET_;
			static const Core::NetId SOURCE_NET_;
			static const Core::NetId OUT_NET_;

			static const Core::NetId INNER_NET_;
			static const Core::NetId INOUTPUT_NET_;
			static const Core::NetId INSOURCE_NET_;


			std::vector<const Core::Circuit *> oneTransistorCurrentBiasesPmos_;
			std::vector<const Core::Circuit *> twoTransistorCurrentBiasesPmos_;
			std::vector<const Core::Circuit *> oneTransistorCurrentBiasesNmos_;
			std::vector<const Core::Circuit *> twoTransistorCurrentBiasesNmos_;

	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_CurrentBiases_H_ */
