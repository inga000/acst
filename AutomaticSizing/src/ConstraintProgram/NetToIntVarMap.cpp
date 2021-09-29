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


#include "Partitioning/incl/Results/Result.h"

#include "AutomaticSizing/incl/ConstraintProgram/NetToIntVarMap.h"
#include "AutomaticSizing/incl/Results/Result.h"

#include <gecode/minimodel.hh>

#include "Log/incl/LogMacros.h"
namespace AutomaticSizing {


	NetToIntVarMap::NetToIntVarMap()
	{
	}

	void NetToIntVarMap::add(Core::NetId netId,
		const Gecode::IntVar& intVar)
	{
		assert(!has(netId));
		intVarMap_[netId] = intVar;
	}

	Gecode::IntVar NetToIntVarMap::find(
		const Core::NetId netId) const
	{
		assert(has(netId));
		Gecode::IntVar intVar = intVarMap_.at(netId);
		return intVar;
	}

	bool NetToIntVarMap::has(
		const Core::NetId netId) const
	{
		return intVarMap_.find(netId) != intVarMap_.end();
	}

	void NetToIntVarMap::write(Result & result) const
	{
		for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it ++)
		{
			Core::NetId netId = it->first;
			Gecode::IntVar voltage = it->second;
			float floatVoltage = voltage.med();
			result.addVoltage(netId,floatVoltage / 1000);
		}
	}


	 std::string NetToIntVarMap::toStr() const
	{
		std::ostringstream oss;
		oss << std::endl;
		for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		{
		 oss << it->first << " " << it->second << std::endl;

		}
		return oss.str();
	}

	 void NetToIntVarMap::setVoltages(Gecode::Space & space)
	 {
		 IntVarMap::const_iterator it = intVarMap_.begin();
		 Gecode::rel(space, it->second == 0);
		 it++;
		 Gecode::rel(space, it->second == 0.7);
		 it++;
		 Gecode::rel(space, it->second == 1.5);
		 it++;
		 Gecode::rel(space, it->second == 1.5);
		 it++;
		 Gecode::rel(space, it->second == 1.2);
		 it++;
		 Gecode::rel(space, it->second == 1.5);
		 it++;
		 Gecode::rel(space, it->second == 2.22);
		 it++;
		 Gecode::rel(space, it->second == 1.1);
		 it++;
		 Gecode::rel(space, it->second == 2.3);
		 it++;
		 Gecode::rel(space, it->second == 2.3);
		 it++;
		 Gecode::rel(space, it->second == 0.54);
		 it++;
		 Gecode::rel(space, it->second == 0.54);
		 it++;
		 Gecode::rel(space, it->second == 1.5);
		 it++;
		 Gecode::rel(space, it->second == 3);

	 }


}

