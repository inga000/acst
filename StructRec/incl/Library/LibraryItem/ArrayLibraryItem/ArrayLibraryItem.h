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

#ifndef STRUCTREC_INCL_LIBRARY_ARRAYITEM_H_
#define STRUCTREC_INCL_LIBRARY_ARRAYITEM_H_

#include "StructRec/incl/Library/LibraryItem/LibraryItem.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMapping.h"
#include "StructRec/incl/Library/LibraryItem/ArrayLibraryItem/ParallelNets/ParallelNetsMapKey.h"
#include <vector>
#include <map>

namespace Core
{
	class Device;
	class Circuit;
	class Instance;
	class VisitedInstances;
}

namespace StructRec {

	class ParallelNets;
	class StructureCore;
	class StructureCircuit;
	class ArrayConnection;
	class ArrayRecognitionRule;
	class LibraryItemMappings;
	class ArrayLibraryItem : public LibraryItem
	{
	public:
		ArrayLibraryItem();
		ArrayLibraryItem(const ArrayLibraryItem& other);
		~ArrayLibraryItem();

		void setParallelNets(const ParallelNets& parallelNets);
		void setArrayConnection(const ArrayConnection& arrayConnection);
		void addRecognitionRule(const ArrayRecognitionRule& recognitionRule);

		void recognize(const Core::Circuit & circuit,
					StructureCircuit & levelCircuit, Core::VisitedInstances & visitedInstances);

		const ArrayConnection& getArrayConnection() const;
		const ParallelNets& getParallelNets() const;
		const std::vector<const ArrayRecognitionRule*>& getRecognitionRules() const;

		bool isIdentical(const ArrayLibraryItem& other) const;
		LibraryItemMapping createMapping(const ArrayLibraryItem& other) const;

		std::vector<StructurePinType> getStructurePinTypes() const;
		bool isInitialized() const;
	private:

		bool checkRecognitionRules(const Core::Device& device) const;

	private:
		bool compareRecognitionRules(const ArrayLibraryItem& other) const;

		bool hasRecognitionRules() const;
		bool hasParallelNets() const;
		bool hasArrayConnection() const;

		void copyRecognitionRules(const ArrayLibraryItem& other);
		void eraseRecognitionRules();

	private:
		std::vector<const ArrayRecognitionRule*> recoginitionsRules_;
		const ParallelNets* parallelNets_;
		const ArrayConnection* arrayConnection_;
	};
}

#endif /* STRUCTREC_INCL_LIBRARY_ARRAYITEM_H_ */
