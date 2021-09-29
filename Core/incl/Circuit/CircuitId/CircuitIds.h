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

#ifndef CORE_CIRCUITIDS_H_
#define CORE_CIRCUITIDS_H_

#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"

#include <string>

namespace Core {


    class CircuitIds 
    {
    public:
        CircuitIds();
        ~CircuitIds();

        CircuitId normalTransistor(int num=0); 
        CircuitId diodeTransistor(int num=0);
        CircuitId capacitor(int num=0);

        CircuitId voltageBias(int num=0);
        CircuitId currentBias(int num=0);
        CircuitId analogInverter(int num=0);
        CircuitId currentMirror(int num=0);
        CircuitId differentialPair(int num=0);

        CircuitId loadPart(int num=0);
        CircuitId voltageBiasLoadPart(int num=0);
        CircuitId load(int num=0);
        CircuitId voltageBiasLoad(int num=0);
        CircuitId stageBias(int num=0);
        CircuitId transconductance(int num =0);

        CircuitId invertingStage(int num=0);
        CircuitId firstStage(int num=0);
        CircuitId feedbackStage(int num=0);

        CircuitId simpleOpAmp(int num=0);
        CircuitId simpleTwoStageOpAmp(int num=0);
        CircuitId fullyDifferentialOpAmp(int num=0);
        CircuitId fullyDifferentialTwoStageOpAmp(int num=0);
        CircuitId complementaryOpAmp(int num=0);
        CircuitId symmetricalOpAmp(int num=0);




       
    private:
        static const CircuitName NORMALTRANSISTOR_;
        static const CircuitName DIODETRANSISTOR_;
        static const CircuitName CAPACITOR_;
        
        static const CircuitName VOLTAGEBIAS_;
        static const CircuitName CURRENTBIAS_;
        static const CircuitName ANALOGINVERTER_;
        static const CircuitName CURRENTMIRROR_;
        static const CircuitName DIFFERENTIALPAIR_;

        static const CircuitName LOADPART_;
        static const CircuitName VOLTAGEBIASLOADPART_;
        static const CircuitName LOAD_;
        static const CircuitName VOLTAGEBIASLOAD_;
        static const CircuitName STAGEBIAS_;
        static const CircuitName TRANSCONDUCTANCE_;

        static const CircuitName INVERTINGSTAGE_;
        static const CircuitName FIRSTSTAGE_;
        static const CircuitName FEEDBACKSTAGE_;
        
        static const CircuitName SINGLEOUTPUTOPAMP_;
        static const CircuitName SINGLEOUTPUTTWOSTAGEOPAMP_;
        static const CircuitName FULLYDIFFERENTIALOPAMP_;
        static const CircuitName FULLYDIFFERENTIALTWOSTAGEOPAMP_;
        static const CircuitName SYMMETRICALOPAMP_;
        static const CircuitName COMPLEMENTARYOPAMP_;

    
    };
}


#endif
