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

#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuitParser.h"
#include "HSpice/incl/InputFile/DeviceLine/DeviceLine.h"
#include "HSpice/incl/InputFile/InstanceLine/InstanceLine.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuit.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Log/incl/LogMacros.h"
#include <algorithm>
#include <locale>
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        const std::string SubCircuitParser::SUBCIRCUIT_ID1_ = ".subckt";
        const std::string SubCircuitParser::SUBCIRCUIT_ID2_ = ".SUBCKT";
        const std::string SubCircuitParser::SUBCIRCUIT_ID3_ = ".Subckt";
        const std::string SubCircuitParser::SUBCIRCUIT_ID4_ = ".SubCkt";
        const std::string SubCircuitParser::SUBCIRCUIT_ID5_ = ".subcircuit";
        const std::string SubCircuitParser::SUBCIRCUIT_ID6_ = ".SubCircuit";
        const std::string SubCircuitParser::SUBCIRCUIT_ID7_ = ".Subcircuit";
        const std::string SubCircuitParser::SUBCIRCUIT_ID8_ = ".SUBCIRCUIT";

        const std::string SubCircuitParser::ENDS_ID1_ = ".ends";
        const std::string SubCircuitParser::ENDS_ID2_ = ".ENDS";
        const std::string SubCircuitParser::ENDS_ID3_ = ".Ends";

        SubCircuitParser::SubCircuitParser():
                subCircuit_(NULL),
				deviceTypeRegister_(NULL),
				deviceLineMapper_(NULL)
        {
        }

        SubCircuitParser::~SubCircuitParser()
        {
        }

        void SubCircuitParser::setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister)
        {
        	deviceTypeRegister_ = &deviceTypeRegister;
        }

        void SubCircuitParser::setDeviceLineMapper(const DeviceLineMapper& deviceLineMapper)
        {
        	deviceLineMapper_ = &deviceLineMapper;
        }

        void SubCircuitParser::setSubCircuitString(const std::string& subCircuitString)
        {
            subCircuitString_ = subCircuitString;
        }

        bool SubCircuitParser::isSubCircuitLine(const std::string & line)
        {
            return  line.find(SUBCIRCUIT_ID1_) == 0 ||
                    line.find(SUBCIRCUIT_ID2_) == 0 ||
                    line.find(SUBCIRCUIT_ID3_) == 0 ||
                    line.find(SUBCIRCUIT_ID4_) == 0 ||
                    line.find(SUBCIRCUIT_ID5_) == 0 ||
                    line.find(SUBCIRCUIT_ID6_) == 0 ||
                    line.find(SUBCIRCUIT_ID7_) == 0 ||
                    line.find(SUBCIRCUIT_ID8_) == 0;
        }

        bool SubCircuitParser::isEndsLine(const std::string & line)
        {
            return  line.find(ENDS_ID1_) == 0 ||
                    line.find(ENDS_ID2_) == 0 ||
                    line.find(ENDS_ID3_) == 0;
        }

        SubCircuit* SubCircuitParser::createNewSubCircuit()
        {
            subCircuit_ = new SubCircuit();

            std::istringstream subCircuitStream;
            subCircuitStream.str(getSubCircuitString());

            std::string line;
            while(getline(subCircuitStream, line)) {
                logTrace("Line: " << line);
                char identifier = line[0];
                if(isSubCircuit(line)) {
                    parseModelNameAndTerminalsCdl(line);
                } else if(isEnds(line)) {
                    break;
                } else if (getDeviceLineMapper().isDevice(identifier) ||
                             InstanceLine::isInstance(identifier)   ) {
                    parseLine(line);
                } else {
                    // skip line
                }
            }
            return subCircuit_;
        }

        bool SubCircuitParser::isSubCircuit(const std::string& line)
        {
            return  line.find(SUBCIRCUIT_ID1_) == 0 ||
                    line.find(SUBCIRCUIT_ID2_) == 0 ||
                    line.find(SUBCIRCUIT_ID3_) == 0 ||
                    line.find(SUBCIRCUIT_ID4_) == 0 ||
                    line.find(SUBCIRCUIT_ID5_) == 0 ||
                    line.find(SUBCIRCUIT_ID6_) == 0 ||
                    line.find(SUBCIRCUIT_ID7_) == 0 ||
                    line.find(SUBCIRCUIT_ID8_) == 0;
        }

        bool SubCircuitParser::isEnds(const std::string& line)
        {
            return  line.find(ENDS_ID1_) == 0 ||
                    line.find(ENDS_ID2_) == 0 ||
                    line.find(ENDS_ID3_) == 0;
        }

        void SubCircuitParser::parseModelNameAndTerminalsHSpice(const std::string& line)
        {
            std::istringstream iss;
            iss.str(line);

            std::string skipTokenSubckt;
            std::string modelNameString;
            iss >> skipTokenSubckt;
            iss >> modelNameString;

            ModelName modelName(modelNameString);
            getSubCircuit().setModelName(modelName);

            while(!iss.eof())
            {
                std::string terminalName;
                iss >> terminalName;
                if(terminalName != "" && isValid(terminalName)) {
                    Core::TerminalId terminalId = Core::TerminalName(terminalName).createIdentifier();
                    Core::NetId netId = Core::NetName(terminalName).createRootIdentifier();

                    getSubCircuit().addTerminalIdentifier(terminalId);
                    getSubCircuit().addNetId(netId);
                }

            }
        }

        void SubCircuitParser::parseModelNameAndTerminalsCdl(const std::string& line)
        {
            std::istringstream iss;
            iss.str(line);

            std::string skipTokenSubckt;
            std::string modelNameString;
            iss >> skipTokenSubckt;
            iss >> modelNameString;

            ModelName modelName(modelNameString);
            getSubCircuit().setModelName(modelName);

            while(!iss.eof())
            {
                std::string terminalName;
                iss >> terminalName;
                if(terminalName != "" && isValid(terminalName)) {
                    Core::TerminalId terminalId = Core::TerminalName(terminalName).createIdentifier();
                    Core::NetId netId = Core::NetName(terminalName).createRootIdentifier();

                    getSubCircuit().addTerminalIdentifier(terminalId);
                    getSubCircuit().addNetId(netId);
                }

            }
        }

        void SubCircuitParser::parseLine(const std::string& line)
        {
            char identifier = line[0];
            if(getDeviceLineMapper().isDevice(identifier)) {
                parseDeviceLine(line);
            } else if(InstanceLine::isInstance(identifier)) {
                parseInstanceLine(line);
            } else {
                logDebug("Error: unexpected line identifier: " << identifier);
                assert(false);
            }
        }

        void SubCircuitParser::parseDeviceLine(const std::string& line)
        {
            DeviceLine deviceLine;
            deviceLine.setDeviceTypeRegister(getDeviceTypeRegister());
            deviceLine.setLine(line);
            deviceLine.setDeviceLineMapper(getDeviceLineMapper());
            deviceLine.parse();

            std::vector<Core::NetId> netIds = deviceLine.getNetIds();
            getSubCircuit().addNetIds(netIds);

            AbstractDevice* device = deviceLine.getDevice();
            getSubCircuit().addDevice(*device);
        }

        void SubCircuitParser::parseInstanceLine(const std::string& line)
        {
            InstanceLine instanceLine;
            instanceLine.setLine(line);
            instanceLine.parse();

            Instance instance = instanceLine.getInstance();
            getSubCircuit().addInstance(instance);

            std::vector<Core::NetId> netIds = instanceLine.getNetIds();
            getSubCircuit().addNetIds(netIds);
        }

        bool SubCircuitParser::isNotValid(char character)
        {
            return !(  std::isalnum(character) ||
                        character == '_' ||
                        character == '+' ||
                        character == '-' ||
                        character == '<' ||
                        character == '>' ||
                        character == '[' ||
                        character == ']' ||
                        character == '(' ||
                        character == ')');
        }

        bool SubCircuitParser::isValid(const std::string& string)
        {
            return std::find_if(string.begin(), string.end(), isNotValid) == string.end();
        }

        const std::string& SubCircuitParser::getSubCircuitString() const
        {
            assert(!subCircuitString_.empty());
            return subCircuitString_;
        }


        const Core::DeviceTypeRegister& SubCircuitParser::getDeviceTypeRegister() const
        {
        	assert(deviceTypeRegister_ != NULL);
        	return *deviceTypeRegister_;
        }

        const DeviceLineMapper& SubCircuitParser::getDeviceLineMapper() const
        {
            assert(deviceLineMapper_ != NULL);
            return *deviceLineMapper_;
        }

        SubCircuit& SubCircuitParser::getSubCircuit()
        {
            assert(subCircuit_ != NULL);
            return *subCircuit_;
        }

    }
}



