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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURE_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURE_H_


#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Device/TechType.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"
#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"
#include "StructRec/incl/StructureCircuit/Structure/StructurePins.h"
#include "Core/incl/Circuit/Device/Pin/Pin.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalId.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>
#include <vector>

namespace StructRec {

	class StructureCircuit;
	class StructureCore;
	class StructureCircuits;
	class StructureNet;

 class Structure: public Core::Object
    {
    public:
        Structure();
        ~Structure();

        virtual Structure & clone(StructureCircuit & circuit, StructureCircuits & circuits) const = 0;

        void setStructureId(const StructureId& structureId);
        void setTechType(const Core::TechType & techType);
        void setPersistence(const int persistence);
        void addPin(const StructurePinType & pinType,  StructurePin * pin);

        StructureId getIdentifier() const;
        StructureName getStructureName();
        StructureName getParentStructureName() const;
        const StructureName getStructureName() const;
        Core::TechType getTechType() const;
        int getPersistence() const;
        const StructurePin & findPin(const StructurePinType & pinType) const;
        StructurePin & findPin(const StructurePinType & pinType);
    	void findDevicePin(const StructurePinType& pinType, Core::DeviceId& deviceId, Core::TerminalId& terminalId) const;
    	const Core::Pin& findDevicePin(const StructurePinType& pinType) const;
        std::vector<StructurePin*> findAllPins() const;

        virtual std::vector<const Core::Device*> getDevices() const = 0;
        virtual std::vector<const Structure*> getArrayChildren() const =0;

        const StructureNet & findNet(const StructurePinType & pinType) const;
        StructureNet & findNet(const StructurePinType & pinType);

        std::string toStr() const;
        void print(std::ostream & stream, int column) const;

        void writeXml(Core::XmlNode & node, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const;
        virtual void writeXmlOfChildren(Core::XmlNode & structure, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const = 0;

        bool isConnected(const StructurePinType& pinType) const;
        bool isConnected(const Core::NetId& netId) const;

        bool hasDeviceConnectedTo(const StructurePinType& pin1, const StructurePinType& pin2) const;
        const Core::Device& findDeviceConnectedTo(const StructurePinType& pin1, const StructurePinType& pin2) const;

        bool hasMaxPersistence() const;
        bool hasPersistence() const;

        bool operator==(const Structure & other) const;
        bool operator!=(const Structure & other) const;
        bool operator<(const Structure & other) const;

        void setParent(Structure & structure);
        bool hasPin(const StructurePinType & pinType) const;
        bool hasParent() const;
        bool hasDirectParent(const StructureId& parentId) const;
        bool hasCommonParent(const Structure& other) const;

        bool hasDirectParent(const std::vector<StructRec::StructureName>& structureNames) const;
        bool hasParentInSameStructureCircuits(const StructureCircuits & structureCircuits) const; //Whole parent is in the same instance and parts are not in any other circuit
        bool hasDirectParent(const StructureName& structureName) const;
        bool hasExactlyOneParent() const;
        bool hasMoreThanOneParent() const;
        bool hasCommonDevices(const Structure & other) const;
        bool isCurrentMirror() const;
        bool isPartOfCurrentMirror() const;

        const std::vector<Structure*> getParents() const;
        std::vector<const Structure*> getTopmostParents() const;
        const Structure& getParent() const;
        const Structure& getTopmostParent() const;
        const Structure& getTopmostParentWithDifferentDevices(const Structure & other) const;
        const std::vector<Structure *> getAllHigherLevelParents() const;
        void removeParent(const StructureId & parentId);

        void disConnect(StructureCircuits & circuit);

        virtual void deleteChildrenRecursivly() const = 0;
        virtual bool isPair() const = 0;
        virtual bool isArray() const = 0;
    protected:
        virtual void removeThisAsParentOfChildren() const = 0;
        virtual void removeThisAsParentOfStructuresInTheseCircuits(StructureCircuits & circuits, Core::InstanceNamePath instanceNamePath) const = 0;

        virtual void printChildren(std::ostream & stream, int column) const = 0;

        void copyBasics(Structure & other) const;
        void copyPins(Structure & newStructure, StructureCircuits & circuits, StructureCircuit & circuit) const;

    private:
        void removeThisAsParentOfInInstanceChildren(StructureCircuits & circuits, Core::InstanceNamePath instanceNamePath);
    	void findDevicePin(const StructurePin& pin, Core::DeviceId& deviceId, Core::TerminalId& terminalId) const;
    	void findDevicePin(const StructurePin& pin, const Core::Pin*& corePin) const;

        bool hasStructureId() const;
        bool hasTechType() const;
        bool hasPins() const;

        void deleteParents();

        static std::string spaces(int columns);

        static const int PERSISTENCE_NOT_INITIALIZED_;
        static const int MAX_PERSISTENCE_;

        int persistence_;
        StructureId structureId_;
        Core::TechType techType_;
        StructurePins pins_;
        std::vector<Structure*> parents_;
    };
}


#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_STRUCTURE_H_ */
