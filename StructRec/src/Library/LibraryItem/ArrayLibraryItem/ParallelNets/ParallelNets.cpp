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
/*
* ParallelNetsRule.cpp
 *
 *  Created on: Apr 13, 2016
 *      Author: ga76fuw
 */

#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNets.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	ParallelNets::ParallelNets()
	{
	}

	ParallelNets::ParallelNets(const ParallelNets& other):
		parallelNets_(other.parallelNets_)
	{

	}

	ParallelNets::~ParallelNets()
	{
	}

	void ParallelNets::addParallelNet(const Core::PinType& parallelNet)
	{
		parallelNets_.push_back(parallelNet);
	}


	ParallelNetsMapKey ParallelNets::computeParallelNetsMapKey(const Core::Device& device) const
	{
		assert(!parallelNets_.empty());
		ParallelNetsMapKey key;
		key.setTechType(device.getTechType());
		for(PinTypeVector::const_iterator it = parallelNets_.begin(); it != parallelNets_.end(); it++)
		{
			Core::PinType pinType = *it;
			key.addNet(device.findNet(pinType));
		}
		return key;
	}

	bool ParallelNets::isIdentical(const ParallelNets& other) const
	{
		if(parallelNets_.size() != other.parallelNets_.size()) {
			return false;
		}

		bool identicalParallelNets = true;
		for(PinTypeVector::const_iterator it = parallelNets_.begin(); it != parallelNets_.end(); it++)
		{
			Core::PinType currentPinType = *it;
			bool hasPinType = false;
			for(PinTypeVector::const_iterator it2 = other.parallelNets_.begin(); it2 != other.parallelNets_.end(); it2++)
			{
				Core::PinType otherPinType = *it2;
				if(currentPinType == otherPinType)
				{
					hasPinType = true;
					break;
				}
			}

			if(!hasPinType) {
				identicalParallelNets = false;
				break;
			}
		}
		return identicalParallelNets;
	}

}


