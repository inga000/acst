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
#include "Core/incl/Common/RapidXmlUtils.h"
#include <boost/lexical_cast.hpp>
#include <string>
// Added a new header file
#include <time.h>

namespace Core {

    void RapidXmlUtils::addAttr(XmlNode & node, XmlDocument & doc, const std::string & name, const std::string & value)
    {
        char* nc = doc.allocate_string(name.c_str());
        char* vc = doc.allocate_string(value.c_str());
        XmlAttribute* newAattr = doc.allocate_attribute(nc,vc);
        node.append_attribute(newAattr);
    }

    XmlNode & RapidXmlUtils::addNode(XmlNode & node, XmlDocument & doc, const std::string & name,
            const std::string & content)
    {
        char* nc = doc.allocate_string(name.c_str());
        char* cc = doc.allocate_string(content.c_str());
        XmlNode* newNode = doc.allocate_node(rapidxml::node_element,nc,cc);
        node.append_node(newNode);
        return *newNode;
    }

    void RapidXmlUtils::addSimpleNode(XmlNode & node, XmlDocument & doc, const std::string & name,
                const std::string & content)
    {
    	char* nc = doc.allocate_string(name.c_str());
        char* cc = doc.allocate_string(content.c_str());
        XmlNode* newNode = doc.allocate_node(rapidxml::node_element,nc,cc);
        node.append_node(newNode);
    }

    XmlNode& RapidXmlUtils::createXMLRootNodeForResults(XmlDocument& doc)
    {
        XmlNode* root = doc.allocate_node(rapidxml::node_element,"acst_results");
        doc.append_node(root);
        return *root;
    }

    void RapidXmlUtils::writeDateXML(XmlNode& node, XmlDocument& doc)
    {
        XmlAttribute* attr;

        XmlNode* date= doc.allocate_node(rapidxml::node_element,"date");
        node.append_node(date);

        time_t currentTime;
        time(&currentTime);
        tm* ltime = localtime(&currentTime);

        char* day = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_mday).c_str());
        attr = doc.allocate_attribute("day",day);
        date->append_attribute(attr);

        char* month = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_mon+1).c_str());
        attr = doc.allocate_attribute("month",month);
        date->append_attribute(attr);

        char* year = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_year+1900).c_str());
        attr = doc.allocate_attribute("year",year);
        date->append_attribute(attr);

        char* hour = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_hour).c_str());
        attr = doc.allocate_attribute("hour",hour);
        date->append_attribute(attr);

        char* minute = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_min).c_str());
        attr = doc.allocate_attribute("minute",minute);
        date->append_attribute(attr);

        char* second = doc.allocate_string(boost::lexical_cast<std::string>(ltime->tm_sec).c_str());
        attr = doc.allocate_attribute("second",second);
        date->append_attribute(attr);
    }

    void RapidXmlUtils::writeStyleDeclaration(XmlDocument & doc)
    {
    	Core::XmlNode* decl = doc.allocate_node(rapidxml::node_declaration);
    	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    	doc.append_node(decl);
    }

    XmlNode& RapidXmlUtils::createXMLRootNode(XmlDocument& doc, const std::string & name)
    {
    	char* nc = doc.allocate_string(name.c_str());
    	XmlNode* root = doc.allocate_node(rapidxml::node_element, nc);
        doc.append_node(root);
        return *root;
    }

    // Added a new function here.
    XmlNode& RapidXmlUtils::createXMLRootNodeForWicked(XmlDocument& doc)
    {
    	Core::XmlNode* wickedRoot = doc.allocate_node(rapidxml::node_element, "SETTINGS");
    	doc.append_node(wickedRoot);

    	time_t currentTime;
    	time(&currentTime);
    	tm* ltime = localtime(&currentTime);

    	std::stringstream ss;

    	std::string day = boost::lexical_cast<std::string>(ltime->tm_mday);
    	std::string month = boost::lexical_cast<std::string>(ltime->tm_mon + 1);
    	std::string year = boost::lexical_cast<std::string>(ltime->tm_year + 1900);
    	std::string date = month + "/" + day + "/" + year;
    	wickedRoot->append_attribute(doc.allocate_attribute("DATE", doc.allocate_string(date.c_str())));

    	std::string hour = boost::lexical_cast<std::string>(ltime->tm_hour);
    	std::string min = boost::lexical_cast<std::string>(ltime->tm_min);
    	std::string sec = boost::lexical_cast<std::string>(ltime->tm_sec);
    	std::string time = hour + ":" + min + ":" + sec;
    	wickedRoot->append_attribute(doc.allocate_attribute("TIME", doc.allocate_string(time.c_str())));

    	wickedRoot->append_attribute(doc.allocate_attribute("TOOLVERSION", "6.7"));

    	wickedRoot->append_attribute(doc.allocate_attribute("VERSION", "1.3"));

    	return *wickedRoot;
    }

}
