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

#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"

namespace StructRec {

	StructureTerminal::StructureTerminal() :
			circuit_(nullptr),
			net_(nullptr)
	{
	}

	void StructureTerminal::setCircuit(const StructureCircuit& circuit)
	{
		circuit_ = & circuit;
	}

	void StructureTerminal::setTerminalId(const Core::TerminalId& terminalId)
	{
		terminalId_ = terminalId;
	}

	void StructureTerminal::setTerminalType(const Core::TerminalType& terminalType)
	{
		terminalType_ = terminalType;
	}

	void StructureTerminal::setNet(StructureNet& net)
	{
		net_ = &net;
	}

	void StructureTerminal::setAndConnectNet(StructureNet& net)
	{
		net_ = &net;
		net_->setTerminal(*this);
	}

	Core::TerminalId StructureTerminal::getTerminalId() const
	{
		assert(terminalId_.isIntialized());
		return terminalId_;
	}

	Core::TerminalType StructureTerminal::getTerminalType() const
	{
		assert(terminalType_.isInitialized());
		return terminalType_;
	}


	const StructureNet & StructureTerminal::getNet() const
	{
		assert(net_ != nullptr);
		return *net_;
	}

	const StructureCircuit & StructureTerminal::getCircuit() const
	{
		assert(circuit_ != nullptr);
		return *circuit_;
	}

	bool StructureTerminal::hasTerminalType() const
	{
		return terminalType_.isInitialized();
	}

	std::string StructureTerminal::toStr() const
	{
		std::ostringstream stream;
		 stream << "(T:" <<terminalId_ << ", " << "N:" << net_ << ")";
		 return stream.str();
	}

}

