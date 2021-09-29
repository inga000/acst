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

#include "StructRec/incl/StructureCircuit/Structure/StructureId/StructureName.h"

#include "Core/incl/Circuit/Instance/InstanceId/InstanceNamePath.h"
#include "Core/incl/Common/RapidXmlUtils.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml_print.hpp>

#include <string>

namespace StructRec {

	StructureName::StructureName() :
		Name<StructureName>()
	{
	}

	StructureName::StructureName(const std::string& name) :
		Name<StructureName>(name)
	{
	}

	StructureName::operator std::string() const
	{
		return getName();
	}

	void StructureName::writeXml(Core::XmlNode& node,
			Core::XmlDocument& doc) const
	{
		Core::RapidXmlUtils::addSimpleNode(node, doc, "structureName", toStr());

	}

	void StructureName::removeInstanceNamePathFromName(Core::InstanceNamePath instanceNamePath)
	{
		std::size_t pos = name_.find(instanceNamePath.getPath());

		if(pos != std::string::npos)
		{
			name_.erase(pos, instanceNamePath.getPath().length());
		}
	}

}


