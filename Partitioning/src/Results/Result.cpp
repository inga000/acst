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


#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"

#include "Partitioning/incl/Results/Result.h"
#include "StructRec/incl/StructureCircuit/Structure/Array.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {


Result::Result()
{
}

Result::~Result()
{
	eraseTransconductanceParts();
	eraseLoadParts();
	eraseBiasParts();
	eraseCapacitanceParts();
	eraseUndefinedParts();
	eraseCommonModeSignalDetectorParts();
	eraseResistorParts();
	erasePositiveFeedbackParts();
}

std::string Result::toStr() const
{
	std::ostringstream stream;
	stream << "Parts: " << std::endl;
	stream << "TransconductaneParts:" << std::endl;
	for(std::map<PartId, TransconductancePart*>::const_iterator it_transParts = transconductanceParts_.begin(); it_transParts != transconductanceParts_.end(); it_transParts++)
	{

		TransconductancePart * part = it_transParts->second;

		stream << part->toStr();

	}
	stream << std::endl;
	stream << "BiasParts:" << std::endl;
	for(std::map<PartId, BiasPart*>::const_iterator it_biasParts = biasParts_.begin(); it_biasParts != biasParts_.end(); it_biasParts++)
	{
		BiasPart * part = it_biasParts->second;
		stream << part->toStr();
	}
	stream << std::endl;
	stream << "LoadParts:" << std::endl;
	for(std::map<PartId, LoadPart*>::const_iterator it_loadParts = loadParts_.begin(); it_loadParts != loadParts_.end(); it_loadParts++)
	{
		LoadPart * part = it_loadParts->second;
		stream << part->toStr();
	}
	stream << std::endl;

	stream << "CapacitanceParts:" << std::endl;
	for(std::map<PartId, CapacitancePart*>::const_iterator it_capParts = capacitanceParts_.begin(); it_capParts != capacitanceParts_.end(); it_capParts++)
	{
		CapacitancePart * part = it_capParts->second;
		stream << part->toStr();
	}
	stream << std::endl;

	stream << "ResistorParts:" << std::endl;
	for(auto & it : resistorParts_)
	{
		ResistorPart * part = it.second;
		stream << part->toStr();
	}
	stream << std::endl;

	stream << "CommonModeSignalDetectorParts:" << std::endl;
	for(auto & it: commonModeSignalDetectorParts_)
	{
		CommonModeSignalDetectorPart * part = it.second;
		stream << part->toStr();
	}
	stream << std::endl;

	stream << "PositiveFeedbackParts:" << std::endl;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart * part = it.second;
		stream << part->toStr();
	}
	stream << std::endl;


	stream << "UndefinedParts:" << std::endl;
	for(std::map<PartId, UndefinedPart*>::const_iterator it_unParts = undefinedParts_.begin(); it_unParts != undefinedParts_.end(); it_unParts++)
	{
		UndefinedPart * part = it_unParts->second;
		stream << part->toStr();
	}

	return stream.str();
}

bool Result::structureAlreadyClassified(const StructRec::Structure& structure) const
{
	bool classified = true;
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure& array = *it;
		if(twoPorts_.find(array.getIdentifier()) == twoPorts_.end() && transistors_.find(array.getIdentifier()) == transistors_.end() )
		{
			classified = false;
		}
	}

	return classified;
}

bool Result::deviceAlreadyClassified(const Core::Device & device) const
{
	bool isClassified = false;
	for(auto & it : transistors_)
	{
		const Transistor & transistor = *it.second;
		for(auto & dev : transistor.getArray().getDevices())
		{
			if(device.getIdentifier() == dev->getIdentifier())
			{
				isClassified = true;
				break;
			}
		}
		if(isClassified)
			break;
	}
	if(!isClassified)
	{
		for(auto & it : twoPorts_)
		{
			const TwoPort & twoPort = *it.second;
			for(auto & dev : twoPort.getArray().getDevices())
			{
				if(device.getIdentifier() == dev->getIdentifier())
				{
					isClassified = true;
					break;
				}
			}
			if(isClassified)
				break;
		}
	}
	return isClassified;
}

Part & Result::getPart(const StructRec::Structure & structure )
{
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(twoPorts_.find(array.getIdentifier()) != twoPorts_.end())
		{
			TwoPort * comp = twoPorts_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			break;
		}
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			break;
		}
	}
	return * part;
}

const Part & Result::getPart(const StructRec::Structure & structure ) const
{
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(twoPorts_.find(array.getIdentifier()) != twoPorts_.end())
		{
			TwoPort * comp = twoPorts_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			break;
		}
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			break;
		}
	}
	return * part;
}

Part & Result::getPart(const Core::Device & device)
{
	Part * part = NULL;
	assert(deviceAlreadyClassified(device), "Device must be classified");
	for(auto & it : transistors_)
	{
		const Transistor & transistor = *it.second;
		for(auto & dev : transistor.getArray().getDevices())
		{
			if(device.getIdentifier() == dev->getIdentifier())
			{
				part = &transistor.getPart();
				break;
			}
		}
		if(part != nullptr)
			break;
	}
	if(part != nullptr)
	{
		for(auto & it : twoPorts_)
		{
			const TwoPort & twoPort = *it.second;
			for(auto & dev : twoPort.getArray().getDevices())
			{
				if(device.getIdentifier() == dev->getIdentifier())
				{
					part = &twoPort.getPart();
					break;
				}
			}
			if(part != nullptr)
				break;
		}
	}
	return * part;
}

const Part & Result::getPart(const Core::Device & device ) const
{
	Part * part = nullptr;
	assert(deviceAlreadyClassified(device), "Device must be classified");
	for(auto & it : transistors_)
	{
		const Transistor & transistor = *it.second;
		for(auto & dev : transistor.getArray().getDevices())
		{
			if(device.getIdentifier() == dev->getIdentifier())
			{
				part = &transistor.getPart();
				break;
			}
		}
		if(part != nullptr)
			break;
	}
	if(part == nullptr)
	{
		for(auto & it : twoPorts_)
		{
			const TwoPort & twoPort = *it.second;
			for(auto & dev : twoPort.getArray().getDevices())
			{
				if(device.getIdentifier() == dev->getIdentifier())
				{
					part = &twoPort.getPart();
					break;
				}
			}
			if(part != nullptr)
				break;
		}
	}
	assert(part != nullptr, "No part found for the device!");
	return * part;
}

std::vector<BiasPart *> Result::getAllVoltageBiases() const
{
	std::vector<BiasPart*> voltageBiases;
	for(auto & it: getAllBiasParts())
	{
		if(it->isVoltageBias())
		{
			voltageBiases.push_back(it);
		}
	}
	return voltageBiases;
}

std::vector<BiasPart*> Result::getAllCurrentBiases() const
{
	std::vector<BiasPart*> currentBiases;
	for(auto & it: getAllBiasParts())
	{
		if(it->isCurrentBias())
		{
			currentBiases.push_back(it);
		}
	}
	return currentBiases;
}

void Result::addTwoPort(TwoPort& twoPort)
{
	twoPorts_.insert(std::pair<StructRec::StructureId, TwoPort* >(twoPort.getArray().getIdentifier(),& twoPort));
}

void Result::addTransistor(Transistor& transistor)
{
	transistors_.insert(std::pair<StructRec::StructureId, Transistor* >(transistor.getArray().getIdentifier(),& transistor));
}

void Result::addTransconductancePart(TransconductancePart& part)
{
	transconductanceParts_.insert(std::pair<PartId, TransconductancePart* >(part.getPartId(),& part));
}

void Result::addLoadPart(LoadPart& part)
{
	loadParts_.insert(std::pair<PartId, LoadPart* >(part.getPartId(),& part));
}

void Result::addBiasPart(BiasPart& part)
{
	biasParts_.insert(std::pair<PartId, BiasPart* >(part.getPartId(),& part));
}

void Result::addResistorPart(ResistorPart & part)
{
	resistorParts_.insert(std::pair<PartId, ResistorPart* >(part.getPartId(),& part));
}

void Result::addCommonModeSignalDetectorPart(CommonModeSignalDetectorPart & detectPart)
{
	commonModeSignalDetectorParts_.insert(std::pair<PartId, CommonModeSignalDetectorPart* >(detectPart.getPartId(),& detectPart));
}


TransconductancePart& Result::getTransconductancePart(
		const StructRec::Structure& structure)
{
	TransconductancePart * transPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isTransconductancePart(), "Is not a transconductance part.");
			transPart =  transconductanceParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * transPart;
}

const TransconductancePart& Result::getTransconductancePart(
		const StructRec::Structure& structure) const
{
	TransconductancePart * transPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isTransconductancePart(), "Is not a transconductance part.");
			transPart =  transconductanceParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * transPart;
}

BiasPart& Result::getBiasPart(const StructRec::Structure& structure)
{
	BiasPart * biasPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isBiasPart(), "Is not a bias part.");
			biasPart =  biasParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * biasPart;
}

const BiasPart& Result::getBiasPart(const StructRec::Structure& structure) const
{
	BiasPart * biasPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isBiasPart(), "Is not a bias part.");
			biasPart =  biasParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * biasPart;
}

LoadPart& Result::getLoadPart(const StructRec::Structure& structure)
{
	LoadPart * loadPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isLoadPart(), "Is not a load part.");
			loadPart =  loadParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * loadPart;
}

const LoadPart& Result::getLoadPart(const StructRec::Structure& structure) const
{
	LoadPart * loadPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isLoadPart(), "Is not a load part.");
			loadPart =  loadParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * loadPart;
}

std::vector<TransconductancePart*> Result::getAllTransconductanceParts() const
{
	assert(hasTransconductanceParts());
	std::vector<TransconductancePart*> transParts;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin(); it != transconductanceParts_.end(); it++)
	{
		TransconductancePart* transPart = it->second;
		transParts.push_back(transPart);
	}
	return transParts;
}

std::vector<LoadPart*> Result::getAllLoadParts() const
{
	assert(hasLoadParts());
	std::vector<LoadPart*> loadParts;
	for(std::map<PartId, LoadPart*>::const_iterator it = loadParts_.begin(); it != loadParts_.end(); it++)
	{
		LoadPart* loadPart = it->second;
		loadParts.push_back(loadPart);
	}
	return loadParts;
}

std::vector<BiasPart*> Result::getAllBiasParts() const
{
	assert(hasBiasParts());
	std::vector<BiasPart*> biasParts;
	for(std::map<PartId, BiasPart*>::const_iterator it = biasParts_.begin(); it != biasParts_.end(); it++)
	{
		BiasPart* biasPart = it->second;
		biasParts.push_back(biasPart);
	}
	return biasParts;
}

std::vector<Transistor*> Result::getAllTransistors() const
{
	assert(!transistors_.empty());
	std::vector<Transistor*> transistors;
	for(std::map<StructRec::StructureId, Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Transistor * tran = it->second;
		transistors.push_back(tran);
	}
	return transistors;
}

std::vector<TwoPort*> Result::getAllTwoPorts() const
{
	assert(!twoPorts_.empty());
	std::vector<TwoPort*> twoPorts;
	for(std::map<StructRec::StructureId, TwoPort*>::const_iterator it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		TwoPort * twoPort = it->second;
		twoPorts.push_back(twoPort);
	}
	return twoPorts;
}

std::vector<Component*> Result::getAllComponents() const
{
	assert(!twoPorts_.empty() || !transistors_.empty());
	std::vector<Component*> components;
	for(std::map<StructRec::StructureId, Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Transistor * tran = it->second;
		components.push_back(tran);
	}
	for(std::map<StructRec::StructureId, TwoPort*>::const_iterator it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		TwoPort * twoPort = it->second;
		components.push_back(twoPort);
	}
	return components;

}

void Result::addUndefinedPart(UndefinedPart & part)
{
	undefinedParts_.insert(std::pair<PartId,UndefinedPart*>(part.getPartId(), & part));
}



bool Result::hasTransconductanceParts() const
{
	return !transconductanceParts_.empty();
}

bool Result::hasLoadParts() const
{
	return !loadParts_.empty();
}

bool Result::hasBiasParts() const
{
	return !biasParts_.empty();
}

bool Result::isLoadPartOfFirstStage(const Part & part) const
{
	bool isIt = false;
	for(auto & loadPart : getFirstStage().getLoadPart())
	{
		if(loadPart->getPartId() == part.getPartId())
		{
			isIt = true;
			break;
		}
	}
	return isIt;
}


bool Result::isClassifiedVoltageBias(const StructRec::Structure & structure) const
{
	if(!structureAlreadyClassified(structure))
	{
		return false;
	}
	else if(!getPart(structure).isBiasPart())
	{
		return false;
	}
	else if(getBiasPart(structure).isVoltageBias())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Result::isClassifiedCurrentBias(const StructRec::Structure & structure) const
{
	if(!structureAlreadyClassified(structure))
	{
		return false;
	}
	else if(&getBiasPart(structure) == NULL)
	{
		return false;
	}
	else if(getBiasPart(structure).isCurrentBias())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Result::addCapacitancePart(CapacitancePart& part)
{
	capacitanceParts_.insert(std::pair<PartId, CapacitancePart*> (part.getPartId(), & part));
}

CapacitancePart& Result::getCapacitancePart(
		const StructRec::Structure& structure)
{
	CapacitancePart * capPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(twoPorts_.find(array.getIdentifier()) != twoPorts_.end())
		{
			TwoPort * comp = twoPorts_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isCapacitancePart(), "Is not a capacitance part.");
			capPart =  capacitanceParts_.find(part->getPartId())->second;
			break;

		}
	}
	return * capPart;
}

const CapacitancePart& Result::getCapacitancePart(
		const StructRec::Structure& structure) const
{
	CapacitancePart * capPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(twoPorts_.find(array.getIdentifier()) != twoPorts_.end())
		{
			TwoPort * comp = twoPorts_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isCapacitancePart(), "Is not a capacitance part.");
			capPart =  capacitanceParts_.find(part->getPartId())->second;
			break;

		}
	}
	return * capPart;
}


ResistorPart & Result::getResistorPart(const StructRec::Structure & structure) const
{
	ResistorPart* wantedPart = nullptr;

	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(twoPorts_.find(array.getIdentifier()) != twoPorts_.end())
		{
			TwoPort * comp = twoPorts_.find(array.getIdentifier())->second;

			part = & comp->getPart();
			assert(part->isResistorPart(), "Is not a resistor part.");
			wantedPart =  resistorParts_.find(part->getPartId())->second;
			break;

		}
		else if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isResistorPart(), "Is not a resistor part.");
			wantedPart =  resistorParts_.find(part->getPartId())->second;
			break;

		}
	}
	assert(wantedPart != nullptr, "Why is the resistor part not in the results?");
	return* wantedPart;
}

std::vector<CapacitancePart*> Result::getAllCapacitanceParts() const
{
	assert(hasCapacitanceParts());
	std::vector<CapacitancePart*> capParts;
	for(auto & it : capacitanceParts_)
	{
		CapacitancePart* capPart = it.second;
		capParts.push_back(capPart);
	}
	return capParts;
}

std::vector<CommonModeSignalDetectorPart*> Result::getAllCommonModeSignalDetectorParts() const
{
	assert(hasCommonModeSignalDetectorParts());
	std::vector<CommonModeSignalDetectorPart*> detectParts;
	for(auto & it : commonModeSignalDetectorParts_)
	{
		CommonModeSignalDetectorPart* detectPart = it.second;
		detectParts.push_back(detectPart);
	}
	return detectParts;
}
std::vector<ResistorPart*> Result::getAllResistorParts() const
{
	assert(hasResistorParts());
	std::vector<ResistorPart*> resParts;
	for(auto & it : resistorParts_)
	{
		ResistorPart* resPart = it.second;
		resParts.push_back(resPart);
	}
	return resParts;
}


std::vector<Component*> Result::getBelongingComponents(const Part & part) const
{
	std::vector<Component*> components;

	auto compAllReadyIn = [] (Component & comp, std::vector<Component*> components) {bool hasIt = false;
		for(auto & vectorComp : components)
		{
			if(*vectorComp == comp)
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	};

	std::vector<const StructRec::Structure*> mainStructures = part.getMainStructures();
	for(std::vector<const StructRec::Structure*>::const_iterator it_struct = mainStructures.begin(); it_struct != mainStructures.end(); it_struct++)
	{
		const StructRec::Structure * mainStructure = * it_struct;
		std::vector<const StructRec::Structure*> arrayChildren = mainStructure->getArrayChildren();
		for(auto& it_arrays : arrayChildren)
		{
			const StructRec::Structure & arrayChild = * it_arrays;
			Component & comp = findComponent(arrayChild.getIdentifier());
			if(!compAllReadyIn(comp, components))
			{
				components.push_back(&comp);
			}
		}
	}
	return components;
}

Component & Result::findComponent(StructRec::StructureId structureId) const
{
	Component* comp = NULL;
	for(std::map<StructRec::StructureId, Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Transistor * tran = it->second;
		if(tran->getArray().getIdentifier() == structureId)
		{
			comp = tran;
		}
	}
	for(std::map<StructRec::StructureId, TwoPort*>::const_iterator it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		TwoPort * port = it->second;
		if(port->getArray().getIdentifier() == structureId)
		{
			comp = port;
		}
	}
	assert(comp != NULL, "No suited Components found, Structure might be a pair.");
	return *comp;
}


std::vector<Core::NetId> Result::findBiasNets() const
{
	std::vector<Core::NetId> biasNets;
	std::vector<BiasPart*> biasPartsFirstStage = getFirstStage().getBiasPart();
	Core::PinType gatePin = Core::PinType(Core::DeviceTypeName("Mosfet"),Core::PinName( "Gate"));
	for(auto biasPartFirstStage : biasPartsFirstStage)
	{
		std::vector<Component*> comps = getBelongingComponents(*biasPartFirstStage);
		for(auto & comp : comps)
		{
			const Core::Device & device = **comp->getArray().getDevices().begin();

			Core::NetId gateNet = device.findNet(gatePin).getIdentifier();
			biasNets.push_back(gateNet);
		}
	}
	if(!hasSecondarySecondStage() && hasPrimarySecondStage())
	{
		std::vector<BiasPart*> biasPartsSecondStage = (**getSecondStages().begin()).getBiasPart();
		for(auto biasPartSecondStage : biasPartsSecondStage)
		{
			std::vector<Component*> comps = getBelongingComponents(*biasPartSecondStage);
			for(auto & comp : comps)
			{
				const Core::Device & device = **comp->getArray().getDevices().begin();
				Core::NetId gateNet = device.findNet(gatePin).getIdentifier();
				biasNets.push_back(gateNet);
			}
		}
	}
	std::vector<LoadPart*> loadParts = getAllLoadParts();
	for(auto & loadPart : loadParts)
	{
		std::vector<Component*> comps = getBelongingComponents(*loadPart);
		const Component * supplyComp = nullptr;
		const Component * outputComp = nullptr;
		for(auto & comp : comps)
		{
			if(supplyComp == nullptr
					&& comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply()
					&& !comp->getArray().isPartOfCurrentMirror())
			{
				supplyComp = comp;
			}
			else if(outputComp == nullptr
					&& !comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName().toStr(), "Source")).isSupply()
					&& !comp->getArray().isPartOfCurrentMirror())
			{
				outputComp = comp;
			}
		}

		if(supplyComp != nullptr)
		{
			const Core::Device & device = **supplyComp->getArray().getDevices().begin();
			Core::NetId gateNet = device.findNet(gatePin).getIdentifier();
			biasNets.push_back(gateNet);
		}
		if(outputComp != nullptr)
		{
			const Core::Device & device = **outputComp->getArray().getDevices().begin();
			Core::NetId gateNet = device.findNet(gatePin).getIdentifier();
			biasNets.push_back(gateNet);
		}

	}
	return biasNets;
}

bool Result::hasCapacitanceParts() const
{
	return !capacitanceParts_.empty();
}

bool Result::hasCommonModeSignalDetectorParts() const
{
	return !commonModeSignalDetectorParts_.empty();
}

bool Result::hasResistorParts() const
{
	return !resistorParts_.empty();
}

std::vector<Component*> Result::findComponents(const StructRec::Structure & structure) const
{
	std::vector<Component*> components;
	std::vector<const StructRec::Structure*> arrayChildren = structure.getArrayChildren();
	for(auto& it : arrayChildren)
	{
		const StructRec::Structure & arrayChild = *it;
		Component & component = findComponent(arrayChild.getIdentifier());
		components.push_back(& component);
	}
	return components;
}

bool Result::hasFirstStage() const
{
	bool hasIt = false;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
			it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isFirstStage() )
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasSecondStage() const
{
	bool hasIt = false;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
			it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isPrimarySecondStage() || transPart->isSecondarySecondStage() )
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasThirdStage() const
{
	bool hasIt = false;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
			it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isThirdStage())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasPrimarySecondStage() const
{
	if(hasSecondStage())
	{
		bool hasIt = false;
		for(auto & secondStage : getSecondStages())
		{
			if(secondStage->isPrimarySecondStage())
			{
				hasIt = true;
			}
		}
		return hasIt;
	}
	else
	{
		return false;
	}

}

bool Result::hasSecondarySecondStage() const
{
	if(hasSecondStage())
	{
		bool hasIt = false;
		for(auto & secondStage : getSecondStages())
		{
			if(secondStage->isSecondarySecondStage())
			{
				hasIt = true;
			}
		}
		return hasIt;
	}
	else
	{
		return false;
	}
}

std::vector<TransconductancePart*> Result::getSecondStages() const
{
	assert(hasSecondStage(), "Does not have SecondStage!");
	std::vector<TransconductancePart*> secondStages;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
			it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isPrimarySecondStage() || transPart->isSecondarySecondStage())
		{
			secondStages.push_back(transPart);
		}
	}

	return secondStages;

}

TransconductancePart & Result::getPrimarySecondStage() const
{
	TransconductancePart * primarySecondStage = nullptr;
	for(auto & it : getSecondStages())
	{
		if(it->isPrimarySecondStage())
		{
			primarySecondStage = it;
		}
	}
	assert(primarySecondStage != nullptr, "Has no primary second stage");
	return *primarySecondStage;
}

TransconductancePart & Result::getSecondarySecondStage() const
{
	TransconductancePart * secondarySecondStage = nullptr;
	for(auto & it : getSecondStages())
	{
		if(it->isSecondarySecondStage())
		{
			secondarySecondStage = it;
		}
	}
	assert(secondarySecondStage != nullptr, "Has no secondary second stage");
	return *secondarySecondStage;
}

TransconductancePart& Result::getThirdStage() const
{
	assert(hasThirdStage(), "Does not have ThirdStage!");
	TransconductancePart * thirdStage = NULL;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
			it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isThirdStage())
		{
			thirdStage =  transPart;
			break;
		}
	}

	return *thirdStage;

}

TransconductancePart & Result::getFirstStage() const
{
	assert(hasFirstStage());
	TransconductancePart * firstStage = NULL;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
					it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isFirstStage())
		{
			firstStage =  transPart;
			break;
		}
	}

	return *firstStage;

}

TransconductancePart & Result::getFeedbackStage() const
{
	assert(hasFeedbackStage());
	TransconductancePart * feedbackStage = NULL;
	for(std::map<PartId, TransconductancePart*>::const_iterator it = transconductanceParts_.begin();
					it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		if(transPart->isFeedBack())
		{
			feedbackStage =  transPart;
			break;
		}
	}

	return *feedbackStage;

}

bool Result::hasCompensationCapacitance() const
{
	bool hasIt = false;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
					it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		if(capPart->isCompensationCapacitance())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasCompensationResistor() const
{
	bool hasIt = false;
	for(auto & it : resistorParts_)
	{
		ResistorPart * resPart = it.second;
		if(resPart->isCompensationResistor())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

CapacitancePart & Result::getCompensationCapacitance() const
{
	assert(hasCompensationCapacitance(), "Does not have a Compensationcapacitance!");
	CapacitancePart * compCap = NULL;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
					it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		if(capPart->isCompensationCapacitance())
		{
			compCap = capPart;
			break;
		}
	}
	return *compCap;
}

std::vector<CapacitancePart*>  Result::getCompensationCapacitors() const
{
	assert(hasCompensationCapacitance(), "Does not have a Compensationcapacitance!");
	std::vector<CapacitancePart*> compCaps;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
					it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		if(capPart->isCompensationCapacitance())
		{
			compCaps.push_back(capPart);
		}
	}
	return compCaps;
}

ResistorPart & Result::getCompensationResistor() const
{
	assert(hasCompensationResistor(), "Does not have a Compensation Resistor!");

	Partitioning::Component & compCap = **getBelongingComponents(getCompensationCapacitance()).begin();
	const StructRec::StructureNet & minusNetCap = compCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Minus"));
	const StructRec::StructureNet & plusNetCap = compCap.getArray().findNet(StructRec::StructurePinType("CapacitorArray","Plus"));

	ResistorPart * compRes = NULL;
	for(auto & it : resistorParts_)
	{
		ResistorPart * resPart = it.second;
		Component & resComp = **getBelongingComponents(*resPart).begin();
		if(resComp.getArray().getStructureName().toStr() == "ResistorArray")
		{
			const StructRec::StructureNet & minusNetRes = resComp.getArray().findNet(StructRec::StructurePinType("ResistorArray","Minus"));
			const StructRec::StructureNet & plusNetRes = resComp.getArray().findNet(StructRec::StructurePinType("ResistorArray","Plus"));
			if(resPart->isCompensationResistor() && (minusNetRes == minusNetCap || minusNetRes == plusNetCap
					|| plusNetRes == plusNetCap || plusNetRes == minusNetCap))
			{
			compRes = resPart;
			break;
			}
		}
		else
		{
			const StructRec::StructureNet & minusNetRes = resComp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray","Drain"));
			if(resPart->isCompensationResistor() && (minusNetRes == minusNetCap || minusNetRes == plusNetCap))
			{
				compRes = resPart;
				break;
			}
		}
	}
	assert(compRes, "The compensation resistor should be connected to compensation capacitor!");
	return *compRes;
}

bool Result::hasLoadCapacitance() const
{
	bool hasIt = false;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
				it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		if(capPart->isLoadCapacitance())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasFeedbackStage() const
{
	bool hasIt = false;
	for(auto & transPart : getAllTransconductanceParts())
	{
		if(transPart->isFeedBack())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasFirstStageGainEnhancer() const
{
	bool hasIt = false;
	if(hasGainEnhancer())
	{
		for(auto & pfPart : getAllGainEnhancer())
		{
			if(pfPart->getStage().getPartId() == getFirstStage().getPartId())
			{
				hasIt = true;
			}
		}
	}
	return hasIt;
}

bool Result::isInFeedback(const Component & comp) const
{
	bool isIn = false;
	Part & part = comp.getPart();

	for(auto & transPart : getAllTransconductanceParts())
	{
		if(transPart->isFeedBack())
		{
			if(transPart->getPartId() == part.getPartId())
			{
				isIn = true;
				break;
			}
			else
			{
				for(auto & loadPart : transPart->getLoadPart())
				{
					if(loadPart->getPartId() == part.getPartId())
					{
						isIn = true;
						break;
					}
				}
				if(isIn)
					break;
				for(auto & biasPart: transPart->getBiasPart())
				{
					if(biasPart->getPartId() == part.getPartId())
					{
						isIn = true;
						break;
					}
				}
				if(isIn)
					break;
			}
		}
	}
	return isIn;


}

bool Result::isInOpAmp(const Component & comp) const
{
	bool isIn = false;
	Part & part = comp.getPart();

	for(auto & transPart : getAllTransconductanceParts())
	{
		if(transPart->getPartId() == part.getPartId())
		{
			isIn = true;
			break;
		}
		else
		{
			if(transPart->hasLoadPart())
			{
				for(auto & loadPart : transPart->getLoadPart())
				{
					if(loadPart->getPartId() == part.getPartId())
					{
						isIn = true;
						break;
					}
				}
			}
			if(isIn)
				break;


			for(auto & biasPart: transPart->getBiasPart())
			{
				if(biasPart->getPartId() == part.getPartId())
				{
					isIn = true;
					break;
				}
			}
			if(isIn)
				break;
		}
	}
	return isIn;
}

bool Result::isInMainBias(const Component & comp) const
{
	return !isInOpAmp(comp);
}

std::vector<CapacitancePart*> Result::getLoadCapacities() const
{
	assert(hasLoadCapacitance(), "Does not have a loadCapacitance!");
	std::vector<CapacitancePart * > loadCaps;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
					it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		if(capPart->isLoadCapacitance())
		{
			loadCaps.push_back(capPart);
		}
	}
	return loadCaps;
}

CapacitancePart & Result::getLoadCapacity(Core::DeviceId deviceId) const
{
	assert(hasCapacity(deviceId), "Does not have a capacity with that device id!");

	CapacitancePart * wantedPart = nullptr;
	for(std::map<PartId, CapacitancePart*>::const_iterator it = capacitanceParts_.begin();
					it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capPart = it->second;
		Component & capPartComp = **getBelongingComponents(*capPart).begin();
		Core::DeviceId capPartDeviceId = (**capPartComp.getArray().getDevices().begin()).getIdentifier();
		if(capPartDeviceId == deviceId && capPart->isLoadCapacitance())
		{
			wantedPart = capPart;
			break;
		}

	}

	return *wantedPart;
}

const PositiveFeedbackPart &  Result::getFirstStageGainEnhancer() const
{
	assert(hasFirstStageGainEnhancer(), "Need to have a first stage Gain enhancer!");
	PositiveFeedbackPart* firstStageGainEnhancer = nullptr;

	for(auto & pfPart : getAllGainEnhancer())
	{
		if(pfPart->getStage().getPartId() == getFirstStage().getPartId())
		{
			 firstStageGainEnhancer = pfPart;
		}
	}
	return *firstStageGainEnhancer;
}

bool Result::isPartOfHelperStructureOfDifferentialPair(Component & component) const
{
	bool isIt = false;
	TransconductancePart & firstStage = getFirstStage();
	if(firstStage.hasHelperStructure())
	{
		const StructRec::Structure & helperStruct = firstStage.getHelperStructure();
		std::vector<Component*> helperComps = findComponents(helperStruct);

		for(std::vector<Component*>::const_iterator it = helperComps.begin(); it != helperComps.end(); it++)
		{
			Component & helperComp = **it;
			if(component == helperComp)
			{
				isIt = true;
				break;
			}
		}
	}
	return isIt;
}

bool Result::isBiasOfFoldedPair(Component & component) const
{
	bool isIt = false;
	TransconductancePart & firstStage = getFirstStage();
	if(firstStage.hasHelperStructure())
	{
		std::vector<LoadPart*> loadParts = getAllLoadParts();
		for(std::vector<LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
		{
			LoadPart* loadPart = *it;
			if(loadPart->hasCurrentBiasOfFoldedPair())
			{
				std::vector<const StructRec::Structure*> biasOfFoldedPair = loadPart->getCurrentBiasOfFoldedPair();
				for(auto & it_struc : biasOfFoldedPair)
				{
					if(component.getArray() == *it_struc)
					{
						isIt = true;
						break;
					}
				}
				break;
			}
		}
	}
		return isIt;
}

Component & Result::getBiasOfFoldedPair() const
{
	Component * biasOfFoldedPair = NULL;
	std::vector<LoadPart*> loadParts = getAllLoadParts();
	for(std::vector<LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
	{
		LoadPart* loadPart = *it;
		if(loadPart->hasCurrentBiasOfFoldedPair())
		{
			biasOfFoldedPair = &findComponent((*loadPart->getCurrentBiasOfFoldedPair().begin())->getIdentifier());
			break;
		}
	}

	assert(biasOfFoldedPair != NULL, "No Folded Pair in the circuit!");
	return * biasOfFoldedPair;
}

bool Result::hasBiasOfFoldedPair() const
{
	bool hasIt = false;
	std::vector<LoadPart*> loadParts = getAllLoadParts();
	for(std::vector<LoadPart*>::const_iterator it = loadParts.begin(); it != loadParts.end(); it++)
	{
		LoadPart* loadPart = *it;
		if(loadPart->hasCurrentBiasOfFoldedPair())
		{
			hasIt = true;
			break;
		}
	}


	return hasIt;
}

void Result::eraseTransistors()
{
	for(std::map<StructRec::StructureId,Transistor*>::iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Transistor* tran = it->second;
		delete tran;
	}

}
void Result::eraseTwoPorts()
{
	for(std::map<StructRec::StructureId,TwoPort*>::iterator  it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		TwoPort * twoPort = it->second;
		delete twoPort;
	}

}

void Result::eraseTransconductanceParts()
{
	for(std::map<PartId, TransconductancePart*>::iterator it = transconductanceParts_.begin(); it != transconductanceParts_.end(); it++)
	{
		TransconductancePart * transPart = it->second;
		delete transPart;
	}

}

void Result::eraseLoadParts()
{
	for(std::map<PartId, LoadPart*>::iterator it = loadParts_.begin(); it != loadParts_.end(); it++)
	{
		LoadPart * loadPart = it->second;
		delete loadPart;
	}

}

void Result::eraseBiasParts()
{
	for(std::map<PartId, BiasPart*>::iterator it =  biasParts_.begin(); it != biasParts_.end(); it++)
	{
		BiasPart * biasPart = it->second;
		delete biasPart;
	}

}

void Result::eraseCommonModeSignalDetectorParts()
{
	for(auto & it : commonModeSignalDetectorParts_)
	{
		CommonModeSignalDetectorPart * detectPart = it.second;
		delete detectPart;
	}
}

void Result::eraseResistorParts()
{
	for(auto & it : resistorParts_)
	{
		ResistorPart * resPart = it.second;
		delete resPart;
	}
}

void Result::eraseCapacitanceParts()
{
	for(std::map<PartId, CapacitancePart*>::iterator it =  capacitanceParts_.begin(); it != capacitanceParts_.end(); it++)
	{
		CapacitancePart * capacitancePart = it->second;
		delete capacitancePart;
	}

}

bool Result::hasCapacity(Core::DeviceId deviceId) const
{
	bool hasIt = false;

	for(auto & capPartMapItem : capacitanceParts_)
	{
		CapacitancePart * capPart = capPartMapItem.second;
		Component & capPartComp = **getBelongingComponents(*capPart).begin();
		Core::DeviceId capPartDeviceId = (**capPartComp.getArray().getDevices().begin()).getIdentifier();
		if(capPartDeviceId == deviceId)
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

void Result::eraseUndefinedParts()
{
	for(std::map<PartId, UndefinedPart*>::iterator it = undefinedParts_.begin(); it != undefinedParts_.end(); it++)
	{
		UndefinedPart * undefinedPart = it->second;
		delete undefinedPart;
	}
}


void Result::writeXmlPartitioningResult(Control::OutputFile & file)const
{
	file.open();

	Core::XmlDocument doc;
	Core::XmlNode & root = Core::RapidXmlUtils::createXMLRootNodeForResults(doc);
	Core::RapidXmlUtils::writeDateXML(root, doc);
	Core::XmlNode & headLine = Core::RapidXmlUtils::addNode(root, doc, "circuit_partitioning_results");

//	if(hasTransconductanceParts())
	{
		Core::XmlNode &transPartNode = Core::RapidXmlUtils::addNode(headLine, doc, "gmParts");
		for(std::map<PartId, TransconductancePart*>::const_iterator it_transParts = transconductanceParts_.begin(); it_transParts != transconductanceParts_.end(); it_transParts++)
		{
			TransconductancePart * part = it_transParts->second;
			part->writeXml(transPartNode, doc);
		}
	}

//	if(hasLoadParts())
	{
		Core::XmlNode &loadPartNode = Core::RapidXmlUtils::addNode(headLine, doc, "loadParts");
		for(std::map<PartId, LoadPart*>::const_iterator it_loadParts = loadParts_.begin(); it_loadParts != loadParts_.end(); it_loadParts++)
		{
			LoadPart * part = it_loadParts->second;
			part->writeXml(loadPartNode, doc);
		}
	}

//	if(hasBiasParts())
	{
		Core::XmlNode &biasPartNode = Core::RapidXmlUtils::addNode(headLine, doc, "biasParts");
		for(std::map<PartId, BiasPart*>::const_iterator it_biasParts = biasParts_.begin(); it_biasParts != biasParts_.end(); it_biasParts++)
		{
			BiasPart * part = it_biasParts->second;
			part->writeXml(biasPartNode, doc);
		}
	}

//	if(hasCapacitanceParts())
	{
		Core::XmlNode &capacitancePartNode = Core::RapidXmlUtils::addNode(headLine, doc, "capacitances");
		for(std::map<PartId, CapacitancePart*>::const_iterator it_capParts = capacitanceParts_.begin(); it_capParts != capacitanceParts_.end(); it_capParts++)
		{
			CapacitancePart * part = it_capParts->second;
			part->writeXml(capacitancePartNode, doc);
		}
	}

//	if(hasResistorParts())
	{
		Core::XmlNode &resistorPartNode = Core::RapidXmlUtils::addNode(headLine, doc, "resistorParts");
		for(auto & it : resistorParts_)
		{
			ResistorPart * part = it.second;
			part->writeXml(resistorPartNode, doc);
		}
	}

//	if(hasCommonModeSignalDetectorParts())
	{
		Core::XmlNode &partNode = Core::RapidXmlUtils::addNode(headLine, doc, "commonModeSignalDetectorParts");
		for(auto & it: commonModeSignalDetectorParts_)
		{
			CommonModeSignalDetectorPart * part = it.second;
			part->writeXml(partNode, doc);
		}
	}

//	if(hasPositiveFeedbackParts())
	{
		Core::XmlNode &partNode = Core::RapidXmlUtils::addNode(headLine, doc, "positiveFeedbackParts");
		for(auto & it: positiveFeedbackParts_)
		{
			PositiveFeedbackPart * part = it.second;
			part->writeXml(partNode, doc);
		}
	}

//	if(hasPositiveFeedbackParts())
	{
		Core::XmlNode &partNode = Core::RapidXmlUtils::addNode(headLine, doc, "undefinedParts");
		for(auto & it: undefinedParts_)
		{
			UndefinedPart * part = it.second;
			part->writeXml(partNode, doc);
		}
	}

	std::ostringstream oss;
	oss << doc;
	file << oss.str();

	file.close();

}

void Result::eraseDoubledSecondStage()
{
	for(auto & secondStage : getSecondStages())
	{
		if(*secondStage->getMainStructures().begin() == *getThirdStage().getMainStructures().begin())
		{
			transconductanceParts_.erase(transconductanceParts_.find(secondStage->getPartId()));

		}
	}

}



void Result::addPositiveFeedbackPart(PositiveFeedbackPart& part)
{
	positiveFeedbackParts_.insert(std::pair<PartId, PositiveFeedbackPart* >(part.getPartId(),& part));
}

const PositiveFeedbackPart& Result::getPositiveFeedbackPart(
		const StructRec::Structure& structure) const
{
	PositiveFeedbackPart * positiveFeedbackPart = NULL;
	Part * part = NULL;
	assert(structureAlreadyClassified(structure));
	std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
	for(auto& it : arrays)
	{
		const StructRec::Structure & array = *it;
		if(transistors_.find(array.getIdentifier()) != transistors_.end())
		{
			Transistor * comp = transistors_.find(array.getIdentifier())->second;
			part = & comp->getPart();
			assert(part->isPositiveFeedbackPart(), "Is not a positive feedback part.");
			positiveFeedbackPart =  positiveFeedbackParts_.find(part->getPartId())->second;
			break;
		}
	}
	return * positiveFeedbackPart;
}

std::vector<PositiveFeedbackPart*> Result::getAllPositiveFeedbackParts() const
{
	assert(hasPositiveFeedbackParts());
	std::vector<PositiveFeedbackPart*> positiveFeedbackParts;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart* positiveFeedbackPart = it.second;
		positiveFeedbackParts.push_back(positiveFeedbackPart);
	}
	return positiveFeedbackParts;
}

std::vector<PositiveFeedbackPart*> Result::getAllSlewRateEnhancer() const
{
	assert(hasSlewRateEnhancer());
	std::vector<PositiveFeedbackPart*> slewRateEnhancer;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart* positiveFeedbackPart = it.second;
		if(positiveFeedbackPart->isSlewRateEnhancer())
			slewRateEnhancer.push_back(positiveFeedbackPart);
	}
	return slewRateEnhancer;
}

std::vector<PositiveFeedbackPart*> Result::getAllGainEnhancer() const
{
	assert(hasGainEnhancer());
	std::vector<PositiveFeedbackPart*> gainEnhancer;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart* positiveFeedbackPart = it.second;
		if(positiveFeedbackPart->isGainEnhancer())
			gainEnhancer.push_back(positiveFeedbackPart);
	}
	return gainEnhancer;
}

bool Result::hasPositiveFeedbackParts() const
{
	return !positiveFeedbackParts_.empty();
}

bool Result::hasGainEnhancer() const
{
	bool hasIt = false;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart* positiveFeedbackPart = it.second;
		if(positiveFeedbackPart->isGainEnhancer())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

bool Result::hasSlewRateEnhancer() const
{
	bool hasIt = false;
	for(auto & it: positiveFeedbackParts_)
	{
		PositiveFeedbackPart* positiveFeedbackPart = it.second;
		if(positiveFeedbackPart->isSlewRateEnhancer())
		{
			hasIt = true;
			break;
		}
	}
	return hasIt;
}

void Result::erasePositiveFeedbackParts()
{
	for(auto & it : positiveFeedbackParts_)
	{
		PositiveFeedbackPart * positiveFeedbackPart = it.second;
		delete positiveFeedbackPart;
	}
}

}
