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


#include "../../incl/ConstraintProgram/SimpleSearchSpace.h"

#include "../../incl/ConstraintProgram/Constraints/CircuitSpecificationsConstraints.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Partitioning/incl/Results/Result.h"


namespace AutomaticSizing {

	const float SimpleSearchSpace::NOT_INITIALIZED_ = -1;


	SimpleSearchSpace::~SimpleSearchSpace()
	{

	}

	SimpleSearchSpace::SimpleSearchSpace()
	{
	}

	SimpleSearchSpace::SimpleSearchSpace(SimpleSearchSpace& other) : SearchSpace(other)
	{

	}

	 SimpleSearchSpace* SimpleSearchSpace::copy()
	{
		return new SimpleSearchSpace(*this);
	}


	void SimpleSearchSpace::constrain(const Gecode::Space& other)
	{
		const SimpleSearchSpace& otherSpace = static_cast<const SimpleSearchSpace&>(other);

		Gecode::rel(*this, cost_ >= otherSpace.cost_ * 1.01);
		if(otherSpace.fulfillsSpecifications())
		{
		Partitioning::Component * compN1 = nullptr;
		Partitioning::Component * compN2 = nullptr;
		Partitioning::Component * compP1 = nullptr;
		Partitioning::Component * compP2 = nullptr;


		std::vector<Partitioning::Component* > comps = getPartitioningResult().getBelongingComponents(getPartitioningResult().getFirstStage());
		for(auto & comp : comps)
		{
			if(compN1 == nullptr && comp->getArray().getTechType().isN())
			{
				compN1 = comp;
			}
			else if(compN2 == nullptr && comp->getArray().getTechType().isN())
			{
				compN2 = comp;
			}
			else if(compP1 == nullptr && comp->getArray().getTechType().isP())
			{
				compP1 = comp;
			}
			else if(compP2 == nullptr && comp->getArray().getTechType().isP())
			{
				compP2 = comp;
			}
			else
			{
				assert(false, " There should be two nmos components and/or two pmos components  in the first stage but nothing more!");
			}
		}

		assert((compN1 != nullptr && compN2 != nullptr)
				|| (compP1 != nullptr && compP2 != nullptr), " There should be two nmos components and/or two pmos components  in the first stage!");

		if((compN1 != nullptr && compN2 != nullptr))
		{
			Core::NetId outputNetFirstStage1 = compN1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();
			Core::NetId outputNetFirstStage2 = compN2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();


		 	Gecode::IntVar outputNetFirstStage1Voltage = netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2Voltage = netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::IntVar outputNetFirstStage1VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::rel(*this, abs(outputNetFirstStage1Voltage - outputNetFirstStage2Voltage) <= abs(outputNetFirstStage1VoltageOther - outputNetFirstStage2VoltageOther));
		}

		if((compP1 != nullptr && compP2 != nullptr))
		{
			Core::NetId outputNetFirstStage1 = compP1->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();
			Core::NetId outputNetFirstStage2 = compP2->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Drain")).getIdentifier();


		 	Gecode::IntVar outputNetFirstStage1Voltage = netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2Voltage = netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::IntVar outputNetFirstStage1VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage1);
		 	Gecode::IntVar outputNetFirstStage2VoltageOther = otherSpace.netToVoltageMap_.find(outputNetFirstStage2);
		 	Gecode::rel(*this, abs(outputNetFirstStage1Voltage - outputNetFirstStage2Voltage) <= abs(outputNetFirstStage1VoltageOther - outputNetFirstStage2VoltageOther));
		}
		}



//		for(int i =1 ; i!= importantNonDominantPoles_.size(); i++ )
//		{
//			Gecode::rel(*this, importantNonDominantPoles_[i] >= 0.99 * otherSpace.importantNonDominantPoles_[i]);
//		}


	}

	bool SimpleSearchSpace::master(const Gecode::MetaInfo & mi)
	{
		if(mi.last() != NULL)
		{
			constrain(*mi.last());
		}
		if(mi.solution() == 0)
		{
			mi.nogoods().post(*this);
		}
		return true;
	}



	void SimpleSearchSpace::createCircuitSpecificationConstraints()
	{
		CircuitSpecificationsConstraints specificationsConstraints;
		specificationsConstraints.setTwoPortToValueMap(twoPortToValueMap_);
		specificationsConstraints.setCircuitInformation(getCircuitInformation());
		specificationsConstraints.setGraph(getGraph());
		specificationsConstraints.setNetToVoltageMap(netToVoltageMap_);
		specificationsConstraints.setPartitioningResult(getPartitioningResult());
		specificationsConstraints.setSpace(*this);
		specificationsConstraints.setTransistorToCurrentMap(transistorToCurrentMap_);
		specificationsConstraints.setTransistorToLengthMap(transistorToLengthMap_);
		specificationsConstraints.setTransistorToWidthMap(transistorToWidthMap_);
		specificationsConstraints.setTransistorModel(getTransistorModel());
		specificationsConstraints.setStructureRecognitionResult(getStructureRecognitionResult());
		specificationsConstraints.createConstraints();
//		rel(*this, gain_ <= getCircuitInformation().getCircuitSpecification().getGain()+15);

	}


}

