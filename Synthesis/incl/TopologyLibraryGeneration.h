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
 *************************************************************************************************/#ifndef STRUCTREC_INCL_STRUCTURERECOGNITIONANALYSIS_H_
#define SYNTHESIS_INCL_TOPOLOGYGENERATION_H_

#include "Synthesis/incl/LocalOptionsTopologyLibraryGeneration.h"
#include "Control/incl/Analysis/Analysis.h"

namespace StructRec {

	class Library;
	class StructureCircuits;
	class Result;
}

 namespace Partitioning {

 	class Result;

 }

 namespace AutomaticSizing {
 	 class CircuitParameter;
 }


namespace Synthesis {


	class FunctionalBlockLibrary;

    class TopologyLibraryGeneration : public Control::Analysis<LocalOptionsTopologyLibraryGeneration>
    {
    public:
    	TopologyLibraryGeneration();
        ~TopologyLibraryGeneration();

        void initialize();
        void compute();
        void write();

    private:
		void createAllSimpleOpAmps();
		void createAllFullyDifferentialOpAmps();
		void createAllComplementaryOpAmps();

		const Partitioning::Result * analyseCircuit(const Core::Circuit & circuit);
		StructRec::Library * parseStructRecLibrary() const;
		const StructRec::StructureCircuits * createStructRecResult(const Core::Circuit & circuit);
		const Partitioning::Result * createPartitioningResult(const StructRec::StructureCircuits & strucRecResult) const;


		void createOpAmps(const FunctionalBlockLibrary & functionalBlockLibrary, const AutomaticSizing::CircuitParameter & circuitParameter);
		const Core::Circuit & createFlatCircuit(const Core::Circuit & circuit) const;
		void writeHSpiceFile(const Core::Circuit &  circuit, const AutomaticSizing::CircuitParameter & circuitParameter, std::string opAmpId = "empty");

		void setStructRecResult(StructRec::Result & strucRecResult);
		void deleteStructRecResult();

		std::string complementaryOpAmpFilePath_;
		std::string fullyDifferentialOpAmpFilePath_;
		std::string singleOutputOpAmpFilePath_;

		StructRec::Library * structureLibrary_;
		StructRec::Result * structRecResult_;

    };

}


#endif
