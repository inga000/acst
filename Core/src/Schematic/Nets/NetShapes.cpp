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

#include "Core/incl/Schematic/Nets/NetShapes.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        NetShapes::NetShapes()
        {
        }

        NetShapes::~NetShapes()
        {
            netLineMap_.clear();
        }

        bool NetShapes::has(const NetId & netName) const
        {
            NetLineMap::const_iterator found = netLineMap_.find(netName);
            return found != netLineMap_.end();
        }

        bool NetShapes::hasGlobalNet(const NetId& netId) const
        {
            bool global = false;
            NetLineMap::const_iterator found = netLineMap_.find(netId);
            if(found != netLineMap_.end()) {
                NetLines netLines = found->second;
                global = netLines.isGlobal();
            }
            return global;
        }

        NetLines& NetShapes::findNetLines(const NetId& netId)
        {
            assert(has(netId));
            return netLineMap_.find(netId)->second;
        }

        std::vector<const Point*> NetShapes::getPointsBetweenTwoPoints(Point& point1, Point& point2) const
        {
        	std::vector<const Point*> pointResult;
        	pointResult.push_back(&point1);
        	for(NetLineMap::const_iterator it = netLineMap_.begin();it!=netLineMap_.end();it++)
        	{
        		NetLines netLines = it->second;
        		if(netLines.hasPoint(point1))
        		{
        			netLines.getPointsBetweenTwoPoints(point1,point2, pointResult);
        			break;
        		}
        	}
        	return pointResult;
        }

        const NetLines& NetShapes::findNetLines(const NetId & netName) const
        {
            assert(has(netName));
            return netLineMap_.find(netName)->second;
        }

        std::vector<NetId> NetShapes::getNetIds() const
        {
        	std::vector<NetId> items;
        	for(auto& it : netLineMap_)
        	{
        		items.push_back(it.first);
        	}
        	return items;
        }

        void NetShapes::addNetLines(const NetId & netName, NetLines & netLines)
        {
            assert(!has(netName));
            netLineMap_.insert(NetLineMap::value_type(netName, netLines));
        }

        void NetShapes::addSolderDot(const Ellipse & ellipse)
        {
            solderDots_.push_back(ellipse);
        }

        std::vector<const NetLines*> NetShapes::getNetLines() const
        {
            std::vector<const NetLines*> netConstPointers;
            for(NetLineMap::const_iterator i = netLineMap_.begin(); i != netLineMap_.end(); i++) {
                const NetLines & netLines = i->second;
                netConstPointers.push_back(&netLines);
            }
            return netConstPointers;
        }

        std::vector<const Ellipse*> NetShapes::getSolderDots() const
        {
            std::vector<const Ellipse*> ellipsiods;
            for(auto& it: solderDots_)
            {
                ellipsiods.push_back(&it);
            }
            return ellipsiods;
        }

        void NetShapes::removeLine(const NetId& netId, const Line& line)
        {
            assert(has(netId));
            NetLines& netLines = netLineMap_[netId];
            netLines.remove(line);
        }

        void NetShapes::removeNetLines(const NetId& netId)
        {
            assert(has(netId));
            netLineMap_.erase(netId);
        }

        void NetShapes::setEvaluatedNetId(const NetId& defaultNetId, const NetId& evaluatedNetId)
        {
            NetLines& netLines = netLineMap_.at(defaultNetId);
            netLines.setEvaluatedNetId(evaluatedNetId);
        }

        void NetShapes::moveX(int xOffset)
        {
            for(NetLineMap::iterator it = netLineMap_.begin(); it != netLineMap_.end(); it++)
            {
                NetLines& netLine = it->second;
                netLine.moveX(xOffset);
            }

            for(EllipseVector::iterator it = solderDots_.begin(); it != solderDots_.end(); it++)
            {
                Ellipse& ellipse = *it;
                ellipse.moveX(xOffset);
            }
        }

        void NetShapes::moveY(int yOffset)
        {
            for(NetLineMap::iterator it = netLineMap_.begin(); it != netLineMap_.end(); it++)
            {
                NetLines& netLine = it->second;
                netLine.moveY(yOffset);
            }

            for(EllipseVector::iterator it = solderDots_.begin(); it != solderDots_.end(); it++)
            {
                Ellipse& ellipse = *it;
                ellipse.moveY(yOffset);
            }
        }

        void NetShapes::mirrorX(int mirrorAxisY)
        {
            for(NetLineMap::iterator it = netLineMap_.begin(); it != netLineMap_.end(); it++)
            {
                NetLines& netLine = it->second;
                netLine.mirrorX(mirrorAxisY);
            }

            for(EllipseVector::iterator it = solderDots_.begin(); it != solderDots_.end(); it++)
            {
                Ellipse& ellipse = *it;
                ellipse.mirrorX(mirrorAxisY);
            }
        }

        void NetShapes::mirrorY(int mirrorAxisX)
        {
            for(NetLineMap::iterator it = netLineMap_.begin(); it != netLineMap_.end(); it++)
            {
                NetLines& netLine = it->second;
                netLine.mirrorY(mirrorAxisX);
            }

            for(EllipseVector::iterator it = solderDots_.begin(); it != solderDots_.end(); it++)
            {
                Ellipse& ellipse = *it;
                ellipse.mirrorY(mirrorAxisX);
            }
        }

        void NetShapes::transform(const Transformation& transformation, const Point& center)
        {
            for(NetLineMap::iterator it = netLineMap_.begin(); it != netLineMap_.end(); it++)
            {
                NetLines& netLine = it->second;
                netLine.transform(transformation, center);
            }

            for(EllipseVector::iterator it = solderDots_.begin(); it != solderDots_.end(); it++)
            {
                Ellipse& ellipse = *it;
                ellipse.transform(transformation, center);
            }
        }

        void NetShapes::merge(const NetShapes& other)
        {

            for(NetLineMap::const_iterator it = other.netLineMap_.begin(); it != other.netLineMap_.end(); it++)
            {
                NetId otherNetId = it->first;
                NetLines otherNetLines = it->second;

                if(netLineMap_.find(otherNetId) != netLineMap_.end()) {
                    NetLines netLines = netLineMap_.find(otherNetId)->second;
                    netLines.merge(otherNetLines);
                    netLineMap_.erase(otherNetId);
                    netLineMap_.insert(NetLineMap::value_type(otherNetId, netLines));
                } else {
                    netLineMap_.insert(NetLineMap::value_type(otherNetId, otherNetLines));
                }
            }

            solderDots_.insert(solderDots_.end(), other.solderDots_.begin(), other.solderDots_.end());
        }
    }
}


