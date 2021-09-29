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

#include "HSpice/incl/InputFile/InstanceLine/Instance.h"

namespace HSpice {
    namespace InputFile {

        Instance::Instance()
        {
        }

        Instance::~Instance()
        {
        }

        void Instance::setModelName(const ModelName& modelName)
        {
            modelName_ = modelName;
        }

        void Instance::setInstanceId(const Core::InstanceId& instanceId)
        {
           instanceId_ = instanceId;
        }

        void Instance::appendTerminalNetId(const Core::NetId& instanceTerminalNetId)
        {
            instanceTerminalOrdering_.appendNetId(instanceTerminalNetId);
        }

        ModelName Instance::getModelName() const
        {
            assert(modelName_.isInitialized());
            return modelName_;
        }

        Core::InstanceId Instance::getInstanceId() const
        {
            assert(instanceId_.isInitialized());
            return instanceId_;
        }

        InstanceTerminalOrdering Instance::getInstanceTerminalOrdering() const
        {
            assert(instanceTerminalOrdering_.isInitialized());
            return instanceTerminalOrdering_;
        }

        bool Instance::isInitialized() const
        {
           return instanceId_.isInitialized() &&
                   instanceId_.isInitialized() &&
                   instanceTerminalOrdering_.isInitialized();
        }

    }
}
