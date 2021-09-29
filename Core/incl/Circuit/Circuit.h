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

#ifndef CORE_CIRCUIT_H
#define CORE_CIRCUIT_H

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/Net.h"
#include "Core/incl/Circuit/Net/SupplyNets.h"
#include "Core/incl/Circuit/Terminal/Terminal.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Instance/Instance.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"

#include "Core/incl/FlatCircuitRecursion/NetsToInstancesConnections.h"
#include <map>

namespace Core {

    class Net;
    class LocalGlobalNet;
    class NetId;
    class DeviceId;
    class DeviceType;

    class Instance;
    class InstanceId;
    class Terminal;
    class TerminalId;

    class Circuit : public Object
    {
    private:
    	typedef std::map<NetId, Net* > NetMap;
    	typedef std::map<TerminalId, Terminal*> TerminalMap;
    	typedef std::map<DeviceId, Device*> DeviceMap;
    	typedef std::map<InstanceId, Instance*> InstanceMap;

    public:
        Circuit();
        virtual ~Circuit();

        Circuit & clone() const;

        void setCircuitIdentifier(const CircuitId& circuitIdentifier);
        CircuitId getCircuitIdentifier() const;

        Net& addNet(const NetId & identifier);
        LocalGlobalNet& addLocalGlobalNet(const NetId& identifier);

        void addDevice(Device & device);
        void addNet(Net & net);
        void addInstance(Instance & instance);

        Device& addDevice(const DeviceId & deviceId);
        Instance& addInstance(const InstanceId & instanceId);
        Terminal& addTerminal(const TerminalId & terminalId);

        const Net& findNet(const NetId& identifier) const;
        Net& findNet(const NetId& identifier);

        const Device& findDevice(const DeviceId& componentId) const;
        Device& findDevice(const DeviceId& componentId);

        const Terminal& findTerminal(const TerminalId& terminalId) const;
        Terminal& findTerminal(const TerminalId& terminalId);
        const Terminal& findTerminal(NetId netId) const;
        Terminal& findTerminal(NetId netId);

        const Instance& findInstance(const InstanceId& instanceId) const;
        Instance& findInstance(const InstanceId& instanceId);

        

        std::vector<const Net*> findNets() const;
        std::vector<Device*> getDevices() const;
        std::vector<Net*> getNets() const;
        std::vector<const Device*> findDevices() const;
        std::vector<const Terminal*> findTerminals() const;
        std::vector<const Instance*> findInstances() const;
        std::vector<Instance*> findInstances();

        virtual std::vector<NetId> findGlobalNetIds() const;
        virtual std::vector<NetId> findVddSupplyNetIds() const;
        virtual std::vector<NetId> findGndSupplyNetIds() const;
        virtual SupplyNets findSupplyNets() const;

        void removeNet(Net & net);
        void shortenNets(Net& net1, Net& net2);

        bool hasInstances() const;
        bool hasTerminals() const;
        bool hasNet(const NetId& identifier) const;
		bool hasDevice(const DeviceId& componentId) const;
		bool hasTerminal(const TerminalId& terminalId) const;
		bool hasTerminal(const NetId& netId) const;
		bool hasInstance(const InstanceId& instanceId) const;

        bool hasGateNetsNotConnectedToADrain() const;
        std::vector<NetId> getGateNetsNotConnectedToADrain() const;

        bool GatesOfInstanceTransistorsConnectedToSameNet(const Instance & instance) const;
        
        bool everyGateNetIsNotConnectedToMoreThanOneDrainOfComponentWithSameTechType() const;

        std::string toStr() const;
    private:
        void print(std::ostream& stream) const;
        void printSummary(std::ostream& stream) const;

        void printTerminals(std::ostream& stream) const;
        void printNets(std::ostream& stream) const;
        void printDevices(std::ostream& stream) const;
        void printInstances(std::ostream& stream) const;

        void eraseNets();
        void eraseDevices();
        void eraseInstances();
        void eraseTerminals();

        Supply computeMergedSupply(Net& net1, Net& net2);

        bool gatePinsAreOnlyNDoped(std::vector<Pin*> gatePins) const;
        bool gatePinsAreOnlyPDoped(std::vector<Pin*> gatePins) const;
        bool moreThanOnePDopdedDrainPin(std::vector<Pin*> drainPins) const;
        bool moreThanOneNDopdedDrainPin(std::vector<Pin*> drainPins) const;

        DeviceId createInInstanceDeviceId(InstanceName instanceName, DeviceId oldDeviceId) const;

    private:
        const NetMap& getNetMap() const;
        NetMap& getNetMap();

        const DeviceMap& getDeviceMap() const;
        DeviceMap& getDeviceMap();

        const TerminalMap& getTerminalMap() const;
        TerminalMap& getTerminalMap();

        const InstanceMap& getInstanceMap() const;
        InstanceMap& getInstanceMap();

        std::vector<Terminal*> findTerminals();

    private:
        CircuitId circuitIdentifier_;
        NetMap netMap_;
        DeviceMap deviceMap_;
        TerminalMap terminalMap_;
        InstanceMap instanceMap_;


    };
}

#endif
