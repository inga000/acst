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
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "StructRec/incl/StructureCircuit/Instance/StructureInstance.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/StructurePin.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include "StructRec/incl/Results/StructureCircuits.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/ArrayPin.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PairPin.h"
#include "StructRec/incl/StructureCore.h"


#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

const int Structure::PERSISTENCE_NOT_INITIALIZED_ = 0;
const int Structure::MAX_PERSISTENCE_ = -1;

	Structure::Structure() :
		persistence_(PERSISTENCE_NOT_INITIALIZED_)
	{
	}

	Structure::~Structure()
	{
		pins_.deleteAllPins();
	}

    void Structure::setStructureId(const StructureId& structureId)
    {
        structureId_ = structureId;
    }

	void Structure::setTechType(const Core::TechType& techType)
	{
		techType_ = techType;
	}

	void Structure::addPin(const StructurePinType& pinType, StructurePin * pin)
	{
		pins_.add(pinType,pin);
	}

	StructureId Structure::getIdentifier() const
	{
		assert(hasStructureId());
	    return structureId_;
	}

    StructureName Structure::getStructureName()
    {
		assert(hasStructureId());
	    return structureId_.getName();
    }

    StructureName Structure::getParentStructureName() const
    {
    	assert(hasParent());
    	return parents_[0]->getStructureName();
    }

    const StructureName Structure::getStructureName() const
    {
    	assert(hasStructureId());
        return structureId_.getName();
    }
	Core::TechType Structure::getTechType() const
	{
		assert(hasTechType());
		return techType_;
	}

	const StructurePin& Structure::findPin(const StructurePinType& pinType) const
	{
		assert(hasPin(pinType));
		const StructurePin & pin = pins_.findPin(pinType);
		return pin;
	}

	StructurePin& Structure::findPin(const StructurePinType& pinType)
	{
		assert(hasPin(pinType));
		StructurePin & pin = pins_.findPin(pinType);
		return pin;
	}

	void Structure::findDevicePin(const StructurePinType& pinType, Core::DeviceId& deviceId, Core::TerminalId& terminalId) const
	{
		StructurePin & pin = pins_.findPin(pinType);
		findDevicePin(pin, deviceId, terminalId);
	}

	void Structure::findDevicePin(const StructurePin& pin, Core::DeviceId& deviceId, Core::TerminalId& terminalId) const
	{
		if(pin.isArrayPin()){
			const ArrayPin *arrayPin = static_cast<const ArrayPin*>(&pin);
			deviceId = arrayPin->getPin()->getDevice().getIdentifier();
			terminalId = Core::TerminalName(arrayPin->getPinType().getPinName().toStr()).createIdentifier();
		}
		else if(pin.isPairPin()){
			const PairPin *pairPin = static_cast<const PairPin*>(&pin);
			const StructurePin& childPin = pairPin->getChildPin();
			findDevicePin(childPin, deviceId, terminalId);
		}
	}

	const Core::Pin& Structure::findDevicePin(const StructurePinType& pinType) const
	{
		StructurePin & pin = pins_.findPin(pinType);
		const Core::Pin* corePin = nullptr;
		findDevicePin(pin, corePin);
		return *corePin;
	}

	void Structure::findDevicePin(const StructurePin& pin, const Core::Pin*& corePin) const
	{
		if(pin.isArrayPin()){
			const ArrayPin *arrayPin = static_cast<const ArrayPin*>(&pin);
			corePin = arrayPin->getPin();
		}
		else if(pin.isPairPin()){
			const PairPin *pairPin = static_cast<const PairPin*>(&pin);
			const StructurePin& childPin = pairPin->getChildPin();
			findDevicePin(childPin, corePin);
		}
	}

	const StructureNet& Structure::findNet(const StructurePinType& pinType) const
	{
		assert(hasPin(pinType));
		const StructurePin & pin = findPin(pinType);
		assert(pin.isConnected());
		return pin.getNet();
	}

	StructureNet& Structure::findNet(const StructurePinType& pinType)
	{
		assert(hasPin(pinType));
		StructurePin & pin = findPin(pinType);
		assert(pin.isConnected());
		return pin.getNet();
	}

	bool Structure::hasPin(const StructurePinType& pinType) const
	{
		return pins_.hasPin(pinType);
	}

	std::string Structure::toStr() const
	{
		std::ostringstream stream;
		stream << "Structure: " << getIdentifier() << getTechType() << std::endl;
		pins_.print(stream);
		stream << "Parent: ";
		if(hasParent())
		{
			for(std::vector<Structure*>::const_iterator it = parents_.begin(); it != parents_.end(); it++)
			{
				Structure * structurePointer = *it;
				stream << structurePointer->getIdentifier();
			}
		}
		else
		{
			stream << "non";
		}
		stream << std::endl;
		return stream.str();
	}

	bool Structure::operator ==(const Structure& other) const
	{
		return structureId_ == other.structureId_;
	}

	bool Structure::operator !=(const Structure& other) const
	{
		return !(*this == other);
	}

	bool Structure::operator <(const Structure& other) const
	{
		return structureId_ < other.structureId_;
	}

	bool Structure::hasStructureId() const
	{
		return structureId_.isInitialized();
	}

	void Structure::print(std::ostream& stream, int column) const
	{
		stream << spaces(column);
		stream << getIdentifier() << " (" << getTechType() << ") ";
		pins_.print(stream);
		stream << std::endl;
		column++;
		printChildren(stream, column);
	}

	void Structure::writeXml(Core::XmlNode & node, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const
	{
		Core::XmlNode & structure = Core::RapidXmlUtils::addNode(node, doc, "structure");
		Core::RapidXmlUtils::addAttr(structure, doc, "name", getIdentifier().toStr());
		Core::RapidXmlUtils::addAttr(structure, doc, "techType", getTechType().toStr());
		Core::RapidXmlUtils::addAttr(structure, doc, "instance", instanceNamePath.toStr());
		Core::XmlNode & pins = Core::RapidXmlUtils::addNode(structure, doc, "pins");
		pins_.writeXml(pins, doc);
		writeXmlOfChildren(structure, doc, instanceNamePath);
	}

	bool Structure::hasTechType() const
	{
		return techType_.isInitialized();
	}

	void Structure::setPersistence(int persistence)
	{
	    assert(persistence >= -2);
		persistence_ = persistence;
	}

    bool Structure::isConnected(const StructurePinType& pinType) const
    {
    	return findPin(pinType).isConnected();
    }

    bool Structure::isConnected(const Core::NetId& netId) const
    {
    	std::vector<StructurePin*> pins = findAllPins();
    	for(auto& it : pins)
    	{
    		if(it->getNet().getIdentifier() == netId)
    		{
    			return true;
    		}
    	}
    	return false;
    }

    bool Structure::hasDeviceConnectedTo(const StructurePinType& pin1, const StructurePinType& pin2) const
    {
    	Core::NetId netId1 = findPin(pin1).getNet().getIdentifier();
    	Core::NetId netId2 = findPin(pin2).getNet().getIdentifier();

    	std::vector<const Core::Device*> devices = getDevices();
    	for(auto& it : devices)
    	{
    		const Core::Device& device = *it;
    		if(device.isConnected(netId1) && device.isConnected(netId2)) {
    			return true;
    		}
    	}
    	return false;
    }

    const Core::Device& Structure::findDeviceConnectedTo(const StructurePinType& pin1, const StructurePinType& pin2) const
    {
    	assert(hasDeviceConnectedTo(pin1, pin2));
    	Core::NetId netId1 = findPin(pin1).getNet().getIdentifier();
    	Core::NetId netId2 = findPin(pin2).getNet().getIdentifier();

    	const Core::Device* device = nullptr;
    	std::vector<const Core::Device*> devices = getDevices();
    	for(auto& it : devices)
    	{
    		const Core::Device& currentDevice = *it;
    		if(currentDevice.isConnected(netId1) && currentDevice.isConnected(netId2)) {
    			device = & currentDevice;
    			break;
    		}
    	}
    	return *device;
    }

	bool Structure::hasMaxPersistence() const
	{
		return persistence_ == MAX_PERSISTENCE_;
	}

	bool Structure::hasPersistence() const
	{
		return persistence_ != PERSISTENCE_NOT_INITIALIZED_;
	}

	int Structure::getPersistence() const
	{
		assert(hasPersistence());
		return persistence_;
	}

	void Structure::setParent(Structure  & structure)
	{

		parents_.push_back(&structure);
	}

	bool Structure::hasParent() const
	{
		return !(parents_.empty());
	}

    bool Structure::hasDirectParent(const StructureId& parentId) const
    {
    	assert(hasParent());
    	bool parent = false;
    	for(std::vector<Structure*>::const_iterator it = parents_.begin(); it != parents_.end(); it++)
    	{
    		const Structure* currentParent = *it;
    		if(currentParent->getIdentifier() == parentId)
    		{
    			parent = true;
    			break;
    		}
    	}
    	return parent;
    }

    bool Structure::hasDirectParent(const StructureName& structureName) const
    {
    	bool parent = false;
    	for(std::vector<Structure*>::const_iterator it = parents_.begin(); it != parents_.end(); it++)
    	{
    		const Structure* currentParent = *it;
    		if(currentParent->getIdentifier().getName() == structureName)
    		{
    			parent = true;
    			break;
    		}
    	}
    	return parent;
    }

    bool Structure::hasCommonParent(const Structure& other) const
    {
    	bool hasIt = false;
		const std::vector<Structure*> parents = getParents();
		const std::vector<Structure*> otherParents = other.getParents();
		for(std::vector<Structure*>::const_iterator it = parents.begin(); it != parents.end(); it++)
		{
			const Structure* parent = *it;
			for(std::vector<Structure*>::const_iterator it = otherParents.begin(); it != otherParents.end(); it++)
			{
				const Structure* otherParent = *it;
				if(parent->getIdentifier() == otherParent->getIdentifier() && parent->hasCommonDevices(*otherParent)
						&& parent->getTechType() == otherParent->getTechType())
				{
					hasIt = true;
					break;
				}
			}
		}
		return hasIt;
    }

    bool Structure::hasDirectParent(const std::vector<StructRec::StructureName>& structureNames) const
    {
    	bool parent = false;
    	for(auto& it : structureNames)
    	{
    		parent = hasDirectParent(it);
    		if(parent) {
    			break;
    		}
    	}
    	return parent;
    }

    bool Structure::hasParentInSameStructureCircuits(const StructureCircuits & structureCircuits) const
    {
    	//Whole parent is in the same instance and parts are not in any other (Instance)circuit
    	for(auto & parent : getParents())
    	{
    		StructureId parentId = parent->getIdentifier();

    		if(structureCircuits.hasStructureCircuit(structureCircuits.getStructureCore().findLevel(parentId.getName())))
    		{
    			const StructureCircuit & parentCircuit = structureCircuits.findStructureCircuit(structureCircuits.getStructureCore().findLevel(parentId.getName()));
    			return parentCircuit.hasStructure(parentId);
    		}
    	}
    	return false;
    }

    bool Structure::hasExactlyOneParent() const
    {
    	return parents_.size() == 1;
    }

    bool Structure::hasMoreThanOneParent() const
    {
    	return parents_.size() > 1;
    }

	const std::vector<Structure*> Structure::getParents() const
	{
		return parents_;
	}

    const Structure& Structure::getParent() const
    {
    	std::ostringstream oss;
    	oss << "Structure " << getIdentifier() << ", " << getDevices() << " has more than one parent!";
    	assert(hasExactlyOneParent(), oss.str());
    	return *parents_.front();
    }

	bool Structure::hasPins() const
	{
		return(!pins_.isInitialized());
	}

	std::string Structure::spaces(int columns)
	{
		std::string ret = "";
	    for (int i = 1; i <= columns; i++)
	    	ret += " ";
	   return ret;
	}

	void Structure::deleteParents()
	{
		while (parents_.begin() != parents_.end())
		{
			std::vector<Structure*>::iterator it = parents_.begin();
			parents_.erase(it);
		}
	}

	std::vector<StructurePin*> Structure::findAllPins() const
	{
		return pins_.findAllPins();
	}

	void Structure::disConnect(StructureCircuits & circuits)
	{
		int level = circuits.getStructureCore().findLevel(getStructureName());
		StructureCircuit & circuit = circuits.findStructureCircuit(level);
		std::vector<StructurePin*> pins = findAllPins();
		for(std::vector<StructurePin*>::iterator it = pins.begin(); it != pins.end(); it++)
		{
			StructurePin * pinPointer = *it;
			StructureNet & net = pinPointer->getNet();
			net.deletePin(*pinPointer);
			if(!net.isConnected())
			{
				circuit.deleteNet(net);
			}
		}
		removeThisAsParentOfChildren();

		Core::InstanceNamePath namePath;
		namePath.setIsRoot();
		removeThisAsParentOfInInstanceChildren(circuits, namePath);

		circuit.deleteStructure(*this);
	}

	void Structure::removeThisAsParentOfInInstanceChildren(StructureCircuits & circuits, Core::InstanceNamePath instanceNamePath)
	{
		for(auto & it : circuits.getInstanceMap())
		{
			StructureInstance * instance = it.second;
			instanceNamePath.append(instance->getName());
			StructureCircuits & inInstanceCircuits = instance->getSubCircuits();
			removeThisAsParentOfStructuresInTheseCircuits(inInstanceCircuits, instanceNamePath);

			removeThisAsParentOfInInstanceChildren(inInstanceCircuits, instanceNamePath);
			instanceNamePath.pop();

		}
	}

	void Structure::removeParent(const StructureId& parentId)
	{

		for(std::vector<Structure*>::iterator it = parents_.begin(); it != parents_.end(); it++)
		{
			Structure * parent = *it;

			if(parent->getIdentifier() == parentId)
			{
				parents_.erase(it);
				break;
			}
		}

	}

	void Structure::copyBasics(Structure & other) const
	{
		other.setTechType(getTechType());
		other.setStructureId(getIdentifier());
		if(hasPersistence())
		{
			other.setPersistence(getPersistence());
		}
	}

	void Structure::copyPins(Structure & newStructure, StructureCircuits & circuits, StructureCircuit & circuit) const
	{
		std::vector<StructurePin*> allPins = findAllPins();

		for(std::vector<StructurePin*>::const_iterator it = allPins.begin(); it != allPins.end(); it ++)
		{
			StructurePin * oldPin = * it;
			StructurePin & newPin = oldPin->clone(newStructure, circuits, circuit );

			newStructure.addPin(newPin.getPinType(), & newPin);
		}
	}

	const Structure& Structure::getTopmostParent() const
	{
		assert(hasParent());
		const Structure & parent = getParent();
		if(parent.hasParent())
		{
			return parent.getTopmostParent();
		}
		else
		{
			return parent;
		}
	}

	   std::vector<const Structure*> Structure::getTopmostParents() const
	   {
		   assert(hasParent());
		   std::vector<const Structure*> topmostParents;
		   const std::vector<Structure*> parents = getParents();
		   for(std::vector<Structure*>::const_iterator it = parents.begin(); it != parents.end(); it++)
		   {
			   Structure * parent = *it;
			   if(parent->hasParent())
			   {
				   std::vector<const Structure*> currentTopmostParents = parent->getTopmostParents();
				   topmostParents.insert(topmostParents.end(), currentTopmostParents.begin(), currentTopmostParents.end());
			   }
			   else
			   {
				   topmostParents.push_back(parent);
			   }
		   }
		   return topmostParents;
	   }

       const std::vector<Structure *> Structure::getAllHigherLevelParents() const
		{
		   std::vector<Structure*> higherLevelParents;
		   const std::vector<Structure*> parents = getParents();
		   higherLevelParents.insert(higherLevelParents.end(), parents.begin(),parents.end());
		   for(std::vector<Structure*>::const_iterator it = parents.begin(); it != parents.end(); it++)
		   {
			   Structure * parent = *it;
			   if(parent->hasParent())
			   {
				   std::vector<Structure*> nextHigherLevelParents = parent->getAllHigherLevelParents();
				   higherLevelParents.insert(higherLevelParents.end(), nextHigherLevelParents.begin(), nextHigherLevelParents.end());
			   }
		   }
		   return higherLevelParents;
	   }



	 bool Structure::hasCommonDevices(const Structure & other) const
	 {
		 std::vector<const Core::Device*> devices1 = getDevices();
		 std::vector<const Core::Device*> devices2 = other.getDevices();
		 bool hasCommonDevices =false;
		 for(auto& it1 : devices1)
		 {
			 const Core::Device & device1 = * it1;
			 for(auto& it2 : devices2)
			 {
				 const Core::Device& device2 = *it2;
				 if(device1.getIdentifier() == device2.getIdentifier())
				 {
					 hasCommonDevices = true;
					 break;
				 }
			 }
		 }

		 return hasCommonDevices;
	 }

	bool Structure::isCurrentMirror() const
	{
		const StructureName& simpleCurrentMirror = StructRec::StructureName("MosfetSimpleCurrentMirror");
		const StructureName& cascodeCurrentMirror = StructRec::StructureName("MosfetCascodeCurrentMirror");
		const StructureName& improvedWilsonCurrentMirror = StructRec::StructureName("MosfetImprovedWilsonCurrentMirror");
		const StructureName& wideSwingCascodeCurrentMirror = StructRec::StructureName("MosfetWideSwingCascodeCurrentMirror");
		const StructureName& wideSwingCurrentMirror = StructRec::StructureName("MosfetWideSwingCurrentMirror");
		const StructureName& fourTransistorCurrentMirror = StructRec::StructureName("MosfetFourTransistorCurrentMirror");
		const StructureName& wilsonCurrentMirror = StructRec::StructureName("MosfetWilsonCurrentMirror");


		return (getStructureName() == simpleCurrentMirror) ||
		        (getStructureName() == cascodeCurrentMirror) ||
		        (getStructureName() == improvedWilsonCurrentMirror) ||
		         (getStructureName() == wideSwingCascodeCurrentMirror) ||
		         (getStructureName() == fourTransistorCurrentMirror) ||
		         (getStructureName() == wilsonCurrentMirror) ||
		          (getStructureName() == wideSwingCurrentMirror);
	}

	bool Structure::isPartOfCurrentMirror() const
	{
		if(hasParent())
		{
			bool isTrue = false;
			for(auto parent : getParents())
			{
				if(isTrue ==false)
				{
					isTrue = parent->isPartOfCurrentMirror();
				}
				else
				{
					break;
				}
			}
			return isTrue;
		}
		if(isCurrentMirror())
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	 const Structure& Structure::getTopmostParentWithDifferentDevices(const Structure & other) const
	 {
	 	assert(hasParent());
	 	const Structure & parent = getParent();
	 	if(parent.hasParent())
	 	{
	 		return parent.getTopmostParent();
	 	}
	 	else if(!parent.hasCommonDevices(other))
	 	{
	 			return parent;
	 	}
	 	else
	 	{
	 		Structure * struc = NULL;
	 		return * struc;
	 	}
	 }

}

