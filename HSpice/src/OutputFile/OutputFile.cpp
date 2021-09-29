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

#include "HSpice/incl/OutputFile/OutputFile.h"
#include "HSpice/incl/OutputFile/DeviceLine.h"
#include "HSpice/incl/OutputFile/HSpiceString.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Control/incl/File/OutputFile.h"
#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuitMap.h"
#include "HSpice/incl/OutputFile/SubCircuitMap/SubCircuit.h"
#include "HSpice/incl/OutputFile/SubCircuitRecursion.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace HSpice {
    namespace OutputFile {

        OutputFile::OutputFile():
                circuit_(NULL),
                outputFile_(NULL)
        {
        }

        void OutputFile::setCircuit(const Core::Circuit& circuit)
        {
            circuit_ = &circuit;
        }

        void OutputFile::setOutputFile(Control::OutputFile& outputFile)
        {
            outputFile_ = &outputFile;
        }

        void OutputFile::write()
        {
            getOutputFile().open();
            writeGlobalNets();
            writeSubCircuits();
            writeDevices();
            writeInstances();

            getOutputFile() <<".END";
            getOutputFile().close();
        }

        void OutputFile::writeGlobalNets()
        {
            getOutputFile() << ".GLOBAL";
            std::vector<Core::NetId> globalNetIds = getCircuit().findGlobalNetIds();
            for(auto& it : globalNetIds)
            {
                if(it == globalNetIds.front()) {
                    getOutputFile() << " ";
                }
                std::string netName = HSpiceString::convertToHSpice(it);
                getOutputFile() << netName << " ";
            }
            getOutputFile( )<< "\n\n";
        }

        void OutputFile::writeSubCircuits()
        {
            SubCircuitMap subCircuitMap;
            subCircuitMap.setTerminalOrderingMap(getTerminalOrderingMap());

            SubCircuitRecursion subCircuitRecursion;
            subCircuitRecursion.setSubCircuitMap(subCircuitMap);
            subCircuitRecursion.fillSubCircuitMapRecursivly(getCircuit());

            subCircuitMap.buildTerminalOrderingMap();
            subCircuitMap.writeSubCircuits(getOutputFile());
        }

        void OutputFile::writeDevices()
        {
            std::vector<const Core::Device*> devices = getCircuit().findDevices();
            for(auto& it : devices)
            {
               const Core::Device* device = it;
               DeviceLine deviceLine;
               deviceLine.setDevice(*device);
               deviceLine.setOutputFile(getOutputFile());
               deviceLine.write();
            }
        }

        void OutputFile::writeInstances()
        {   
            if(getCircuit().hasInstances())
            {
                std::vector<const Core::Instance*> instances = getCircuit().findInstances();
                for(auto& instIt : instances)
                {
                    const Core::Instance* instance = instIt;
                    const Core::InstanceId instanceId = instance->getIdentifier();
                    std::string instanceName = HSpiceString::convertToHSpice(instanceId);
                    getOutputFile() << instanceName << " ";

                    Core::CircuitId circuitId = instance->getMaster().getCircuitIdentifier();
                    TerminalOrdering terminalOrdering = getTerminalOrderingMap().findTerminalOrdering(circuitId);
                    terminalOrdering.setInstance(*instance);
                    terminalOrdering.writeInstanceTerminals(getOutputFile());

                    std::string circuitName = HSpiceString::convertToHSpice(circuitId);
                    getOutputFile() << circuitName << "\n";
                }
            }

        }

        const Core::Circuit& OutputFile::getCircuit() const
        {
            assert(circuit_ != NULL);
            return *circuit_;
        }

        TerminalOrderingMap& OutputFile::getTerminalOrderingMap()
        {
            return terminalOrderingMap_;
        }

        Control::OutputFile& OutputFile::getOutputFile()
        {
            assert(outputFile_ != NULL);
            return *outputFile_;
        }

    }
}
