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

#ifndef STRUCTREC_INCL_LIBRARY_ARRAYLIBRARY_H_
#define STRUCTREC_INCL_LIBRARY_ARRAYLIBRARY_H_

#include <vector>
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMapping.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include "StructRec/incl/Library/LibraryItem/LibraryItem.h"
#include "StructRec/incl/Results/Result.h"

namespace Core  {

	class Circuit;
	class Instance;
	class VisitedInstances;
}

namespace StructRec {

	class StructureCore;
	class ArrayLibraryItem;
	class StructureCircuit;
	class StructureCircuits;
	class CircuitRegister;
	class ArrayLibrary
	{
	private:
		//store items in map format? => faster lookup
		typedef std::vector<ArrayLibraryItem*> ArrayLibraryItemVector;
	public:
		ArrayLibrary();
		~ArrayLibrary();

		void setFilePath(const std::string& filePath);
		void setStructureCore(StructureCore& structureCore);

		void addItem(ArrayLibraryItem & item);
		void addAndMerge(const ArrayLibrary& other, LibraryItemMappings& mappings);

		Result* recognize(const Core::Circuit& circuit) const;

		//required by rule generation, possible to make private?
		ArrayLibraryItem & findArrayLibraryItem(const StructureName& structureName) const;

		//information for user interface
		std::string getFilePath() const;
		std::string getImageFilePath(const StructureName& structureName) const;
		std::vector<StructureName> getStructureNames() const;
		std::vector<StructurePinType> getStructurePinTypes(const StructureName& structureName) const;

		bool hasArrayLibraryItem(const StructureName& structureName) const;

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;
	private:
		StructureCircuits & recognizeArraysRecursively(const Core::Circuit & circuit,
				CircuitRegister & circuitRegister, Core::VisitedInstances & visitedInstances) const;

		void recognizeArraysInInstances(const Core::Circuit & circuit, CircuitRegister & circuitRegister,
				StructureCircuits & recognizedCircuits, Core::VisitedInstances & visitedInstances) const;

		void completeCircuit(const Core::Circuit & circuit, StructureCircuit & recognizedCircuit, StructureCircuits & circuits) const;
		void addInstanceTerminals(const Core::Circuit & circuit, StructureCircuit & recognizedCircuit, StructureCircuits & circuits) const;
		void addTerminals(const Core::Circuit & circuit, StructureCircuit & recognizedCircuit) const;

		//for merging array libraries
		bool hasIdenticalArrayLibraryItem(const ArrayLibraryItem& otherItem) const;
		LibraryItemMapping createMapping(const ArrayLibraryItem& otherItem) const;
		//

		void eraseAllLibraryItems();

		int getLevel() const;

	private:
		const StructureCore& getStructureCore() const;
		StructureCore& getStructureCore();

		const ArrayLibraryItemVector& getArrayLibraryItems() const;
		ArrayLibraryItemVector& getArrayLibraryItems();

	private:
		StructureCore* structureCore_;
		ArrayLibraryItemVector arrayLibraryItems_;

		std::string filePath_;
	};

}


#endif /* STRUCTREC_INCL_LIBRARY_ARRAYLIBRARY_H_ */
