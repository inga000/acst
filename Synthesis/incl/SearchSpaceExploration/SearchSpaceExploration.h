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

#ifndef SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_SEARCHSPACEEXPLORATION_H_
#define SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_SEARCHSPACEEXPLORATION_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/TechType.h"

#include <string>
#include <vector>


namespace Core {

	class Circuit;

}

namespace AutomaticSizing {

	class CircuitInformation;
	class Graph;

}
// Use this class if HSpice files are used for Synthesis
// It sizes the circuits

namespace Synthesis
{
	class Library;
	class Result;
	class Circuit;

	class SearchSpaceExploration
	{
	public:
		SearchSpaceExploration();
		~SearchSpaceExploration();

		void setLibrary(const Library & library);
		void setCircuitInformation(const AutomaticSizing::CircuitInformation & circuitInformation);
		void setTransistorModel(std::string transistorModel);
		void setDefinition(std::string definition);

		const Result & compute();

	private:
		const Library & getLibrary() const;
		const AutomaticSizing::CircuitInformation & getCircuitInformation() const;
		Result & getResult();
		std::string getTransistorModel() const;
		std::string getDefinition() const;

		std::vector<Circuit*> testOneStageOpAmps();
		void testSymmetricalOpAmps();
		void testTwoStageOpAmps(std::vector<Circuit*> firstStages);

		const AutomaticSizing::CircuitInformation &  createCircuitInformationForCircuit(Core::NetId outputNet, Core::TechType dopingBiasTran);
		Core::NetId createNetId(std::string netName);
		Core::TechType findDopingBiasTran(const Core::Circuit & circuit);

		Circuit & testCircuit(Circuit & circuit);
		void runStrictSearch(Circuit & circuit);
		Circuit & runOptimizingSearch(Circuit & circuit);

		AutomaticSizing::Graph &computeGraph(Circuit &circuit);
	private:
		Result * result_;
		const Library * library_;
		const AutomaticSizing::CircuitInformation * circuitInformation_;
		std::string transistorModel_;
		std::string definition_;
	};


}



#endif /* SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_SEARCHSPACEEXPLORATION_H_ */
