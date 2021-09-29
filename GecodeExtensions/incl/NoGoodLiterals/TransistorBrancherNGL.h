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

//What does subsumption means in Gecode, does it only takes place when the transisor variable is assigned?



#ifndef GECODEEXTENSIONS_INCL_BRANCHER_TRANSISTORBRANCHERNGL_H_
#define GECODEEXTENSIONS_INCL_BRANCHER_TRANSISTORBRANCHERNGL_H_

#include <gecode/int.hh>

namespace Gecode {
class TransistorBrancherNGL : public NGL
{
	public:
		TransistorBrancherNGL(Space & home, Int::IntView transistorVariable_, int value );
		TransistorBrancherNGL(Space & home, TransistorBrancherNGL& ngl);
		
		virtual NGL* copy(Space & home);

		virtual size_t dispose(Space & home);
	
		virtual NGL::Status status(const Space & home) const;
	
		virtual ExecStatus prune(Space & home);
	
		virtual void subscribe(Space & home, Propagator & p);
	
		virtual void cancel(Space& home, Propagator& p);
	
		virtual void reschedule(Space& home, Propagator& p);
	
	
	
	protected:
		Int::IntView transistorVariable_;
		int value_;
	
	
};
}
#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_AUTOMATICSIZING_H_ */
