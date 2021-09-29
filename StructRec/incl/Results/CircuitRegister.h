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

#ifndef STRUCTREC_INCL_RESULTS_CIRCUITREGISTER_H_
#define STRUCTREC_INCL_RESULTS_CIRCUITREGISTER_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "StructRec/incl/Library/DominanceRelation.h"
#include "StructRec/incl/Library/DominanceRelationForCurrentMirrors.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include <map>

namespace StructRec {

	class CircuitRegister : public Core::Object
	{
	private:
		typedef std::map<Core::CircuitId, StructureCircuits*>  StructureCircuitsMap;
	public:
		CircuitRegister();
		~CircuitRegister();

		void addStructureCircuits(const Core::CircuitId&  circuitId, StructureCircuits & results);

		bool hasStructureCircuits(const Core::CircuitId& circuitId) const;
		StructureCircuits & findStructureCircuits(const Core::CircuitId& circuitId) const;

		void removeDominatedStructures(const DominanceRelation& dominanceRelation, int level);
		void removeDominatedStructures(const DominanceRelationForCurrentMirrors& dominanceRelation, int level);
		void removeUncertainStructures();

		std::string toStr() const;

	private:
		void eraseAllStructureCircuits();

	private:
		StructureCircuitsMap structureCircuitsMap_;
	};

}

#endif /* STRUCTREC_INCL_RESULTS_CIRCUITREGISTER_H_ */
