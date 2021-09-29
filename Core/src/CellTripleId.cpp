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

#include "Core/incl/CellTripleId.h"
#include "Core/incl/Circuit/CircuitId/CircuitName.h"
#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Core {

    CellTripleId::CellTripleId()
    {
    }

    void CellTripleId::setLib(const std::string& library)
    {
        libraryName_ = library;
    }

    void CellTripleId::setCell(const std::string& cell)
    {
        cellName_ = cell;
    }

    void CellTripleId::setView(const std::string& view)
    {
        viewName_ = view;
    }

    std::string CellTripleId::getLib() const
    {
        assert(isInitialized());
        return libraryName_;
    }

    std::string CellTripleId::getCell() const
    {
        assert(isInitialized());
        return cellName_;
    }

    std::string CellTripleId::getView() const
    {
        assert(isInitialized());
        return viewName_;
    }

    CircuitId CellTripleId::createCircuitIdentifier() const
    {
        std::string circuitIdString = getLib() + "_" + getCell() + "_" + getView();
        CircuitId curcuitId = CircuitName(circuitIdString).createIdentifier();
        return curcuitId;
    }


    bool CellTripleId::operator==(const CellTripleId& other) const
	{
    	return getLib() == other.getLib() &&
    		   getCell() == other.getCell() &&
			   getView() == other.getView();
	}

    bool CellTripleId::operator <(const CellTripleId& other) const
    {
        if(libraryName_ == other.libraryName_) {
            if(cellName_ == other.cellName_) {
                return viewName_ < other.viewName_;
            }
            else {
                return cellName_ < other.cellName_;
            }
        }
        else {
            return libraryName_ < other.libraryName_;
        }
    }

    std::string CellTripleId::toStr() const
    {
        std::ostringstream stream;
        stream << "(";
        stream << getLib() << ",";
        stream << getCell() << ",";
        if(hasViewName()) {
            stream<< getView();
        } else {
            stream << "UNINIT";
        }
        stream << ")";
        return stream.str();

    }

    bool CellTripleId::isInitialized() const
    {
        return hasLibraryName() && hasCellName() && hasViewName();
    }

    bool CellTripleId::hasLibraryName() const
    {
        return !libraryName_.empty();
    }

    bool CellTripleId::hasCellName() const
    {
        return !cellName_.empty();
    }

    bool CellTripleId::hasViewName() const
    {
        return !viewName_.empty();
    }

}
