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

#ifndef AUTOMATICSIZING_INCL_RESULTS_RESULT_H_
#define AUTOMATICSIZING_INCL_RESULTS_RESULT_H_


#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"

#include "Partitioning/incl/Results/TwoPort.h"
#include "Partitioning/incl/Results/Transistor.h"

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"

#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include "Control/incl/File/OutputFile.h"
#include <rapidxml_print.hpp>

namespace Partitioning {
 class Result;
 class Component;
}

namespace AutomaticSizing {

class Result : public Core::Object
{
public:
	Result();
	~Result();

	void initializeTwoPortMap(const Partitioning::Result & partitioningResult);
	void initializeTransistorMap(const Partitioning::Result & partitioningResult);

	void setGain(float gain);
	void setPower(float power);
	void setSlewRate(float slewRate);
	void setPhaseMargin(float phaseMargin);
	void setTransitFrequency(float transitFrequency);
	void setTransitFrequencyWithErrorFactor(float transitFrequency);
	void setArea(float area);
	void setCMRR(float CMRR);
	void setPosPSRR(float posPSRR);
	void setNegPSRR(float negPSRR);
	void setMaxOutputVoltage(float voutMax);
	void setMinOutputVoltage(float voutMin);
	void setMaxCommonModeInputVoltage(float vcmMax);
	void setMinCommonModeInputVoltage(float vcmMin);

	void addCurrent(Partitioning::Component & component, float current);
	void addVoltage(Core::NetId netId, float voltage);

	std::vector<Partitioning::Component*> findComponents(const StructRec::Structure & structure) const;
	Partitioning::Component & findComponent(StructRec::StructureId structureId) const;

	void writeXmlAutomaticSizingResult(Control::OutputFile & file) const;
	std::string toStr() const;

public:

	float getGain() const;
	float getPower() const;
	float getSlewRate() const;
	float getPhaseMargin() const;
	float getTransitFrequency() const;
	float getTransitFrequencyWithErrorFactor() const;
	float getArea() const;
	float getCMRR() const;

	float getNegPSRR() const;
	float getPosPSRR() const;
	float getMaxOutputVoltage() const;
	float getMinOutputVoltage() const;
	float getMaxCommonModeInputVoltage() const;
	float getMinCommonModeInputVoltage() const;

	std::vector<Partitioning::TwoPort*> getAllTwoPorts() const;
	std::vector<Partitioning::Transistor*> getAllTransistors() const;

	bool hasArea() const;
	bool hasPower() const;
	bool hasGain() const;
	bool hasSlewRate() const;
	bool hasTransitFrequency() const;
	bool hasTransitFrequencyWithErrorFactor() const;
	bool hasPhaseMargin() const;
	bool hasCMRR() const;
	bool hasNegPSRR() const;
	bool hasPosPSRR() const;
	bool hasMaxCommonModeInputVoltage() const;
	bool hasMinCommonModeInputVoltage() const;
	bool hasMaxOutputVoltage() const;
	bool hasMinOutputVoltage() const;


	std::map<Core::DeviceId, float>  getCurrentMap() const;
	std::map<Core::NetId, float>  getVoltageMap() const;

private:
	static const int NOT_INITIALIZED_;

	void writeXmlExpectedPerformance(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlCurrents(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlVoltages(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlComponentDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlCapacitorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;
	void writeXmlTransistorDimensions(Core::XmlNode & node, Core::XmlDocument & doc) const;

	void writeExpectedPerformance(std::ostream& stream) const;
	void writeVoltages(std::ostream& stream) const;
	void writeCurrents(std::ostream & stream) const;

	bool hasCapacity(Core::DeviceId deviceId) const;

private:
	std::map<StructRec::StructureId,Partitioning::Transistor*> transistors_;
	std::map<StructRec::StructureId,Partitioning::TwoPort*> twoPorts_;


	float gain_;
	float power_;
	float slewRate_;
	float phaseMargin_;
	float transitFrequency_;
	float transitFrequencyWithErrorFactor_;
	float area_;
	float CMRR_;
	float negPSRR_;
	float posPSRR_;
	float voutMax_;
	float voutMin_;
	float vcmMax_;
	float vcmMin_;

	std::map<Core::DeviceId, float> currentMap_;
	std::map<Core::NetId, float> voltageMap_;
};
}






#endif /* AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_RESULTS_RESULT_H_ */
