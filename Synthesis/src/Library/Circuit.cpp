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

#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"
#include "AutomaticSizing/incl/Results/Result.h"

#include "Synthesis/incl/Library/Circuit.h"

#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/Results/Result.h"

#include "Partitioning/incl/Partitioning.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace Synthesis
{

	Circuit::Circuit() :
			flatCircuit_(nullptr),
			hierarchicalCircuit_(nullptr),
			structRecResults_(nullptr),
			partitioningResults_(nullptr),
			sizingResult_(nullptr),
			circuitInformation_(nullptr)
	{
	}

	Circuit::~Circuit()
	{

		if(circuitInformation_ != nullptr && hierarchicalCircuit_ == nullptr)
			delete circuitInformation_;
		delete hierarchicalCircuit_;
		delete flatCircuit_;
		delete structRecResults_;
		delete partitioningResults_;


	}

	void Circuit::initialize(const Core::Circuit & circuit, StructRec::Library & structRecLibrary)
	{
		logDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>> Initialize circuit:" << circuit.getCircuitIdentifier() << " >>>>>>>>>>>>>>>>>");
		if(circuit.hasInstances())
		{
			setHierarchicalCircuit(circuit);
			Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
			setFlatCircuit(*flatCircuitRecursion->createNewFlatCopy(circuit));
			delete flatCircuitRecursion;
		}
		else
		{
			setFlatCircuit(circuit);
		}
		logDebug("Create Structure recognition results");
		structRecResults_ = & createStructRecResult(structRecLibrary);
		logDebug("Create Partitioning results");
		partitioningResults_ = & createPartitioningResult();
		logDebug("Finish creating partitioning results");

	}

	void Circuit::setCircuitInformation(const AutomaticSizing::CircuitInformation & circuitInformation)
	{
		circuitInformation_ = & circuitInformation;
	}

	void Circuit::setSizingResult(const  AutomaticSizing::Result & sizingResult)
	{
		sizingResult_ = & sizingResult;
	}

	const Core::Circuit & Circuit::getFlatCircuit() const
	{
		assert(flatCircuit_ != nullptr);
		return *flatCircuit_;
	}

	const Core::Circuit & Circuit::getHierarchicalCircuit() const
	{
		assert(hierarchicalCircuit_ != nullptr);
		return *hierarchicalCircuit_;
	}

	const StructRec::StructureCircuits & Circuit::getConstStructRecResult() const
	{
		assert(structRecResults_ != nullptr);
		return *structRecResults_;
	}

	const StructRec::StructureCircuits & Circuit::getStructRecResult()
	{
		assert(structRecResults_ != nullptr);
		return *structRecResults_;
	}

	const Partitioning::Result & Circuit::getPartitioningResult() const
	{
		assert(partitioningResults_ != nullptr);
		return *partitioningResults_;
	}

	const AutomaticSizing::Result & Circuit::getSizingResult() const
	{
		assert(sizingResult_ != nullptr);
		return * sizingResult_;
	}

	const AutomaticSizing::CircuitInformation & Circuit::getCircuitInformation() const
	{
		assert(circuitInformation_ != nullptr);
		return * circuitInformation_;
	}

	std::string Circuit::toStr() const
	{
		std::ostringstream oss;

		oss << "<<<<<<<<<<<<<<< DEVICE CIRCUIT >>>>>>>>>>>>>>>>" << std::endl;
		oss << getFlatCircuit().toStr()<< std::endl;
		oss << "<<<<<<<<<<<<<<<<<< RECOGNIZED STRUCTURES >>>>>>>>>>>>>>>>>>" << std::endl;
		oss << getConstStructRecResult().toStr() << std::endl;
		oss << "<<<<<<<<<<<<<<<<<<<<< FUNCTIONAL BLOCKS >>>>>>>>>>>>>>>>>>>>" << std::endl;
		oss << getPartitioningResult().toStr() << std::endl;


		return oss.str();
	}

	bool Circuit::fulfillsFirstStageSpecifications() const
	{
		bool fulfills = true;
		logDebug(">>>>>>>>>>>>>>>Does not fulfill following specifications: ");
		if(getSizingResult().getCMRR() < getCircuitInformation().getCircuitSpecification().getCMRR())
		{
			fulfills = false;
			logDebug("CMRR " << getSizingResult().getCMRR());
		}
		if(getSizingResult().getArea() > getCircuitInformation().getCircuitSpecification().getMaxArea())
		{
			fulfills = false;
			logDebug("Area " << getSizingResult().getArea());
		}
		if(getSizingResult().getPower() > getCircuitInformation().getCircuitSpecification().getPowerConsumption())
		{
			fulfills = false;
			logDebug("Power " << getSizingResult().getPower());
		}
		if(getSizingResult().getPhaseMargin() < getCircuitInformation().getCircuitSpecification().getPhaseMargin())
		{
			fulfills = false;
			logDebug("PhaseMargin " << getSizingResult().getPhaseMargin());
		}
		if(getSizingResult().hasMaxCommonModeInputVoltage()
				&& getSizingResult().getMaxCommonModeInputVoltage() <
				getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() + getCircuitInformation().getCircuitSpecification().getMinCommonModeInputVoltage())
		{
			fulfills = false;
			logDebug("MaxCommonModeInputVoltageVoltage " << getSizingResult().getMaxCommonModeInputVoltage());
		}
		if(getSizingResult().getMinCommonModeInputVoltage() >
		getCircuitInformation().getCircuitParameter().getInputPinMinus().getValue() + getCircuitInformation().getCircuitSpecification().getMaxCommonModeInputVoltage())
		{
			fulfills = false;
			logDebug("MinCommonModeInputVoltage");
		}

		logDebug("Fulfills? " << fulfills);
		return fulfills;
	}

	bool Circuit::fulfillsAllSpecifications() const
	{
		bool fulfills = true;

		logDebug("Does not fulfill following specifications: ")
		if(!fulfillsFirstStageSpecifications())
		{
			fulfills = false;
			logDebug("FirstStageSpecifications!")
		}
		if(getSizingResult().getGain() < getCircuitInformation().getCircuitSpecification().getGain())
		{
			fulfills = false;
			logDebug("Gain " << getSizingResult().getGain());
		}
		if(getSizingResult().getMaxOutputVoltage() < getCircuitInformation().getCircuitSpecification().getMaxOutputVoltage())
		{
			fulfills = false;
			logDebug("MaxOutputVoltage " << getSizingResult().getMaxOutputVoltage());
		}
		if(getSizingResult().getMinOutputVoltage() > getCircuitInformation().getCircuitSpecification().getMinOutputVoltage())
		{
			fulfills = false;
			logDebug("MinOutputVoltage " << getSizingResult().getMinOutputVoltage());
		}
		if(getSizingResult().hasNegPSRR() &&
				getSizingResult().getNegPSRR() < getCircuitInformation().getCircuitSpecification().getNegPSRR())
		{
			fulfills =false;
			logDebug("NegPSRR " << getSizingResult().getNegPSRR());
		}
		if(getSizingResult().hasPosPSRR() &&
				getSizingResult().getPosPSRR() < getCircuitInformation().getCircuitSpecification().getPosPSRR())
		{
			fulfills = false;
			logDebug("PosPSRR " << getSizingResult().hasPosPSRR());
		}
		if(getSizingResult().getSlewRate() < getCircuitInformation().getCircuitSpecification().getSlewRate())
		{
			fulfills = false;
			logDebug("SlewRate " << getSizingResult().hasSlewRate());
		}
		if(getSizingResult().getTransitFrequency() < getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
			fulfills = false;
			logDebug("TransitFrequency " << getSizingResult().getTransitFrequency());
		}
		if(getSizingResult().getTransitFrequencyWithErrorFactor() < getCircuitInformation().getCircuitSpecification().getTransientFrequency())
		{
			fulfills = false;
			logDebug("TransitFrequencyWithErrorFunction " << getSizingResult().getTransitFrequencyWithErrorFactor());
		}
		logDebug("Fulfills: " << fulfills);
		return fulfills;
	}

	void Circuit::setFlatCircuit(const Core::Circuit & circuit)
	{
		assert(!circuit.hasInstances(), "The circuit should be flat without any instances");
		flatCircuit_ = & circuit;
	}

	void Circuit::setHierarchicalCircuit(const Core::Circuit & circuit)
	{
		assert(circuit.hasInstances(), "The circuit is hierarchical and must have Instances");
		hierarchicalCircuit_ = & circuit;
	}

	const StructRec::StructureCircuits & Circuit::createStructRecResult(StructRec::Library & structRecLibrary)
	{
		StructRec::Result* result = structRecLibrary.recognize(getFlatCircuit());
		return result->getTopLevelResults();
	}

	const Partitioning::Result & Circuit::createPartitioningResult()
	{
		Partitioning::Partitioning * partitioning = new Partitioning::Partitioning;
		Partitioning::Result & partitioningResult = partitioning->compute(getStructRecResult());
		delete partitioning;
		return partitioningResult;
	}


}
