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

#ifndef SYNTHESIS_INCL_LIBRARY_LIBRARYITEM_H_
#define SYNTHESIS_INCL_LIBRARY_LIBRARYITEM_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"

#include <vector>


namespace Core
{
	class Instance;
	class Circuit;
}

namespace Synthesis
{

	class LibraryItem : public Core::Object
	{
	public:
		LibraryItem();
		~LibraryItem();

		Core::Instance & createInstance(const Core::Circuit & circuit, Core::InstanceName instanceName) const;
		Core::InstanceId createInstanceId(Core::InstanceName instanceName) const;

		virtual std::string toStr() const = 0;
	protected:
			void createInstanceId(Core::Instance & instance, Core::InstanceName instanceName) const;
			Core::TerminalId createTerminalId(Core::TerminalName terminalName) const;
			
			void addInstanceTerminals(Core::Instance & instance) const;
			void addNetsToCircuit(Core::Circuit & circuit, std::vector<Core::NetId> netNames)  const;
			void addTerminalsToCircuit(Core::Circuit & circuit, std::map<Core::TerminalName, Core::NetId> terminalToNetMap) const;
			
			void connectInstanceTerminal(Core::Circuit & circuit, Core::Instance & instance, Core::TerminalName terminalName, Core::NetId netId) const;
			void addAdditionalTerminalToCircuit(Core::Circuit & circuit, Core::NetId netName, Core::TerminalName terminalName);

			bool transistorsWithSameGateNetAreNotSourceConnected(const Core::Circuit & circuit,std::vector<Core::NetId> gateNets) const;
			bool transistorsAreSourceConnected(const Core::Circuit & circuit, std::vector<Core::DeviceId > devices ) const;

			std::vector<Core::DeviceId> getGateConnectedFlatDevices(const Core::Circuit & circuit, std::vector<Core::NetId> gateNets) const;
			std::vector<Core::DeviceId> getDeviceNamesOfFlatCircuit(const Core::Circuit & circuit) const;

			bool isSingleDiodeTransistor(const Core::Circuit & circuit) const;
			bool bothTransistorStacksAreVoltageBiases(const Core::Instance & loadPart) const;
			bool hasGCC(const Core::Instance & load) const;
			bool sourceTransistorIsDiodeTransistor(const Core::Circuit & transistorStack) const;
			bool outputTransistorIsDiodeTransistor(const Core::Circuit & transistorStack) const;
	};

}
#endif /* SYNTHESIS_INCL_LIBRARY_LIBRARYITEM_H_ */
