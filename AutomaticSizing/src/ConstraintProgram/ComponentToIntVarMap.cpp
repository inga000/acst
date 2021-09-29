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


#include "AutomaticSizing/incl/ConstraintProgram/ComponentToIntVarMap.h"
#include "Partitioning/incl/Results/Component.h"

#include "AutomaticSizing/incl/Results/Result.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include <gecode/minimodel.hh>


namespace AutomaticSizing {


	ComponentToIntVarMap::ComponentToIntVarMap()
	{
	}

	void ComponentToIntVarMap::setDefinition(std::string definition)
	{
		definition_ = definition;
	}

	void ComponentToIntVarMap::add(
			const StructRec::StructureId structureId,
			const Gecode::IntVar& intVar)
	{
		assert(!has(structureId));
		intVarMap_[structureId] = intVar;
	}

	void ComponentToIntVarMap::add(
			Partitioning::Component & component,
		const Gecode::IntVar& intVar)
	{

		assert(!has(component));
		intVarMap_[component.getArray().getIdentifier()] = intVar;
	}

	Gecode::IntVar ComponentToIntVarMap::find(
			const Partitioning::Component& component) const
	{
		assert(has(component));
		Gecode::IntVar intVar = intVarMap_.at(component.getArray().getIdentifier());
		return intVar;
	}

	Gecode::IntVar ComponentToIntVarMap::find(
		const StructRec::StructureId& structureId) const
	{
		assert(intVarMap_.find(structureId) != intVarMap_.end(), "Component to intVar Map does not have this component.");
		Gecode::IntVar intVar = intVarMap_.at(structureId);
		return intVar;
	}

	bool ComponentToIntVarMap::has(
		const Partitioning::Component& component) const
	{
		return intVarMap_.find(component.getArray().getIdentifier()) != intVarMap_.end();
	}

	bool ComponentToIntVarMap::has(
		const StructRec::StructureId structureId) const
	{
			return intVarMap_.find(structureId) != intVarMap_.end();
	}

	 void ComponentToIntVarMap::writeWidthToTransistor(Result & result)
	 {
		 for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		 {
			 Partitioning::Component & comp = result.findComponent(it->first);
			 if(comp.isTransistor())
			 {
				 Partitioning::Transistor & tran = static_cast<Partitioning::Transistor & >(comp);
			 		int value = it->second.med();
					float floatValue = value;
					 if(definition_ == "1mum")
					 {
						 tran.setWidth(floatValue);
					 }
					 else if(definition_ == "0.1mum")
					 {
						 tran.setWidth(floatValue/10);
					 }
			 }
		 }
	 }

	 void ComponentToIntVarMap::writeCurrent(Result & result)
	 {
		 for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		 {
	    	StructRec::StructureId structureId = it->first;
	    	Gecode::IntVar current = it->second;
			 result.addCurrent(result.findComponent(structureId), current.med()*pow(10,-3));
		 }
	 }

	 void ComponentToIntVarMap::setWidth(Gecode::Space & space)
	 {
	 		IntVarMap::const_iterator it = intVarMap_.begin();
//	 		MillerPMOS
//	 		Gecode::rel(space, it->second == 11);
//	 		it++;
//	 		Gecode::rel(space, it->second == 11);
//	 		it++;
//	 		Gecode::rel(space, it->second == 11);
//	 		it++;
//	 		Gecode::rel(space, it->second == 121);
//	 		it++;
//	 		Gecode::rel(space, it->second == 6);
//	 		it++;
//	 		Gecode::rel(space, it->second == 18);
//	 		it++;
//	 		Gecode::rel(space, it->second == 6);
//	 		it++;
//	 		Gecode::rel(space, it->second == 119);
//	 		it++;

//	 		MillerNMOS
//	 		Gecode::rel(space, it->second == 57);
//	 		it++;
//	 		Gecode::rel(space, it->second == 301);
//	 		it++;
//	 		Gecode::rel(space, it->second == 9);
//	 		it++;
//	 		Gecode::rel(space, it->second == 344);
//	 		it++;
//	 		Gecode::rel(space, it->second == 344);
//	 		it++;
//	 		Gecode::rel(space, it->second == 21);
//	 		it++;
//	 		Gecode::rel(space, it->second == 126);
//	 		it++;
//	 		Gecode::rel(space, it->second == 584);
//	 		it++;

	 		//Folded Cascode
//	 	 	 Gecode::rel(space, it->second == 10);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 60);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 60);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 360);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 306);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 18);
//	 		 it++;
//	 		 Gecode::rel(space, it->second ==45);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 600);
//	 		 it++;
//	 	 	 Gecode::rel(space, it->second == 600);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 60);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 60);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 239);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 410);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 410);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 239);

	 		//3 Stage symmetrical OTA (Laker/Sansen)
//	 	 	 Gecode::rel(space, it->second == 39);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 11);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 35);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 35);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 15);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 198);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 15);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 11);
//	 		 it++;
//	 	 	 Gecode::rel(space, it->second == 599);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 35);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 35);
//	 		 it++;
//	 		 Gecode::rel(space, it->second == 595);

		 	 //TwoStageFoldedCascodeOpAmp
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 55);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 74);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 74);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 14);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 14);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 26);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 11);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 567);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 33);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 72);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 567);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 33);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 424);
//		 	 it++;

		 	 //Rail-to-Rail
//		 	 Gecode::rel(space, it->second == 5);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 80);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 160);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 5);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 5);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 160);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 320);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 80);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 320);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;

		 	 //Three-Stage Op-Amp
//		 	 Gecode::rel(space, it->second == 25);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 50);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 15);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 15);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 15);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 40);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 50);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 400);
//		 	 it++;


//		 	 //CMFBRC
//		 	 Gecode::rel(space, it->second == 8);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 10);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 12);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 12);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 8);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 8);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 500);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 500);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 100);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 20);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 30);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 100);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 100);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 100);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 100);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 250);
//		 	 it++;
//		 	 Gecode::rel(space, it->second == 250);
//		 	 it++;

		 	 //SymmetricalOpAmpWithCrossCouplePair
//		 	Gecode::rel(space, it->second == 96);
//		 	it++;
//		 	Gecode::rel(space, it->second == 192);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 40);
//		 	it++;
//		 	Gecode::rel(space, it->second == 40);
//		 	it++;
//		 	Gecode::rel(space, it->second == 600);
//		 	it++;
//		 	Gecode::rel(space, it->second == 192);
//		 	it++;
//		 	Gecode::rel(space, it->second == 48);
//		 	it++;
//		 	Gecode::rel(space, it->second == 120);
//		 	it++;
//		 	Gecode::rel(space, it->second == 48);
//		 	it++;
//		 	Gecode::rel(space, it->second == 120);

//		 	 //FoldedCascodeOpAmpWithSlewRateEnhancement
//		 	Gecode::rel(space, it->second == 10);
//		 	it++;
//		 	Gecode::rel(space, it->second == 10);
//		 	it++;
//		 	Gecode::rel(space, it->second == 10);
//		 	it++;
//		 	Gecode::rel(space, it->second == 10);
//		 	it++;
//		 	Gecode::rel(space, it->second == 300);
//		 	it++;
//		 	Gecode::rel(space, it->second == 300);
//		 	it++;
//		 	Gecode::rel(space, it->second == 320);
//		 	it++;
//		 	Gecode::rel(space, it->second == 20);
//		 	it++;
//		 	Gecode::rel(space, it->second == 20);
//		 	it++;
//		 	Gecode::rel(space, it->second == 20);
//		 	it++;
//		 	Gecode::rel(space, it->second == 20);
//		 	it++;
//		 	Gecode::rel(space, it->second == 20);
//		 	it++;
//		 	Gecode::rel(space, it->second == 300);
//		 	it++;
//		 	Gecode::rel(space, it->second == 300);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;

//		 	 FoldedCascodeOpAmpWithGainEnhancement
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 3);
//		 	it++;
//		 	Gecode::rel(space, it->second == 1);
//		 	it++;
//		 	Gecode::rel(space, it->second == 1);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 90);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 90);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 60);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 210);
//		 	it++;
//		 	Gecode::rel(space, it->second == 225);
//		 	it++;
//		 	Gecode::rel(space, it->second == 225);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 210);
//		 	it++;
//		 	Gecode::rel(space, it->second == 450);
//		 	it++;
//		 	Gecode::rel(space, it->second == 210);
//		 	it++;
//		 	Gecode::rel(space, it->second == 210);

	 }

	 void ComponentToIntVarMap::setLength(Gecode::Space & space)
	 {
	 	 IntVarMap::const_iterator it = intVarMap_.begin();
	 	 //MillerPMOS
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;


	 	 //MillerNMOS
//	 	 Gecode::rel(space, it->second == 9);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 9);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 9);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;

	 	 //FoldedCascode
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);

//	 	 3 Stage Symmetrical OTA (Laker/Sansen)
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);


	 	 //SymmetricalCMOSOTA
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;

	 	 //TwoStageFoldedCascodeOpAmp
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;


	 	 //Rail-to-Rail
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 2);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 1);
//	 	 it++;

	 	 //Three-Stage Op-Amp
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 5);
//	 	 it++;



	 	 //CMFBRC
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 3);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 6);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 4);
//	 	 it++;


	 	 //SymmewtricalOTAWithCrossCouplePair
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);


//	 	 //FoldedCascodeOpAmpWithSlewRateEnhancement
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);
//	 	it++;
//	 	Gecode::rel(space, it->second == 2);

	 	 //FoldedCascodeOpAmpWithGainEnhancement
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 10);
//	 	it++;
//	 	Gecode::rel(space, it->second == 10);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);
//	 	it++;
//	 	Gecode::rel(space, it->second == 1);

	 }

	 void ComponentToIntVarMap::setCapacitance(Gecode::Space & space)
	 {
	 	 IntVarMap::const_iterator it = intVarMap_.begin();
	 	 Gecode::rel(space, it->second == 200);
	 	it++;
//	 	 Gecode::rel(space, it->second == 15);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 15);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 40);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 40);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 10);
//	 	 it++;
//	 	 Gecode::rel(space, it->second == 40);
//	 	 it++;
//	  	Gecode::rel(space, it->second == 40);
//	 	it++;

	 }

	 void ComponentToIntVarMap::writeLengthToTransistor(Result & result)
	 {
		 for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		 {
			 Partitioning::Component & comp = result.findComponent(it->first);
		 	if(comp.isTransistor())
		 	{
		 		Partitioning::Transistor & tran = static_cast<Partitioning::Transistor & >(comp);
		 		int value = it->second.med();
				float floatValue = value;
				 if(definition_ == "1mum")
				 {
					 tran.setLength(floatValue);
				 }
				 else if(definition_ == "0.1mum")
				 {
					 tran.setLength(floatValue/10);
				 }
		 	}
		 }
	 }

	 void ComponentToIntVarMap::writeValueToCapacitance(Result & result)
	 {
		 for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		 {
			 Partitioning::Component & comp = result.findComponent(it->first);
		 	 if(comp.isTwoPort())
		 	{
		 		Partitioning::TwoPort & twoPort = static_cast<Partitioning::TwoPort & >(comp);
		 		 float value = it->second.med();
		 		 if(twoPort.getArray().getStructureName().toStr() == "CapacitorArray")
		 		 {
		 			 twoPort.setValue(value/10);
		 		 }
		 		 else
		 		 {
		 			twoPort.setValue(value);
		 		 }
		 	}
		 }
	 }

	 std::string ComponentToIntVarMap::toStr() const
	 {
		 std::ostringstream oss;
		 oss << std::endl;
		 for(IntVarMap::const_iterator it = intVarMap_.begin(); it != intVarMap_.end(); it++)
		 {
			 if(definition_ == "1mum")
			 {
				 oss <<  it->first << " " << it->second << std::endl;
			 }
			 else if(definition_ == "0.1mum")
			 {
				 int value = it->second.med();
				 float floatValue = value;
				 oss <<  it->first << " " << floatValue/10<< std::endl;
			 }
			 else
			 {
				 int value = it->second.med();
				 float floatValue = value;

				 StructRec::StructureId id = it->first;
				 if(id.getName().toStr() =="CapacitorArray")
				 {
					 oss <<  it->first << " " << floatValue/10<< std::endl;
				 }
				 else
				 {
					 oss <<  it->first << " " << it->second << std::endl;
				 }
			 }
		 }
		 return oss.str();
	 }



}


