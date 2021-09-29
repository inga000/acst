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


#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variable.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Other.h"

namespace AutomaticSizing
{

		Variables::Variables()
		{

		}

		Variables::~Variables()
		{
			eraseTransistors();
			eraseOthers();
		}

		void Variables::add(Transistor &  tran)
		{
			transistorMap_.insert(std::pair<std::string, Transistor*>(tran.getName(), &tran));
			addVariable(tran);
		}

		void Variables::add(Other & other)
		{
			otherMap_.insert(std::pair<std::string, Other*>(other.getName(), &other));
			addVariable(other);
		}


		void Variables::addVariable(Variable & var)
		{
			variableMap_.insert(std::pair<std::string, Variable*>(var.getName(), &var));
		}

		bool Variables::isInitialized() const
		{
			return !variableMap_.empty();
		}

		Transistor & Variables::findTransistor(StructRec::StructureId id)
		{
			assert(transistorMap_.find(id.toStr()) != transistorMap_.end());
			return * transistorMap_.at(id.toStr());
		}
		const Transistor & Variables::findTransistor(StructRec::StructureId id) const
		{
			assert(transistorMap_.find(id.toStr()) != transistorMap_.end());
			return *transistorMap_.at(id.toStr());
		}

		Transistor & Variables::findTransistor(std::string name)
		{
			assert(transistorMap_.find(name) != transistorMap_.end());
			return *transistorMap_.at(name);
		}

		const Transistor & Variables::findTransistor(std::string name) const
		{
			assert(transistorMap_.find(name) != transistorMap_.end());
			return *transistorMap_.at(name);
		}

		Other & Variables::findOther(StructRec::StructureId id)
		{
			assert(otherMap_.find(id.toStr()) != otherMap_.end());
			return *otherMap_.at(id.toStr());
		}

		const Other & Variables::findOther(StructRec::StructureId id) const
		{
			assert(otherMap_.find(id.toStr()) != otherMap_.end());
			return *otherMap_.at(id.toStr());
		}

		Other & Variables::findOther(std::string name)
		{
			assert(otherMap_.find(name) != otherMap_.end());
			return *otherMap_.at(name);
		}

		const Other & Variables::findOther(std::string name) const
		{
			assert(otherMap_.find(name) != otherMap_.end());
			return *otherMap_.at(name);
		}

		std::vector<Transistor*> Variables::getTransistors() const
		{
			std::vector<Transistor*> transistors;
			for(auto & it: transistorMap_)
			{
				transistors.push_back(it.second);
			}

			return transistors;
		}

		std::vector<Other*> Variables::getOthers() const
		{
			std::vector<Other*> others;
			for(auto & it: otherMap_)
			{
				others.push_back(it.second);
			}

			return others;
		}

		void Variables::eraseTransistors()
		{
			for(auto & it : transistorMap_)
			{
				delete it.second;
			}
		}

		void Variables::eraseOthers()
		{
			for(auto & it : otherMap_)
			{
				delete it.second;
			}
		}

		std::string Variables::toStr() const
		{
			std::ostringstream oss;
			for(auto & it : variableMap_)
				oss << it.second->print();

			return oss.str();
		}

		void Variables::initializeRndSeedMap(int variableSize)
		{
			unsigned int rndSeed = 1;
			for(int i = 0; i < variableSize; i++)
			{
				indexToRndSeedMap_.insert(std::pair<int, unsigned int>(i, rndSeed));
			}
		}
		void Variables::changeRndSeed(int index, unsigned int newRndSeed)
		{
			if(hasRndSeed(index))
			{
				indexToRndSeedMap_.at(index) = newRndSeed;
			}
			else
			{
				indexToRndSeedMap_.insert(std::pair<int, unsigned int>(index, newRndSeed));
			}
		}

		unsigned int Variables::getRndSeed(int index)
		{
			if(hasRndSeed(index))
				return indexToRndSeedMap_.at(index);
			else
			{
				indexToRndSeedMap_.insert(std::pair<int, unsigned int>(index, 1));
				return 1;
			}
		}

		bool Variables::hasRndSeed(int index) const
		{
			return indexToRndSeedMap_.find(index) != indexToRndSeedMap_.end();
		}

}


