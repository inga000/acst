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

#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNetsMap.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayRecognitionRules/ArrayRecognitionRule.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNets.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/ArrayConnection.h"
#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	ArrayLibraryItem::ArrayLibraryItem():
			parallelNets_(NULL),
			arrayConnection_(NULL)
	{
	}

	ArrayLibraryItem::ArrayLibraryItem(const ArrayLibraryItem& other):
		LibraryItem(other),
		parallelNets_(new ParallelNets(other.getParallelNets())),
		arrayConnection_(new ArrayConnection(other.getArrayConnection()))
	{
		copyRecognitionRules(other);
	}

	ArrayLibraryItem::~ArrayLibraryItem()
	{
		delete parallelNets_;
		delete arrayConnection_;
		eraseRecognitionRules();
	}

	void ArrayLibraryItem::setArrayConnection(const ArrayConnection& arrayConnection)
	{
		arrayConnection_ = &arrayConnection;
	}

	void ArrayLibraryItem::addRecognitionRule(const ArrayRecognitionRule & recognitionRule)
	{
		recoginitionsRules_.push_back(&recognitionRule);
	}

	void ArrayLibraryItem::setParallelNets(const ParallelNets& parallelNets)
	{
		parallelNets_ = &parallelNets;
	}

	void ArrayLibraryItem::recognize(const Core::Circuit& circuit,
			StructureCircuit& levelCircuit, Core::VisitedInstances & visitedInstances)
	{
	    ParallelNetsMap parallelNetsMap;
        std::vector<const Core::Device*> devices = circuit.findDevices();
        for (auto& it : devices)
        {
            const Core::Device* device = it;
            if (checkRecognitionRules(*device)) {
                ParallelNetsMapKey parallelNetsMapKey = getParallelNets().computeParallelNetsMapKey(*device);
                parallelNetsMap.add(parallelNetsMapKey, *device);
            }
        }

        parallelNetsMap.createArrays(circuit, levelCircuit, getArrayConnection(), getName(), visitedInstances, getIdCounter(), getPersistence());
	}

	const std::vector<const ArrayRecognitionRule*>& ArrayLibraryItem::getRecognitionRules() const
	{
		assert(hasRecognitionRules());
		return recoginitionsRules_;
	}

	const ParallelNets& ArrayLibraryItem::getParallelNets() const
	{
		assert(hasParallelNets());
		return *parallelNets_;
	}

	const ArrayConnection& ArrayLibraryItem::getArrayConnection() const
	{
		assert(hasArrayConnection());
		return *arrayConnection_;
	}

	bool ArrayLibraryItem::checkRecognitionRules(const Core::Device& device) const
	{
	    bool isPossibleArray = true;
	    const std::vector<const ArrayRecognitionRule*>& primaryRules = getRecognitionRules();
		for (std::vector<const ArrayRecognitionRule*>::const_iterator it = primaryRules.begin(); it != primaryRules.end(); it++)
		{
			const ArrayRecognitionRule* rule = *it;
			if(!rule->evaluate(device)) {
			    isPossibleArray = false;
			    break;
			}
		}
		return isPossibleArray;
	}

	bool ArrayLibraryItem::hasRecognitionRules() const
	{
		return !(recoginitionsRules_.empty());
	}

	bool ArrayLibraryItem::hasParallelNets() const
	{
		return parallelNets_ != NULL;
	}

	bool ArrayLibraryItem::hasArrayConnection() const
	{
		return arrayConnection_ != NULL;
	}
	void ArrayLibraryItem::copyRecognitionRules(const ArrayLibraryItem& other)
	{
	    for(std::vector<const ArrayRecognitionRule*>::const_iterator it = other.recoginitionsRules_.begin();
	    		it != other.recoginitionsRules_.end(); it++)
		{
	    	const ArrayRecognitionRule* currentRule = *it;
	    	ArrayRecognitionRule* copy = currentRule->clone();
	    	recoginitionsRules_.push_back(copy);
		}
	}

	void ArrayLibraryItem::eraseRecognitionRules()
	{
	    for(std::vector<const ArrayRecognitionRule*>::iterator it = recoginitionsRules_.begin(); it != recoginitionsRules_.end(); it++)
		{
			delete * it;
		}
	}

	bool ArrayLibraryItem::isIdentical(const ArrayLibraryItem& other) const
	{
		bool identicalArrayConnection = getArrayConnection().isIdentical(other.getArrayConnection());
		bool identicalParallelNets = getParallelNets().isIdentical(other.getParallelNets());
		bool identicalRecognitionRules = compareRecognitionRules(other);
		return identicalArrayConnection && identicalParallelNets && identicalRecognitionRules;
	}

	LibraryItemMapping ArrayLibraryItem::createMapping(const ArrayLibraryItem& other) const
	{
		assert(isIdentical(other));
		return getArrayConnection().createMapping(other.getArrayConnection());
	}

	bool ArrayLibraryItem::compareRecognitionRules(const ArrayLibraryItem& other) const
	{
		if(recoginitionsRules_.size() != other.recoginitionsRules_.size()) {
			return false;
		}

		bool identicalRules = true;
		for(std::vector<const ArrayRecognitionRule*>::const_iterator it = recoginitionsRules_.begin();
				it != recoginitionsRules_.end(); it++)
		{
			const ArrayRecognitionRule* originalRule = *it;
			bool hasIdenticalRule = false;
			for(std::vector<const ArrayRecognitionRule*>::const_iterator it2 = other.recoginitionsRules_.begin();
					it2 != other.recoginitionsRules_.end(); it2++)
			{
				const ArrayRecognitionRule* otherRule = *it2;
				if(originalRule->isIdentical(*otherRule)) {
					hasIdenticalRule = true;
					break;
				}
			}

			if(!hasIdenticalRule) {
				identicalRules = false;
				break;
			}
		}
		return identicalRules;
	}

	std::vector<StructurePinType> ArrayLibraryItem::getStructurePinTypes() const
	{
		return getArrayConnection().getStructurePinTypes();
	}

	bool ArrayLibraryItem::isInitialized() const
	{
		return hasRecognitionRules() && hasParallelNets() && hasArrayConnection();
	}

}
