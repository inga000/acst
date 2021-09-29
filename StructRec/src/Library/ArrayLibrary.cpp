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

#include "StructRec/incl/Library/ArrayLibrary.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/Results/CircuitRegister.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ArrayLibraryItem.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Structure/Connection/ArrayConnection.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/StructureCircuit/Net/ArrayNet.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "StructRec/incl/StructureCore.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	ArrayLibrary::ArrayLibrary():
			structureCore_(NULL)
	{
	}

	ArrayLibrary::~ArrayLibrary()
	{
		eraseAllLibraryItems();
	}

	void ArrayLibrary::setFilePath(const std::string& filePath)
	{
		filePath_ = filePath;
	}

	void ArrayLibrary::setStructureCore(StructureCore& structureCore)
	{
		structureCore_ = &structureCore;
	}

	void ArrayLibrary::addItem(ArrayLibraryItem& item)
	{
		arrayLibraryItems_.push_back(&item);
	}

	void ArrayLibrary::addAndMerge(const ArrayLibrary& other, LibraryItemMappings& mappings)
	{
		const ArrayLibraryItemVector& otherItems = other.getArrayLibraryItems();
		for(std::vector<ArrayLibraryItem*>::const_iterator it = otherItems.begin(); it != otherItems.end(); it++)
		{
			ArrayLibraryItem* otherItem = *it;
			if(!hasIdenticalArrayLibraryItem(*otherItem)) {
				ArrayLibraryItem* copy = new ArrayLibraryItem(*otherItem);
				getStructureCore().registerArrayLibraryItem(*copy);
				addItem(*copy);
			} else {
				LibraryItemMapping newMapping = createMapping(*otherItem);
				mappings.addLibraryItemMapping(otherItem->getName(), newMapping);
			}
		}
	}

	Result* ArrayLibrary::recognize(const Core::Circuit& circuit) const
	{
		Core::VisitedInstances visitedInstances;
		CircuitRegister* circuitRegister = new CircuitRegister();
		StructureCircuits& topLevelCircuits = recognizeArraysRecursively(circuit, *circuitRegister, visitedInstances);

		Result * result = new Result;
		result->setCircuitRegister(*circuitRegister);
		result->setTopLevelResults(topLevelCircuits);
		return result;
	}

	ArrayLibraryItem & ArrayLibrary::findArrayLibraryItem(const StructureName& structureName) const
	{
		assert(hasArrayLibraryItem(structureName));
		ArrayLibraryItem* arrayLibraryItem = NULL;
		for(std::vector<ArrayLibraryItem*>::const_iterator it= getArrayLibraryItems().begin(); it!=getArrayLibraryItems().end(); it++)
		{
			ArrayLibraryItem * possibleItem = *it;
			if(possibleItem->getName() == structureName)
			{
				arrayLibraryItem = possibleItem;
				break;
			}
		}
		return *arrayLibraryItem;
	}

	std::string ArrayLibrary::getFilePath() const
	{
		assert(!filePath_.empty());
		return filePath_;
	}

	std::string ArrayLibrary::getImageFilePath(const StructureName& structureName) const
	{
		assert(hasArrayLibraryItem(structureName));
		ArrayLibraryItem& item = findArrayLibraryItem(structureName);
		return item.getFilePath();
	}

	std::vector<StructureName> ArrayLibrary::getStructureNames() const
	{
		std::vector<StructureName> arrayNames;
		for(auto& it : arrayLibraryItems_)
		{
			ArrayLibraryItem* currentItem = it;
			arrayNames.push_back(currentItem->getName());
		}
		return arrayNames;
	}

	std::vector<StructurePinType> ArrayLibrary::getStructurePinTypes(const StructureName& structureName) const
	{
		ArrayLibraryItem& libraryItem = findArrayLibraryItem(structureName);
		return libraryItem.getStructurePinTypes();
	}

	bool ArrayLibrary::hasArrayLibraryItem(const StructureName& structureName) const
	{
		bool hasIt = false;
		for(std::vector<ArrayLibraryItem*>::const_iterator it= getArrayLibraryItems().begin(); it!=getArrayLibraryItems().end(); it++)
		{
			ArrayLibraryItem * possibleItem = *it;
			if(possibleItem->getName() == structureName)
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	void ArrayLibrary::writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const
	{
		Core::XmlNode & arrayLibrary = Core::RapidXmlUtils::addNode(node, doc, "arrayLibrary");
		Core::XmlNode & arrayLibraryItemFiles = Core::RapidXmlUtils::addNode(arrayLibrary, doc, "arrayLibraryItemFiles");
		Core::XmlNode & arrayLibraryItemsNode = Core::RapidXmlUtils::addNode(arrayLibrary, doc, "arrayLibraryItems");
		Core::RapidXmlUtils::addAttr(arrayLibraryItemsNode,doc, "level", "0");

		const std::vector<ArrayLibraryItem*>& arrayLibraryItems = getArrayLibraryItems();
		for(std::vector<ArrayLibraryItem*>::const_iterator it = arrayLibraryItems.begin(); it != arrayLibraryItems.end(); it++)
		{
			const ArrayLibraryItem * item = *it;
			Core::RapidXmlUtils::addSimpleNode(arrayLibraryItemFiles, doc, "arrayLibraryItemFile", item->getFilePath() );
			Core::XmlNode & arrayLibraryItemNode = Core::RapidXmlUtils::addNode(arrayLibraryItemsNode, doc, "arrayLibraryItem", item->getName().toStr() );
			if(!item->hasMaxPersistence())
			{
				std::stringstream ss;
				ss << item->getPersistence();
				std::string persistence = ss.str();
				Core::RapidXmlUtils::addAttr(arrayLibraryItemNode,doc, "persistence", persistence);
			}
		}
	}

	StructureCircuits & ArrayLibrary::recognizeArraysRecursively(const Core::Circuit & circuit, CircuitRegister & circuitRegister,
			Core::VisitedInstances & visitedInstances) const
	{
		StructureCircuits * recognizedCircuits = new StructureCircuits();
		recognizedCircuits->setCircuitId(circuit.getCircuitIdentifier());
		recognizedCircuits->setStructureCore(getStructureCore());

		//first descends to the loweset hierarchylevel and recognizes arrays there, ascends afterwards
		recognizeArraysInInstances(circuit, circuitRegister, *recognizedCircuits, visitedInstances);

		StructureCircuit* arrayCircuit = new StructureCircuit;
		arrayCircuit->setHierarchyLevel(getLevel());
		for(std::vector<ArrayLibraryItem*>::const_iterator it = arrayLibraryItems_.begin(); it != arrayLibraryItems_.end(); it++)
		{
			ArrayLibraryItem* item = *it;
			item->recognize(circuit, *arrayCircuit, visitedInstances);
		}

		completeCircuit(circuit, *arrayCircuit, *recognizedCircuits);
		recognizedCircuits->addStructureCircuit(*arrayCircuit);
		return *recognizedCircuits;
	}

	void ArrayLibrary::recognizeArraysInInstances(const Core::Circuit & circuit, CircuitRegister & circuitRegister,
			StructureCircuits & recognizedCircuits, Core::VisitedInstances & visitedInstances) const
	{
		if(circuit.hasInstances())
		{
			std::vector<const Core::Instance*> instances = circuit.findInstances();
			for(auto& it : instances)
			{
				const Core::Instance* instance = it;
				Core::CircuitId circuitId = instance->getMaster().getCircuitIdentifier();

	    		if(!circuitRegister.hasStructureCircuits(circuitId))
			 	{
	    			//recursive call
					visitedInstances.push(*instance);
	    			const Core::Circuit & instanceCircuit = instance->getMaster();
			   		StructureCircuits & recognizedInstanceCircuits = recognizeArraysRecursively(instanceCircuit, circuitRegister, visitedInstances);
			   		circuitRegister.addStructureCircuits(circuitId, recognizedInstanceCircuits);
			    	visitedInstances.pop();
	    		}

	    		Core::InstanceId instanceId = instance->getInstanceIdentifier();
				StructureCircuits& structureCircuits = circuitRegister.findStructureCircuits(circuitId);
				recognizedCircuits.addNewInstance(instanceId, structureCircuits);

				//füge terminals hinzu
			}
			//if instances is empty, lowest hierarchy level reached => returns to recognize arrays

		}
		
	}

	void ArrayLibrary::completeCircuit(const Core::Circuit& circuit, StructureCircuit& recognizedCircuit,
			StructureCircuits & recognizedCircuits) const
	{
		addInstanceTerminals(circuit, recognizedCircuit, recognizedCircuits);
		addTerminals(circuit, recognizedCircuit);
	}

	void ArrayLibrary::addInstanceTerminals(const Core::Circuit & circuit, StructureCircuit& recognizedCircuit,
			StructureCircuits & circuits) const
	{
		if(circuit.hasInstances())
		{
			std::vector<const Core::Instance*> instancePointers = circuit.findInstances();
			for(auto& instIt : instancePointers)
			{
				const Core::Instance& lowerInstance = *instIt;
				StructureInstance & structureInstance = circuits.findInstance(lowerInstance.getIdentifier());

				std::vector<const Core::InstanceTerminal*> instTermPointers = lowerInstance.getInstanceTerminals();
				for(auto& instTermIt : instTermPointers)
				{
					const Core::InstanceTerminal& instTerm = *instTermIt;

					if(structureInstance.hasTerminalInHighestInitializedCircuit(instTerm.getIdentifier()))
					{
						StructureInstanceTerminal * newInstTerm = new StructureInstanceTerminal;
						newInstTerm->setInstance(structureInstance);
						newInstTerm->setTerminal(structureInstance.getTerminalOfHighestInitializedCircuit(instTerm.getIdentifier()));

						if(recognizedCircuit.hasStructureNet(instTerm.getNet().getIdentifier()))
						{
							newInstTerm->connect(recognizedCircuit.findStructureNet(instTerm.getNet().getIdentifier()));
						}
						else
						{
							ArrayNet* newNet = new ArrayNet;
							newNet->setNetId(instTerm.getNet().getIdentifier());
							newNet->setSupply(instTerm.getNet().getSupply());
							newNet->setArrayNet(instTerm.getNet());
							newInstTerm->connect(*newNet);
							recognizedCircuit.addStructureNet(*newNet);
						}

						structureInstance.addInstanceTerminal(getLevel(), *newInstTerm);
						recognizedCircuit.addInstanceTerminal(*newInstTerm);
					}
				}
			}
		}
		
	}

	void ArrayLibrary::addTerminals(const Core::Circuit & circuit, StructureCircuit & recognizedCircuit) const
	{
		std::vector<const Core::Terminal*> terminals = circuit.findTerminals();
		for(auto& it : terminals)
		{
			const Core::Terminal* terminal = it;
			Core::NetId terminalNetId = terminal->getNetId();
			if(recognizedCircuit.hasStructureNet(terminalNetId))
			{
				StructureNet& structureNet = recognizedCircuit.findStructureNet(terminalNetId);

				StructureTerminal* newTerminal = new StructureTerminal();
				newTerminal->setCircuit(recognizedCircuit);
				newTerminal->setTerminalId(terminal->getIdentifier());
				newTerminal->setTerminalType(terminal->getTerminalType());
				newTerminal->setAndConnectNet(structureNet);
				recognizedCircuit.addTerminal(*newTerminal);
			}
		}
	}

	bool ArrayLibrary::hasIdenticalArrayLibraryItem(const ArrayLibraryItem& otherItem) const
	{
		bool hasIt = false;
		for(std::vector<ArrayLibraryItem*>::const_iterator it = arrayLibraryItems_.begin(); it !=arrayLibraryItems_.end(); it++)
		{
			ArrayLibraryItem* currentItem = *it;
			if(currentItem->isIdentical(otherItem))
			{
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	LibraryItemMapping ArrayLibrary::createMapping(const ArrayLibraryItem& otherItem) const
	{
		assert(hasIdenticalArrayLibraryItem(otherItem));
		LibraryItemMapping newMapping;
		for(std::vector<ArrayLibraryItem*>::const_iterator it = arrayLibraryItems_.begin(); it !=arrayLibraryItems_.end(); it++)
		{
			ArrayLibraryItem* currentItem = *it;
			if(currentItem->isIdentical(otherItem))
			{
				newMapping = currentItem->createMapping(otherItem);
				newMapping.setMappedName(currentItem->getName());
				break;
			}
		}
		return newMapping;
	}

	void ArrayLibrary::eraseAllLibraryItems()
	{
		for(std::vector<ArrayLibraryItem*>::const_iterator it = arrayLibraryItems_.begin(); it != arrayLibraryItems_.end(); it++)
		{
			delete *it;
		}
	}

	int ArrayLibrary::getLevel() const
	{
		ArrayLibraryItem* item = *arrayLibraryItems_.begin();
		return item->getHierarchyLevel();
	}

	const StructureCore& ArrayLibrary::getStructureCore() const
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	StructureCore& ArrayLibrary::getStructureCore()
	{
		assert(structureCore_ != NULL);
		return *structureCore_;
	}

	const ArrayLibrary::ArrayLibraryItemVector& ArrayLibrary::getArrayLibraryItems() const
	{
		return arrayLibraryItems_;
	}

	ArrayLibrary::ArrayLibraryItemVector& ArrayLibrary::getArrayLibraryItems()
	{
		return arrayLibraryItems_;
	}
}


