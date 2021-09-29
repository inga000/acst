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
#include "Core/incl/Circuit/Terminal/TerminalType.h"

namespace Core {

	const std::string TerminalType::INPUT_ = "Input";
	const std::string TerminalType::OUTPUT_ = "Output";
	const std::string TerminalType::UNDEFINED_ = "Undefined";
	const std::string TerminalType::INPUTOUTPUT_ = "InputOutput";

	TerminalType::TerminalType():
			terminalTypeString_(UNDEFINED_)
	{
	}

	TerminalType::~TerminalType()
	{
	}

	bool TerminalType::isInputTerminal() const
	{
		return getTerminalTypeString() == INPUT_;
	}

	bool TerminalType::isOutputTerminal() const
	{
		return getTerminalTypeString() == OUTPUT_;
	}

	bool TerminalType::isInputOutputTerminal() const
	{
		return getTerminalTypeString() == INPUTOUTPUT_;
	}

	TerminalType TerminalType::input()
	{
		return TerminalType(TerminalType::INPUT_);
	}

	TerminalType TerminalType::output()
	{
		return TerminalType(TerminalType::OUTPUT_);
	}

	TerminalType TerminalType::inputOutput()
	{
		return TerminalType(TerminalType::INPUTOUTPUT_);
	}

	bool TerminalType::isInitialized() const
	{
		return terminalTypeString_ != UNDEFINED_;
	}

	bool TerminalType::operator ==(const TerminalType& other) const
	{
		return getTerminalTypeString() == other.getTerminalTypeString();
	}

	std::string TerminalType::toStr() const
	{
		return getTerminalTypeString();
	}

	TerminalType::TerminalType(const std::string& terminalType):
			terminalTypeString_(terminalType)
	{
	}

	std::string TerminalType::getTerminalTypeString() const
	{
		assert(isInitialized());
		return terminalTypeString_;
	}

}


