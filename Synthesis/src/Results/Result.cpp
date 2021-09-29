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

#include "Synthesis/incl/Results/Result.h"
#include "Synthesis/incl/Library/Circuit.h"
#include "Synthesis/incl/Results/HSPICEOutputFile.h"

#include "Log/incl/LogMacros.h"

#include <sstream>

namespace Synthesis
{


	Result::Result()
	{

	}

	Result::~Result()
	{
		eraseCircuits();
	}

	void Result::add(const Circuit & circuit)
	{
		circuits_.push_back(&circuit);
	}

	std::string Result::toStr() const
	{
		std::ostringstream oss;
		oss.clear();

		int num = 1;
		for(auto & circuit : circuits_)
		{
			oss << "<<<<<<<<<<<<<<<<< OPAMP[" << num <<"] >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
			oss << circuit->toStr();
			num++;
		}
		return oss.str();
	}

	void Result::write(std::string HSPICENetlistDirectoryPath, bool usesHSPICELibrary) const
	{
		int num =1;
		for(auto & circuit : circuits_)
		{
			std::ostringstream numString;
			numString << num;
			std::string stringFilePath(HSPICENetlistDirectoryPath + "/OpAmp" + numString.str() + ".hspice");
			HSPICEOutputFile outputFile;
			outputFile.setPath(stringFilePath);
			outputFile.setId("OpAmp" + numString.str());
			outputFile.setUsesHspiceLibrary(usesHSPICELibrary);
			outputFile.write(*circuit);
			num++;
		}
	}


	void Result::eraseCircuits()
	{
		for(auto & circuit : circuits_)
		{
			delete circuit;
		}
	}



}



