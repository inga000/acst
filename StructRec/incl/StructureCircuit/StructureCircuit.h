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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURECIRCUIT_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURECIRCUIT_H_

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>

namespace StructRec {

	class HierarchyLevel;
	class StructureInstanceTerminal;
	class StructureTerminal;
	class StructureCircuits;
	class StructureCore;
	class StructureNet;
	class Structure;
	class StructureCircuit : public Core::Object
	{
	public:
		StructureCircuit();
		~StructureCircuit();

		StructureCircuit & clone(StructureCircuits& circuits) const;

		void addStructureNet(StructureNet & net);
		void addStructure(Structure & structure);
		void setHierarchyLevel(const int level);
		void addTerminal(StructureTerminal & terminal);
		void addInstanceTerminal(StructureInstanceTerminal & instanceTerminal);

		int getHierarchyLevel() const;

		StructureNet& findStructureNet(Core::NetId netId) const;
		Structure& findStructure(StructureId structureId) const;
		StructureTerminal & findTerminal(Core::TerminalId terminalId) const;
		const std::vector<StructureInstanceTerminal*> findInstanceTerminals (Core::InstanceId instanceId) const;


		std::vector<Structure*> findStructures(const StructureName& name);
		std::vector<const Structure*> findStructures(const StructureName& name) const;

		const std::vector<StructureNet*> findAllNets() const;
		std::vector<Structure*> findAllStructures();
		std::vector<const Structure*> findAllStructures() const;
		std::vector<Structure*> findAllStructuresWithoutParents() const;
		const std::vector<StructureTerminal*> findAllTerminals() const;
		const std::vector<StructureInstanceTerminal*> findAllInstanceTerminals() const;

		std::vector<const Structure*> findConnectedStructures(const Core::NetId& netId) const;

		bool hasStructureNet(Core::NetId netId) const;
		bool hasStructure(StructureId structureId) const;
		bool hasTerminal(Core::TerminalId terminalId) const;

		void deleteNet(StructureNet & net);
		void deleteStructure(Structure & structure);

		bool hasTerminals()const;
		bool hasInstanceTerminals()const;
		bool hasStructures() const;

		bool empty() const;

		std::string toStr() const;
		void print(std::ostream& stream) const;
        void printSummary(std::ostream& stream) const;
        void printResult(std::ostream& stream, int column, const StructureCircuits & circuits) const;
		void writeXml(Core::XmlNode & node, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath, const StructureCircuits & circuits) const;

	private:
		static const int LEVEL_NOT_INITIALIZED_;

		int getNumStructures() const;
		int getNumNets() const;

		bool hasNets() const;

		void printNets(std::ostream& stream) const;
		void printStructures(std::ostream& stream) const;
		void printTerminals(std::ostream & stream) const;
		void printInstanceTerminals(std::ostream& stream) const;

		bool hasHierarchyLevel() const;
		bool hasInstanceTerminalsForThisInstance(Core::InstanceId instanceId) const;

		void eraseAllNets();
		void eraseAllStructures();
		void eraseAllTerminals();
		void eraseAllInstanceTerminals();

	private:
		int hierarchyLevel_;

		std::map<Core::NetId,StructureNet*> netMap_;
		std::map<StructureId, Structure*> structureMap_;
		std::map<Core::TerminalId, StructureTerminal*> terminalMap_;
		std::map<Core::InstanceId, std::vector<StructureInstanceTerminal*> > instanceToInstanceTerminalsMap_;
	};

}



#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURECIRCUIT_H_ */
