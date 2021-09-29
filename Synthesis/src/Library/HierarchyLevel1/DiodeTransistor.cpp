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

#include "Synthesis/incl/Library/HierarchyLevel1/DiodeTransistor.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis {

	const Core::TerminalName DiodeTransistor::GATE_TERMINAL_ = Core::TerminalName("Gate");
	const Core::TerminalName DiodeTransistor::DRAIN_TERMINAL_ = Core::TerminalName("Drain");
	const Core::TerminalName DiodeTransistor::SOURCE_TERMINAL_ = Core::TerminalName("Source");

	const Core::DeviceId DiodeTransistor::DIODETRANSISTOR_PMOS_DEVICEID_ = Core::DeviceName("DiodeTransistorPmos").createRootIdentifier();
	const Core::DeviceId DiodeTransistor::DIODETRANSISTOR_NMOS_DEVICEID_ = Core::DeviceName("DiodeTransistorNmos").createRootIdentifier();

	const Core::NetId DiodeTransistor::DRAIN_NET_ = Core::NetName("drain").createRootIdentifier();
	const Core::NetId DiodeTransistor::SOURCE_NET_= Core::NetName("source").createRootIdentifier();



	DiodeTransistor::DiodeTransistor() :
			diodeTransistorNmos_(nullptr),
			diodeTransistorPmos_(nullptr)
	{
		initializeDiodeTransistorNmos();
		initializeDiodeTransistorPmos();

	}

	DiodeTransistor::~DiodeTransistor()
	{
		delete diodeTransistorPmos_;
		delete diodeTransistorNmos_;
	}

	void DiodeTransistor::initializeDiodeTransistorNmos()
	{
		Core::Circuit * circuit = new Core::Circuit;
		
		Core::TechType techType = Core::TechType::n();

		Core::CircuitIds circuitIds;
		Core::CircuitId diodeTranId = circuitIds.diodeTransistor(1);
		diodeTranId.setTechType(techType);
		circuit->setCircuitIdentifier(diodeTranId);

		addNetsToCircuit(*circuit);

		

    	Core::Device& device = circuit->addDevice(DIODETRANSISTOR_NMOS_DEVICEID_);
    	device.setTechType(techType);

    	createPinConnections(*circuit, device);
    	addTerminalsToCircuit(*circuit);

		diodeTransistorNmos_ = circuit;
	}

	void DiodeTransistor::initializeDiodeTransistorPmos()
	{
		Core::Circuit * circuit = new Core::Circuit;
		Core::TechType techType = Core::TechType::p();
		
		
		Core::CircuitIds circuitIds;
		Core::CircuitId diodeTranId = circuitIds.diodeTransistor(1);
		diodeTranId.setTechType(techType);
		circuit->setCircuitIdentifier(diodeTranId);

		addNetsToCircuit(*circuit);

    	Core::Device& device = circuit->addDevice(DIODETRANSISTOR_PMOS_DEVICEID_);
    	device.setTechType(techType);

    	createPinConnections(*circuit, device);
    	addTerminalsToCircuit(*circuit);

		diodeTransistorPmos_ = circuit;
	}

	void DiodeTransistor::addNetsToCircuit(Core::Circuit & circuit)
	{
		circuit.addNet(DRAIN_NET_);
		circuit.addNet(SOURCE_NET_);
	}

	void DiodeTransistor::createPinConnections(Core::Circuit & circuit, Core::Device & device)
	{
		Core::MosfetDeviceType * deviceType = new Core::MosfetDeviceType;
		device.setDeviceType(*deviceType);

        Core::PinConnections pinConnections = deviceType->getEmptyPinToNetConnections();

        Core::PinType drain = deviceType->drain();
        pinConnections.addNet(drain, circuit.findNet(DRAIN_NET_));

        Core::PinType gate = deviceType->gate();
        pinConnections.addNet(gate, circuit.findNet(DRAIN_NET_));

        Core::PinType source = deviceType->source();
        pinConnections.addNet(source, circuit.findNet(SOURCE_NET_));

        Core::PinType bulk = deviceType->bulk();
        pinConnections.addNet(bulk, circuit.findNet(SOURCE_NET_));

        pinConnections.addAndConnectPins(device);
		
	}

	void DiodeTransistor::addTerminalsToCircuit(Core::Circuit & circuit)
	{
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(GATE_TERMINAL_);

        std::string terminalNameGate = terminalIdGate.getName();
        Core::Terminal& terminalGate = circuit.addTerminal(terminalIdGate);
       
        Core::Net& netGate = circuit.findNet(DRAIN_NET_);
        terminalGate.setNet(netGate);
		terminalGate.setTerminalType(Core::TerminalType::input());


        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(DRAIN_TERMINAL_);

        std::string terminalNameDrain = terminalIdDrain.getName();
        Core::Terminal& terminalDrain = circuit.addTerminal(terminalIdDrain);

        Core::NetId netIdDrain = Core::NetName(terminalNameDrain).createRootIdentifier();
        Core::Net& netDrain = circuit.findNet(DRAIN_NET_);
        terminalDrain.setNet(netDrain);
		terminalDrain.setTerminalType(Core::TerminalType::input());

        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(SOURCE_TERMINAL_);

        std::string terminalNameSource = terminalIdSource.getName();
        Core::Terminal& terminalSource = circuit.addTerminal(terminalIdSource);

        Core::Net& netSource = circuit.findNet(SOURCE_NET_);
        terminalSource.setNet(netSource);
		terminalSource.setTerminalType(Core::TerminalType::input());

	}

	Core::Instance & DiodeTransistor::createNewDiodeTransistorInstancePmos(Core::InstanceName instanceName) const
	{
		assert(diodeTransistorPmos_ != nullptr);
		Core::Instance * diodeTransistor = new Core::Instance;
		diodeTransistor->setMaster(getDiodeTransistorPmosCircuit());
		addInstanceTerminals(*diodeTransistor);
		createInstanceId(*diodeTransistor, instanceName);

		return * diodeTransistor;
	}

	Core::Instance & DiodeTransistor::createNewDiodeTransistorInstanceNmos(Core::InstanceName instanceName) const
	{
		assert(diodeTransistorNmos_ != nullptr);
		Core::Instance * diodeTransistor = new Core::Instance;
		diodeTransistor->setMaster(getDiodeTransistorNmosCircuit());
		addInstanceTerminals(*diodeTransistor);
		createInstanceId(*diodeTransistor, instanceName);


		return * diodeTransistor;

	}

	std::string DiodeTransistor::toStr() const
	{
		std::ostringstream oss;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Diode Transistors: " << std::endl;
		oss << diodeTransistorPmos_->toStr() << std::endl;
		Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        Core::Circuit * flatCircuit= flatCircuitRecursion->createNewFlatCopy(* diodeTransistorPmos_);
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
            delete flatCircuitRecursion;
		oss << diodeTransistorNmos_->toStr() << std::endl;

		return oss.str();
	}

	const Core::Circuit & DiodeTransistor::getDiodeTransistorNmosCircuit() const
	{
		assert(diodeTransistorNmos_!= nullptr);
		return * diodeTransistorNmos_;
	}

		const Core::Circuit & DiodeTransistor::getDiodeTransistorPmosCircuit() const
	{
		assert(diodeTransistorPmos_!= nullptr);
		return * diodeTransistorPmos_;
	}

}