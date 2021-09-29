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

#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {

    InstanceNamePath::InstanceNamePath() :
        initialized_(false)
    {
    }

    void InstanceNamePath::setIsRoot()
    {
        path_.clear();
        initialized_ = true;
    }

    bool InstanceNamePath::isInitialized() const
    {
        return initialized_;
    }

    bool InstanceNamePath::isRoot() const
    {
        assert(isInitialized());
        return path_.empty();
    }

    void InstanceNamePath::append(const InstanceName& name)
    {
        assert(isInitialized());
        path_.push_back(name);
    }

    InstanceName InstanceNamePath::pop()
    {
        assert(isInitialized());
        assert(!isRoot());
        InstanceName lastName = path_.back();
        path_.pop_back();
        return lastName;
    }

    std::string InstanceNamePath::getPath() const
    {
        return toStr();
    }

    const std::vector<InstanceName> & InstanceNamePath::getPathVector() const
    {
    	assert(isInitialized());
    	return path_;
    }

    std::string InstanceNamePath::toStr() const
    {
        assert(isInitialized());
        std::ostringstream tempStream;
        tempStream << "/";
        Path::const_iterator it = path_.begin();
        while(it != path_.end()) {
            const InstanceName & instanceName = *it;
            tempStream << instanceName;
            it++;
            tempStream << "/";
        }
        return tempStream.str();
    }

    bool InstanceNamePath::operator ==(const InstanceNamePath& other) const
    {
        assert(isInitialized());
        return path_ == other.path_;
    }

    bool InstanceNamePath::operator !=(const InstanceNamePath& other) const
    {
        return !(*this == other);
    }

    bool InstanceNamePath::operator <(const InstanceNamePath& other) const
    {
        assert(isInitialized());
        return path_ < other.path_;
    }

    bool InstanceNamePath::operator >(const InstanceNamePath& other) const
    {
        return !(*this < other) && !(*this == other);
    }
}
