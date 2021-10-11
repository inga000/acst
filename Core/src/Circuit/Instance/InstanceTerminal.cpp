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

#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"

namespace Core {

    InstanceTerminal::InstanceTerminal():
            instance_(NULL),
            net_(NULL),
            terminal_(NULL)
    {
    }

    void InstanceTerminal::connect(Net& net)
    {
        assert(!isConnected(), "Is already connected");
        net.addInstanceTerminal(*this);
        this->net_ = &net;
    }

    void InstanceTerminal::setInstance(Instance& instance)
    {
        this->instance_ = &instance;
    }

    void InstanceTerminal::setTerminal(const Terminal& terminal)
    {
        this->terminal_ = &terminal;
    }

    bool InstanceTerminal::isConnected() const
    {
        return net_ != NULL;
    }

    const Net& InstanceTerminal::getNet() const
    {
        assert(isConnected());
        return *net_;
    }

    Net& InstanceTerminal::getNet()
    {
        assert(isConnected());
        return *net_;
    }

    const Instance& InstanceTerminal::getInstance() const
    {
        assert(hasInstance());
        return *instance_;
    }

    Instance& InstanceTerminal::getInstance()
    {
        assert(hasInstance());
        return *instance_;
    }

    const Terminal& InstanceTerminal::getTerminal() const
    {
        assert(hasTerminal());
        return *terminal_;
    }

    TerminalId InstanceTerminal::getIdentifier() const
    {
        return getTerminal().getIdentifier();
    }

    bool InstanceTerminal::isInputTerminal() const
    {
    	return getTerminal().isInputTerminal();
    }

    bool InstanceTerminal::isOutputTerminal() const
    {
    	return getTerminal().isOutputTerminal();
    }

    void InstanceTerminal::print(std::ostream& stream) const
    {
        if(net_ != nullptr)
        {
            stream << "Net: " << net_->toStr() << " Term: " << terminal_;
        }
        else
        {
            stream << "NO NET!!!! Term: " << terminal_;
        }
        
    }

    bool InstanceTerminal::hasEqualTerminalAndConnection(const InstanceTerminal& other) const
    {
        return getIdentifier() == other.getIdentifier() &&
                hasEqualNetConnection(other);
    }

    bool InstanceTerminal::hasInstance() const
    {
        return instance_ != NULL;
    }

    bool InstanceTerminal::hasTerminal() const
    {
        return terminal_ != NULL;
    }

    bool InstanceTerminal::hasEqualNetConnection(const InstanceTerminal& other) const
    {
        if(isConnected() && other.isConnected()) {
            return getNet().getIdentifier() == other.getNet().getIdentifier();
        } else {
            return isConnected() == other.isConnected();
        }
    }

    bool InstanceTerminal::operator ==(const InstanceTerminal& other) const
    {
    	assert(hasInstance() && hasTerminal());
        if(isConnected() && other.isConnected())
        {
        	return getIdentifier() == other.getIdentifier() && getNet().getIdentifier() == other.getNet().getIdentifier()
        			&& getInstance().getIdentifier() == other.getInstance().getIdentifier();
        }
        else if(isConnected() || other.isConnected())
        {
        	return false;
        }
        else
        {
        	return getIdentifier() == other.getIdentifier()
        			&& getInstance().getIdentifier() == other.getInstance().getIdentifier();
        }

    }

    bool InstanceTerminal::operator !=(const InstanceTerminal& other) const
    {
    	assert(hasInstance() && hasTerminal());
        return !(*this == other);
    }


}


