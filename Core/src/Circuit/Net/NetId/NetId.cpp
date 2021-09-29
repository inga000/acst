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

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Net/NetId/NetPath.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {

    NetId::NetId():
            netPath_(new NetPath())
    {
    }

    NetId::~NetId()
    {
        delete netPath_;
    }

    NetId::NetId(const NetId& other):
        netPath_(new NetPath(other.getNetPath()))
    {
    }

    void NetId::operator =(const NetId& other)
    {
        netPath_ = new NetPath(other.getNetPath());
    }

    void NetId::setPath(const InstanceNamePath& path)
    {
       getNetPath().setPath(path);
    }

    void NetId::setName(const NetName& netName)
    {
        getNetPath().setName(netName);
    }

    bool NetId::isInitialized() const
    {
        return getNetPath().isInitialized();
    }

    bool NetId::hasNetPath() const
    {
        return netPath_ != NULL;
    }

    NetPath& NetId::getNetPath() const
    {
        assert(hasNetPath());
        return *netPath_;
    }

    const InstanceNamePath NetId::getInstanceNamePath()
    {
    	return getNetPath().getInstanceNamePath();
    }

    std::string NetId::toStr() const
    {
    	return getNetPath().toStr();
    }

    std::string NetId::getName() const
    {
        return getNetPath().getName();
    }

    std::string NetId::getGuiName() const
    {
        return toStr();
    }

    std::string NetId::getFlatName() const
    {
       const std::vector<InstanceName> instanceNameVector = getNetPath().getInstanceNamePath().getPathVector();
       std::ostringstream oss;

       for(auto & instanceName : instanceNameVector)
       {
    	   oss << instanceName.toStr() << "Y";
       }
       oss << getNetPath().getName();

       return oss.str();
    }

    bool NetId::operator ==(const NetId& other) const
    {
        assert(isInitialized());
        return getNetPath() == other.getNetPath();
    }

    bool NetId::operator !=(const NetId& other) const
    {
        return !(*this == other);
    }

    bool NetId::operator <(const NetId& other) const
    {
        assert(isInitialized());
        return getNetPath() < other.getNetPath();
    }

    bool NetId::operator >(const NetId& other) const
    {
        return !(*this < other) && !(*this == other);
    }
}
