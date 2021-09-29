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
#include "Synthesis/incl/Library/HierarchyLevel4/NonInvertingStages.h"
#include "Synthesis/incl/Library/HierarchyLevel3/AmplificationStagesSubBlockLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Transconductances.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Loads.h"
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"
#include "Synthesis/incl/Library/HierarchyLevel3/StageBiases.h"


#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/CircuitId/CircuitIds.h"
#include "Core/incl/Circuit/Net/NetId/NetName.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

#include "Core/incl/FlatCircuitRecursion/FlatCircuitRecursion.h"

#include <sstream>


namespace Synthesis {

    const Core::TerminalName NonInvertingStages::IN1_TERMINAL_ = Core::TerminalName("In1");
	const Core::TerminalName NonInvertingStages::IN2_TERMINAL_ = Core::TerminalName("In2");
	const Core::TerminalName NonInvertingStages::OUT1_TERMINAL_ = Core::TerminalName("Out1");
	const Core::TerminalName NonInvertingStages::OUT2_TERMINAL_ = Core::TerminalName("Out2");

	const Core::TerminalName NonInvertingStages::SOURCETRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("SourceTransconductance");
	const Core::TerminalName NonInvertingStages::SOURCETRANSCONDUCTANCE1_TERMINAL_ = Core::TerminalName("SourceTransconductance1");
	const Core::TerminalName NonInvertingStages::SOURCETRANSCONDUCTANCE2_TERMINAL_ = Core::TerminalName("SourceTransconductance2");
	const Core::TerminalName NonInvertingStages::INNERTRANSCONDUCTANCE_TERMINAL_ = Core::TerminalName("InnerTransconductance");
	const Core::TerminalName NonInvertingStages::SOURCETRANSCONDUCTANCEPMOS_TERMINAL_ = Core::TerminalName("SourceTransconductancePmos");
	const Core::TerminalName NonInvertingStages::SOURCETRANSCONDUCTANCENMOS_TERMINAL_ = Core::TerminalName("SourceTransconductanceNmos");

	const Core::TerminalName NonInvertingStages::SOURCENMOS_TERMINAL_ = Core::TerminalName("SourceNmos");
	const Core::TerminalName NonInvertingStages::SOURCEPMOS_TERMINAL_ = Core::TerminalName("SourcePmos");

	const Core::TerminalName NonInvertingStages::INPUTSTAGEBIAS_TERMINAL_ = Core::TerminalName("InputStageBias");
	const Core::TerminalName NonInvertingStages::INSOURCESTAGEBIAS_TERMINAL_ = Core::TerminalName("InSourceStageBias");
	const Core::TerminalName NonInvertingStages::INOUTPUTSTAGEBIAS_TERMINAL_ = Core::TerminalName("InOutputStageBias");
	const Core::TerminalName NonInvertingStages::INNERSTAGEBIAS_TERMINAL_ = Core::TerminalName("InnerStageBias");
	const Core::TerminalName NonInvertingStages::INNERSTAGEBIAS1_TERMINAL_ = Core::TerminalName("InnerStageBias1");
	const Core::TerminalName NonInvertingStages::INNERSTAGEBIAS2_TERMINAL_ = Core::TerminalName("InnerStageBias2");

	const Core::TerminalName NonInvertingStages::INPUTSTAGEBIASPMOS_TERMINAL_ = Core::TerminalName("InputStageBiasPmos");
	const Core::TerminalName NonInvertingStages::INSOURCESTAGEBIASPMOS_TERMINAL_ = Core::TerminalName("InSourceStageBiasPmos");
	const Core::TerminalName NonInvertingStages::INOUTPUTSTAGEBIASPMOS_TERMINAL_ = Core::TerminalName("InOutputStageBiasPmos");
	const Core::TerminalName NonInvertingStages::INNERSTAGEBIASPMOS_TERMINAL_ = Core::TerminalName("InnerStageBiasPmos");

	const Core::TerminalName NonInvertingStages::INPUTSTAGEBIASNMOS_TERMINAL_ = Core::TerminalName("InputStageBiasNmos");
	const Core::TerminalName NonInvertingStages::INSOURCESTAGEBIASNMOS_TERMINAL_ = Core::TerminalName("InSourceStageBiasNmos");
	const Core::TerminalName NonInvertingStages::INOUTPUTSTAGEBIASNMOS_TERMINAL_ = Core::TerminalName("InOutputStageBiasNmos");
	const Core::TerminalName NonInvertingStages::INNERSTAGEBIASNMOS_TERMINAL_ = Core::TerminalName("InnerStageBiasNmos");

	const Core::TerminalName NonInvertingStages::SOURCEGCC1_TERMINAL_ = Core::TerminalName("SourceGCC1");
	const Core::TerminalName NonInvertingStages::SOURCEGCC2_TERMINAL_ = Core::TerminalName("SourceGCC2");

	const Core::TerminalName NonInvertingStages::INNERLOAD1_TERMINAL_ = Core::TerminalName("InnerLoad1");
	const Core::TerminalName NonInvertingStages::INNERLOAD2_TERMINAL_ = Core::TerminalName("InnerLoad2");
	const Core::TerminalName NonInvertingStages::INNERGCC_TERMINAL_ = Core::TerminalName("InnerGCC");
	const Core::TerminalName NonInvertingStages::INNERBIASGCC_TERMINAL_ = Core::TerminalName("InnerBiasGCC");
	const Core::TerminalName NonInvertingStages::INNERSOURCELOAD1_TERMINAL_ = Core::TerminalName("InnerSourceLOad1");			
	const Core::TerminalName NonInvertingStages::INNEROUTPUTLOAD1_TERMINAL_ = Core::TerminalName("InnerOutputLoad1");
	const Core::TerminalName NonInvertingStages::INNERSOURCELOAD2_TERMINAL_ = Core::TerminalName("InnerSourceLoad2");
	const Core::TerminalName NonInvertingStages::INNEROUTPUTLOAD2_TERMINAL_ = Core::TerminalName("InnerOutputLoad2");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK1LOAD1_TERMINAL_ = Core::TerminalName("InnerTransistorStack1Load1");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK2LOAD1_TERMINAL_ = Core::TerminalName("InnerTransistorStack2Load1");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK1LOAD2_TERMINAL_ = Core::TerminalName("InnerTransistorStack1Load2");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK2LOAD2_TERMINAL_ = Core::TerminalName("InnerTransistorStack2Load2");

	const Core::TerminalName NonInvertingStages::INNERSOURCELOADNMOS_TERMINAL_ = Core::TerminalName("InnerSourceLoadNmos");			
	const Core::TerminalName NonInvertingStages::INNEROUTPUTLOADNMOS_TERMINAL_ = Core::TerminalName("InnerOutputLoadNmos");
	const Core::TerminalName NonInvertingStages::INNERSOURCELOADPMOS_TERMINAL_ = Core::TerminalName("InnerSourceLoadPmos");
	const Core::TerminalName NonInvertingStages::INNEROUTPUTLOADPMOS_TERMINAL_ = Core::TerminalName("InnerOutputLoadPmos");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK1LOADNMOS_TERMINAL_ = Core::TerminalName("InnerTransistorStack1LoadNmos");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK2LOADNMOS_TERMINAL_ = Core::TerminalName("InnerTransistorStack2LoadNmos");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK1LOADPMOS_TERMINAL_ = Core::TerminalName("InnerTransistorStack1LoadPmos");
	const Core::TerminalName NonInvertingStages::INNERTRANSISTORSTACK2LOADPMOS_TERMINAL_ = Core::TerminalName("InnerTransistorStack2LoadPmos");

	const Core::TerminalName NonInvertingStages::OUTOUTPUT1LOAD1_TERMINAL_ = Core::TerminalName("OutOutput1Load1");
	const Core::TerminalName NonInvertingStages::OUTOUTPUT2LOAD1_TERMINAL_ = Core::TerminalName("OutOutput2Load1");
	const Core::TerminalName NonInvertingStages::OUTSOURCE1LOAD1_TERMINAL_ = Core::TerminalName("OutSource1Load1");
    const Core::TerminalName NonInvertingStages::OUTSOURCE2LOAD1_TERMINAL_ = Core::TerminalName("OutSource2Load1");

    const Core::InstanceName NonInvertingStages::LOAD_ = Core::InstanceName("Load");
	const Core::InstanceName NonInvertingStages::TRANSCONDUCTANCE_ = Core::InstanceName("Transconductance");
	const Core::InstanceName NonInvertingStages::STAGEBIAS_ = Core::InstanceName("StageBias");
	const Core::InstanceName NonInvertingStages::STAGEBIAS1_ = Core::InstanceName("StageBias1");
	const Core::InstanceName NonInvertingStages::STAGEBIAS2_ = Core::InstanceName("StageBias2");
	const Core::InstanceName NonInvertingStages::STAGEBIASNMOS_ = Core::InstanceName("StageBiasNmos");
	const Core::InstanceName NonInvertingStages::STAGEBIASPMOS_ = Core::InstanceName("StageBiasPmos");

	const Core::NetId NonInvertingStages::IN1_NET_ = Core::NetName("in1").createRootIdentifier();
	const Core::NetId NonInvertingStages::IN2_NET_ = Core::NetName("in2").createRootIdentifier();
	const Core::NetId NonInvertingStages::OUT1_NET_ = Core::NetName("out1").createRootIdentifier();
	const Core::NetId NonInvertingStages::OUT2_NET_ = Core::NetName("out2").createRootIdentifier();

	const Core::NetId NonInvertingStages::SOURCETRANSCONDUCTANCE_NET_ = Core::NetName("sourceTransconductance").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCETRANSCONDUCTANCE1_NET_ = Core::NetName("sourceTransconductance1").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCETRANSCONDUCTANCE2_NET_ = Core::NetName("sourceTransconductance2").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSCONDUCTANCE_NET_ = Core::NetName("innerTransconductance").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCETRANSCONDUCTANCEPMOS_NET_ = Core::NetName("sourceTransconductancePmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCETRANSCONDUCTANCENMOS_NET_ = Core::NetName("sourceTransconductanceNmos").createRootIdentifier();

	const Core::NetId NonInvertingStages::SOURCENMOS_NET_ = Core::NetName("sourceNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCEPMOS_NET_ = Core::NetName("sourcePmos").createRootIdentifier();

	const Core::NetId NonInvertingStages::INPUTSTAGEBIAS_NET_ = Core::NetName("inputStageBias").createRootIdentifier();
	const Core::NetId NonInvertingStages::INSOURCESTAGEBIAS_NET_ = Core::NetName("inSourceStageBias").createRootIdentifier();
	const Core::NetId NonInvertingStages::INOUTPUTSTAGEBIAS_NET_ = Core::NetName("inOutputStageBias").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSTAGEBIAS_NET_ = Core::NetName("innerStageBias").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSTAGEBIAS1_NET_ = Core::NetName("innerStageBias1").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSTAGEBIAS2_NET_ = Core::NetName("innerStageBias2").createRootIdentifier();

	const Core::NetId NonInvertingStages::INPUTSTAGEBIASPMOS_NET_ = Core::NetName("inputStageBiasPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INSOURCESTAGEBIASPMOS_NET_ = Core::NetName("inSourceStageBiasPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INOUTPUTSTAGEBIASPMOS_NET_ = Core::NetName("inOutputStageBiasPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSTAGEBIASPMOS_NET_ = Core::NetName("innerStageBiasPmos").createRootIdentifier();

	const Core::NetId NonInvertingStages::INPUTSTAGEBIASNMOS_NET_ = Core::NetName("inputStageBiasNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INSOURCESTAGEBIASNMOS_NET_ = Core::NetName("inSourceStageBiasNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INOUTPUTSTAGEBIASNMOS_NET_ = Core::NetName("inOutputStageBiasNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSTAGEBIASNMOS_NET_ = Core::NetName("innerStageBiasNmos").createRootIdentifier();

	const Core::NetId NonInvertingStages::SOURCEGCC1_NET_ = Core::NetName("sourceGCC1").createRootIdentifier();
	const Core::NetId NonInvertingStages::SOURCEGCC2_NET_ = Core::NetName("sourceGCC2").createRootIdentifier();

	const Core::NetId NonInvertingStages::INNERLOAD1_NET_ = Core::NetName("innerLoad1").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERLOAD2_NET_ = Core::NetName("innerLoad2").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERGCC_NET_ = Core::NetName("innerGCC").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERBIASGCC_NET_ = Core::NetName("innerBiasGCC").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSOURCELOAD1_NET_ = Core::NetName("innerSourceLoad1").createRootIdentifier();			
	const Core::NetId NonInvertingStages::INNEROUTPUTLOAD1_NET_ = Core::NetName("innerOutputLoad1").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSOURCELOAD2_NET_ = Core::NetName("innerSourceLoad2").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNEROUTPUTLOAD2_NET_ = Core::NetName("innerOutputLoad2").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK1LOAD1_NET_ = Core::NetName("innerTransistorStack1Load1").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK2LOAD1_NET_ = Core::NetName("innerTransistorStack2Load1").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK1LOAD2_NET_ = Core::NetName("innerTransistorStack1Load2").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK2LOAD2_NET_ = Core::NetName("innerTransistorStack2Load2").createRootIdentifier();

	const Core::NetId NonInvertingStages::INNERSOURCELOADNMOS_NET_ = Core::NetName("innerSourceLoadNmos").createRootIdentifier();			
	const Core::NetId NonInvertingStages::INNEROUTPUTLOADNMOS_NET_ = Core::NetName("innerOutputLoadNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERSOURCELOADPMOS_NET_ = Core::NetName("innerSourceLoadPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNEROUTPUTLOADPMOS_NET_ = Core::NetName("innerOutputLoadPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK1LOADNMOS_NET_ = Core::NetName("innerTransistorStack1LoadNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK2LOADNMOS_NET_ = Core::NetName("innerTransistorStack2LoadNmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK1LOADPMOS_NET_ = Core::NetName("innerTransistorStack1LoadPmos").createRootIdentifier();
	const Core::NetId NonInvertingStages::INNERTRANSISTORSTACK2LOADPMOS_NET_ = Core::NetName("innerTransistorStack2LoadPmos").createRootIdentifier();

	const Core::NetId NonInvertingStages::OUTOUTPUT1LOAD1_NET_ = Core::NetName("outOutput1Load1").createRootIdentifier();
	const Core::NetId NonInvertingStages::OUTOUTPUT2LOAD1_NET_ = Core::NetName("outOutpu2tLoad1").createRootIdentifier();
	const Core::NetId NonInvertingStages::OUTSOURCE1LOAD1_NET_ = Core::NetName("outSource1Load1").createRootIdentifier();
    const Core::NetId NonInvertingStages::OUTSOURCE2LOAD1_NET_ = Core::NetName("outSource2Load1").createRootIdentifier();


	NonInvertingStages::NonInvertingStages(const AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, const AutomaticSizing::CircuitInformation & circuitInformation):
		amplificationStagesSubBlockLevel_(&amplificationStagesSubBlockLevel)
    {
		int indexPmos = 1;
		int indexNmos = 1;
		if(circuitInformation.getCircuitParameter().isFullyDifferential())
		{
			initializeFeedbackNonInvertingStages(amplificationStagesSubBlockLevel, indexPmos, indexNmos);
		}
    }
	
    NonInvertingStages::~NonInvertingStages()
    {
		eraseFeedbackNonInvertingStagesPmosTransconductance();
		eraseFeedbackNonInvertingStagesNmosTransconductance();
    }

	void NonInvertingStages::eraseTemporaryFunctionalblocks() const
	{
		for (const Core::Circuit* temporaryElement: temporaryNonInvertingStages_)
		{
			if(temporaryElement)
			{
				delete temporaryElement;
			}
		}
		getAmplificationStagesSubBlockLevel().getLoads().eraseTemporaryFunctionalblocks();
		getAmplificationStagesSubBlockLevel().getLoadParts().eraseTemporaryFunctionalblocks();
	}

	const AmplificationStagesSubBlockLevel & NonInvertingStages::getAmplificationStagesSubBlockLevel() const
	{
		assert(amplificationStagesSubBlockLevel_ != nullptr);
		return * amplificationStagesSubBlockLevel_;
	}

	std::vector<const Core::Circuit *> NonInvertingStages::getTemporaryNonInvertingStages() const
	{
		return temporaryNonInvertingStages_;
	}


    std::vector<const Core::Circuit *> NonInvertingStages::createSimpleNonInvertingStages(int caseNumber) const
    {
    	eraseTemporaryFunctionalblocks();
    	int index = 1;
		std::vector<const Core::Circuit*> nonInvertingStages;

		switch(caseNumber) {
		case 1: { // PmosTransconductance SimpleMixedLoadNmos, OneTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleMixedLoadNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(), index);
			break;
		}
		case 2: { // NmosTransconductance SimpleMixedLoadPmos, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleMixedLoadPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(), index);
			break;
		}
		case 3: { // PmosTransconductance SimpleMixedLoadNmos, TwoTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleMixedLoadNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(), index);
			break;
		}
		case 4: { // NmosTransconductance SimpleMixedLoadPmos, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleMixedLoadPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(), index);
			break;
		}
		case 5: {// PmosTransconductance SimpleTwoLoadPartsFoldedGCCMixedLoadNmos, OneTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleTwoLoadPartsFoldedGCCMixedLoadNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(), index);
			break;
		}
		case 6: {// NmosTransconductance SimpleTwoLoadPartsFoldedGCCMixedLoadPmos, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleTwoLoadPartsFoldedGCCMixedLoadPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 7: {// PmosTransconductance SimpleTwoLoadPartsFoldedGCCMixedLoadNmos, TwoTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleTwoLoadPartsFoldedGCCMixedLoadNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 8: {// NmosTransconductance SimpleTwoLoadPartsFoldedGCCMixedLoadPmos, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createSimpleTwoLoadPartsFoldedGCCMixedLoadPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 9: {// PmosTransconductance TwoLoadPartsCascodeGCCMixed/twoLoadPartLoadsMixedCascodePmosGCC, OneTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsCascodeGCCMixedPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 10: {// NmosTransconductance TwoLoadPartsCascodeGCCMixedNmos, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsCascodeGCCMixedNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 11: {// PmosTransconductance TwoLoadPartsCascodeGCCMixedPmos, TwoTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsCascodeGCCMixedPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 12: {// NmosTransconductance TwoLoadPartsCascodeGCCMixedNmos, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsCascodeGCCMixedNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 13: {// PmosTransconductance TwoLoadPartsMixedCurrentBiasesPmos, OneTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsMixedCurrentBiasesPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 14: {// NmosTransconductance TwoLoadPartsMixedCurrentBiasesNmos, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsMixedCurrentBiasesNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 15: {// PmosTransconductance TwoLoadPartsMixedCurrentBiasesPmos, TwoTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsMixedCurrentBiasesPmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 16: {// NmosTransconductance TwoLoadPartsMixedCurrentBiasesNmos, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsTwoLoadPartsMixedCurrentBiasesNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 17: {
			nonInvertingStages.clear();
			break;
		}
		default: {
			logDebug("default");
			break;
		}
		}
		getTemporaryNonInvertingStages() = nonInvertingStages;
		return nonInvertingStages;
	}



	std::vector<const Core::Circuit *> NonInvertingStages::createFullyDifferentialNonInvertingStages(int caseNumber) const
	{
		eraseTemporaryFunctionalblocks();
		int index = 1;
		std::vector<const Core::Circuit*> nonInvertingStages;

		switch(caseNumber) {
		case 1: { // PmosTransconductance, OneTransistorStageBiasesPmos.
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosForFullyDifferentialNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(), index);

			break;
		}
		case 2: { // NmosTransconductance, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosForFullyDifferentialNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(), index);

			break;
		}
		case 3: { // PmosTransconductance, TwoTransistorStageBiasesPmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosForFullyDifferentialNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(), index);
			break;
		}
		case 4: { // NmosTransconductance, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosForFullyDifferentialNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(), index);
			break;
		}
		case 5: {
			nonInvertingStages.clear();
			break;
		}
		default: {
			logDebug("default");
			break;
		}
		}
		getTemporaryNonInvertingStages() = nonInvertingStages;
		return nonInvertingStages;
	}


	std::vector<const Core::Circuit *> NonInvertingStages::createComplementaryNonInvertingStages(int caseNumber) const
	{
		eraseTemporaryFunctionalblocks();
		std::vector<const Core::Circuit*> nonInvertingStages;

		switch(caseNumber) {
		case 1: { // PmosTransconductance OneCurrentBiasesLoadPartNmos, OneTransistorStageBiasesPmos.
			nonInvertingStages = createComplementaryTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getComplementaryTransconductance(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsForComplementaryNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos());
			break;
		}
		case 2: { // NmosTransconductance OneCurrentBiasesLoadPartPmos, OneTransistorStageBiasesNmos
			nonInvertingStages = createComplementaryTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getComplementaryTransconductance(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsForComplementaryNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos());
			break;
		}
		case 3: {
			nonInvertingStages.clear();
			break;
		}
		default: {
			logDebug("default");
			break;
		}
		}
		getTemporaryNonInvertingStages() = nonInvertingStages;
		return nonInvertingStages;
	}

	std::vector<const Core::Circuit *> NonInvertingStages::createSymmetricalNonInvertingStages(int caseNumber) const
	{
		eraseTemporaryFunctionalblocks();
		int index = 1;
		std::vector<const Core::Circuit*> nonInvertingStages;

		switch(caseNumber) {
		case 1: { // PmosTransconductance LoadsNmosTwoForSymmetricalOpAmpNonInvertingStage, OneTransistorStageBiasesPmos.
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosTwoForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 2: { // NmosTransconductance LoadsPmosTwoForSymmetricalOpAmpNonInvertingStage, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosTwoForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 3: { // PmosTransconductance LoadsNmosTwoForSymmetricalOpAmpNonInvertingStage, TwoTransistorStageBiasesPmos.
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosTwoForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 4: { // NmosTransconductance LoadsPmosTwoForSymmetricalOpAmpNonInvertingStage, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosTwoForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 5: { // PmosTransconductance LoadsNmosTwoForSymmetricalOpAmpNonInvertingStage, OneTransistorStageBiasesPmos.
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosFourForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 6: { // NmosTransconductance LoadsPmosTwoForSymmetricalOpAmpNonInvertingStage, OneTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosFourForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getOneTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 7: { // PmosTransconductance LoadsNmosTwoForSymmetricalOpAmpNonInvertingStage, TwoTransistorStageBiasesPmos.
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductancePmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsNmosFourForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesPmos(),
					index);
			break;
		}
		case 8: { // NmosTransconductance LoadsPmosTwoForSymmetricalOpAmpNonInvertingStage, TwoTransistorStageBiasesNmos
			nonInvertingStages = createSimpleTransconductanceNonInvertingStages(getAmplificationStagesSubBlockLevel().getTransconductances().getSimpleTransconductanceNmos(),
					getAmplificationStagesSubBlockLevel().getLoads().createLoadsPmosFourForSymmetricalOpAmpNonInvertingStage(),
					getAmplificationStagesSubBlockLevel().getStageBiases().getTwoTransistorStageBiasesNmos(),
					index);
			break;
		}
		case 9: {
			nonInvertingStages.clear();
			break;
		}
		default: {
			logDebug("default");
			break;
		}
		}
		getTemporaryNonInvertingStages() = nonInvertingStages;
		return nonInvertingStages;
	}

	std::vector<const Core::Circuit *> NonInvertingStages::getFeedbackNonInvertingStagesPmosTransconductance() const
	{
		assert(!feedbackNonInvertingStagesPmosTransconductance_.empty());
		return feedbackNonInvertingStagesPmosTransconductance_;
	}
    
	std::vector<const Core::Circuit *> NonInvertingStages::getFeedbackNonInvertingStagesNmosTransconductance() const
	{
		assert(!feedbackNonInvertingStagesNmosTransconductance_.empty());
		return feedbackNonInvertingStagesNmosTransconductance_;
	}
			

	std::string NonInvertingStages::toStr() const
    {
        std::ostringstream oss;
		Core::FlatCircuitRecursion * flatCircuitRecursion = new Core::FlatCircuitRecursion;
        oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<< NonInvertingStages: " << std::endl;

		oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<SIMPLE FIRST STAGE" << std::endl;

		int caseNumber = 1;
		std::vector<const Core::Circuit*> nonInvertingStages;
		do
		{
			nonInvertingStages = createSimpleNonInvertingStages(caseNumber);
			for(auto & stage : nonInvertingStages)
			{
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;
				Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
				oss << stage->toStr() << std::endl;
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
				oss << flatCircuit->toStr() << std::endl;
				delete flatCircuit;
			}
        	caseNumber++;
		} while (!nonInvertingStages.empty());

		caseNumber = 1;
		do
		{
			nonInvertingStages = createComplementaryNonInvertingStages(caseNumber);
			for(auto & stage : nonInvertingStages)
			{
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;
				Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
				oss << stage->toStr() << std::endl;
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
				oss << flatCircuit->toStr() << std::endl;
				delete flatCircuit;
			}
        	caseNumber++;
		} while (!nonInvertingStages.empty());

       	oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FEEDBACK STAGE" << std::endl;
        oss << "PMOS TRANSCONDUCTANCE" << std::endl;
        for(auto & stage : feedbackNonInvertingStagesPmosTransconductance_)
        {
			oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
            oss << stage->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }

        oss << "NMOS TRANSCONDUCTANCE" << std::endl;
        for(auto & stage : feedbackNonInvertingStagesNmosTransconductance_)
        {
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;      
            Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
            oss << stage->toStr() << std::endl;
            oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            oss << flatCircuit->toStr() << std::endl;
            delete flatCircuit;
        }

		caseNumber = 1;
		do
		{
			nonInvertingStages = createFullyDifferentialNonInvertingStages(caseNumber);
			for(auto & stage : nonInvertingStages)
			{
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;
				Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
				oss << stage->toStr() << std::endl;
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
				oss << flatCircuit->toStr() << std::endl;
				delete flatCircuit;
			}
        	caseNumber++;
		} while (!nonInvertingStages.empty());

		caseNumber = 1;
		do
		{
			nonInvertingStages = createSymmetricalNonInvertingStages(caseNumber);
			for(auto & stage : nonInvertingStages)
			{
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << stage->getCircuitIdentifier().toStr() << ": " << std::endl;
				Core::Circuit * flatCircuit = flatCircuitRecursion->createNewFlatCopy(*stage);
				oss << stage->toStr() << std::endl;
				oss << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FLATCIRCUIT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
				oss << flatCircuit->toStr() << std::endl;
				delete flatCircuit;
			}
        	caseNumber++;
		} while (!nonInvertingStages.empty());

		delete flatCircuitRecursion;
        return oss.str();
    }

	void NonInvertingStages::initializeFeedbackNonInvertingStages(const AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel, int & indexPmos, int & indexNmos)
	{
		const Core::Circuit & transconductanceNmos = amplificationStagesSubBlockLevel.getTransconductances().getFeedbackTransconductanceNmos();
		const Core::Circuit & transconductancePmos = amplificationStagesSubBlockLevel.getTransconductances().getFeedbackTransconductancePmos();

		std::vector<const Core::Circuit *> loadsPmos = amplificationStagesSubBlockLevel.getLoads().getLoadsPmosForFeedbackNonInvertingStage();
		std::vector<const Core::Circuit *> loadsNmos = amplificationStagesSubBlockLevel.getLoads().getLoadsNmosForFeedbackNonInvertingStage();

		std::vector<const Core::Circuit *> stageBiasesNmos = amplificationStagesSubBlockLevel.getStageBiases().getAllStageBiasesNmos();
		std::vector<const Core::Circuit *> stageBiasesPmos = amplificationStagesSubBlockLevel.getStageBiases().getAllStageBiasesPmos();

		feedbackNonInvertingStagesPmosTransconductance_ = createFeedbackTransconductanceNonInvertingStages(transconductancePmos, loadsNmos, stageBiasesPmos, indexPmos);
		feedbackNonInvertingStagesNmosTransconductance_ = createFeedbackTransconductanceNonInvertingStages(transconductanceNmos, loadsPmos, stageBiasesNmos, indexNmos);
}

	std::vector<const Core::Circuit*> NonInvertingStages::createSimpleTransconductanceNonInvertingStages(const Core::Circuit & transconductance,
								std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiases, int & index) const
	{
		std::vector<const Core::Circuit *> nonInvertingStages;
		for(auto & load : loads)
		{
			for(auto & stageBias : stageBiases)
			{
				Core::Instance & transconductanceInstance = createInstance(transconductance, TRANSCONDUCTANCE_);
				Core::Instance & loadInstance = createInstance(*load, LOAD_);
				Core::Instance & stageBiasInstance = createInstance(*stageBias, STAGEBIAS_);
				const Core::Circuit & nonInvertingStage = createSimpleTransconductanceNonInvertingStage(transconductanceInstance, loadInstance, stageBiasInstance, index);

				nonInvertingStages.push_back(&nonInvertingStage);
				index++;
			}
		}
		return nonInvertingStages;
	}

	std::vector<const Core::Circuit*> NonInvertingStages::createFeedbackTransconductanceNonInvertingStages(const Core::Circuit & transconductance, 
								std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiases, int & index) const ///
	{
		std::vector<const Core::Circuit *> nonInvertingStages;

		for(auto & load : loads)
		{
			for(auto & stageBias : stageBiases)
			{
				Core::Instance & transconductanceInstance = createInstance(transconductance, TRANSCONDUCTANCE_);
				Core::Instance & loadInstance = createInstance(*load, LOAD_);
				Core::Instance & stageBias1Instance = createInstance(*stageBias, STAGEBIAS1_);
				Core::Instance & stageBias2Instance = createInstance(*stageBias, STAGEBIAS2_);
				const Core::Circuit & nonInvertingStage = createFeedbackTransconductanceNonInvertingStage(transconductanceInstance, loadInstance, stageBias1Instance, stageBias2Instance, index);
				if(nonInvertingStage.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
				{
					nonInvertingStages.push_back(&nonInvertingStage);
					index++;
				}
				else
				{
					delete &nonInvertingStage;
				}

			}
		}
		return nonInvertingStages;
	}

	std::vector<const Core::Circuit*> NonInvertingStages::createComplementaryTransconductanceNonInvertingStages(const Core::Circuit & transconductance,
								std::vector<const Core::Circuit*> loads, std::vector<const Core::Circuit*> stageBiasesNmos,
								std::vector<const Core::Circuit*> stageBiasesPmos) const
	{
		std::vector<const Core::Circuit *> nonInvertingStages;
		int index = 1;

		for(auto & load : loads)
		{
			std::vector<const Core::Circuit*>::const_iterator pmosIterator = stageBiasesPmos.begin();
			std::vector<const Core::Circuit*>::const_iterator nmosIterator = stageBiasesNmos.begin();

			while(pmosIterator != stageBiasesPmos.end())
			{
				const Core::Circuit & stageBiasPmos = **pmosIterator;
				const Core::Circuit & stageBiasNmos = **nmosIterator;

				Core::Instance & transconductanceInstance = createInstance(transconductance, TRANSCONDUCTANCE_);
				Core::Instance & loadInstance = createInstance(*load, LOAD_);
				Core::Instance & stageBiasNmosInstance = createInstance(stageBiasNmos, STAGEBIASNMOS_);
				Core::Instance & stageBiasPmosInstance = createInstance(stageBiasPmos, STAGEBIASPMOS_);
				const Core::Circuit & nonInvertingStage = createComplementaryTransconductanceNonInvertingStage(transconductanceInstance, loadInstance, stageBiasNmosInstance, stageBiasPmosInstance, index);
				if(nonInvertingStage.everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType())
				{
					nonInvertingStages.push_back(&nonInvertingStage);
					index++;
				}
				else
				{
					delete &nonInvertingStage;
				}
				pmosIterator++;
				nmosIterator++;

			}
		}
		return nonInvertingStages;
	}

	const Core::Circuit& NonInvertingStages::createSimpleTransconductanceNonInvertingStage(Core::Instance & transconductance,
								Core::Instance & load, Core::Instance & stageBias, int index) const
	{
		Core::Circuit * nonInvertingStage = new Core::Circuit;

        Core::CircuitIds circuitIds;
        Core::CircuitId nonInvertingStageId = circuitIds.firstStage(index);
        nonInvertingStageId.setTechType(transconductance.getMaster().getCircuitIdentifier().getTechType());
		nonInvertingStage->setCircuitIdentifier(nonInvertingStageId);

		std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

		netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
		netNames.push_back(IN1_NET_);
		netNames.push_back(IN2_NET_);
		netNames.push_back(SOURCETRANSCONDUCTANCE_NET_);
		netNames.push_back(SOURCEPMOS_NET_);
		netNames.push_back(SOURCENMOS_NET_);

        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN1_TERMINAL_, IN1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN2_TERMINAL_, IN2_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCETRANSCONDUCTANCE_TERMINAL_, SOURCETRANSCONDUCTANCE_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));

		addStageBiasNets(netNames, terminalToNetMap, stageBias);
		addLoadNets(netNames, terminalToNetMap, load);


		addNetsToCircuit(*nonInvertingStage, netNames);
        addTerminalsToCircuit(*nonInvertingStage, terminalToNetMap);
        nonInvertingStage->addInstance(load);
		nonInvertingStage->addInstance(transconductance);
		nonInvertingStage->addInstance(stageBias);


        connectInstanceTerminalsOfSimpleTransconductance(*nonInvertingStage, transconductance);
		connectInstanceTerminalsOfLoad(*nonInvertingStage, load);
		connectInstanceTerminalsOfStageBias(*nonInvertingStage, stageBias);

        load.setCircuit(*nonInvertingStage);
		transconductance.setCircuit(*nonInvertingStage);
		stageBias.setCircuit(*nonInvertingStage);

        return *nonInvertingStage;
	}

	const Core::Circuit& NonInvertingStages::createFeedbackTransconductanceNonInvertingStage(Core::Instance & transconductance, // feedback
								Core::Instance & load, Core::Instance & stageBias1, Core::Instance & stageBias2, int index) const ///
	{
		Core::Circuit * nonInvertingStage = new Core::Circuit;
		
        Core::CircuitIds circuitIds;
        Core::CircuitId nonInvertingStageId = circuitIds.feedbackStage(index);
        nonInvertingStageId.setTechType(transconductance.getMaster().getCircuitIdentifier().getTechType());
		nonInvertingStage->setCircuitIdentifier(nonInvertingStageId);

		nonInvertingStage->addInstance(load);
		nonInvertingStage->addInstance(transconductance);
		nonInvertingStage->addInstance(stageBias1);
		nonInvertingStage->addInstance(stageBias2);

		std::vector<Core::NetId> netNames;
        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

		netNames.push_back(OUT1_NET_);
        netNames.push_back(OUT2_NET_);
		netNames.push_back(IN1_NET_);
		netNames.push_back(IN2_NET_);
		netNames.push_back(SOURCETRANSCONDUCTANCE1_NET_);
		netNames.push_back(SOURCETRANSCONDUCTANCE2_NET_);
		netNames.push_back(INNERTRANSCONDUCTANCE_NET_);
		netNames.push_back(SOURCEPMOS_NET_);
		netNames.push_back(SOURCENMOS_NET_);

        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN1_TERMINAL_, IN1_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN2_TERMINAL_, IN2_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCETRANSCONDUCTANCE1_TERMINAL_, SOURCETRANSCONDUCTANCE1_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCETRANSCONDUCTANCE2_TERMINAL_, SOURCETRANSCONDUCTANCE2_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSCONDUCTANCE_TERMINAL_, INNERTRANSCONDUCTANCE_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));
		
		addStageBiasNets(netNames, terminalToNetMap, stageBias1);
		addLoadNets(netNames, terminalToNetMap, load);
		

		addNetsToCircuit(*nonInvertingStage, netNames);
        addTerminalsToCircuit(*nonInvertingStage, terminalToNetMap);

        
        connectInstanceTerminalsOfFeedbackTransconductance(*nonInvertingStage, transconductance);
		connectInstanceTerminalsOfLoad(*nonInvertingStage, load);
		connectInstanceTerminalsOfStageBiases(*nonInvertingStage, stageBias1, stageBias2);

        load.setCircuit(*nonInvertingStage);
		transconductance.setCircuit(*nonInvertingStage);
		stageBias1.setCircuit(*nonInvertingStage);
		stageBias2.setCircuit(*nonInvertingStage);

        return *nonInvertingStage;
	}

	const Core::Circuit& NonInvertingStages::createComplementaryTransconductanceNonInvertingStage(Core::Instance & transconductance,
									Core::Instance & load, Core::Instance & stageBiasNmos, Core::Instance & stageBiasPmos, int index) const
		{
			Core::Circuit * nonInvertingStage = new Core::Circuit;

	        Core::CircuitIds circuitIds;
	        Core::CircuitId nonInvertingStageId = circuitIds.firstStage(index);
	        nonInvertingStageId.setTechType(transconductance.getMaster().getCircuitIdentifier().getTechType());
			nonInvertingStage->setCircuitIdentifier(nonInvertingStageId);

			std::vector<Core::NetId> netNames;
	        std::map<Core::TerminalName, Core::NetId> terminalToNetMap;

			netNames.push_back(OUT1_NET_);
	        netNames.push_back(OUT2_NET_);
			netNames.push_back(IN1_NET_);
			netNames.push_back(IN2_NET_);
			netNames.push_back(SOURCETRANSCONDUCTANCENMOS_NET_);
			netNames.push_back(SOURCETRANSCONDUCTANCEPMOS_NET_);
			netNames.push_back(SOURCEPMOS_NET_);
			netNames.push_back(SOURCENMOS_NET_);

	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT1_TERMINAL_, OUT1_NET_));
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUT2_TERMINAL_, OUT2_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN1_TERMINAL_, IN1_NET_));
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(IN2_TERMINAL_, IN2_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCETRANSCONDUCTANCENMOS_TERMINAL_, SOURCETRANSCONDUCTANCENMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCETRANSCONDUCTANCEPMOS_TERMINAL_, SOURCETRANSCONDUCTANCEPMOS_NET_));
	        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCENMOS_TERMINAL_, SOURCENMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEPMOS_TERMINAL_, SOURCEPMOS_NET_));

			addStageBiasesNets(netNames, terminalToNetMap, stageBiasNmos, stageBiasPmos);
			addComplementaryLoadNets(netNames, terminalToNetMap, load);


			addNetsToCircuit(*nonInvertingStage, netNames);
	        addTerminalsToCircuit(*nonInvertingStage, terminalToNetMap);
	        nonInvertingStage->addInstance(load);
			nonInvertingStage->addInstance(transconductance);
			nonInvertingStage->addInstance(stageBiasNmos);
			nonInvertingStage->addInstance(stageBiasPmos);

	        connectInstanceTerminalsOfComplementaryTransconductance(*nonInvertingStage, transconductance);
			connectInstanceTerminalsOfComplementaryLoad(*nonInvertingStage, load);
			connectInstanceTerminalsOfComplementaryStageBiases(*nonInvertingStage, stageBiasNmos, stageBiasPmos);

	        load.setCircuit(*nonInvertingStage);
			transconductance.setCircuit(*nonInvertingStage);
			stageBiasNmos.setCircuit(*nonInvertingStage);
			stageBiasPmos.setCircuit(*nonInvertingStage);

	        return *nonInvertingStage;
		}

	void NonInvertingStages::addLoadNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, // feedback
								Core::Instance & load) const ///
	{
		addLoadPart1Nets(netNames, terminalToNetMap, load);
		if(load.getMaster().findInstances().size() ==2)
		{
			addLoadPart2Nets(netNames, terminalToNetMap, load);
		}
	}	


	void NonInvertingStages::addLoadPart1Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, // feedback
								Core::Instance & load) const
	{
		const Core::Instance & loadPart1 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART1_));
		if(bothTransistorStacksAreVoltageBiases(loadPart1))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				netNames.push_back(OUTOUTPUT1LOAD1_NET_);
            	netNames.push_back(OUTOUTPUT2LOAD1_NET_);
            	netNames.push_back(OUTSOURCE1LOAD1_NET_);
            	netNames.push_back(OUTSOURCE2LOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT1LOAD1_TERMINAL_, OUTOUTPUT1LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTOUTPUT2LOAD1_TERMINAL_, OUTOUTPUT2LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE1LOAD1_TERMINAL_, OUTSOURCE1LOAD1_NET_));
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(OUTSOURCE2LOAD1_TERMINAL_, OUTSOURCE2LOAD1_NET_));
			}
		}
		else if(hasGCC(load))
		{
			netNames.push_back(SOURCEGCC1_NET_);
        	netNames.push_back(SOURCEGCC2_NET_);
			netNames.push_back(INNERGCC_NET_);
        	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEGCC1_TERMINAL_, SOURCEGCC1_NET_));
        	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(SOURCEGCC2_TERMINAL_, SOURCEGCC2_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERGCC_TERMINAL_, INNERGCC_NET_));

			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
       		
				netNames.push_back(INNERBIASGCC_NET_);
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERBIASGCC_TERMINAL_, INNERBIASGCC_NET_));
			}
		}
		else
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() == 2)
			{
				netNames.push_back(INNERLOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERLOAD1_TERMINAL_, INNERLOAD1_NET_));
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
        		netNames.push_back(INNERSOURCELOAD1_NET_);
        		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOAD1_TERMINAL_, INNERSOURCELOAD1_NET_));

        		if(isSingleDiodeTransistor(loadPart1.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       			{
            		netNames.push_back(INNEROUTPUTLOAD1_NET_);
            		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_));
        		}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				netNames.push_back(INNEROUTPUTLOAD1_NET_);
            	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_));
			}
		}
		if(!hasGCC(load))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				netNames.push_back(INNERTRANSISTORSTACK2LOAD1_NET_);
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOAD1_TERMINAL_, INNERTRANSISTORSTACK2LOAD1_NET_));
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				netNames.push_back(INNERTRANSISTORSTACK1LOAD1_NET_);
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOAD1_TERMINAL_, INNERTRANSISTORSTACK1LOAD1_NET_));
			}
		}
	}

	void NonInvertingStages::addLoadPart2Nets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, // feedback
											Core::Instance & load) const
	{
		const Core::Instance & loadPart2 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART2_));
		
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() == 2)
		{
			netNames.push_back(INNERLOAD2_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERLOAD2_TERMINAL_, INNERLOAD2_NET_));
		}
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 2)
		{
        	netNames.push_back(INNERSOURCELOAD2_NET_);
			netNames.push_back(INNERTRANSISTORSTACK2LOAD2_NET_);
        	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOAD2_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOAD2_TERMINAL_, INNERTRANSISTORSTACK2LOAD2_NET_));

        	if(isSingleDiodeTransistor(loadPart2.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       		{
            		netNames.push_back(INNEROUTPUTLOAD2_NET_);
            		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_));
        	}
		}
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 3)
		{
			netNames.push_back(INNEROUTPUTLOAD2_NET_);
			netNames.push_back(INNERTRANSISTORSTACK1LOAD2_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOAD2_TERMINAL_, INNERTRANSISTORSTACK1LOAD2_NET_));
		}


	}	

	void NonInvertingStages::addStageBiasNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap, // feedback
								Core::Instance & stageBias) const
	{
		if(getDeviceNamesOfFlatCircuit(stageBias.getMaster()).size() == 1)
		{
			netNames.push_back(INPUTSTAGEBIAS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUTSTAGEBIAS_TERMINAL_, INPUTSTAGEBIAS_NET_));
		}
		else
		{
			netNames.push_back(INOUTPUTSTAGEBIAS_NET_);
			netNames.push_back(INSOURCESTAGEBIAS_NET_);
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCESTAGEBIAS_TERMINAL_, INSOURCESTAGEBIAS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUTSTAGEBIAS_TERMINAL_, INOUTPUTSTAGEBIAS_NET_));
			
			if(stageBias.getIdentifier() == createInstanceId(STAGEBIAS_))
			{
				netNames.push_back(INNERSTAGEBIAS_NET_);
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIAS_TERMINAL_, INNERSTAGEBIAS_NET_));
			}
			else
			{
				netNames.push_back(INNERSTAGEBIAS1_NET_);
				netNames.push_back(INNERSTAGEBIAS2_NET_);
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIAS1_TERMINAL_, INNERSTAGEBIAS1_NET_));
				terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIAS2_TERMINAL_, INNERSTAGEBIAS2_NET_));
			}
		}
	}

	void NonInvertingStages::addComplementaryLoadNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
								Core::Instance & load) const
	{
		netNames.push_back(INNEROUTPUTLOADNMOS_NET_);
        netNames.push_back(INNERSOURCELOADNMOS_NET_);
		netNames.push_back(INNEROUTPUTLOADPMOS_NET_);
        netNames.push_back(INNERSOURCELOADPMOS_NET_);
		netNames.push_back(INNERTRANSISTORSTACK1LOADNMOS_NET_);
		netNames.push_back(INNERTRANSISTORSTACK2LOADNMOS_NET_);
		netNames.push_back(INNERTRANSISTORSTACK1LOADPMOS_NET_);
		netNames.push_back(INNERTRANSISTORSTACK2LOADPMOS_NET_);
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOADNMOS_TERMINAL_, INNEROUTPUTLOADNMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOADNMOS_TERMINAL_, INNERSOURCELOADNMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNEROUTPUTLOADPMOS_TERMINAL_, INNEROUTPUTLOADPMOS_NET_));
        terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSOURCELOADPMOS_TERMINAL_, INNERSOURCELOADPMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOADNMOS_TERMINAL_, INNERTRANSISTORSTACK1LOADNMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOADNMOS_TERMINAL_, INNERTRANSISTORSTACK2LOADNMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK1LOADPMOS_TERMINAL_, INNERTRANSISTORSTACK1LOADPMOS_NET_));
		terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERTRANSISTORSTACK2LOADPMOS_TERMINAL_, INNERTRANSISTORSTACK2LOADPMOS_NET_));
	}

	void NonInvertingStages::addStageBiasesNets(std::vector<Core::NetId> & netNames, std::map<Core::TerminalName, Core::NetId> & terminalToNetMap,
								Core::Instance & stageBiasNmos, Core::Instance& stageBiasPmos) const
	{
		if(getDeviceNamesOfFlatCircuit(stageBiasPmos.getMaster()).size() == 1)
		{
			netNames.push_back(INPUTSTAGEBIASNMOS_NET_);
			netNames.push_back(INPUTSTAGEBIASPMOS_NET_);
            terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUTSTAGEBIASNMOS_TERMINAL_, INPUTSTAGEBIASNMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INPUTSTAGEBIASPMOS_TERMINAL_, INPUTSTAGEBIASPMOS_NET_));
		}
		else
		{
        	netNames.push_back(INNERSTAGEBIASNMOS_NET_);
			netNames.push_back(INOUTPUTSTAGEBIASNMOS_NET_);
			netNames.push_back(INSOURCESTAGEBIASNMOS_NET_);
			netNames.push_back(INNERSTAGEBIASPMOS_NET_);
			netNames.push_back(INOUTPUTSTAGEBIASPMOS_NET_);
			netNames.push_back(INSOURCESTAGEBIASPMOS_NET_);
        	terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIASNMOS_TERMINAL_, INNERSTAGEBIASNMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCESTAGEBIASNMOS_TERMINAL_, INSOURCESTAGEBIASNMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUTSTAGEBIASNMOS_TERMINAL_, INOUTPUTSTAGEBIASNMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INNERSTAGEBIASPMOS_TERMINAL_, INNERSTAGEBIASPMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INSOURCESTAGEBIASPMOS_TERMINAL_, INSOURCESTAGEBIASPMOS_NET_));
			terminalToNetMap.insert(std::pair<Core::TerminalName, Core::NetId>(INOUTPUTSTAGEBIASPMOS_TERMINAL_, INOUTPUTSTAGEBIASPMOS_NET_));
		}
	}

	void NonInvertingStages::connectInstanceTerminalsOfSimpleTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const
	{
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT1_TERMINAL_, IN1_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT2_TERMINAL_, IN2_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::SOURCE_TERMINAL_, SOURCETRANSCONDUCTANCE_NET_);

		if(hasGCC(nonInvertingStage.findInstance(createInstanceId(LOAD_))))
		{
			connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT1_TERMINAL_, SOURCEGCC1_NET_);
			connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT2_TERMINAL_, SOURCEGCC2_NET_);
		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT1_TERMINAL_, OUT1_NET_);
			connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT2_TERMINAL_, OUT2_NET_);
		}
	}

	void NonInvertingStages::connectInstanceTerminalsOfFeedbackTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const
	{
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT1_TERMINAL_, IN1_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT2_TERMINAL_, IN2_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::SOURCE1_TERMINAL_, SOURCETRANSCONDUCTANCE1_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::SOURCE2_TERMINAL_, SOURCETRANSCONDUCTANCE2_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INNER_TERMINAL_, INNERTRANSCONDUCTANCE_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT1_TERMINAL_, OUT1_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT2_TERMINAL_, OUT2_NET_);
	}

	void NonInvertingStages::connectInstanceTerminalsOfComplementaryTransconductance(Core::Circuit & nonInvertingStage, Core::Instance & transconductance) const
	{
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT1_TERMINAL_, IN1_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::INPUT2_TERMINAL_, IN2_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::SOURCENMOS_TERMINAL_, SOURCETRANSCONDUCTANCENMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::SOURCEPMOS_TERMINAL_, SOURCETRANSCONDUCTANCEPMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT1NMOS_TERMINAL_, INNERTRANSISTORSTACK1LOADPMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT2NMOS_TERMINAL_, INNERTRANSISTORSTACK2LOADPMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT1PMOS_TERMINAL_, INNERTRANSISTORSTACK1LOADNMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, transconductance, Transconductances::OUT2PMOS_TERMINAL_, INNERTRANSISTORSTACK2LOADNMOS_NET_);
	}

	void NonInvertingStages::connectInstanceTerminalsOfLoad(Core::Circuit & nonInvertingStage, Core::Instance & load) const// feedback
	{
		connectInstanceTerminal(nonInvertingStage, load, Loads::OUT1_TERMINAL_, OUT1_NET_);
		connectInstanceTerminal(nonInvertingStage, load, Loads::OUT2_TERMINAL_, OUT2_NET_);
		connectInstanceTerminalsOfLoadPart1(nonInvertingStage, load);
		if(load.getMaster().findInstances().size() ==2)
		{
			connectInstanceTerminalsOfLoadPart2(nonInvertingStage, load);
		}
	}

	void NonInvertingStages::connectInstanceTerminalsOfLoadPart1(Core::Circuit & nonInvertingStage, Core::Instance & load) const// feedback
	{
		const Core::Instance & loadPart1 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART1_));

		if(!hasGCC(load) ||	getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
		{
			if(loadPart1.getMaster().getCircuitIdentifier().getTechType() == Core::TechType::n())
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD1_TERMINAL_, SOURCENMOS_NET_);	
			}
			else
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD1_TERMINAL_, SOURCEPMOS_NET_);
			}
		}
		
		if(bothTransistorStacksAreVoltageBiases(loadPart1))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::OUTOUTPUT1LOAD1_TERMINAL_, OUTOUTPUT1LOAD1_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::OUTOUTPUT2LOAD1_TERMINAL_, OUTOUTPUT2LOAD1_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::OUTSOURCE1LOAD1_TERMINAL_, OUTSOURCE1LOAD1_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::OUTSOURCE2LOAD1_TERMINAL_, OUTSOURCE2LOAD1_NET_);
			}
		}
		else if(hasGCC(load))
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC1_TERMINAL_, SOURCEGCC1_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC2_TERMINAL_, SOURCEGCC2_NET_);		
        	connectInstanceTerminal(nonInvertingStage, load, Loads::INNERGCC_TERMINAL_, INNERGCC_NET_);
			
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERBIASGCC_TERMINAL_, INNERBIASGCC_NET_);
			}
		}
		else
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() == 2)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERLOAD1_TERMINAL_, INNERLOAD1_NET_);
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD1_TERMINAL_, INNERSOURCELOAD1_NET_);
        		
        		if(isSingleDiodeTransistor(loadPart1.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       			{
					connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_);
        		}
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOAD1_NET_);
			}
		}
		if(!hasGCC(load))
		{
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 2)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD1_TERMINAL_, INNERTRANSISTORSTACK2LOAD1_NET_);
			}
			if(getDeviceNamesOfFlatCircuit(loadPart1.getMaster()).size() > 3)
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD1_TERMINAL_, INNERTRANSISTORSTACK1LOAD1_NET_);
			}
		}
		
	}

	void NonInvertingStages::connectInstanceTerminalsOfLoadPart2(Core::Circuit & nonInvertingStage, Core::Instance & load) const// feedback
	{
		const Core::Instance & loadPart2 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART2_));

		if(loadPart2.getMaster().getCircuitIdentifier().getTechType() == Core::TechType::n())
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD2_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD2_TERMINAL_, SOURCEPMOS_NET_);
		}
		
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() == 2)
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERLOAD2_TERMINAL_, INNERLOAD2_NET_);
		}
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 2)
		{
        	connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOAD2_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD2_TERMINAL_, INNERTRANSISTORSTACK2LOAD2_NET_);

        	if(isSingleDiodeTransistor(loadPart2.getMaster().findInstance(createInstanceId(LoadParts::TRANSISTORSTACK1_)).getMaster()))
       		{
            		connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_);
        	}
		}
		if(getDeviceNamesOfFlatCircuit(loadPart2.getMaster()).size() > 3)
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOAD2_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD2_TERMINAL_, INNERTRANSISTORSTACK1LOAD2_NET_);
		}

	}

	void NonInvertingStages::connectInstanceTerminalsOfComplementaryLoad(Core::Circuit & nonInvertingStage, Core::Instance & load) const
	{
		const Core::Instance & loadPart1 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART1_));
		const Core::Instance & loadPart2 = load.getMaster().findInstance(createInstanceId(Loads::LOADPART2_));

		connectInstanceTerminal(nonInvertingStage, load, Loads::OUT1_TERMINAL_, OUT1_NET_);
		connectInstanceTerminal(nonInvertingStage, load, Loads::OUT2_TERMINAL_, OUT2_NET_);

		if(loadPart1.getMaster().getCircuitIdentifier().getTechType() == Core::TechType::n())
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD1_TERMINAL_, SOURCENMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD2_TERMINAL_, SOURCEPMOS_NET_);
			if(hasGCC(load))
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC1_TERMINAL_, INNERTRANSISTORSTACK1LOADNMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC2_TERMINAL_, INNERTRANSISTORSTACK2LOADNMOS_NET_);
        		connectInstanceTerminal(nonInvertingStage, load, Loads::INNERGCC_TERMINAL_, INNEROUTPUTLOADNMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERBIASGCC_TERMINAL_, INNERSOURCELOADNMOS_NET_);
			}
			else
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD1_TERMINAL_, INNERTRANSISTORSTACK1LOADNMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD1_TERMINAL_, INNERTRANSISTORSTACK2LOADNMOS_NET_);
        		connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOADNMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD1_TERMINAL_, INNERSOURCELOADNMOS_NET_);
			}

			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD2_TERMINAL_, INNERTRANSISTORSTACK1LOADPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD2_TERMINAL_, INNERTRANSISTORSTACK2LOADPMOS_NET_);
        	connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOADPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOADPMOS_NET_);

		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD1_TERMINAL_, SOURCEPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCELOAD2_TERMINAL_, SOURCENMOS_NET_);
			if(hasGCC(load))
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC1_TERMINAL_, INNERTRANSISTORSTACK1LOADPMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::SOURCEGCC2_TERMINAL_, INNERTRANSISTORSTACK2LOADPMOS_NET_);
        		connectInstanceTerminal(nonInvertingStage, load, Loads::INNERGCC_TERMINAL_, INNEROUTPUTLOADPMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERBIASGCC_TERMINAL_, INNERSOURCELOADPMOS_NET_);
			}
			else
			{
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD1_TERMINAL_, INNERTRANSISTORSTACK1LOADPMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD1_TERMINAL_, INNERTRANSISTORSTACK2LOADPMOS_NET_);
        		connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD1_TERMINAL_, INNEROUTPUTLOADPMOS_NET_);
				connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD1_TERMINAL_, INNERSOURCELOADPMOS_NET_);
			}

			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK1LOAD2_TERMINAL_, INNERTRANSISTORSTACK1LOADNMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERTRANSISTORSTACK2LOAD2_TERMINAL_, INNERTRANSISTORSTACK2LOADNMOS_NET_);
        	connectInstanceTerminal(nonInvertingStage, load, Loads::INNEROUTPUTLOAD2_TERMINAL_, INNEROUTPUTLOADNMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, load, Loads::INNERSOURCELOAD2_TERMINAL_, INNERSOURCELOADNMOS_NET_);
		}
	}

	void NonInvertingStages::connectInstanceTerminalsOfStageBias(Core::Circuit & nonInvertingStage, Core::Instance & stageBias) const
		{
			connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::OUT_TERMINAL_, SOURCETRANSCONDUCTANCE_NET_);
			if(stageBias.getMaster().getCircuitIdentifier().getTechType() == Core::TechType::n())
			{
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
			}
			else
			{
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
			}

			if(getDeviceNamesOfFlatCircuit(stageBias.getMaster()).size()== 1)
			{
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::IN_TERMINAL_, INPUTSTAGEBIAS_NET_);
			}
			else
			{
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::INSOURCE_TERMINAL_, INSOURCESTAGEBIAS_NET_);
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::INOUTPUT_TERMINAL_, INOUTPUTSTAGEBIAS_NET_);
				connectInstanceTerminal(nonInvertingStage, stageBias, StageBiases::INNER_TERMINAL_, INNERSTAGEBIAS_NET_);
			}

		}

	void NonInvertingStages::connectInstanceTerminalsOfStageBiases(Core::Circuit & nonInvertingStage, Core::Instance & stageBias1, Core::Instance & stageBias2) const
	{
		connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::OUT_TERMINAL_, SOURCETRANSCONDUCTANCE1_NET_);
		connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::OUT_TERMINAL_, SOURCETRANSCONDUCTANCE2_NET_);
		if(stageBias1.getMaster().getCircuitIdentifier().getTechType() == Core::TechType::n())
		{
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);
		}

		if(getDeviceNamesOfFlatCircuit(stageBias1.getMaster()).size()== 1)
		{
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::IN_TERMINAL_, INPUTSTAGEBIAS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::IN_TERMINAL_, INPUTSTAGEBIAS_NET_);
		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::INSOURCE_TERMINAL_, INSOURCESTAGEBIAS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::INOUTPUT_TERMINAL_, INOUTPUTSTAGEBIAS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias1, StageBiases::INNER_TERMINAL_, INNERSTAGEBIAS1_NET_);

			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::INSOURCE_TERMINAL_, INSOURCESTAGEBIAS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::INOUTPUT_TERMINAL_, INOUTPUTSTAGEBIAS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBias2, StageBiases::INNER_TERMINAL_, INNERSTAGEBIAS2_NET_);
		}
	}

	void NonInvertingStages::connectInstanceTerminalsOfComplementaryStageBiases(Core::Circuit & nonInvertingStage, Core::Instance & stageBiasNmos, Core::Instance & stageBiasPmos)	const
	{
		connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::OUT_TERMINAL_, SOURCETRANSCONDUCTANCENMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::OUT_TERMINAL_, SOURCETRANSCONDUCTANCEPMOS_NET_);

		connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::SOURCE_TERMINAL_, SOURCENMOS_NET_);
		connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::SOURCE_TERMINAL_, SOURCEPMOS_NET_);


		if(getDeviceNamesOfFlatCircuit(stageBiasNmos.getMaster()).size()== 1)
		{
			connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::IN_TERMINAL_, INPUTSTAGEBIASNMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::IN_TERMINAL_, INPUTSTAGEBIASPMOS_NET_);
		}
		else
		{
			connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::INSOURCE_TERMINAL_, INSOURCESTAGEBIASNMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::INOUTPUT_TERMINAL_, INOUTPUTSTAGEBIASNMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBiasNmos, StageBiases::INNER_TERMINAL_, INNERSTAGEBIASNMOS_NET_);

			connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::INSOURCE_TERMINAL_, INSOURCESTAGEBIASPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::INOUTPUT_TERMINAL_, INOUTPUTSTAGEBIASPMOS_NET_);
			connectInstanceTerminal(nonInvertingStage, stageBiasPmos, StageBiases::INNER_TERMINAL_, INNERSTAGEBIASPMOS_NET_);
		}
	}

	void NonInvertingStages::eraseFeedbackNonInvertingStagesPmosTransconductance()
	{
		for(auto & nonInvertingStage : feedbackNonInvertingStagesPmosTransconductance_)
		{
			delete nonInvertingStage;
		}
	}

	void NonInvertingStages::eraseFeedbackNonInvertingStagesNmosTransconductance()
	{
		for(auto & nonInvertingStage : feedbackNonInvertingStagesNmosTransconductance_)
		{
			delete nonInvertingStage;
		}
	}

}
