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

#include "HSpice/incl/InputFile/TechTypeMapper.h"
#include "HSpice/incl/InputFile/ModelName.h"
#include "Log/incl/LogMacros.h"

namespace HSpice {
    namespace InputFile {

        TechTypeMapper::TechTypeMapper() :
                initializedFlag_(false)
        {
        }

        TechTypeMapper::~TechTypeMapper()
        {
        }

        void TechTypeMapper::initializeEmpty()
        {
            nTechTypeModels_.clear();
            pTechTypeModels_.clear();
            initializedFlag_ = true;
        }

        bool TechTypeMapper::isInitialized() const
        {
            return initializedFlag_;
        }

        void TechTypeMapper::addNType(const ModelName& modelName)
        {
            assert(isInitialized());
            nTechTypeModels_.insert(modelName);
        }

        void TechTypeMapper::addPType(const ModelName& modelName)
        {
            assert(isInitialized());
            pTechTypeModels_.insert(modelName);
        }

        bool TechTypeMapper::hasTechType(const ModelName& modelName) const
        {
            assert(isInitialized());
            return isNType(modelName) || isPType(modelName);
        }

        Core::TechType TechTypeMapper::findTechType(const ModelName& modelName) const
        {
            assert(isInitialized());
            Core::TechType techType = Core::TechType::undefined();
            if(isNType(modelName)) {
                techType = Core::TechType::n();
            } else if(isPType(modelName)) {
                techType = Core::TechType::p();
            } else {
                logDebug("Error: Undefined techtype for: \"" << modelName << "\"");
                assert(false);
            }
            return techType;
        }

        bool TechTypeMapper::isNType(const ModelName& modelName) const
        {
            assert(isInitialized());
            return nTechTypeModels_.find(modelName) != nTechTypeModels_.end();
        }

        bool TechTypeMapper::isPType(const ModelName& modelName) const
        {
            assert(isInitialized());
            return pTechTypeModels_.find(modelName) != pTechTypeModels_.end();
        }

    }
}

