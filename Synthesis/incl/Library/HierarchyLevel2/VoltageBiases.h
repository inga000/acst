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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_VOLTAGEBIASES_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_VOLTAGEBIASES_H_

#include "Synthesis/incl/Library/LibraryItem.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"


namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
	class DeviceLevel;

	class VoltageBiases : public LibraryItem
	{
		public:
			VoltageBiases(const DeviceLevel & deviceLevel);
			~VoltageBiases();

			std::vector<const Core::Circuit*>  getAllVoltageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getAllVoltageBiasesNmos() const;

			std::vector<const Core::Circuit*>  getOneTransistorVoltageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorVoltageBiasesPmos() const;
			std::vector<const Core::Circuit*>  getOneTransistorVoltageBiasesNmos() const;
			std::vector<const Core::Circuit*>  getTwoTransistorVoltageBiasesNmos() const;

			const Core::Circuit& getDiodeTransistorVoltageBiasNmos() const;
			const Core::Circuit& getDiodeTransistorVoltageBiasPmos() const;

			const Core::Circuit& getTwoDiodeTransistorVoltageBiasNmos() const;
			const Core::Circuit& getTwoDiodeTransistorVoltageBiasPmos() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName IN_TERMINAL_;
			static const Core::TerminalName SOURCE_TERMINAL_;
			static const Core::TerminalName OUT_TERMINAL_;

			static const Core::TerminalName INNER_TERMINAL_;
			static const Core::TerminalName OUTINPUT_TERMINAL_;
			static const Core::TerminalName OUTSOURCE_TERMINAL_;

		private:
			void initializeVoltageBiasesPmos(const DeviceLevel & DeviceLevel);
			void initializeVoltageBiasesNmos(const DeviceLevel & DeviceLevel);

			void initializeOneTransistorVoltageBiasesPmos(const DeviceLevel &deviceLevel, int & index);
        	void initializeTwoTransistorVoltageBiasesPmos(const DeviceLevel &deviceLevel, int & index);

			void initializeOneTransistorVoltageBiasesNmos(const DeviceLevel &deviceLevel, int & index);
        	void initializeTwoTransistorVoltageBiasesNmos(const DeviceLevel &deviceLevel, int & index);

			std::vector<const Core::Circuit*> createOneTransistorVoltageBiases(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & index);
			std::vector<const Core::Circuit*> createTwoTransistorVoltageBiases(const Core::Circuit & normalTransistor, const Core::Circuit & diodeTransistor, int & index);

			Core::Circuit & createOneTransistorCircuit(Core::Instance & instance, int & num);
			std::vector<const Core::Circuit *> createTwoTransistorCircuit(const Core::Circuit & sourceTransitor, const Core::Circuit & outputTransistor, int & num);

			void connectInstanceTerminalsOneTransistorVoltageBias(Core::Circuit & circuit, Core::Instance & transistor);
			void connectInstanceTerminalsTwoTransistorVoltageBias(Core::Circuit & circuit, 
							Core::Instance & sourceTransistor, Core::Instance & outputTransistor);

			void eraseOneTransistorVoltageBiasesPmos();
			void eraseTwoTransistorVoltageBiasesPmos();
			void eraseOneTransistorVoltageBiasesNmos();
			void eraseTwoTransistorVoltageBiasesNmos();

		private:
			static const Core::InstanceName TRANSISTOR_;
			static const Core::InstanceName SOURCETRANSISTOR_;
			static const Core::InstanceName OUTPUTTRANSISTOR_;

			static const Core::NetId IN_NET_;
			static const Core::NetId SOURCE_NET_;
			static const Core::NetId OUT_NET_;

			static const Core::NetId INNER_NET_;
			static const Core::NetId OUTINPUT_NET_;
			static const Core::NetId OUTSOURCE_NET_;

			


			std::vector<const Core::Circuit *> oneTransistorVoltageBiasesPmos_;
			std::vector<const Core::Circuit *> twoTransistorVoltageBiasesPmos_;
			std::vector<const Core::Circuit *> oneTransistorVoltageBiasesNmos_;
			std::vector<const Core::Circuit *> twoTransistorVoltageBiasesNmos_;

	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_VoltageBiases_H_ */
