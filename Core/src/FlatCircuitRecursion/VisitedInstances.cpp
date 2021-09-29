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

#include "Core/incl/FlatCircuitRecursion/VisitedInstances.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"

namespace Core {

    VisitedInstances::VisitedInstances():
            flatCircuit_(NULL),
            device_(NULL)
    {
    }

    void VisitedInstances::setCircuit(const Circuit& flatCircuit)
    {
        flatCircuit_ = &flatCircuit;
    }

    void VisitedInstances::push(const Instance& instance)
    {
        visitedInstances_.push_back(&instance);
    }

    void VisitedInstances::push(const Device& device)
    {
        assert(!hasDevice());
        device_ = &device;
    }

    void VisitedInstances::pop()
    {
        if(hasDevice()) {
            device_ = NULL;
        } else {
            visitedInstances_.pop_back();
        }
    }

    InstanceNamePath VisitedInstances::computeInstanceNamePath() const
    {
        InstanceNamePath instanceNamePath;
        instanceNamePath.setIsRoot();

        for(InstancePointerVector::const_iterator it = visitedInstances_.begin(); it != visitedInstances_.end(); it++)
        {
            const Instance& instance = **it;
            InstanceId instanceId = instance.getIdentifier();
            InstanceName instanceName = instanceId.getInstanceName();
            instanceNamePath.append(instanceName);
        }

        return instanceNamePath;
    }

    NetId VisitedInstances::resolveAssignment(const NetAssignment& netAssignment) const
    {
        InstanceNamePath currentInstanceNamePath = computeInstanceNamePath();
        PropertyName propertyName = netAssignment.getPropertyName();
        NetId resolvedNetId = netAssignment.getNetIdentifier();

        // Resolve Properties of Device
        if(hasDevice()) {
            if(device_->hasAssignment(propertyName)) {
                NetAssignment newNetAssignment = device_->findAssignment(propertyName);
                NetId newAssignmentNetId = newNetAssignment.getNetIdentifier();
                propertyName =  newNetAssignment.getPropertyName();
                resolvedNetId = computeNetIdentifier(currentInstanceNamePath, newAssignmentNetId);
            }
            else if(device_->hasValue(propertyName)) {
                NetId valueNetId = device_->findValue(propertyName);
                resolvedNetId = computeNetIdentifier(currentInstanceNamePath, valueNetId);
            }
        }

        // Climb up Instances
        for(InstancePointerVector::const_reverse_iterator rIt = visitedInstances_.rbegin(); rIt != visitedInstances_.rend(); rIt++)
        {
            currentInstanceNamePath.pop();
            const Instance* currentInstance = *rIt;

            if(currentInstance->hasPropertyAssignment(propertyName)) {
                NetAssignment newNetAssignment = currentInstance->findPropertyAssignment(propertyName);
                NetId newAssignmentNetId = newNetAssignment.getNetIdentifier();
                propertyName =  newNetAssignment.getPropertyName();
                resolvedNetId = computeNetIdentifier(currentInstanceNamePath, newAssignmentNetId);
                continue;
            }
            else if(currentInstance->hasPropertyValue(propertyName)) {
                NetId valueNetId = currentInstance->findPropertyValue(propertyName);
                resolvedNetId = computeNetIdentifier(currentInstanceNamePath, valueNetId);
                break;
            }
        }
        return resolvedNetId;
    }

    NetId VisitedInstances::computeNetIdentifier(const InstanceNamePath& instanceNamePath,
            const NetId& netId) const
    {
       NetId globalNetId = netId;
       NetId localNetId = netId;
       localNetId.setPath(instanceNamePath);

       NetId resolvedNetId;
       if(getFlatCircuit().hasNet(localNetId)){
           resolvedNetId = localNetId;
       } else if(getFlatCircuit().hasNet(globalNetId)) {
           resolvedNetId = globalNetId;
       } else {
           assert(false);
       }
       return resolvedNetId;
    }

    bool VisitedInstances::hasFlatCircuit() const
    {
        return flatCircuit_ != NULL;
    }

    bool VisitedInstances::hasDevice() const
    {
        return device_ != NULL;
    }

    const Circuit& VisitedInstances::getFlatCircuit() const
    {
        assert(hasFlatCircuit());
        return *flatCircuit_;
    }
}

