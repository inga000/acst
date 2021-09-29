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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_PAIR_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_PAIR_H_

#include "StructRec/incl/StructureCircuit/Structure/Structure.h"

namespace StructRec {

	class StructurePin;
	class StructureNet;

	class Pair : public Structure
	{
	public:
		Pair();
		~Pair();

		Pair& clone(StructureCircuit & circuit, StructureCircuits & circuits) const;

		void setChild1(Structure & child1);
		void setChild2(Structure & child2);
		void setSymmetric(bool symmetric);

		StructurePin & findChild1Pin(const StructurePinType & pinType) const;
		StructurePin & findChild2Pin(const StructurePinType & pinType) const;

		std::vector<const Core::Device*> getDevices() const;
		std::vector<const Structure*> getArrayChildren() const;

		Structure& getChild1() const;
		Structure& getChild2() const;
		bool isSymmetric() const;

		const Structure& getChild(int childNumber) const;

		bool isPair() const;
		bool isArray() const;
	private:
		bool hasChild1() const;
		bool hasChild2() const;

		void deleteChildrenOfCrossInstanceStructure();
		void deleteChildrenRecursivly() const;

		void removeThisAsParentOfChildren() const;
		void removeThisAsParentOfStructuresInTheseCircuits(StructureCircuits  &circuits, Core::InstanceNamePath namePath) const;

		void printChildren(std::ostream & stream, int column) const;
        void writeXmlOfChildren(Core::XmlNode & structure, Core::XmlDocument & doc, Core::InstanceNamePath instanceNamePath) const;

	private:
        static const int CHILD_ONE_;
        static const int CHILD_TWO_;

		Structure * child1_;
		Structure * child2_;
		bool symmetric_;
	};

}


#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_PAIR_H_ */
