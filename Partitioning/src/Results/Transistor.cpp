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

#include "Partitioning/incl/Results/Transistor.h"
#include "StructRec/incl/StructureCircuit/Structure/Array.h"

namespace Partitioning {

 	 const float Transistor::NOT_INITIALIZED_ = -100;

	Transistor::Transistor() :
		width_(NOT_INITIALIZED_),
		length_(NOT_INITIALIZED_)
	{
	}

	Transistor::~Transistor()
	{
	}

	Core::DeviceId Transistor::getDeviceId() const
	{
		const StructRec::Array& array = static_cast<const StructRec::Array&>(getArray());
		return array.getFirstDevice()->getIdentifier();
	}

	bool Transistor::isTransistor() const
	{
		return true;
	}

	bool Transistor::isTwoPort() const
	{
		return false;
	}

	void Transistor::setWidth(float width)
	{
		width_ = width;
	}

	void Transistor::setLength(float length)
	{
		length_ = length;
	}

	float Transistor::getLength() const
	{
		assert(hasLength());
		return length_;
	}

	float Transistor::getWidth() const
	{
		assert(hasWidth());
		return width_;
	}

	void Transistor::print(std::ostream& stream) const
	{
		std::vector<const Core::Device*> devices = getArray().getDevices();
		for(auto& it : devices)
		{
			const Core::Device* device = it;
			stream << device->getIdentifier().toStr() << " " << getPart().getPartId().toStr();
			if(hasWidth() && hasLength())
					stream << " Width: " << getWidth() << " Length: " << getLength();
			stream << std::endl;
		}

	}

	bool Transistor::hasLength() const
	{
		return length_ != NOT_INITIALIZED_;
	}

	bool Transistor::hasWidth() const
	{
		return width_ != NOT_INITIALIZED_;
	}



}


