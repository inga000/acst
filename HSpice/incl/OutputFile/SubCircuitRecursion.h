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

#ifndef HSPICE_OUTPUTFILE_SUBCIRCUITRECURSION_H_
#define HSPICE_OUTPUTFILE_SUBCIRCUITRECURSION_H_

#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include <vector>

namespace Core {

    class Circuit;
    class Instance;
}

namespace HSpice {
    namespace OutputFile {

        class SubCircuitMap;

        class SubCircuitRecursion
        {
        private:
            typedef std::vector<const Core::Instance*> InstanceVector;

        public:
            SubCircuitRecursion();
            ~SubCircuitRecursion();

            void setSubCircuitMap(SubCircuitMap& subCircuitMap);

            void fillSubCircuitMapRecursivly(const Core::Circuit& circuit);

        private:
            Core::CircuitId getCircuitId(const Core::Instance& instance) const;
            const SubCircuitMap& getSubCircuitMap() const;
            SubCircuitMap& getSubCircuitMap();

            InstanceVector instances_;
            SubCircuitMap* subCircuitMap_;
        };

    }
}


#endif
