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


#include "Control/incl/File/XmlInputFile.h"
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include "Log/incl/LogMacros.h"

namespace Control {

    XmlInputFile::XmlInputFile() :
        isBuiltIn_(false),
        buffer_(NULL),
        doc_(new Core::XmlDocument),
        parsingDone_(false)
    {
    }

    XmlInputFile::~XmlInputFile()
    {
        delete doc_;
        delete[] buffer_;
    }

    void XmlInputFile::setIsBuiltIn()
    {
        isBuiltIn_ = true;
    }

    bool XmlInputFile::isBuiltIn() const
    {
        return isBuiltIn_;
    }

    const Core::XmlDocument& XmlInputFile::getDocumentNode() const
    {
        assert(parsingDone_);
        return *doc_;
    }

    bool XmlInputFile::hasNode(const Core::XmlNode& node, const std::string& nodeName) const
    {
    	return node.first_node(nodeName.c_str()) != NULL;
    }

    void XmlInputFile::setFileContent(const std::string& string)
    {
        assert(isBuiltIn());
        assert(!hasFileContent());
        buffer_ = new char [string.length()+1];
        std::strcpy(buffer_, string.c_str());
    }

    void XmlInputFile::readFileIntoBuffer()
    {
        open();
        std::istream& in = getStream();
        in.seekg(0, std::ios::end);
        int length = static_cast<int> (in.tellg()) + 1;
        in.seekg(0, std::ios::beg);

        buffer_ = new char[length];

        buffer_[length - 1] = 0;
        in.read(buffer_, length - 1);
        close();
    }

    void XmlInputFile::parse()
    {
        if(!isBuiltIn()) {
            readFileIntoBuffer();
        } else {
            assert(hasFileContent(), "File has no content!");
        }

        parseXml();
        parsingDone_ = true;
    }


    void XmlInputFile::parseXml()
    {
        try {
            doc_->parse<0> (buffer_);

        }
        catch (rapidxml::parse_error & e) {
           if (!isBuiltIn()) {
               logDebug("Parsing of file failed:" << getPath());
               logDebug(e.what());
           } else {
               logDebug("Parsing of built in file failed");
           }
           throw;
        }
    }

    bool XmlInputFile::hasFileContent() const
    {
        return buffer_ != NULL;
    }

}


