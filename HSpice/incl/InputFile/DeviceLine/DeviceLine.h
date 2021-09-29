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

#ifndef HSPICE_INPUTFILE_DEVICELINE_H_
#define HSPICE_INPUTFILE_DEVICELINE_H_

#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include <vector>
#include <string>


namespace Core {

    class DeviceTypeRegister;

}

namespace HSpice {

	class DeviceLineMapper;
    namespace InputFile {

        class AbstractDevice;

        class DeviceLine
        {
        public:
            DeviceLine();

            bool isDevice(char identifier);

            void setLine(const std::string& line);
            void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);
            void setDeviceLineMapper(const DeviceLineMapper & deviceLineMapper);
            void parse();

            AbstractDevice* getDevice() const;
            std::vector<Core::NetId> getNetIds() const;

        private:
            bool hasLine() const;
            bool hasDeviceTypeRegister() const;
            bool hasDeviceLineMapper() const;

            const std::string& getLine() const;
            const Core::DeviceTypeRegister& getDeviceTypeRegister() const;
            const DeviceLineMapper & getDeviceLineMapper() const;

        private:
            AbstractDevice* abstractDevice_;
            std::vector<Core::NetId> netIds_;

            const Core::DeviceTypeRegister* deviceTypeRegister_;
            const DeviceLineMapper* deviceLineMapper_;
            std::string line_;
        };

    }
}


#endif
