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


#include "Core/incl/FlatCircuitRecursion/NetsToInstancesConnections.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"

namespace Core {


    NetsToInstancesConnections::NetsToInstancesConnections(const Circuit & circuit) :
                isInitialized_(false)
    {
        initilaizeMapOfAllNetsPointingToSameInnerInstanceNets(circuit);
    }

    NetsToInstancesConnections::~NetsToInstancesConnections()
    {

    }

        
    void NetsToInstancesConnections::initilaizeMapOfAllNetsPointingToSameInnerInstanceNets(const Circuit & topCircuit)
    {
        for(auto & net : topCircuit.findNets())
        {
            if(net->hasInstanceTerminals())
            {
                for(auto & instanceTerminal : net->getInstanceTerminals())
                {
                    NetId innerNet = createInnerNetNetId(*instanceTerminal);
                    std::vector<const Net*> & circuitNets = innerInstanceNetToTopCircuitNets_[innerNet];
                    circuitNets.push_back(net);
                }
            }
        }
        isInitialized_= true;
    }

    bool NetsToInstancesConnections::isInitialized() const
    {
        return isInitialized_;
    }

    bool NetsToInstancesConnections::isDominatedDoubleNet(const Net & net) const
    {   
        if(net.hasInstanceTerminals())
        {
            for(auto & instanceTerminal : net.getInstanceTerminals())
            {
                NetId innerNet = createInnerNetNetId(*instanceTerminal);
                
                std::vector<const Net*> circuitNets = innerInstanceNetToTopCircuitNets_.at(innerNet);
  
                if(isInVector(circuitNets, net) && circuitNets.size() >= 2)
                {
                    if(allNetsSupplyNets(circuitNets) || noNetSupplyNet(circuitNets))
                    {
                        const Net * firstNet = *circuitNets.begin();
                        if(firstNet->getIdentifier() != net.getIdentifier())
                        {
                            return true;
                        }
                        
                    }
                    else if(!isFirstSupplyNetInVector(circuitNets, net))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    const Net & NetsToInstancesConnections::getDominatingNet(const Net & net) const
    {
        assert(isDominatedDoubleNet(net), "Net must be dominated double Net!");
        for(auto & instanceTerminal : net.getInstanceTerminals())
        {
            NetId innerNet = createInnerNetNetId(*instanceTerminal);

            std::vector<const Net*> circuitNets = innerInstanceNetToTopCircuitNets_.at(innerNet);

            if(isInVector(circuitNets, net) && circuitNets.size() >= 2)
            {
                if(allNetsSupplyNets(circuitNets) || noNetSupplyNet(circuitNets))
                {
                    return  **circuitNets.begin();
                      
                }
                else 
                {
                        return getFirstSupplyNetInVector(circuitNets);
                }
               
            }
        }
        assert("Something went wrong no proper net was found!");
        const Net* dummy = nullptr;
        return *dummy;
        
    }

    NetId NetsToInstancesConnections::createInnerNetNetId(const InstanceTerminal & instanceTerminal) const 
    {
        NetId innerNet = instanceTerminal.getTerminal().getNet().getIdentifier();
        InstanceNamePath path;
        path.setIsRoot();
        path.append(instanceTerminal.getInstance().getIdentifier().getInstanceName());
        innerNet.setPath(path);
        return innerNet;
    }

    bool NetsToInstancesConnections::allNetsSupplyNets(std::vector<const Net*> circuitNets) const
    {
        bool allSupply = true;
        for(auto & net : circuitNets)
        {
            if(!net->isSupply())
            {
                allSupply = false;
                break;
            }
        }
        return allSupply;
    }

    bool NetsToInstancesConnections::noNetSupplyNet(std::vector<const Net*> circuitNets) const
    {
        bool noSupply = true;
        for(auto & net : circuitNets)
        {
            if(net->isSupply())
            {
                noSupply = false;
                break;
            }
        }
        return noSupply;
    }

    bool NetsToInstancesConnections::isFirstSupplyNetInVector(std::vector<const Net*> circuitNets, const Net & net) const
    {
        for(auto & circuitNet: circuitNets)
        {
            if(circuitNet->isSupply() && circuitNet->getIdentifier() != net.getIdentifier())
            {
                return false;
            }
            else if(circuitNet->isSupply())
            {
                return true;
            }
        }
        return false;
    }

    const Net & NetsToInstancesConnections::getFirstSupplyNetInVector(std::vector<const Net*> circuitNets) const
    {
        assert(noNetSupplyNet(circuitNets), "Vector does not has any supply nets!");
        const Net * firstSupplyNet = nullptr;
        for(auto & circuitNet: circuitNets)
        {
            if(circuitNet->isSupply())
            {
                firstSupplyNet = circuitNet;
                break;
            }
        }
           
        assert(firstSupplyNet != nullptr, "Something went wrong. Ther should be a supply net in vector!");
        return *firstSupplyNet;
    }

        
    bool NetsToInstancesConnections::isInVector(std::vector<const Net*> circuitNets, const Net & net) const
    {
        bool isIn = false;
        for(auto & circuitNet : circuitNets)
        {
            if(circuitNet->getIdentifier() == net.getIdentifier())
            {
                isIn = true;
                break;
            }
        }

        return isIn;
    }

    std::string NetsToInstancesConnections::print() const
    {
        std::ostringstream oss;
        oss << "Net to Instance Connection " << std::endl;
        for(auto & it : innerInstanceNetToTopCircuitNets_)
        {
            oss << "Inner net: " << it.first << " Outer nets: ";
            for(auto & net : it.second)
            {
                oss << net->getIdentifier().toStr() << ", ";
            }
            oss << std::endl;
        }
        return oss.str();
    }

    const Net & NetsToInstancesConnections::findOuterNet(NetId innerNetId) const
    {
        assert(innerInstanceNetToTopCircuitNets_.find(innerNetId) 
                    != innerInstanceNetToTopCircuitNets_.end(), "Net must be in map! Is that the right circuit you search for?");
        
        std::vector<const Net *> outerNets = innerInstanceNetToTopCircuitNets_.at(innerNetId);
        if(outerNets.size() == 1 || noNetSupplyNet(outerNets))
        {
            return ** outerNets.begin();
        }
        else
        {
            return getFirstSupplyNetInVector(outerNets);
        }
        
    }

    std::vector<const Net *> NetsToInstancesConnections::getOuterNets(NetId innerNetId) const
    {
        if(innerInstanceNetToTopCircuitNets_.find(innerNetId)  != innerInstanceNetToTopCircuitNets_.end())
            return  innerInstanceNetToTopCircuitNets_.at(innerNetId);
        else
        {   
            std::vector<const Net *> emptyVector;
            return emptyVector;
        }
            
    }

}



