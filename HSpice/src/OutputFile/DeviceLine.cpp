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

#include "HSpice/incl/OutputFile/DeviceLine.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeNames.h"
#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/BipolarDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/CapacitorDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/InductorDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/ResistorDeviceType.h"
#include "Core/incl/Circuit/Device/DeviceType/DiodeDeviceType.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "HSpice/incl/OutputFile/HSpiceString.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Control/incl/File/OutputFile.h"
#include "Core/incl/Common/BacktraceAssert.h"


namespace HSpice {
    namespace OutputFile {

        DeviceLine::DeviceLine():
                device_(NULL),
                outputFile_(NULL)
        {
        }

        void DeviceLine::setDevice(const Core::Device& component)
        {
            device_ = &component;
        }

        void DeviceLine::setOutputFile(Control::OutputFile& outputFile)
        {
            outputFile_ = &outputFile;
        }

        void DeviceLine::write()
        {
            const Core::DeviceType& componentType = getDevice().getDeviceType();
            std::string componentName = HSpiceString::convertToHSpice(getDevice());

            getOutputFile() << componentName << " ";
            if(componentType == Core::DeviceTypeNames::Mosfet) {
                writeMosfetLine();
            } else if (componentType == Core::DeviceTypeNames::Bipolar) {
                writeBipolarLine();
            } else if(componentType == Core::DeviceTypeNames::Capacitor) {
                writeCapacitorLine();
            } else if(componentType == Core::DeviceTypeNames::Resistor) {
                writeResistorLine();
            } else if(componentType == Core::DeviceTypeNames::Inductor) {
                writeInductorLine();
            } else if(componentType == Core::DeviceTypeNames::Diode) {
                writeDiodeLine();
            } else {
                assert(false, "Unsupported DeviceType!");
            }
            getOutputFile() << "\n";
        }

        void DeviceLine::writeMosfetLine()
        {
            Core::MosfetDeviceType mosfetDeviceType;

            const Core::Pin& drainPin = getDevice().findPin(mosfetDeviceType.drain());
            const Core::Pin& gatePin = getDevice().findPin(mosfetDeviceType.gate());
            const Core::Pin& sourcePin = getDevice().findPin(mosfetDeviceType.source());
            const Core::Pin& bulkPin = getDevice().findPin(mosfetDeviceType.bulk());

            std::string drainNetName = getNetName(drainPin.getNet());
            std::string gateNetName = getNetName(gatePin.getNet());
            std::string sourceNetName = getNetName(sourcePin.getNet());
            std::string bulkNetName = getNetName(bulkPin.getNet());

            getOutputFile() << drainNetName << " ";
            getOutputFile() << gateNetName << " ";
            getOutputFile() << sourceNetName << " ";
            getOutputFile() << bulkNetName << " ";

            Core::TechType techType = getDevice().getTechType();
            if(techType == Core::TechType::n()) {
                getOutputFile() << "nmos";
            } else if(techType == Core::TechType::p()) {
                getOutputFile() << "pmos";
            } else {
                assert(false, "Wrong TechType!");
            }
        }

        void DeviceLine::writeBipolarLine()
        {
            Core::BipolarDeviceType bipolarDeviceType;

            const Core::Pin& collectorPin = getDevice().findPin(bipolarDeviceType.collector());
            const Core::Pin& basePin = getDevice().findPin(bipolarDeviceType.base());
            const Core::Pin& emitterPin = getDevice().findPin(bipolarDeviceType.emitter());

            std::string collectorNetName = getNetName(collectorPin.getNet());
            std::string baseNetName = getNetName(basePin.getNet());
            std::string emitterNetName = getNetName(emitterPin.getNet());

            getOutputFile() << collectorNetName << " ";
            getOutputFile() << baseNetName << " ";
            getOutputFile() << emitterNetName << " ";
            getOutputFile() << "0 "; //required for hspice

            Core::TechType techType = getDevice().getTechType();
            if(techType == Core::TechType::n()) {
                getOutputFile() << "npn";
            } else if(techType == Core::TechType::p()) {
                getOutputFile() << "pnp";
            } else {
                assert(false, "Wrong TechType!");
            }
        }

        void DeviceLine::writeCapacitorLine()
        {
            Core::CapacitorDeviceType capacitorDeviceType;

            const Core::Pin& plusPin = getDevice().findPin(capacitorDeviceType.plus());
            const Core::Pin& minusPin = getDevice().findPin(capacitorDeviceType.minus());

            std::string plusNetName = getNetName(plusPin.getNet());
            std::string minusNetName = getNetName(minusPin.getNet());

            getOutputFile() << plusNetName << " ";
            getOutputFile() << minusNetName << " ";
            getOutputFile() << "capacitor";
        }

        void DeviceLine::writeResistorLine()
        {
            Core::ResistorDeviceType resistorDeviceType;

            const Core::Pin& plusPin = getDevice().findPin(resistorDeviceType.plus());
            const Core::Pin& minusPin = getDevice().findPin(resistorDeviceType.minus());

            std::string plusNetName = getNetName(plusPin.getNet());
            std::string minusNetName = getNetName(minusPin.getNet());

            getOutputFile() << plusNetName << " ";
            getOutputFile() << minusNetName << " ";
            getOutputFile() << "resistor";
        }

        void DeviceLine::writeInductorLine()
        {
            Core::InductorDeviceType inductorDeviceType;

            const Core::Pin& plusPin = getDevice().findPin(inductorDeviceType.plus());
            const Core::Pin& minusPin = getDevice().findPin(inductorDeviceType.minus());

            std::string plusNetName = getNetName(plusPin.getNet());
            std::string minusNetName = getNetName(minusPin.getNet());

            getOutputFile() << plusNetName << " ";
            getOutputFile() << minusNetName << " ";
            getOutputFile() << "inductor";
        }

        void DeviceLine::writeDiodeLine()
        {
            Core::DiodeDeviceType diodeDeviceType;

            const Core::Pin& cathodePin = getDevice().findPin(diodeDeviceType.cathode());
            const Core::Pin& anodePin = getDevice().findPin(diodeDeviceType.anode());

            std::string cathodeNetName = getNetName(cathodePin.getNet());
            std::string anodeNetName = getNetName(anodePin.getNet());

            getOutputFile() << cathodeNetName << " ";
            getOutputFile() << anodeNetName << " ";
            getOutputFile() << "diode";
        }

        std::string DeviceLine::getNetName(const Core::Net& net) const
        {
            Core::NetId netId = net.getIdentifier();
            std::string netName = HSpiceString::convertToHSpice(netId);
            return netName;
        }

        const Core::Device& DeviceLine::getDevice() const
        {
            assert(device_ != NULL);
            return *device_;
        }

        Control::OutputFile& DeviceLine::getOutputFile()
        {
            assert(outputFile_ != NULL);
            return *outputFile_;
        }

    }
}


