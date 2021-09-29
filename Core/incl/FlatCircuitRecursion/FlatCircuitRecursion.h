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

#ifndef CORE_FLATCIRCUITRECURSION_H_
#define CORE_FLATCIRCUITRECURSION_H_

#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/FlatCircuitRecursion/NetsToInstancesConnections.h"

namespace Core {

    class Device;
    class CircuitsToNetsToInstancesConnectionsMap;


    class FlatCircuitRecursion
    {
    public:
        FlatCircuitRecursion();
        ~FlatCircuitRecursion();
        Circuit* createNewFlatCopy(const Circuit& topCircuit);

    private:
        const CircuitsToNetsToInstancesConnectionsMap & getCircuitToNetsToInstancesConnectionsMap() const;
        CircuitsToNetsToInstancesConnectionsMap & getCircuitToNetsToInstancesConnectionsMap();
        void createNetsToInstancesConnectionsMap(const Circuit & circuit);

        void flattenCircuitRecursively(const Circuit& circuit);
        void addTopLevelTerminals(const Circuit& topCircuit);

        void addNetsToFlatCopy(const Circuit& circuit);
        void addDevicesToFlatCopy(const Circuit& circuit);
        void addInstancesToFlatCopy(const Circuit& circuit);

        void connectComponent(const Circuit & circuit, const Device& component, Device& flatDevice);
        void connectInstance(const Circuit & circuit, const Instance& instance);

        Net & computeFlatInstanceTerminalNet(const InstanceTerminal& instanceTerminal);
        Net & computeFlatTerminalNet(const InstanceTerminal& instanceTerminal, const Instance & instance);

        NetId computeFlatNetIdentifier(const Pin& pin) const;
        NetId computeFlatNetIdentifier(const Net& net) const;
        DeviceId computeFlatDeviceIdentifier(const Device& component) const;

        InstanceNamePath getInstanceNamePath() const;
         CircuitId createInInstanceCircuitId(const Circuit & circuit, InstanceNamePath path) const;

        bool hasFlatCircuit() const;

        Circuit& getFlatCircuit();
        const Circuit& getFlatCircuit() const;

        const VisitedInstances& getVisitedInstances() const;
        VisitedInstances& getVisitedInstances();

    private:
        Circuit* flatCircuit_;
        VisitedInstances visitedInstances_;

        CircuitsToNetsToInstancesConnectionsMap * circuitToNetsToInstancesConnectionsMap_;
        
    };
}


#endif
