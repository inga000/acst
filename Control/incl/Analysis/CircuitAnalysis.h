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

 
#ifndef CONTROL_CIRCUITANALYSIS_H_
#define CONTROL_CIRCUITANALYSIS_H_

#include "../Options/CircuitReadInOptions.h"
#include "Control/incl/Analysis/Analysis.h"
#include "Control/incl/Options/OptionsBase.h"
#include "HSpice/incl/InputFile/SupplyNet/SupplyNetIdentifiers.h"
#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"

namespace Core {
    class Circuit;
    class DeviceTypeRegister;
}

namespace Control {

    class OptionsBase;
    

    template <class LocalOptions>
    class CircuitAnalysis : public AbstractAnalysis
    {
    public:
        CircuitAnalysis();
        virtual ~CircuitAnalysis();

        void initialize();
        virtual void compute() = 0;
        virtual void write() = 0;

        void setLocalOptions(const OptionsBase & localOptions);

    protected:
        const Core::Circuit & getCircuit() const;
        const Core::DeviceTypeRegister& getDeviceTypeRegister() const;


        bool hasOutputFile() const;
        OutputFile& getOutputFile();

    protected:
        const LocalOptions & getLocalOptions() const;


    private:
        void setCircuit(const Core::Circuit &  circuit);
        void setDeviceTypeRegister(const Core::DeviceTypeRegister& deviceTypeRegister);

        void setOutputFile(OutputFile& outputFile);


        const Core::Circuit * circuit_;
        const Core::DeviceTypeRegister* deviceTypeRegister_;

        const LocalOptions* localOptions_;

        OutputFile outputFile_;
    };
}

#include "Control/src/Analysis/CircuitAnalysis.impl"
#endif
