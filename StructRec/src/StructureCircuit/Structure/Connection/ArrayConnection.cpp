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

#include "StructRec/incl/StructureCircuit/Structure/Connection/ArrayConnection.h"
#include "StructRec/incl/StructureCircuit/Net/ArrayNet.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/ArrayPin.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	ArrayConnection::ArrayConnection()
	{

	}

	ArrayConnection::ArrayConnection(const ArrayConnection& other):
		arrayPinToDevicePinConnection_(other.arrayPinToDevicePinConnection_)
	{

	}

	ArrayConnection::~ArrayConnection()
	{

	}

	void ArrayConnection::addConnection(const StructurePinType arrayPin,
		const Core::PinType devicePin)
	{
		arrayPinToDevicePinConnection_.insert(std::pair<StructurePinType, Core::PinType>(arrayPin, devicePin));
	}

	Core::PinType ArrayConnection::findPinType(
		const StructurePinType arrayPin)
	{
		assert(hasPin(arrayPin));
		return arrayPinToDevicePinConnection_.at(arrayPin);
	}

	bool ArrayConnection::hasPin(const StructurePinType arrayPin)
	{
		return arrayPinToDevicePinConnection_.find(arrayPin) != arrayPinToDevicePinConnection_.end();
	}

	void ArrayConnection::connectArray(const Core::Circuit & circuit, StructureCircuit & arrayCircuit, Array& array,
			const Core::Device& device, Core::VisitedInstances & visitedInstances) const
	{
		for (std::map<StructurePinType, Core::PinType>::const_iterator it=getArrayPinToDevicePinConnection().begin();
				it != getArrayPinToDevicePinConnection().end(); it++)
		{
			StructurePinType structurePinType = it->first;
			Core::PinType pinType = it->second;

			const Core::Pin & pin = device.findPin(pinType);
			const Core::Net & net = computeCorrespondingNet(circuit, device, pin, visitedInstances);
	        StructureNet& structureNet = findOrCreateArrayNet(arrayCircuit,net);

	        ArrayPin *newPin = new ArrayPin;
	        newPin->setArrayPin(device.findPin(pinType));
	        newPin->setPinType(structurePinType);
	        newPin->setStructure(array);
	        newPin->setNet(structureNet);

	        structureNet.addPin(*newPin);
	        array.addPin(structurePinType,newPin);
		}
	}

	StructureNet& ArrayConnection::findOrCreateArrayNet(StructureCircuit & circuit, const Core::Net& net) const
    {
	    StructureNet* arrayNet = NULL;

        Core::NetId netId = net.getIdentifier();
        if(circuit.hasStructureNet(netId))
        {
            arrayNet = &circuit.findStructureNet(netId);
        } else {

            arrayNet = &createNewArrayNet(net);
            circuit.addStructureNet(*arrayNet);
        }
        return *arrayNet;
    }

	const Core::Net& ArrayConnection::computeCorrespondingNet(const Core::Circuit& circuit, const Core::Device& device,
			const Core::Pin& pin, Core::VisitedInstances& visitedInstances) const
	{
		const Core::Net* net = NULL;
		if(pin.isConnected())
		{
			 net = &pin.getNet();
		} else if(pin.hasNetAssignment())
		{
			Core::NetAssignment netAssignment = pin.getNetAssignment();
			visitedInstances.setCircuit(circuit);
			visitedInstances.push(device);
			Core::NetId netId = visitedInstances.resolveAssignment(netAssignment);
			net = &circuit.findNet(netId);
		} else {
			assert(false);
		}
		return *net;
	}

	const std::map<StructurePinType, Core::PinType>& ArrayConnection::getArrayPinToDevicePinConnection() const
	{
		return arrayPinToDevicePinConnection_;
	}

	ArrayNet & ArrayConnection::createNewArrayNet(const Core::Net & net) const
	{
		ArrayNet *newNet = new ArrayNet;
		newNet->setNetId(net.getIdentifier());
		newNet->setSupply(net.getSupply());
		newNet->setArrayNet(net);
		return *newNet;
	}

	bool ArrayConnection::isIdentical(const ArrayConnection& other) const
	{
		if(arrayPinToDevicePinConnection_.size() != other.arrayPinToDevicePinConnection_.size()) {
			return false;
		}

		bool hasSameDevicePins = true;
		for(std::map<StructurePinType, Core::PinType>::const_iterator it = arrayPinToDevicePinConnection_.begin();
				it != arrayPinToDevicePinConnection_.end(); it++)
		{
			Core::PinType currentPinType = it->second;
			bool hasSamePinType = false;
			for(std::map<StructurePinType, Core::PinType>::const_iterator it2 = other.arrayPinToDevicePinConnection_.begin();
					it2 != other.arrayPinToDevicePinConnection_.end(); it2++)
			{
				Core::PinType otherPinType = it2->second;
				if(currentPinType == otherPinType) {
					hasSamePinType = true;
					break;
				}
			}
			if(!hasSamePinType) {
				hasSameDevicePins = false;
				break;
			}
		}
		return hasSameDevicePins;
	}

	LibraryItemMapping ArrayConnection::createMapping(const ArrayConnection& other) const
	{
		assert(isIdentical(other));
		LibraryItemMapping newMapping;
		for(std::map<StructurePinType, Core::PinType>::const_iterator it = arrayPinToDevicePinConnection_.begin();
				it != arrayPinToDevicePinConnection_.end(); it++)
		{
			Core::PinType currentPinType = it->second;
			for(std::map<StructurePinType, Core::PinType>::const_iterator it2 = other.arrayPinToDevicePinConnection_.begin();
					it2 != other.arrayPinToDevicePinConnection_.end(); it2++)
			{
				Core::PinType otherPinType = it2->second;
				if(currentPinType == otherPinType) {
					StructurePinType originalPinType = it->first;
					StructurePinType otherPinType = it2->first;
					newMapping.addPinMapping(otherPinType, originalPinType);
					break;
				}
			}
		}
		return newMapping;
	}

	std::vector<StructurePinType> ArrayConnection::getStructurePinTypes() const
	{
		std::vector<StructurePinType> pinTypeItems;
		for(auto& it : getArrayPinToDevicePinConnection())
		{
			pinTypeItems.push_back(it.first);
		}
		return pinTypeItems;
	}

}


