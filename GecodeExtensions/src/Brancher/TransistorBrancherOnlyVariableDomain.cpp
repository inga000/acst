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


#include "GecodeExtensions/incl/Brancher/TransistorBrancherOnlyVariableDomain.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherNGL.h"

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>


namespace Gecode{

	TransistorBrancherOnlyVariableDomain::TransistorBrancherOnlyVariableDomain(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors) :
	TransistorBrancher(home)
	{
		transistorVariables_ = transistorVariables;
		transistors_ =transistors;
		start_ = 0;
		allTransistorsAssigned_ = false;
	}
	
	TransistorBrancherOnlyVariableDomain::TransistorBrancherOnlyVariableDomain(Space & home, TransistorBrancherOnlyVariableDomain& transistorBrancher) :
			TransistorBrancher(home, transistorBrancher)
	{
		transistors_ = transistorBrancher.transistors_;
		start_ = transistorBrancher.start_;
		allTransistorsAssigned_ = transistorBrancher.allTransistorsAssigned_;
		transistorVariables_.update(home, transistorBrancher.transistorVariables_);
	}

	Brancher * TransistorBrancherOnlyVariableDomain::copy(Space & home)
	{

		return new (home) TransistorBrancherOnlyVariableDomain(home, *this);
	}

	size_t TransistorBrancherOnlyVariableDomain::dispose(Space& home)
	{
		return sizeof(*this);
	}

		
	void TransistorBrancherOnlyVariableDomain::post(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors)
	{

		(void) new (home) TransistorBrancherOnlyVariableDomain(home, transistorVariables, transistors);
	}

	bool TransistorBrancherOnlyVariableDomain::status(const Space & home) const
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

	NGL* TransistorBrancherOnlyVariableDomain::ngl(Space& home, const Choice& c, unsigned int a) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a ==0)
			return new (home) TransistorBrancherNGL(home, transistorVariables_[position], value);
		else
            return NULL;
	}


	TransistorBrancherOnlyVariableDomain::PosVal::PosVal(const TransistorBrancherOnlyVariableDomain & brancher, int position, int value) :
		Choice(brancher, 2),
		position_(position),
		value_(value)
	{
	}
			
		
	void TransistorBrancherOnlyVariableDomain::PosVal::archive(Archive & e) const
	{
		 Choice::archive(e);
		e << position_ << value_;
	}

	
	Choice* TransistorBrancherOnlyVariableDomain::choice(Space & home)
	{
		std::map<float, int> variablesMap = sortVariablesAccordingToSmallestDomain(); //The map might be incomplete due to equal domain sizes, this however should not matter

		for(auto & it : variablesMap)
		{
			float domain = it.first;
			if(domain != 0)
			{
				int position = it.second;

				int size = transistorVariables_[position].size();

				int flag = 0;
				while(flag == 0)
				{
					int posValue = 0;
					if(isLengthPosition(position) || isWidthPosition(position))
					{
						unsigned int i;
						if(isLengthPosition(position))
						{
							i = getTransistor(position).getLengthRndSeed();
							getTransistor(position).changeLengthRndSeed(i+1);
						}
						else
						{
							i = getTransistor(position).getWidthRndSeed();
							getTransistor(position).changeWidthRndSeed(i+1);
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
	
	std::map<float, int> TransistorBrancherOnlyVariableDomain::sortVariablesAccordingToSmallestDomain() const
	{
		std::map<float, int> variableMap;
		
		float maxWidthDomain = findLargestWidthDomain();
		float maxLengthDomain = findLargestLengthDomain();
		float maxCurrentDomain = findLargestCurrentDomain();
		float maxGateVoltageDomain = findLargestGateVoltageDomain();
		float maxSourceVoltageDomain = findLargestSourceVoltageDomain();

		for(auto & transistor : transistors_)
		{

			float widthDomain = computeDomain(transistor->getWidthIndex());
			float widthDegree = computeDegree(transistor->getWidthIndex());
			float widthAFC = computeAFC(transistor->getWidthIndex());
			if (widthDegree != 0)
				variableMap.insert(std::pair<float, int>(widthDomain/widthAFC,transistor->getWidthIndex()));
			else
				variableMap.insert(std::pair<float, int>(widthDomain,transistor->getWidthIndex()));

			float lengthDomain = computeDomain(transistor->getLengthIndex());
			float lengthDegree = computeDegree(transistor->getLengthIndex());
			float lengthAFC = computeAFC(transistor->getLengthIndex());
			if (lengthDegree != 0)
				variableMap.insert(std::pair<float, int>(lengthDomain/lengthAFC,transistor->getLengthIndex()));
			else
				variableMap.insert(std::pair<float, int>(lengthDomain,transistor->getLengthIndex()));

			float currentDomain = computeDomain(transistor->getCurrentIndex());
			float currentDegree = computeDegree(transistor->getCurrentIndex());
			float currentAFC = computeAFC(transistor->getCurrentIndex());
			if (currentDegree != 0)
				variableMap.insert(std::pair<float, int>(currentDomain/currentAFC,transistor->getCurrentIndex()));
			else
				variableMap.insert(std::pair<float, int>(currentDomain,transistor->getCurrentIndex()));

			float gateVoltageDomain = computeDomain(transistor->getGateVoltageIndex());
			float gateVoltageDegree = computeDegree(transistor->getGateVoltageIndex());
			float gateVoltageAFC = computeAFC(transistor->getGateVoltageIndex());
			if (gateVoltageDegree != 0)
				variableMap.insert(std::pair<float, int>(gateVoltageDomain/gateVoltageAFC,transistor->getGateVoltageIndex()));
			else
				variableMap.insert(std::pair<float, int>(gateVoltageDomain,transistor->getGateVoltageIndex()));

			float sourceVoltageDomain = computeDomain(transistor->getSourceVoltageIndex());
			float sourceVoltageDegree = computeDegree(transistor->getSourceVoltageIndex());
			float sourceVoltageAFC = computeAFC(transistor->getSourceVoltageIndex());
			if (sourceVoltageDegree != 0)
				variableMap.insert(std::pair<float, int>(sourceVoltageDomain/sourceVoltageAFC,transistor->getSourceVoltageIndex()));
			else
				variableMap.insert(std::pair<float, int>(sourceVoltageDomain,transistor->getSourceVoltageIndex()));

		}
		
		return variableMap;
	}
	
	Choice* TransistorBrancherOnlyVariableDomain::choice(const Space & home, Archive & e)
	{
		int position, value;
		e >> position >> value;
		return new PosVal(*this, position, value);
	}

	ExecStatus TransistorBrancherOnlyVariableDomain::commit(Space & home, const Choice & c, unsigned int a)
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;

		if(a == 0)
			return me_failed(transistorVariables_[position].eq(home, value)) ? ES_FAILED : ES_OK;
		else
			return me_failed(transistorVariables_[position].nq(home, value)) ? ES_FAILED : ES_OK;

	}
		
	void TransistorBrancherOnlyVariableDomain::print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a == 0)
			o << "transistorVariables_[" << position << "] = " << value;
		else
			o << "transistorVariables_[" << position << "] != " << value;
	}

	std::string TransistorBrancherOnlyVariableDomain::print()
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


	void branchTransistorsOnlyVariableDomain(Home home, const IntVarArgs & transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors)
	{
		if(home.failed()) return;
		ViewArray<Int::IntView> transistorVariablesViews(home, transistorVariables);

		TransistorBrancherOnlyVariableDomain::post(home, transistorVariablesViews, transistors);
	}


}
