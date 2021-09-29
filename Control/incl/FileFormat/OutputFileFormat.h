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


#ifndef CONTROL_OUTPUTFILEFORMAT_H_
#define CONTROL_OUTPUTFILEFORMAT_H_

#include <string>
#include "Control/incl/FileFormat/FileFormatBase.h"

namespace Control {

    class OutputFileFormat : public FileFormatBase<OutputFileFormat>
    {
    public:
        OutputFileFormat();
        explicit OutputFileFormat(const std::string & format);

        static OutputFileFormat skill();
        static OutputFileFormat xml();
        static OutputFileFormat hspice();
        static OutputFileFormat text();

        bool operator==(const OutputFileFormat & other) const;

    private:
        static const std::string SKILL_FORMAT_STRING_;
        static const std::string XML_FORMAT_STRING_;
        static const std::string HSPICE_FORMAT_STRING_;
        static const std::string TEXT_FORMAT_STRING_;

        bool isValid(const std::string& format) const;
    };
}


#endif
