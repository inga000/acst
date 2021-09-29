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

#include "Synthesis/incl/Library/HierarchyLevel1/NormalTransistor.h"

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

	const Core::TerminalName NormalTransistor::GATE_TERMINAL_ = Core::TerminalName("Gate");
	const Core::TerminalName NormalTransistor::DRAIN_TERMINAL_ = Core::TerminalName("Drain");
	const Core::TerminalName NormalTransistor::SOURCE_TERMINAL_ = Core::TerminalName("Source");

	const Core::DeviceId NormalTransistor::NORMALTRANSISTOR_PMOS_DEVICEID_ = Core::DeviceName("NormalTransistorPmos").createRootIdentifier();
	const Core::DeviceId NormalTransistor::NORMALTRANSISTOR_NMOS_DEVICEID_ = Core::DeviceName("NormalTransistorNmos").createRootIdentifier();

	const Core::NetId NormalTransistor::GATE_NET_ = Core::NetName("gate").createRootIdentifier();
	const Core::NetId NormalTransistor::DRAIN_NET_ = Core::NetName("drain").createRootIdentifier();
	const Core::NetId NormalTransistor::SOURCE_NET_= Core::NetName("source").createRootIdentifier();


	NormalTransistor::NormalTransistor() :
			normalTransistorNmos_(nullptr),
			normalTransistorPmos_(nullptr)
	{
		initializeNormalTransistorNmos();
		initializeNormalTransistorPmos();

	}

	NormalTransistor::~NormalTransistor()
	{
		delete normalTransistorNmos_;
		delete normalTransistorPmos_;
	}

	void NormalTransistor::initializeNormalTransistorNmos()
	{
		Core::Circuit * circuit = new Core::Circuit;
		Core::TechType techType = Core::TechType::n();
		
		Core::CircuitIds circuitIds; 
		Core::CircuitId normalTranId = circuitIds.normalTransistor(1);
		normalTranId.setTechType(techType);
		circuit->setCircuitIdentifier(normalTranId);

		addNetsToCircuit(*circuit);
		
		

    	Core::Device& device = circuit->addDevice(NORMALTRANSISTOR_NMOS_DEVICEID_);
    	device.setTechType(techType);

    	createPinConnections(*circuit, device);
    	addTerminalsToCircuit(*circuit);

		normalTransistorNmos_= circuit;
	}

	void NormalTransistor::initializeNormalTransistorPmos()
	{
		Core::Circuit * circuit = new Core::Circuit;
		Core::TechType techType = Core::TechType::p();
		
		Core::CircuitIds circuitIds; 
		Core::CircuitId normalTranId = circuitIds.normalTransistor(1);
		normalTranId.setTechType(techType);
		circuit->setCircuitIdentifier(normalTranId);

		addNetsToCircuit(*circuit);
		

    	Core::Device& device = circuit->addDevice(NORMALTRANSISTOR_PMOS_DEVICEID_);
    	device.setTechType(techType);

    	createPinConnections(*circuit, device);
    	addTerminalsToCircuit(*circuit);

		normalTransistorPmos_ = circuit;
	}

	void NormalTransistor::addNetsToCircuit(Core::Circuit & circuit)
	{
		circuit.addNet(GATE_NET_);
		circuit.addNet(DRAIN_NET_);
		circuit.addNet(SOURCE_NET_);
	}

	void NormalTransistor::createPinConnections(Core::Circuit & circuit, Core::Device & device)
	{
		Core::MosfetDeviceType  * deviceType = new Core::MosfetDeviceType;
		device.setDeviceType(*deviceType);

        Core::PinConnections pinConnections = deviceType->getEmptyPinToNetConnections();
	
        Core::PinType drain = deviceType->drain();
	
        pinConnections.addNet(drain, circuit.findNet(DRAIN_NET_));

        Core::PinType gate = deviceType->gate();
        pinConnections.addNet(gate, circuit.findNet(GATE_NET_));

        Core::PinType source = deviceType->source();
        pinConnections.addNet(source, circuit.findNet(SOURCE_NET_));

		Core::PinType bulk = deviceType->bulk();
		pinConnections.addNet(bulk, circuit.findNet(SOURCE_NET_));

        pinConnections.addAndConnectPins(device);
	}

	void NormalTransistor::addTerminalsToCircuit(Core::Circuit & circuit)
	{
        Core::TerminalId terminalIdGate;
        terminalIdGate.setTerminalName(GATE_TERMINAL_);

        Core::Terminal& terminalGate = circuit.addTerminal(terminalIdGate);

        Core::Net& netGate = circuit.findNet(GATE_NET_);
        terminalGate.setNet(netGate);
		terminalGate.setTerminalType(Core::TerminalType::input());

        Core::TerminalId terminalIdDrain;
        terminalIdDrain.setTerminalName(DRAIN_TERMINAL_);

        Core::Terminal& terminalDrain = circuit.addTerminal(terminalIdDrain);

        Core::Net& netDrain = circuit.findNet(DRAIN_NET_);
        terminalDrain.setNet(netDrain);
		terminalDrain.setTerminalType(Core::TerminalType::input());

        Core::TerminalId terminalIdSource;
        terminalIdSource.setTerminalName(SOURCE_TERMINAL_);

        Core::Terminal& terminalSource = circuit.addTerminal(terminalIdSource);
        Core::Net& netSource = circuit.findNet(SOURCE_NET_);
        terminalSource.setNet(netSource);
		terminalSource.setTerminalType(Core::TerminalType::input());

	}

	Core::Instance & NormalTransistor::createNewNormalTransistorInstancePmos(Core::InstanceName instanceName) const
	{
		assert(normalTransistorPmos_ != nullptr);
		Core::Instance * normalTransistor = new Core::Instance;
		normalTransistor->setMaster(getNormalTransistorPmosCircuit());
		createInstanceId(*normalTransistor, instanceName);
		addInstanceTerminals(*normalTransistor);

		return * normalTransistor;
	}

	Core::Instance & NormalTransistor::createNewNormalTransistorInstanceNmos(Core::InstanceName instanceName) const
	{
		assert(normalTransistorNmos_ != nullptr);
		Core::Instance * normalTransistor = new Core::Instance;
		normalTransistor->setMaster(getNormalTransistorNmosCircuit());
		createInstanceId(*normalTransistor, instanceName);
		addInstanceTerminals(*normalTransistor);

		return * normalTransistor;

	}

	std::string NormalTransistor::toStr() const
	{
		std::ostringstream oss;
		oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< Normal Transistors: " << std::endl;
		oss << normalTransistorPmos_->toStr() << std::endl;            Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
            Core::Circuit * flatCircuit= flatCircuitRecursion->createNewFlatCopy(* normalTransistorPmos_);
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
            delete flatCircuitRecursion;

		oss << normalTransistorNmos_->toStr() << std::endl;

		return oss.str();
	}

	const Core::Circuit & NormalTransistor::getNormalTransistorNmosCircuit() const
	{
		assert(normalTransistorNmos_!= nullptr);
		return *normalTransistorNmos_;
	}

	const Core::Circuit & NormalTransistor::getNormalTransistorPmosCircuit() const
	{
		assert(normalTransistorPmos_!= nullptr);
		return * normalTransistorPmos_;
	}


}


