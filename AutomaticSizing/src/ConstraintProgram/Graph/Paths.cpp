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



#include "AutomaticSizing/incl/ConstraintProgram/Graph/Paths.h"

#include "Log/incl/LogMacros.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include <sstream>

namespace AutomaticSizing {

	Paths::Paths() :
		isInitialized_(false)
	{
	}

	void Paths::initializeEmpty()
	{
		isInitialized_= true;
	}

	void Paths::add(const Path& path)
	{
		assert(isInitialized());
		if(path.isInputToGroundPath())
			inputToGroundPaths_.push_back(path);
		else if(path.isOutputToGroundPath())
			outputToGroundPaths_.push_back(path);
		else if(path.isVddToBiasPath())
			vddToBiasInputPaths_.push_back(path);
		else if(path.isVddToGroundPath())
			vddToGroundPaths_.push_back(path);
		else
		{
			logDebug(path);
			assert(false, "Path Type of could not be determined correctly!");
		}

	}



	const std::vector<Path>& Paths::getInputToGroundPaths() const
	{
		assert(!inputToGroundPaths_.empty());
		return inputToGroundPaths_;
	}

	const std::vector<Path>& Paths::getVddToGroundPaths() const
	{
		assert(!vddToGroundPaths_.empty());
		return vddToGroundPaths_;
	}

	const std::vector<Path>& Paths::getOutputToGroundPaths() const
	{
		assert(!outputToGroundPaths_.empty());
		return outputToGroundPaths_;
	}

	const std::vector<Path>& Paths::getVddToBiasInputPaths() const
	{
		assert(!vddToBiasInputPaths_.empty());
		return vddToBiasInputPaths_;
	}

	bool Paths::isInitialized() const
	{
		return isInitialized_;
	}

	std::string Paths::toStr() const
	{
		assert(isInitialized());
		std::ostringstream oss;
		oss << "Input to Ground Paths:\n";
		for(PathVector::const_iterator it = inputToGroundPaths_.begin(); it != inputToGroundPaths_.end(); it++)
		{
			Path path = *it;
			oss << path << "\n";
		}
		oss << "Vdd to Ground Paths:\n";
		for(PathVector::const_iterator it = vddToGroundPaths_.begin(); it != vddToGroundPaths_.end(); it++)
		{
			Path   path = *it;
			oss << path << "\n";
		}
		oss << "Output to Ground Paths:\n";
		for(PathVector::const_iterator it = outputToGroundPaths_.begin(); it != outputToGroundPaths_.end(); it++)
		{
			Path path = *it;
			oss << path << "\n";
		}
		oss << "Vdd to Bias Input Paths:\n";
		for(PathVector::const_iterator it = vddToBiasInputPaths_.begin(); it != vddToBiasInputPaths_.end(); it++)
		{
			Path path = *it;
			oss << path << "\n";
		}
		return oss.str();
	}
}







