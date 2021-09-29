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

#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/Net.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {

    Terminal::Terminal() :
            net_(NULL)
    {
    }

    void Terminal::setNet(const Net& net)
    {
        net_ = &net;
    }

    void Terminal::setTerminalId(const TerminalId& identifier)
    {
    	identifier_ = identifier;
    }

    void Terminal::setTerminalType(const TerminalType& terminalType)
    {
    	terminalType_ = terminalType;
    }

    const Net& Terminal::getNet() const
    {
        assert(net_ != NULL);
        return *net_;
    }

    NetId Terminal::getNetId() const
    {
    	return getNet().getIdentifier();
    }

    TerminalId Terminal::getIdentifier() const
    {
        assert(identifier_.isIntialized());
        return identifier_;
    }

    TerminalType Terminal::getTerminalType() const
    {
    	return terminalType_;
    }

    std::string Terminal::toStr() const
    {
        std::stringstream stream;
        stream << "(T: " <<identifier_ << ", " << "N: " << net_->getIdentifier() << ", IO: " << terminalType_ << ")";
        return stream.str();
    }

    bool Terminal::isInputTerminal() const
    {
    	return terminalType_.isInputTerminal();
    }

    bool Terminal::isOutputTerminal() const
    {
    	return terminalType_.isOutputTerminal();
    }

    bool Terminal::operator==(const Terminal& other) const
	{
    	return getNet() == other.getNet() && getIdentifier() == other.getIdentifier() && getTerminalType() == other.getTerminalType();
	}

    bool Terminal::operator<(const Terminal& other) const
    {
    	return getIdentifier() < other.getIdentifier();
    }
}


