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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_ANALOGINVERTERS_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_ANALOGINVERTERS_H_

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

namespace AutomaticSizing	{
	class CircuitInformation;
}


namespace Synthesis
{
    class StructuralLevel;

	class AnalogInverters : public LibraryItem
	{
		public:
			AnalogInverters(StructuralLevel & StructuralLevel);
			~AnalogInverters();

			std::vector<const Core::Circuit*>  getAnalogInverters() const;


			std::string toStr() const;

		public:
			static const Core::TerminalName OUTPUT_TERMINAL_;
			
			static const Core::TerminalName SOURCE_CURRENTBIASNMOS_TERMINAL_;
			static const Core::TerminalName SOURCE_CURRENTBIASPMOS_TERMINAL_;
			
			static const Core::TerminalName IN_CURRENTBIASNMOS_TERMINAL_;
			static const Core::TerminalName IN_CURRENTBIASPMOS_TERMINAL_;

			static const Core::TerminalName INSOURCE_CURRENTBIASNMOS_TERMINAL_;
			static const Core::TerminalName INOUTPUT_CURRENTBIASNMOS_TERMINAL_;
			static const Core::TerminalName INOUTPUT_CURRENTBIASPMOS_TERMINAL_;
			static const Core::TerminalName INSOURCE_CURRENTBIASPMOS_TERMINAL_;

			static const Core::TerminalName INNER_CURRENTBIASNMOS_TERMINAL_;
			static const Core::TerminalName INNER_CURRENTBIASPMOS_TERMINAL_;

			static const Core::InstanceName CURRENTBIASNMOS_;
			static const Core::InstanceName CURRENTBIASPMOS_;
			

		private:
			void initializeAnalogInverters(StructuralLevel & StructuralLevel);

			const Core::Circuit & createNewAnalogInverter(const Core::Circuit & currentBiasPmos, 
									const Core::Circuit & currentBiasNmos, int & index);

			void connectInstanceTerminals(Core::Circuit & circuit, Core::Instance & currentBiasNmosInstance, 
											Core::Instance & currentBiasPmosInstance);

			void eraseAllAnalogInverters();

		private:
			static const Core::NetId OUTPUT_NET_;
			
			static const Core::NetId SOURCE_CURRENTBIASNMOS_NET_;
			static const Core::NetId SOURCE_CURRENTBIASPMOS_NET_;
			
			static const Core::NetId IN_CURRENTBIASNMOS_NET_;
			static const Core::NetId IN_CURRENTBIASPMOS_NET_;

			static const Core::NetId INSOURCE_CURRENTBIASNMOS_NET_;
			static const Core::NetId INOUTPUT_CURRENTBIASNMOS_NET_;
			static const Core::NetId INOUTPUT_CURRENTBIASPMOS_NET_;
			static const Core::NetId INSOURCE_CURRENTBIASPMOS_NET_;

			static const Core::NetId INNER_CURRENTBIASNMOS_NET_;
			static const Core::NetId INNER_CURRENTBIASPMOS_NET_;

			std::vector<const Core::Circuit *> analogInverters_;

	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL2_ANALOGINVERTERS_H_ */
