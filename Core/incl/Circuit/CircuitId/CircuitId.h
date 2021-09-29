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


#ifndef CORE_CIRCUITID_H_
#define CORE_CIRCUITID_H_

#include "Core/incl/Common/Object.h"
#include "Core/incl/Circuit/Device/TechType.h"

#include <string>

namespace Core {

    class CircuitName;

    class CircuitId : public Object
    {
    public:
        CircuitId();
        ~CircuitId();

        CircuitId(const CircuitId & other);
        void operator=(const CircuitId & other);

        void setCircuitName(const CircuitName& name);
        void setId(int id);
        void setTechType(TechType techType);

        bool isInitialized() const;


        std::string toStr() const;
        std::string getName() const;
        int getId() const;
        TechType getTechType() const;

        bool operator==(const CircuitId & other) const;
        bool operator!=(const CircuitId & other) const;
        bool operator<(const CircuitId & other) const;
        bool operator>(const CircuitId & other) const;

    private:

        const static int NOT_INITIALIZED_;

    private:
        CircuitName getCircuitName() const;

        CircuitName * circuitName_;
        int id_;
        TechType techType_; //Specifies if pmos or nmos transistors are dominating the circuit
    };
}


#endif
