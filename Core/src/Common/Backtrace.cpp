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

#include "Core/incl/Common/Backtrace.h"
#include <execinfo.h>
#include <signal.h> 
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace Core {
    
    Trace::Trace()
    {
        // get void*'s for all entries on the stack
        size = backtrace(trace, sizeof(trace)/sizeof(void*));

    }

    void Trace::print(std::ostream& out) const
    {
        char** syms = backtrace_symbols(trace,size);

        for(char** i = syms; i < syms+size; i++)
        {
            char* sp = strchr(*i,'(');
            if(sp == nullptr)
            {
                out << *i << std::endl;
                continue;
            }
            char* ep = strchr(sp,'+');
            if(ep == nullptr)
                ep = strchr(sp,')')+1;
            if(ep == nullptr)
            {
                out << *i << std::endl;
                continue;
            }
            char ec = *ep;
            //char* sym = strncpy(sp+1,(ep-sp-1));
            *sp = 0;
            *ep = 0;
            int status;
            char* fullname = abi::__cxa_demangle(sp+1,0,0,&status);

            if(status == 0)
                out << *i << "(" << fullname << ec << (ep+1) << std::endl;
            else
                out << *i << "(" << (sp+1) << ec << (ep+1) << std::endl;
        }
        free(syms);
    }

    BacktraceException::BacktraceException(std::string w):message(w)
    {

    }
    BacktraceException::BacktraceException(const BacktraceException& r):message(r.message),backtrace(r.backtrace)
    {
    }
        
    const char* BacktraceException::what() const throw()
    {
        return message.c_str();
    }

    void BacktraceException::print(std::ostream& out) const
    {
        out << message << std::endl << backtrace;
    }

    void crashHandler(int) {
        Trace b;
        b.print();
        exit(-1);
    }

    void installBacktraceExceptionHandler()
    {
        signal(SIGSEGV, crashHandler);
    }

    std::ostream& operator<<(std::ostream& o, const Trace& b)
    {
        b.print(o);
        return o;
    }
    
    std::ostream& operator<<(std::ostream& o, const BacktraceException& e)
    {
        e.print(o);
        return o;
    }
    
    std::ostream& operator<<(std::ostream& o, BacktraceException& e)
    {
        e.print(o);
        return o;
    }

}


