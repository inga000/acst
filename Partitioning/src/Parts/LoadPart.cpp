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

#include "Partitioning/incl/Parts/LoadPart.h"
#include "Partitioning/incl/Results/Result.h"

namespace Partitioning {

	LoadPart::LoadPart() :
			cascodedPair_(NULL)
	{

	}

	LoadPart::LoadPart(int & num) :
		cascodedPair_(NULL)
	{
		const PartName & name = PartName("LoadPart");
	    PartId id;
	    id.setName(name);
	    id.setId(num);
	    setPartId(id);
	    num++;
	}

	void LoadPart::createPartId(int & num)
	{
		const PartName & name = PartName("LoadPart");
	    PartId id;
	    id.setName(name);
	    id.setId(num);
	    setPartId(id);
	    num++;
	}

	LoadPart::~LoadPart()
	{
	}

	void LoadPart::setCascodedPair(
		const StructRec::Structure& foldedPair)
	{
		cascodedPair_ = &foldedPair;
	}

	void LoadPart::addCurrentBiasOfFoldedPair(
		const StructRec::Structure& currentBiasOfFoldedPair)
	{
		currentBiasOfFoldedPair_.push_back(&currentBiasOfFoldedPair);
	}

	void LoadPart::addBiasPart(Part& biasPart)
	{
		biasParts_.push_back(&biasPart);
	}

	bool LoadPart::isInitialized() const
	{
		return hasMainStructures() && hasPartId();
	}

	const StructRec::Structure& LoadPart::getCascodedPair() const
	{
		assert(hasCascodedPair());
		return * cascodedPair_;
	}

	std::vector<const StructRec::Structure*>& LoadPart::getCurrentBiasOfFoldedPair()
	{
		assert(hasCurrentBiasOfFoldedPair());
		return currentBiasOfFoldedPair_;
	}

	Part& LoadPart::getBiasPart(PartId partId) const
	{
		assert(hasBiasPart(partId));
		Part * part = NULL;
		for(std::vector<Part*>::const_iterator it = biasParts_.begin(); it != biasParts_.end(); it++)
		{
			Part * posPart = *it;
			if(posPart->getPartId() == partId)
			{
				part = posPart;
				break;
			}
		}
		return *part;
	}

	bool LoadPart::hasCascodedPair() const
	{
		return cascodedPair_ != NULL;
	}

	bool LoadPart::hasCurrentBiasOfFoldedPair() const
	{
		return !currentBiasOfFoldedPair_.empty();
	}

	bool LoadPart::hasBiasPart() const
	{
		return !biasParts_.empty();
	}

	bool LoadPart::hasBiasPart(PartId partId) const
	{
		bool hasBiasPart = false;

		for(std::vector<Part*>::const_iterator it = biasParts_.begin(); it != biasParts_.end(); it++)
		{
			Part * posPart = *it;
			if(posPart->getPartId() == partId)
			{
				hasBiasPart = true;
				break;
			}
		}
		return hasBiasPart;
	}

	void LoadPart::print(std::ostream & stream) const
	{
		if(hasCascodedPair())
		{
			stream << " FoldedPair: " << getCascodedPair().getIdentifier().toStr() << std::endl;
			if(hasCurrentBiasOfFoldedPair())
			{
				stream << " Current bias of folded pair: ";

				for(std::vector<const StructRec::Structure*>::const_iterator it = currentBiasOfFoldedPair_.begin();
						it != currentBiasOfFoldedPair_.end(); it++)
				{
					const StructRec::Structure * structure = * it;
					stream << structure->getIdentifier().toStr();
					stream << ", ";
				}
					stream << std::endl;
			}
		}
		if(hasBiasPart())
		{
			stream << " BiasParts: ";

			for(std::vector<Part*>::const_iterator it = biasParts_.begin(); it != biasParts_.end(); it++)
			{
				Part * biasPart = * it;
				stream << biasPart->getPartId().toStr();
				biasPart->printMainStructures(stream);
			}
			stream << std::endl;
		}

	}

	void LoadPart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
	   Core::XmlNode& loadPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "loadPart");
	   Part::writeXml(loadPart, doc);
	}

	bool LoadPart::isBiasPart() const
	{
		return false;
	}

	bool LoadPart::isLoadPart() const
	{
		return true;
	}

	bool LoadPart::isTransconductancePart() const
	{
		return false;
	}

	bool LoadPart::isCapacitancePart() const
	{
		return false;
	}


	bool LoadPart::isUndefinedPart() const
	{
		return false;
	}

	const std::vector<Part*>  & LoadPart::getBiasParts() const
	{
		return biasParts_;
	}

	bool LoadPart::consistsOfTwoDiodeArraysConnectedToSecondStageInput(const Partitioning::Result & result) const
	{
		bool isTrue = true;
		
		const StructRec::Structure * dta1 = nullptr;
		const StructRec::Structure * dta2 = nullptr;

		for(auto & struc : getMainStructures())
		{
			if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray")
				&& dta1 == nullptr)
			{
					dta1 = struc;
			}
			else if(struc->getIdentifier().getName() == StructRec::StructureName("MosfetDiodeArray"))
			{
				dta2 = struc;
			}
		}
		if( dta1 != nullptr && dta2 != nullptr && result.hasSecondStage())
		{
			const StructRec::StructureNet & gateNet1 = dta1->findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain")); //Is a diode array, therefore  gate and drain net are equal
			const StructRec::StructureNet & gateNet2 = dta2->findNet(StructRec::StructurePinType("MosfetDiodeArray", "Drain"));
			for(auto & secondStage : result.getSecondStages())
			{
				Component & supplyConnectedComp = **secondStage->findSupplyConnectedComponents(result).begin();
				const StructRec::StructureNet & gateNetSecondStage = supplyConnectedComp.getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
				if(gateNet1.getIdentifier() != gateNetSecondStage.getIdentifier() && gateNet2.getIdentifier() != gateNetSecondStage.getIdentifier())
				{
					isTrue = false;
				}
			}
			return isTrue;
		}
		else
		{
			return false;
		}
	}

	bool LoadPart::isResistorPart() const
	{
		return false;
	}

	bool LoadPart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool LoadPart::isPositiveFeedbackPart() const
	{
		return false;
	}


	bool  LoadPart::hasCrossCoupledPair(const Partitioning::Result & result) const
	{
		bool hasIt = false;
		const StructRec::Structure * nta1 = nullptr;
		const StructRec::Structure * nta2 = nullptr;

		for(auto & comp : result.getBelongingComponents(*this))
		{
			if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray" && nta1 == nullptr)
			{
				nta1 = &comp->getArray();
			}
			else if(comp->getArray().getStructureName().toStr() == "MosfetNormalArray")
			{
				nta2 = & comp->getArray();
			}
		}
		if(nta1 != nullptr && nta2 != nullptr)
		{
			const StructRec::Structure * crossCoupledPair = nullptr;
			for(auto & parent: nta1->getParents())
			{
				if(parent->getStructureName().toStr() == "MosfetCrossCoupledPair")
				{
					crossCoupledPair = parent;
					break;
				}
			}
			if(crossCoupledPair != nullptr)
			{
				for(auto & parent: nta2->getParents())
				{
					if(parent->getStructureName().toStr() == "MosfetCrossCoupledPair")
					{
						if(crossCoupledPair->getIdentifier() == parent->getIdentifier())
						{
							hasIt = true;
						}
					}
				}
			}
		}
		return hasIt;
	};

}


