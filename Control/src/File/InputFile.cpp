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

#include "Control/incl/File/InputFile.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/Backtrace.h"

namespace Control {

    InputFile::InputFile() :
        isOpen_(false),
        hasFileContent_(false),
        inputFile_(NULL)
    {
    }

    InputFile::~InputFile()
    {
        //assert(!isOpen_);
        if(isOpen_ && !isStdIn()) {
            inputFile_->close();
        }
    }

    void InputFile::open()
    {
        assert(!isOpen_, "File already open!");
        if (!isStdIn()) {
            std::string filePath = toStr();
            inputFile_ = new std::ifstream();
            inputFile_->open(filePath.c_str());
            assert(inputFile_->good(), "File not good!");
        }
        isOpen_ = true;
    }

    std::istream& InputFile::getStream()
    {
        assert(isOpen_);
        if (!isStdIn()) {
            return *inputFile_;
        } else {
            return std::cin;
        }
    }

    const std::istream& InputFile::getStream() const
    {
        assert(isOpen_);
        if (!isStdIn()) {
            return *inputFile_;
        } else {
            return std::cin;
        }
    }

    void InputFile::close()
    {
        assert(isOpen_);
        if (!isStdIn()) {
            inputFile_->close();
            delete inputFile_;
            inputFile_ = NULL;
        }
        isOpen_ = false;
    }

    InputFile::InputFile(const InputFile& other) :
        File(other),
        isOpen_(false),
        hasFileContent_(other.hasFileContent_),
        fileContent_(other.fileContent_),
        inputFile_(NULL)
    {
        assert(!isOpen_);
    }

    void InputFile::operator =(const InputFile& other)
    {
        File::operator=(other);
        assert(!isOpen_);
        isOpen_ = false;
        inputFile_ = NULL;
    }

    void InputFile::readCompleteFile()
    {
        open();
        getStream().seekg(0,std::ios::end);
        int length = static_cast<int>(getStream().tellg())+1; // +1 for NULL
        getStream().seekg(0,std::ios::beg);

        char * buffer = new char[length];
        buffer[length-1]=0; // terminate c-string with NULL
        getStream().read(buffer, length-1);

        fileContent_ = std::string(buffer);
        delete[] buffer;
        hasFileContent_ = true;
        close();
    }

    const std::string& InputFile::getFileContent() const
    {
        assert(hasFileContent_);
        return fileContent_;
    }

    std::string& InputFile::getFileContent()
    {
        assert(hasFileContent_);
        return fileContent_;
    }
}

