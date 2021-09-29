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


#include "AutomaticSizing/incl/ConstraintProgram/EqualDimensionMap.h"
#include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

namespace AutomaticSizing {


	void EqualDimensionMap::add(
		const StructRec::StructureId main, Partitioning::Component& component)
	{
		std::vector<Partitioning::Component*> & components = map_[main];
		bool hasIt = false;
		for(std::vector<Partitioning::Component*>::iterator it = components.begin(); it != components.end(); it++)
		{
			Partitioning::Component * comp = * it;
			if(comp->getArray().getIdentifier() == component.getArray().getIdentifier())
			{
				hasIt = true;
				break;
			}
		}
		if(!hasIt)
		{
			components.push_back(&component);
		}
	}

	void EqualDimensionMap::add(std::vector<Partitioning::Component*> components, const Partitioning::Result & partitioningResult)
	{
		std::vector<Partitioning::Component*> newComps;
		for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
		{

			Partitioning::Component* comp = *it;

			if(hasComponentAsMain(*comp))
			{
				std::vector<Partitioning::Component*> comps = getToMainBelongingComponents(comp->getArray().getIdentifier());

				newComps.insert(newComps.end(), comps.begin(), comps.end());

				map_.erase(map_.find(comp->getArray().getIdentifier()));

			}
			if(hasComponentAsSecondary(*comp))
			{
				std::vector<Partitioning::Component*> comps = getToSecondaryBelongingComponents(*comp, partitioningResult);
				newComps.insert(newComps.end(), comps.begin(), comps.end());
				eraseToSecondaryBelongingMain(*comp);
			}
		}
		const StructRec::StructureId newMain = (*components.begin())->getArray().getIdentifier();

		for(std::vector<Partitioning::Component*>::const_iterator it = newComps.begin(); it != newComps.end(); it++)
		{
			Partitioning::Component * comp = *it;

			if(!hasComponentAsMain(*comp) && !hasComponentAsSecondary(*comp))
			{
				add(newMain, *comp);
			}
		}
		for(std::vector<Partitioning::Component*>::const_iterator it = std::next(components.begin()); it != components.end(); it++)
		{
			Partitioning::Component* comp = * it;
			if(!hasComponentAsMain(*comp) && !hasComponentAsSecondary(*comp))
			{
				add(newMain, *comp);
			}
		}

	}

	bool EqualDimensionMap::hasComponentAsMain(
			Partitioning::Component& component) const
	{
		return map_.find(component.getArray().getIdentifier()) != map_.end();
	}

	bool EqualDimensionMap::hasComponentAsSecondary(Partitioning::Component & component) const
	{
		bool hasIt;
		for(Map::const_iterator it_map = map_.begin(); it_map != map_.end(); it_map++)
		{
			std::vector<Partitioning::Component*> components = it_map->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = components.begin(); it_comps != components.end(); it_comps++)
			{
				Partitioning::Component * comp = * it_comps;
				if(comp->getArray().getIdentifier() == component.getArray().getIdentifier())
				{
					hasIt = true;
					break;
				}
			}
			if(hasIt)
				break;
		}
		return hasIt;
	}

	std::string EqualDimensionMap::toStr() const
	{
		std::ostringstream oss;
		for(Map::const_iterator it_map = map_.begin(); it_map != map_.end(); it_map++)
		{
			oss << "Main Component: " << it_map->first << " Components : ";
			std::vector<Partitioning::Component*> components = it_map->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = components.begin(); it_comps != components.end(); it_comps++)
			{
				Partitioning::Component * comp = * it_comps;

				oss << comp->getArray().getIdentifier() << ", ";

			}
			oss << std::endl;
		}
		return oss.str();
	}


	std::vector<Partitioning::Component*> EqualDimensionMap::getToMainBelongingComponents(const StructRec::StructureId main)
	{
		assert(map_.find(main) != map_.end());
		return map_[main];

	}

	std::vector<Partitioning::Component*> EqualDimensionMap::getToSecondaryBelongingComponents(Partitioning::Component & component, const Partitioning::Result & partitioningResult)
	{
		assert(hasComponentAsSecondary(component));
		std::vector<Partitioning::Component*> belongingComps;
		for(Map::const_iterator it_map = map_.begin(); it_map != map_.end(); it_map++)
		{
			std::vector<Partitioning::Component*> components = it_map->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = components.begin(); it_comps != components.end(); it_comps++)
			{
				Partitioning::Component * comp = * it_comps;
				if(comp->getArray().getIdentifier() == component.getArray().getIdentifier())
				{
					for(std::vector<Partitioning::Component*>::const_iterator it = components.begin(); it != components.end(); it++)
					{
						Partitioning::Component * belongingComp = *it;
						if(belongingComp->getArray().getIdentifier() != component.getArray().getIdentifier())
						{
							belongingComps.push_back(belongingComp);
						}
					}
					belongingComps.push_back(&partitioningResult.findComponent(it_map->first));
					break;
				}
				if(!belongingComps.empty())
					break;
			}

		}
		return belongingComps;
	}

	void EqualDimensionMap::eraseToSecondaryBelongingMain(Partitioning::Component & component)
	{
		bool flag = false;
		for(Map::const_iterator it_map = map_.begin(); it_map != map_.end(); it_map++)
		{
			std::vector<Partitioning::Component*> components = it_map->second;
			for(std::vector<Partitioning::Component*>::const_iterator it_comps = components.begin(); it_comps != components.end(); it_comps++)
			{
				Partitioning::Component * comp = * it_comps;
				if(comp->getArray().getIdentifier() == component.getArray().getIdentifier())
				{
					map_.erase(map_.find(it_map->first));
					it_map--;
					flag = true;
					break;
				}

			}
			if(flag)
				break;

		}

	}


	int EqualDimensionMap::computeNumberOfNeededParameter(std::vector<Partitioning::Transistor*> components)
	{
		int num = map_.size();
		for(std::vector<Partitioning::Transistor*>::const_iterator it = components.begin(); it != components.end(); it ++)
		{
			Partitioning::Component * comp = * it;
			if(!hasComponentAsMain(*comp) && !hasComponentAsSecondary(*comp))
			{
				num++;
			}
		}
		return num;
	}

	std::map<const StructRec::StructureId, std::vector<Partitioning::Component*> > & EqualDimensionMap::getMap()
	{
		assert(!map_.empty());
		return map_;
	}


}
