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
#include "Synthesis/incl/Library/HierarchyLevel1/DeviceLevel.h"
#include "Synthesis/incl/Library/HierarchyLevel1/NormalTransistor.h"
#include "Synthesis/incl/Library/HierarchyLevel1/DiodeTransistor.h"
#include "Synthesis/incl/Library/HierarchyLevel1/Capacitor.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include "Log/incl/LogMacros.h"

#include <sstream>

namespace Synthesis {

		DeviceLevel::DeviceLevel() :
            normalTransistor_(nullptr),
            diodeTransistor_(nullptr)
        {
            logDebug("Create Normal transistor");
            normalTransistor_ = new NormalTransistor;
            logDebug("Create diode transistor");
            diodeTransistor_ = new DiodeTransistor;

            logDebug("Create capacitor");
            capacitor_ = new Capacitor;

            
        }
		
        DeviceLevel::~DeviceLevel()
        {
            delete capacitor_;
            delete diodeTransistor_;
            delete normalTransistor_;
        }

		const NormalTransistor & DeviceLevel::getNormalTransistor() const
        {
            assert(normalTransistor_ != nullptr);
            return *normalTransistor_;
        }

        const DiodeTransistor & DeviceLevel::getDiodeTransistor() const
        {
            assert(diodeTransistor_ != nullptr);
            return *diodeTransistor_;
        }

        const Capacitor & DeviceLevel::getCapacitor() const
        {
            assert(capacitor_ != nullptr);
            return *capacitor_;
        }

        std::string DeviceLevel::toStr() const
        {
            std::ostringstream oss;
            oss << "Device Level: " << std::endl;
            oss << getNormalTransistor().toStr() << std::endl;
            oss << getDiodeTransistor().toStr() << std::endl;
            oss << getCapacitor().toStr() << std::endl;
            oss << std::endl;

            return oss.str();

        }


}
