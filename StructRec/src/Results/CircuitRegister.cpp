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

#include "StructRec/incl/Results/CircuitRegister.h"
#include <sstream>

namespace StructRec {

	CircuitRegister::CircuitRegister()
	{
	}

	CircuitRegister::~CircuitRegister()
	{
		eraseAllStructureCircuits();
	}

	void CircuitRegister::addStructureCircuits(const Core::CircuitId& circuitId, StructureCircuits& results)
	{
		structureCircuitsMap_.insert(StructureCircuitsMap::value_type( circuitId, &results));
	}

	bool CircuitRegister::hasStructureCircuits(const Core::CircuitId& circuitId) const
	{
		return structureCircuitsMap_.find(circuitId) != structureCircuitsMap_.end();
	}

	StructureCircuits& CircuitRegister::findStructureCircuits(const Core::CircuitId& circuitId) const
	{
		assert(hasStructureCircuits(circuitId));
		StructureCircuits* results = structureCircuitsMap_.at(circuitId);
		return *results;
	}

	void CircuitRegister::removeDominatedStructures(const DominanceRelation& dominanceRelation, int level)
	{
		for(StructureCircuitsMap::const_iterator it = structureCircuitsMap_.begin(); it != structureCircuitsMap_.end(); it++)
		{
			StructureCircuits* circuits = it->second;
			dominanceRelation.removeDominatedStructures(*circuits, level);
		}
	}

	void CircuitRegister::removeDominatedStructures(const DominanceRelationForCurrentMirrors& dominanceRelation, int level)
	{
		for(StructureCircuitsMap::const_iterator it = structureCircuitsMap_.begin(); it != structureCircuitsMap_.end(); it++)
		{
			StructureCircuits* circuits = it->second;
			dominanceRelation.removeDominatedStructures(*circuits, level);
		}
	}

	void CircuitRegister::removeUncertainStructures()
	{
		for(StructureCircuitsMap::const_iterator it = structureCircuitsMap_.begin(); it != structureCircuitsMap_.end(); it++)
		{
			StructureCircuits* circuits = it->second;
			circuits->removeUncertainStructures();
		}
	}

	std::string CircuitRegister::toStr() const
	{
		std::ostringstream stream;
		stream << "Registered Circuits: " << std::endl;
		for(StructureCircuitsMap::const_iterator it = structureCircuitsMap_.begin(); it != structureCircuitsMap_.end(); it++)
		{
			const Core::CircuitId id = it->first;
			stream << "CircuitName " << id << std::endl;
			StructureCircuits * results = it->second;
			stream << results;
		}
		stream << std::endl;
		return stream.str();
	}


	void CircuitRegister::eraseAllStructureCircuits()
	{
		for(StructureCircuitsMap::const_iterator it = structureCircuitsMap_.begin(); it != structureCircuitsMap_.end(); it++)
		{
				StructureCircuits * currentCircuits = it->second;
				delete currentCircuits;
		}
	}


}

