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

#include "HSpice/incl/InputFile/InputFile.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/TopCircuit.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"
#include "HSpice/incl/InputFile/GlobalNetMap.h"
#include "HSpice/incl/InputFile/DeviceLine/DeviceLine.h"
#include "HSpice/incl/InputFile/InstanceLine/InstanceLine.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuitParser.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include "HSpice/incl/InputFile/InstanceLine/Instance.h"
#include "HSpice/incl/InputFile/SubCircuitParser/InstantiatedSubCircuitMap.h"
#include "Log/incl/LogMacros.h"
#include <sstream>

namespace HSpice {
    namespace InputFile {

        const std::string InputFile::GLOBALNET_ID1_ = ".global";
        const std::string InputFile::GLOBALNET_ID2_ = ".GLOBAL";
        const std::string InputFile::GLOBALNET_ID3_ = ".Global";

        const std::string InputFile::LINE_CONTINUATION_STRING_ = "+";

        InputFile::InputFile():
        		deviceTypeRegister_(nullptr),
				deviceLineMapper_(nullptr)
        {
        }

        InputFile::~InputFile()
        {
        	delete deviceLineMapper_;
        }

        void InputFile::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
		{
        	deviceTypeRegister_ = &deviceTypeRegister;
		}

        void InputFile::setDeviceLineMapper(const DeviceLineMapper& deviceLineMapper)
		{
        	deviceLineMapper_ = &deviceLineMapper;
		}

		void InputFile::setSupplyNetIds(const SupplyNetIdentifiers supplyNetIdentifiers)
		{
		   supplyNetIds_ = supplyNetIdentifiers;
		}

        Core::Circuit* InputFile::readNewCircuit()
        {
            setAllSupplyNetIdentifers();
            logDebug("Reading HSpice-Netlist...");

            readCompleteFile();
            preProcessLineContinuation();
            initializeFileContentStream();
            parseEachLine();

            Core::TopCircuit * topCircuit = createNewTopCircuit();
            logTrace("+++++Global Nets: " << globalNetIdentifiers_);
            GlobalNetMap globalNetMap = globalNetIdentifiers_.addNetsToCircuit(*topCircuit);

            InstantiatedSubCircuitMap instantiatedSubCircuitMap;
            subCircuits_.setAllGlobalNetMaps(globalNetMap);
            subCircuits_.setAllInstantiatedSubCircuitMaps(instantiatedSubCircuitMap);
            subCircuits_.fillInstantiatedSubCircuitMap(instantiatedSubCircuitMap);

            netIdentifiers_.setGlobalNetMap(globalNetMap);
            netIdentifiers_.addNetsToCircuit(*topCircuit);

            devices_.setAllGlobalNetMaps(globalNetMap);
            devices_.addDevicesToCircuit(*topCircuit);

            instances_.setInstantiatedSubCircuitMap(instantiatedSubCircuitMap);
            instances_.setGlobalNetMap(globalNetMap);
            instances_.addInstancesToCircuit(*topCircuit);

            instantiatedSubCircuitMap.copyMasterPointersIntoTopCircuit(*topCircuit);

            return topCircuit;
        }

        void InputFile::setAllSupplyNetIdentifers()
        {
            if(hasSupplyNetIds()) {
                globalNetIdentifiers_.setSupplyNetIdentifiers(getSupplyNetIds());
                subCircuits_.setAllSupplyNetIdentifiers(getSupplyNetIds());
                netIdentifiers_.setSupplyNetIdentifiers(getSupplyNetIds());
            }
        }

        Core::TopCircuit* InputFile::createNewTopCircuit() const
        {
            Core::TopCircuit* topCircuit = new Core::TopCircuit();
            Core::CircuitId circuitId = Core::CircuitName("TOPCIRCUIT").createIdentifier();
            circuitId.setId(0);
            circuitId.setTechType(Core::TechType::undefined());
            topCircuit->setCircuitIdentifier(circuitId);
            return topCircuit;
        }

        void InputFile::preProcessLineContinuation()
        {
            findAndReplace(getFileContent(), "\n"+LINE_CONTINUATION_STRING_, "");
            findAndReplace(getFileContent(), "\n "+LINE_CONTINUATION_STRING_, "");
            findAndReplace(getFileContent(), "\n  "+LINE_CONTINUATION_STRING_, "");
            findAndReplace(getFileContent(), "\n   "+LINE_CONTINUATION_STRING_, "");
        }

        void InputFile::initializeFileContentStream()
        {
            fileContentStream_.str(getFileContent());
        }

        void InputFile::parseEachLine()
        {
            while(!getFileContentStream().eof()) {
                std::string line;
                std::getline(getFileContentStream(), line);
                parseLine(line);
            }
        }

        void InputFile::parseLine(const std::string& line)
        {
            char identifier = line[0];

            if(isGlobalNetLine(line)) {
                parseGlobalNets(line);
            } else if(SubCircuitParser::isSubCircuitLine(line)) {
                parseSubCircuit(line);
            } else if(InstanceLine::isInstance(identifier)) {
                parseInstance(line);
            } else  if(getDeviceLineMapper().isDevice(identifier)) {
                parseDevice(line);
            } else {
                // ignore line
            }
        }

        bool InputFile::isGlobalNetLine(const std::string & line)
        {
            return  line.find(GLOBALNET_ID1_) == 0 ||
                    line.find(GLOBALNET_ID2_) == 0 ||
                    line.find(GLOBALNET_ID3_) == 0;
        }

        void InputFile::parseGlobalNets(const std::string& line)
        {
            std::istringstream iss;
            iss.str(line);

            std::string globalToken;
            iss >> globalToken;

            std::string globalNetName;
            while(!iss.eof())
            {
                iss >> globalNetName;
                Core::NetId globalNetId = Core::NetName(globalNetName).createRootIdentifier();
                globalNetIdentifiers_.addNetId(globalNetId);
            }
        }

        void InputFile::parseSubCircuit(const std::string& line)
        {
            std::string subCircuitLines = extractSubCircuitLines(line);

            SubCircuitParser subCircuitParser;
            subCircuitParser.setDeviceTypeRegister(getDeviceTypeRegister());
            subCircuitParser.setDeviceLineMapper(getDeviceLineMapper());
            subCircuitParser.setSubCircuitString(subCircuitLines);
            SubCircuit* newMaster = subCircuitParser.createNewSubCircuit();
            subCircuits_.add(newMaster);
        }

        void InputFile::parseInstance(const std::string& line)
        {
            InstanceLine instanceLine;
            instanceLine.setLine(line);
            instanceLine.parse();
            Instance instance = instanceLine.getInstance();
            instances_.addInstance(instance);

            std::vector<Core::NetId> netIds = instanceLine.getNetIds();
            netIdentifiers_.addNetIds(netIds);
        }

        void InputFile::parseDevice(const std::string& line)
        {
            DeviceLine deviceLine;
            deviceLine.setDeviceTypeRegister(getDeviceTypeRegister());
            deviceLine.setLine(line);
            deviceLine.setDeviceLineMapper(getDeviceLineMapper());
            deviceLine.parse();

            AbstractDevice* device = deviceLine.getDevice();
            devices_.addDevice(*device);

            std::vector<Core::NetId> netIds = deviceLine.getNetIds();
            netIdentifiers_.addNetIds(netIds);
        }

        std::string InputFile::extractSubCircuitLines(const std::string& line)
        {
            std::string subCircuitString;
            subCircuitString.append(line);
            subCircuitString.append("\n");

            std::string subCircuitLine;
            while(getline(getFileContentStream(), subCircuitLine))
            {
                subCircuitString.append(subCircuitLine);
                subCircuitString.append("\n");
                if(SubCircuitParser::isEndsLine(subCircuitLine)) {
                    break;
                }
            }

            return subCircuitString;
        }

        SubCircuits& InputFile::getSubCircuits()
        {
            return subCircuits_;
        }

        const SubCircuits& InputFile::getSubCircuits() const
        {
            return subCircuits_;
        }

        bool InputFile::hasDeviceLineMapper() const
        {
            return deviceLineMapper_ != NULL;
        }

        const DeviceLineMapper& InputFile::getDeviceLineMapper() const
        {
            assert(hasDeviceLineMapper());
            return *deviceLineMapper_;
        }

        bool InputFile::hasSupplyNetIds() const
        {
            return supplyNetIds_.isInitialized();
        }

        const SupplyNetIdentifiers InputFile::getSupplyNetIds() const
        {
            assert(hasSupplyNetIds());
            return supplyNetIds_;
        }

        std::istream& InputFile::getFileContentStream()
        {
            return fileContentStream_;
        }

		bool InputFile::hasDeviceTypeRegister() const
		{
			return deviceTypeRegister_ != NULL;
		}

		const Core::DeviceTypeRegister& InputFile::getDeviceTypeRegister() const
		{
			assert(hasDeviceTypeRegister());
			return *deviceTypeRegister_;
		}

        void InputFile::findAndReplace(std::string& source, const std::string & find, const std::string & replace)
        {
            for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
            {
                source.replace(i, find.length(), replace);
                i += replace.length();
            }
        }

    }
}


