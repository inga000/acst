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

#ifndef PARTITIONING_INCL_PARTS_PART_H_
#define PARTITIONING_INCL_PARTS_PART_H_

#include "Partitioning/incl/Parts/PartId/PartId.h"
#include "StructRec/incl/StructureCircuit/Structure/Structure.h"
#include "Core/incl/Common/RapidXml.h"
#include "Core/incl/Common/Object.h"

namespace Partitioning {

	class Result;

	class Part : public Core::Object
	{
	public:
		Part();
		~Part();

		void addMainStructure(const StructRec::Structure & mainStructure, Result & result);
		void setPartId(const PartId & partId);

		const std::vector<const StructRec::Structure*> & getMainStructures() const;
		std::vector<const StructRec::Structure*> & getMainStructures();
		PartId getPartId() const;


		void addBelongingMainStructure(std::vector<const StructRec::Structure*>  structures, Result & result);

		bool hasAsMainStructure(const StructRec::Structure & structure) const;
		bool hasMainStructures() const;

		virtual bool isBiasPart() const = 0;
		virtual bool isLoadPart() const = 0;
		virtual bool isTransconductancePart() const = 0;
		virtual bool isCapacitancePart() const = 0;
		virtual bool isUndefinedPart() const = 0;
		virtual bool isResistorPart() const = 0;
		virtual bool isCommonModeSignalDetectorPart() const = 0;
		virtual bool isPositiveFeedbackPart() const = 0;

		std::string toStr() const;

		void printMainStructures(std::ostream & stream) const;
		virtual void writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const;

	protected:
		bool hasPartId() const;

		void initializeComponents(const StructRec::Structure & structure, Result & result);

		virtual void print(std::ostream & stream) const = 0;

	private:
		PartId partId_;
		std::vector<const StructRec::Structure *> mainStructures_;
	};

}


#endif /* STRUCTUREDANALOGDESIGN_INCL_PARTITIONING_PARTS_PART_H_ */
