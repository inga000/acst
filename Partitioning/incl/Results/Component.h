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

#ifndef AUTOMATICSIZING_INCL_RESULTS_COMPONENT_H_
#define AUTOMATICSIZING_INCL_RESULTS_COMPONENT_H_

#include "Partitioning/incl/Parts/Part.h"
#include "Core/incl/Circuit/Device/Device.h"
#include <string>

namespace Partitioning {

	class Component : public Core::Object
	{
	public:
		Component();
		~Component();

		void setArray(const StructRec::Structure& array);
		void setPart(Part& part);

		const StructRec::Structure& getArray() const;
		Part& getPart() const;

		bool isInitialized() const;

		virtual bool isTransistor() const = 0;
		virtual bool isTwoPort() const = 0;

		virtual void print(std::ostream& stream) const = 0;

		bool operator==(const Component& other) const;
		std::string toStr() const;

	private:
		bool hasArray() const;
		bool hasPart() const;

		const StructRec::Structure* array_;
		Part* part_;
	};

}


#endif /* STRUCTUREDANALOGDESIGN_INCL_RESULTS_COMPONENT_H_ */
