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

#include "StructRec/incl/Results/Result.h"
#include "Core/incl/Common/BacktraceAssert.h"
#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "Control/incl/File/OutputFile.h"
#include <sstream>

namespace StructRec {

	Result::Result() :
			topLevelResults_(nullptr),
			circuitRegister_(nullptr)
	{
	}

	Result::~Result()
	{
		delete &getTopLevelResults().getStructureCore();
		delete topLevelResults_;
		delete circuitRegister_;
	}

	void Result::setTopLevelResults(StructureCircuits& topLevelResults)
	{
		topLevelResults_=& topLevelResults;
	}

	void Result::setCircuitRegister(CircuitRegister& circuitRegister)
	{
		circuitRegister_ = & circuitRegister;
	}

	void Result::removeDominatedStructures(const DominanceRelation& dominanceRelation, int level)
	{
		dominanceRelation.removeDominatedStructures(getTopLevelResults(), level);
		getCircuitRegister().removeDominatedStructures(dominanceRelation, level);
	}

	void Result::removeDominatedStructures(const DominanceRelationForCurrentMirrors& dominanceRelation, int level)
	{
		dominanceRelation.removeDominatedStructures(getTopLevelResults(), level);
		getCircuitRegister().removeDominatedStructures(dominanceRelation, level);
	}


	void Result::removeUncertainStructures()
	{

		getTopLevelResults().removeUncertainStructures();
		getCircuitRegister().removeUncertainStructures();
	}

	const CircuitRegister& Result::getCircuitRegister() const
	{
		assert(circuitRegister_ != nullptr);
		return * circuitRegister_;
	}

	const StructureCircuits& Result::getTopLevelResults() const
	{
		assert(topLevelResults_ != nullptr);
		return *topLevelResults_;
	}

	StructureCircuits& Result::getTopLevelResults()
	{
		assert(topLevelResults_ != nullptr);
		return *topLevelResults_;
	}

	CircuitRegister& Result::getCircuitRegister()
	{
		assert(circuitRegister_ != nullptr);
		return * circuitRegister_;
	}

	std::string Result::toStr() const
	{
		std::ostringstream stream;
		int columns = 0;
		stream << "TopLevelCircuit: " << std::endl;
		print(stream, columns);
		return stream.str();
	}

	void Result::print(std::ostream& stream, int columns) const
	{
		Core::InstanceNamePath instanceNamePath;
		instanceNamePath.setIsRoot();
		const StructureCircuits& results = getTopLevelResults();
		results.print(stream,columns, instanceNamePath);

	}

	void Result::writeXml(Control::OutputFile& file) const
	{
		file.open();

		Core::XmlDocument doc;
		Core::XmlNode & root = Core::RapidXmlUtils::createXMLRootNodeForResults(doc);
		Core::RapidXmlUtils::writeDateXML(root, doc);
		Core::XmlNode & headLine = Core::RapidXmlUtils::addNode(root, doc, "structure_recognition_results");


		const StructureCircuits & results = getTopLevelResults();
		results.writeXml(headLine,doc);

		std::ostringstream oss;
		oss << doc;
		file << oss.str();

		file.close();
	}
}
