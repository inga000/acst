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

#include "GecodeExtensions/incl/NoGoodLiterals/TransistorBrancherIntervalSplittingNGL.h"
#include "GecodeExtensions/incl/Brancher/TransistorBrancherIntervalSplitting.h"

namespace Gecode {
	
	TransistorBrancherIntervalSplittingNGL::TransistorBrancherIntervalSplittingNGL(Space & home, Int::IntView transistorVariable, int value ) :
		NGL(home),
		transistorVariable_(transistorVariable),
		value_(value)
	{
	}
	TransistorBrancherIntervalSplittingNGL::TransistorBrancherIntervalSplittingNGL(Space & home, TransistorBrancherIntervalSplittingNGL& ngl) :
		NGL(home, ngl),
		value_(ngl.value_)
	{
		transistorVariable_.update(home, ngl.transistorVariable_);
	}
		
	NGL* TransistorBrancherIntervalSplittingNGL::copy(Space & home)
	{
		return new (home) TransistorBrancherIntervalSplittingNGL(home, *this);
	}

	size_t TransistorBrancherIntervalSplittingNGL::dispose(Space & home)
	{
		(void) NGL::dispose(home);
		return sizeof(*this);
	}
	
	NGL::Status TransistorBrancherIntervalSplittingNGL::status(const Space & home) const
	{
		if(transistorVariable_.max() <= value_)
			return  NGL::SUBSUMED;
		else if(transistorVariable_.min() > value_)
			return NGL::FAILED;
		else
			return NGL::NONE;
	}
	
	ExecStatus TransistorBrancherIntervalSplittingNGL::prune(Space & home)
	{
		return me_failed(transistorVariable_.gr(home, value_)) ? ES_FAILED : ES_OK;
	}
	
	void TransistorBrancherIntervalSplittingNGL::subscribe(Space & home, Propagator & p)
	{
		transistorVariable_.subscribe(home, p, Int::PC_INT_BND);
	}
	
	void TransistorBrancherIntervalSplittingNGL::cancel(Space& home, Propagator& p)
	{
		transistorVariable_.cancel(home, p, Int::PC_INT_BND);
	}
	
	void TransistorBrancherIntervalSplittingNGL::reschedule(Space& home, Propagator& p)
	{
		transistorVariable_.reschedule(home, p, Int::PC_INT_BND);
	}
}

