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

#ifndef CORE_STREAMOPERATORS_H_
#define CORE_STREAMOPERATORS_H_

// for copy&paste:
// #include "Core/incl/Common/StreamOperators.h"

#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <boost/smart_ptr.hpp>
#include <iostream>

namespace Core {

    template <class Container> void printContainer(std::ostream & ostr, const Container c)
    {
        typename Container::const_iterator it = c.begin();
        if ( it != c.end() ) {
            ostr << *it;
            it++;
            while ( it != c.end()){
               ostr << ", " << *it;
                it++;
            }
        }
    }

    template <class A, class B> std::ostream & operator<<(std::ostream & ostr, const std::pair<A,B> & p)
    {
        ostr << "(" << p.first;
        ostr << ", " << p.second << ")";
        return ostr;
    }

    template<class T, class C> std::ostream & operator<<(std::ostream & ostr, const std::set<T,C> & x)
    {
        ostr << "set{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }

    template<class KeyT, class DataT> std::ostream & operator<<(std::ostream & ostr, const std::map<KeyT, DataT> & x)
    {
        ostr << "map{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }

    template<class KeyT, class DataT> std::ostream & operator<<(std::ostream & ostr, const std::multimap<KeyT, DataT> & x)
    {
        ostr << "multimap{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }

    template<class T> std::ostream & operator<<(std::ostream & ostr, const std::list<T> & x)
    {
        ostr << "list{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }


    template<class T, class CharT, class Traits> std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & ostr, const std::list<T> & x)
    {
        ostr << "list{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }

    template<class T> std::ostream & operator<<(std::ostream & ostr, const std::vector<T> & x)
    {
        ostr << "vector{";
        printContainer(ostr, x);
        ostr << "}";
        return ostr;
    }

}
#endif
