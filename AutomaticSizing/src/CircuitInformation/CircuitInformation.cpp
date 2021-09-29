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


#include "AutomaticSizing/incl/CircuitInformation/CircuitInformation.h"

#include "Partitioning/incl/Results/Component.h"
#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"


namespace AutomaticSizing {

	CircuitInformation::CircuitInformation() :
			nmosEKV_(NULL),
			pmosEKV_(NULL),
			nmosSHM_(NULL),
			pmosSHM_(NULL),
			circuitParameter_(NULL),
			circuitSpecification_(NULL),
			designAttributes_(nullptr)
	{
	}

	CircuitInformation::~CircuitInformation()
	{
		delete circuitParameter_;
		delete nmosEKV_;
		delete pmosEKV_;
		delete nmosSHM_;
		delete pmosSHM_;
		delete designAttributes_;
		delete circuitSpecification_;
	}

	CircuitInformation & CircuitInformation::clone() const
	{
		CircuitInformation * newInformation = new CircuitInformation;

		newInformation->setCircuitParameter(getCircuitParameter().clone());
		newInformation->setCircuitSpecification(getCircuitSpecification().clone());
		newInformation->setDesignAttributes(getDesignAttributes().clone());

		if(nmosEKV_ != nullptr)
			newInformation->setTechnologieSpecificationEKVNmos(getTechnologieSpecificationEKVNmos().clone());


		if(pmosEKV_ != nullptr)
			newInformation->setTechnologieSpecificationEKVPmos(getTechnologieSpecificationEKVPmos().clone());

		if(nmosSHM_ != nullptr)
			newInformation->setTechnologieSpecificationSHMNmos(getTechnologieSpecificationSHMNmos().clone());


		if(pmosSHM_ != nullptr)
			newInformation->setTechnologieSpecificationSHMPmos(getTechnologieSpecificationSHMPmos().clone());

		return * newInformation;
	}

	void CircuitInformation::setTechnologieSpecificationEKVPmos(
		TechnologieSpecificationEKV& pmosSpecification)
	{
		pmosEKV_ = & pmosSpecification;
	}

	void CircuitInformation::setTechnologieSpecificationEKVNmos(
		TechnologieSpecificationEKV& nmosSpecification)
	{
		nmosEKV_ = & nmosSpecification;
	}

	void CircuitInformation::setTechnologieSpecificationSHMPmos(
		TechnologieSpecificationSHM& pmosSpecification)
	{
		pmosSHM_ = & pmosSpecification;
	}

	void CircuitInformation::setTechnologieSpecificationSHMNmos(
		TechnologieSpecificationSHM& nmosSpecification)
	{
		nmosSHM_ = & nmosSpecification;
	}

	void CircuitInformation::setCircuitParameter(
		CircuitParameter& circuitParameter)
	{
		circuitParameter_ = & circuitParameter;
	}

	void CircuitInformation::setCircuitSpecification(
		CircuitSpecification& circuitSpecification)
	{
		circuitSpecification_ = & circuitSpecification;
	}

	void CircuitInformation::setDesignAttributes(
		DesignAttributes& designAttributes)
	{
		designAttributes_ = & designAttributes;
	}

	const TechnologieSpecificationEKV& CircuitInformation::getTechnologieSpecificationEKVPmos() const
	{
		assert(hasTechnologieSpecificationEKVPmos());
		return * pmosEKV_;
	}

	const TechnologieSpecificationEKV& CircuitInformation::getTechnologieSpecificationEKVNmos() const
	{
		assert(hasTechnologieSpecificationEKVNmos());
		return *nmosEKV_;
	}

	const TechnologieSpecificationSHM& CircuitInformation::getTechnologieSpecificationSHMPmos() const
	{
		assert(hasTechnologieSpecificationSHMPmos());
		return * pmosSHM_;
	}

	const TechnologieSpecificationSHM& CircuitInformation::getTechnologieSpecificationSHMNmos() const
	{
		assert(hasTechnologieSpecificationSHMNmos());
		return *nmosSHM_;
	}

	const CircuitParameter& CircuitInformation::getCircuitParameter() const
	{
		assert(hasCircuitParameter());
		return *circuitParameter_;
	}

	CircuitParameter& CircuitInformation::getCircuitParameter()
	{
		assert(hasCircuitParameter());
		return *circuitParameter_;
	}

	const CircuitSpecification& CircuitInformation::getCircuitSpecification() const
	{
		assert(hasCircuitSpecification());
		return * circuitSpecification_;
	}

	const DesignAttributes& CircuitInformation::getDesignAttributes() const
	{
		assert(hasDesignAttributes());
		return * designAttributes_;
	}

	bool CircuitInformation::hasTechnologieSpecificationEKVPmos() const
	{
		return pmosEKV_ != NULL;
	}

	bool CircuitInformation::hasTechnologieSpecificationEKVNmos() const
	{
		return nmosEKV_ != NULL;
	}

	bool CircuitInformation::hasTechnologieSpecificationSHMPmos() const
	{
		return pmosSHM_ != NULL;
	}

	bool CircuitInformation::hasTechnologieSpecificationSHMNmos() const
	{
		return nmosSHM_ != NULL;
	}

	bool CircuitInformation::hasCircuitParameter() const
	{
		return circuitParameter_ != NULL;
	}

	bool CircuitInformation::hasCircuitSpecification() const
	{
		return circuitSpecification_ != NULL;
	}

	bool CircuitInformation::hasDesignAttributes() const
	{
		return designAttributes_ != nullptr;
	}

	 const TechnologieSpecificationSHM & CircuitInformation::getTechnologieSpecificationSHM(Partitioning::Component & component) const
	{
		const TechnologieSpecificationSHM * techSpec;
		if(component.getArray().getTechType().isN())
		{
			techSpec = &getTechnologieSpecificationSHMNmos();
		}
		else
		{
			techSpec = &getTechnologieSpecificationSHMPmos();
		}

		return *techSpec;
	}

	const TechnologieSpecificationEKV & CircuitInformation::getTechnologieSpecificationEKV(const StructRec::Structure & structure) const
	{

		const TechnologieSpecificationEKV * techSpec;
		if(structure.getTechType().isN())
		{
		 	techSpec = &getTechnologieSpecificationEKVNmos();
		}
		else
		{
			techSpec = &getTechnologieSpecificationEKVPmos();
		}

		return *techSpec;
	}

	 const TechnologieSpecificationEKV & CircuitInformation::getTechnologieSpecificationEKV(Partitioning::Component & component) const
	{
		const TechnologieSpecificationEKV * techSpec;
		if(component.getArray().getTechType().isN())
		{
			techSpec = &getTechnologieSpecificationEKVNmos();
		}
		else
		{
			techSpec = &getTechnologieSpecificationEKVPmos();
		}

		return *techSpec;
	}

	const TechnologieSpecificationSHM & CircuitInformation::getTechnologieSpecificationSHM(const StructRec::Structure & structure) const
	{

		const TechnologieSpecificationSHM * techSpec;
		if(structure.getTechType().isN())
		{
		 	techSpec = &getTechnologieSpecificationSHMNmos();
		}
		else
		{
			techSpec = &getTechnologieSpecificationSHMPmos();
		}

		return *techSpec;
	}


}


