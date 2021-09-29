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


#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancher.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherIntervalSplittingNGL.h"

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include "Partitioning/incl/Results/Result.h"

#include <sstream>


namespace Gecode{

	IntegerVariableBrancher::IntegerVariableBrancher(Home home, ViewArray<Int::IntView>& variables, AutomaticSizing::Variables & variablesMap) :
			Brancher(home),
			variables_(variables),
			start_(0),
			variablesMap_(&variablesMap)
	{
	}
	
	IntegerVariableBrancher::IntegerVariableBrancher(Home home) :
	Brancher(home)
	{
	}


	IntegerVariableBrancher::IntegerVariableBrancher(Space & home, IntegerVariableBrancher& transistorBrancher) :
			Brancher(home, transistorBrancher)
	{
		variablesMap_ = transistorBrancher.variablesMap_;
		start_ = transistorBrancher.start_;
		variables_.update(home, transistorBrancher.variables_);
	}

	Brancher * IntegerVariableBrancher::copy(Space & home)
	{

		return new (home) IntegerVariableBrancher(home, *this);
	}

	size_t IntegerVariableBrancher::dispose(Space& home)
	{
		return sizeof(*this);
	}

		
	void IntegerVariableBrancher::post(Home home, ViewArray<Int::IntView>& variables,  AutomaticSizing::Variables& variablesMap)
	{

		(void) new (home) IntegerVariableBrancher(home, variables, variablesMap);
	}

	bool IntegerVariableBrancher::status(const Space & home) const
	{
		for(int i = start_; i< variables_.size(); i++)
		{
			if(!variables_[i].assigned())
			{
				start_ = i;
				return true;
			}
		}
		return false;
	}

	NGL* IntegerVariableBrancher::ngl(Space& home, const Choice& c, unsigned int a) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a ==0)
			return new (home) TransistorBrancherIntervalSplittingNGL(home, variables_[position], value);
		else
            return NULL;
	}


	IntegerVariableBrancher::PosVal::PosVal(const IntegerVariableBrancher & brancher, int position, int value) :
		Choice(brancher, 2),
		position_(position),
		value_(value)
	{
	}
			
		
	void IntegerVariableBrancher::PosVal::archive(Archive & e) const
	{
		 Choice::archive(e);
		e << position_ << value_;
	}

	
	Choice* IntegerVariableBrancher::choice(Space & home)
	{
		std::map<float, int> variableMap = sortVariablesAccordingToSmallestDomain(); //The map might be incomplete due to equal domain sizes, this however should not matter

		for(auto & it : variableMap)
		{
			float domain = it.first;
			if(domain != 0)
			{
				int position = it.second;
				int flag = 0;
				while(flag ==0)
				{
					int posValue = 0;
					if(isLengthPosition(position) || isWidthPosition(position))
					{
						unsigned int i;

						i = variablesMap_->getRndSeed(position);
						variablesMap_->changeRndSeed(position, i+1);

						Rnd rnd(i);
						int size = variables_[position].size();
						int rNum = rnd.operator()(size);
						posValue = variables_[position].min() + rNum;
					}
					else
					{
						posValue = variables_[position].med();
					}
					if(variables_[position].in(posValue))
					{
						flag = 1;
						return new PosVal(*this, position, posValue);
					}

				}
			}	
		}
		GECODE_NEVER;
		return NULL;
		
	}
	
	std::map<float, int> IntegerVariableBrancher::sortVariablesAccordingToSmallestDomain() const
	{
		std::map<float, int> variableMap;
		
		for(int it =0 ; it != variables_.size(); it++)
		{
			float domain = computeDomain(it);
			float degree = computeDegree(it);
			float afc = computeAFC(it);
			if(afc != 0)
				domain = domain/afc;

			variableMap.insert(std::pair<float, int>(domain, it));
		}
		
		return variableMap;
	}
	

	
	bool IntegerVariableBrancher::isLengthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getLengthIndex();
	}

	bool IntegerVariableBrancher::isWidthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getWidthIndex();
	}

	float IntegerVariableBrancher::computeDomain(int index) const
	{
		if(variables_[index].assigned())
			return 0;
		else
			return variables_[index].size();
	}

	float IntegerVariableBrancher::computeDegree(int index) const
	{
		return variables_[index].degree();
	}
	
	float IntegerVariableBrancher::computeAFC(int index) const
	{
		return variables_[index].afc();
	}

	Choice* IntegerVariableBrancher::choice(const Space & home, Archive & e)
	{
		int position, value;
		e >> position >> value;
		return new PosVal(*this, position, value);
	}

	ExecStatus IntegerVariableBrancher::commit(Space & home, const Choice & c, unsigned int a)
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		

		if(isLengthPosition(position) || isWidthPosition(position))
		{
			if(a == 0)
				return me_failed(variables_[position].eq(home, value)) ? ES_FAILED : ES_OK;
			else
				return me_failed(variables_[position].nq(home, value)) ? ES_FAILED : ES_OK;
		}
		else
		{
			if(a == 0)
				return me_failed(variables_[position].lq(home, value)) ? ES_FAILED : ES_OK;
			else
				return me_failed(variables_[position].gr(home, value)) ? ES_FAILED : ES_OK;
		}
	}
		
	void IntegerVariableBrancher::print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a == 0)
			o << "transistorVariables_[" << position << "] = " << value;
		else
			o << "transistorVariables_[" << position << "] != " << value;
	}

	std::string IntegerVariableBrancher::print()
	{
		std::ostringstream oss;

		oss << "Variables: " << variables_ << std::endl;
		oss << "Start value: " << start_ << std::endl;

		return oss.str();
	}

	bool IntegerVariableBrancher::isLengthPosition(int position) const
	{
		bool isIt = false;

		for(auto & tran : variablesMap_->getTransistors())
		{
			if(isLengthPosition(position, *tran))
			{
				isIt = true;
				break;
			}
		}
		return isIt;
	}

	bool IntegerVariableBrancher::isWidthPosition(int position) const
	{
		bool isIt = false;

		for(auto & tran : variablesMap_->getTransistors())
		{
			if(isWidthPosition(position, *tran))
			{
				isIt = true;
				break;
			}
		}
		return isIt;
	}



void branchIntegerVariables(Home home, const IntVarArgs & variables, AutomaticSizing::Variables& variablesMap)
{
	if(home.failed()) return;
	ViewArray<Int::IntView> variablesViews(home, variables);

	IntegerVariableBrancher::post(home, variablesViews, variablesMap);

}


}
