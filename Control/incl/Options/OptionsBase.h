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

#ifndef CONTROL_OPTIONSBASE_H_
#define CONTROL_OPTIONSBASE_H_

#include <string>
#include <boost/program_options.hpp>

namespace Control {

    class OptionsBase
    {
    protected:
        typedef boost::program_options::options_description BoostOptionsDescription;
        typedef boost::program_options::variables_map BoostVariablesMap;

        OptionsBase(const std::string& description);
        ~OptionsBase();

    public:
        std::string getStringDescription() const;
        void parse(int argc, char ** argv);

        virtual bool isCircuitReadInOption() const = 0;

    protected:
        void addBoolOption(const std::string & optionName, const std::string & optionDescription);
        void addStringOption(const std::string & optionName, const std::string & optionDescription);
        void addStringOption(const std::string & optionName, const std::string & optionDescription, const std::string & defaultValue);

        bool hasOptionValue(const std::string & optionName) const;
        std::string findStringOptionValue(const std::string & optionName) const;
        bool findBoolOptionValue(const std::string & optionName) const; // New function here


    private:
        OptionsBase(const OptionsBase & other); // don't implement
        void operator=(const OptionsBase & other); // don't implement

        bool parsedFlag_;
        BoostOptionsDescription optionsDescription_;
        BoostVariablesMap variablesMap_;
    };
}

#endif
