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

#ifndef STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_CROSSINSTANCERECOGNITION_CROSSINSTANCECHILD_H_
#define STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_CROSSINSTANCERECOGNITION_CROSSINSTANCECHILD_H_

#include "Core/incl/Circuit/Instance/InstanceId/InstanceId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"

#include "Core/incl/Common/Object.h"
#include <map>


namespace StructRec {

	class StructurePin;
	class Structure;
	class StructureNet;
	class StructurePinType;
	class StructureInstanceTerminal;
	class StructureCircuits;

	class CrossInstanceStructure : public Core::Object
	{
	private:
		typedef std::map<StructurePinType, const StructureInstanceTerminal*> ConnectivityMap;
	public:
		CrossInstanceStructure();
		~CrossInstanceStructure();

		void setStructure(Structure & structure);
		void setInstanceNamePath(const Core::InstanceNamePath& instanceNamePath);
		void addConnection(const StructurePinType& pinType, const StructureInstanceTerminal& instanceTerminal);

		const StructureNet& findTopInstanceNet(const StructurePinType & pinType) const;
		StructureCircuits & findStructureInstance(StructureCircuits & topCircuits) const;
		const StructureInstanceTerminal& findConnectedInstanceTerminal(const StructurePinType & pinType) const;

		Structure & getStructure() const;
		Core::InstanceNamePath getInstanceNamePath() const;

		bool isInTopCircuit() const;
		bool hasTopCircuitConnection(const StructurePinType & pinType) const;

		bool operator==(const CrossInstanceStructure & other) const;
		bool operator!=(const CrossInstanceStructure & other) const;
		bool operator<(const CrossInstanceStructure & other) const;

		std::string toStr() const;
	private:
		bool hasCrossInstanceConnection(const StructurePinType & pinType) const;

	private:
		Structure* structure_;
		ConnectivityMap crossInstanceConnectionsMap_;

		Core::InstanceNamePath instanceNamePath_;
	};

}


#endif /* STRUCTREC_INCL_LIBRARY_LIBRARYITEM_PAIRLIBRARYITEM_CROSSINSTANCERECOGNITION_CROSSINSTANCECHILD_H_ */
