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

#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_COMPONENTTOINTVARMAP_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_COMPONENTTOINTVARMAP_H_

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureId.h"

#include <gecode/float.hh>
#include <map>




namespace Partitioning{

	class Component;


}

namespace AutomaticSizing {


	class Result;

    class ComponentToIntVarMap
    {

        typedef std::map<const StructRec::StructureId, Gecode::IntVar> IntVarMap;
    public:
        ComponentToIntVarMap();

        void setDefinition(std::string definition);

        void add(const StructRec::StructureId, const Gecode::IntVar & intVar);
        void add(Partitioning::Component & component, const Gecode::IntVar& intVar);
        Gecode::IntVar find(const Partitioning::Component & component) const;
        Gecode::IntVar find(const StructRec::StructureId& structureId) const;

        void writeWidthToTransistor(Result & result);
        void writeLengthToTransistor(Result & result);
        void writeValueToCapacitance(Result & result);
        void writeCurrent(Result & result);


        //Debug function for Gecode
        void setWidth(Gecode::Space & space);
        void setLength(Gecode::Space & space);
        void setCapacitance(Gecode::Space & space);

        std::string toStr() const;
    private:
        bool has(const Partitioning::Component& component) const;
        bool has(const StructRec::StructureId structRecId) const;

        IntVarMap intVarMap_;
        std::string definition_;
    };
}



#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_COMPONENTTOINTVARMAP_H_ */
