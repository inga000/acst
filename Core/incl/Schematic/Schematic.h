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

#ifndef CORE_SCHEMATIC_SCHEMATIC_H_
#define CORE_SCHEMATIC_SCHEMATIC_H_

#include "Core/incl/CellTripleId.h"
#include "Core/incl/Schematic/Nets/NetShapes.h"
#include "Core/incl/Schematic/Symbols/Symbols.h"
#include "Core/incl/Schematic/PinLabels.h"
#include "Core/incl/Schematic/Shapes/Text.h"
#include "Core/incl/Schematic/Shapes/Rect.h"

namespace Core {
    namespace Schematic {

        class SymbolMaster;

        class Schematic : public Object
        {
        private:
            typedef std::vector<Text> Labels;

        private:
            Schematic(const Schematic& other); //not implemented
            Schematic& operator=(const Schematic& other); //not implemented

        public:
            Schematic();
            ~Schematic();

            void initializeEmpty();

            void setSchematicName(const std::string & schematicName);
            void setCellTriple(const CellTripleId& cellTriple);

            void addNetLines(const NetId& netId, NetLines& netLines);
            void addSolderDot(const Ellipse & ellipse);

            void addSymbolMaster(const CellTripleId & cellTriple, SymbolMaster * symbolMaster);
            void addSymbolInstance(const DeviceId& componentId, SymbolInstance * symbolInstance);
            void addIteratedInstance(const DeviceId& componentId, IteratedInstance* iteratedInstance);

            void addLabel(const Text& text);
            void addEvaluatedNetId(const NetId& defaultNetId, const NetId& evaluatedNetId);
            void addEvaluatedPinAssignmentId(const DeviceId& componentId, const TerminalId& terminalId,
                    const NetId& evaluatedNetId);

            void addTerminalId(const TerminalId& terminalId);
            std::vector<TerminalId> getTerminalIds() const;
            std::vector<NetId> getNetIds() const;

            bool isInitialized() const;
            bool hasSymbolMaster(const CellTripleId & cellTriple) const;
            bool hasNetLines(const NetId& netId) const;
            bool hasSymbolInstance(const DeviceId& componentId) const;
            bool hasGlobalNet(const NetId& netId) const;

            std::string getSchematicName() const;
            CellTripleId getCellTriple() const;
            std::vector<const NetLines*> getNetLines() const;
            std::vector<const Ellipse*> getSolderDots() const;
            std::vector<const SymbolInstance*> getSymbolInstances() const;
            std::vector<const IteratedInstance*> getIteratedInstances() const;
            const PinLabels& getPinLabels() const;
            std::vector<const Point*> getPointsBetweenTwoPoints(Point& point1, Point& point2) const;

            Point getPointWithinBoundingBox(const Rect& Rect, const NetId& netId) const;

            SymbolMaster& findSymbolMaster(const CellTripleId & cellTriple);
            const SymbolMaster& findSymbolMaster(const CellTripleId & cellTriple) const;
            NetLines& findNetLines(const NetId& netId);
            const NetLines& findNetLines(const NetId& netId) const;
            SymbolInstance& findSymbolInstance(const DeviceId& componentId);
            const SymbolInstance& findSymbolInstance(const DeviceId& componentId) const;

            std::vector<const SymbolInstance*> computeSymbolInstances(const TerminalId& terminalId) const;

        private:
            void computeBoundingBoxTopLeft(const Point& point, Point& topLeft) const;
            void computeBoundingBoxBottomRight(const Point& point, Point& bottomRight) const;

            Symbols& getSymbols();
            const Symbols & getSymbols() const;

            NetShapes& getNetShapes();
            const NetShapes & getNetShapes() const;

        private:
            bool initializedFlag_;
            std::vector<TerminalId> terminalIds_;
            CellTripleId cellTriple_;
            NetShapes netShapes_;
            Symbols symbols_;
            PinLabels pinLabels_;
            std::string schematicName_;
        };
    }
}


#endif
