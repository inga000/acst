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

#include "Core/incl/Schematic/Symbols/SymbolInstance.h"
#include "Core/incl/Schematic/Symbols/Terminal.h"
#include "Core/incl/Schematic/Symbols/TransformationMatrices.h"
#include "Core/incl/Common/BacktraceAssert.h"

namespace Core {
    namespace Schematic {

        SymbolInstance::SymbolInstance():
                symbolMaster_(NULL)
        {
        }

        SymbolInstance::~SymbolInstance()
        {
        }

        SymbolInstance::SymbolInstance(const SymbolInstance& other):
                    symbolMaster_(other.symbolMaster_),
                    position_(other.position_),
                    transformation_(other.transformation_),
                    componentId_(other.componentId_),
                    symbolName_(other.symbolName_),
                    propertyMap_(other.propertyMap_),
                    pinAssignmentMap_(other.pinAssignmentMap_),
                    terminalMap_(other.terminalMap_)
        {
        }

        CellTripleId SymbolInstance::getCellTriple() const
        {
            return getSymbolMaster().getCellTriple();
        }

        void SymbolInstance::setSymbolMaster(const SymbolMaster& symbolMaster)
        {
            symbolMaster_ = &symbolMaster;
        }

        void SymbolInstance::setPosition(const Point & position)
        {
            position_ = position;
        }

        void SymbolInstance::setTransformation(const Transformation & transformation)
        {
            transformation_ = transformation;
        }

        void SymbolInstance::setComponentId(const Core::DeviceId & componentId)
        {
            componentId_ = componentId;
        }

        void SymbolInstance::setSymbolName(const SymbolName & symbolName)
        {
            symbolName_ = symbolName;
        }

        void SymbolInstance::setEvaluatedPinAssignmentId(const TerminalId& terminalId,
                const NetId& evaluatedNetId)
        {
            pinAssignmentMap_.setEvaluatedNetId(terminalId, evaluatedNetId);
        }

        void SymbolInstance::updatePinAssignmentMap(const PinAssignmentMap& pinAssignmentMap)
        {
            pinAssignmentMap_ = pinAssignmentMap;
        }

        std::string SymbolInstance::toStr() const
        {
            std::ostringstream oss;
            oss << getDeviceId();
            return oss.str();
        }

        void SymbolInstance::addTerminal(const TerminalId& terminalId, const NetId& netId)
        {
            terminalMap_.insert(TerminalMap::value_type(terminalId, netId));
        }

        void SymbolInstance::addPinAssignment(const TerminalId& termId, const PinAssignment& pinAssignment)
        {
            pinAssignmentMap_.add(termId, pinAssignment);
        }

        void SymbolInstance::addPropertyValue(const PropertyValue& valueProperty)
        {
            getPropertyMap().addValue(valueProperty);
        }

        void SymbolInstance::addPropertyValues(const std::vector<PropertyValue>& propertyValues)
        {
            getPropertyMap().addValues(propertyValues);
        }

        void SymbolInstance::addPropertyAssignment(const PropertyAssignment& assignmentProperty)
        {
            getPropertyMap().addAssignment(assignmentProperty);
        }

        void SymbolInstance::addPropertyAssignments(const std::vector<PropertyAssignment>& propertyAssignments)
        {
            getPropertyMap().addAssignments(propertyAssignments);
        }

        bool SymbolInstance::hasTerminal(const TerminalId& terminalId) const
        {
            return terminalMap_.find(terminalId) != terminalMap_.end();
        }

        bool SymbolInstance::hasTerminalWithPosition(const TerminalId& terminalId, const Point& point) const
        {
            bool terminalWithPosition = false;
            if(hasTerminal(terminalId)) {
                Point terminalPosition = computeAbsoluteTerminalPosition(terminalId);
                if(terminalPosition == point) {
                    terminalWithPosition = true;
                }
            }
            return terminalWithPosition;
        }

        bool SymbolInstance::hasPropertyValue(const PropertyName& propertyName) const
        {
            return getPropertyMap().hasValue(propertyName);
        }

        bool SymbolInstance::hasPropertyAssignment(const PropertyName& propertyName) const
        {
            return getPropertyMap().hasAssignment(propertyName);
        }

        bool SymbolInstance::hasPinAssignments() const
        {
            return pinAssignmentMap_.hasPinAssignments();
        }

        Point SymbolInstance::computeAbsoluteTerminalPosition(const TerminalId& terminalId) const
        {
            assert(hasTerminal(terminalId));
            const Terminal& terminal = getSymbolMaster().findTerminal(terminalId);
            Core::Schematic::Point relativeTerminalPositions = terminal.getPosition();
            return getTransformation()*relativeTerminalPositions + getPosition();
        }

        Point SymbolInstance::getRelativeTerminalPosition(const TerminalId& terminalId) const
        {
            assert(hasTerminal(terminalId));
            const Terminal& terminal = getSymbolMaster().findTerminal(terminalId);
            return terminal.getPosition();
        }

        NetId SymbolInstance::findConnectedNet(const TerminalId& terminalId) const
        {
            assert(hasTerminal(terminalId));
            return terminalMap_.find(terminalId)->second;
        }

        NetId SymbolInstance::findPropertyValue(const PropertyName& propertyName) const
        {
            return getPropertyMap().findValue(propertyName);
        }

        NetAssignment SymbolInstance::findPropertyAssignment(const PropertyName& propertyName) const
        {
            return getPropertyMap().findAssignment(propertyName);
        }

        std::vector<PropertyValue> SymbolInstance::getPropertyValues() const
        {
            return getPropertyMap().getPropertyValues();
        }

        std::vector<PropertyAssignment> SymbolInstance::getPropertyAssignments() const
        {
            return getPropertyMap().getPropertyAssignments();
        }

        Terminal SymbolInstance::findTerminal(const TerminalId& terminalId) const
        {
            assert(hasTerminal(terminalId));
            Terminal terminal = getSymbolMaster().findTerminal(terminalId);
            Point position = computeAbsoluteTerminalPosition(terminalId);
            terminal.setPosition(position);
            terminal.updateBoundingBoxAfterPositionChanged();
            return terminal;
        }

        std::vector<TerminalId> SymbolInstance::getPinAssignmentTerminalIds() const
        {
            return getPinAssignmentMap().getPinAssignmentTerminalIds();
        }

        const SymbolMaster& SymbolInstance::getSymbolMaster() const
        {
            assert(symbolMaster_ != NULL);
            return *symbolMaster_;
        }

        Point SymbolInstance::getPosition() const
        {
            return position_;
        }

        Transformation SymbolInstance::getTransformation() const
        {
            return transformation_;
        }

        Core::DeviceId SymbolInstance::getDeviceId() const
        {
            return componentId_;
        }

        SymbolName SymbolInstance::getSymbolName() const
        {
            return symbolName_;
        }

        std::vector<const PinAssignment*> SymbolInstance::getPinAssignments() const
        {
            return pinAssignmentMap_.getPinAssignments();
        }

        PinAssignment SymbolInstance::getPinAssignment(const TerminalId& termId) const
        {
            return pinAssignmentMap_.getAssignment(termId);
        }

        Rect SymbolInstance::computeBoundingBox() const
        {
            Rect boundingBox = getSymbolMaster().getBoundingBox();;
            boundingBox = getTransformation()*boundingBox + getPosition();
            return boundingBox;
        }

        bool SymbolInstance::hasBitInstances() const
        {
            return false;
        }

        PropertyMap& SymbolInstance::getPropertyMap()
        {
            return propertyMap_;
        }

        const PropertyMap& SymbolInstance::getPropertyMap() const
        {
            return propertyMap_;
        }

        PinAssignmentMap& SymbolInstance::getPinAssignmentMap()
        {
            return pinAssignmentMap_;
        }

        const PinAssignmentMap& SymbolInstance::getPinAssignmentMap() const
        {
            return pinAssignmentMap_;
        }

}
}
