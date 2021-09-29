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


#include "Synthesis/incl/LibraryFile/LibraryFile.h"
#include "Synthesis/incl/Library/Library.h"

#include "Core/incl/DeviceTypeRegister/DeviceTypeRegister.h"
#include "Core/incl/Circuit/Circuit.h"

#include "HSpice/incl/DeviceLineMappingFile/DeviceLineMapper.h"

#include "Control/incl/FileSystemPath.h"
#include "Log/incl/LogMacros.h"



namespace Synthesis {

	const std::string LibraryFile::AMPLIFICATION_STAGES_NODE_ = "AmplificationStages";
	const std::string LibraryFile::FIRST_STAGES_NODE_ = "FirstStages";
	const std::string LibraryFile::SYMMETRICAL_FIRST_STAGES_NODE_ = "SymmetricalFirstStages";
	const std::string LibraryFile::SECOND_STAGES_NODE_ = "SecondStages";
	const std::string LibraryFile::COMPLEMENTARY_SECOND_STAGES_NODE_ = "ComplementarySecondStages";
	const std::string LibraryFile::STAGE_NODE_ = "Stage";

	LibraryFile::LibraryFile():
		library_(NULL),
		deviceTypeRegister_(nullptr),
		deviceLineMapper_(nullptr)
	{
	}

	LibraryFile::~LibraryFile()
	{
		delete deviceLineMapper_;
	}


	Library* LibraryFile::parse()
	{

		Control::XmlInputFile::parse();
		library_ = new Library();

		Core::XmlNode* amplificationStagesNode = getDocumentNode().first_node(AMPLIFICATION_STAGES_NODE_.c_str());
		parseFirstStages(amplificationStagesNode);
		logDebug("Parsed FirstStages successfully")
		parseSymmetricalFirstStages(amplificationStagesNode);
		logDebug("Parsed SymmetricalFirstStages successfully")
		parseSecondStages(amplificationStagesNode);
		logDebug("Parsed SecondStages successfully")
		parseComplementarySecondStages(amplificationStagesNode);
		logDebug("Parsed ComplementarySecondStages successfully")
		return &getLibrary();
	}


	void LibraryFile::parseFirstStages(Core::XmlNode* node)
	{
		Core::XmlNode* firstStagesNode = node->first_node(FIRST_STAGES_NODE_.c_str());
		Core::XmlNode* stageNode = firstStagesNode->first_node(STAGE_NODE_.c_str());

		while(stageNode != NULL)
		{
			std::string relativeFilePath = stageNode->value();
			std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);

			Core::Circuit & stage = parseStage(absoluteFilePath);
			getLibrary().addFirstStage(stage);
			stageNode = stageNode->next_sibling(STAGE_NODE_.c_str());
		}

	}

	void LibraryFile::parseSymmetricalFirstStages(Core::XmlNode* node)
	{
		Core::XmlNode* firstStagesNode = node->first_node(SYMMETRICAL_FIRST_STAGES_NODE_.c_str());
		Core::XmlNode* stageNode = firstStagesNode->first_node(STAGE_NODE_.c_str());

		while(stageNode != NULL)
		{
			std::string relativeFilePath = stageNode->value();
			std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);
			logDebug("Parsing: " << relativeFilePath);

			Core::Circuit & stage = parseStage(absoluteFilePath);
			getLibrary().addSymmetricalFirstStage(stage);
			stageNode = stageNode->next_sibling(STAGE_NODE_.c_str());
		}

	}

	void LibraryFile::parseSecondStages(Core::XmlNode* node)
	{
		Core::XmlNode* secondStagesNode = node->first_node(SECOND_STAGES_NODE_.c_str());
		Core::XmlNode* stageNode = secondStagesNode->first_node(STAGE_NODE_.c_str());

		while(stageNode != NULL)
		{
			std::string relativeFilePath = stageNode->value();
			std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);
			logDebug("Parsing: " << relativeFilePath);

			Core::Circuit & stage = parseStage(absoluteFilePath);
			getLibrary().addSecondStage(stage);
			stageNode = stageNode->next_sibling(STAGE_NODE_.c_str());
		}
	}

	void LibraryFile::parseComplementarySecondStages(Core::XmlNode* node)
	{
		Core::XmlNode* secondStagesNode = node->first_node(COMPLEMENTARY_SECOND_STAGES_NODE_.c_str());
		Core::XmlNode* stageNode = secondStagesNode->first_node(STAGE_NODE_.c_str());

		while(stageNode != NULL)
		{
			std::string relativeFilePath = stageNode->value();
			std::string absoluteFilePath = computeAbsoluteFilePath(relativeFilePath);
			logDebug("Parsing: " << relativeFilePath);

			Core::Circuit & stage = parseStage(absoluteFilePath);
			getLibrary().addComplementarySecondStage(stage);
			stageNode = stageNode->next_sibling(STAGE_NODE_.c_str());
		}
	}

	Core::Circuit& LibraryFile::parseStage(std::string filePath)
	{
        HSpice::InputFile::InputFile* hspiceInputFile = new HSpice::InputFile::InputFile();

        hspiceInputFile->setPath(filePath);
        hspiceInputFile->setSupplyNetIds(getSupplyNetIdentifiers());

        hspiceInputFile->setDeviceTypeRegister(getDeviceTypeRegister());
        hspiceInputFile->setDeviceLineMapper(getDeviceLineMapper());

        return *hspiceInputFile->readNewCircuit();
	}

	std::string LibraryFile::computeAbsoluteFilePath(const std::string& relativePath) const
	{
		Control::FileSystemPath fileSystemPath;
		fileSystemPath.setPath(relativePath);
		return fileSystemPath.getAbsolutePathRelativeToFile(getPath());
	}

	const Library& LibraryFile::getLibrary() const
	{
		assert(library_ != NULL);
		return *library_;
	}

	const Core::DeviceTypeRegister& LibraryFile::getDeviceTypeRegister() const
	{
		assert(deviceTypeRegister_ != NULL);
		return *deviceTypeRegister_;
	}

	Library& LibraryFile::getLibrary()
	{
		assert(library_ != nullptr);
		return *library_;
	}

	const HSpice::DeviceLineMapper & LibraryFile::getDeviceLineMapper() const
	{

		assert(deviceLineMapper_ != nullptr);
		return *deviceLineMapper_;
	}
	HSpice::InputFile::SupplyNetIdentifiers LibraryFile::getSupplyNetIdentifiers() const
	{
		assert(supplyNetIds_.isInitialized());
		return supplyNetIds_;
	}

	void LibraryFile::setDeviceTypeRegister(const Core::DeviceTypeRegister & deviceTypeRegister)
	{
		deviceTypeRegister_ = & deviceTypeRegister;
	}

	void LibraryFile::setDeviceLineMapper(const HSpice::DeviceLineMapper & deviceLineMapper)
	{
		deviceLineMapper_ = &deviceLineMapper;
	}

	void LibraryFile::setSupplyNetIdentifiers(const HSpice::InputFile::SupplyNetIdentifiers supplyNetIds)
	{
		supplyNetIds_ = supplyNetIds;
	}
}
