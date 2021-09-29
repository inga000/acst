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

#include "Synthesis/incl/Library/HierarchyLevel1/Capacitor.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/CapacitorDeviceType.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"



#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {

	const Core::TerminalName Capacitor::MINUS_TERMINAL_ = Core::TerminalName("Minus");
	const Core::TerminalName Capacitor::PLUS_TERMINAL_  = Core::TerminalName("Plus");

	const Core::DeviceId Capacitor::CAPACITOR_DEVICEID_ = Core::DeviceName("Capacitor").createRootIdentifier();

	const Core::NetId Capacitor::MINUS_NET_ = Core::NetName("minus").createRootIdentifier();
	const Core::NetId Capacitor::PLUS_NET_ = Core::NetName("plus").createRootIdentifier();



	Capacitor::Capacitor() :
			capacitor_(nullptr)	
	{
		initializeCapacitor();
	}

	Capacitor::~Capacitor()
	{
		delete capacitor_;
	}

	void Capacitor::initializeCapacitor()
	{
		Core::Circuit * circuit = new Core::Circuit;
		Core::TechType techType = Core::TechType::undefined();

		Core::CircuitIds circuitIds; 
		Core::CircuitId capacitorId = circuitIds.capacitor(1);
		capacitorId.setTechType(techType);
		circuit->setCircuitIdentifier(capacitorId);

		addNetsToCircuit(*circuit);

    	Core::Device& device = circuit->addDevice(CAPACITOR_DEVICEID_);
    	device.setTechType(techType);

    	createPinConnections(*circuit, device);
    	addTerminalsToCircuit(*circuit);
		capacitor_ = circuit;
	}

	void Capacitor::addNetsToCircuit(Core::Circuit & circuit)
	{
		circuit.addNet(PLUS_NET_);
		circuit.addNet(MINUS_NET_);
	}

	void Capacitor::createPinConnections(Core::Circuit & circuit, Core::Device & device)
	{
		Core::CapacitorDeviceType * deviceType = new Core::CapacitorDeviceType;
		device.setDeviceType(*deviceType);

        Core::PinConnections pinConnections = deviceType->getEmptyPinToNetConnections();

	
        Core::PinType minus = deviceType->minus();
        pinConnections.addNet(minus, circuit.findNet(MINUS_NET_));

        Core::PinType plus = deviceType->plus();
        pinConnections.addNet(plus, circuit.findNet(PLUS_NET_));

        pinConnections.addAndConnectPins(device);
	}

	void Capacitor::addTerminalsToCircuit(Core::Circuit & circuit)
	{
        Core::TerminalId terminalIdPlus;
        terminalIdPlus.setTerminalName(PLUS_TERMINAL_);

        std::string terminalNamePlus = terminalIdPlus.getName();
        Core::Terminal& terminalPlus = circuit.addTerminal(terminalIdPlus);

        Core::Net& netPlus = circuit.findNet(PLUS_NET_);
        terminalPlus.setNet(netPlus);
		terminalPlus.setTerminalType(Core::TerminalType::input());

        Core::TerminalId terminalIdMinus;
        terminalIdMinus.setTerminalName(MINUS_TERMINAL_);

        std::string terminalNameMinus = terminalIdMinus.getName();
        Core::Terminal& terminalMinus = circuit.addTerminal(terminalIdMinus);

        Core::Net& netMinus = circuit.findNet(MINUS_NET_);
        terminalMinus.setNet(netMinus);
		terminalMinus.setTerminalType(Core::TerminalType::input());

	}

	Core::Instance & Capacitor::createNewCapacitorInstance(Core::InstanceName instanceName) const
	{
		assert(capacitor_ != nullptr);
		Core::Instance * capacitor = new Core::Instance;
		capacitor->setMaster(getCapacitorCircuit());
		createInstanceId(*capacitor, instanceName);
		addInstanceTerminals(*capacitor);

		return * capacitor;
	}

	std::string Capacitor::toStr() const
	{
		std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Capacitor: " << std::endl;
		oss << capacitor_->toStr() << std::endl;

		return oss.str();
	}

	const Core::Circuit & Capacitor::getCapacitorCircuit() const
	{
		assert(capacitor_!= nullptr);
		return * capacitor_;
	}

}