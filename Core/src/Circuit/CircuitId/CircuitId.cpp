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

#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Core {

    const  int CircuitId::NOT_INITIALIZED_ = -100;

    CircuitId::CircuitId() :
        circuitName_(new CircuitName()),
        id_(NOT_INITIALIZED_)
    {
        
    }

    CircuitId::~CircuitId()
    {
        delete circuitName_;
    }

    CircuitId::CircuitId(const CircuitId& other) :
        circuitName_(new CircuitName(*other.circuitName_)),
        id_(other.id_),
        techType_(other.techType_)
    {
    }

    void CircuitId::operator =(const CircuitId& other)
    {
        circuitName_ = new CircuitName(*other.circuitName_);
        id_ = other.getId();
        techType_ = other.getTechType();
    }

    void CircuitId::setCircuitName(const CircuitName& name)
    {
        *circuitName_ = name;
    }

    void CircuitId::setId(int id)
    {
        id_ = id;
    }

    void CircuitId::setTechType(TechType techType)
    {
        techType_ = techType;
    }

    TechType CircuitId::getTechType() const
    {
        assert(techType_.isInitialized());
        return techType_;
    }

    CircuitName CircuitId::getCircuitName() const
    {
        assert(circuitName_->isInitialized());
        return *circuitName_;
    }

    int CircuitId::getId() const
    {
        assert(id_!=NOT_INITIALIZED_);
        return id_;
    }

    std::string CircuitId::toStr() const
    {
        assert(isInitialized());
        std::ostringstream oss;
        oss << getCircuitName().toStr() << "[" << techType_.toStr() << ", " << id_ << "]";
        return oss.str();
    }

    std::string CircuitId::getName() const
    {
        assert(circuitName_->isInitialized());
        return getCircuitName().toStr();
    }


    bool CircuitId::isInitialized() const
    {
        return circuitName_->isInitialized() && id_ != NOT_INITIALIZED_ && techType_.isInitialized();
    }

    bool CircuitId::operator ==(const CircuitId& other) const
    {
        assert(isInitialized());
        if(id_ == 0 || other.getId() == 0)
            return getCircuitName() == other.getCircuitName();
        else
            return getCircuitName() == other.getCircuitName() 
                    &&  getId() == other.getId() 
                    && getTechType() ==  other.getTechType();

    }

    bool CircuitId::operator !=(const CircuitId& other) const
    {
        assert(isInitialized());
        return !(*this == other);
    }

    bool CircuitId::operator <(const CircuitId& other) const
    {
        assert(isInitialized());
        if(getCircuitName() != other.getCircuitName())
            return getCircuitName() < other.getCircuitName();
        else if(getTechType() != other.getTechType())
            return getTechType() < other.getTechType();
        else
            return getId() < other.getId();
    }

    bool CircuitId::operator >(const CircuitId& other) const
    {
        assert(isInitialized());
        return !(*this < other) && !(*this == other);
    }

}
