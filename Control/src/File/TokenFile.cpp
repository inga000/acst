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


#include "Control/incl/File/TokenFile.h"
#include <sstream>
#include "Core/incl/Common/BacktraceAssert.h"

namespace Control {

    TokenFile::TokenFile()
    {
    }

    TokenFile::~TokenFile()
    {
    }

    std::vector<std::string> TokenFile::readToken(const std::string& tokenName, int column)
    {
        assert(column >= 1);
        open();
        std::vector<std::string> parsedTokens;
        std::string line;
        while (getline(getStream(), line))
        {
            if(line.find(tokenName) == 0) {
                std::vector<std::string> columns = getColumns(line);
                std::string tokenValueWithQuotes = columns[column];
                std::string tokenValue = tokenValueWithQuotes.substr(1, tokenValueWithQuotes.length()-2);
                parsedTokens.push_back(tokenValue);
            }
        }
        close();
        return parsedTokens;
    }

    std::vector<std::string> TokenFile::getColumns(const std::string& line) const
    {
        std::stringstream stringStream(line);
        std::string bufferString;
        std::vector<std::string> elements;
        while(stringStream >> bufferString) {
            elements.push_back(bufferString);
        }
        assert(elements.size() >= 2);
        return elements;
    }

}
