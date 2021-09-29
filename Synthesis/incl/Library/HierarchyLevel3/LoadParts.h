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

#ifndef SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_LOADPARTS_H_
#define SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_LOADPARTS_H_

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
  class StructuralLevel;

	class LoadParts : public LibraryItem
	{
		public:
			LoadParts(const StructuralLevel & structuralLevel);
			~LoadParts();
			const StructuralLevel & getStructuralLevel() const;
			void eraseTemporaryFunctionalblocks() const;
			std::vector<const Core::Circuit *> getTemporaryLoadParts() const;

			std::vector<const Core::Circuit *> createTwoTransistorsLoadPartsLoadPartsPmosVoltageBiases() const;
			std::vector<const Core::Circuit *> createTwoTransistorsLoadPartsLoadPartsNmosVoltageBiases() const;
			std::vector<const Core::Circuit *> createFourTransistorsLoadPartsLoadPartsPmosVoltageBiases() const;
			std::vector<const Core::Circuit *> createFourTransistorsLoadPartsLoadPartsNmosVoltageBiases() const;

			std::vector<const Core::Circuit *> createLoadPartsPmosTwoTransistorCurrentBiasesDifferentSources() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosTwoTransistorCurrentBiasesDifferentSources() const;
			std::vector<const Core::Circuit *> createLoadPartsPmosFourTransistorCurrentBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosFourTransistorCurrentBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsPmosCurrentBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosCurrentBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsPmosVoltageBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosVoltageBiases() const;
			std::vector<const Core::Circuit *> createLoadPartsPmosMixed() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosMixed() const;
			std::vector<const Core::Circuit *> createLoadPartsPmosFourTransistorMixed() const;
			std::vector<const Core::Circuit *> createLoadPartsNmosFourTransistorMixed() const;

			std::vector<const Core::Circuit *> getLoadPartsPmosVoltageBiases() const;  // feedback
			std::vector<const Core::Circuit *> getLoadPartsNmosVoltageBiases() const; // feedback

			std::string toStr() const;

		public:
			static const Core::TerminalName OUT1_TERMINAL_;
			static const Core::TerminalName OUT2_TERMINAL_;
			static const Core::TerminalName SOURCE_TERMINAL_;

			static const Core::TerminalName SOURCE1_TERMINAL_;
			static const Core::TerminalName SOURCE2_TERMINAL_;

			static const Core::TerminalName INNER_TERMINAL_;

			static const Core::TerminalName INNEROUTPUT_TERMINAL_;
			static const Core::TerminalName INNERSOURCE_TERMINAL_;

			static const Core::TerminalName OUTOUTPUT1_TERMINAL_;
    		static const Core::TerminalName OUTOUTPUT2_TERMINAL_;
			static const Core::TerminalName OUTSOURCE1_TERMINAL_;
			static const Core::TerminalName OUTSOURCE2_TERMINAL_;

			static const Core::TerminalName INNERTRANSISTORSTACK1_TERMINAL_;
			static const Core::TerminalName INNERTRANSISTORSTACK2_TERMINAL_;

			static const Core::InstanceName TRANSISTORSTACK1_;
			static const Core::InstanceName TRANSISTORSTACK2_;

		private:
			void initializeLoadPartsPmos(const StructuralLevel & structuralLevel);
			void initializeLoadPartsNmos(const StructuralLevel & structuralLevel);

			std::vector<const Core::Circuit *> createTwoTransistorLoadPartsCurrentBiases(
									std::vector<const Core::Circuit* > oneTransistorCurrentBiases, int & index) const;

			std::vector<const Core::Circuit *> createTwoTransistorLoadPartsCurrentBiasesDifferentSources(
						std::vector<const Core::Circuit *> oneTransistorCurrentBiases, int & index) const;

			std::vector<const Core::Circuit *> createTwoTransistorLoadPartsVoltageBiases(
						std::vector<const Core::Circuit* > oneTransistorVoltageBiases,  int & index) const;

			std::vector<const Core::Circuit *> createTwoTransistorLoadPartsMixed(
									std::vector<const Core::Circuit* > oneTransistorVoltageBiases,
									std::vector<const Core::Circuit* > oneTransistorCurrentBiases, int & index) const;

			std::vector<const Core::Circuit *> createThreeTransistorLoadPartsMixed(
									std::vector<const Core::Circuit* > oneTransistorVoltageBiases,
									std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const;

			std::vector<const Core::Circuit *> createFourTransistorLoadPartsCurrentBiases(
									std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const;

			std::vector<const Core::Circuit *> createFourTransistorLoadPartsVoltageBiases(
						std::vector<const Core::Circuit* > twoTransistorVoltageBiases, int & index) const;

			std::vector<const Core::Circuit *> createFourTransistorLoadPartsMixed(
									std::vector<const Core::Circuit* > twoTransistorVoltageBiases,
									std::vector<const Core::Circuit* > twoTransistorCurrentBiases, int & index) const;

			const Core::Circuit & createTwoTransistorLoadPart(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const;

			const Core::Circuit & createTwoTransistorLoadPartDifferentSources(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const;

			const Core::Circuit & createThreeTransistorLoadPart(Core::Instance & transistorStack1,
									Core::Instance & transistorStack2, int & index) const;

			const Core::Circuit & createFourTransistorLoadPart(Core::Instance & transistorStack1,
						Core::Instance & transistorStack2, int & index) const;

			void connectInstanceTerminalsOfTwoTransistorLoadPart(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const;

			void connectInstanceTerminalsOfTwoTransistorLoadPartDifferentSources(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const;

			void connectInstanceTerminalsOfThreeTransistorLoadPart(Core::Circuit & loadPart,
									Core::Instance & transistorStack1, Core::Instance & transistorStack2) const;

			void connectInstanceTerminalsOfFourTransistorLoadPart(Core::Circuit & loadPart,
						Core::Instance & transistorStack1, Core::Instance & transistorStack2) const;

			void eraseTwoTransistorsLoadPartsPmosVoltageBiases();
            void eraseFourTransistorsLoadPartsPmosVoltageBiases();
            void eraseTwoTransistorsLoadPartsNmosVoltageBiases();
            void eraseFourTransistorsLoadPartsNmosVoltageBiases();

		
		private:
			static const Core::NetId OUT1_NET_;
			static const Core::NetId OUT2_NET_;
			static const Core::NetId SOURCE_NET_;
			static const Core::NetId SOURCE1_NET_;
			static const Core::NetId SOURCE2_NET_;

			static const Core::NetId INNER_NET_;

			static const Core::NetId INNEROUTPUT_NET_;
			static const Core::NetId INNERSOURCE_NET_;

			static const Core::NetId OUTOUTPUT1_NET_;
			static const Core::NetId OUTOUTPUT2_NET_;
			static const Core::NetId OUTSOURCE1_NET_;
    		static const Core::NetId OUTSOURCE2_NET_;

			static const Core::NetId INNERTRANSISTORSTACK1_NET_;
			static const Core::NetId INNERTRANSISTORSTACK2_NET_;


			std::vector<const Core::Circuit *> twoTransistorsLoadPartsPmosVoltageBiases_;
            std::vector<const Core::Circuit *> fourTransistorsLoadPartsPmosVoltageBiases_;
            std::vector<const Core::Circuit *> twoTransistorsLoadPartsNmosVoltageBiases_;
            std::vector<const Core::Circuit *> fourTransistorsLoadPartsNmosVoltageBiases_;

            const StructuralLevel * structuralLevel_;
            std::vector<const Core::Circuit*> temporaryLoadParts_;
	};



}
#endif /* SYNTHESIS_INCL_LIBRARY_HIERARCHYLEVEL3_LoadParts_H_ */
