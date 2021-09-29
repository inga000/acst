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


#include "GecodeExtensions/incl/Brancher/TransistorBrancherIntervalSplitting.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherIntervalSplittingNGL.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherNGL.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>


namespace Gecode{

	TransistorBrancherIntervalSplitting::TransistorBrancherIntervalSplitting(Home home,
			ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result) :
	TransistorBrancher(home)
	{
		transistorVariables_ = transistorVariables;
		transistors_ = transistors;
		start_ = 0;
		allTransistorsAssigned_ =false;
		partitioningResult_ = &result;
	}
	
	TransistorBrancherIntervalSplitting::TransistorBrancherIntervalSplitting(Space & home, TransistorBrancherIntervalSplitting& transistorBrancher) :
			TransistorBrancher(home, transistorBrancher)
	{
		transistors_ = transistorBrancher.transistors_;
		start_ = transistorBrancher.start_;
		allTransistorsAssigned_ = transistorBrancher.allTransistorsAssigned_;
		partitioningResult_ = transistorBrancher.partitioningResult_;
		transistorVariables_.update(home, transistorBrancher.transistorVariables_);
	}

	Brancher * TransistorBrancherIntervalSplitting::copy(Space & home)
	{

		return new (home) TransistorBrancherIntervalSplitting(home, *this);
	}

	size_t TransistorBrancherIntervalSplitting::dispose(Space& home)
	{
		return sizeof(*this);
	}

		
	void TransistorBrancherIntervalSplitting::post(Home home, ViewArray<Int::IntView>& transistorVariables,
			std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result)
	{

		(void) new (home) TransistorBrancherIntervalSplitting(home, transistorVariables, transistors, result);
	}

	bool TransistorBrancherIntervalSplitting::status(const Space & home) const
	{
		if(allTransistorsAssigned_)
			return false;

		for(int i = start_; i< transistorVariables_.size(); i++)
		{
			if(!transistorVariables_[i].assigned())
			{
				start_ = i;
				return true;
			}
		}
		return false;
	}

	NGL* TransistorBrancherIntervalSplitting::ngl(Space& home, const Choice& c, unsigned int a) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;

		if(isLengthPosition(position) || isWidthPosition(position))
		{
			if(a ==0)
				return new (home) TransistorBrancherNGL(home, transistorVariables_[position], value);
			else
				return NULL;
		}
		else
		{
			if(a ==0)
				return new (home) TransistorBrancherIntervalSplittingNGL(home, transistorVariables_[position], value);
			else
				return NULL;
		}
	}


	TransistorBrancherIntervalSplitting::PosVal::PosVal(const TransistorBrancherIntervalSplitting & brancher, int position, int value) :
		Choice(brancher, 2),
		position_(position),
		value_(value)
	{
	}
			
		
	void TransistorBrancherIntervalSplitting::PosVal::archive(Archive & e) const
	{
		 Choice::archive(e);
		e << position_ << value_;
	}

	
	Choice* TransistorBrancherIntervalSplitting::choice(Space & home)
	{
		std::map<float, AutomaticSizing::Transistor*> transistorMap = sortTransistorsAccordingToSmallestDomain(); //The map might be incomplete due to equal domain sizes, this however should not matter

		if(transistorMap.size() == 1 && transistorMap.begin()->first == 0)
		{
			allTransistorsAssigned_ = true;

			for(int pos = start_; pos< transistorVariables_.size(); pos++)
			{
				if(!transistorVariables_[pos].assigned())
				{
					int posValue = transistorVariables_[pos].med();
					return new PosVal(*this, pos, posValue);
				}
			}
		}

		for(auto & it : transistorMap)
		{
			float domain = it.first;
			if(domain != 0)
			{
				int position = computePositionOfTransistor(*it.second);

				int size = transistorVariables_[position].size();
				int flag = 0;
				while(flag == 0)
				{
					int posValue = 0;
					if(isLengthPosition(position,*it.second) || isWidthPosition(position, *it.second))
					{
						unsigned int i;
						if(isLengthPosition(position,*it.second))
						{
							i = it.second->getLengthRndSeed();
							it.second->changeLengthRndSeed(i+1);
						}
						else
						{
							i = it.second->getWidthRndSeed();
							it.second->changeWidthRndSeed(i+1);
						}
						Rnd rnd(i);
						int rNum = rnd.operator()(size);
						posValue = transistorVariables_[position].min() + rNum;
					}
					else
					{
						posValue = transistorVariables_[position].med();

					}

					if(transistorVariables_[position].in(posValue))
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
	

	Choice* TransistorBrancherIntervalSplitting::choice(const Space & home, Archive & e)
	{
		int position, value;
		e >> position >> value;
		return new PosVal(*this, position, value);
	}

	ExecStatus TransistorBrancherIntervalSplitting::commit(Space & home, const Choice & c, unsigned int a)
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		

		if(isLengthPosition(position) || isWidthPosition(position))
		{
			if(a == 0)
				return me_failed(transistorVariables_[position].eq(home, value)) ? ES_FAILED : ES_OK;
			else
				return me_failed(transistorVariables_[position].nq(home, value)) ? ES_FAILED : ES_OK;
		}
		else
		{
			if(a == 0)
				return me_failed(transistorVariables_[position].lq(home, value)) ? ES_FAILED : ES_OK;
			else
				return me_failed(transistorVariables_[position].gr(home, value)) ? ES_FAILED : ES_OK;
		}
	}
		
	void TransistorBrancherIntervalSplitting::print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a == 0)
			o << "transistorVariables_[" << position << "] = " << value;
		else
			o << "transistorVariables_[" << position << "] != " << value;
	}

	std::string TransistorBrancherIntervalSplitting::print()
	{
		std::ostringstream oss;

		oss << "Transistor Variables: " << transistorVariables_ << std::endl;
		oss << "Start value: " << start_ << std::endl;
		oss << "Transistors: " << std::endl;
		for(auto & tran : transistors_)
		{
			oss <<   tran->print() << std::endl;
		}

		return oss.str();
	}




void branchTransistorsIntervalSplitting(Home home,
		const IntVarArgs & transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result)
{
	if(home.failed()) return;
	ViewArray<Int::IntView> transistorVariablesViews(home, transistorVariables);

	TransistorBrancherIntervalSplitting::post(home, transistorVariablesViews, transistors, result);

}


}
