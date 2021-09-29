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
 *************************************************************************************************/#include "Synthesis/incl/Library/HierarchyLevel2/StructuralLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel2/AnalogInverters.h"
#include "Synthesis/incl/Library/HierarchyLevel2/DifferentialPair.h"
#include "Synthesis/incl/Library/HierarchyLevel2/VoltageBiases.h"
#include "Synthesis/incl/Library/HierarchyLevel2/CurrentBiases.h"

#include "Synthesis/incl/Library/HierarchyLevel1/DeviceLevel.h"


#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Synthesis {

    		StructuralLevel::StructuralLevel(const DeviceLevel & deviceLevel, const AutomaticSizing::CircuitInformation & circuitInformation) :
                voltageBiases_(nullptr),
                currentBiases_(nullptr),
                currentMirrors_(nullptr),
                differentialPair_(nullptr),
                analogInverters_(nullptr)
            {
    			logDebug("Creating voltage biases");
                voltageBiases_ = new VoltageBiases(deviceLevel);
                logDebug("Creating current biases");
                currentBiases_ = new CurrentBiases(deviceLevel);
                logDebug("Creating differential pairs");
                differentialPair_ = new DifferentialPair(deviceLevel);

                if(!circuitInformation.getCircuitParameter().isComplementary())
                {
                	logDebug("Creating analog inverters");
                	analogInverters_ = new AnalogInverters(*this);
                }
            }


		StructuralLevel::~StructuralLevel()
        {
			delete voltageBiases_;
			delete currentBiases_;
			delete currentMirrors_;
			delete differentialPair_;
			delete analogInverters_;
        }

		const VoltageBiases & StructuralLevel::getVoltageBiases() const
        {
            assert(voltageBiases_ != nullptr);
            return *voltageBiases_;
        }

		const CurrentBiases & StructuralLevel::getCurrentBiases() const
        {
            assert(currentBiases_ != nullptr);
            return *currentBiases_;
        }

        const CurrentMirrors & StructuralLevel::getCurrentMirrors() const
        {
            assert(currentMirrors_ != nullptr);
            return * currentMirrors_;
        }

		const DifferentialPair & StructuralLevel::getDifferentialPair() const
        {
            assert(differentialPair_ != nullptr);
            return *differentialPair_;
        }

        const AnalogInverters & StructuralLevel::getAnalogInverters() const
        {
            assert(analogInverters_ != nullptr);
            return *analogInverters_;
        }

		std::string StructuralLevel::toStr() const
        {
                        std::ostringstream oss;
            oss << "<<<<<<<<<<<<<<<<<<<<< Structural Level >>>>>>>>>>>>>>>>>>>>> " << std::endl;
            oss << getVoltageBiases().toStr() << std::endl;
            oss << getCurrentBiases().toStr() << std::endl;
            oss << getDifferentialPair().toStr() << std::endl;
            oss << getAnalogInverters().toStr() << std::endl;
            oss << std::endl;

            return oss.str();
        }











}
