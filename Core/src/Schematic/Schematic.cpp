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

#include "Core/incl/Schematic/Schematic.h"
#include "Core/incl/Schematic/Shapes/Ellipse.h"
#include "Core/incl/Schematic/Shapes/Line.h"
#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include "Core/incl/Schematic/Symbols/SymbolMaster.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core{
    namespace Schematic {

        Schematic::Schematic() :
                initializedFlag_(false)
        {
        }

        Schematic::~Schematic()
        {
        }

        void Schematic::initializeEmpty()
        {
            initializedFlag_ = true;
        }

        bool Schematic::isInitialized() const
        {
            return initializedFlag_;
        }

        void Schematic::setSchematicName(const std::string& schematicName)
        {
            schematicName_ = schematicName;
        }

        void Schematic::setCellTriple(const CellTripleId& cellTriple)
        {
            cellTriple_ = cellTriple;
        }

        void Schematic::addNetLines(const NetId& netId, NetLines& netLines)
        {
            netShapes_.addNetLines(netId, netLines);
        }

        void Schematic::addSolderDot(const Ellipse& ellipse)
        {
            assert(isInitialized());
            netShapes_.addSolderDot(ellipse);
        }

        void Schematic::addSymbolMaster(const CellTripleId& cellTriple, SymbolMaster * symbolMaster)
        {
            assert(isInitialized());
            symbols_.addSymbolMaster(cellTriple, symbolMaster);
        }

        void Schematic::addSymbolInstance(const DeviceId& componentId, SymbolInstance * symbolInstance)
        {
            assert(isInitialized());
            symbols_.addSymbolInstance(componentId, symbolInstance);
        }

        void Schematic::addIteratedInstance(const DeviceId& componentId, IteratedInstance* iteratedInstance)
        {
            assert(isInitialized());
            symbols_.addIteratedInstance(componentId, iteratedInstance);
        }


        void Schematic::addLabel(const Text& text)
        {
            pinLabels_.addPinLabel(text);
        }

        void Schematic::addEvaluatedNetId(const NetId& defaultNetId, const NetId& evaluatedNetId)
        {
            getNetShapes().setEvaluatedNetId(defaultNetId, evaluatedNetId);
        }

        void Schematic::addEvaluatedPinAssignmentId(const DeviceId& componentId,
                const TerminalId& terminalId, const NetId& evaluatedNetId)
        {
            getSymbols().setEvaluatedPinAssignmentId(componentId, terminalId,  evaluatedNetId);
        }

        bool Schematic::hasSymbolMaster(const CellTripleId& cellTriple) const
        {
            assert(isInitialized());
            return symbols_.hasSymbolMaster(cellTriple);
        }

        bool Schematic::hasNetLines(const NetId& netId) const
        {
            assert(isInitialized());
            return getNetShapes().has(netId);
        }

        bool Schematic::hasSymbolInstance(const DeviceId& componentId) const
        {
            assert(isInitialized());
            return getSymbols().hasSymbolInstance(componentId);
        }

        bool Schematic::hasGlobalNet(const NetId& netId) const
        {
            assert(isInitialized());
            return getNetShapes().hasGlobalNet(netId);
        }

        NetLines& Schematic::findNetLines(const NetId& netId)
        {
            assert(hasNetLines(netId));
            return getNetShapes().findNetLines(netId);
        }

        const NetLines& Schematic::findNetLines(const NetId& netId) const
        {
            assert(hasNetLines(netId));
            return getNetShapes().findNetLines(netId);
        }

        SymbolInstance& Schematic::findSymbolInstance(const DeviceId& componentId)
        {
            assert(hasSymbolInstance(componentId));
            return getSymbols().findSymbolInstance(componentId);
        }

        const SymbolInstance& Schematic::findSymbolInstance(const DeviceId& componentId) const
        {
            assert(hasSymbolInstance(componentId));
            return getSymbols().findSymbolInstance(componentId);
        }

        SymbolMaster & Schematic::findSymbolMaster(const CellTripleId& cellTriple)
        {
            assert(isInitialized());
            return symbols_.findSymbolMaster(cellTriple);
        }

		void Schematic::addTerminalId(const TerminalId& terminalId)
		{
			terminalIds_.push_back(terminalId);
		}

		std::vector<TerminalId> Schematic::getTerminalIds() const
		{
			return terminalIds_;
		}

        std::vector<NetId> Schematic::getNetIds() const
        {
        	return netShapes_.getNetIds();
        }

        const SymbolMaster& Schematic::findSymbolMaster(const CellTripleId & cellTriple) const
        {
            assert(isInitialized());
            return symbols_.findSymbolMaster(cellTriple);
        }

        std::vector<const NetLines*> Schematic::getNetLines() const
        {
            return getNetShapes().getNetLines();
        }

        std::vector<const Ellipse*> Schematic::getSolderDots() const
        {
            return getNetShapes().getSolderDots();
        }

        std::vector<const SymbolInstance*> Schematic::computeSymbolInstances(const TerminalId& terminalId) const
        {
            return getSymbols().computeSymbolInstancesWithTerminal(terminalId);
        }

        std::vector<const SymbolInstance*> Schematic::getSymbolInstances() const
        {
            return getSymbols().getSymbolInstances();
        }

        std::vector<const IteratedInstance*> Schematic::getIteratedInstances() const
        {
            return getSymbols().getIteratedInstances();
        }

        std::string Schematic::getSchematicName() const
        {
            return schematicName_;
        }

        CellTripleId Schematic::getCellTriple() const
        {
            assert(cellTriple_.isInitialized());
            return cellTriple_;
        }

        NetShapes& Schematic::getNetShapes()
        {
            assert(isInitialized());
            return netShapes_;
        }

        const NetShapes & Schematic::getNetShapes() const
        {
            assert(isInitialized());
            return netShapes_;
        }

        void Schematic::computeBoundingBoxTopLeft(const Point& point, Point& topLeft) const
        {
            if(point.getX() < topLeft.getX()) {
                topLeft.setX(point.getX());
            }

            if(point.getY() > topLeft.getY()) {
                topLeft.setY(point.getY());
            }
        }

        void Schematic::computeBoundingBoxBottomRight(const Point& point, Point& bottomRight) const
        {
            if(point.getX() > bottomRight.getX()) {
                bottomRight.setX(point.getX());
            }

            if(point.getY() < bottomRight.getY()) {
                bottomRight.setY(point.getY());
            }
        }

        Symbols& Schematic::getSymbols()
        {
            assert(isInitialized());
            return symbols_;
        }

        const Symbols & Schematic::getSymbols() const
        {
            assert(isInitialized());
            return symbols_;
        }

        const PinLabels& Schematic::getPinLabels() const
        {
            assert(isInitialized());
            return pinLabels_;
        }

        std::vector<const Point*> Schematic::getPointsBetweenTwoPoints(Point& point1, Point& point2) const
        {
        	return getNetShapes().getPointsBetweenTwoPoints(point1, point2);
        }

        Point Schematic::getPointWithinBoundingBox(const Rect& rect, const NetId& netId) const
        {
        	Point point;
    		const Core::Schematic::NetLines netLine = getNetShapes().findNetLines(netId);
			std::vector<const Line*> lines = netLine.getLines();
			for(auto& it : lines)
			{
				const Line* line = it;
				if(rect.hasPointInside(line->getStartPoint())){
					point = line->getStartPoint();
					break;
				}
				else if(rect.hasPointInside(line->getEndPoint())){
					point = line->getEndPoint();
					break;
				}
    		}
    		return point;
        }
    }
}
