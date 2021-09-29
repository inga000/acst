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

#ifndef SYNTHESIS_LOCALOPTIONS_H_
#define SYNTHESIS_LOCALOPTIONS_H_

#include "Control/incl/Options/HspiceOptions.h"
#include "Synthesis/incl/CircuitSpecificationFile/CircuitSpecificationFile.h"
#include <string>

namespace AutomaticSizing {
	class TechnologieFileEKV;
	class TechnologieFileSHM;
}

namespace StructRec {
	class LibraryFile;
}

namespace Synthesis {

	class LibraryFile;

    class LocalOptions : public Control::HspiceOptions
    {
    public:
        LocalOptions(const std::string& description);

        CircuitSpecificationFile getCircuitSpecificationsFile() const;
        StructRec::LibraryFile getStructRecLibraryFile() const;
        LibraryFile getCircuitLibraryFile() const;
        AutomaticSizing::TechnologieFileEKV getTechnologieFileEKV() const;
        AutomaticSizing::TechnologieFileSHM getTechnologieFileSHM() const;
        std::string getHSPICENetlistDirectoryPath() const;
        bool useHSpiceLibrary() const;

        std::string getTransistorModel() const;
        std::string getScaling() const;

        bool isCircuitReadInOption() const;

    private:
        static const std::string XML_CIRCUITSPECIFICATIONS_FILE_OPTION_;
        static const std::string XML_CIRCUITSPECIFICATIONS_FILE_DESCRIPTION_;
        static const std::string XML_STRUCTREC_LIBRARY_FILE_OPTION_;
        static const std::string XML_STRUCTREC_LIBRARY_FILE_DESCRIPTION_;
        static const std::string XML_CIRCUIT_LIBRARY_FILE_OPTION_;
        static const std::string XML_CIRCUIT_LIBRARY_FILE_DESCRIPTION_;
        static const std::string XML_TECHNOLOGIE_FILE_OPTION_;
        static const std::string XML_TECHNOLOGIE_FILE_DESCRIPTION_;
        static const std::string TRANSISTOR_MODEL_DESCRIPTION_;
        static const std::string TRANSISTOR_MODEL_OPTION_;
        static const std::string SCALING_OPTION_;
        static const std::string SCALING_DESCRIPTION_;
        static const std::string HSPICE_NETLIST_DIRECTORY_OPTION_;
        static const std::string HSPICE_NETLIST_DIRECTORY_DESCRIPTION_;
        static const std::string USE_HSPICE_LIBRARY_OPTION_;
        static const std::string USE_HSPICE_LIBRARY_DESCRIPTION_;

        void initialize();
    };

}

#endif
