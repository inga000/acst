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

#ifndef PARTITIONING_INCL_PARTS_LOADPART_H_
#define PARTITIONING_INCL_PARTS_LOADPART_H_

#include "Partitioning/incl/Parts/BiasPart.h"
#include "Partitioning/incl/Parts/Part.h"

namespace Partitioning {

	class Result;

	class LoadPart : public Part
	{
	public:
		LoadPart(int & num);
		LoadPart();
		~LoadPart();

		void setCascodedPair(const StructRec::Structure & cascodedPair);
		void addCurrentBiasOfFoldedPair(const StructRec::Structure & currentBiasOfFoldedPair);
		void addBiasPart(Part & biasPart);

		bool isInitialized() const;

		const StructRec::Structure & getCascodedPair() const;
		std::vector<const StructRec::Structure*> & getCurrentBiasOfFoldedPair();
		Part & getBiasPart(PartId partId) const;
		const std::vector<Part*> & getBiasParts() const;

		bool hasCascodedPair() const;
		bool hasCurrentBiasOfFoldedPair() const;
		bool hasBiasPart() const;
		bool hasBiasPart(PartId partId) const;
		bool hasCrossCoupledPair(const Partitioning::Result & result) const;

		bool isBiasPart() const;
		bool isLoadPart() const;
		bool isTransconductancePart() const;
		bool isCapacitancePart() const;
		bool isUndefinedPart() const;
		bool isResistorPart() const;
		bool isCommonModeSignalDetectorPart() const;
		bool isPositiveFeedbackPart() const;

		void createPartId(int & id);

		bool consistsOfTwoDiodeArraysConnectedToSecondStageInput(const Partitioning::Result & result) const;

		void print(std::ostream & stream) const;
		void writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const;

	private:

		const StructRec::Structure * cascodedPair_;
		std::vector<const StructRec::Structure *> currentBiasOfFoldedPair_;
		std::vector<Part*> biasParts_;

	};

}



#endif /* STRUCTUREDANALOGDESIGN_INCL_PARTITIONING_PARTS_LOADPART_H_ */
