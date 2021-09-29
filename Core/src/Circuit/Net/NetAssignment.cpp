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

#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace Core {

    NetAssignment::NetAssignment()
    {
    }

    void NetAssignment::setNetIdentifier(const NetId& netId)
    {
        netId_ = netId;
    }

    void NetAssignment::setPropertyName(const PropertyName& propertyName)
    {
        propertyName_ = propertyName;
    }

    bool NetAssignment::isInitialized() const
    {
        return hasNetIdentifier() && hasPropertyName();
    }

    NetId NetAssignment::getNetIdentifier() const
    {
        assert(hasNetIdentifier());
        return netId_;
    }

    PropertyName NetAssignment::getPropertyName() const
    {
        assert(hasPropertyName());
        return propertyName_;
    }

    std::string NetAssignment::toStr() const
    {
        std::ostringstream oss;
        oss << "[" << getPropertyName() << "," << getNetIdentifier() << "]";
        return oss.str();
    }

    bool NetAssignment::operator ==(const NetAssignment& other) const
    {
        return getNetIdentifier() == other.getNetIdentifier() && getPropertyName() == other.getPropertyName();
    }

    bool NetAssignment::hasNetIdentifier() const
    {
        return netId_.hasNetPath();
    }

    bool NetAssignment::hasPropertyName() const
    {
        return propertyName_.isInitialized();
    }

}
