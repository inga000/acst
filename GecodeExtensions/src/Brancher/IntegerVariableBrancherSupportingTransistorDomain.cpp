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

#include "GecodeExtensions/incl/Brancher/IntegerVariableBrancherSupportingTransistorDomain.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherIntervalSplittingNGL.h"

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Other.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variable.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include "Partitioning/incl/Results/Result.h"

#include <sstream>


namespace Gecode{

	IntegerVariableBrancherSupportingTransistorDomain::IntegerVariableBrancherSupportingTransistorDomain(Home home, ViewArray<Int::IntView>& variables,
			AutomaticSizing::Variables & variablesMap) :
			Brancher(home),
			variables_(variables),
			start_(0),
			variablesMap_(&variablesMap)
	{
	}
	
	IntegerVariableBrancherSupportingTransistorDomain::IntegerVariableBrancherSupportingTransistorDomain(Home home) :
	Brancher(home)
	{
	}


	IntegerVariableBrancherSupportingTransistorDomain::IntegerVariableBrancherSupportingTransistorDomain(Space & home, IntegerVariableBrancherSupportingTransistorDomain& transistorBrancher) :
			Brancher(home, transistorBrancher)
	{
		variablesMap_ = transistorBrancher.variablesMap_;
		start_ = transistorBrancher.start_;
		variables_.update(home, transistorBrancher.variables_);
	}

	Brancher * IntegerVariableBrancherSupportingTransistorDomain::copy(Space & home)
	{

		return new (home) IntegerVariableBrancherSupportingTransistorDomain(home, *this);
	}

	size_t IntegerVariableBrancherSupportingTransistorDomain::dispose(Space& home)
	{
		return sizeof(*this);
	}

		
	void IntegerVariableBrancherSupportingTransistorDomain::post(Home home, ViewArray<Int::IntView>& variables,  AutomaticSizing::Variables & variablesMap)
	{

		(void) new (home) IntegerVariableBrancherSupportingTransistorDomain(home, variables, variablesMap);
	}

	bool IntegerVariableBrancherSupportingTransistorDomain::status(const Space & home) const
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

	NGL* IntegerVariableBrancherSupportingTransistorDomain::ngl(Space& home, const Choice& c, unsigned int a) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a ==0)
			return new (home) TransistorBrancherIntervalSplittingNGL(home, variables_[position], value);
		else
            return NULL;
	}


	IntegerVariableBrancherSupportingTransistorDomain::PosVal::PosVal(const IntegerVariableBrancherSupportingTransistorDomain & brancher, int position, int value) :
		Choice(brancher, 2),
		position_(position),
		value_(value)
	{
	}
			
		
	void IntegerVariableBrancherSupportingTransistorDomain::PosVal::archive(Archive & e) const
	{
		 Choice::archive(e);
		e << position_ << value_;
	}

	
	Choice* IntegerVariableBrancherSupportingTransistorDomain::choice(Space & home)
	{

		std::map<float, AutomaticSizing::Variable *> variableMap = sortVariablesAccordingToSmallestDomain(); //The map might be incomplete due to equal domain sizes, this however should not matter

		for(auto & it : variableMap)
		{
			if(it.second->isTransistor())
			{
				AutomaticSizing::Transistor & tran = variablesMap_->findTransistor(it.second->getName());
			}
		}

		for(auto & it : variableMap)
		{
			float domain = it.first;
			if(domain != 0)
			{
				AutomaticSizing::Variable & var  = *it.second;

				int position;

				if(var.isTransistor())
					position = computePositionOfTransistor(variablesMap_->findTransistor(var.getName()));
				else
					position = variablesMap_->findOther(var.getName()).getIndex();

				int flag = 0;
				while(flag ==0)
				{
					int posValue = 0;
					if(isLengthPosition(position) || isWidthPosition(position))
					{
						unsigned int i;
						if(isLengthPosition(position))
						{
							i = variablesMap_->getRndSeed(position);
							variablesMap_->changeRndSeed(position, i+1);
						}
						else
						{
							i = variablesMap_->getRndSeed(position);
							variablesMap_->changeRndSeed(position,i+1);
						}
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
	
	std::map<float, AutomaticSizing::Variable *> IntegerVariableBrancherSupportingTransistorDomain::sortVariablesAccordingToSmallestDomain() const
	{
		std::map<float, AutomaticSizing::Variable*> variableMap;
		
		for(auto & it : variablesMap_->getTransistors())
		{
			float domain = computeDomain(*it);
			float degree = computeDegree(*it);
			float afc = computeAFC(*it);
			
			if(afc != 0)
				domain = domain/afc;

			variableMap.insert(std::pair<float, AutomaticSizing::Variable*>(domain, it));

		}
		for(auto & other : variablesMap_->getOthers())
		{
			float domain = computeDomain(*other);
			float degree = computeDegree(*other);
			float afc = computeAFC(*other);

			if( afc!= 0)
				domain = domain/afc;

			variableMap.insert(std::pair<float, AutomaticSizing::Variable*>(domain, other));
		}
		


		return variableMap;
	}
	

	
	bool IntegerVariableBrancherSupportingTransistorDomain::isLengthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getLengthIndex();
	}

	bool IntegerVariableBrancherSupportingTransistorDomain::isWidthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getWidthIndex();
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDomain(int index) const
	{
		if(variables_[index].assigned())
			return 0;
		else
			return variables_[index].size();
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDegree(int index) const
	{
		return variables_[index].degree();
	}
	
	float IntegerVariableBrancherSupportingTransistorDomain::computeAFC(int index) const
	{
		return variables_[index].afc();
	}

	Choice* IntegerVariableBrancherSupportingTransistorDomain::choice(const Space & home, Archive & e)
	{
		int position, value;
		e >> position >> value;
		return new PosVal(*this, position, value);
	}

	ExecStatus IntegerVariableBrancherSupportingTransistorDomain::commit(Space & home, const Choice & c, unsigned int a)
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
		
	void IntegerVariableBrancherSupportingTransistorDomain::print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a == 0)
			o << "transistorVariables_[" << position << "] = " << value;
		else
			o << "transistorVariables_[" << position << "] != " << value;
	}

	std::string IntegerVariableBrancherSupportingTransistorDomain::print()
	{
		std::ostringstream oss;

		oss << "Variables: " << variables_ << std::endl;
		oss << "Start value: " << start_ << std::endl;
		oss << "Variables: " << std::endl;
		oss <<  variablesMap_->toStr() << std::endl;


		return oss.str();
	}

	bool IntegerVariableBrancherSupportingTransistorDomain::isLengthPosition(int position) const
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

	bool IntegerVariableBrancherSupportingTransistorDomain::isWidthPosition(int position) const
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


	int IntegerVariableBrancherSupportingTransistorDomain::computePositionOfTransistor(AutomaticSizing::Transistor & transistor) const
	{
		if(!variables_[transistor.getLengthIndex()].assigned())
			return transistor.getLengthIndex();
		else if(!variables_[transistor.getWidthIndex()].assigned())
			return transistor.getWidthIndex();
		else if(!variables_[transistor.getCurrentIndex()].assigned())
			return transistor.getCurrentIndex();
		else if(!variables_[transistor.getSourceVoltageIndex()].assigned())
			return transistor.getSourceVoltageIndex();
		else if(!variables_[transistor.getGateVoltageIndex()].assigned())
			return transistor.getGateVoltageIndex();
		else if(!variables_[transistor.getDrainVoltageIndex()].assigned())
			return transistor.getDrainVoltageIndex();
		else
		{
			assert(false, "There should be an unassigned variable!");
			return -1;
		}
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDomain(AutomaticSizing::Transistor & tran) const
	{
		float maxWidthDomain = findLargestWidthDomain();
		float maxLengthDomain = findLargestLengthDomain();
		float maxCurrentDomain = findLargestCurrentDomain();
		float maxGateVoltageDomain = findLargestGateVoltageDomain();
		float maxDrainVoltageDomain = findLargestDrainVoltageDomain();
		float maxSourceVoltageDomain = findLargestSourceVoltageDomain();


		float widthDomain = computeDomain(tran.getWidthIndex())/maxWidthDomain;
		float lengthDomain = computeDomain(tran.getLengthIndex())/maxLengthDomain;
		float currentDomain = computeDomain(tran.getCurrentIndex())/maxCurrentDomain;
		float gateVoltageDomain = computeDomain(tran.getGateVoltageIndex())/maxGateVoltageDomain;
		float drainVoltageDomain = computeDomain(tran.getDrainVoltageIndex())/maxDrainVoltageDomain;
		float sourceVoltageDomain = computeDomain(tran.getSourceVoltageIndex())/maxSourceVoltageDomain;

		float domain = (widthDomain + lengthDomain + currentDomain + gateVoltageDomain + drainVoltageDomain + sourceVoltageDomain)/10000;
		return domain;
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDegree(AutomaticSizing::Transistor & tran) const
	{
		float widthDegree = computeDegree(tran.getWidthIndex());
		float lengthDegree = computeDegree(tran.getLengthIndex());
		float currentDegree = computeDegree(tran.getCurrentIndex());
		float gateVoltageDegree = computeDegree(tran.getGateVoltageIndex());
		float drainVoltageDegree = computeDegree(tran.getDrainVoltageIndex());
		float sourceVoltageDegree = computeDegree(tran.getSourceVoltageIndex());

		return (widthDegree + lengthDegree + currentDegree + gateVoltageDegree + sourceVoltageDegree + drainVoltageDegree);
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeAFC(AutomaticSizing::Transistor & tran) const
	{
		float widthAFC = computeAFC(tran.getWidthIndex());
		float lengthAFC = computeAFC(tran.getLengthIndex());
		float currentAFC = computeAFC(tran.getCurrentIndex());
		float gateVoltageAFC = computeAFC(tran.getGateVoltageIndex());
		float drainVoltageAFC = computeAFC(tran.getDrainVoltageIndex());
		float sourceVoltageAFC = computeAFC(tran.getSourceVoltageIndex());

		return (widthAFC + lengthAFC + currentAFC + gateVoltageAFC + sourceVoltageAFC + drainVoltageAFC);
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDomain(AutomaticSizing::Other & other) const
	{

		float domain = computeDomain(other.getIndex());
		if(other.isPoleOrZero())
		{
			int maxDomain = 1;
			maxDomain = findLargestPolesAndZerosDomain();
		}
		else if(other.isSpecification())
		{
			int maxDomain = 1;
			maxDomain = findLargestSpecificationDomain();
		}
		else if(other.isTwoPort())
		{
			int maxDomain = 1;
			maxDomain = findLargestTwoPortDomain();
		}
		else if(other.isVoltage())
		{
			int maxDomain = 1;
			maxDomain = findLargestVoltageDomain();
		}
		return domain;
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeDegree(AutomaticSizing::Other & other) const
	{
		return computeDegree(other.getIndex());
	}

	float IntegerVariableBrancherSupportingTransistorDomain::computeAFC(AutomaticSizing::Other & other) const
	{
		return computeAFC(other.getIndex());
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestWidthDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getWidthIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestLengthDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getLengthIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestCurrentDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getCurrentIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestGateVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getGateVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestDrainVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getDrainVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int IntegerVariableBrancherSupportingTransistorDomain::findLargestSourceVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: variablesMap_->getTransistors())
		{
			int domain = variables_[tran->getSourceVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}


	float IntegerVariableBrancherSupportingTransistorDomain::findLargestPolesAndZerosDomain() const
	{
		float maxDomain = 1;
		for(auto& other: variablesMap_->getOthers())
		{
			if(other->isPoleOrZero())
			{
				float domain = variables_[other->getIndex()].size();
				if(domain > maxDomain)
				{
					maxDomain = domain;
				}
			}
		}
		return maxDomain;
	}

	float IntegerVariableBrancherSupportingTransistorDomain::findLargestTwoPortDomain() const
	{
		float maxDomain = 1;
		for(auto& other: variablesMap_->getOthers())
		{
			if(other->isTwoPort())
			{
				float domain = variables_[other->getIndex()].size();
				if(domain > maxDomain)
				{
					maxDomain = domain;
				}
			}
		}
		return maxDomain;
	}

	float IntegerVariableBrancherSupportingTransistorDomain::findLargestSpecificationDomain() const
	{
		float maxDomain = 1;
		for(auto& other: variablesMap_->getOthers())
		{
			if(other->isSpecification())
			{
				float domain = variables_[other->getIndex()].size();
				if(domain > maxDomain)
				{
					maxDomain = domain;
				}
			}
		}
		return maxDomain;
	}

	float IntegerVariableBrancherSupportingTransistorDomain::findLargestVoltageDomain() const
	{
		float maxDomain = 1;
		for(auto& other: variablesMap_->getOthers())
		{
			if(other->isVoltage())
			{
				float domain = variables_[other->getIndex()].size();
				if(domain > maxDomain)
				{
					maxDomain = domain;
				}
			}
		}
		return maxDomain;
	}


	void branchIntegerVariablesWithSupportOfTransistorDomain(Home home, const IntVarArgs & variables, AutomaticSizing::Variables &transistors)
	{
		if(home.failed()) return;
		ViewArray<Int::IntView> variablesViews(home, variables);

		IntegerVariableBrancherSupportingTransistorDomain::post(home, variablesViews, transistors);
	}


}
