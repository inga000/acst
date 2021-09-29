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

#ifndef HSPICE_INPUTFILE_SUBCIRCUITPARSER_H_
#define HSPICE_INPUTFILE_SUBCIRCUITPARSER_H_

#include "HSpice/incl/InputFile/ModelName.h"
#include <string>

namespace Core {
    class DeviceTypeRegister;
}

namespace HSpice {

	class DeviceLineMapper;
    namespace InputFile {

        class SubCircuit;

        class SubCircuitParser
        {
        public:
            SubCircuitParser();
            ~SubCircuitParser();

            void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);
            void setDeviceLineMapper(const DeviceLineMapper & deviceLineMapper);
            void setSubCircuitString(const std::string& subCircuitString);

            static bool isSubCircuitLine(const std::string & line);
            static bool isEndsLine(const std::string & line);

            SubCircuit* createNewSubCircuit();

        private:
            static const std::string SUBCIRCUIT_ID1_;
            static const std::string SUBCIRCUIT_ID2_;
            static const std::string SUBCIRCUIT_ID3_;
            static const std::string SUBCIRCUIT_ID4_;
            static const std::string SUBCIRCUIT_ID5_;
            static const std::string SUBCIRCUIT_ID6_;
            static const std::string SUBCIRCUIT_ID7_;
            static const std::string SUBCIRCUIT_ID8_;

            static const std::string ENDS_ID1_;
            static const std::string ENDS_ID2_;
            static const std::string ENDS_ID3_;

            static bool isSubCircuit(const std::string& line);
            static bool isEnds(const std::string& line);

            void parseModelNameAndTerminalsHSpice(const std::string& line);
            void parseModelNameAndTerminalsCdl(const std::string& line);
            void parseLine(const std::string& line);
            void parseDeviceLine(const std::string& line);
            void parseInstanceLine(const std::string& line);

            static bool isNotValid(char character);
            static bool isValid(const std::string & string);

            const Core::DeviceTypeRegister& getDeviceTypeRegister() const;
            const DeviceLineMapper & getDeviceLineMapper() const;
            const std::string& getSubCircuitString() const;

            SubCircuit& getSubCircuit();

        private:
            SubCircuit* subCircuit_;

            const Core::DeviceTypeRegister* deviceTypeRegister_;
            const DeviceLineMapper* deviceLineMapper_;
            std::string subCircuitString_;
        };

    }
}


#endif
