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

#ifndef GECODEEXTENSIONS_INCL_BRANCHER_INTEGERVARIABLEBRANCHER_H_
#define GECODEEXTENSIONS_INCL_BRANCHER_INTEGERVARIABLEBRANCHER_H_



#include <map>
#include <gecode/int.hh>


namespace AutomaticSizing {
class Transistor;
class Variables;
}


namespace Gecode {
	
class IntegerVariableBrancher : public Brancher {
	public:
		IntegerVariableBrancher(Home home, ViewArray<Int::IntView>& variables, 	AutomaticSizing::Variables & variablesMap);
		IntegerVariableBrancher(Space & home, IntegerVariableBrancher& transistorBrancher);

		IntegerVariableBrancher(Home home);

		static void post(Home home, ViewArray<Int::IntView>& transistorVariables, AutomaticSizing::Variables & variablesMap);
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
			PosVal(const IntegerVariableBrancher & brancher, int position, int value);
		
			virtual void archive(Archive & e) const;
		
		public:
			int position_;
			int value_;
	};
	
	
	protected:
		std::map<float, int> sortVariablesAccordingToSmallestDomain() const;

		float computeDomain(int index) const;
		float computeDegree(int index) const;
		float computeAFC(int index) const;

		bool isLengthPosition(int position) const;
		bool isLengthPosition(int position, AutomaticSizing::Transistor & transistor) const;

		bool isWidthPosition(int position) const;
		bool isWidthPosition(int position, AutomaticSizing::Transistor & transistor) const;

	protected:
		ViewArray<Int::IntView> variables_;
		AutomaticSizing::Variables * variablesMap_;
		mutable int start_;
};
void branchIntegerVariables(Home home, const IntVarArgs & variables, AutomaticSizing::Variables & transistors);
}
#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_AUTOMATICSIZING_H_ */



