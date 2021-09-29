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

#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_VARIABLES_VARIABLES_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_VARIABLES_VARIABLES_H_

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"

#include "Core/incl/Common/Object.h"

#include <map>

namespace AutomaticSizing
{
	class Transistor;
	class Other;
	class Variable;

	class Variables : public Core::Object
	{
		public:
			Variables();
			~Variables();

			void add(Transistor &  tran);
			void add(Other & other);

			bool isInitialized() const;

			void initializeRndSeedMap(int variableSize);
			void changeRndSeed(int index, unsigned int newRndSeed);
			unsigned int getRndSeed(int index);

			Transistor & findTransistor(StructRec::StructureId id);
			const Transistor & findTransistor(StructRec::StructureId id) const;

			Transistor & findTransistor(std::string name);
			const Transistor & findTransistor(std::string name) const;

			Other & findOther(StructRec::StructureId id);
			const Other & findOther(StructRec::StructureId id) const;

			Other & findOther(std::string name);
			const Other & findOther(std::string name) const;

			std::vector<Transistor*> getTransistors() const;
			std::vector<Other*> getOthers() const;

			std::string toStr() const;

		private:
			void addVariable(Variable & var);

			void eraseTransistors();
			void eraseOthers();
			bool hasRndSeed(int index) const;

		private:
			std::map<std::string, Variable*> variableMap_;
			std::map<std::string, Transistor*> transistorMap_;
			std::map<std::string, Other*> otherMap_;
			std::map<int, unsigned int> indexToRndSeedMap_;

	};

}


#endif /* AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_VARIABLES_VARIABLES_H_ */
