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

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/Results/Result.h"

#include "Partitioning/incl/Results/Result.h"

#include "Synthesis/incl/Results/HSPICEOutputFile.h"
#include "Synthesis/incl/Library/Circuit.h"

#include "Control/incl/File/OutputFile.h"

#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/CapacitorDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/ResistorDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis
{

	const std::string HSPICEOutputFile::PATH_NOT_SET_ = "";
	const std::string HSPICEOutputFile::ID_NOT_SET_ = "";


	HSPICEOutputFile::HSPICEOutputFile() :
			id_(ID_NOT_SET_),
			path_(PATH_NOT_SET_),
			outputFile_(nullptr),
			usesHspiceLibrary_(false)
	{
	}


	HSPICEOutputFile::~HSPICEOutputFile()
	{
		delete outputFile_;
	}

	void HSPICEOutputFile::setId(std::string id)
	{
		id_ = id;
	}

	void HSPICEOutputFile::setPath(std::string filepath)
	{
		path_ = filepath;
	}
	void HSPICEOutputFile::setUsesHspiceLibrary(bool usesHSPICELibrary)
	{
		usesHspiceLibrary_ = usesHSPICELibrary;
	}


	void HSPICEOutputFile::write(const Circuit & circuit)
	{
		outputFile_ = new Control::OutputFile;
		logDebug("Path the out put file is written to " << getPath());
		outputFile_->setPath(getPath());
		outputFile_->open();

		writeName();
		writeCircuit(circuit);
		writeExpectedPerformance(circuit.getSizingResult());
		writeCurrents(circuit.getSizingResult());
		writeVoltages(circuit.getSizingResult());

		getOutputFile() <<".END";
		outputFile_->close();
	}

	void HSPICEOutputFile::write(const Core::Circuit & circuit, const Partitioning::Result * partitioningResult)
	{
		outputFile_ = new Control::OutputFile;
		logDebug("Path the out put file is written to " << getPath());

		outputFile_->setPath(getPath());
		outputFile_->open();

		getOutputFile() << ".suckt  " << getId();

		if(circuit.hasTerminals())
		{
			for(auto & terminal : circuit.findTerminals())
			{
				getOutputFile() << " " << terminal->getNetId().getName();
			}
		}
		getOutputFile() << "\n";

		logDebug("Write Devices");
		writeDevices(circuit, partitioningResult);
		getOutputFile() << ".end "  << getId() << "\n\n";
		outputFile_->close();
	}

	std::string HSPICEOutputFile::getPath() const
	{
		assert(path_ != PATH_NOT_SET_);
		return path_;
	}

	std::string HSPICEOutputFile::getId() const
	{
		assert(id_ != ID_NOT_SET_);
		return id_;

	}


	bool HSPICEOutputFile::usesHspiceLibrary() const
	{
		return usesHspiceLibrary_;
	}

	Control::OutputFile & HSPICEOutputFile::getOutputFile()
	{
		assert(outputFile_ != nullptr);
		return * outputFile_;
	}

	void HSPICEOutputFile::writeName()
	{
		getOutputFile() << "** Name: " << getId() << "\n\n";
	}

	void HSPICEOutputFile::writeCircuit(const Circuit & circuit)
	{
		getOutputFile() << ".MACRO " << getId();
		if(circuit.getFlatCircuit().hasTerminals())
		{
			for(auto & terminal : circuit.getFlatCircuit().findTerminals())
			{
				getOutputFile() << " " << terminal->getNetId().getName();
			}
		}
		else
		{
			std::vector<Core::NetId> biasNets = circuit.getPartitioningResult().findBiasNets();
			 getOutputFile() << " gnd vdd in1 in2 "
					<< circuit.getCircuitInformation().getCircuitParameter().getOutputPin().getNetId().getName();
			for(auto & biasNet : biasNets)
			{
				getOutputFile() << " " << biasNet.getName();
			}

			if(circuit.getPartitioningResult().hasSecondStage())
			{
				Partitioning::TransconductancePart & primarySecondStage = circuit.getPartitioningResult().getPrimarySecondStage();
				if(circuit.getPartitioningResult().getBelongingComponents(primarySecondStage).size() == 2)
				{
					Core::NetId gateNet = findGateNetOfSecondTransconductanceDevice(circuit.getPartitioningResult(),primarySecondStage);

					getOutputFile() << " " << gateNet.getName();
				}
			}
		}
		getOutputFile() << "\n";
		int tranIndex = 1;
		for(auto & transistor : circuit.getSizingResult().getAllTransistors())
		{
			writeTransistor(circuit.getPartitioningResult(), *transistor, tranIndex);
			tranIndex ++;
		}

		int twoPortIndex = 1;
		for(auto & twoPort : circuit.getSizingResult().getAllTwoPorts())
		{
			writeTwoPort(circuit.getPartitioningResult(), *twoPort, twoPortIndex);
			twoPortIndex++;
		}

		getOutputFile() << ".EOM "  << getId() << "\n\n";
	}

	void HSPICEOutputFile::writeTransistor(const Partitioning::Result & partitioningResult, const Partitioning::Transistor & transistor, int tranIndex)
	{
		const Core::Device & device = **transistor.getArray().getDevices().begin();

		std::ostringstream deviceInformation;
		deviceInformation << createDeviceInformation(partitioningResult, device);
		getOutputFile() << "m" << deviceInformation.str() << tranIndex << " ";

        Core::MosfetDeviceType mosfetDeviceType;


        const Core::Pin& drainPin = device.findPin(mosfetDeviceType.drain());
        const Core::Pin& gatePin = device.findPin(mosfetDeviceType.gate());
        const Core::Pin& sourcePin = device.findPin(mosfetDeviceType.source());
        const Core::Pin& bulkPin = device.findPin(mosfetDeviceType.bulk());

        std::string drainNetName = drainPin.getNet().getIdentifier().getFlatName();
        std::string gateNetName = gatePin.getNet().getIdentifier().getFlatName();
        std::string sourceNetName = sourcePin.getNet().getIdentifier().getFlatName();
        std::string bulkNetName = bulkPin.getNet().getIdentifier().getFlatName();

        getOutputFile() << drainNetName << " ";
        getOutputFile() << gateNetName << " ";
        getOutputFile() << sourceNetName << " ";
        getOutputFile() << bulkNetName << " ";

        Core::TechType techType = device.getTechType();
        if(techType == Core::TechType::n()) {
            getOutputFile() << "nmos4";
        } else if(techType == Core::TechType::p()) {
            getOutputFile() << "pmos4";
        } else {
            assert(false, "Wrong TechType!");
        }

        std::ostringstream length;
        length << transistor.getLength();

        std::ostringstream width;
        width << transistor.getWidth();

        getOutputFile() << " L=" << length.str() << "e-6";
        getOutputFile() << " W=" << width.str() << "e-6";

		getOutputFile() << "\n";

	}

	void HSPICEOutputFile::writeTwoPort(const Partitioning::Result & partitioningResult, const Partitioning::TwoPort & twoPort, int twoPortIndex)
	{
		const Core::Device & device = **twoPort.getArray().getDevices().begin();

		std::ostringstream deviceInformation;
		deviceInformation << createDeviceInformation(partitioningResult, device);
		getOutputFile() << "m" << deviceInformation.str() << twoPortIndex << " ";

		if(device.getDeviceType() == Core::DeviceTypeNames::Capacitor)
		{
			Core::CapacitorDeviceType capacitorDeviceType;

            const Core::Pin& plusPin = device.findPin(capacitorDeviceType.plus());
            const Core::Pin& minusPin = device.findPin(capacitorDeviceType.minus());

            std::string plusNetName = plusPin.getNet().getIdentifier().getFlatName();
            std::string minusNetName = minusPin.getNet().getIdentifier().getFlatName();

            getOutputFile() << plusNetName << " ";
            getOutputFile() << minusNetName << " ";

            std::ostringstream value;
            value << twoPort.getValue();

            getOutputFile() << value.str() << "e-12";

		}
		else if(device.getDeviceType()  == Core::DeviceTypeNames::Resistor)
		{
            Core::ResistorDeviceType resistorDeviceType;

            const Core::Pin& plusPin = device.findPin(resistorDeviceType.plus());
            const Core::Pin& minusPin = device.findPin(resistorDeviceType.minus());

            std::string plusNetName = plusPin.getNet().getIdentifier().getFlatName();
            std::string minusNetName = minusPin.getNet().getIdentifier().getFlatName();

            getOutputFile() << plusNetName << " ";
            getOutputFile() << minusNetName << " ";
            std::ostringstream value;
            value << twoPort.getValue();

            getOutputFile() << value.str() << "e-12";
		}


		getOutputFile() << "\n";
	}

	void HSPICEOutputFile::writeDevices(const Core::Circuit & circuit, const Partitioning::Result * partitioningResult)
	{
		int tranIndex = 1;
		int capIndex = 1;
		int resIndex = 1;
		for(auto & it : circuit.getDevices())
		{

			const Core::Device & device = *it;

			if(device.getDeviceType() == Core::DeviceTypeNames::Mosfet)
			{
				Core::MosfetDeviceType mosfetDeviceType;

				std::ostringstream deviceInformation;
				if(partitioningResult != nullptr)
				{
					deviceInformation << createDeviceInformation(*partitioningResult, device);
				}
				getOutputFile() << "m" << deviceInformation.str() << tranIndex << " ";

				const Core::Pin& drainPin = device.findPin(mosfetDeviceType.drain());
				const Core::Pin& gatePin = device.findPin(mosfetDeviceType.gate());
				const Core::Pin& sourcePin = device.findPin(mosfetDeviceType.source());
				const Core::Pin& bulkPin = device.findPin(mosfetDeviceType.bulk());

				std::string drainNetName = drainPin.getNet().getIdentifier().getFlatName();
				std::string gateNetName = gatePin.getNet().getIdentifier().getFlatName();
				std::string sourceNetName = sourcePin.getNet().getIdentifier().getFlatName();
				std::string bulkNetName = bulkPin.getNet().getIdentifier().getFlatName();

				getOutputFile() << drainNetName << " ";
				getOutputFile() << gateNetName << " ";
				getOutputFile() << sourceNetName << " ";
				getOutputFile() << bulkNetName << " ";

				Core::TechType techType = device.getTechType();
				if(techType == Core::TechType::n())
				{
					getOutputFile() << "nmos";
				}
				else if(techType == Core::TechType::p())
				{
					getOutputFile() << "pmos";
				}
				else
				{
					assert(false, "Wrong TechType!");
				}
				tranIndex++;
			}
			else if(device.getDeviceType() == Core::DeviceTypeNames::Capacitor)
			{
				Core::CapacitorDeviceType capacitorDeviceType;

				std::ostringstream deviceInformation;
				if(partitioningResult != nullptr)
				{
					deviceInformation << createDeviceInformation(*partitioningResult, device);
				}
				getOutputFile() << "c" << deviceInformation.str() << capIndex << " ";

				const Core::Pin& plusPin = device.findPin(capacitorDeviceType.plus());
			    const Core::Pin& minusPin = device.findPin(capacitorDeviceType.minus());

			    std::string plusNetName = plusPin.getNet().getIdentifier().getFlatName();
			    std::string minusNetName = minusPin.getNet().getIdentifier().getFlatName();

			    getOutputFile() << plusNetName << " ";
			    getOutputFile() << minusNetName << " ";

			    capIndex++;
			}
			else if(device.getDeviceType()  == Core::DeviceTypeNames::Resistor)
			{
				Core::ResistorDeviceType resistorDeviceType;

				std::ostringstream deviceInformation;
				if(partitioningResult != nullptr)
				{
					deviceInformation << createDeviceInformation(*partitioningResult, device);
				}
				getOutputFile() << "r" << deviceInformation.str() << resIndex << " ";

				const Core::Pin& plusPin = device.findPin(resistorDeviceType.plus());
				const Core::Pin& minusPin = device.findPin(resistorDeviceType.minus());

				std::string plusNetName = plusPin.getNet().getIdentifier().getFlatName();
				std::string minusNetName = minusPin.getNet().getIdentifier().getFlatName();

				getOutputFile() << plusNetName << " ";
				getOutputFile() << minusNetName << " ";

			}
			getOutputFile() << "\n";

		}

	}

	void HSPICEOutputFile::writeExpectedPerformance(const AutomaticSizing::Result & sizingResult)
	{
		getOutputFile() << "** Expected Performance Values: " << "\n";

		std::ostringstream gainString;
		gainString << sizingResult.getGain();
		getOutputFile() << "** Gain: "<< gainString.str() << " dB" << "\n";

		float normedPower = sizingResult.getPower() * 1000;
		std::ostringstream powerString;
		powerString << normedPower;
		getOutputFile() << "** Power consumption: " << powerString.str() << " mW" << "\n";

		std::ostringstream areaString;
		areaString << sizingResult.getArea();
		getOutputFile()  << "** Area: " << areaString.str() << " (mu_m)^2" << "\n";

		float normedTransitFrequency = sizingResult.getTransitFrequency()/1000000;
		std::ostringstream transitFrequencyString;
		transitFrequencyString << normedTransitFrequency;
		getOutputFile() << "** Transit frequency: " << transitFrequencyString.str() << " MHz" << "\n";

		float normedTransitFrequencyWithErrorFactor = sizingResult.getTransitFrequencyWithErrorFactor()/1000000;
		std::ostringstream transitFrequencyWithErrorFactorString;
		transitFrequencyWithErrorFactorString << normedTransitFrequencyWithErrorFactor;
		getOutputFile() << "** Transit frequency with error factor: " << transitFrequencyWithErrorFactorString.str() << " MHz" << "\n";

		float normedSlewRate = sizingResult.getSlewRate()/ 1000000;
		std::ostringstream slewRateString;
		slewRateString << normedSlewRate;
		getOutputFile() << "** Slew rate: " << slewRateString.str() << " V/mu_s" << "\n";

		float normedPhaseMargin = sizingResult.getPhaseMargin() * 180/3.14159265;
		std::ostringstream phaseMarginString;
		phaseMarginString << normedPhaseMargin;
		getOutputFile() << "** Phase margin: " << phaseMarginString.str()  << "°"  << "\n";

		std::ostringstream CMRRString;
		CMRRString << sizingResult.getCMRR();
		getOutputFile() << "** CMRR: " << CMRRString.str() << " dB" << "\n";

		if(sizingResult.hasNegPSRR())
		{
			std::ostringstream negPSRRString;
			negPSRRString << sizingResult.getNegPSRR();
			getOutputFile() << "** negPSRR: " << negPSRRString.str() << " dB" << "\n";
		}

		if(sizingResult.hasPosPSRR())
		{
			std::ostringstream posPSRRString;
			posPSRRString << sizingResult.getPosPSRR();
			getOutputFile() << "** posPSRR: " << posPSRRString.str() << " dB" << "\n";
		}

		std::ostringstream voutMaxString;
		voutMaxString << sizingResult.getMaxOutputVoltage();
		getOutputFile() << "** VoutMax: " << voutMaxString.str() << " V" << "\n";

		std::ostringstream voutMinString;
		voutMinString << sizingResult.getMinOutputVoltage();
		getOutputFile() << "** VoutMin: " << voutMinString.str() <<  " V" <<"\n";

		if(sizingResult.hasMaxCommonModeInputVoltage())
		{
			std::ostringstream vcmMaxString;
			vcmMaxString << sizingResult.getMaxCommonModeInputVoltage();
			getOutputFile() << "** VcmMax: " << vcmMaxString.str() << " V" << "\n";

			std::ostringstream vcmMinString;
			vcmMinString << sizingResult.getMinCommonModeInputVoltage();
			getOutputFile() << "** VcmMin: " << vcmMinString.str() << " V" << "\n";
		}


		getOutputFile() << "\n\n";
	}

	void HSPICEOutputFile::writeCurrents(const AutomaticSizing::Result & sizingResult)
	{
		getOutputFile() << "** Expected Currents: " << "\n";
		for(auto & it : sizingResult.getCurrentMap())
		{
			Core::DeviceId name = it.first;
			float current = it.second;
			float normedCurrent = current /pow(10,-6);
			std::ostringstream currentString;
			currentString << normedCurrent;

			getOutputFile() << "** " << name.getDeviceName() << ": "<< currentString.str() << " muA" << "\n";
		}

		getOutputFile() << "\n\n";
	}

	void HSPICEOutputFile::writeVoltages(const AutomaticSizing::Result & sizingResult)
	{

		getOutputFile() << "** Expected Voltages: " << "\n";
		for(auto & it : sizingResult.getVoltageMap())
		{
			Core::NetId name = it.first;
			float voltage = it.second;
			std::ostringstream voltageString;
			voltageString << voltage;

			getOutputFile() << "** " << name.getName() << ": "<< voltageString.str() << "  V" << "\n";
		}

		getOutputFile() << "\n\n";

	}

	Core::NetId HSPICEOutputFile::findGateNetOfSecondTransconductanceDevice(const Partitioning::Result & partitioningResult,
			Partitioning::TransconductancePart & secondStage) const
	{
		Partitioning::Component * outputComp = nullptr;

		for(auto & comp : partitioningResult.getBelongingComponents(secondStage) )
		{
			if(outputComp == nullptr
					&& !comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source")).isSupply() )
			{
				outputComp = comp;
			}
		}
		Core::PinType gatePin = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName( "Gate"));
		const Core::Device & device = **outputComp->getArray().getDevices().begin();

		return device.findNet(gatePin).getIdentifier();
	}

	std::string HSPICEOutputFile::createDeviceInformation(const Partitioning::Result & partitioningResult, const Core::Device & device) const
	{
		std::ostringstream deviceInformation;

		const Partitioning::Part & part = partitioningResult.getPart(device);
		std::string circuitTypeInformation = createCircuitTypeInformation(partitioningResult);
//		deviceInformation << circuitTypeInformation;

		if(part.isTransconductancePart())
		{
			const Partitioning::TransconductancePart& transconductor = static_cast<const Partitioning::TransconductancePart&>(part);
			deviceInformation << createTransconductorInformation(partitioningResult, transconductor);
		}
		else if(part.isBiasPart())
		{
			const Partitioning::BiasPart& bias = static_cast<const Partitioning::BiasPart&>(part);
			deviceInformation << createBiasInformation(partitioningResult, bias);
		}
		else if(part.isLoadPart())
		{
			const Partitioning::LoadPart& load = static_cast<const Partitioning::LoadPart&>(part);
			deviceInformation << createLoadInformation(partitioningResult, load);
		}
		else if(part.isCapacitancePart())
		{
			const Partitioning::CapacitancePart& capacitor = static_cast<const Partitioning::CapacitancePart&>(part);
			deviceInformation << createCapacitorInformation(partitioningResult, capacitor);
		}
		else if(part.isResistorPart())
		{
			const Partitioning::ResistorPart& resistor = static_cast<const Partitioning::ResistorPart&>(part);
			deviceInformation << createResistorInformation(partitioningResult, resistor);
		}


		return deviceInformation.str();
	}

	std::string HSPICEOutputFile::createTransconductorInformation(const Partitioning::Result & partitioningResult, const Partitioning::TransconductancePart & transconductor) const
	{
		std::ostringstream transconductorInformation;

		if(transconductor.isFirstStage())
		{
			if(transconductor.isSimple())
			{
				transconductorInformation << "Simple";
			}
			else if(transconductor.isFoldedCascode())
			{
				transconductorInformation << "FoldedCascode";
			}
			else if(transconductor.isTelescopic())
			{
				transconductorInformation << "Telescopic";
			}
			else if(transconductor.isSymmetrical())
			{
				transconductorInformation << "Symmetrical";
			}
			else if(transconductor.isComplementary())
			{
				transconductorInformation << "Complementary";
			}
			transconductorInformation << "FirstStage";
		}
		else if(transconductor.isFeedBack())
		{
			transconductorInformation << "FeedbackStage";
		}
		else if(transconductor.isPrimarySecondStage())
		{
			int index = createIndexSecondStage(partitioningResult,transconductor);
			transconductorInformation << "SecondStage" << index;

		}
		else if(transconductor.isSecondarySecondStage())
		{
			transconductorInformation << "SecondStageWithVoltageBiasAsStageBias";
		}
		else if(transconductor.isThirdStage())
		{
			transconductorInformation << "ThirdStage";
		}

		transconductorInformation << "Transconductor";

		return transconductorInformation.str();
	}

	std::string HSPICEOutputFile::createBiasInformation(const Partitioning::Result & partitioningResult, const Partitioning::BiasPart & bias) const
	{
		std::ostringstream biasInformation;
		bool isFirstStageBias = false;
		bool isSecondStageBias = false;
		bool isFeedbackStageBias = false;
		bool isSecondStageBiasWithVoltageBiasAsStageBias = false;
		bool isThirdStageBias = false;


		for(auto & firstStageBias : partitioningResult.getFirstStage().getBiasPart())
		{
			if(firstStageBias->getPartId() == bias.getPartId())
			{
				isFirstStageBias = true;
			}
		}

		if(partitioningResult.hasSecondStage())
		{
			for(auto & secondStage : partitioningResult.getSecondStages())
			{
				for(auto & secondStageBias : secondStage->getBiasPart())
				{
					if(secondStageBias->getPartId() == bias.getPartId())
					{
						if(secondStage->isPrimarySecondStage())
						{
							isSecondStageBias = true;
						}
						else
						{
							 isSecondStageBiasWithVoltageBiasAsStageBias = true;
						}

					}
				}
			}
		}

		if(partitioningResult.hasThirdStage())
		{
			for(auto & thirdStageBias : partitioningResult.getThirdStage().getBiasPart())
			{
				if(thirdStageBias->getPartId() == bias.getPartId())
				{
					isThirdStageBias = true;
				}
			}
		}

		if(partitioningResult.hasFeedbackStage())
		{
			for(auto & feedbackStageBias : partitioningResult.getFeedbackStage().getBiasPart())
			{
				if(feedbackStageBias->getPartId() == bias.getPartId())
				{
					isFeedbackStageBias = true;
				}
			}
		}

		if(isFirstStageBias)
		{
			const Partitioning::TransconductancePart & transconductor = partitioningResult.getFirstStage();
			if(transconductor.isSimple())
			{
				biasInformation << "Simple";
			}
			else if(transconductor.isFoldedCascode())
			{
				biasInformation << "FoldedCascode";
			}
			else if(transconductor.isTelescopic())
			{
				biasInformation << "Telescopic";
			}
			else if(transconductor.isSymmetrical())
			{
				biasInformation << "Symmetrical";
			}
			else if(transconductor.isComplementary())
			{
				biasInformation << "Complementary";
			}
			biasInformation << "FirstStageStage";
		}
		else if(isSecondStageBias)
		{
			int index = createIndexSecondStage(partitioningResult,bias);
			biasInformation << "SecondStage" << index << "Stage";
		}
		else if(isSecondStageBiasWithVoltageBiasAsStageBias)
		{
			biasInformation << "SecondStageWithVoltageBiasAsStageBiasStage";
		}
		else if(isThirdStageBias)
		{
			biasInformation << "ThirdStageStage";
		}
		else if(isFeedbackStageBias)
		{
			biasInformation << "FeedbackdStageStage";
		}
		else
		{
			biasInformation << "Main";
		}

		biasInformation << "Bias";

		return biasInformation.str();
	}

	std::string HSPICEOutputFile::createLoadInformation(const Partitioning::Result & partitioningResult, const Partitioning::LoadPart & load) const
	{
		std::ostringstream loadInformation;

		bool isFirstStageLoad = false;
		bool isFeedbackStageLoad = false;



		for(auto & firstStageLoad : partitioningResult.getFirstStage().getLoadPart())
		{
			if(firstStageLoad->getPartId() == load.getPartId())
			{
				isFirstStageLoad = true;
			}
		}

		if(partitioningResult.hasFeedbackStage())
		{
			for(auto & feedbackStageLoad : partitioningResult.getFeedbackStage().getLoadPart())
			{
				if(feedbackStageLoad->getPartId() == load.getPartId())
				{
					isFeedbackStageLoad = true;
				}
			}
		}

		if(isFirstStageLoad)
		{
			const Partitioning::TransconductancePart & transconductor = partitioningResult.getFirstStage();
			if(transconductor.isSimple())
			{
				loadInformation << "Simple";
			}
			else if(transconductor.isFoldedCascode())
			{
				loadInformation << "FoldedCascode";
			}
			else if(transconductor.isTelescopic())
			{
				loadInformation << "Telescopic";
			}
			else if(transconductor.isSymmetrical())
			{
				loadInformation << "Symmetrical";
			}
			else if(transconductor.isComplementary())
			{
				loadInformation << "Complementary";
			}

			loadInformation << "FirstStage";
		}
		else if(isFeedbackStageLoad)
		{
			loadInformation << "FeedbackdStage";
		}

		loadInformation << "Load";

		return loadInformation.str();
	}

	std::string HSPICEOutputFile::createResistorInformation(const Partitioning::Result & partitioningResult, const Partitioning::ResistorPart & resistor) const
	{
		std::ostringstream resistorInformation;

		if(resistor.isCompensationResistor())
		{
			resistorInformation << "Compensation";
		}
		resistorInformation << "Resistor";



		return resistorInformation.str();
	}

	std::string HSPICEOutputFile::createCapacitorInformation(const Partitioning::Result & partitioningResult, const Partitioning::CapacitancePart & capacitor) const
	{
		std::ostringstream capacitorInformation;

		if(capacitor.isCompensationCapacitance())
		{
			capacitorInformation << "Compensation";
		}
		else if(capacitor.isLoadCapacitance())
		{
			capacitorInformation << "Load";
		}

		capacitorInformation << "Capacitor";

		return capacitorInformation.str();
	}


	std::string HSPICEOutputFile::createCircuitTypeInformation(const Partitioning::Result & partitioningResult) const
	{
		std::ostringstream circuitTypeInformation;


		if(partitioningResult.hasFeedbackStage())
		{
			circuitTypeInformation << "FullyDifferential";
		}
		else if(partitioningResult.getFirstStage().isComplementary())
		{
			circuitTypeInformation << "Complementary";
		}
		else if(partitioningResult.hasSecondarySecondStage())
		{
			circuitTypeInformation << "Symmetrical";
		}
		else
		{
			circuitTypeInformation << "SingleOutput";
		}

		return circuitTypeInformation.str();
	}

	int HSPICEOutputFile::createIndexSecondStage(const Partitioning::Result & partitioningResult, const Partitioning::Part & part) const
	{
		assert(part.isBiasPart() || part.isTransconductancePart(), "second stage part can only be bias or transconductance part!");

		int index = 1;

		for(auto & secondStage : partitioningResult.getSecondStages())
		{

			if(part.isTransconductancePart())
			{
				if(secondStage->getPartId() == part.getPartId())
				{
					return index;
				}
			}
			else if(part.isBiasPart())
			{
				for(auto & biasSecondStage : secondStage->getBiasPart())
				{
					if(biasSecondStage->getPartId() == part.getPartId())
					{
						return index;
					}
				}
			}
			index++;
		}
		assert(false, "Part must be part of a second stage");
		return 0;
	}
}

