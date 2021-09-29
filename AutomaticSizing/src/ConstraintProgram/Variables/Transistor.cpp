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

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Transistor.h"
#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variable.h"

#include <sstream>

namespace AutomaticSizing
{
	const std::string Transistor::BIAS_STRING_ = "bias";
   const std::string Transistor::OPAMP_STRING_ = "opAmp";

	const int Transistor::NOT_INITIALIZED_ = -1;

	Transistor::Transistor() :
			type_(TYPE_UNINITIALIZED),
			lengthIndex_(NOT_INITIALIZED_),
			lengthRndSeed_(1),
			widthIndex_(NOT_INITIALIZED_),
			widthRndSeed_(1),
			currentIndex_(NOT_INITIALIZED_),
			currentRndSeed_(1),
			gateVoltageIndex_(NOT_INITIALIZED_),
			gateVoltageRndSeed_(1),
			drainVoltageIndex_(NOT_INITIALIZED_),
			drainVoltageRndSeed_(1),
			sourceVoltageIndex_(NOT_INITIALIZED_),
			sourceVoltageRndSeed_(1)
	{
	}

	Transistor::~Transistor()
	{

	}

	 void Transistor::setType(const std::string string)
	 {
	      type_ = mapStringToEnum(string);
	 }

	 bool Transistor::isValidString(const std::string & str)
	 {
	    return getStringToEnumMap().find(str) != getStringToEnumMap().end();
	 }

	 std::string Transistor::getType() const
	 {
	     assert(hasType());
	     return mapEnumToString(type_);
	 }

	 Transistor::TypeEnum Transistor::mapStringToEnum(const std::string & str)
	 {
	     assert(isValidString(str));
	     return getStringToEnumMap().at(str);
	 }

	bool Transistor::isInBias() const
	{
	       assert(hasType());
	       return type_ == TYPE_BIAS;
	}

	bool Transistor::isInOpAmp() const
	{
	     assert(hasType());
	     return type_ == TYPE_OPAMP;
	}

	void Transistor::setWidthIndex(int widthIndex)
	{
		widthIndex_ = widthIndex;
	}

	void Transistor::setLengthIndex(int lengthIndex)
	{
		lengthIndex_ = lengthIndex;
	}

	void Transistor::setCurrentIndex(int currentIndex)
	{
		currentIndex_ = currentIndex;
	}

	void Transistor::setGateVoltageIndex(int gateVoltageIndex)
	{
		gateVoltageIndex_ = gateVoltageIndex;
	}

	void Transistor::setDrainVoltageIndex(int drainVoltageIndex)
	{
		drainVoltageIndex_ = drainVoltageIndex;
	}


	void Transistor::setSourceVoltageIndex(int sourceVoltageIndex)
	{
		sourceVoltageIndex_ = sourceVoltageIndex;
	}

	int Transistor::getWidthIndex() const
	{
		assert(widthIndex_ != NOT_INITIALIZED_);
		return widthIndex_;
	}

	int Transistor::getLengthIndex() const
	{
		assert(lengthIndex_ != NOT_INITIALIZED_);
		return lengthIndex_;
	}

	int Transistor::getCurrentIndex() const
	{
		assert(currentIndex_ != NOT_INITIALIZED_);
		return currentIndex_;
	}

	int Transistor::getGateVoltageIndex() const
	{
		assert(gateVoltageIndex_ != NOT_INITIALIZED_);
		return gateVoltageIndex_;
	}

	int Transistor::getDrainVoltageIndex() const
	{
		assert(drainVoltageIndex_ != NOT_INITIALIZED_);
		return drainVoltageIndex_;
	}

	int Transistor::getSourceVoltageIndex() const
	{
		assert(sourceVoltageIndex_ != NOT_INITIALIZED_);
		return sourceVoltageIndex_;
	}

	void Transistor::changeLengthRndSeed(unsigned int newSeed)
	{
		lengthRndSeed_ = newSeed;
	}

	void Transistor::changeWidthRndSeed(unsigned int newSeed)
	{
		widthRndSeed_ = newSeed;
	}

	void Transistor::changeCurrentRndSeed(unsigned int newSeed)
	{
		currentRndSeed_ = newSeed;
	}

	void Transistor::changeGateVoltageRndSeed(unsigned int newSeed)
	{
		gateVoltageRndSeed_ = newSeed;
	}

	void Transistor::changeDrainVoltageRndSeed(unsigned int newSeed)
	{
		drainVoltageRndSeed_ = newSeed;
	}

	void Transistor::changeSourceVoltageRndSeed(unsigned int newSeed)
	{
		sourceVoltageRndSeed_ = newSeed;
	}

	unsigned int Transistor::getLengthRndSeed()
	{
		return lengthRndSeed_;
	}

	unsigned int Transistor::getWidthRndSeed()
	{

		return widthRndSeed_;

	}

	unsigned int Transistor::getCurrentRndSeed()
	{
		return currentRndSeed_;
	}

	unsigned int Transistor::getGateVoltageRndSeed()
	{
		return gateVoltageRndSeed_;
	}

	unsigned int Transistor::getDrainVoltageRndSeed()
	{
		return drainVoltageRndSeed_;
	}

	unsigned int Transistor::getSourceVoltageRndSeed()
	{
		return sourceVoltageRndSeed_;
	}

	bool Transistor::hasVariableIndex(int index) const
	{
		if(getLengthIndex() == index)
		{
			return true;
		}
		else if(getWidthIndex() == index)
		{
			return true;
		}
		else if(getCurrentIndex() == index)
		{
			return true;
		}
		else if(getGateVoltageIndex() == index)
		{
			return true;
		}
		else if(getDrainVoltageIndex() == index)
		{
			return true;
		}
		else if(getSourceVoltageIndex() == index)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string Transistor::print() const
	{
		std::ostringstream oss;
		oss << "Name: " << getName() << std::endl;
		oss << "Type: " << getType() << std::endl;
		oss << "LengthIndex: " <<lengthIndex_ << std::endl;
		oss << "WidthIndex: " << widthIndex_ << std::endl;
		oss << "CurrentIndex: " << currentIndex_ << std::endl;
		oss << "GateVoltageIndex: " << gateVoltageIndex_ << std::endl;
		oss << "DrainVoltageIndex: " << drainVoltageIndex_ << std::endl;
		oss << "SourceVoltageIndex: " << sourceVoltageIndex_ << std::endl;

		return oss.str();
	}

	std::string Transistor::mapEnumToString(const TypeEnum & tt)
	{
	   return getEnumToStringMap().at(tt);
	}

	const Transistor::StringToEnumMap & Transistor::getStringToEnumMap()
	{
	    static StringToEnumMap theMap;

	    if(theMap.empty()) {
	         theMap[BIAS_STRING_] = TYPE_BIAS;
	         theMap[OPAMP_STRING_] = TYPE_OPAMP;
	    }
	    return theMap;
	 }

	 const Transistor::EnumToStringMap & Transistor::getEnumToStringMap()
	 {
	     static EnumToStringMap theMap;

	     if(theMap.empty()) {
	           theMap[TYPE_OPAMP] = OPAMP_STRING_;
	           theMap[TYPE_BIAS] = BIAS_STRING_;

	      }
	      return theMap;
	 }

	 bool Transistor::hasType() const
	 {
		 return type_ != TYPE_UNINITIALIZED;
	 }

	bool Transistor::isTransistor() const
	{
		return true;
	}

	bool Transistor::isOther() const
	{
		return false;
	}

}
