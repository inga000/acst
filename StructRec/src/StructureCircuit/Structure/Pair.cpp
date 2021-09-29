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

#include "StructRec/incl/StructureCircuit/Structure/Pair.h"
#include "StructRec/incl/Results/StructureCircuits.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace StructRec {

	const int Pair::CHILD_ONE_ = 1;
	const int Pair::CHILD_TWO_ = 2;

	Pair::Pair() :
		child1_(NULL),
		child2_(NULL),
		symmetric_(false)
	{
	}

	Pair::~Pair()
	{
	}

 	Pair & Pair::clone(StructureCircuit & circuit, StructureCircuits & circuits) const
 	{
 		Pair * newPair = new Pair();

 		copyBasics(*newPair);
 		copyPins(*newPair,circuits, circuit);

 		Structure & newChild1 = circuits.findStructureInTopCircuit(getChild1().getIdentifier());
 		newChild1.setParent(*newPair);
 		newPair->setChild1(newChild1);

 		Structure & newChild2 = circuits.findStructureInTopCircuit(getChild2().getIdentifier());
 		newChild2.setParent(*newPair);
 		newPair->setChild2(newChild2);

 		return * newPair;
 	}

	void Pair::setChild1(Structure & child1)
	{
		child1_ = & child1;
	}

	void Pair::setChild2(Structure & child2)
	{
		child2_ = & child2;
	}

	void Pair::setSymmetric(bool symmetric)
	{
		symmetric_ = symmetric;
	}

	bool Pair::hasChild1() const
	{
		return child1_ != NULL;
	}

	bool Pair::hasChild2() const
	{
		return child2_ != NULL;
	}

	Structure& Pair::getChild1() const
	{
		assert(hasChild1());
		return *child1_;
	}

	Structure& Pair::getChild2() const
	{
		assert(hasChild2());
		return *child2_;
	}

	bool Pair::isSymmetric() const
	{
		return symmetric_;
	}

	const Structure& Pair::getChild(int childNumber) const
	{
		const Structure* child = NULL;
		if(childNumber == CHILD_ONE_) {
			child = &getChild1();
		} else if(childNumber == CHILD_TWO_) {
			child = &getChild2();
		}
		assert(child != NULL);
		return *child;
	}

	bool Pair::isPair() const
	{
		return true;
	}

	bool Pair::isArray() const
	{
		return false;
	}

	void Pair::printChildren(std::ostream& stream, int column) const
	{
		getChild1().print(stream, column);
		getChild2().print(stream, column);
	}

	void Pair::writeXmlOfChildren(Core::XmlNode & structure, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const
		{
			getChild1().writeXml(structure, doc, instanceNamePath);
			getChild2().writeXml(structure, doc, instanceNamePath);
		}

	StructurePin& Pair::findChild1Pin(const StructurePinType & pinType) const
	{
		return getChild1().findPin(pinType);
	}

	StructurePin& Pair::findChild2Pin(const StructurePinType & pinType) const
	{
		return getChild2().findPin(pinType);
	}

	std::vector<const Core::Device*> Pair::getDevices() const
	{
		std::vector<const Core::Device*> child1Devices = getChild1().getDevices();
		std::vector<const Core::Device*> child2Devices = getChild2().getDevices();

		std::vector<const Core::Device*> allDevices;
	    allDevices.insert(allDevices.end(), child1Devices.begin(), child1Devices.end());
	    allDevices.insert(allDevices.end(), child2Devices.begin(), child2Devices.end());
	    return allDevices;
	}

	std::vector<const Structure*> Pair::getArrayChildren() const
	{
		std::vector<const Structure*> child1Arrays = getChild1().getArrayChildren();
		std::vector<const Structure*> child2Arrays = getChild2().getArrayChildren();

		std::vector<const Structure*> arrays;
		arrays.insert(arrays.end(), child1Arrays.begin(), child1Arrays.end());
		arrays.insert(arrays.end(), child2Arrays.begin(), child2Arrays.end());
		return arrays;
	}

	void Pair::removeThisAsParentOfChildren() const //Parent and child in same circuit
	{

		getChild1().removeParent(getIdentifier());

		getChild2().removeParent(getIdentifier());

	}

	void Pair::removeThisAsParentOfStructuresInTheseCircuits(StructureCircuits & circuits, Core::InstanceNamePath instanceNamePath) const  // Parent and child in different circuit
	{
		// The parent is a cross instance structure, we now search for the children in different instances having this structure as parent
		//Remove the parent of child 1
		StructureName nameChild1 = getChild1().getStructureName();
		nameChild1.removeInstanceNamePathFromName(instanceNamePath);

		StructureName nameChild2 = getChild2().getStructureName();
		nameChild2.removeInstanceNamePathFromName(instanceNamePath);

		for(auto possibleChild : circuits.getStructures(nameChild1))
		{
			if(possibleChild->hasParent() && possibleChild->hasDirectParent(getIdentifier()))
			{
				possibleChild->removeParent(getIdentifier());
			}
		}
		//Remove Parent of child 2
		if(nameChild1 != nameChild2)
		{
			for(auto possibleChild : circuits.getStructures(nameChild2))
			{
				if(possibleChild->hasParent() && possibleChild->hasDirectParent(getIdentifier()))
				{
					possibleChild->removeParent(getIdentifier());
				}
			}
		}
	}

	void Pair::deleteChildrenOfCrossInstanceStructure()
	{
		if(getChild1().getIdentifier().getName().toStr().compare(0,1, "/") == 0)
		{
			getChild1().deleteChildrenRecursivly();
			delete child1_;
		}
		if(getChild2().getIdentifier().getName().toStr().compare(0,1, "/") == 0)
		{
			getChild2().deleteChildrenRecursivly();
			delete child2_;
		}

	}

	void Pair::deleteChildrenRecursivly() const
	{
		if(!(getChild1().getIdentifier().getName().toStr().compare(0,1, "/") == 0))
		{
			getChild1().deleteChildrenRecursivly();
			delete child1_;
		}

		if(!(getChild2().getIdentifier().getName().toStr().compare(0,1, "/") == 0))
		{
			getChild2().deleteChildrenRecursivly();
			delete child2_;
		}
	}

}
