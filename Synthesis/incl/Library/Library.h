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

#ifndef SYNTHESIS_INCL_LIBRARY_LIBRARY_H_
#define SYNTHESIS_INCL_LIBRARY_LIBRARY_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"

#include <vector>

namespace StructRec
{
	class Library;
}

namespace Core
{
	class Circuit;
}

namespace Synthesis
{

	class Circuit;

	class Library : public Core::Object
	{
		public:
			Library();
			~Library();

			void addFirstStage(Core::Circuit & firstStage);
			void addSecondStage(Core::Circuit & secondStage);
			void addSymmetricalFirstStage(Core::Circuit& firstStage);
			void addComplementarySecondStage(Core::Circuit& secondStage);

			void setStructRecLibrary(StructRec::Library& library);


			std::vector<Circuit*> createOneStageOpAmps() const;
			std::vector<Circuit*> createSymmetricalOpAmps() const;
			std::vector<Circuit*> createTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const;

			void addStageToNewCircuit(Core::Circuit & newCircuit, const Core::Circuit & stage) const;
			void addSecondStageToSymOpAmp(Core::Circuit & symOpAmp, Core::Circuit & secondStage) const;
			void addLoadCapacityToCircuit(Core::Circuit & circuit, const Core::NetId outputNetStage) const;

			std::string toStr() const;
		private:
			void printFirstStages(std::ostream & stream) const;
			void printSecondStages(std::ostream & stream) const;
			void printSymmetricalFirstStages(std::ostream & stream) const;
			void printComplementarySecondStages(std::ostream & stream) const;
		private:
			StructRec::Library & getStructRecLibrary() const;
			bool isValidSymOpAmp(Circuit &symOpAmp) const;

			void eraseFirstStages();
			void eraseSymmetricalFirstStages();
			void eraseSecondStages();
			void eraseComplementarySecondStages();

			std::vector<Core::Circuit*> firstStages_;
			std::vector<Core::Circuit*> secondStages_;
			std::vector<Core::Circuit*> symmetricalFirstStages_;
			std::vector<Core::Circuit*> complementarySecondStages_;

			StructRec::Library * structRecLibrary_;

	};

}
#endif /* SYNTHESIS_INCL_LIBRARY_LIBRARY_H_ */
