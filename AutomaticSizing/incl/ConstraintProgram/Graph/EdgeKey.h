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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_EDGEKEY_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_EDGEKEY_H_

#include "Partitioning/incl/Results/Component.h"
#include "Core/incl/Circuit/Device/Pin/PinType/PinType.h"
#include "Core/incl/Circuit/Device/DeviceId/DeviceId.h"
#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Common/Object.h"

namespace AutomaticSizing {

	class EdgeKey
	{
	public:
		EdgeKey();
		~EdgeKey();

		void setStartPinType(const StructRec::StructurePinType& startPinType);
		void setEndPinType(const StructRec::StructurePinType& endPinType);
		void setComponent(Partitioning::Component& structure);

		Core::NetId getStartNodeId() const;
		Core::NetId getEndNodeId() const;
		StructRec::StructureId getStructureId() const;
		const Partitioning::Component& getComponent() const;

		bool isDrainSourceEdge() const;
		bool isCapacitanceEdge() const;
		bool isGateSourceEdge() const;
		bool isGateDrainEdge() const;

		bool operator<(const EdgeKey& other) const;
		bool operator==(const EdgeKey & other) const;

		bool isInitialized() const;
	protected:
		const StructRec::StructurePinType& getStartPinType() const;
		const StructRec::StructurePinType& getEndPinType() const;


	private:
		StructRec::StructurePinType startPinType_;
		StructRec::StructurePinType endPinType_;
		Partitioning::Component* component_;
	};



}



#endif /* AUTOMATICSIZING_INCL_ANALYSIS_GRAPH_EDGEKEY_H_ */
