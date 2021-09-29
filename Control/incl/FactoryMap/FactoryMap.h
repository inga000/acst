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

 
#ifndef CONTROL_FACTORYMAP_H_
#define CONTROL_FACTORYMAP_H_

#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include "Core/incl/Common/Backtrace.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Control/incl/FactoryMap/FactoryMapKey.h"

namespace Control {

    template <class Gc>
    class FactoryMap
    {
    protected:
        typedef std::map<FactoryMapKey, Gc* > GeneratorMap;
        GeneratorMap members;
    
        FactoryMap()
        {
        };

        virtual ~FactoryMap()
        {
        };

    public:

        Gc* find(const FactoryMapKey& name)
        {
            typename GeneratorMap::const_iterator res = members.find(name);
            if(res != members.end())
            {
                assert(res->second);
                return dynamic_cast<Gc*>(res->second);
            }
            else {
                throw new Core::BacktraceException("Name not found!");
            }
        };

        template <class afc> void  reg()
        {
            Gc* af = afc::getInstance();
            assert(af);
            regInst(af);
        };

        virtual void regInst(Gc* gen)
        {
            if(members.find(gen->getKey()) != members.end())
                throw new Core::BacktraceException("Duplicate registration!");
            members.insert(std::make_pair(gen->getKey(),gen));
        };

        std::string getDescription() const
        {
            std::ostringstream oss;
            for(typename FactoryMap::GeneratorMap::const_iterator it = members.begin(); it != members.end(); it++)
            {
                oss << std::endl <<  "  " << std::setw(10) << std::left << it->first.toStr() << " : \t" << it->second->getDescription();
            }
            return oss.str();
        }
    };

}
#endif
