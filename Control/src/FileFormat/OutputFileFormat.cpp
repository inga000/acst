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

#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Control {

    const std::string OutputFileFormat::SKILL_FORMAT_STRING_ = "cds";
    const std::string OutputFileFormat::XML_FORMAT_STRING_ = "xml";
    const std::string OutputFileFormat::HSPICE_FORMAT_STRING_ = "hspice";
    const std::string OutputFileFormat::TEXT_FORMAT_STRING_ = "text";

    OutputFileFormat::OutputFileFormat() :
            FileFormatBase<OutputFileFormat>()
    {
    }

    OutputFileFormat::OutputFileFormat(const std::string& format) :
            FileFormatBase<OutputFileFormat>(format)
    {
        assert(isValid(format));
    }

    OutputFileFormat OutputFileFormat::skill()
    {
        OutputFileFormat skillFormat(SKILL_FORMAT_STRING_);
        return skillFormat;
    }

    OutputFileFormat OutputFileFormat::xml()
    {
        OutputFileFormat xmlFormat(XML_FORMAT_STRING_);
        return xmlFormat;
    }

    OutputFileFormat OutputFileFormat::hspice()
    {
        OutputFileFormat hspiceFormat(HSPICE_FORMAT_STRING_);
        return hspiceFormat;
    }

    OutputFileFormat OutputFileFormat::text()
    {
        OutputFileFormat textFormat(TEXT_FORMAT_STRING_);
        return textFormat;
    }

    bool OutputFileFormat::operator ==(const OutputFileFormat& other) const
    {
        return getFormat() == other.getFormat();
    }

    bool OutputFileFormat::isValid(const std::string& format) const
    {
        return format == SKILL_FORMAT_STRING_ ||
                format == XML_FORMAT_STRING_ ||
                format == HSPICE_FORMAT_STRING_ ||
                format == TEXT_FORMAT_STRING_;

    }

}
