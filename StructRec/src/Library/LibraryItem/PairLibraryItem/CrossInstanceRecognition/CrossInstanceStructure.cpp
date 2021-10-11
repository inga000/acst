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

#include "StructRec/incl/Library/LibraryItem/PairLibraryItem/CrossInstanceRecognition/CrossInstanceStructure.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstanceTerminal.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Terminal/StructureTerminal.h"
#include "StructRec/incl/Results/StructureCircuits.h"

#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	CrossInstanceStructure::CrossInstanceStructure() :
			structure_(nullptr)
	{
	}

	CrossInstanceStructure::~CrossInstanceStructure()
	{
	}

	void CrossInstanceStructure::setStructure(Structure& structure)
	{
		structure_ = & structure;
	}

	void CrossInstanceStructure::addConnection(const StructurePinType& pinType,
			const StructureInstanceTerminal& connection)
	{
		crossInstanceConnectionsMap_.insert(ConnectivityMap::value_type(pinType, &connection));
	}

	void CrossInstanceStructure::setInstanceNamePath(const Core::InstanceNamePath& instanceNamePath)
	{
		instanceNamePath_ = instanceNamePath;
	}

	const StructureInstanceTerminal& CrossInstanceStructure::findConnectedInstanceTerminal(const StructurePinType& pinType) const
	{
		assert(hasCrossInstanceConnection(pinType));
		return *crossInstanceConnectionsMap_.find(pinType)->second;
	}

	bool CrossInstanceStructure::hasCrossInstanceConnection(const StructurePinType& pinType) const
	{
		return crossInstanceConnectionsMap_.find(pinType) != crossInstanceConnectionsMap_.end();
	}

	Structure& CrossInstanceStructure::getStructure() const
	{
		assert(structure_ != NULL);
		return *structure_;
	}

	bool CrossInstanceStructure::hasTopCircuitConnection(const StructurePinType& pinType) const
	{
		return isInTopCircuit() ? getStructure().isConnected(pinType) : hasCrossInstanceConnection(pinType);
	}

	Core::InstanceNamePath CrossInstanceStructure::getInstanceNamePath() const
	{
		assert(instanceNamePath_.isInitialized());
		return instanceNamePath_;
	}

	bool CrossInstanceStructure::isInTopCircuit() const
	{
		return instanceNamePath_.isRoot();
	}

	const StructureNet& CrossInstanceStructure::findTopInstanceNet(const StructurePinType & pinType) const
	{
		return isInTopCircuit() ? getStructure().findNet(pinType) : findConnectedInstanceTerminal(pinType).getNet();
	}

	StructureCircuits& CrossInstanceStructure::findStructureInstance(StructureCircuits & topCircuits) const
	{
		StructureCircuits *toSearchInCircuits = &topCircuits;
		const std::vector<Core::InstanceName>  instanceNamePath = getInstanceNamePath().getPathVector();
		for(std::vector<Core::InstanceName>::const_iterator it = instanceNamePath.begin(); it != instanceNamePath.end(); it++)
		{
			Core::InstanceId id;
			id.setInstanceName(*it);

			StructureInstance& instanceInTopCircuit = toSearchInCircuits->findInstance(id);
			toSearchInCircuits = &instanceInTopCircuit.getSubCircuits();
		}

		return * toSearchInCircuits;
	}

	 bool CrossInstanceStructure::operator==(const CrossInstanceStructure & other) const
	{
		 return (getStructure().getIdentifier() == other.getStructure().getIdentifier()) && (getInstanceNamePath() == other.getInstanceNamePath());
	}

	 bool CrossInstanceStructure::operator!=(const CrossInstanceStructure & other) const
	{
		 return !(*this == other);
	}

	 bool CrossInstanceStructure::operator<(const CrossInstanceStructure & other) const
	 {
		 return (getStructure().getIdentifier() < other.getStructure().getIdentifier()) || (getInstanceNamePath() < other.getInstanceNamePath());
	 }

	 std::string CrossInstanceStructure::toStr() const
	 {
		 std::ostringstream stream;
		 stream << "Structure: " << getStructure() << std::endl;
		 if(!crossInstanceConnectionsMap_.empty())
		 {
			 for(ConnectivityMap::const_iterator it = crossInstanceConnectionsMap_.begin(); it != crossInstanceConnectionsMap_.end(); it++)
			 {
				 StructurePinType   pinType = it->first;
				 stream << "Pin: " << pinType;
				 stream << " InstanceTerminal: " << it->second;
				 stream << std::endl;
			 }
		 }
		 else
		 {
			 stream << "Has no Connection. Must be in Top Circuit";
		 }
		 return stream.str();
	 }
}


