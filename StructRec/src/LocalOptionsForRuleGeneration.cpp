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

#include "StructRec/incl/LocalOptionsForRuleGeneration.h"

#include "Control/incl/Control.h"
#include "Control/incl/FileSystemPath.h"

namespace StructRec {

	const std::string LocalOptionsForRuleGeneration::STRUCTURE_NAME_OPTION_ = "structure-name";
	const std::string LocalOptionsForRuleGeneration::STRUCTURE_NAME_DESCRIPTION_ = "name of structure for which rules shall be generated";

	LocalOptionsForRuleGeneration::LocalOptionsForRuleGeneration(const std::string& description) :
            LocalOptions(description)
    {
        initialize();
    }

	std::string LocalOptionsForRuleGeneration::getPathOfXmlLibraryWithoutFileName() const
	{
		std::string libraryFilePath =  findStringOptionValue(XML_STRUCTREC_LIBRARY_FILE_OPTION_);
		Control::FileSystemPath fileSystemPath;
		std::string pathWithoutFileName = fileSystemPath.stripFileFromPath(libraryFilePath);
		return pathWithoutFileName;
	}

    Control::OutputFile LocalOptionsForRuleGeneration::getLibraryOutputFile() const
    {
    	std::string libraryFilePath =  findStringOptionValue(XML_STRUCTREC_LIBRARY_FILE_OPTION_);
    	Control::OutputFile libraryOutputFile;
    	libraryOutputFile.setPath(libraryFilePath);
    	return libraryOutputFile;
    }

    std::string LocalOptionsForRuleGeneration::getStructureName() const
    {
    	std::string structureName =  findStringOptionValue(STRUCTURE_NAME_OPTION_);

    	return structureName;
    }


    void LocalOptionsForRuleGeneration::initialize()
    {
    	Control::OptionsBase::addStringOption(STRUCTURE_NAME_OPTION_, STRUCTURE_NAME_DESCRIPTION_);
    }


}


