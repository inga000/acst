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
 *************************************************************************************************/#include "Synthesis/incl/Library/HierarchyLevel3/AmplificationStagesSubBlockLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Loads.h"
#include "Synthesis/incl/Library/HierarchyLevel3/LoadParts.h"
#include "Synthesis/incl/Library/HierarchyLevel3/Transconductances.h"
#include "Synthesis/incl/Library/HierarchyLevel3/StageBiases.h"

#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"


#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Synthesis {

    		AmplificationStagesSubBlockLevel::AmplificationStagesSubBlockLevel(const StructuralLevel & structuralLevel, const AutomaticSizing::CircuitInformation & circuitInformation) :
		                loads_(nullptr),
                loadParts_(nullptr),
                transconductances_(nullptr),
                stageBiases_(nullptr)
            {
                logDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>Create load parts");
                loadParts_ = new LoadParts(structuralLevel);
                logDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>Create transconductance");
                transconductances_ = new Transconductances(structuralLevel, circuitInformation);
                logDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>Create stage biases");
                stageBiases_ = new StageBiases(structuralLevel);
                logDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>Create loads");
                loads_ = new  Loads(*this);
            }


		AmplificationStagesSubBlockLevel::~AmplificationStagesSubBlockLevel()
        {
			delete loadParts_;
			delete transconductances_;
			delete stageBiases_;
			delete loads_;
        }

		const Loads & AmplificationStagesSubBlockLevel::getLoads() const
        {
            assert(loads_ != nullptr);
            return *loads_;
        }

		const LoadParts & AmplificationStagesSubBlockLevel::getLoadParts() const
        {
            assert(loadParts_ != nullptr);
            return *loadParts_;
        }

        const Transconductances & AmplificationStagesSubBlockLevel::getTransconductances() const
        {
            assert(transconductances_ != nullptr);
            return * transconductances_;
        }

		const StageBiases & AmplificationStagesSubBlockLevel::getStageBiases() const
        {
            assert(stageBiases_ != nullptr);
            return *stageBiases_;
        }



		std::string AmplificationStagesSubBlockLevel::toStr() const
        {
                        std::ostringstream oss;
            oss << "<<<<<<<<<<<<<<<<<<<<<Amplification Stages Sub Block Level >>>>>>>>>>>>>>>>>>>>>>>>>>> " << std::endl;
            oss << getStageBiases().toStr() << std::endl;
            oss << getTransconductances().toStr() << std::endl;
            oss << getLoadParts().toStr() << std::endl;
            oss << getLoads().toStr() << std::endl;
            oss << std::endl;

            return oss.str();
        }

}
