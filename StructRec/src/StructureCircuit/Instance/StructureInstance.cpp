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

#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	StructureInstance::StructureInstance() :
			superiorCircuits_(nullptr),
			subCircuits_(nullptr)

	{
	}

	void StructureInstance::setSuperiorCircuits(
		StructureCircuits& superiorCircuits)
	{
		superiorCircuits_ = &superiorCircuits;
	}

	void StructureInstance::setSubCircuits(
		StructureCircuits& subCircuits)
	{
		subCircuits_ = & subCircuits;
	}

	void StructureInstance::setInstanceId(Core::InstanceId instanceId)
	{
		instanceId_ = instanceId;
	}

	void StructureInstance::addInstanceTerminal(int level, StructureInstanceTerminal& instanceTerminal)
	{
		InstanceTerminalMap& levelMap = instanceTerminals_[level];
		levelMap[instanceTerminal.getTerminalId()] = &instanceTerminal;
	}

	const StructureCircuits& StructureInstance::getSubCircuits() const
	{
		assert(hasSubCircuits());
		return * subCircuits_;
	}

	StructureCircuits& StructureInstance::getSubCircuits()
	{
		assert(hasSubCircuits());
		return * subCircuits_;
	}

	Core::InstanceId StructureInstance::getId() const
	{
		assert(hasInstanceId());
		return instanceId_;
	}

	Core::InstanceName StructureInstance::getName() const
	{
		return getId().getInstanceName();
	}

	const StructureCircuits& StructureInstance::getSuperiorCircuits() const
	{
		assert(hasSuperiorCircuits());
		return * superiorCircuits_;
	}

	std::string StructureInstance::toStr() const
	{
		return getId().toStr();
	}

	bool StructureInstance::hasSuperiorCircuits() const
	{
		return superiorCircuits_ != nullptr;
	}

	bool StructureInstance::hasSubCircuits() const
	{
		return subCircuits_ != nullptr;
	}

	bool StructureInstance::hasInstanceId() const
	{
		return instanceId_.isInitialized();
	}

	bool StructureInstance::hasTerminalInHighestInitializedCircuit(Core::TerminalId terminalId) const
	{
		return getSubCircuits().findHighestInitializedCircuit().hasTerminal(terminalId);
	}

	StructureTerminal & StructureInstance::getTerminalOfHighestInitializedCircuit(Core::TerminalId terminalId)
	{
		assert(hasTerminalInHighestInitializedCircuit(terminalId));
		return getSubCircuits().findHighestInitializedCircuit().findTerminal(terminalId);
	}

	bool StructureInstance::hasInstanceTerminals(int level) const
	{
		return instanceTerminals_.find(level) != instanceTerminals_.end();
	}

	std::vector<const StructureInstanceTerminal*> StructureInstance::findInstanceTerminals(int level) const
	{
		std::ostringstream msg;
		msg << "No instanceTerminals on level " << level << " for instance: " << getId() << ", " << getSubCircuits().getCircuitId();

		assert(hasInstanceTerminals(level), msg.str());
		std::vector<const StructureInstanceTerminal*> instanceTerminals;
		InstanceTerminalMap levelMap = instanceTerminals_.find(level)->second;
		for(auto it : levelMap)
		{
			instanceTerminals.push_back(it.second);
		}
		return instanceTerminals;
	}

}

