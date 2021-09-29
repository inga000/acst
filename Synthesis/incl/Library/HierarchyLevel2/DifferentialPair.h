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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_DIFFERENTIALPAIR_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_DIFFERENTIALPAIR_H_

#include "Core/incl/Circuit/Net/NetId/NetId.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceName.h"
#include "Core/incl/Circuit/Terminal/TerminalId/TerminalName.h"

#include "Synthesis/incl/Library/LibraryItem.h"


namespace Core
{
	class Circuit;
	class Instance;
	class Device;
}


namespace Synthesis
{
	class DeviceLevel;
	
	class DifferentialPair : public LibraryItem
	{
		public:
			DifferentialPair(const DeviceLevel & deviceLevel);
			~DifferentialPair();

			const Core::Circuit & getDifferentialPairPmos() const;
			const Core::Circuit & getDifferentialPairNmos() const;

			std::string toStr() const;

		public:
			static const Core::TerminalName OUTPUT1_TERMINAL_;
			static const Core::TerminalName OUTPUT2_TERMINAL_;

			static const Core::TerminalName INPUT1_TERMINAL_;
			static const Core::TerminalName INPUT2_TERMINAL_;

			static const Core::TerminalName SOURCE_TERMINAL_;

		private:
			void initializeDifferentialPairPmos(const DeviceLevel & DeviceLevel);
			void initializeDifferentialPairNmos(const DeviceLevel & DeviceLevel);

			const Core::Circuit & createDifferentialPair(const Core::Circuit & normalTransistor, int & index);

			void connectInstanceTerminals(Core::Circuit & differentialPair, Core::Instance & normalTransistor1,
											Core::Instance &  normalTransistor2);

		private:
			static const Core::InstanceName NORMALTRANSISTOR1_;
			static const Core::InstanceName NORMALTRANSISTOR2_;
			
			static const Core::NetId OUTPUT1_NET_;
			static const Core::NetId OUTPUT2_NET_;

			static const Core::NetId INPUT1_NET_;
			static const Core::NetId INPUT2_NET_;

			static const Core::NetId SOURCE_NET_;
			

			const Core::Circuit * differentialPairPmos_;
			const Core::Circuit *  differentialPairNmos_;

	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_DifferentialPair_H_ */
