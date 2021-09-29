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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_NET_STRUCTURENET_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_NET_STRUCTURENET_H_

#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/Supply.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include <vector>
#include <map>

namespace StructRec {

	class StructurePin;
	class Structure;
	class StructureTerminal;
	class StructureInstanceTerminal;
	class CrossInstanceStructure;
	class StructureInstance;
	class StructureCircuits;
	class StructureCircuit;
	class ArrayNet;

	class StructureNet : public Core::Object
	{
	private:
		typedef std::vector<StructurePin*> StructurePinVector;
		typedef std::map<StructurePinType, StructurePinVector > StructurePinMap;
	public:
		StructureNet();
		~StructureNet();

		virtual StructureNet & cloneWithoutStructureConnection(StructureCircuit & circuit, StructureCircuits & circuits) const = 0;

		void addPin(StructurePin & pin);
		void setNetId(Core::NetId  netId);
		void setSupply(Core::Supply  supply);
		void setTerminal(StructureTerminal & terminal);
		void addInstanceTerminal(StructureInstanceTerminal &instanceTerminal);

		Core::NetId getIdentifier() const;
		Core::Supply getSupply() const;
		const StructureTerminal & getTerminal() const;
		const std::vector<Structure*> findConnectedStructures(const StructurePinType & pinType);
		std::vector<const Structure*> findConnectedStructures(
			const StructurePinType & pinType) const;
		const std::vector<CrossInstanceStructure*> findConnectedInstanceStructures(
					const StructurePinType & pinType, int hierarchyLevel) const;
		std::vector<const Structure*> getAllConnectedStructures() const;

		const std::vector<StructureInstanceTerminal*> getConnectedInstanceTerminals() const;
		const StructurePinVector getConnectedPins() const;

		bool hasInstanceTerminals() const;
		bool hasTerminal() const;
		bool isConnectedToTerminal() const;

		void deletePin(StructurePin & pin);

		bool operator==(const StructureNet & other) const;
		bool operator!=(const StructureNet & other) const;

		bool isSupply() const;
		bool isConnected() const;

		std::string toStr() const;
		void print(std::ostream & stream) const;

		virtual const StructureNet & findArrayNet() const = 0;

	protected:

		void createAndConnectTerminal(StructureNet & newNet, StructureCircuit & circuit) const;
		void createAndConnectInstanceTerminals(StructureNet & newNet, StructureCircuits & circuits, StructureCircuit & circuit) const;

	private:
		bool hasNetId() const;
		bool hasSupply() const;

		std::vector<CrossInstanceStructure*> findDirectlyConnectedStructures(const StructurePinType & pinType,
				Core::InstanceNamePath instanceNamePath) const;

		const std::vector<CrossInstanceStructure*> findConnectedInstanceStructuresInInstance(const StructurePinType & pinType,
				const int hierarchyLevel, std::vector<StructureInstance*> visitedInstances, Core::InstanceNamePath instanceNamePath) const;
		void connectInstanceStructureToTopCircuit(CrossInstanceStructure & instStructure,
				std::vector<StructureInstance*> visitedInstances, int hierarchyLevel) const;

		std::vector<StructurePin*> findPins(const StructurePinType & pinType) const;
		bool hasPins(StructurePinType pinType) const;

		void disconnectAllPins();

		StructurePinMap connectedPins_;
		Core::NetId netId_;
		Core::Supply supply_;
		std::vector<StructureInstanceTerminal*> instanceTerminals_;
		StructureTerminal * terminal_;
	};


}



#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_NET_STRUCTURENET_H_ */
