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


#ifndef AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_PATHS_H_
#define AUTOMATICSIZING_INCL_CONSTRAINTPROGRAM_GRAPH_PATHS_H_


#include "AutomaticSizing/incl/ConstraintProgram/Graph/Path.h"
#include "Core/incl/Common/Object.h"


namespace AutomaticSizing {

	class Paths : public Core::Object
	{
	private:
		typedef std::vector<Path> PathVector;
	public:
		Paths();

		void initializeEmpty();
		void add(const Path& path);



		const PathVector & getInputToGroundPaths() const;
		const PathVector & getVddToGroundPaths() const;
		const PathVector & getOutputToGroundPaths() const;
		const PathVector & getVddToBiasInputPaths() const;


		bool isInitialized() const;
		std::string toStr() const;
	private:
		bool isInitialized_;

		PathVector inputToGroundPaths_;
		PathVector vddToGroundPaths_;
		PathVector outputToGroundPaths_;
		PathVector vddToBiasInputPaths_;
	};
}



#endif /* AUTOMATICSIZING_INCL_AUTOMATICSIZING_GRAPH_PATHS_H_ */
