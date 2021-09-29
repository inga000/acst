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

#ifndef STRUCTREC_INCL_STRUCTURECIRCUIT_NET_ARRAYNET_H_
#define STRUCTREC_INCL_STRUCTURECIRCUIT_NET_ARRAYNET_H_

#include "StructRec/incl/StructureCircuit/Net/StructureNet.h"

namespace Core {
	class Net;
}

namespace StructRec {

	class ArrayNet : public StructureNet
	{
	public:
		ArrayNet();

		virtual ArrayNet & cloneWithoutStructureConnection(StructureCircuit & circuit, StructureCircuits & circuits) const;

		void setArrayNet(const Core::Net & arrayNet);
		const Core::Net & getArrayNet() const;

		 const StructureNet &findArrayNet() const;

	private:
		bool hasArrayNet() const;

		const Core::Net * arrayNet_;
	};



}



#endif /* STRUCTREC_INCL_STRUCTURECIRCUIT_NET_ARRAYNET_H_ */
