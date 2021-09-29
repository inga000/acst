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
 
#ifndef CORE_RAPIDXMLUTILS_H_
#define CORE_RAPIDXMLUTILS_H_

#include <Core/incl/Common/RapidXml.h>
#include <string>

namespace Core {

    class RapidXmlUtils
    {
    public:
        static void addAttr(XmlNode & node, XmlDocument & doc, const std::string & name, const std::string & value);
        static XmlNode & addNode(XmlNode & node, XmlDocument & doc, const std::string & name, const std::string & content = "");
        static void addSimpleNode(XmlNode & node, XmlDocument & doc, const std::string & name, const std::string & content = "");

    public:
        static XmlNode& createXMLRootNode(XmlDocument& doc, const std::string & name);
        static XmlNode& createXMLRootNodeForResults(XmlDocument & doc);
        static XmlNode& createXMLRootNodeForWicked(XmlDocument & doc); // New function here
        static void writeStyleDeclaration(XmlDocument & doc);
        static void writeDateXML (XmlNode& node, XmlDocument& doc);
    };

}

#endif
