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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_PAIRCONNECTION_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_PAIRCONNECTION_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/Structure/Pair.h"
#include "StructRec/incl/StructureCircuit/Net/PairNet.h"
#include "StructRec/incl/RuleGeneration/NewRecognizedChild.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMappings.h"
#include <map>

namespace StructRec {

	class CrossInstancePair;
	class PairConnection
	{
	public:
		PairConnection();
		PairConnection(const PairConnection& other);
		~PairConnection();

		void initialize(StructureName newPair, const NewRecognizedChild & child1, const NewRecognizedChild & child2);

		void addConnection (const StructurePinType& topPin, const StructurePinType& childPin, const int child);

		void connectPair(StructureCircuit& circuit, Pair& pair) const;
		void connectCrossInstancePair(StructureCircuit& circuit, Pair& pair, CrossInstancePair& tempPair) const;

		StructureName getFirstChildName() const;
		StructureName getSecondChildName() const;

		std::vector<StructurePinType> getStructurePins() const;

		bool isIdentical(const PairConnection& other, const LibraryItemMappings& mappings) const;
		LibraryItemMapping creatNewMapping(const PairConnection& other, const LibraryItemMappings& mappings) const;
		void update(const LibraryItemMappings& mappings);

		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc) const;

	private:
		const std::map<StructurePinType, StructurePinType>& getTopPinToChild1PinConnection() const;
		const std::map<StructurePinType, StructurePinType>& getTopPinToChild2PinConnection() const;

		bool hasIdenticalChildPinTypes(const std::map<StructurePinType, StructurePinType>& original, const std::map<StructurePinType, StructurePinType>& other,
				const LibraryItemMappings& mappings) const;
		void createPinMapping(const std::map<StructurePinType, StructurePinType>& original, const std::map<StructurePinType, StructurePinType>& other,
				const LibraryItemMappings& mappings, LibraryItemMapping& newMapping) const;

		bool hasChild1Pin(const StructurePinType& topPin) const;
		bool hasChild2Pin(const StructurePinType& topPin) const;

		StructurePinType findChild1PinType(const StructurePinType& topPin) const;
		StructurePinType findChild2PinType(const StructurePinType& topPin) const;

		StructureNet& findOrCreatePairNet(StructureCircuit& circuit, const StructureNet& childNet) const;
		PairNet & createNewPairNet(const StructureNet & net) const;

		void connectPairToChild1(StructureCircuit & circuit, Pair & pair) const;
		void connectPairToChild2(StructureCircuit & circuit, Pair & pair) const;

		void connectPairToCrossInstanceChild1(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const;
		void connectPairToCrossInstanceChild2(StructureCircuit & circuit, Pair & pair, CrossInstancePair & tempPair) const;

		Core::NetId createNewNetId(Structure & structure, Core::NetId childNet, Core::InstanceNamePath childNamePath) const;
		PairNet & createNewEmptyNet(Structure & structure, const StructureNet & childNet, Core::InstanceNamePath instanceNamePath) const;

		std::string createPinName(std::string netName) const;

		bool isInitialized() const;
	private:
		std::map<StructurePinType, StructurePinType>   topPinToChild1PinConnection_;
		std::map<StructurePinType, StructurePinType>   topPinToChild2PinConnection_;
	};

}

#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_PAIRCONNECTION_H_ */
