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

#ifndef HSPICE_OUTPUTFILE_DEVICELINE_H_
#define HSPICE_OUTPUTFILE_DEVICELINE_H_

#include <string>

namespace Control {
    class OutputFile;
}

namespace Core {
    class Net;
    class Core;
    class Device;
}

namespace HSpice {
    namespace OutputFile {

        class DeviceLine
        {
        public:
            DeviceLine();

            void setDevice(const Core::Device& component);
            void setOutputFile(Control::OutputFile& outputFile);
            void write();

        private:
            void writeMosfetLine();
            void writeBipolarLine();
            void writeCapacitorLine();
            void writeResistorLine();
            void writeInductorLine();
            void writeDiodeLine();

            std::string getNetName(const Core::Net& net) const;

            const Core::Device& getDevice() const;
            Control::OutputFile& getOutputFile();

            const Core::Device* device_;
            Control::OutputFile* outputFile_;
        };

    }
}


#endif
