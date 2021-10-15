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

#ifndef SYNTHESIS_INCL_RESULTS_HSPICEOUTPUTFILE_H_
#define SYNTHESIS_INCL_RESULTS_HSPICEOUTPUTFILE_H_


#include "Core/incl/Circuit/Net/NetId/NetId.h"

#include <string>

namespace Core {

	class Device;

}

namespace Partitioning {

	class Transistor;
	class TwoPort;
	class TransconductancePart;
	class BiasPart;
	class LoadPart;
	class ResistorPart;
	class CapacitancePart;
	class Part;
}

namespace AutomaticSizing {

	class Result;

}


namespace Control {

	class OutputFile;
}

namespace Synthesis
{
	class Circuit;
	class HSPICEOutputFile
	{
	public:
		HSPICEOutputFile();
		~HSPICEOutputFile();

		void setId(std::string id);
		void setPath(std::string filepath);
		void setUsesHspiceLibrary(bool usesHSPICELibrary);

		void write(const Circuit & circuit);
		void write(const Core::Circuit & circuit, const Partitioning::Result *partitioningResult = nullptr);
	private:
		void writeName();
		void writeCircuit(const Circuit & circuit);
		void writeExpectedPerformance(const AutomaticSizing::Result & sizingResult);
		void writeCurrents(const AutomaticSizing::Result & sizingResult);
		void writeVoltages(const AutomaticSizing::Result & sizingResult);
		void writeTransistor(const Partitioning::Result & partitioningResult, const Partitioning::Transistor & transistor, int tranIndex);
		void writeTwoPort(const Partitioning::Result & partitioningResult, const Partitioning::TwoPort & twoPort, int twoPortIndex);
		void writeDevices(const Core::Circuit & circuit, const Partitioning::Result * partitioningResult = nullptr);

		Core::NetId findGateNetOfSecondTransconductanceDevice(const Partitioning::Result & partitioningResult,
				Partitioning::TransconductancePart & secondStage) const;

		std::string createDeviceInformation(const Partitioning::Result & partitioningResult, const Core::Device & device) const;
		std::string createTransconductorInformation(const Partitioning::Result & partitioningResult, const Partitioning::TransconductancePart & transcoductor) const;
		std::string createBiasInformation(const Partitioning::Result & partitioningResult, const Partitioning::BiasPart & bias) const;
		std::string createLoadInformation(const Partitioning::Result & partitioningResult, const Partitioning::LoadPart & load) const;
		std::string createResistorInformation(const Partitioning::Result & partitioningResult, const Partitioning::ResistorPart & resistor) const;
		std::string createCapacitorInformation(const Partitioning::Result & partitioningResult, const Partitioning::CapacitancePart & capacitor) const;
		std::string createCircuitTypeInformation(const Partitioning::Result & partitioningResult) const;

		int  createIndexSecondStage(const Partitioning::Result & partitioningResult, const Partitioning::Part & part) const;

	private:
		static const std::string PATH_NOT_SET_;
		static const std::string ID_NOT_SET_;

		std::string getPath() const;
		std::string getId() const;
		bool usesHspiceLibrary() const;

		Control::OutputFile & getOutputFile();

		std::string id_;
		std::string path_;
		bool usesHspiceLibrary_;
		Control::OutputFile * outputFile_;

	};

}

#endif /* SYNTHESIS_INCL_RESULTS_HSPICEOUTPUTFILE_H_ */
