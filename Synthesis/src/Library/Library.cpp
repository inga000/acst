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

#include "Synthesis/incl/Library/Library.h"
#include "Synthesis/incl/Library/Circuit.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/CapacitorDeviceType.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceName.h"

#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "Partitioning/incl/Results/Result.h"

#include <string>
#include <sstream>

namespace Synthesis
{
		Library::Library() :
				structRecLibrary_(nullptr)
		{

		}

		Library::~Library()
		{
			eraseFirstStages();
			eraseSymmetricalFirstStages();
			eraseSecondStages();
			eraseComplementarySecondStages();


		}

		void Library::addFirstStage(Core::Circuit & firstStage)
		{
			firstStages_.push_back(&firstStage);
		}

		void Library::addSecondStage(Core::Circuit & secondStage)
		{
			secondStages_.push_back(&secondStage);
		}

		void Library::addSymmetricalFirstStage(Core::Circuit & firstStage)
		{
			symmetricalFirstStages_.push_back(&firstStage);
		}

		void Library::addComplementarySecondStage(Core::Circuit & secondStage)
		{
			complementarySecondStages_.push_back(&secondStage);
		}

		void Library::setStructRecLibrary(StructRec::Library& library)
		{
			structRecLibrary_ = & library;
		}

		StructRec::Library & Library::getStructRecLibrary() const
		{
			assert(structRecLibrary_ != nullptr);
			return * structRecLibrary_;
		}

		std::vector<Circuit*> Library::createOneStageOpAmps() const
		{
			std::vector<Circuit*> oneStageOpAmps;
			for(auto & firstStage : firstStages_)
			{
				Circuit * stage = new Circuit;
				Core::Circuit & oneStageOpAmp = firstStage->clone();
				addLoadCapacityToCircuit(oneStageOpAmp, Core::NetName("outFirstStage1").createRootIdentifier());
				stage->initialize(oneStageOpAmp, getStructRecLibrary());
				oneStageOpAmps.push_back(stage);
			}

			return oneStageOpAmps;
		}

		std::vector<Circuit*> Library::createSymmetricalOpAmps() const
		{
			std::vector<Circuit*> symOpAmps;

			int num =1;
			for(auto & firstStage : symmetricalFirstStages_)
			{
				for(auto & secondStage : secondStages_)
				{
					for(auto &complementarySecondStage : complementarySecondStages_)
					{
						Core::Circuit * symOpAmp = new Core::Circuit;

						std::ostringstream name;
						name << "SymOpAmp";
						Core::CircuitId circuitId = Core::CircuitName(name.str()).createIdentifier();
						circuitId.setId(num);
						circuitId.setTechType(Core::TechType::undefined());
						symOpAmp->setCircuitIdentifier(circuitId);

						addStageToNewCircuit(*symOpAmp, *firstStage);
						addSecondStageToSymOpAmp(*symOpAmp, *secondStage);
						addStageToNewCircuit(*symOpAmp, *complementarySecondStage);
						addLoadCapacityToCircuit(*symOpAmp, Core::NetName("outSecondStage").createRootIdentifier());

						Circuit * symOpAmpCirc = new Circuit;
						symOpAmpCirc->initialize(*symOpAmp, getStructRecLibrary());

						if(isValidSymOpAmp(*symOpAmpCirc))
						{

							symOpAmps.push_back(symOpAmpCirc);
							num++;
						}

					}
				}
			}

			return symOpAmps;
		}

		std::vector<Circuit*> Library::createTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const
		{
			std::vector<Circuit*> twoStageOpAmps;

			int num =1;
			for(auto & firstStage : oneStageOpAmps)
			{
				for(auto & secondStage : secondStages_)
				{
					Core::Circuit * twoStageOpAmp = new Core::Circuit;

					std::ostringstream name;
					name << "TwoStageOpAmp";
					Core::CircuitId circuitId = Core::CircuitName(name.str()).createIdentifier();
					circuitId.setId(num);
					circuitId.setTechType(Core::TechType::undefined());
					twoStageOpAmp->setCircuitIdentifier(circuitId);

					addStageToNewCircuit(*twoStageOpAmp, firstStage->getFlatCircuit());
					addStageToNewCircuit(*twoStageOpAmp, *secondStage);
					addLoadCapacityToCircuit(*twoStageOpAmp, Core::NetName("outSecondStage").createRootIdentifier());

					Circuit * twoStageOpAmpCirc = new Circuit;
					twoStageOpAmpCirc->initialize(*twoStageOpAmp, getStructRecLibrary());

					twoStageOpAmps.push_back(twoStageOpAmpCirc);
					num++;

				}
			}

			return twoStageOpAmps;
		}

		bool Library::isValidSymOpAmp(Circuit &symOpAmp) const
		{
			const Partitioning::Result & partRes = symOpAmp.getPartitioningResult();

			if(partRes.hasSecondarySecondStage())
			{
				Core::TechType techTypeFirstStageLoad = (**(**partRes.getFirstStage().getLoadPart().begin()).getMainStructures().begin()).getTechType();
				Core::TechType techTypePrimarySecondStageTrans = (**partRes.getPrimarySecondStage().getMainStructures().begin()).getTechType();
				Core::TechType techTypeSecondarySecondStageTrans = (**partRes.getSecondarySecondStage().getMainStructures().begin()).getTechType();

				std::vector<Partitioning::Component*> primarySecondStageComps = partRes.getBelongingComponents(partRes.getPrimarySecondStage());
				std::vector<Partitioning::Component*> secondarySecondStageComps = partRes.getBelongingComponents(partRes.getSecondarySecondStage());

				return techTypeFirstStageLoad == techTypePrimarySecondStageTrans && techTypePrimarySecondStageTrans == techTypePrimarySecondStageTrans
						&& primarySecondStageComps.size() ==secondarySecondStageComps.size();
			}
			else
			{
				return false;
			}
 		}

		void Library::addStageToNewCircuit(Core::Circuit & newCircuit, const Core::Circuit & stage) const
		{
	        for(auto & net : stage.getNets())
	        {
	        	Core::Net & newNet = net->clone();
	        	newCircuit.addNet(newNet);
	        }

	        for(auto & device : stage.getDevices())
	        {
				if(device->getIdentifier() != Core::DeviceName("cl").createRootIdentifier())
				{
					Core::Device & newDevice = device->clone();
					newCircuit.addDevice(newDevice);

					Core::PinConnections pinConnections = newDevice.getDeviceType().getEmptyPinToNetConnections();
					std::vector<Core::PinType> pinTypes = newDevice.getDeviceType().getPinTypes();
					for(auto& it : pinTypes)
					{
						Core::PinType pinType = it;
						Core::NetId netId = device->findNet(pinType).getIdentifier();
						pinConnections.addNet(pinType,newCircuit.findNet(netId));
					}

					pinConnections.addAndConnectPins(newDevice);
				}
	        }

		}

		void Library::addSecondStageToSymOpAmp(Core::Circuit & symOpAmp, Core::Circuit & secondStage) const
		{

	        for(auto & net : secondStage.getNets())
	        {
	        	Core::Net & newNet = net->clone();
	        	symOpAmp.addNet(newNet);
	        }

	        for(auto & device : secondStage.getDevices())
	        {
				if(device->getDeviceType().getName() != Core::DeviceTypeName("Capacitor"))
				{
					Core::Device & newDevice = device->clone();
					symOpAmp.addDevice(newDevice);

					Core::PinConnections pinConnections = newDevice.getDeviceType().getEmptyPinToNetConnections();
					std::vector<Core::PinType> pinTypes = newDevice.getDeviceType().getPinTypes();
					for(auto& it : pinTypes)
					{
						Core::PinType pinType = it;
						Core::NetId netId = device->findNet(pinType).getIdentifier();
						pinConnections.addNet(pinType,symOpAmp.findNet(netId));
					}

					pinConnections.addAndConnectPins(newDevice);
				}
	        }

		}


		std::string Library::toStr() const
		{
			std::ostringstream stream;
			stream << "<<<<<<<<<<<<<< LIBRARY >>>>>>>>>>>>>> " << std::endl;
			printFirstStages(stream);
			printSymmetricalFirstStages(stream);
			printSecondStages(stream);
			printComplementarySecondStages(stream);
			return stream.str();
		}

		void Library::addLoadCapacityToCircuit(Core::Circuit & circuit, Core::NetId outputNetStage) const
		{
        	Core::DeviceId deviceId = Core::DeviceName("cl").createRootIdentifier();

        	Core::TechType techType = Core::TechType::undefined();
        	Core::CapacitorDeviceType * deviceType = new Core::CapacitorDeviceType;

        	Core::PinConnections pinConnections = deviceType->getEmptyPinToNetConnections();

        	Core::PinType plus = deviceType->plus();
        	pinConnections.addNet(plus, circuit.findNet(outputNetStage));

        	Core::PinType minus = deviceType->minus();
        	Core::NetName groundNet("gnd!");
        	Core::NetId groundNetId = groundNet.createRootIdentifier();

        	pinConnections.addNet(minus, circuit.findNet(groundNetId));


        	Core::Device& device = circuit.addDevice(deviceId);
        	device.setDeviceType(*deviceType);
        	device.setTechType(techType);


        	pinConnections.addAndConnectPins(device);
		}

		void Library::printFirstStages(std::ostream & stream) const
		{

			stream << "<<<<<<<<<<<<<<<<< FIRST STAGES >>>>>>>>>>>>>>>>>>>>: " << std::endl;
			int id = 1;
			for(auto & stage: firstStages_)
			{
				stream << "STAGE[" << id << "]:" << std::endl;
				stream << stage->toStr() << std::endl;
				id++;
			}
		}

		void Library::printSecondStages(std::ostream & stream) const
		{
			stream << "<<<<<<<<<<<<<<<<< SECOND STAGES >>>>>>>>>>>>>>>>>>>>: " << std::endl;
			int id = 1;
			for(auto & stage: secondStages_)
			{
				stream << "STAGE[" << id << "]:" << std::endl;
				stream << stage->toStr() << std::endl;
				id++;
			}
		}

		void Library::printSymmetricalFirstStages(std::ostream & stream) const
		{

			stream << "<<<<<<<<<<<<<<<<< SYMMETRICAL FIRST STAGES >>>>>>>>>>>>>>>>>>>>: " << std::endl;
			int id = 1;
			for(auto & stage: symmetricalFirstStages_)
			{
				stream << "STAGE[" << id << "]:" << std::endl;
				stream << stage->toStr() << std::endl;
				id++;
			}
		}

		void Library::printComplementarySecondStages(std::ostream & stream) const
		{
			stream << "<<<<<<<<<<<<<<<<< COMPLEMENTARY SECOND STAGES >>>>>>>>>>>>>>>>>>>>: " << std::endl;
			int id = 1;
			for(auto & stage: complementarySecondStages_)
			{
				stream << "STAGE[" << id << "]:" << std::endl;
				stream << stage->toStr() << std::endl;
				id++;
			}
		}

		void Library::eraseFirstStages()
		{
			for(auto & firstStage : firstStages_)
			{
				delete firstStage;
			}
		}

		void Library::eraseSymmetricalFirstStages()
		{
			for(auto & firstStage : symmetricalFirstStages_)
			{
				delete firstStage;
			}
		}

		void Library::eraseSecondStages()
		{
			for(auto & secondStage : secondStages_)
			{
				delete secondStage;
			}
		}

		void Library::eraseComplementarySecondStages()
		{
			for(auto & secondStage : complementarySecondStages_)
			{
				delete secondStage;
			}
		}

}
