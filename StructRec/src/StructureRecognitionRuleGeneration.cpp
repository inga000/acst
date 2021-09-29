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

#include "StructRec/incl/StructureRecognitionRuleGeneration.h"
#include "StructRec/incl/RuleGeneration/PairLibraryItemCreator.h"
#include "StructRec/incl/StructureRecognitionAnalysis.h"
#include "StructRec/incl/Library/Library.h"
#include "StructRec/incl/Library/PairLibrary.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "HSpice/incl/OutputFile/OutputFile.h"
#include "Control/incl/FileFormat/OutputFileFormat.h"
#include "Control/incl/FileSystemPath.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"
#include "StructRec/incl/RuleGeneration/NewRecognizedChild.h"
#include "StructRec/incl/RuleGeneration/NewPairLibraryItem.h"
#include <sys/types.h>
#include <sys/stat.h>

namespace StructRec {

	StructureRecognitionRuleGeneration::StructureRecognitionRuleGeneration() :
		            Control::FlatCircuitAnalysis<LocalOptionsForRuleGeneration>(),
					newLibrary_(NULL),
					library_(NULL)
    {
    }

    StructureRecognitionRuleGeneration::~StructureRecognitionRuleGeneration()
    {
    	delete library_;
    }

    void StructureRecognitionRuleGeneration::compute()
    {
    	parseLibrary();
    	initializeNewPairLibrary();

    	parseLibrary();
    	initializeNewPairLibrary();

		Result* result = getLibrary().recognize(getFlatCircuit());
		StructureCircuits& resultingCircuits = result->getTopLevelResults();

		std::vector<Structure*> children = resultingCircuits.findAllStructuresWithoutParents();
		int idCounter = 1;
		do
		{
			 PairLibraryItemCreator itemCreation;
			 itemCreation.setStructureCore(getLibrary().getStructureCore());
			 itemCreation.setPairLibraryItemName(getLocalOptions().getStructureName());

			 std::vector<NewPairLibraryItem*> newLibraryItems = itemCreation.build(children, idCounter);
			 for(std::vector<NewPairLibraryItem*>::iterator it_items = newLibraryItems.begin();
					 it_items != newLibraryItems.end(); it_items++)
			 {
				 NewPairLibraryItem* newItem = * it_items;
				 getPairLibrary().addItem(*newItem);
				 getNewPairLibraryItems().push_back(newItem);
			 }

			 getPairLibrary().recognizeOnStructuresWithoutParents(resultingCircuits);
			 children = resultingCircuits.findAllStructuresWithoutParents();
		} while(children.size() > 1);

		logDebug("<<<<<<<<<<<<FINAL STRUCTURE>>>>>>>>>>>>");
		logDebug(resultingCircuits);

		computePersistence();

		getPairLibrary().flagItemsNotOnTopLevelAndWithoutPersistence();
		delete result;


    }

    void StructureRecognitionRuleGeneration::write()
    {
    	std::string directoryPath(getLocalOptions().getPathOfXmlLibraryWithoutFileName() + getLocalOptions().getStructureName());
    	mkdir(directoryPath.c_str(), ACCESSPERMS);
    	std::string itemsDirectoryPath(directoryPath + "/Items");
    	mkdir(itemsDirectoryPath.c_str(), ACCESSPERMS);

		std::vector<NewPairLibraryItem*>& libraryItems = getNewPairLibraryItems();
		for(std::vector<NewPairLibraryItem*>::const_iterator it_items = libraryItems.begin(); it_items != libraryItems.end(); it_items++)
		{
			NewPairLibraryItem * item = * it_items;
			if(!item->IsItemNotOnTopLevelAndWithoutPersistence())
			{
				std::string itemFilePath(itemsDirectoryPath + "/" + item->getName() + ".xml");
				Control::OutputFile itemFile;
				itemFile.setPath(itemFilePath);
				std::string itemRelativeFilePath = createRelativeFilePath(itemFilePath, itemsDirectoryPath);
				item->setFilePath(itemRelativeFilePath);
				item->writeXml(itemFile);
			}
		}

    	Control::OutputFile pairLibraryFile;
    	std::string pairLibraryFilePath(directoryPath + "/" + getLocalOptions().getStructureName() + "Library.xml");
    	pairLibraryFile.setPath(pairLibraryFilePath);
    	getPairLibrary().writeXml(pairLibraryFile);
    	std::string pairLibraryRelativeFilePath = createRelativeFilePath(pairLibraryFilePath, directoryPath);
    	getPairLibrary().setFilePath(pairLibraryRelativeFilePath);

    	Control::OutputFile libraryOutputFile = getLocalOptions().getLibraryOutputFile();
    	libraryOutputFile.setPath(getLocalOptions().getPathOfXmlLibraryWithoutFileName() + getLocalOptions().getStructureName() + "Library.xml");
    	getLibrary().addPairLibraryFilePath(pairLibraryRelativeFilePath);
    	getLibrary().writeXml(libraryOutputFile);
    }

    void StructureRecognitionRuleGeneration::parseLibrary()
    {
		LibraryFile libraryFile = getLocalOptions().getLibraryFile();
		libraryFile.setDeviceTypeRegister(getDeviceTypeRegister());
		Library * library = libraryFile.parse();
		library_ = library;
    }

    void StructureRecognitionRuleGeneration::initializeNewPairLibrary()
    {
    	newLibrary_ = new PairLibrary;
		getPairLibrary().setStructureCore(getLibrary().getStructureCore());
    }

    void StructureRecognitionRuleGeneration::computePersistence()
    {
    	StructureName mainName(getLocalOptions().getStructureName());
    	std::vector<NewPairLibraryItem*>& newPairLibraryItems = getNewPairLibraryItems();
    	for( std::vector<NewPairLibraryItem*>::const_iterator it_items = newPairLibraryItems.begin(); it_items != newPairLibraryItems.end(); it_items++)
    	{
    	    	NewPairLibraryItem* item = * it_items;
    	    	item->computePersistenceOfChildren( getPairLibrary(), getLibrary().getArrayLibrary(), mainName);
    	}
    }

    void StructureRecognitionRuleGeneration::deleteEqualItems(std::vector<NewPairLibraryItem*> newItems) const
    {
    	const std::vector<NewPairLibraryItem*>& newLibraryItems = getNewPairLibraryItems();
    	for(std::vector<NewPairLibraryItem*>::const_iterator it_oldItems = newLibraryItems.begin(); it_oldItems != newLibraryItems.end(); it_oldItems++)
    	{
    	    NewPairLibraryItem * oldItem = *it_oldItems;

    	    for(std::vector<NewPairLibraryItem*>::iterator it_newItems = newItems.begin(); it_newItems != newItems.end(); it_newItems++)
    	    {
    	    	NewPairLibraryItem * newItem = * it_newItems;
    	    	if(newItem->isEqualButInName(*oldItem))
    	    	{
    	    		delete newItem;
    	    		newItems.erase(it_newItems);
    	    		it_newItems--;
    	    	}
    	    }
    	}
    }

    const std::vector<NewPairLibraryItem*> & StructureRecognitionRuleGeneration::getNewPairLibraryItems() const
    {
    	return  newPairLibraryItems_;
    }

    std::vector<NewPairLibraryItem*> & StructureRecognitionRuleGeneration::getNewPairLibraryItems()
    {
    	return  newPairLibraryItems_;
    }

    const PairLibrary& StructureRecognitionRuleGeneration::getPairLibrary() const
    {
    	assert(newLibrary_ != NULL);
    	return * newLibrary_;
    }

    PairLibrary& StructureRecognitionRuleGeneration::getPairLibrary()
    {
      	assert(newLibrary_ != NULL);
       	return * newLibrary_;
    }


    const Library& StructureRecognitionRuleGeneration::getLibrary() const
    {
    	assert(library_ != NULL);
    	return * library_;
    }

    Library& StructureRecognitionRuleGeneration::getLibrary()
    {
    	assert(library_ != NULL);
    	return * library_;
    }

    std::string StructureRecognitionRuleGeneration::createRelativeFilePath(std::string pairLibraryItemPath, std::string analogLibraryFilePath)
    {
    	Control::FileSystemPath fileSystemPath;
    	fileSystemPath.setPath(pairLibraryItemPath);
    	std::string filePath = fileSystemPath.getRelativeToFilePath(analogLibraryFilePath);
    	assert(filePath != "Unsupported Path relations" , "Unsupported Path relations");
    	return filePath;
    }


}
