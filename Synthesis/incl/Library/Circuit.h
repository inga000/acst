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

#ifndef SYNTHESIS_INCL_LIBRARY_CIRCUIT_H_
#define SYNTHESIS_INCL_LIBRARY_CIRCUIT_H_

#include "Core/incl/Common/Object.h"


namespace Core
{
	class Circuit;
}

namespace StructRec
{
	class StructureCircuits;
	class Library;
}

namespace Partitioning
{
	class Result;
}

namespace AutomaticSizing
{
	class CircuitInformation;
	class Result;
}

namespace Synthesis
{


	class Circuit : public Core::Object
	{
		public:
			Circuit();
			~Circuit();

			void initialize(const Core::Circuit & circuit, StructRec::Library & library);
			void setCircuitInformation(const AutomaticSizing::CircuitInformation &circuitInformation);
			void setSizingResult(const  AutomaticSizing::Result & sizingResult);

			std::string toStr() const;

			const Core::Circuit & getHierarchicalCircuit() const;
			const Core::Circuit & getFlatCircuit() const;
			const StructRec::StructureCircuits & getConstStructRecResult() const;
			const Partitioning::Result & getPartitioningResult() const;
			const AutomaticSizing::CircuitInformation & getCircuitInformation() const;
			const AutomaticSizing::Result & getSizingResult() const;


			bool fulfillsAllSpecifications() const;
			bool fulfillsFirstStageSpecifications() const;

		private:
			void setFlatCircuit(const Core::Circuit & circuit);
			void setHierarchicalCircuit(const Core::Circuit & circuit);
			const StructRec::StructureCircuits & getStructRecResult();

			const StructRec::StructureCircuits & createStructRecResult(StructRec::Library & structRecLibrary);
			const Partitioning::Result & createPartitioningResult();

			const Core::Circuit * hierarchicalCircuit_;
			const Core::Circuit * flatCircuit_;
			const StructRec::StructureCircuits* structRecResults_;
			const Partitioning::Result* partitioningResults_;
			const AutomaticSizing::Result* sizingResult_;
			const AutomaticSizing::CircuitInformation * circuitInformation_;


	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_CIRCUIT_H_ */
