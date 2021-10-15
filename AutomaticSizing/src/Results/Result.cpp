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


#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"

#include "AutomaticSizing/incl/Results/Result.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"

#include "StructRec/incl/StructureCircuit/Structure/Array.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {

const int Result::NOT_INITIALIZED_= -1000000000;

Result::Result() :
		area_(NOT_INITIALIZED_),
		power_(NOT_INITIALIZED_),
		gain_(NOT_INITIALIZED_),
		slewRate_(NOT_INITIALIZED_),
		transitFrequency_(NOT_INITIALIZED_),
		transitFrequencyWithErrorFactor_(NOT_INITIALIZED_),
		phaseMargin_(NOT_INITIALIZED_),
		CMRR_(NOT_INITIALIZED_),
		posPSRR_(NOT_INITIALIZED_),
		negPSRR_(NOT_INITIALIZED_),
		vcmMax_(NOT_INITIALIZED_),
		vcmMin_(NOT_INITIALIZED_),
		voutMin_(NOT_INITIALIZED_),
		voutMax_(NOT_INITIALIZED_)
{
}

Result::~Result()
{
}

void Result::initializeTwoPortMap(const Partitioning::Result & partitioningResult)
{
	for(auto & twoPort : partitioningResult.getAllTwoPorts())
	{
		twoPorts_.insert(std::pair<StructRec::StructureId,Partitioning::TwoPort*>(twoPort->getArray().getIdentifier(), twoPort));
	}
}

void Result::initializeTransistorMap(const Partitioning::Result & partitioningResult)
{
	for(auto & transistor : partitioningResult.getAllTransistors())
	{
		transistors_.insert(std::pair<StructRec::StructureId,Partitioning::Transistor*>(transistor->getArray().getIdentifier(), transistor));
	}
}


void Result::addCurrent(Partitioning::Component & component, float current)
{
	const StructRec::Structure& structure = component.getArray();
	const StructRec::Array& array = static_cast<const StructRec::Array&>(structure);
	Core::DeviceId deviceId = array.getFirstDevice()->getIdentifier();
	currentMap_.insert(std::pair<Core::DeviceId, float>(deviceId, current));
}

void Result::addVoltage(Core::NetId netId, float voltage)
{
	voltageMap_.insert(std::pair<Core::NetId, float>(netId, voltage));
}

std::string Result::toStr() const
{
	std::ostringstream stream;

	stream << std::endl << std::endl;
	for(std::map<StructRec::StructureId, Partitioning::Transistor*>::const_iterator it_tran = transistors_.begin(); it_tran != transistors_.end(); it_tran++)
	{
		Partitioning::Transistor * component = it_tran->second;
		component->print(stream);
	}
	for(std::map<StructRec::StructureId, Partitioning::TwoPort*>::const_iterator it_twoPort = twoPorts_.begin(); it_twoPort != twoPorts_.end(); it_twoPort++)
	{
		Partitioning::TwoPort * component = it_twoPort->second;
		component->print(stream);
	}

	stream << std::endl;

	writeExpectedPerformance(stream);
	writeVoltages(stream);
	writeCurrents(stream);

	return stream.str();
}

std::vector<Partitioning::Component*> Result::findComponents(const StructRec::Structure & structure) const
{
	std::vector<Partitioning::Component*> components;
	std::vector<const StructRec::Structure*> arrayChildren = structure.getArrayChildren();
	for(auto& it : arrayChildren)
	{
		const StructRec::Structure & arrayChild = *it;
		Partitioning::Component & component = findComponent(arrayChild.getIdentifier());
		components.push_back(& component);
	}
	return components;
}

Partitioning::Component & Result::findComponent(StructRec::StructureId structureId) const
{
	Partitioning::Component* comp = NULL;
	for(std::map<StructRec::StructureId, Partitioning::Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Partitioning::Transistor * tran = it->second;
		if(tran->getArray().getIdentifier() == structureId)
		{
			comp = tran;
		}
	}
	for(std::map<StructRec::StructureId, Partitioning::TwoPort*>::const_iterator it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		Partitioning::TwoPort * port = it->second;
		if(port->getArray().getIdentifier() == structureId)
		{
			comp = port;
		}
	}
	assert(comp != NULL, "No suited Components found, Structure might be a pair.");
	return *comp;
}



void Result::setGain(float gain)
{
	gain_ = gain;
}

void Result::setPower(float power)
{
	power_ = power;
}

void Result::setSlewRate(float slewRate)
{
	slewRate_ = slewRate;
}

void Result::setPhaseMargin(float phaseMargin)
{
	phaseMargin_ = phaseMargin;
}

void Result::setTransitFrequency(float transitFrequency)
{
	transitFrequency_ = transitFrequency;
}

void Result::setTransitFrequencyWithErrorFactor(float transitFrequency)
{
	transitFrequencyWithErrorFactor_ = transitFrequency;
}

void Result::setArea(float area)
{
	area_ = area;
}

void Result::setCMRR(float CMRR)
{
	CMRR_ = CMRR;
}


void Result::setPosPSRR(float posPSRR)
{
	posPSRR_ = posPSRR;
}

void Result::setNegPSRR(float negPSRR)
{
	negPSRR_ = negPSRR;
}

std::vector<Partitioning::Transistor*> Result::getAllTransistors() const
{
	assert(!transistors_.empty());

	std::vector<Partitioning::Transistor*> transistors;
	for(std::map<StructRec::StructureId, Partitioning::Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Partitioning::Transistor * tran = it->second;
		transistors.push_back(tran);
	}
	return transistors;
}

std::vector<Partitioning::TwoPort*> Result::getAllTwoPorts() const
{
	assert(!twoPorts_.empty());
	std::vector<Partitioning::TwoPort*> twoPorts;
	for(std::map<StructRec::StructureId, Partitioning::TwoPort*>::const_iterator it = twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		Partitioning::TwoPort * twoPort = it->second;
		twoPorts.push_back(twoPort);
	}
	return twoPorts;
}



float Result::getGain() const
{
	assert(gain_ != NOT_INITIALIZED_);
	return gain_;
}

float Result::getPower() const
{
	assert(power_ != NOT_INITIALIZED_);
	return power_;
}

float Result::getSlewRate() const
{
	assert(slewRate_ != NOT_INITIALIZED_);
	return slewRate_;
}



float Result::getPhaseMargin() const
{
	assert(phaseMargin_ != NOT_INITIALIZED_);
	return phaseMargin_;
}

float Result::getTransitFrequency() const
{
	assert( transitFrequency_ != NOT_INITIALIZED_);
	return transitFrequency_;
}

float Result::getTransitFrequencyWithErrorFactor() const
{
	assert( transitFrequencyWithErrorFactor_ != NOT_INITIALIZED_);
	return transitFrequencyWithErrorFactor_;
}

float Result::getArea() const
{
	assert(area_ != NOT_INITIALIZED_);
	return area_;
}

float Result::getCMRR() const
{
	assert( CMRR_ != NOT_INITIALIZED_);
	return CMRR_;
	}


float Result::getPosPSRR() const
{
	assert(posPSRR_ != NOT_INITIALIZED_);
	return posPSRR_;
}

float Result::getNegPSRR() const
{
//	assert(negPSRR_ != NOT_INITIALIZED_);
	return negPSRR_;
}

bool Result::hasArea() const
{
	return area_ != NOT_INITIALIZED_;
}

bool Result::hasPower() const
{
	return power_ != NOT_INITIALIZED_;
}

bool Result::hasGain() const
{
	return gain_ != NOT_INITIALIZED_;
}

bool Result::hasSlewRate() const
{
	return slewRate_ != NOT_INITIALIZED_;
}

bool Result::hasTransitFrequency() const
{
	return transitFrequency_ != NOT_INITIALIZED_;
}

bool Result::hasTransitFrequencyWithErrorFactor() const
{
	return transitFrequencyWithErrorFactor_ != NOT_INITIALIZED_;
}

bool Result::hasPhaseMargin() const
{
	return phaseMargin_ != NOT_INITIALIZED_;
}

bool Result::hasCMRR() const
{
	return CMRR_ != NOT_INITIALIZED_;
}

bool Result::hasNegPSRR() const
{
	return negPSRR_ != NOT_INITIALIZED_;
}

bool Result::hasPosPSRR() const
{
	return negPSRR_ != NOT_INITIALIZED_;
}

bool Result::hasMaxCommonModeInputVoltage() const
{
	return vcmMax_ > -1000000;
}

bool Result::hasMinCommonModeInputVoltage() const
{
	return vcmMin_ > -10000;
}

bool Result::hasMaxOutputVoltage() const
{
	return voutMax_ != NOT_INITIALIZED_;
}

bool Result::hasMinOutputVoltage() const
{
	return voutMin_ != NOT_INITIALIZED_;
}



void Result::writeXmlAutomaticSizingResult(Control::OutputFile & file)const
{
	file.open();

	Core::XmlDocument doc;
	Core::XmlNode & root = Core::RapidXmlUtils::createXMLRootNodeForResults(doc);
	Core::RapidXmlUtils::writeDateXML(root, doc);
	Core::XmlNode & headLine = Core::RapidXmlUtils::addNode(root, doc, "automatic_sizing-results");

	writeXmlExpectedPerformance(headLine, doc);
	writeXmlVoltages(headLine, doc);
	writeXmlCurrents(headLine, doc);

	writeXmlComponentDimensions(headLine, doc);

	std::ostringstream oss;
	oss << doc;
	file << oss.str();

	file.close();

}

void Result::writeXmlExpectedPerformance(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & performance = Core::RapidXmlUtils::addNode(node, doc, "ExpectedPerformance");

	std::ostringstream gainString;
	gainString << getGain();
	Core::XmlNode & gainNode = Core::RapidXmlUtils::addNode(performance, doc, "Gain", gainString.str());
	Core::RapidXmlUtils::addAttr(gainNode, doc, "unit", "dB");


	float normedPower = getPower() * 1000;
	std::ostringstream powerString;
	powerString << normedPower;
	Core::XmlNode & powerNode = Core::RapidXmlUtils::addNode(performance, doc, "Power", powerString.str());
	Core::RapidXmlUtils::addAttr(powerNode, doc, "unit", "m_W");


	std::ostringstream areaString;
	areaString << getArea();
	Core::XmlNode & areaNode = Core::RapidXmlUtils::addNode(performance, doc, "Area", areaString.str());
	Core::RapidXmlUtils::addAttr(areaNode, doc, "unit", "(mu_m)^2");

	float normedTransitFrequency = getTransitFrequency()/1000000;
	std::ostringstream transitFrequencyString;
	transitFrequencyString << normedTransitFrequency;
	Core::XmlNode & transitFrequencyNode = Core::RapidXmlUtils::addNode(performance, doc, "TransitFrequency", transitFrequencyString.str());
	Core::RapidXmlUtils::addAttr(transitFrequencyNode, doc, "unit", "M_Hz");

	float normedTransitFrequencyWithErrorFactor = getTransitFrequencyWithErrorFactor()/1000000;
	std::ostringstream transitFrequencyWithErrorFactorString;
	transitFrequencyWithErrorFactorString << normedTransitFrequencyWithErrorFactor;
	Core::XmlNode & transitFrequencyWithErrorFactorNode = Core::RapidXmlUtils::addNode(performance, doc, "TransitFrequencyWithErrorFactor", transitFrequencyWithErrorFactorString.str());
	Core::RapidXmlUtils::addAttr(transitFrequencyWithErrorFactorNode, doc, "unit", "M_Hz");

	float normedSlewRate = getSlewRate()/ 1000000;
	std::ostringstream slewRateString;
	slewRateString << normedSlewRate;
	Core::XmlNode & slewRateNode = Core::RapidXmlUtils::addNode(performance, doc, "SlewRate", slewRateString.str());
	Core::RapidXmlUtils::addAttr(slewRateNode, doc, "unit", "V/mum_s");


	float normedPhaseMargin = getPhaseMargin() * 180/3.14159265;
	std::ostringstream phaseMarginString;
	phaseMarginString << normedPhaseMargin;
	Core::XmlNode & phaseMarginNode = Core::RapidXmlUtils::addNode(performance, doc, "PhaseMargin", phaseMarginString.str());
	Core::RapidXmlUtils::addAttr(phaseMarginNode, doc, "unit", "degree");

	if(hasCMRR())
	{
		std::ostringstream CMRRString;
		CMRRString << getCMRR();
		Core::XmlNode & CMRRNode = Core::RapidXmlUtils::addNode(performance, doc, "CMRR", CMRRString.str());
		Core::RapidXmlUtils::addAttr(CMRRNode, doc, "unit", "dB");
	}


	if(hasNegPSRR())
	{
	std::ostringstream negPSRRString;
	negPSRRString << getNegPSRR();
	Core::XmlNode & negPSRRNode = Core::RapidXmlUtils::addNode(performance, doc, "negPSRR", negPSRRString.str());
	Core::RapidXmlUtils::addAttr(negPSRRNode, doc, "unit", "degree");
	}

	if(hasPosPSRR())
	{
	std::ostringstream posPSRRString;
	posPSRRString << getPosPSRR();
	Core::XmlNode & posPSRRNode = Core::RapidXmlUtils::addNode(performance, doc, "posPSRR", posPSRRString.str());
	Core::RapidXmlUtils::addAttr(posPSRRNode, doc, "unit", "degree");
	}


	std::ostringstream maxOutputVoltageString;
	maxOutputVoltageString << getMaxOutputVoltage();
	Core::XmlNode & maxOutputVoltageNode = Core::RapidXmlUtils::addNode(performance, doc, "MaximumOutputVoltage", maxOutputVoltageString.str());
	Core::RapidXmlUtils::addAttr(maxOutputVoltageNode, doc, "unit", "V");


	std::ostringstream minOutputVoltageString;
	minOutputVoltageString << getMinOutputVoltage();
	Core::XmlNode & minOutputVoltageNode = Core::RapidXmlUtils::addNode(performance, doc, "MinimumOutputVoltage", minOutputVoltageString.str());
	Core::RapidXmlUtils::addAttr(minOutputVoltageNode, doc, "unit", "V");

	if(hasMaxCommonModeInputVoltage())
	{
		std::ostringstream maxCommonModeInputVoltageString;
		maxCommonModeInputVoltageString << getMaxCommonModeInputVoltage();
		Core::XmlNode & maxCommonModeInputVoltageNode = Core::RapidXmlUtils::addNode(performance, doc, "maxCommonModeInputVoltage", maxCommonModeInputVoltageString.str());
		Core::RapidXmlUtils::addAttr(maxCommonModeInputVoltageNode, doc, "unit", "V");
	}

	if(hasMinCommonModeInputVoltage())
	{
		std::ostringstream minCommonModeInputVoltageString;
		minCommonModeInputVoltageString << getMinCommonModeInputVoltage();
		Core::XmlNode & minCommonModeInputVoltageNode = Core::RapidXmlUtils::addNode(performance, doc, "minCommonModeInputVoltage", minCommonModeInputVoltageString.str());
		Core::RapidXmlUtils::addAttr(minCommonModeInputVoltageNode, doc, "unit", "V");
	}

}


void Result::writeExpectedPerformance(std::ostream& stream) const
{
	if(gain_ != NOT_INITIALIZED_)
	{
		stream << "Expected Performance Values: " << std::endl;

		stream << "Gain: "<< getGain() << " dB" << std::endl;;
	}

	if(power_ != NOT_INITIALIZED_)
	{
		float normedPower = getPower() * 1000;
		stream << "Power consumption: " << normedPower << " mW" << std::endl;
	}

	if(area_ != NOT_INITIALIZED_)
	{
		stream  << "Area: " << getArea() << " (mu_m)^2" << std::endl;
	}

	if(transitFrequency_ != NOT_INITIALIZED_)
	{
		float normedTransitFrequency = getTransitFrequency()/1000000;
		stream << "Transit frequency: " << normedTransitFrequency << " MHz" << std::endl;
	}

	if(transitFrequencyWithErrorFactor_ != NOT_INITIALIZED_)
	{
		float normedTransitFrequency = getTransitFrequencyWithErrorFactor()/1000000;
		stream << "Transit frequency with error factor: " << normedTransitFrequency << " MHz" << std::endl;
	}

	if(slewRate_ != NOT_INITIALIZED_)
	{
		float normedSlewRate = getSlewRate()/ 1000000;
		stream << "Slew rate: " << normedSlewRate << " V/mu_s" << std::endl;
	}

	if(phaseMargin_ != NOT_INITIALIZED_)
	{
		float normedPhaseMargin = getPhaseMargin() * 180/3.14159265;
		stream << "Phase margin: " << normedPhaseMargin  << "°"  << std::endl;
	}

	if(CMRR_ != NOT_INITIALIZED_)
	{
		stream << "CMRR: " << getCMRR() << " dB" << std::endl;
	}

	if(negPSRR_ != NOT_INITIALIZED_)
	{
		stream << "negPSRR: " << getNegPSRR() << " dB" << std::endl;
	}

	if(posPSRR_ != NOT_INITIALIZED_)
	{
		stream << "posPSRR: " << getPosPSRR() << " dB" << std::endl;
	}

	if(voutMax_ != NOT_INITIALIZED_)
	{
		stream << "VoutMax: " << getMaxOutputVoltage() << " V" << std::endl;
	}

	if(voutMin_ != NOT_INITIALIZED_)
	{
	 stream << "VoutMin: " << getMinOutputVoltage() <<  " V" <<std::endl;
	}

	if(hasMaxCommonModeInputVoltage())
	{
		stream << "VcmMax: " << getMaxCommonModeInputVoltage() << " V" << std::endl;
	}

	if(hasMinCommonModeInputVoltage())
	{
		stream << "VcmMin: " << getMinCommonModeInputVoltage() << " V" << std::endl;
	}
	stream << std::endl;
}

void Result::writeVoltages(std::ostream& stream) const
{
	if(!voltageMap_.empty())
	{
		stream << "Voltages: " << std::endl;
		for(std::map<Core::NetId, float >::const_iterator it = voltageMap_.begin(); it != voltageMap_.end(); it++)
		{
			Core::NetId name = it->first;
			float voltage = it->second;
			stream << name << ": "<< voltage << " V" << std::endl;
		}
		stream << std::endl;
	}

}

void Result::writeCurrents(std::ostream & stream) const
{
	if(!currentMap_.empty())
	{
		stream << "Currents: " << std::endl;
		for(std::map<Core::DeviceId, float >::const_iterator it = currentMap_.begin(); it != currentMap_.end(); it++)
		{
		Core::DeviceId name = it->first;
		float current = it->second;
		float normedCurrent = current;
		stream << name << ": "<< normedCurrent << " muA" << std::endl;
		}
		stream << std::endl;
	}
}

void Result::writeXmlCurrents(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & currents = Core::RapidXmlUtils::addNode(node, doc, "Currents");
	Core::RapidXmlUtils::addAttr(currents, doc, "unit", "mu_A");


	for(std::map<Core::DeviceId, float >::const_iterator it = currentMap_.begin(); it != currentMap_.end(); it++)
	{
		Core::DeviceId name = it->first;
		float current = it->second;
		float normedCurrent = current;
		std::ostringstream currentString;
		currentString << normedCurrent;
		Core::XmlNode & component = Core::RapidXmlUtils::addNode(currents, doc, "Component", currentString.str());
		Core::RapidXmlUtils::addAttr(component, doc, "name", name.toStr());
	}
}

void Result::writeXmlVoltages(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & voltages = Core::RapidXmlUtils::addNode(node, doc, "Voltages");
	Core::RapidXmlUtils::addAttr(voltages, doc, "unit", "V");

	for(std::map<Core::NetId, float >::const_iterator it = voltageMap_.begin(); it != voltageMap_.end(); it++)
	{
		Core::NetId name = it->first;
		float voltage = it->second;
		std::ostringstream voltageString;
		voltageString << voltage;
		Core::XmlNode & net = Core::RapidXmlUtils::addNode(voltages, doc, "Net", voltageString.str());
		Core::RapidXmlUtils::addAttr(net, doc, "name", name.toStr());
	}
}

void Result::writeXmlComponentDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & dimensions = Core::RapidXmlUtils::addNode(node, doc, "Dimensions");
	writeXmlTransistorDimensions(dimensions, doc);
	writeXmlCapacitorDimensions(dimensions, doc);
}

void Result::writeXmlTransistorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & transistors = Core::RapidXmlUtils::addNode(node, doc, "Transistors");
	for(std::map<StructRec::StructureId,Partitioning::Transistor*>::const_iterator it = transistors_.begin(); it != transistors_.end(); it++)
	{
		Partitioning::Transistor * tran = it->second;
		Core::DeviceId name = tran->getDeviceId();
		Core::XmlNode & tranNode = Core::RapidXmlUtils::addNode(transistors, doc, "Transistor");
		Core::RapidXmlUtils::addAttr(tranNode, doc, "name", name.toStr());

		float width = tran->getWidth();
		std::ostringstream widthString;
		widthString << width;
		Core::XmlNode & widthNode = Core::RapidXmlUtils::addNode(tranNode, doc, "Width", widthString.str());
		Core::RapidXmlUtils::addAttr(widthNode, doc, "unit", "mu_m");

		float length = tran->getLength();
		std::ostringstream lengthString;
		lengthString << length;
		Core::XmlNode & lengthNode = Core::RapidXmlUtils::addNode(tranNode, doc, "Length", lengthString.str());
		Core::RapidXmlUtils::addAttr(lengthNode, doc, "unit", "mu_m");

	}

}
void Result::writeXmlCapacitorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const
{
	Core::XmlNode & capacitors = Core::RapidXmlUtils::addNode(node, doc, "Capacitors");
	for(std::map<StructRec::StructureId,Partitioning::TwoPort*>::const_iterator it =  twoPorts_.begin(); it != twoPorts_.end(); it++)
	{
		Partitioning::TwoPort* twoPort = it->second;
		Core::DeviceId name = twoPort->getDeviceId();
		Core::XmlNode & twoPortNode = Core::RapidXmlUtils::addNode(capacitors, doc, "Capacitor");
		Core::RapidXmlUtils::addAttr(twoPortNode, doc, "name", name.toStr());

		float value = twoPort->getValue();
		std::ostringstream valueString;
		valueString << value;
		Core::XmlNode & valueNode = Core::RapidXmlUtils::addNode(twoPortNode, doc, "Value", valueString.str() );
		Core::RapidXmlUtils::addAttr(valueNode, doc, "unit", "p_F");


	}
}


void Result::setMaxOutputVoltage(float voutMax)
{
	voutMax_ = voutMax;
}

void Result::setMinOutputVoltage(float voutMin)
{
	voutMin_ = voutMin;
}

void Result::setMaxCommonModeInputVoltage(float vcmMax)
{
	vcmMax_ = vcmMax;
}

void Result::setMinCommonModeInputVoltage(float vcmMin)
{
	vcmMin_ = vcmMin;
}

float Result::getMaxOutputVoltage() const
{
	assert( voutMax_ != NOT_INITIALIZED_);
	return voutMax_;
}

float Result::getMinOutputVoltage() const
{
	assert( voutMin_ != NOT_INITIALIZED_);
	return voutMin_;
}

float Result::getMaxCommonModeInputVoltage() const
{
	assert( vcmMax_ != NOT_INITIALIZED_ && vcmMax_ > -1000000);
	return vcmMax_;
}

float Result::getMinCommonModeInputVoltage() const
{
	assert( vcmMin_ != NOT_INITIALIZED_);
	return vcmMin_;
}


std::map<Core::DeviceId, float>  Result::getCurrentMap() const
{
	assert(!currentMap_.empty());
	return currentMap_;
}

std::map<Core::NetId, float>  Result::getVoltageMap() const
{
	assert(!voltageMap_.empty());
	return voltageMap_;
}


}

