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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_ARRAYCONNECTION_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_ARRAYCONNECTION_H_

#include "StructRec/incl/StructureCircuit/Structure/Array.h"
#include "StructRec/incl/StructureCircuit/Structure/Pin/PinType/StructurePinType.h"
#include "StructRec/incl/StructureCircuit/StructureCircuit.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/Device.h"
#include "StructRec/incl/Library/LibraryItemMapping/LibraryItemMapping.h"

namespace Core {
	class VisitedInstances;
}

namespace StructRec {

	class ArrayNet;
	class VisitedInstances;
	class ArrayConnection
	{
	public:
		ArrayConnection();
		ArrayConnection(const ArrayConnection& other);
		~ArrayConnection();

		void addConnection(const StructurePinType arrayPin,const Core::PinType devicePin);
		void connectArray(const Core::Circuit & circuit, StructureCircuit & arrayCircuit,
				Array & array, const Core::Device & device, Core::VisitedInstances & visitedInstances) const;
		std::vector<StructurePinType> getStructurePinTypes() const;

		bool isIdentical(const ArrayConnection& other) const;
		LibraryItemMapping createMapping(const ArrayConnection& other) const;
	private:
		StructureNet& findOrCreateArrayNet(StructureCircuit & circuit, const Core::Net& netId) const;
		const Core::Net& computeCorrespondingNet(const Core::Circuit& circuit, const Core::Device& device,
				const Core::Pin& pin, Core::VisitedInstances& visitedInstances) const;

		const std::map<StructurePinType, Core::PinType>& getArrayPinToDevicePinConnection() const;

		bool hasPin(const StructurePinType arrayPin);
		Core::PinType findPinType(const StructurePinType arrayPin);

		ArrayNet & createNewArrayNet(const Core::Net & net) const;


	private:
		std::map<StructurePinType, Core::PinType> arrayPinToDevicePinConnection_;
	};


}



#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_CONNECTION_ARRAYCONNECTION_H_ */
