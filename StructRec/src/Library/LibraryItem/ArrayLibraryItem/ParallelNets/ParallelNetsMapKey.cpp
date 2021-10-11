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

#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNetsMapKey.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

    ParallelNetsMapKey::ParallelNetsMapKey()
    {
    	netKey_.clear();
    }

    void ParallelNetsMapKey::setTechType(Core::TechType techType)
    {
        techType_ = techType;
    }

    void ParallelNetsMapKey::addNet(const Core::Net& net)
    {
        parallelNets_.push_back(&net);
        netKey_ += net.getIdentifier().toStr();
    }

    const Core::TechType& ParallelNetsMapKey::getTechType()const
    {
        assert(hasTechType());
        return techType_;
    }

    const std::string& ParallelNetsMapKey::getNetKey() const
    {
    	assert(hasNetKey());
    	return netKey_;
    }

    bool ParallelNetsMapKey::operator <(const ParallelNetsMapKey& other) const
    {
        if(getTechType() == other.getTechType()) {
            return getNetKey() < other.getNetKey();
        } else {
            return getTechType() < other.getTechType();
        }
    }

    bool ParallelNetsMapKey::operator==(const ParallelNetsMapKey& other) const
	{
    	return (getTechType() == other.getTechType() && getNetKey() == other.getNetKey());
	}

    bool ParallelNetsMapKey::operator!=(const ParallelNetsMapKey& other) const
    {
    	return !(*this == other);
    }

    bool ParallelNetsMapKey::hasTechType() const
    {
        return techType_.isInitialized();
    }

    bool ParallelNetsMapKey::hasNetKey() const
    {
        return !netKey_.empty();
    }

    bool ParallelNetsMapKey::hasParallelNets() const
    {
        return !parallelNets_.empty();
    }

    const ParallelNetsMapKey::ParallelNets& ParallelNetsMapKey::getParallelNets() const
    {
        assert(hasParallelNets());
        return parallelNets_;
    }

}


