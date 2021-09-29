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

#include "Core/incl/Schematic/Symbols/SymbolMaster.h"
#include "Core/incl/Schematic/Symbols/Terminal.h"
#include "Log/incl/LogMacros.h"

namespace Core {
    namespace Schematic {

        SymbolMaster::SymbolMaster()
        {
        }

        SymbolMaster::SymbolMaster(const SymbolMaster & other):
                cellTriple_(other.cellTriple_),
                symbolArcVector_(other.symbolArcVector_),
                symbolEllipseVector_(other.symbolEllipseVector_),
                symbolLineVector_(other.symbolLineVector_),
                symbolPolygonVector_(other.symbolPolygonVector_),
                symbolRectVector_(other.symbolRectVector_),
                symbolTextVector_(other.symbolTextVector_),
                symbolEvalTextVector_(other.symbolEvalTextVector_),
                pinAssignmentPositions_(other.pinAssignmentPositions_),
                boundingBox_(other.boundingBox_),
                terminalMap_(other.terminalMap_)
        {
        }

        void SymbolMaster::addTerminal(const Terminal& terminal)
        {
            assert(!hasTerminal(terminal.getTerminalId()));
            terminalMap_.insert(TerminalMap::value_type(terminal.getTerminalId(), terminal));
        }

        void Core::Schematic::SymbolMaster::setCellTriple(const CellTripleId& cellTriple)
        {
            cellTriple_ = cellTriple;
        }

        CellTripleId Core::Schematic::SymbolMaster::getCellTriple() const
        {
            assert(cellTriple_.isInitialized());
            return cellTriple_;
        }

        void SymbolMaster::addSymbolArc(const Arc& arc)
        {
            symbolArcVector_.push_back(arc);
        }

        void SymbolMaster::addSymbolEllipse(const Ellipse& ellipse)
        {
            symbolEllipseVector_.push_back(ellipse);
        }


        void SymbolMaster::addSymbolLine(const Line& line)
        {
            symbolLineVector_.push_back(line);
        }


        void SymbolMaster::addSymbolRect(const Rect& rect)
        {
            symbolRectVector_.push_back(rect);
        }

        void SymbolMaster::addSymbolPolygon(const Polygon& polygon)
        {
            symbolPolygonVector_.push_back(polygon);
        }

        void SymbolMaster::addSymbolText(const Text& text)
        {
            symbolTextVector_.push_back(text);
        }

        void SymbolMaster::addSymbolEvalText(const EvalText & evalText)
        {
            symbolEvalTextVector_.push_back(evalText);
        }

        void SymbolMaster::addPinAssignmentPosition(const TerminalId& termId, const Point& position)
        {
            pinAssignmentPositions_.insert(PinAssignmentPositions::value_type(termId, position));
        }

        void SymbolMaster::setBoundingBox(const Rect& boundingBox)
        {
            boundingBox_ = boundingBox;
        }

        std::vector<const Rect*> SymbolMaster::getSymbolRects() const
        {
            std::vector<const Rect*> rects;
            for(auto& it: symbolRectVector_) {
                const Rect& rect = it;
                rects.push_back(&rect);
            }
            return rects;
        }

        bool SymbolMaster::hasTerminal(const TerminalId& terminalId) const
        {
            return terminalMap_.find(terminalId) != terminalMap_.end();
        }

        Terminal SymbolMaster::findTerminal(const TerminalId& terminalId) const
        {
            assert(hasTerminal(terminalId));
            return terminalMap_.find(terminalId)->second;
        }

        std::vector<const Arc*> SymbolMaster::getSymbolArcs() const
        {
            std::vector<const Arc*> arcs;
            for(auto& it : symbolArcVector_)
            {
                const Arc & arc = it;
                arcs.push_back(&arc);
            }
            return arcs;
        }

        std::vector<const Ellipse*> SymbolMaster::getSymbolEllipses() const
        {
            std::vector<const Ellipse*> ellipses;
            for(auto& it : symbolEllipseVector_)
            {
                const Ellipse & ellipse = it;
                ellipses.push_back(&ellipse);
            }
            return ellipses;
        }

        std::vector<const Line*> SymbolMaster::getSymbolLines() const
        {
            std::vector<const Line*> lines;
            for(auto& it : symbolLineVector_)
            {
                const Line & line = it;
                lines.push_back(&line);
            }
            return lines;
        }

        std::vector<const Polygon*> SymbolMaster::getSymbolPolygons() const
        {
            std::vector<const Polygon*> polygons;
            for(auto& it : symbolPolygonVector_)
            {
                const Polygon & polygon = it;
                polygons.push_back(&polygon);
            }
            return polygons;
        }

        std::vector<const Text*> SymbolMaster::getSymbolTexts() const
        {
            std::vector<const Text*> texts;
            for(auto& it : symbolTextVector_)
            {
                const Text & text = it;
                texts.push_back(&text);
            }
            return texts;
        }


        std::vector<const EvalText*> SymbolMaster::getSymbolEvalTexts() const
        {
            std::vector<const EvalText*> evalTexts;
            for(auto& it : symbolEvalTextVector_)
            {
                const EvalText& evalText = it;
                evalTexts.push_back(&evalText);
            }
            return evalTexts;
        }

        Rect SymbolMaster::getBoundingBox() const
        {
            return boundingBox_;
        }

        Core::Schematic::Point SymbolMaster::getPinAssignmentPosition(const Core::TerminalId& termId) const
        {
            return pinAssignmentPositions_.find(termId)->second;
        }

    }
}
