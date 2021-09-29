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

#include "HSpice/incl/InputFile/InstanceLine/InstanceLine.h"
#include "HSpice/incl/InputFile/InstanceLine/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include <sstream>
#include <vector>
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/StreamOperators.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace InputFile {

        const char InstanceLine::INSTANCE_ID_LOWER_ = 'x';
        const char InstanceLine::INSTANCE_ID_UPPER_ = 'X';

        InstanceLine::InstanceLine()
        {
        }

        void InstanceLine::setLine(const std::string& line)
        {
            line_ = line;
        }

        bool InstanceLine::isInstance(char identifier)
        {
            return identifier == INSTANCE_ID_LOWER_ || identifier == INSTANCE_ID_UPPER_;
        }

        void InstanceLine::parse()
        {
            char identifier = getLine()[0];
            assert(isInstance(identifier));

            std::istringstream lineStream;
            lineStream.str(getLine());
            parseInstanceName(lineStream);
            parseInstanceTerminalsAndName(lineStream);
        }

        Instance InstanceLine::getInstance() const
        {
            assert(instance_.isInitialized());
            return instance_;
        }

        std::vector<Core::NetId> InstanceLine::getNetIds() const
        {
            return netIds_;
        }

        void InstanceLine::parseInstanceName(std::istringstream& lineStream)
        {
            std::string instanceName;
            lineStream >> instanceName;
            Core::InstanceId instanceId = Core::InstanceName(instanceName).createIdentifier();
            instance_.setInstanceId(instanceId);
        }

        void InstanceLine::parseInstanceTerminalsAndName(std::istringstream& lineStream)
        {
            std::vector<std::string> instanceTerminalsAndModel;
            int slashToken = -1;
            while(!lineStream.eof() && slashToken != 1) {
                if(slashToken != -1) {
                    slashToken++;
                }
                std::string tokenName;
                lineStream >> tokenName;
                if(tokenName == "/") {
                    slashToken = 0; // start counting
                    continue; // ignore any slash token
                }
                if(tokenName != "") {
                    instanceTerminalsAndModel.push_back(tokenName);
                }
            }
            assert(instanceTerminalsAndModel.size() > 1);
            std::string modelNameString = instanceTerminalsAndModel[instanceTerminalsAndModel.size() -1];
            ModelName modelName(modelNameString);
            instance_.setModelName(modelName);

            instanceTerminalsAndModel.pop_back();
            for(std::vector<std::string>::const_iterator it = instanceTerminalsAndModel.begin();
                    it != instanceTerminalsAndModel.end(); it++)
            {
                Core::NetId netId = Core::NetName(*it).createRootIdentifier();
                netIds_.push_back(netId);
                instance_.appendTerminalNetId(netId);
            }
        }

        std::string InstanceLine::getLine() const
        {
            assert(!line_.empty());
            return line_;
        }

    }
}
