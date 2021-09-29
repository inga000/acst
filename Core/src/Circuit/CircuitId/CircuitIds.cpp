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
 *************************************************************************************************/#include "Core/incl/Circuit/CircuitId/CircuitIds.h"

namespace Core {

    const CircuitName CircuitIds::NORMALTRANSISTOR_ = CircuitName("NormalTransistor");
    const CircuitName CircuitIds::DIODETRANSISTOR_ = CircuitName("DiodeTransistor");
    const CircuitName CircuitIds::CAPACITOR_ = CircuitName("Capacitor");

    const CircuitName CircuitIds::VOLTAGEBIAS_ = CircuitName("VoltageBias");
    const CircuitName CircuitIds::CURRENTBIAS_ = CircuitName("CurrentBias");
    const CircuitName CircuitIds::ANALOGINVERTER_ = CircuitName("AnalogInverter");
    const CircuitName CircuitIds::CURRENTMIRROR_ = CircuitName("CurrentMirror");
    const CircuitName CircuitIds::DIFFERENTIALPAIR_ = CircuitName("DifferentialPair");

    const CircuitName CircuitIds::LOADPART_= CircuitName("LoadPart");
    const CircuitName CircuitIds::VOLTAGEBIASLOADPART_= CircuitName("VoltageBiasLoadPart");
    const CircuitName CircuitIds::LOAD_ = CircuitName("Load");
    const CircuitName CircuitIds::VOLTAGEBIASLOAD_ = CircuitName("VoltageBiasLoad");
    const CircuitName CircuitIds::STAGEBIAS_ = CircuitName("StageBias");
    const CircuitName CircuitIds::TRANSCONDUCTANCE_ = CircuitName("Transconductance");

    const  CircuitName CircuitIds::INVERTINGSTAGE_ = CircuitName("InvertingStage");
    const  CircuitName CircuitIds::FIRSTSTAGE_ = CircuitName("FirstStage");
    const  CircuitName CircuitIds::FEEDBACKSTAGE_ = CircuitName("NonInvertingStage");
        
    const CircuitName CircuitIds::SINGLEOUTPUTOPAMP_ = CircuitName("one_stage_single_output_op_amp");
    const CircuitName CircuitIds::SINGLEOUTPUTTWOSTAGEOPAMP_ = CircuitName("two_stage_single_output_op_amp");
    const CircuitName CircuitIds::FULLYDIFFERENTIALOPAMP_ = CircuitName("one_stage_fully_differential_op_amp");
    const CircuitName CircuitIds::FULLYDIFFERENTIALTWOSTAGEOPAMP_ = CircuitName("two_stage_fully_differential_op_amp");
    const CircuitName CircuitIds::COMPLEMENTARYOPAMP_ = CircuitName("complementary_op_amp");
    const CircuitName CircuitIds::SYMMETRICALOPAMP_ = CircuitName("symmetrical_op_amp");

     CircuitIds::CircuitIds()
     {

     }
     
     CircuitIds::~CircuitIds()
     {

     }

    CircuitId  CircuitIds::normalTransistor(int num)
    {
        Core::CircuitId circuitId = NORMALTRANSISTOR_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    } 
        
    CircuitId  CircuitIds::diodeTransistor(int num)
    {
        Core::CircuitId circuitId = DIODETRANSISTOR_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::capacitor(int num)
    {
        Core::CircuitId circuitId = CAPACITOR_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::voltageBias(int num)
    {
        Core::CircuitId circuitId = VOLTAGEBIAS_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }
    
    CircuitId  CircuitIds::currentBias(int num)
    {
        Core::CircuitId circuitId = CURRENTBIAS_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::analogInverter(int num)
    {
        Core::CircuitId circuitId = ANALOGINVERTER_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }
    
    CircuitId  CircuitIds::currentMirror(int num)
    {
        Core::CircuitId circuitId = CURRENTMIRROR_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::differentialPair(int num)
    {
        Core::CircuitId circuitId = DIFFERENTIALPAIR_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::loadPart(int num)
    {
        Core::CircuitId circuitId = LOADPART_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::voltageBiasLoadPart(int num)
    {
        Core::CircuitId circuitId = VOLTAGEBIASLOADPART_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::load(int num)
    {
        Core::CircuitId circuitId = LOAD_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::voltageBiasLoad(int num)
    {
        Core::CircuitId circuitId = VOLTAGEBIASLOAD_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::stageBias(int num)
    {
        Core::CircuitId circuitId = STAGEBIAS_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }
        
    CircuitId  CircuitIds::transconductance(int num )
    {
        Core::CircuitId circuitId = TRANSCONDUCTANCE_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::invertingStage(int num)
    {
        Core::CircuitId circuitId = INVERTINGSTAGE_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::firstStage(int num)
    {
        Core::CircuitId circuitId = FIRSTSTAGE_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId  CircuitIds::feedbackStage(int num)
    {
        Core::CircuitId circuitId = FEEDBACKSTAGE_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::simpleOpAmp(int num)
    {
        Core::CircuitId circuitId = SINGLEOUTPUTOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::simpleTwoStageOpAmp(int num)
    {
        Core::CircuitId circuitId = SINGLEOUTPUTTWOSTAGEOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::fullyDifferentialOpAmp(int num)
    {
        Core::CircuitId circuitId = FULLYDIFFERENTIALOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::fullyDifferentialTwoStageOpAmp(int num)
    {
        Core::CircuitId circuitId = FULLYDIFFERENTIALTWOSTAGEOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::complementaryOpAmp(int num)
    {
        Core::CircuitId circuitId = COMPLEMENTARYOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }

    CircuitId CircuitIds::symmetricalOpAmp(int num)
    {
        Core::CircuitId circuitId = SYMMETRICALOPAMP_.createIdentifier();
        circuitId.setId(num);
        return circuitId;
    }




}
