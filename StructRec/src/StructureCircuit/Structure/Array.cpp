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

#include "StructRec/incl/StructureCircuit/Structure/Array.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	Array & Array::clone(StructureCircuit& circuit, StructureCircuits& circuits) const
	{
		Array* newArray = new Array();
		copyBasics(* newArray);
		copyPins(*newArray, circuits, circuit);
		newArray->setDevices(getDevices());
		return* newArray;
	}

    void Array::setDevices(const std::vector<const Core::Device*>& devices)
    {
    	array_ = devices;
    }

    const Core::Device* Array::getFirstDevice() const
    {
    	assert(!array_.empty(), "Array empty!");
    	return array_.front();
    }

    std::vector<const Core::Device*> Array::getDevices() const
	{
	   return array_;
	}

    std::vector<const Structure*> Array::getArrayChildren() const
  	{
      	std::vector<const Structure*> arrays;



      	arrays.push_back(this);




  		return arrays;
  	}

	bool Array::isPair() const
	{
		return false;
	}

	bool Array::isArray() const
	{
		return true;
	}

	void Array::printChildren(std::ostream& stream, int column) const
	{
		for( std::vector<const Core::Device*>::const_iterator it = array_.begin(); it != array_.end(); it++)
		{
			const Core::Device * device = *it;
			device->print(stream, column);
			stream << std::endl;
		}
	}

	void Array::writeXmlOfChildren(Core::XmlNode & structure, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const
	{
		Core::XmlNode & devices = Core::RapidXmlUtils::addNode(structure, doc, "devices");
		for( std::vector<const Core::Device*>::const_iterator it = array_.begin(); it != array_.end(); it++)
		{
			const Core::Device * device = *it;
			Core::XmlNode & deviceNode = Core::RapidXmlUtils::addNode(devices, doc, "device");
			Core::RapidXmlUtils::addAttr(deviceNode, doc, "name", device->getIdentifier().toStr());
			Core::RapidXmlUtils::addAttr(deviceNode, doc, "deviceType", device->getDeviceType().toStr());
			Core::RapidXmlUtils::addAttr(deviceNode, doc, "techType", device->getTechType().toStr());
			Core::RapidXmlUtils::addAttr(deviceNode, doc, "instance", instanceNamePath.toStr());
		}
	}

	void Array::removeThisAsParentOfChildren() const
	{
	}

	void Array::removeThisAsParentOfStructuresInTheseCircuits(StructureCircuits  &circuits, Core::InstanceNamePath instanceNamePath) const
	{

	}

	void Array::deleteChildrenRecursivly() const
	{

	}
}
