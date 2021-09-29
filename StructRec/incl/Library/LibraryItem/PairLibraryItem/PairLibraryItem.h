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

#ifndef STRUCTREC_INCL_LIBRARY_PAIRITEM_H_
#define STRUCTREC_INCL_LIBRARY_PAIRITEM_H_

#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/Results/CircuitRegister.h"
#include "StructRec/incl/StructureCore.h"
#include "StructRec/incl/Library/LibraryItem/LibraryItem.h"
#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/PairRecognitionRules/PairRecognitionRule.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"

namespace StructRec {

	class CrossInstancePair;
	class CharacteristicConnection;
	class PairConnection;
	class PairLibraryItem : public LibraryItem
	{
	public:
		PairLibraryItem();
		PairLibraryItem(const PairLibraryItem& other);
		~PairLibraryItem();

		void setStructureCore(const StructureCore & core);
		void setCharacteristicConnection(CharacteristicConnection& characteristicConnection);
		void setPairConnection(PairConnection& pairConnection);
		void setSymmetric(bool isSymmetric);
		void setHelperStructure(bool isHelperStructure);
		void addRecognitionRule(PairRecognitionRule & recognitionRule);
		void setItemNotOnTopLevelAndWithoutPersistence(bool value);

		void recognize(StructureCircuits & circuits, StructureCircuit & levelCircuit);
		void recognizeOnStructuresWithoutParents(StructureCircuits & circuits, StructureCircuit & levelCircuit);

		std::vector<StructurePinType> getStructurePinTypes() const;

		PairConnection & getPairConnection();
		const PairConnection & getPairConnection() const;

		bool checkRecognitionRules(Pair & possiblePair) const;

		bool IsItemNotOnTopLevelAndWithoutPersistence() const;

		bool isIdentical(const PairLibraryItem& other, const LibraryItemMappings& mappings) const;
		LibraryItemMapping creatNewMapping(const PairLibraryItem& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

		void writeXml(Control::OutputFile& file) const;

	protected:
		const StructureCore & getStructureCore() const;
		std::vector<PairRecognitionRule*> & getRecognitionRules();
		const std::vector<PairRecognitionRule*> & getRecognitionRules() const;
		CharacteristicConnection & getCharacteristicConnection();
		const CharacteristicConnection & getCharacteristicConnection() const;
		bool getIsSymmetric() const;

		bool isHelperStructure() const;

	private:
		void addToCircuit(StructureCircuit & circuit, Pair & pair, int idCounter) const;
		Pair & createNewPair(CrossInstancePair & tempPair, int idCounter, StructureCircuits & circuits) const;
		void addPairToCircuit(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const;
		Structure & createNewInInstanceChild(const CrossInstanceStructure & crossInstanceChild, StructureCircuits & circuits) const;

		const std::vector<CrossInstancePair*> findPossibleCrossInstancePairs(StructureCircuits & circuits) const;
		const std::vector<CrossInstanceStructure*> findPossibleCrossInstanceChildren(StructureNet & currentNet,
				StructureCircuit& currentCircuit, StructureCircuits & circuits, const std::vector<StructurePinType>& characteristicConnectionPins) const;

		bool checkRecognitionRules(CrossInstancePair & possiblePair) const;

		void writeXmlStructureSymmetry(Core::XmlNode & node, Core::XmlDocument & doc) const;
		void writeXmlRecognitionRules(Core::XmlNode & node, Core::XmlDocument & doc) const;
	private:
		void eraseAllRecognitionRules();
		void deleteAllCrossInstanceStructures(const std::vector<CrossInstancePair*> & crossInstancePair) const;
		void copyRecognitionRules(const PairLibraryItem& other);

	private:
        bool isSymmetric_;
        bool isHelperStructure_;
		bool itemNotOnTopLevelAndWithoutPersistence_;

		PairConnection* pairConnection_;
		CharacteristicConnection* characteristicConnection_;
		std::vector<PairRecognitionRule*> recognitionRules_;

		const StructureCore * structureCore_;
	};


}
#endif /* STRUCTREC_INCL_LIBRARY_PAIRITEM_H_ */
