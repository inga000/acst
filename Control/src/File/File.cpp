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

#include "Control/incl/File/File.h"
#include "Core/incl/Common/BacktraceAssert.h"

#include <iostream>
namespace Control {

    const std::string File::pathNotSet = "";
    const std::string File::stdInPath = "-";
    const std::string File::stdOutPath = "-";

    File::File() :
            path_(pathNotSet)
    {
    }

    File::~File()
    {
    }

    File::File(const File& other)
    {
        path_ = other.path_;
    }

    void File::operator =(const File& other)
    {
        path_ = other.path_;
    }

    std::string File::toStr() const
    {
        assert(hasPath(), "FilePath not set!");
        return path_;
    }

    bool File::isStdIn() const
    {
        return path_ == stdInPath;
    }

    bool File::isStdOut() const
    {
        return path_ == stdOutPath;
    }

    void File::setPath(const std::string& filePath)
    {
        path_ = filePath;
    }

    bool File::isInitialized() const
    {
        return hasPath();
    }

    std::string File::getPath() const
    {
        assert(hasPath());
        return path_;
    }

    bool File::hasPath() const
    {
        return path_ != pathNotSet;
    }

}

