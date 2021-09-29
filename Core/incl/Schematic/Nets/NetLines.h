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
#ifndef CORE_SCHEMATIC_NETLINES_H_
#define CORE_SCHEMATIC_NETLINES_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Schematic/Shapes/Point.h"
#include "Core/incl/Schematic/Shapes/Line.h"
#include "Core/incl/Schematic/Shapes/Text.h"
#include "Core/incl/Schematic/Nets/BitNets.h"
#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include "Core/incl/Common/Object.h"
#include <vector>

namespace Core {
    namespace Schematic {

        class NetLines : public Object
        {
        private:
            typedef std::vector<Text> NetLabels;

        public:
            NetLines();

            void setNetIdentifier(const NetId & netIdentifier);
            void addLabel(const Text& text);
            void setNetAssignment(const NetAssignment& netAssignment);
            void insertLine(const Line & line);
            void setBitNets(const BitNets& bitNets);
            void setIsBusNet(bool busNet);
            void setIsGlobal(bool global);
            void setEvaluatedNetId(const NetId& netId);

            void changeLabelText(const NetId& netId);
            DeviceId computeSymbolInstanceIdWithTerminal(const Point& startPoint, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstance) const;
            std::vector<const Line*> findLinesConnectedTo(const Point& position) const;
            bool getPointsBetweenTwoPoints(const Point& startPoint,const Point& endPoint, std::vector<const Point*>& result);

            std::vector<const Line*> getLines() const;
            std::vector<const Text*> getLabels() const;
            NetId getNetId() const;
            NetId getEvaluatedNetId() const;
            NetAssignment getNetAssignment() const;
            BitNets getBitNets() const;

            bool hasPoint(const Point& point) const;
            bool hasNetAssignment() const;
            bool hasBitNets() const;
            bool isBusNet() const;
            bool isGlobal() const;

            void remove(const Line& line);
            void removeNetLabels();
            std::string toStr() const;

            void moveX(int xOffset);
            void moveY(int yOffset);
            void mirrorX(int mirrorAxisY);
            void mirrorY(int mirrorAxisX);
            void transform(const Transformation& transformation, const Point& center);
            void merge(const NetLines& other);

        private:
            typedef std::vector<Line> LineVector;

            void findLinesConnectedToRecursivly(const Point& position, std::vector<const Line*>& lines) const;
            void computeIfSymbolInstancesHaveTerminalWithPoint(DeviceId& componentId, const Point& startPoint, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstances) const;
            void computeSymbolInstanceIdWithTerminalRecursively(DeviceId& componentId, const Point& startPoint, std::vector<const Line*>& visitedLines, const TerminalId& terminalId, const std::vector<const SymbolInstance*>& symbolInstances) const;
            std::vector<const Line*> computeLinesWithPoint(const Point& point) const;
            std::vector<Line*> computeNonConstLinesWithPoint(const Point& startPoint);

            NetId netIdentifier_;
            NetId evaluatedIdentifier_;
            NetAssignment netAssignment_;

            LineVector lineVector_;
            NetLabels labels_;
            BitNets bitNets_;
            bool global_;
            bool busNet_;
        };
    }
}




#endif
