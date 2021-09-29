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

#ifndef CORE_SCHEMATIC_SYMBOLMASTER_H_
#define CORE_SCHEMATIC_SYMBOLMASTER_H_

#include "Core/incl/Schematic/Shapes/Arc.h"
#include "Core/incl/Schematic/Shapes/Ellipse.h"
#include "Core/incl/Schematic/Shapes/Line.h"
#include "Core/incl/Schematic/Shapes/Rect.h"
#include "Core/incl/Schematic/Shapes/Polygon.h"
#include "Core/incl/Schematic/Shapes/Text.h"
#include "Core/incl/Schematic/Shapes/EvalText.h"
#include "Core/incl/Schematic/Shapes/Point.h"
#include "Core/incl/Schematic/Symbols/Terminal.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/CellTripleId.h"
#include <vector>

namespace Core {
    namespace Schematic {

        class Terminal;

        class SymbolMaster
        {
        private:
            typedef std::vector<Arc> SymbolArcVector;
            typedef std::vector<Ellipse> SymbolEllipseVector;
            typedef std::vector<Line> SymbolLineVector;
            typedef std::vector<Rect> SymbolRectVector;
            typedef std::vector<Polygon> SymbolPolygonVector;
            typedef std::vector<Text> SymbolTextVector;
            typedef std::vector<EvalText> SymbolEvalTextVector;
            typedef std::map<TerminalId, Point> PinAssignmentPositions;
            typedef std::map<TerminalId, Terminal> TerminalMap;

        private:
            void operator=(const SymbolMaster & other); // not implemented

        public:
            SymbolMaster();
            SymbolMaster(const SymbolMaster & other);

            void setCellTriple(const CellTripleId& cellTriple);
            CellTripleId getCellTriple() const;

            void addTerminal(const Terminal& terminal);
            void addSymbolRect(const Rect & rect);
            void addSymbolLine(const Line & line);
            void addSymbolArc(const Arc & arc);
            void addSymbolEllipse(const Ellipse & ellipse);
            void addSymbolPolygon(const Polygon & polygon);
            void addSymbolText(const Text& text);
            void addSymbolEvalText(const EvalText & evalText);
            void addPinAssignmentPosition(const TerminalId& termId, const Point& position);

            void setBoundingBox(const Rect & boundingBox);

            bool hasTerminal(const TerminalId& terminalId) const;
            Terminal findTerminal(const TerminalId& terminalId) const;
            std::vector<const Arc*> getSymbolArcs() const;
            std::vector<const Ellipse*> getSymbolEllipses() const;
            std::vector<const Line*> getSymbolLines() const;
            std::vector<const Polygon*> getSymbolPolygons() const;
            std::vector<const Rect*> getSymbolRects() const;
            std::vector<const Text*> getSymbolTexts() const;
            std::vector<const EvalText*> getSymbolEvalTexts() const;
            Rect getBoundingBox() const;
            Point getPinAssignmentPosition(const TerminalId& termId) const;

        private:
            CellTripleId cellTriple_;
            SymbolArcVector symbolArcVector_;
            SymbolEllipseVector symbolEllipseVector_;
            SymbolLineVector symbolLineVector_;
            SymbolPolygonVector symbolPolygonVector_;
            SymbolRectVector symbolRectVector_;
            SymbolTextVector symbolTextVector_;
            SymbolEvalTextVector symbolEvalTextVector_;
            PinAssignmentPositions pinAssignmentPositions_;
            Rect boundingBox_;
            TerminalMap terminalMap_;
        };
    }
}
#endif
