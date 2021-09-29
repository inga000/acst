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

#ifndef SYNTHESIS_INCL_LIBRARY_FUNCTIONALBLOCKLIBRARY_H_
#define SYNTHESIS_INCL_LIBRARY_FUNCTIONALBLOCKLIBRARY_H_

#include "Core/incl/Common/Object.h"
#include <mutex>
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

namespace Core
{
	class Circuit;
}

namespace StructRec
{
	class Library;
}

namespace Synthesis {

	class DeviceLevel;
	class StructuralLevel;
	class AmplificationStagesSubBlockLevel;
	class AmplificationStageLevel;
	class Circuit;
	class OpAmps;

	class FunctionalBlockLibrary : public Core::Object
	{
	public:
		FunctionalBlockLibrary(const AutomaticSizing::CircuitInformation & circuitInformation);
		~FunctionalBlockLibrary();

		void setStructRecLibrary(StructRec::Library& library);

		std::string toStr() const;

		std::vector<Circuit*> createSimpleOneStageOpAmps(int caseNumber, int & amountOfOpAmps) const;
		std::vector<Circuit*> createFullyDifferentialOneStageOpAmps(int caseNumber, int & amountOfOpAmps) const;
		std::vector<Circuit*> createComplementaryOpAmps(int caseNumber, int & amountOfOpAmps) const;
		std::vector<Circuit*> createSymmetricalOpAmps(int caseNumber, int & amountOfOpAmps) const;
		std::vector<Circuit*> createSimpleTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const;
		std::vector<Circuit*> createSimpleTwoStageOpAmps(const Circuit & oneStageOpAmp, std::mutex &myMutex) const;
		std::vector<Circuit*> createFullyDifferentialTwoStageOpAmps(std::vector<Circuit*> oneStageOpAmps) const;
		std::vector<Circuit*> createFullyDifferentialTwoStageOpAmps(const Circuit &  oneStageOpAmp, std::mutex &myMutex) const;

		OpAmps & getOpAmps() const;

	private:
		const DeviceLevel & getDeviceLevel() const;
		const StructuralLevel & getStructuralLevel() const;
		const AmplificationStagesSubBlockLevel & getAmplificationStagesSubBlockLevel() const;
		const AmplificationStageLevel & getAmplificationStageLevel() const;


		std::string printOpAmps() const;

		StructRec::Library & getStructRecLibrary() const;

		std::vector<const Core::Circuit *> extractHierachicalOneStageOpAmpCircuits(std::vector<Circuit*> oneStageOpAmps) const;


		const DeviceLevel * deviceLevel_;
		const StructuralLevel * structuralLevel_;
		const AmplificationStagesSubBlockLevel * amplificationStagesSubBlockLevel_;
		const AmplificationStageLevel * amplificationStageLevel_;
		OpAmps * opAmps_;

		StructRec::Library * structRecLibrary_;
	};
}


#endif /* SYNTHESIS_INCL_LIBRARY_FUNCTIONALBLOCKLIBRARY_H_ */
