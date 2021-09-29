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
#ifndef CORE_SCHEMATIC_SYMBOLINSTANCE_H_
#define CORE_SCHEMATIC_SYMBOLINSTANCE_H_

#include "Core/incl/Schematic/Shapes/Point.h"
#include "Core/incl/Schematic/Symbols/SymbolMaster.h"
#include "Core/incl/Schematic/Symbols/BitInstances.h"
#include "Core/incl/Schematic/Symbols/SymbolName.h"
#include "Core/incl/Schematic/Symbols/Transformation.h"
#include "Core/incl/Schematic/PinAssignment.h"
#include "Core/incl/Schematic/PinAssignmentMap.h"
#include "Core/incl/Circuit/Net/NetAssignment.h"
#include "Core/incl/Circuit/Instance/PropertyMap/PropertyMap.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include <map>

namespace Core {

    namespace Schematic {

        class SymbolInstance : public Object
        {
        private:
            typedef std::map<TerminalId, NetId> TerminalMap;

        public:
            SymbolInstance();
            SymbolInstance(const SymbolInstance& other);

            virtual ~SymbolInstance();

            void setSymbolMaster(const SymbolMaster& symbolMaster);
            void setPosition(const Point & position);
            void setTransformation(const Transformation & transformationMatrix);
            void setComponentId(const DeviceId & componentId);
            void setSymbolName(const SymbolName & symbolName);

            void addTerminal(const TerminalId& terminalId, const NetId& netId);
            void addPinAssignment(const TerminalId& termId, const PinAssignment& pinAssignment);

            void addPropertyValue(const PropertyValue& valueProperty);
            void addPropertyValues(const std::vector<PropertyValue>& propertyValues);

            void addPropertyAssignment(const PropertyAssignment& assignmentProperty);
            void addPropertyAssignments(const std::vector<PropertyAssignment>& propertyAssignments);

            bool hasTerminal(const TerminalId& terminalId) const;
            bool hasTerminalWithPosition(const TerminalId& terminalId, const Point& point) const;
            bool hasPropertyValue(const PropertyName& propertyName) const;
            bool hasPropertyAssignment(const PropertyName& propertyName) const;
            bool hasPinAssignments() const;

            Point computeAbsoluteTerminalPosition(const TerminalId& terminalId) const;
            Point getRelativeTerminalPosition(const TerminalId& terminalId) const;
            NetId findConnectedNet(const TerminalId& terminalId) const;

            NetId findPropertyValue(const PropertyName& propertyName) const;
            NetAssignment findPropertyAssignment(const PropertyName& propertyName) const;

            std::vector<PropertyValue> getPropertyValues() const;
            std::vector<PropertyAssignment> getPropertyAssignments() const;

            Terminal findTerminal(const TerminalId& terminalId) const;
            std::vector<TerminalId> getPinAssignmentTerminalIds() const;

            CellTripleId getCellTriple() const;
            const SymbolMaster& getSymbolMaster() const;
            Point getPosition() const;
            Transformation getTransformation() const;
            DeviceId getDeviceId() const;
            SymbolName getSymbolName() const;
            std::vector<const PinAssignment*> getPinAssignments() const;
            PinAssignment getPinAssignment(const TerminalId& termId) const;

            Rect computeBoundingBox() const;
            virtual bool hasBitInstances() const;

            void setEvaluatedPinAssignmentId(const TerminalId& terminalId, const NetId& evaluatedNetId);
            void updatePinAssignmentMap(const PinAssignmentMap& pinAssignmentMap);

            std::string toStr() const;

        protected:
            PropertyMap& getPropertyMap();
            const PropertyMap& getPropertyMap() const;

            PinAssignmentMap& getPinAssignmentMap();
            const PinAssignmentMap& getPinAssignmentMap() const;

        private:
            const SymbolMaster * symbolMaster_;
            Point position_;
            Transformation transformation_;
            DeviceId componentId_;
            SymbolName symbolName_;
            PropertyMap propertyMap_;
            PinAssignmentMap pinAssignmentMap_;
            TerminalMap terminalMap_;
        };
    }
}



#endif
