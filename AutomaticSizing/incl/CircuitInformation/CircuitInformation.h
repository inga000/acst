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
#ifndef AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITINFORMATION_H_
#define AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITINFORMATION_H_

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitSpecification.h"
#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"
#include "TechnologieSpecificationSHM.h"
#include "TechnologieSpecificationEKV.h"

namespace Partitioning {

	class Component;

}

namespace AutomaticSizing {

	class DesignAttributes;

	class CircuitInformation {
	public:
		CircuitInformation();
		~CircuitInformation();

		CircuitInformation & clone() const;

		void setTechnologieSpecificationEKVPmos(TechnologieSpecificationEKV & pmosSpecification);
		void setTechnologieSpecificationEKVNmos(TechnologieSpecificationEKV & nmosSpecification);
		void setTechnologieSpecificationSHMPmos(TechnologieSpecificationSHM & pmosSpecification);
		void setTechnologieSpecificationSHMNmos(TechnologieSpecificationSHM & nmosSpecification);
		void setCircuitParameter(CircuitParameter & circuitParameter);
		void setCircuitSpecification(CircuitSpecification & circuitSpecification);
		void setDesignAttributes(DesignAttributes & designAttributes);

		const TechnologieSpecificationEKV & getTechnologieSpecificationEKVPmos() const;
		const TechnologieSpecificationEKV & getTechnologieSpecificationEKVNmos() const;
		const TechnologieSpecificationSHM & getTechnologieSpecificationSHMPmos() const;
		const TechnologieSpecificationSHM & getTechnologieSpecificationSHMNmos() const;
		const CircuitParameter & getCircuitParameter() const;
		CircuitParameter & getCircuitParameter();
		const CircuitSpecification & getCircuitSpecification() const;
		const DesignAttributes & getDesignAttributes() const;

		const TechnologieSpecificationEKV & getTechnologieSpecificationEKV(Partitioning::Component & component) const;
		const TechnologieSpecificationEKV & getTechnologieSpecificationEKV(const StructRec::Structure & structure) const;
		const TechnologieSpecificationSHM & getTechnologieSpecificationSHM(Partitioning::Component & component) const;
		const TechnologieSpecificationSHM & getTechnologieSpecificationSHM(const StructRec::Structure & structure) const;

		bool hasTechnologieSpecificationEKVPmos() const;
		bool hasTechnologieSpecificationEKVNmos() const;
		bool hasTechnologieSpecificationSHMPmos() const;
		bool hasTechnologieSpecificationSHMNmos() const;

		bool hasCircuitParameter() const;
		bool hasCircuitSpecification() const;
		bool hasDesignAttributes() const;

	private:

		const TechnologieSpecificationSHM * nmosSHM_;
		const TechnologieSpecificationSHM * pmosSHM_;

		const TechnologieSpecificationEKV * nmosEKV_;
		const TechnologieSpecificationEKV * pmosEKV_;
		CircuitParameter * circuitParameter_;
		const CircuitSpecification* circuitSpecification_;
		const DesignAttributes * designAttributes_;

	};


}




#endif /* AUTOMATICSIZING_INCL_CIRCUITINFORMATION_CIRCUITINFORMATION_H_ */
