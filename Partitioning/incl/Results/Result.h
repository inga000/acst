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

#ifndef PARTITIONING_INCL_RESULTS_RESULT_H_
#define PARTITIONING_INCL_RESULTS_RESULT_H_

#include "Partitioning/incl/Results/TwoPort.h"
#include "Partitioning/incl/Results/Transistor.h"
#include "Partitioning/incl/Results/Component.h"

#include "Partitioning/incl/Parts/Part.h"
#include "Partitioning/incl/Parts/TransconductancePart.h"
#include "Partitioning/incl/Parts/BiasPart.h"
#include "Partitioning/incl/Parts/LoadPart.h"
#include "Partitioning/incl/Parts/CapacitancePart.h"
#include "Partitioning/incl/Parts/CommonModeSignalDetectorPart.h"
#include "Partitioning/incl/Parts/ResistorPart.h"
#include "Partitioning/incl/Parts/PositiveFeedbackPart.h"
#include "Partitioning/incl/Parts/UndefinedPart.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include "Control/incl/File/OutputFile.h"
#include <rapidxml_print.hpp>

namespace Partitioning {

class Result : public Core::Object
{
public:
	Result();
	~Result();

	void addTransconductancePart(TransconductancePart & part);
	void addLoadPart(LoadPart& part);
	void addBiasPart(BiasPart & part);
	void addCapacitancePart(CapacitancePart & part);
	void addCommonModeSignalDetectorPart(CommonModeSignalDetectorPart & detectPart);
	void addResistorPart(ResistorPart & resPart);
	void addUndefinedPart(UndefinedPart & part);
	void addPositiveFeedbackPart(PositiveFeedbackPart & part);

	void addTransistor(Transistor & transistor);
	void addTwoPort(TwoPort & twoPort);

	bool structureAlreadyClassified(const StructRec::Structure & structure) const;
	bool deviceAlreadyClassified(const Core::Device & device) const;

	bool isPartOfHelperStructureOfDifferentialPair(Component & component) const;
	bool isBiasOfFoldedPair(Component & component) const;
	bool isClassifiedVoltageBias(const StructRec::Structure & structure) const;
	bool isClassifiedCurrentBias(const StructRec::Structure & structure) const;
	bool isLoadPartOfFirstStage(const Part & part) const;
	bool isInFeedback(const Component & comp) const;
	bool isInOpAmp(const Component & comp) const;
	bool isInMainBias(const Component & comp) const;

	bool hasFirstStage() const;
	bool hasSecondStage() const;
	bool hasThirdStage() const;
	bool hasPrimarySecondStage() const;
	bool hasSecondarySecondStage() const;
	bool hasCompensationCapacitance() const;
	bool hasCompensationResistor() const;
	bool hasLoadCapacitance() const;
	bool hasFeedbackStage() const;
	bool hasFirstStageGainEnhancer() const;

	Part & getPart(const StructRec::Structure & structure );
	const Part & getPart(const StructRec::Structure & structure ) const;
	Part & getPart(const Core::Device & device);
	const Part & getPart(const Core::Device & device ) const;
	TransconductancePart & getTransconductancePart(const StructRec::Structure & structure);
	const TransconductancePart & getTransconductancePart(const StructRec::Structure & structure) const;
	BiasPart & getBiasPart(const StructRec::Structure & structure);
	const BiasPart & getBiasPart(const StructRec::Structure & structure) const;
	LoadPart & getLoadPart(const StructRec::Structure & structure);
	const LoadPart & getLoadPart(const StructRec::Structure & structure) const;
	CapacitancePart & getCapacitancePart(const StructRec::Structure & structure);
	const CapacitancePart & getCapacitancePart(const StructRec::Structure & structure) const;
	ResistorPart & getResistorPart(const StructRec::Structure &structure) const;
	const PositiveFeedbackPart & getPositiveFeedbackPart(const StructRec::Structure &structure) const;
	TransconductancePart & getFirstStage() const;
	TransconductancePart & getFeedbackStage() const;
	std::vector<TransconductancePart*> getSecondStages() const;
	TransconductancePart & getPrimarySecondStage() const;
	TransconductancePart & getSecondarySecondStage() const;
	TransconductancePart & getThirdStage() const;
	CapacitancePart & getCompensationCapacitance() const;
	std::vector<CapacitancePart*>  getCompensationCapacitors() const;
	ResistorPart & getCompensationResistor() const;
	std::vector<CapacitancePart*> getLoadCapacities() const;
	CapacitancePart & getLoadCapacity(Core::DeviceId deviceId) const;
	const PositiveFeedbackPart & getFirstStageGainEnhancer() const;

	Component & getBiasOfFoldedPair() const;
	bool hasBiasOfFoldedPair() const;

	std::vector<TransconductancePart *> getAllTransconductanceParts() const;
	std::vector<LoadPart*> getAllLoadParts() const;
	std::vector<BiasPart*> getAllBiasParts() const;
	std::vector<CapacitancePart*> getAllCapacitanceParts() const;
	std::vector<CommonModeSignalDetectorPart*> getAllCommonModeSignalDetectorParts() const;
	std::vector<ResistorPart*> getAllResistorParts() const;
	std::vector<Component*> getAllComponents() const;
	std::vector<BiasPart *> getAllVoltageBiases() const;
	std::vector<BiasPart*> getAllCurrentBiases() const;
	std::vector<PositiveFeedbackPart*> getAllPositiveFeedbackParts() const;
	std::vector<PositiveFeedbackPart*> getAllSlewRateEnhancer() const;
	std::vector<PositiveFeedbackPart*> getAllGainEnhancer() const;

	std::vector<TwoPort*> getAllTwoPorts() const;
	std::vector<Transistor*> getAllTransistors() const;


	std::vector<Component*> getBelongingComponents(const Part & Part) const;
	std::vector<Component*> findComponents(const StructRec::Structure & structure) const;
	Component & findComponent(StructRec::StructureId structureId) const;

	std::vector<Core::NetId> findBiasNets() const;

	bool hasTransconductanceParts() const;
	bool hasLoadParts() const;
	bool hasBiasParts() const;
	bool hasCapacitanceParts() const;
	bool hasCommonModeSignalDetectorParts() const;
	bool hasResistorParts() const;
	bool hasPositiveFeedbackParts() const;
	bool hasGainEnhancer() const;
	bool hasSlewRateEnhancer() const;

	void eraseDoubledSecondStage();

	std::string toStr() const;
	void writeXmlPartitioningResult(Control::OutputFile & file)const;

private:
	bool hasCapacity(Core::DeviceId deviceId) const;

	void eraseTransconductanceParts();
	void eraseLoadParts();
	void eraseBiasParts();
	void eraseCommonModeSignalDetectorParts();
	void erasePositiveFeedbackParts();
	void eraseResistorParts();
	void eraseCapacitanceParts();
	void eraseUndefinedParts();
	void eraseTransistors();
	void eraseTwoPorts();

private:
	std::map<PartId, TransconductancePart*> transconductanceParts_;
	std::map<PartId, BiasPart*> biasParts_;
	std::map<PartId, LoadPart*> loadParts_;
	std::map<PartId, CapacitancePart*> capacitanceParts_;
	std::map<PartId, CommonModeSignalDetectorPart*> commonModeSignalDetectorParts_;
	std::map<PartId, ResistorPart*> resistorParts_;
	std::map<PartId, PositiveFeedbackPart*> positiveFeedbackParts_;
	std::map<PartId, UndefinedPart*> undefinedParts_;

	std::map<StructRec::StructureId,Transistor*> transistors_;
	std::map<StructRec::StructureId,TwoPort*> twoPorts_;

};
}



#endif /* STRUCTUREDANALOGDESIGN_INCL_RESULTS_PARTITIONINGRESULT_H_ */
