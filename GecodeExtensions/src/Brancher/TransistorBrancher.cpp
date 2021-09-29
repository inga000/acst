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


#include "GecodeExtensions/incl/Brancher/TransistorBrancher.h"
#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherNGL.h"

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include "Partitioning/incl/Results/Result.h"

#include <sstream>


namespace Gecode{

	TransistorBrancher::TransistorBrancher(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result) :
	Brancher(home),
	transistorVariables_(transistorVariables),
	transistors_(transistors),
	start_(0),
	allTransistorsAssigned_(false),
	partitioningResult_(&result)
	{

	}
	
	TransistorBrancher::TransistorBrancher(Home home) :
	Brancher(home)
	{
	}


	TransistorBrancher::TransistorBrancher(Space & home, TransistorBrancher& transistorBrancher) :
			Brancher(home, transistorBrancher)
	{
		transistors_ = transistorBrancher.transistors_;
		start_ = transistorBrancher.start_;
		allTransistorsAssigned_ = transistorBrancher.allTransistorsAssigned_;
		partitioningResult_ = transistorBrancher.partitioningResult_;
		transistorVariables_.update(home, transistorBrancher.transistorVariables_);
	}

	Brancher * TransistorBrancher::copy(Space & home)
	{

		return new (home) TransistorBrancher(home, *this);
	}

	size_t TransistorBrancher::dispose(Space& home)
	{
		return sizeof(*this);
	}

		
	void TransistorBrancher::post(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result)
	{

		(void) new (home) TransistorBrancher(home, transistorVariables, transistors, result);
	}

	bool TransistorBrancher::status(const Space & home) const
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

	NGL* TransistorBrancher::ngl(Space& home, const Choice& c, unsigned int a) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a ==0)
			return new (home) TransistorBrancherNGL(home, transistorVariables_[position], value);
		else
            return NULL;
	}


	TransistorBrancher::PosVal::PosVal(const TransistorBrancher & brancher, int position, int value) :
		Choice(brancher, 2),
		position_(position),
		value_(value)
	{
	}
			
		
	void TransistorBrancher::PosVal::archive(Archive & e) const
	{
		 Choice::archive(e);
		e << position_ << value_;
	}

	
	Choice* TransistorBrancher::choice(Space & home)
	{
		std::map<float, AutomaticSizing::Transistor*> transistorMap = sortTransistorsAccordingToSmallestDomain(); //The map might be incomplete due to equal domain sizes, this however should not matter

		for(auto & it : transistorMap)
		{
			float domain = it.first;
			if(domain != 0)
			{
				int position = computePositionOfTransistor(*it.second);
				if(!isDrainVoltagePosition(position, *it.second))
				{
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
		}

		GECODE_NEVER;
		return NULL;
		
	}
	
	std::map<float, AutomaticSizing::Transistor*> TransistorBrancher::sortTransistorsAccordingToSmallestDomain() const
	{
		std::map<float, AutomaticSizing::Transistor*> transistorMap;
		
		for(auto & transistor : transistors_)
		{
			float domain = computeDomain(*transistor);
			float degree = computeDegree(*transistor);
			float afc = computeAFC(*transistor);
			
			if(afc != 0)
				domain = domain/afc;

			transistorMap.insert(std::pair<float, AutomaticSizing::Transistor*>(domain, transistor));
		}
		
		return transistorMap;
	}
	
	int TransistorBrancher::computePositionOfTransistor(AutomaticSizing::Transistor & transistor) const
	{
		if(!transistorVariables_[transistor.getLengthIndex()].assigned())
			return transistor.getLengthIndex();
		else if(!transistorVariables_[transistor.getWidthIndex()].assigned())
			return transistor.getWidthIndex();
		else if(!transistorVariables_[transistor.getCurrentIndex()].assigned())
			return transistor.getCurrentIndex();
		else if(!transistorVariables_[transistor.getSourceVoltageIndex()].assigned())
			return transistor.getSourceVoltageIndex();
		else if(!transistorVariables_[transistor.getGateVoltageIndex()].assigned())
			return transistor.getGateVoltageIndex();
		else if(!transistorVariables_[transistor.getDrainVoltageIndex()].assigned())
			return transistor.getDrainVoltageIndex();
		else
		{
			assert(false, "There should be an unassigned variable!");
			return -1;
		}
	}
	
	bool TransistorBrancher::isLengthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getLengthIndex();
	}

	bool TransistorBrancher::isWidthPosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getWidthIndex();
	}

	bool TransistorBrancher::isDrainVoltagePosition(int position, AutomaticSizing::Transistor & transistor) const
	{
		return position == transistor.getDrainVoltageIndex();
	}

	float TransistorBrancher::computeDomain(int index) const
	{
		if(transistorVariables_[index].assigned())
			return 0;
		else
			return transistorVariables_[index].size();
	}

	float TransistorBrancher::computeDegree(int index) const
	{
		return transistorVariables_[index].degree();
	}
	
	float TransistorBrancher::computeAFC(int index) const
	{
		return transistorVariables_[index].afc();
	}

	float TransistorBrancher::computeDomain(AutomaticSizing::Transistor & tran) const
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

		float domain = widthDomain + lengthDomain + currentDomain + gateVoltageDomain + drainVoltageDomain + sourceVoltageDomain;

		return domain;
	}

	float TransistorBrancher::computeDegree(AutomaticSizing::Transistor & tran) const
	{
		float widthDegree = computeDegree(tran.getWidthIndex());
		float lengthDegree = computeDegree(tran.getLengthIndex());
		float currentDegree = computeDegree(tran.getCurrentIndex());
		float gateVoltageDegree = computeDegree(tran.getGateVoltageIndex());
		float drainVoltageDegree = computeDegree(tran.getDrainVoltageIndex());
		float sourceVoltageDegree = computeDegree(tran.getSourceVoltageIndex());

		return widthDegree + lengthDegree + currentDegree + gateVoltageDegree + sourceVoltageDegree + drainVoltageDegree;
	}

	float TransistorBrancher::computeAFC(AutomaticSizing::Transistor & tran) const
	{
		float widthAFC = computeAFC(tran.getWidthIndex());
		float lengthAFC = computeAFC(tran.getLengthIndex());
		float currentAFC = computeAFC(tran.getCurrentIndex());
		float gateVoltageAFC = computeAFC(tran.getGateVoltageIndex());
		float drainVoltageAFC = computeAFC(tran.getDrainVoltageIndex());
		float sourceVoltageAFC = computeAFC(tran.getSourceVoltageIndex());

		return widthAFC + lengthAFC + currentAFC + gateVoltageAFC + sourceVoltageAFC + drainVoltageAFC;
	}

	Choice* TransistorBrancher::choice(const Space & home, Archive & e)
	{
		int position, value;
		e >> position >> value;
		return new PosVal(*this, position, value);
	}

	ExecStatus TransistorBrancher::commit(Space & home, const Choice & c, unsigned int a)
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		
		if(a == 0)
			return me_failed(transistorVariables_[position].eq(home, value)) ? ES_FAILED : ES_OK;
		else
			return me_failed(transistorVariables_[position].nq(home, value)) ? ES_FAILED : ES_OK;
	}
		
	void TransistorBrancher::print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const
	{
		const PosVal& pv = static_cast<const PosVal&>(c);
		int position = pv.position_;
		int value = pv.value_;
		if(a == 0)
			o << "transistorVariables_[" << position << "] = " << value;
		else
			o << "transistorVariables_[" << position << "] != " << value;
	}

	std::string TransistorBrancher::print()
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

	int TransistorBrancher::findLargestWidthDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getWidthIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int TransistorBrancher::findLargestLengthDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getLengthIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int TransistorBrancher::findLargestCurrentDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getCurrentIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int TransistorBrancher::findLargestGateVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getGateVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int TransistorBrancher::findLargestDrainVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getDrainVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	int TransistorBrancher::findLargestSourceVoltageDomain() const
	{
		int maxDomain = 1;
		for(auto& tran: transistors_)
		{
			int domain = transistorVariables_[tran->getSourceVoltageIndex()].size();
			if(domain > maxDomain)
			{
				maxDomain = domain;
			}
		}
		return maxDomain;
	}

	bool TransistorBrancher::isLengthPosition(int position) const
	{
		bool isIt = false;

		for(auto & tran : transistors_)
		{
			if(isLengthPosition(position, *tran))
			{
				isIt = true;
				break;
			}
		}
		return isIt;
	}

	bool TransistorBrancher::isWidthPosition(int position) const
	{
		bool isIt = false;

		for(auto & tran : transistors_)
		{
			if(isWidthPosition(position, *tran))
			{
				isIt = true;
				break;
			}
		}
		return isIt;
	}

	bool TransistorBrancher::isDrainVoltagePosition(int position) const
	{
		bool isIt = false;

		for(auto & tran : transistors_)
		{
			if(isDrainVoltagePosition(position, *tran))
			{
				isIt = true;
				break;
			}
		}
		return isIt;
	}

	AutomaticSizing::Transistor & TransistorBrancher::getTransistor(int position)
	{
		AutomaticSizing::Transistor * transistor = nullptr;

		for(auto & tran : transistors_)
		{
			if(tran->hasVariableIndex(position))
			{
				transistor = tran;
				break;
			}
		}

		assert(transistor != nullptr, "Every variable should be part of a transistor!");

		return *transistor;

	}



	void branchTransistors(Home home, const IntVarArgs & transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result result)
	{
		if(home.failed()) return;
		ViewArray<Int::IntView> transistorVariablesViews(home, transistorVariables);

		TransistorBrancher::post(home, transistorVariablesViews, transistors, result);
	}


}
