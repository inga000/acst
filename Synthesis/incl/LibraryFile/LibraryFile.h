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

#ifndef SYNTHESIS_INCL_LIBRARYFILE_LIBRARYFILE_H_
#define SYNTHESIS_INCL_LIBRARYFILE_LIBRARYFILE_H_

#include "HSpice/incl/InputFile/InputFile.h"
#include "Control/incl/File/XmlInputFile.h"
#include "Core/incl/Common/RapidXml.h"
#include <rapidxml.hpp>

namespace HSpice {
	class DeviceLineMapper;

}

namespace Core{
	class Circuit;
	class DeviceTypeRegister;
}

namespace Synthesis {

	class Library;
	class LibraryFile: public Control::XmlInputFile
	{
	public:
		LibraryFile();
		~LibraryFile();

		void setDeviceTypeRegister(const Core::DeviceTypeRegister & deviceTypeRegister);
		void setDeviceLineMapper(const HSpice::DeviceLineMapper & deviceLineMapper);
		void setSupplyNetIdentifiers(HSpice::InputFile::SupplyNetIdentifiers supplyNetIds);

		Library* parse();


	private:
		void parseFirstStages(Core::XmlNode* node);
		void parseSymmetricalFirstStages(Core::XmlNode* node);
		void parseSecondStages(Core::XmlNode* node);
		void parseComplementarySecondStages(Core::XmlNode* node);

		Core::Circuit& parseStage(std::string filePath);

		std::string computeAbsoluteFilePath(const std::string& relativeFilePath) const;

	private:
		Library& getLibrary();
		const Library& getLibrary() const;
		const Core::DeviceTypeRegister & getDeviceTypeRegister() const;
		const HSpice::DeviceLineMapper & getDeviceLineMapper() const;
		HSpice::InputFile::SupplyNetIdentifiers getSupplyNetIdentifiers() const;

	private:
		static const std::string AMPLIFICATION_STAGES_NODE_;
		static const std::string FIRST_STAGES_NODE_;
		static const std::string SYMMETRICAL_FIRST_STAGES_NODE_;
		static const std::string SECOND_STAGES_NODE_;
		static const std::string COMPLEMENTARY_SECOND_STAGES_NODE_;
		static const std::string STAGE_NODE_;

	private:
		Library* library_;

		const Core::DeviceTypeRegister * deviceTypeRegister_;
		const HSpice::DeviceLineMapper * deviceLineMapper_;
		 HSpice::InputFile::SupplyNetIdentifiers supplyNetIds_;
	};
}




#endif /* SYNTHESIS_INCL_LIBRARYFILE_LIBRARYFILE_H_ */
