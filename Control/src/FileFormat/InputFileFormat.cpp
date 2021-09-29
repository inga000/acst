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

#include "Control/incl/FileFormat/InputFileFormat.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Control {

    const std::string InputFileFormat::OPEN_ACCESS_STRING_ = "oa";
    const std::string InputFileFormat::HSPICE_STRING_ = "hspice";

    InputFileFormat::InputFileFormat() :
            FileFormatBase<InputFileFormat>()
    {
    }

    InputFileFormat::InputFileFormat(const std::string& format) :
            FileFormatBase<InputFileFormat>(format)
    {
        assert(isValid(format));
    }

    InputFileFormat InputFileFormat::openAccess()
    {
        InputFileFormat openAccessFormat(OPEN_ACCESS_STRING_);
        return openAccessFormat;
    }

    InputFileFormat InputFileFormat::hspice()
    {
        InputFileFormat hspiceFormat(HSPICE_STRING_);
        return hspiceFormat;
    }

    bool InputFileFormat::operator ==(const InputFileFormat& other) const
    {
        return getFormat() == other.getFormat();
    }

    bool InputFileFormat::isValid(const std::string& formatString) const
    {
        return formatString == OPEN_ACCESS_STRING_ ||
                formatString == HSPICE_STRING_;
    }

}
