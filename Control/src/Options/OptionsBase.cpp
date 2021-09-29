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

#include "Control/incl/Options/OptionsBase.h"

#include "Log/incl/LogMacros.h"

#include "Core/incl/Common/BacktraceAssert.h"

#include <sstream>

namespace Control {

    OptionsBase::OptionsBase(const std::string& description):
        parsedFlag_(false),
        optionsDescription_(description)
    {
    }

    OptionsBase::~OptionsBase()
    {
    }

    std::string OptionsBase::getStringDescription() const
    {
        std::ostringstream oss;
        oss << optionsDescription_ ;
        return oss.str();
    }

    void OptionsBase::parse(int argc, char ** argv)
    {
        boost::program_options::parsed_options parsedOptions = boost::program_options::command_line_parser(argc,argv)
                .options(optionsDescription_)
                .allow_unregistered()
                .run();
        boost::program_options::store(parsedOptions, variablesMap_);
        parsedFlag_ = true;
    }

    void OptionsBase::addBoolOption(const std::string& optionName, const std::string& optionDescription)
    {
        optionsDescription_.add_options()
                       (optionName.c_str(), optionDescription.c_str() );
    }

    void OptionsBase::addStringOption(const std::string& optionName, const std::string& optionDescription)
    {
        optionsDescription_.add_options()
                       (optionName.c_str(),
                        boost::program_options::value<std::string>(),
                        optionDescription.c_str());
    }

    void OptionsBase::addStringOption(const std::string& optionName, const std::string& optionDescription,
            const std::string& defaultValue)
    {
        optionsDescription_.add_options()
                       (optionName.c_str(),
                        boost::program_options::value<std::string>()->default_value(defaultValue),
                        optionDescription.c_str());
    }

    bool OptionsBase::hasOptionValue(const std::string & optionName) const
    {
        return variablesMap_.count(optionName) > 0;
    }

    std::string OptionsBase::findStringOptionValue(const std::string& optionName) const
    {
        assert(hasOptionValue(optionName), optionName);
        std::string optionValue = variablesMap_[optionName].as<std::string>();
        return optionValue;
    }


}

