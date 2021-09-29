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

#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructureInstanceTerminal::StructureInstanceTerminal() :
			instance_(nullptr),
			net_(nullptr),
			terminal_(nullptr)
	{
	}

	void StructureInstanceTerminal::setInstance(
		StructureInstance& instance)
	{
		instance_ = & instance;
	}

	void StructureInstanceTerminal::setTerminal(
		StructureTerminal& terminal)
	{
		terminal_ = & terminal;
	}

	void StructureInstanceTerminal::connect(StructureNet& net)
	{
		assert(!isConnected());
		registerInstanceTerminalAtNet(net);
		net_ = & net;
	}

	bool StructureInstanceTerminal::isConnected() const
	{
		return net_ != nullptr;
	}

	const StructureTerminal& StructureInstanceTerminal::getTerminal() const
	{
		assert(hasTerminal());
		return * terminal_;
	}

	const StructureNet& StructureInstanceTerminal::getTerminalNet() const
	{
		return getTerminal().getNet();
	}

	Core::TerminalId StructureInstanceTerminal::getTerminalId() const
	{
		return getTerminal().getTerminalId();
	}

	StructureInstance & StructureInstanceTerminal::getInstance() const
	{
		assert(hasInstance());
		return * instance_;
	}

	const StructureNet & StructureInstanceTerminal::getNet() const
	{
		assert(isConnected());
		return *net_;
	}

	bool StructureInstanceTerminal::hasInstance() const
	{
		return instance_ != nullptr;
	}

	bool StructureInstanceTerminal::hasTerminal() const
	{
		return terminal_ != nullptr;
	}

	void StructureInstanceTerminal::registerInstanceTerminalAtNet(
		StructureNet& net)
	{
		net.addInstanceTerminal(*this);
	}

	std::string StructureInstanceTerminal::toStr() const
	{
		std::ostringstream stream;
		stream << "Net: " << net_ << ", " << "Term: " << terminal_->getTerminalId();
		return stream.str();
	}

}

