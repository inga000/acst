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
 *************************************************************************************************/#include "Synthesis/incl/Library/HierarchyLevel4/AmplificationStageLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel4/InvertingStages.h"
#include "Synthesis/incl/Library/HierarchyLevel4/NonInvertingStages.h"

#include "Synthesis/incl/Library/HierarchyLevel3/AmplificationStagesSubBlockLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"


#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Synthesis {

    	AmplificationStageLevel::AmplificationStageLevel(
                        const AmplificationStagesSubBlockLevel & amplificationStagesSubBlockLevel,
                        const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation) :
                invertingStages_(nullptr),
                nonInvertingStages_(nullptr)
        {
    		if(!circuitInformation.getCircuitParameter().isComplementary())
    		{
    			logDebug(">>>>>>>>>>>>>>>>>>>>Create Inverting stages");
    			invertingStages_ = new InvertingStages(structuralLevel);
    		}
            logDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Create non Inverting stages");
            nonInvertingStages_ = new NonInvertingStages(amplificationStagesSubBlockLevel, circuitInformation);
         }


		AmplificationStageLevel::~AmplificationStageLevel()
        {
			delete invertingStages_;
			delete nonInvertingStages_;
        }

		const InvertingStages & AmplificationStageLevel::getInvertingStages() const
        {
            assert(invertingStages_ != nullptr);
            return *invertingStages_;
        }

		const NonInvertingStages & AmplificationStageLevel::getNonInvertingStages() const
        {
            assert(nonInvertingStages_ != nullptr);
            return *nonInvertingStages_;
        }

    
		std::string AmplificationStageLevel::toStr() const
        {
                        std::ostringstream oss;
            oss << "<<<<<<<<<<<<<<<<<<<<<Amplification Stage Level >>>>>>>>>>>>>>>>>>>>>>>>>>> " << std::endl;
            oss << getNonInvertingStages().toStr() << std::endl;
            oss << getInvertingStages().toStr() << std::endl;
            oss << std::endl;

            return oss.str();
        }
        
}
