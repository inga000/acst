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

#include "Control/incl/File/OutputFile.h"
#include "Core/incl/Common/Object.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <assert.h>

namespace Control {

    OutputFile::OutputFile() :
            isOpenFlag_(false),
            outputFileStream_(NULL)
    {
    }

    OutputFile::~OutputFile()
    {
        //assert(!isOpen());
    }

    OutputFile::OutputFile(const OutputFile& other) :
                File::File(other),
                isOpenFlag_(other.isOpenFlag_),
                outputFileStream_(other.outputFileStream_)
    {
        assert(!isOpen());
    }

    void OutputFile::operator =(const OutputFile& other)
    {
        File::operator =(other);
        isOpenFlag_ = other.isOpenFlag_;
        outputFileStream_ = other.outputFileStream_;
    }

    void OutputFile::open()
    {
        assert(!isOpen());
        if(!isStdOut())
        {
            std::string filePath = getPath();
            outputFileStream_ = new std::ofstream();
            outputFileStream_->open(filePath.c_str());
        }
        isOpenFlag_ = true;
    }

    void OutputFile::close()
    {
        assert(isOpen());
        if(!isStdOut()) {
            outputFileStream_->close();
            delete outputFileStream_;
            outputFileStream_ = NULL;
        }
        isOpenFlag_ = false;
    }

    OutputFile& OutputFile::operator <<(const std::string& stringToWrite)
    {
        assert(isOpen());
        if(!isStdOut()) {
            *outputFileStream_ << stringToWrite;
        } else {
            std::cout << stringToWrite;
        }
        return *this;
    }

    OutputFile& OutputFile::operator <<(const int& intToWrite)
    {
        return operator<<(boost::lexical_cast<std::string>(intToWrite));
    }

    OutputFile& OutputFile::operator <<(const Core::Object& objectToWrite)
    {
        return operator<<(objectToWrite.toStr());
    }

    bool OutputFile::isOpen() const
    {
        return isOpenFlag_;
    }

}
