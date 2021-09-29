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

#ifndef SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_OPAMPSIZING_H_
#define SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_OPAMPSIZING_H_


#include "Core/incl/Circuit/CircuitId/CircuitId.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>


namespace Core {

	class Circuit;

}

namespace AutomaticSizing {

	class CircuitInformation;
	class Graph;

}
//Use this class if functional block library is used for synthesis
//It sizes the op amp created with the functional block library

namespace Synthesis
{
	class FunctionalBlockLibrary;
	class Circuit;

	class OpAmpSizing
	{
	public:
		OpAmpSizing();
		~OpAmpSizing();

		void setLibrary(const FunctionalBlockLibrary & library);
		void setCircuitInformation(const AutomaticSizing::CircuitInformation & circuitInformation);
		void setTransistorModel(std::string transistorModel);
		void setDefinition(std::string definition);
		void setHSpiceNetlistDirectoryPath(std::string path);

		void compute();
		void sizeOpAmpGroup();

		const AutomaticSizing::CircuitInformation & getCircuitInformation() const;

	private:
		const FunctionalBlockLibrary & getLibrary() const;
		std::string getTransistorModel() const;
		std::string getDefinition() const;
		std::string getHSpiceNetlistDirectoryPath() const;

		void testSimpleOrFullyDifferentialOpAmps();
		void testSimpleOrFullyDifferentialOpAmpsLoop(const std::vector<Circuit*>& oneStageOpAmps, int startOffset, int maxCount, std::mutex &myMutex);
		void testComplOrSymOpAmps();
		void testComplOrSymOpAmpsLoop(const std::vector<Circuit*>& oneStageOpAmps, int startOffset, int maxCount);
		void testSimpleOrFullyDifferentialTwoStageOpAmp( const Circuit & simpleOneStageOpAmp, std::mutex &myMutex);

		Circuit & testCircuit(Circuit & circuit);
		Circuit & runStrictSearch(Circuit & circuit);
		Circuit & runOptimizingSearch(Circuit & circuit);

		AutomaticSizing::Graph &computeGraph(Circuit &circuit);

		void writeHSpiceFile(const Circuit & circuit, std::string oneStageOpAmpId = "empty") const;
	private:
		const FunctionalBlockLibrary * library_;
		const AutomaticSizing::CircuitInformation * circuitInformation_;
		std::string transistorModel_;
		std::string definition_;
		std::string HSPICENetlistDirectoryPath_;
	};
}



#endif /* SYNTHESIS_INCL_SEARCHSPACEEXPLORATION_OPAMPSIZING_H_ */
