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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_INSTANCE_STRUCTUREINSTANCE_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_INSTANCE_STRUCTUREINSTANCE_H_

#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Common/Object.h"

namespace StructRec {

	class StructureCircuits;
	class StructureTerminal;
	class StructureInstanceTerminal;
	class StructureInstance : public Core::Object
	{
	private:
		typedef std::map<Core::TerminalId, StructureInstanceTerminal*> InstanceTerminalMap;
		typedef std::map<int, InstanceTerminalMap> HierarchicalInstanceTerminalMap;
	public:
		StructureInstance();

		void setSuperiorCircuits(StructureCircuits & superiorCircuits);
		void setSubCircuits(StructureCircuits & subCircuits);
		void setInstanceId(Core::InstanceId instanceId);

		void addInstanceTerminal(int level, StructureInstanceTerminal& instanceTerminal);

		StructureCircuits& getSubCircuits();
		const StructureCircuits & getSubCircuits() const;

		const StructureCircuits & getSuperiorCircuits() const;

		Core::InstanceId getId() const;
		Core::InstanceName getName() const;

		bool hasInstanceTerminals(int level) const;
		std::vector<const StructureInstanceTerminal*> findInstanceTerminals(int level) const;
		StructureTerminal & getTerminalOfHighestInitializedCircuit(Core::TerminalId terminalId);
		bool hasTerminalInHighestInitializedCircuit(Core::TerminalId terminalId) const;

		std::string toStr() const;

	private:
		bool hasSuperiorCircuits() const;
		bool hasSubCircuits() const;
		bool hasInstanceId() const;

	private:
		StructureCircuits *superiorCircuits_;
		StructureCircuits *subCircuits_;
		Core::InstanceId instanceId_;

		HierarchicalInstanceTerminalMap instanceTerminals_;
	};
}



#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_INSTANCE_STRUCTUREINSTANCE_H_ */
