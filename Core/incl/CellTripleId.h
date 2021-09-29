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

#ifndef CORE_LIBCELLVIEWTRIPLE_H_
#define CORE_LIBCELLVIEWTRIPLE_H_

#include "Core/incl/Circuit/CircuitId/CircuitId.h"
#include "Core/incl/Common/Object.h"
#include <string>

namespace Core {

    class CellTripleId : public Object
    {
    public:
        CellTripleId();

        void setLib(const std::string& library);
        void setCell(const std::string& cell);
        void setView(const std::string& view);

        bool isInitialized() const;

        std::string getLib() const;
        std::string getCell() const;
        std::string getView() const;

        CircuitId createCircuitIdentifier() const;

        bool operator==(const CellTripleId& other) const;
        bool operator <(const CellTripleId& other) const;

        std::string toStr() const;

    private:
        bool hasLibraryName() const;
        bool hasCellName() const;
        bool hasViewName() const;

        std::string libraryName_;
        std::string cellName_;
        std::string viewName_;
    };
}
#endif
