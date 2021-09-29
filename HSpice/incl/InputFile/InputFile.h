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

#ifndef HSPICE_INPUTFILE_INPUTFILE_H_
#define HSPICE_INPUTFILE_INPUTFILE_H_

#include "Control/incl/File/InputFile.h"
#include "Control/incl/AbstractCircuitSource.h"
#include "HSpice/incl/InputFile/NetIdentifiers/GlobalNetIdentifiers.h"
#include "HSpice/incl/InputFile/NetIdentifiers/NetIdentifiers.h"
#include "HSpice/incl/InputFile/DeviceLine/Devices.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "HSpice/incl/InputFile/InstanceLine/Instances.h"
#include "HSpice/incl/InputFile/SubCircuitParser/SubCircuits.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetIdentifiers.h"
#include <string>

namespace Core {
    class Circuit;
    class DeviceTypeRegister;
}

namespace HSpice {

	class DeviceLineMapper;
    namespace InputFile {

        class InputFile: public Control::InputFile, public Control::AbstractCircuitSource
        {
        public:
            InputFile();
            ~InputFile();
        private:
            InputFile(const InputFile & other); // due to fileContentStream_
            void operator=(const InputFile & other); // due to fileContentStream_

        public:
            void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);
            void setDeviceLineMapper(const DeviceLineMapper& deviceLineMapper);
            void setSupplyNetIds(const SupplyNetIdentifiers  supplyNetIdentifiers);

            Core::Circuit* readNewCircuit();

        private:
            static const std::string GLOBALNET_ID1_;
            static const std::string GLOBALNET_ID2_;
            static const std::string GLOBALNET_ID3_;

            static const std::string LINE_CONTINUATION_STRING_;

            static bool isGlobalNetLine(const std::string & line);

            void setAllSupplyNetIdentifers();
            Core::TopCircuit * createNewTopCircuit() const;

            void preProcessLineContinuation();
            void initializeFileContentStream();
            void parseEachLine();

            void parseLine(const std::string& line);
            void parseGlobalNets(const std::string& line);
            void parseSubCircuit(const std::string& line);
            void parseDevice(const std::string& line);
            void parseInstance(const std::string& line);

            std::string extractSubCircuitLines(const std::string& line);

            SubCircuits& getSubCircuits();
            const SubCircuits& getSubCircuits() const;

            bool hasDeviceTypeRegister() const;
            bool hasDeviceLineMapper() const;
            bool hasSupplyNetIds() const;

            const Core::DeviceTypeRegister& getDeviceTypeRegister() const;
            const DeviceLineMapper& getDeviceLineMapper() const;
            const SupplyNetIdentifiers  getSupplyNetIds() const;

            std::istream & getFileContentStream();
            void findAndReplace(std::string& source, const std::string & find, const std::string & replace);

        private:
            const Core::DeviceTypeRegister* deviceTypeRegister_;
            const DeviceLineMapper* deviceLineMapper_;
            SupplyNetIdentifiers supplyNetIds_;

            std::istringstream fileContentStream_;

            GlobalNetIdentifiers globalNetIdentifiers_;
            NetIdentifiers netIdentifiers_;
            Devices devices_;
            Instances instances_;
            SubCircuits subCircuits_;
        };

    }
}

#endif
