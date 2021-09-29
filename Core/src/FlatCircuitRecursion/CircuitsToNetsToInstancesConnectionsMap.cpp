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
#include "Core/incl/FlatCircuitRecursion/CircuitsToNetsToInstancesConnectionsMap.h"
#include "Core/incl/FlatCircuitRecursion/NetsToInstancesConnections.h"

#include "Core/incl/Circuit/Circuit.h"
#include "Core/incl/Circuit/Instance/InstanceTerminal.h"


namespace Core {

    CircuitsToNetsToInstancesConnectionsMap::CircuitsToNetsToInstancesConnectionsMap(const Core::Circuit & circuit)
    {
        initialize(circuit);
    }

    CircuitsToNetsToInstancesConnectionsMap::~CircuitsToNetsToInstancesConnectionsMap()
    {
        eraseNetsToInstancesConnectionMap();
    }

    void CircuitsToNetsToInstancesConnectionsMap::initialize(const Circuit & circuit)
    {   
        addCircuitToNetsToInstancesConnectionMap(circuit);
        if(circuit.hasInstances())
        {
            for(auto & instance : circuit.findInstances())
            {
                initialize(instance->getMaster());
            }
        }
        
    }

    void CircuitsToNetsToInstancesConnectionsMap::addCircuitToNetsToInstancesConnectionMap(const Circuit & circuit)
    {
        if(circuitToNetsToInstancesConnectionsMap_.find(circuit.getCircuitIdentifier()) == circuitToNetsToInstancesConnectionsMap_.end())
        {
            NetsToInstancesConnections *  connections = new NetsToInstancesConnections(circuit);
            circuitToNetsToInstancesConnectionsMap_.insert(std::pair<CircuitId, const NetsToInstancesConnections *>(circuit.getCircuitIdentifier(), connections));
        }
    }
        
    const NetsToInstancesConnections & CircuitsToNetsToInstancesConnectionsMap::getNetsToInstancesConnections(const Circuit & circuit) const
    {
        assert(circuitToNetsToInstancesConnectionsMap_.find(circuit.getCircuitIdentifier()) != circuitToNetsToInstancesConnectionsMap_.end());
        const NetsToInstancesConnections & netsToInstanceConnection = * circuitToNetsToInstancesConnectionsMap_.at(circuit.getCircuitIdentifier());

        return netsToInstanceConnection;
    }



    bool CircuitsToNetsToInstancesConnectionsMap::isDominatedDoubleNet(const Circuit & circuit, const Net & net) const
    {
        std::set<const Net * > identicalNets = getIdenticalNets(circuit, net);

        if(identicalNets.size() >1)
        {
            if( allNetsSupplyNets(identicalNets) || noNetSupplyNet(identicalNets))
            {
                if(net.getIdentifier() != (**identicalNets.begin()).getIdentifier())
                {
                    return true;
                }
                else
                {
                    return false;
                }
                
            }
            else
            {
                if(isFirstSupplyNetInVector(identicalNets, net))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
        else
        {
            return false;
        }

    }

    const Net & CircuitsToNetsToInstancesConnectionsMap::getDominatingNet(const Circuit & circuit, const Net & net) const
    {
        assert(isDominatedDoubleNet(circuit, net), "Must be a dominated double Net!");
        std::set<const Net * > identicalNets = getIdenticalNets(circuit, net);

        if(allNetsSupplyNets(identicalNets) || noNetSupplyNet(identicalNets))
        {
            return  **identicalNets.begin();
        }
        else 
        {
            return getFirstSupplyNetInVector(identicalNets);
        }
    }

    std::string CircuitsToNetsToInstancesConnectionsMap::print() const
    {
        std::ostringstream oss;
        oss << "Circuit To Net to Instance connection Map" << std::endl;
        for(auto & it : circuitToNetsToInstancesConnectionsMap_)
        {
          oss << it.first.toStr() << std::endl;
           oss << it.second->print() << std::endl;
        }
        return oss.str();
    }

    std::string CircuitsToNetsToInstancesConnectionsMap::printShortenedNets() const
    {
        std::ostringstream oss;
        oss << "Shorten Net Map: " << std::endl;
        for(auto & it : netToShortenedNetMap_)
        {
          oss << "Long Net:" << it.first.toStr() << std::endl;
           oss << "Short net: " << it.second.toStr() << std::endl;
        }
        return oss.str();
    }
    
    bool CircuitsToNetsToInstancesConnectionsMap::isInitialized() const
    {
        return !circuitToNetsToInstancesConnectionsMap_.empty();
    }

    
    void CircuitsToNetsToInstancesConnectionsMap::eraseNetsToInstancesConnectionMap()
    {
        for(auto & it: circuitToNetsToInstancesConnectionsMap_)
        {
            delete it.second;
        }
    }


    NetId CircuitsToNetsToInstancesConnectionsMap::createInnerNetId(NetId netId, InstanceName instanceName) const 
    {
        InstanceNamePath path;
        path.setIsRoot();
        path.append(instanceName);
        netId.setPath(path);
        return netId;
    }

    void CircuitsToNetsToInstancesConnectionsMap::addShortenedNet(NetId longNet, NetId shortNet)
    {
        netToShortenedNetMap_.insert(std::pair<NetId, NetId>(longNet, shortNet));
    }
    
    bool CircuitsToNetsToInstancesConnectionsMap::hasEquivalentShortNet(NetId longNet) const
    {
        return netToShortenedNetMap_.find(longNet) != netToShortenedNetMap_.end();
    }
        
    NetId CircuitsToNetsToInstancesConnectionsMap::getCorrespondingShortNetId(NetId longNet) const
    {
        assert(hasEquivalentShortNet(longNet));
        NetId shortNet = netToShortenedNetMap_.at(longNet);
        if(hasEquivalentShortNet(shortNet))
        {
            shortNet = getCorrespondingShortNetId(shortNet);
        }
        return shortNet;
    }

    std::set<const Net * > CircuitsToNetsToInstancesConnectionsMap::getIdenticalNets(const Circuit & circuit, const Net & net) const
    {
        std::set<const Net*> identicalNets; identicalNets.clear();
        std::vector<const Net *> iterativeSet; iterativeSet.clear();

        identicalNets.insert(&net);
        iterativeSet.push_back(&net);
        int index = 1;

        for(unsigned int  it  = 0; it < iterativeSet.size(); it++)
        {
            index++;
            const Net * searchNet = iterativeSet.at(it);

             if(searchNet->hasInstanceTerminals())
            {
                for(auto & instanceTerminal : searchNet->getInstanceTerminals())
                {
                    const Net & innerNet = instanceTerminal->getTerminal().getNet();
                    const Circuit & innerCircuit = instanceTerminal->getInstance().getMaster();
                    if(!innerCircuit.hasInstances())
                    {
                        const NetId  innerNetId = createInnerNetId(innerNet.getIdentifier(), instanceTerminal->getInstance().getIdentifier().getInstanceName());
                        std::vector<const Net *> outerNets = getNetsToInstancesConnections(circuit).getOuterNets(innerNetId);
                        for(auto & outerNet : outerNets)
                        {
                            if(identicalNets.find(outerNet) == identicalNets.end())
                            {
                                identicalNets.insert(outerNet);
                                iterativeSet.push_back(outerNet);
                            }
                        }

                    }
                    else
                    {
                        std::set<const Net*> innerNets = getIdenticalNets(innerCircuit, innerNet);
                        for(auto& innerNet : innerNets)
                        {
                            const NetId  innerNetId = createInnerNetId(innerNet->getIdentifier(), instanceTerminal->getInstance().getIdentifier().getInstanceName());
                            std::vector<const Net*> outerNets = getNetsToInstancesConnections(circuit).getOuterNets(innerNetId);
                            for(auto & outerNet : outerNets)
                            {
                                if(identicalNets.find(outerNet) == identicalNets.end())
                                {
                                    identicalNets.insert(outerNet);
                                    iterativeSet.push_back(outerNet);
                                }
                            }
                        }  
                    }
                }
            } 
        }
        return identicalNets;
        
    }

    bool CircuitsToNetsToInstancesConnectionsMap::allAreTheSame(std::vector<const Net*> nets) const
    {
        bool allAreTheSame = true;

        for(auto & net1 : nets)
        {
            for(auto & net2 : nets)
            {
                if(net1->getIdentifier() != net2->getIdentifier())
                {
                    allAreTheSame = false;
                    break;
                }
            }
            if(!allAreTheSame)
            {
                break;
            }
        }

        return allAreTheSame;
    }

        bool CircuitsToNetsToInstancesConnectionsMap::allNetsSupplyNets(std::set<const Net*> nets) const
    {
        bool allSupply = true;
        for(auto & net : nets)
        {
            if(!net->isSupply())
            {
                allSupply = false;
                break;
            }
        }
        return allSupply;
    }

    bool CircuitsToNetsToInstancesConnectionsMap::noNetSupplyNet(std::set<const Net*> nets) const
    {
        bool noSupply = true;
        for(auto & net : nets)
        {
            if(net->isSupply())
            {
                noSupply = false;
                break;
            }
        }
        return noSupply;
    }

    bool CircuitsToNetsToInstancesConnectionsMap::isFirstSupplyNetInVector(std::set<const Net*> nets, const Net & net) const
    {
        if(!net.isSupply())
        {
            return false;
        }
        for(auto & circuitNet: nets)
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

    const Net & CircuitsToNetsToInstancesConnectionsMap::getFirstSupplyNetInVector(std::set<const Net*> nets) const
    {
        assert(!noNetSupplyNet(nets), "Vector does not has any supply nets!");
        const Net * firstSupplyNet = nullptr;
        for(auto & net: nets)
        {
            if(net->isSupply())
            {
                firstSupplyNet = net;
                break;
            }
        }
        assert(firstSupplyNet != nullptr, "Something went wrong. Ther should be a supply net in vector!");
        return *firstSupplyNet;
    }



}
