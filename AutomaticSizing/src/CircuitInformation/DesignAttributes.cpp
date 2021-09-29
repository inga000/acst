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


#include "AutomaticSizing/incl/CircuitInformation/DesignAttributes.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Circuit/Device/Device.h"

namespace AutomaticSizing {

DesignAttributes::DesignAttributes() :
		transconductanceInWeakInversion_(false),
		allTransistorsInWeakInversion_(false)
{
}

DesignAttributes::~DesignAttributes()
{

}

DesignAttributes & DesignAttributes::clone() const
{
	DesignAttributes * newAttributes = new DesignAttributes;


	for(auto & device : weakInversionSet_)
	{
		newAttributes->addWeakInversionTransistor(device);
	}

	for(auto & device : moderateInversionSet_)
	{
		newAttributes->addModerateInversionTransistor(device);
	}

	newAttributes->setTransconductancesInWeakInversion(transconductanceInWeakInversion_);
	newAttributes->setAllTransistorsInWeakInversion(allTransistorsInWeakInversion_);

	return * newAttributes;
}


void DesignAttributes::addWeakInversionTransistor(
		Core::DeviceId deviceId)
{
	weakInversionSet_.insert(deviceId);
}

void DesignAttributes::addModerateInversionTransistor(
		Core::DeviceId id)
{
	moderateInversionSet_.insert(id);
}

void DesignAttributes::setTransconductancesInWeakInversion(
		bool isIt)
{
	transconductanceInWeakInversion_ = isIt;
}

void DesignAttributes::setAllTransistorsInWeakInversion(
		bool isIt)
{
	allTransistorsInWeakInversion_ = isIt;
}

bool DesignAttributes::isInWeakInversion(
		Partitioning::Component& comp) const
{
	const Core::Device & device = **comp.getArray().getDevices().begin();
	Core::DeviceId compId = device.getIdentifier();
	if(allTransistorsInWeakInversion_)
	{
		return true;
	}
	else if(weakInversionSet_.find(compId) != weakInversionSet_.end())
	{
		return true;
	}
	else if(transconductanceInWeakInversion_)
	{
		if(comp.getPart().isTransconductancePart())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool DesignAttributes::isInModerateInversion(
		Partitioning::Component& comp) const
{
	const Core::Device & device = **comp.getArray().getDevices().begin();
	Core::DeviceId compId = device.getIdentifier();
	if(moderateInversionSet_.find(compId) != moderateInversionSet_.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DesignAttributes::transconductancesInWeakInversion(const Partitioning::Result & partitioningResult) const
{
	bool transPartInWeakInversion = false;
	for(auto & transPart : partitioningResult.getAllTransconductanceParts())
	{
		for(auto & comp : partitioningResult.getBelongingComponents(*transPart))
		{
			if(isInWeakInversion(*comp))
			{
				transPartInWeakInversion = true;
				break;
			}
		}
		if(transPartInWeakInversion)
			break;
	}
	return allTransistorsInWeakInversion_ || transconductanceInWeakInversion_ || transPartInWeakInversion;
}






}

