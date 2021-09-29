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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_TRANSISTOR_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_TRANSISTOR_H_

#include "AutomaticSizing/incl/ConstraintProgram/Variables/Variable.h"

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"


namespace AutomaticSizing {

class Transistor : public Variable{

public:
	Transistor();
	~Transistor();

	void setType(const std::string type);

	void setWidthIndex(int widthIndex);
	void setLengthIndex(int lengtIndex);
	void setCurrentIndex(int currentIndex);
	void setGateVoltageIndex(int gateVoltageIndex);
	void setDrainVoltageIndex(int drainVoltageIndex);
	void setSourceVoltageIndex(int drainVoltageIndex);

	std::string getType() const;

	int getWidthIndex() const;
	int getLengthIndex() const;
	int getCurrentIndex() const;
	int getGateVoltageIndex() const;
	int getDrainVoltageIndex() const;
	int getSourceVoltageIndex() const;

	void changeLengthRndSeed(unsigned int newSeed);
	void changeWidthRndSeed(unsigned int newSeed);
	void changeCurrentRndSeed(unsigned int newSeed);
	void changeGateVoltageRndSeed(unsigned int newSeed);
	void changeDrainVoltageRndSeed(unsigned int newSeed);
	void changeSourceVoltageRndSeed(unsigned int newSeed);

	unsigned int getLengthRndSeed();
	unsigned int getWidthRndSeed();
	unsigned int getCurrentRndSeed();
	unsigned int getGateVoltageRndSeed();
	unsigned int getDrainVoltageRndSeed();
	unsigned int getSourceVoltageRndSeed();

	bool isInBias() const;
	bool isInOpAmp() const;
	bool hasVariableIndex(int index) const;

	bool isTransistor() const;
	bool isOther() const;

	std::string print() const;

private:
	enum TypeEnum {
		       TYPE_UNINITIALIZED,
		       TYPE_BIAS,
		       TYPE_OPAMP,
		};
	  typedef std::map<std::string,TypeEnum> StringToEnumMap;
	  typedef std::map<TypeEnum,std::string> EnumToStringMap;

	  static bool isValidString(const std::string & ttStr);

	  static const StringToEnumMap & getStringToEnumMap();
	  static const EnumToStringMap & getEnumToStringMap();

	  static TypeEnum mapStringToEnum(const std::string & ttStr);
	  static std::string mapEnumToString(const TypeEnum & tt);

	  bool hasType() const;

private:


	static const std::string OPAMP_STRING_;
	static const std::string BIAS_STRING_;

	static const int NOT_INITIALIZED_;

	int lengthIndex_;
	mutable unsigned int lengthRndSeed_;
	int widthIndex_;
	mutable unsigned int widthRndSeed_;
	int currentIndex_;
	mutable unsigned int currentRndSeed_;
	int gateVoltageIndex_;
	mutable unsigned int gateVoltageRndSeed_;
	int drainVoltageIndex_;
	mutable unsigned int drainVoltageRndSeed_;
	int sourceVoltageIndex_;
	mutable unsigned int sourceVoltageRndSeed_;


	TypeEnum type_;

};

}



#endif /* AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_TRANSISTOR_H_ */
