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


#include "../../../incl/ConstraintProgram/Constraints/PolesAndZeros.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variables.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Other.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/PoleOrZero.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"
#include "AutomaticSizing/incl/CircuitInformation/Pin.h"

#include "Core/incl/Circuit/Net/NetId/NetId.h"

#include "Partitioning/incl/Results/Result.h"

namespace AutomaticSizing {

	const int PolesAndZeros::NOT_INITIALIZED_ = -100;


	PolesAndZeros::PolesAndZeros(const Partitioning::Result & partitioningResult, const CircuitInformation & circuitInformation) :
				numUnimportantZeros_(NOT_INITIALIZED_),
				numImportantZeros_(NOT_INITIALIZED_),
				numUnimportantNonDominantPoles_(NOT_INITIALIZED_),
				numImportantNonDominantPoles_(NOT_INITIALIZED_)
	{
		countNumberOfPolesAndZeros(partitioningResult, circuitInformation);
	}

	void PolesAndZeros::initialize(Gecode::Space& space)
	{
		dominantPole_ = Gecode::FloatVar(space,0, pow(10,9));
		positiveZero_ = Gecode::FloatVar(space,0, pow(10,9));
	}

	void PolesAndZeros::setDominantePole(
		Gecode::FloatVar dominantPole)
	{
		dominantPole_ = dominantPole;
	}

	void PolesAndZeros::setPositiveZero(Gecode::FloatVar positiveZero)
	{
		positiveZero_ = positiveZero;
	}

	void PolesAndZeros::addImportantNonDominantPole(
		Gecode::FloatVar nonDominantPole)
	{
		importantNonDominantPoles_.push_back(nonDominantPole);
	}

	void PolesAndZeros::addImportantZero(
		Gecode::FloatVar positiveZero)
	{
		importantZeros_.push_back(positiveZero);
	}

	void PolesAndZeros::addUnimportantNonDominantPole(
		Gecode::FloatVar nonDominantPole)
	{
		unimportantNonDominantPoles_.push_back(nonDominantPole);
	}

	void PolesAndZeros::addUnimportantZero(
		Gecode::FloatVar positiveZero)
	{
		unimportantZeros_.push_back(positiveZero);
	}

	int PolesAndZeros::getNumberImportantNonDominantPoles() const
	{
		assert(numImportantNonDominantPoles_ != NOT_INITIALIZED_);
		return numImportantNonDominantPoles_;
	}

	int PolesAndZeros::getNumberUnimportantNonDominantPoles() const
	{
		assert(numUnimportantNonDominantPoles_ != NOT_INITIALIZED_);
		return numUnimportantNonDominantPoles_;
	}

	int PolesAndZeros::getNumberImportantZeros() const
	{
		assert(numImportantZeros_ != NOT_INITIALIZED_);
		return numImportantZeros_;
	}

	int PolesAndZeros::getNumberUnimportantZeros() const
	{
		assert(numUnimportantZeros_ != NOT_INITIALIZED_);
		return numUnimportantZeros_;
	}

	Gecode::FloatVar PolesAndZeros::getDominantPole() const
	{
		return dominantPole_;
	}

	Gecode::FloatVar PolesAndZeros::getPositiveZero() const
	{
		return positiveZero_;
	}

	int PolesAndZeros::getPositiveZeroIndex() const
	{
		return positiveZeroIndex_;
	}

	std::vector<Gecode::FloatVar> PolesAndZeros::getImportantNonDominantPoles() const
	{
		return importantNonDominantPoles_;
	}

	std::vector<Gecode::FloatVar> PolesAndZeros::getUnimportantNonDominantPoles() const
	{
		return unimportantNonDominantPoles_;
	}

	std::vector<Gecode::FloatVar> PolesAndZeros::getImportantZeros() const
	{
		return importantZeros_;
	}

	std::vector<Gecode::FloatVar> PolesAndZeros::getUnimportantZeros() const
	{
		return unimportantZeros_;
	}

	int PolesAndZeros::getDominantPoleIndex() const
	{
		return dominatPoleIndex_;
	}

	std::vector<int> PolesAndZeros::getImportantNonDominantPolesIndexVector() const
	{
		return importantNonDominantPolesIndexVector_;
	}

	std::vector<int> PolesAndZeros::getUnimportantNonDominantPolesIndexVector() const
	{
		return unimportantNonDominantPolesIndexVector_;
	}

	std::vector<int> PolesAndZeros::getImportantZerosIndexVector() const
	{
		return importantZerosIndexVector_;
	}

	std::vector<int> PolesAndZeros::getUnimportantZerosIndexVector() const
	{
		return unimportantZerosIndexVector_;
	}

	std::string PolesAndZeros::toStr() const
	{
		std::ostringstream oss;

		oss << "Dominant Pole: " << dominantPole_ << std::endl;
		oss << "Non Dominant Poles used for Phase Margin calculations: ";
		for(auto & it: importantNonDominantPoles_)
		{
			oss << it << ", ";
		}
		oss << std::endl;
		oss << "Positive Zeros used for Phase Margin calculations: ";
		for(auto & it: importantZeros_)
		{
			oss << it << ", ";
		}
		oss << std::endl;
		oss << "Non Dominant Poles NOT used for Phase Margin calculations: ";
		for(auto & it: unimportantNonDominantPoles_)
		{
			oss << it << ", ";
		}
		oss << std::endl;
		oss << "Positive Zeros NOT used for Phase Margin calculations: ";
		for(auto & it: unimportantZeros_)
		{
			oss << it << ", ";
		}
		oss << std::endl;

		return oss.str();
	}

	void PolesAndZeros::countNumberOfPolesAndZeros(const Partitioning::Result & partitioningResult, const CircuitInformation & circuitInformation)
	{
		numImportantNonDominantPoles_ = 0;
		numUnimportantNonDominantPoles_ = 0;
		numImportantZeros_ = 0;
		numUnimportantZeros_ = 0;

		if(partitioningResult.getFirstStage().isComplementary())
		{
			numImportantNonDominantPoles_ = numImportantNonDominantPoles_+2;
		}
		else if(partitioningResult.getFirstStage().getLoadPart().size() == 2 ||
			partitioningResult.getBelongingComponents(**partitioningResult.getFirstStage().getLoadPart().begin()).size() >1	)
		{
			numImportantNonDominantPoles_++;
		}
		if(!partitioningResult.hasSecondStage() && partitioningResult.getFirstStage().getLoadPart().size() == 1)
		{
			numImportantZeros_++;
		}
		if(partitioningResult.getFirstStage().getLoadPart().size() == 2)
		{
			std::vector<Partitioning::LoadPart*> loadsFirstStage = partitioningResult.getFirstStage().getLoadPart();

			Partitioning::Component * outputConnectedComp = nullptr;
			Partitioning::Component * supplyConnectedComp = nullptr;
			for(auto & load : loadsFirstStage)
			{
				if(!load->hasCascodedPair() && partitioningResult.getBelongingComponents(*load).size() > 2)
				{
					for(auto & comp : partitioningResult.getBelongingComponents(*load))
					{
						if(isOutputNetFirstStage(comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Drain")).getIdentifier(), partitioningResult, circuitInformation))
						{
							outputConnectedComp = comp;

							const StructRec::StructureNet & sourceNet = outputConnectedComp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source"));
							for(auto & sourceConnectedStructure : sourceNet.getAllConnectedStructures())
							{
								if(sourceConnectedStructure->findNet(StructRec::StructurePinType(sourceConnectedStructure->getStructureName().toStr(), "Drain")) == sourceNet)
								{
									for(auto & comp2 : partitioningResult.getBelongingComponents(*load))
									{
										if(*sourceConnectedStructure == comp2->getArray())
										{
											supplyConnectedComp = comp2;
											break;
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
			}
			if(outputConnectedComp != nullptr && supplyConnectedComp != nullptr)
			{
				numUnimportantNonDominantPoles_++;
				numUnimportantNonDominantPoles_++;
				numUnimportantZeros_++;
				numUnimportantZeros_++;
			}
		}
		if(partitioningResult.hasSecondStage())
		{
			numImportantNonDominantPoles_++;
			if(partitioningResult.hasCompensationCapacitance()
					&& partitioningResult.getBelongingComponents(partitioningResult.getPrimarySecondStage()).size() == 2)
			{
				numImportantNonDominantPoles_++;
			}
			addBiasHigherStagesNonDominantPolesAndZeros(partitioningResult.getPrimarySecondStage(), partitioningResult);
//

			if(partitioningResult.hasSecondarySecondStage())
			{
				if(partitioningResult.getBelongingComponents(partitioningResult.getPrimarySecondStage()).size() == 1)
				{
					numImportantZeros_++;
				}
			}
//
		}
		if(partitioningResult.hasThirdStage())
		{
			numImportantNonDominantPoles_++;

			addBiasHigherStagesNonDominantPolesAndZeros(partitioningResult.getThirdStage(), partitioningResult);


		}
	}

	void PolesAndZeros::addBiasHigherStagesNonDominantPolesAndZeros(Partitioning::TransconductancePart & stage, const Partitioning::Result & partitioningResult)
	{
		Partitioning::BiasPart & biasPart = **stage.getBiasPart().begin();
		if(partitioningResult.getBelongingComponents(biasPart).size()==2)
		{
			for(auto &biasComp : partitioningResult.getBelongingComponents(biasPart))
			{
				if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{

					numUnimportantNonDominantPoles_++;
					numUnimportantZeros_++;
				}
				else if(stage.isPrimarySecondStage() && partitioningResult.getSecondStages().size() == 2
						&& biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
				{
					numUnimportantNonDominantPoles_++;
					numUnimportantZeros_++;
				}
			}
		}

		if(stage.isPrimarySecondStage() && partitioningResult.hasSecondarySecondStage())
		{
			Partitioning::BiasPart & biasPartSecondaryStage = **partitioningResult.getSecondarySecondStage().getBiasPart().begin();
			if(partitioningResult.getBelongingComponents(biasPartSecondaryStage).size()==2)
			{
				for(auto &biasComp : partitioningResult.getBelongingComponents(biasPartSecondaryStage))
				{
					if(!biasComp->getArray().findNet(StructRec::StructurePinType(biasComp->getArray().getStructureName().toStr(), "Source")).isSupply())
					{

						numUnimportantNonDominantPoles_++;
						numUnimportantZeros_++;
					}
				}
			}
		}
	}

	bool PolesAndZeros::isOutputNetFirstStage(Core::NetId outputNet, const Partitioning::Result & partitioningResult, const CircuitInformation & circuitInformation) const
	{
		bool isOutputNet = false;

		if(partitioningResult.hasSecondStage())
		{
			for(auto & secondStage :partitioningResult.getSecondStages())
			{
				Partitioning::Component & compSecondStage = **partitioningResult.getBelongingComponents(*secondStage).begin();
				if(compSecondStage.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate")).getIdentifier() == outputNet)
				{
					isOutputNet = true;
				}
			}
		}
		else
		{
			if(circuitInformation.getCircuitParameter().isFullyDifferential())
			{
				if(circuitInformation.getCircuitParameter().getOutputPinMinus().getNetId() == outputNet
						|| circuitInformation.getCircuitParameter().getOutputPinPlus().getNetId() == outputNet)
				{
					isOutputNet = true;
				}
			}
			else
			{
				if(circuitInformation.getCircuitParameter().getOutputPin().getNetId() == outputNet)
				{
					isOutputNet = true;
				}
			}
		}

		return isOutputNet;
	}

	void PolesAndZeros::setPolesAndZerosIndexes(int &index, std::map<int, std::string>  & indexToVariableMap,
			Variables & variables)
	{
		dominatPoleIndex_ = index;

		std::ostringstream dominantPoleString;
		dominantPoleString << "DominantPole";

		PoleOrZero *  dominantPole = new PoleOrZero;
		dominantPole->setName(dominantPoleString.str());
		dominantPole->setIndex(index);
		variables.add(*dominantPole);
		indexToVariableMap.insert(std::pair<int, std::string>(index, dominantPoleString.str()));

		index++;

		positiveZeroIndex_ = index;

		std::ostringstream positiveZeroString;
		positiveZeroString << "PositiveZero";

		PoleOrZero *  positiveZero = new PoleOrZero;
		positiveZero->setName(positiveZeroString.str());
		positiveZero->setIndex(index);
		variables.add(*positiveZero);
		indexToVariableMap.insert(std::pair<int, std::string>(index, positiveZeroString.str()));



		index++;

		for(int i = 0; i < numImportantNonDominantPoles_; i++)
		{
			importantNonDominantPolesIndexVector_.push_back(index);
			std::ostringstream nonDominantPoleString;
			int num = i +1;
			nonDominantPoleString << "ImportantNonDominantPole[" << num << "]";

			PoleOrZero *  pole = new PoleOrZero;
			pole->setName(nonDominantPoleString.str());
			pole->setIndex(index);
			variables.add(*pole);
			indexToVariableMap.insert(std::pair<int, std::string>(index, nonDominantPoleString.str()));
			index++;
		}

		for(int i = 0; i < numUnimportantNonDominantPoles_; i++)
		{
			unimportantNonDominantPolesIndexVector_.push_back(index);

			std::ostringstream nonDominantPoleString;
			int num = i +1;
			nonDominantPoleString << "UnimportantNonDominantPole[" << num << "]";

			PoleOrZero *  pole = new PoleOrZero;
			pole->setName(nonDominantPoleString.str());
			pole->setIndex(index);
			variables.add(*pole);

			indexToVariableMap.insert(std::pair<int, std::string>(index, nonDominantPoleString.str()));

			index++;
		}

		for(int i = 0; i < numImportantZeros_; i++)
		{
			importantZerosIndexVector_.push_back(index);

			std::ostringstream positiveZeroString;
			int num = i+1;
			positiveZeroString << "ImportantZeroPole[" << num << "]";

			PoleOrZero *  zero = new PoleOrZero;
			zero->setName(positiveZeroString.str());
			zero->setIndex(index);
			variables.add(*zero);

			indexToVariableMap.insert(std::pair<int, std::string>(index, positiveZeroString.str()));

			index++;
		}

		for(int i = 0; i < numUnimportantNonDominantPoles_; i++)
		{
			unimportantZerosIndexVector_.push_back(index);

			int num = i+1;
			std::ostringstream positiveZeroString;
			positiveZeroString << "UnimportantZeroPole[" << num << "]";

			PoleOrZero *  zero = new PoleOrZero;
			zero->setName(positiveZeroString.str());
			zero->setIndex(index);
			variables.add(*zero);

			indexToVariableMap.insert(std::pair<int, std::string>(index, positiveZeroString.str()));

			index++;
		}

	}

}

