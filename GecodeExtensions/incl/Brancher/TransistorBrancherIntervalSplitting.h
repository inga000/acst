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

#ifndef GECODEEXTENSIONS_INCL_BRANCHER_TRANSISTORBRANCHERINTERVALSPLITTING_H_
#define GECODEEXTENSIONS_INCL_BRANCHER_TRANSISTORBRANCHERINTERVALSPLITTING_H_

#include "GecodeExtensions/incl/Brancher/TransistorBrancher.h"

#include <map>
#include <gecode/int.hh>

namespace AutomaticSizing {
class Transistor;
}



namespace Gecode {
	
class TransistorBrancherIntervalSplitting : public TransistorBrancher {
	
	public:
		TransistorBrancherIntervalSplitting(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result);
		TransistorBrancherIntervalSplitting(Space & home, TransistorBrancherIntervalSplitting& transistorBrancher);
		static void post(Home home, ViewArray<Int::IntView>& transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result);
	//status, choice, commit,print
	
		virtual bool status(const Space & home) const;
	
		virtual Choice* choice(Space & home);
		virtual Choice* choice(const Space & home, Archive & e);
		
		virtual ExecStatus commit(Space & home, const Choice & c, unsigned int a);
		
		virtual void print(const Space & home, const Choice & c, unsigned int a, std::ostream & o) const;
	
	
		virtual NGL* ngl(Space& home, const Choice& c, unsigned int a) const;

		virtual Brancher * copy(Space & home);
		virtual size_t dispose(Space& home);

		std::string print();

	protected:
	
	class PosVal : public Choice 
	{
		public:
			PosVal(const TransistorBrancherIntervalSplitting & brancher, int position, int value);
		
			virtual void archive(Archive & e) const;
		
		public:
			int position_;
			int value_;
	};
	

};

void branchTransistorsIntervalSplitting(Home home,
		const IntVarArgs & transistorVariables, std::vector<AutomaticSizing::Transistor*> transistors, Partitioning::Result & result);

}
#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_AUTOMATICSIZING_H_ */


