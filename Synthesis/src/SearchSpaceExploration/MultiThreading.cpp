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

#include "../../incl/SearchSpaceExploration/MultiThreading.h"
#include "Synthesis/incl/SearchSpaceExploration/OpAmpSizing.h"
#include "Synthesis/incl/Library/Circuit.h"

#include "Log/incl/LogMacros.h"
#include <thread>
#include <tuple>
#include <math.h>
#include <sstream>

namespace Synthesis
{
	std::tuple<int, int, int> MultiThreading::calculate(int maxAmountOfThreads)
	{
		int hardwareCon = std::thread::hardware_concurrency();
		int supportedThreads = hardwareCon == 0 ? 2 : hardwareCon;

		int amountOfThreads = supportedThreads > maxAmountOfThreads ? maxAmountOfThreads : supportedThreads;
		logDebug("Amount of threads: " << amountOfThreads);

		int runsPerThread = std::ceil(static_cast<float>(maxAmountOfThreads)/ static_cast<float>(amountOfThreads));
		logDebug("runsPerThread: " << runsPerThread);

		int startOffsetLast = runsPerThread * (amountOfThreads -1);
		logDebug("startOffset last: " << startOffsetLast);

		int runsPerThreadLast = maxAmountOfThreads - startOffsetLast;
		logDebug("runsPerThread last: " << runsPerThreadLast);

		return {amountOfThreads, runsPerThread, startOffsetLast};
	}
}
