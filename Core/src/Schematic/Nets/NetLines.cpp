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

#include "Core/incl/Schematic/Nets/NetLines.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        NetLines::NetLines():
                global_(false),
                busNet_(false)
        {
        }

        void NetLines::setNetIdentifier(const NetId & netIdentifier)
        {
            netIdentifier_ = netIdentifier;
        }

        void Core::Schematic::NetLines::addLabel(const Text& text)
        {
            labels_.push_back(text);
        }

        void NetLines::setNetAssignment(const NetAssignment& netAssignment)
        {
            netAssignment_ = netAssignment;
        }

        void NetLines::insertLine(const Line & line)
        {
            lineVector_.push_back(line);
        }

        void NetLines::setBitNets(const BitNets& bitNets)
        {
            bitNets_ = bitNets;
        }

        void NetLines::setIsBusNet(bool busNet)
        {
            busNet_ = busNet;
        }

        void NetLines::setIsGlobal(bool global)
        {
            global_ = global;
        }

        void NetLines::setEvaluatedNetId(const NetId& netId)
        {
            evaluatedIdentifier_ = netId;
        }

       void NetLines::changeLabelText(const NetId& netId)
       {
           for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
           {
               Text& text = *it;
               text.setText(netId.getName());
           }
       }

        DeviceId NetLines::computeSymbolInstanceIdWithTerminal(const Point& startPoint, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstances) const
        {
            DeviceId componentId;
            std::vector<const Line*> visitedLines;
            computeSymbolInstanceIdWithTerminalRecursively(componentId, startPoint, visitedLines, terminalId, symbolInstances);
            return componentId;
        }

        std::vector<const Line*> NetLines::findLinesConnectedTo(const Point& position) const
        {
            std::vector<const Line*> visitedLines;
            std::vector<const Line*> linesWithPoint = computeLinesWithPoint(position);
            for(auto& it : linesWithPoint)
            {
                const Line& line = *it;
                if(std::find(visitedLines.begin(), visitedLines.end(), &line) == visitedLines.end()) {
                    visitedLines.push_back(&line);
                    Point nextStartPoint = line.getOtherEnd(position);
                    findLinesConnectedToRecursivly(nextStartPoint, visitedLines);
                }
            }
            return visitedLines;
        }

        void NetLines::findLinesConnectedToRecursivly(const Point& position, std::vector<const Line*>& visitedLines) const
        {
            std::vector<const Line*> linesWithPoint = computeLinesWithPoint(position);
            for(auto& it : linesWithPoint)
            {
                const Line& line = *it;
                if(std::find(visitedLines.begin(), visitedLines.end(), &line) == visitedLines.end()) {
                    visitedLines.push_back(&line);
                    Point nextStartPoint = line.getOtherEnd(position);
                    findLinesConnectedToRecursivly(nextStartPoint, visitedLines);
                }
            }
        }

        void NetLines::computeSymbolInstanceIdWithTerminalRecursively(DeviceId& componentId, const Point& startPoint, std::vector<const Line*>& visitedLines, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstances) const
        {
            std::vector<const Line*> lines = computeLinesWithPoint(startPoint);
            for(auto& it : lines)
            {
                if(componentId.isInitialized()) {
                    return;
                }

                const Line& line = *it;
                if(std::find(visitedLines.begin(), visitedLines.end(), &line) != visitedLines.end())
                {
                    continue;
                } else {
                    visitedLines.push_back(&line);
                    Point nextStartPoint = line.getOtherEnd(startPoint);
                    computeIfSymbolInstancesHaveTerminalWithPoint(componentId, nextStartPoint, terminalId, symbolInstances);
                    if(componentId.isInitialized()) {
                        return;
                    } else {
                        computeSymbolInstanceIdWithTerminalRecursively(componentId, nextStartPoint, visitedLines, terminalId, symbolInstances);
                    }
                }
            }
        }

        void NetLines::computeIfSymbolInstancesHaveTerminalWithPoint(DeviceId& componentId, const Point& point, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstances) const
        {
            for(auto& it : symbolInstances)
            {
                const SymbolInstance& currentSymbolInstance = *it;
                if(currentSymbolInstance.hasTerminalWithPosition(terminalId, point)) {
                    componentId = currentSymbolInstance.getDeviceId();
                }
            }
        }

        std::vector<const Line*> NetLines::computeLinesWithPoint(const Point& startPoint) const
        {
            std::vector<const Line*> lines;
            for(auto& it : lineVector_)
            {
                const Line* line = &it;
                if(line->hasPoint(startPoint)) {
                    lines.push_back(line);
                }
            }
            return lines;
        }

        std::vector<Line*> NetLines::computeNonConstLinesWithPoint(const Point& startPoint)
        {
            std::vector<Line*> lines;
            for(auto& it : lineVector_)
            {
                Line* line = &it;
                if(line->hasPoint(startPoint)) {
                    lines.push_back(line);
                }
            }
            return lines;
        }

        bool NetLines::getPointsBetweenTwoPoints(const Point& startPoint,const Point& endPoint, std::vector<const Point*>& result)
        {
        	bool k;
        	std::vector<Line*> lineConnectedTo1 = computeNonConstLinesWithPoint(startPoint);
        	for(auto& it : lineConnectedTo1)
        	{
				Line& line = *it;
				Point* endPointOfLine = new Point();

				if(line.getEndPoint() == startPoint)
				{
					*endPointOfLine = line.getStartPoint();
				} else {
					*endPointOfLine = line.getEndPoint();
				}

				if(line.visited_ == false){
					line.visited_ = true;
					result.push_back(endPointOfLine);

					if(*endPointOfLine == endPoint){
						k = true;
						break;
					}
					else{
						k = getPointsBetweenTwoPoints(*endPointOfLine, endPoint, result);
						if(k == true)
							break;
					}
				}
				else{
					k = false;
				}
			}
        	if(k == false)
        		result.pop_back();
        	return k;
        }

        std::vector<const Line*> NetLines::getLines() const
        {
            std::vector<const Line*> lines;
            for(auto& it : lineVector_)
            {
                lines.push_back(&it);
            }
            return lines;
        }

        std::vector<const Text*> NetLines::getLabels() const
        {
            std::vector<const Text*> labelPointers;
            for(auto& it : labels_)
            {
                labelPointers.push_back(&it);
            }
            return labelPointers;
        }

        NetId NetLines::getNetId() const
        {
            assert(netIdentifier_.isInitialized());
            return netIdentifier_;
        }

        NetId NetLines::getEvaluatedNetId() const
        {
            NetId netId;
            if(evaluatedIdentifier_.isInitialized()) {
                netId = evaluatedIdentifier_;
            } else {
                netId = netIdentifier_;
            }
            return netId;
        }

        NetAssignment NetLines::getNetAssignment() const
        {
            assert(hasNetAssignment());
            return netAssignment_;
        }

        BitNets NetLines::getBitNets() const
        {
            assert(bitNets_.isInitialized());
            return bitNets_;
        }
        bool NetLines::hasPoint(const Point& point) const
        {
            bool containsPoint = false;
            for(LineVector::const_iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                const Core::Schematic::Line& line = *it;
                if(line.hasPoint(point))
                {
                    containsPoint = true;
                    break;
                }
            }
            return containsPoint;
        }

        bool NetLines::hasNetAssignment() const
        {
            return netAssignment_.isInitialized();
        }

        bool NetLines::hasBitNets() const
        {
            return bitNets_.isInitialized();
        }

        bool NetLines::isBusNet() const
        {
            return busNet_;
        }

        bool NetLines::isGlobal() const
        {
            return global_;
        }

        void NetLines::remove(const Line& line)
        {
            LineVector newLineVector;
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line currentLine = *it;
                if(currentLine != line) {
                    newLineVector.push_back(currentLine);
                }
            }
            lineVector_.empty();
            lineVector_ = newLineVector;
        }

        void NetLines::removeNetLabels()
        {
            labels_.clear();
        }

        std::string NetLines::toStr() const
        {
            std::ostringstream oss;
            oss << getNetId() << ": ";
            for(LineVector::const_iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                const Line& currentLine = *it;
                oss << currentLine;
            }
            return oss.str();
        }

        void NetLines::moveX(int xOffset)
        {
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line& line = *it;
                line.moveX(xOffset);
            }

            for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
            {
                Text& labelText = *it;
                labelText.moveX(xOffset);
            }
        }

        void NetLines::moveY(int yOffset)
        {
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line& line = *it;
                line.moveY(yOffset);
            }

            for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
            {
                Text& labelText = *it;
                labelText.moveY(yOffset);
            }
        }

        void NetLines::mirrorX(int mirrorAxisY)
        {
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line& line = *it;
                line.mirrorX(mirrorAxisY);
            }

            for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
            {
                Text& labelText = *it;
                labelText.mirrorX(mirrorAxisY);
            }
        }

        void NetLines::mirrorY(int mirrorAxisX)
        {
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line& line = *it;
                line.mirrorY(mirrorAxisX);
            }

            for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
            {
                Text& labelText = *it;
                labelText.mirrorY(mirrorAxisX);
            }
        }

        void NetLines::transform(const Transformation& transformation, const Point& center)
        {
            for(LineVector::iterator it = lineVector_.begin(); it != lineVector_.end(); it++)
            {
                Line& line = *it;
                line.transform(transformation, center);
            }

            for(NetLabels::iterator it = labels_.begin(); it != labels_.end(); it++)
            {
                Text& labelText = *it;
                labelText.transform(transformation, center);
            }
        }

        void NetLines::merge(const NetLines& other)
        {
            lineVector_.insert(lineVector_.end(), other.lineVector_.begin(), other.lineVector_.end());
            labels_.insert(labels_.end(), other.labels_.begin(), other.labels_.end());
        }
    }
}
