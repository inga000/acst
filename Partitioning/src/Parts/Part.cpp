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

#include "Partitioning/incl/Parts/Part.h"
#include "Partitioning/incl/Results/Result.h"
#include "Partitioning/incl/Results/Transistor.h"
#include "Partitioning/incl/Results/TwoPort.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceTypeName.h"
#include "Core/incl/Circuit/Device/DeviceType/DeviceType.h"
#include "StructRec/incl/StructureCircuit/Structure/Array.h"

namespace Partitioning {

	Part::Part()
	{
	}

	Part::~Part()
	{
	}

	void Part::addMainStructure(
			const StructRec::Structure & mainStructure, Result& result)
	{
		mainStructures_.push_back(&mainStructure);
		initializeComponents(mainStructure, result);
	}

	const std::vector<const StructRec::Structure*>& Part::getMainStructures() const
	{
		assert(hasMainStructures());
		return mainStructures_;
	}

	std::vector<const StructRec::Structure*>& Part::getMainStructures()
	{
		assert(hasMainStructures());
		return mainStructures_;
	}

	bool Part::hasMainStructures() const
	{
		return !mainStructures_.empty();
	}

	void Part::setPartId(const PartId& partId)
	{
		partId_ = partId;
	}

	PartId Part::getPartId() const
	{
		assert(hasPartId());
		return partId_;
	}

	bool Part::hasPartId() const
	{
		return partId_.isInitialized();
	}

	std::string Part::toStr() const
	{
		std::ostringstream stream;
		if(hasPartId())
		{
			stream << getPartId().toStr() <<  std::endl;
		}
		stream << " Main structures: ";
		for(std::vector<const StructRec::Structure*>::const_iterator it = mainStructures_.begin(); it != mainStructures_.end(); it++)
		{
			const StructRec::Structure * structure = * it;
			stream << structure->getIdentifier() << ", " ;
		}
		stream << std::endl;
		print(stream);
		return stream.str();
	}

	void Part::printMainStructures(std::ostream & stream) const
	{
		stream << " Main structures: ";
		for(std::vector<const StructRec::Structure*>::const_iterator it = mainStructures_.begin(); it != mainStructures_.end(); it++)
		{
			const StructRec::Structure * structure = * it;
			stream << structure->getIdentifier() << ", " ;
		}
	}

	void Part::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
	{
		Core::InstanceNamePath instanceNamePath;
		instanceNamePath.setIsRoot();
		for(auto& it : mainStructures_)
		{
			const StructRec::Structure* structure = it;
			structure->writeXml(xmlNode, doc, instanceNamePath);
		}
	}

	void Part::addBelongingMainStructure(
				std::vector< const StructRec::Structure*>  structures, Result & result)
	{
		for(auto & struc : structures)
		{
			bool isEqual = false;
			for(std::vector<const StructRec::Structure*>::const_iterator it_mainStruc = mainStructures_.begin();
					it_mainStruc != mainStructures_.end(); it_mainStruc++ )
			{
				const StructRec::Structure * mainStruc = * it_mainStruc;
				if(mainStruc == struc)
				{
					isEqual=true;
				}
			}
			if(!isEqual)
			{
				addMainStructure(*struc, result);
			}
		}
	}

	void Part::initializeComponents(const StructRec::Structure & structure, Result & result)
	{
		std::vector<const StructRec::Structure*> arrays = structure.getArrayChildren();
		for(auto& it : arrays)
		{
			const StructRec::Structure& structure = * it;
			const StructRec::Array& array = static_cast<const StructRec::Array&>(structure);
			const Core::Device* testDevice = array.getFirstDevice();
			if(testDevice->getDeviceType().getName() == Core::DeviceTypeName("Mosfet") )
			{
				Transistor * newComp = new Transistor;
				newComp->setArray(array);
				newComp->setPart(*this);
				result.addTransistor(*newComp);
			}
			else
			{
				TwoPort * newComp = new TwoPort;
				newComp->setArray(array);
				newComp->setPart(*this);
				result.addTwoPort(*newComp);
			}
		}
	}

	bool Part::hasAsMainStructure(const StructRec::Structure & structure) const
	{
		bool hasIt = true;
		std::vector<const StructRec::Structure *> arrayChildren = structure.getArrayChildren();
		bool hasThem[arrayChildren.size()] = {false};


		int iterator = 0;
		for(auto & array : arrayChildren)
		{
			if(hasMainStructures())
			{
				for(auto & mainStruct : getMainStructures() )
				{
					std::vector<const StructRec::Structure *>arrayChildrenMainStruct = mainStruct->getArrayChildren();
					for(auto & arrayMainStruct : arrayChildrenMainStruct)
					{
						if(array == arrayMainStruct)
						{
							hasThem[iterator] = true;
							break;
						}
					}
					if(hasThem[iterator])
					{
						break;
					}
				}
			}
		}

		for(auto & it : hasThem)
		{
			if(!it)
			{
				hasIt = false;
			}
		}

		return hasIt;

	}

}

