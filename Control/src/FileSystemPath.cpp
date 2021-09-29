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


#include "Control/incl/FileSystemPath.h"
#include "Control/incl/Control.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Control {

    const size_t  FileSystemPath::END_OF_STRING = std::string::npos;


    FileSystemPath::FileSystemPath()
    {
    }

    void FileSystemPath::setPath(const std::string& path)
    {
        this->path_ = path;
    }

    std::string FileSystemPath::getPath() const
    {
        assert(hasPath());
        return path_;
    }

    std::string FileSystemPath::getAbsolutePathRelativeToFile(const std::string& filePath) const
    {
    	std::string basePathWithoutFile = stripFileFromPath(filePath);

    	if(isAbsolute())
        {
            return getPath();
        }
        else if (getPath().compare(0, basePathWithoutFile.size(), basePathWithoutFile) == 0)
        {
        	return getPath();
        }
        else
        {
            return basePathWithoutFile + getPath();
        }
    }

    bool FileSystemPath::isAbsolute() const
    {
        return path_[0] == '/';
    }

    bool FileSystemPath::hasPath() const
    {
        return !path_.empty();
    }

    std::string FileSystemPath::stripFileFromPath(const std::string& filePath)
    {
        size_t position = findPositionOfLastSlash(filePath);
        if  (position != END_OF_STRING) {
            std::string pathWithoutFile = filePath.substr(0,position+1);
            return pathWithoutFile;
        }
        else {
            std::string emptyString("");
            return emptyString;
        }
    }

    size_t FileSystemPath::findPositionOfLastSlash(const std::string& filePath)
    {
        return filePath.rfind(std::string("/"));
    }

    std::string FileSystemPath::getRelativeToFilePath(const std::string& filePath) const
    {
    	std::string basePathWithoutFile = stripFileFromPath(filePath);
    	if (getPath().compare(0, basePathWithoutFile.size(), basePathWithoutFile) == 0)
    	{
    	    return getPath().substr(basePathWithoutFile.size());
    	}
    	else if(isAbsolute())
    	{
    		return getPath();
    	}
    	else
    	{
    		return "Unsupported Path relations";
    	}

    }

    std::string FileSystemPath::getNameFromPath(const std::string filePath) const
    {
    	size_t positionOfLastSlash = findPositionOfLastSlash(filePath);
    	size_t positionOfLastPoint = filePath.rfind(std::string("."));

    	if(positionOfLastSlash != std::string::npos)
    	{
    	 	std::string nameWithoutSlash = filePath.substr(positionOfLastSlash +1, positionOfLastPoint - positionOfLastSlash-1);

    	 	return nameWithoutSlash;
    	}
    	else
    	{
    	    std::string nameWithoutXML = filePath.substr(0, positionOfLastPoint-1);
    	    return nameWithoutXML;
    	}
    }

}
