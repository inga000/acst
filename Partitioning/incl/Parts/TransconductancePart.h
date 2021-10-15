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

#ifndef PARTITIONING_INCL_PARTS_TRANSCONDUCTANCEPART_H_
#define PARTITIONING_INCL_PARTS_TRANSCONDUCTANCEPART_H_

#include "Partitioning/incl/Parts/BiasPart.h"
#include "Partitioning/incl/Parts/LoadPart.h"
#include "Partitioning/incl/Parts/Part.h"

namespace AutomaticSizing {

	class CircuitParameter;
}


namespace Partitioning {

	class Component;
	class Result;

	class TransconductancePart : public Part
	{
	public:
		TransconductancePart(int & num);
		~TransconductancePart();

		void setType(const std::string type);
		void setFirstStageType(const std::string firstStageType);
		void setHelperStructure(const StructRec::Structure & helperStructure);
		void addBiasPart(BiasPart & biasPart);
		void addLoadPart(LoadPart & loadPart);

		bool isInitialized() const;

		const StructRec::Structure & getHelperStructure() const;
		std::vector<BiasPart*> & getBiasPart();
		std::vector<LoadPart*> & getLoadPart();
		std::string getType() const;
		std::string getFirstStageType() const;

		std::vector<const Core::Net*> getOutputNets();

		bool isFirstStage() const;
        bool isPrimarySecondStage() const;
        bool isSecondarySecondStage() const;
        bool isThirdStage() const;
        bool isFeedBack() const;
        bool isHigherStage() const;

        bool isSimple() const;
        bool isComplementary() const;
        bool isFoldedCascode() const;
        bool isTelescopic() const;
        bool isSymmetrical() const;

        bool hasHelperStructure() const;
        bool hasLoadPart() const;
        bool hasBiasPart(PartId partId) const;

        bool hasFirstStageType() const;

        bool isBiasPart() const;
        bool isLoadPart() const;
        bool isTransconductancePart() const;
        bool isCapacitancePart() const;
        bool isUndefinedPart() const;
		bool isResistorPart() const;
		bool isCommonModeSignalDetectorPart() const;
		bool isPositiveFeedbackPart() const;

		Component & findComponentNotConnectedToVref(
				const AutomaticSizing::CircuitParameter & circuitParameter, const Result & partitioningResult) const;

		std::vector<Component*> findSupplyConnectedComponents(const Result & partitioningResult) const;


        void print(std::ostream &stream) const;
		virtual void writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const;

	private:
		enum TypeEnum {
		       TYPE_UNINITIALIZED,
		       TYPE_FIRSTSTAGE,
		       TYPE_PRIMARYSECONDSTAGE,
			   TYPE_SECONDARYSECONDSTAGE,
		       TYPE_FEEDBACK,
			   TYPE_THIRDSTAGE
	   };

		enum FirstStageTypeEnum {
			FIRSTSTAGETYPE_UNINITIALIZED,
			TYPE_SIMPLE,
			TYPE_COMPLEMENTARY,
			TYPE_FOLDEDCASCODE,
			TYPE_TELESCOPIC,
			TYPE_SYMMETRICAL
		};

	  static const std::string FIRSTSTAGE_STRING_;
	  static const std::string PRIMARYSECONDSTAGE_STRING_;
	  static const std::string SECONDARYSECONDSTAGE_STRING_;
	  static const std::string THIRDSTAGE_STRING_;
	  static const std::string FEEDBACK_STRING_;

	  static const std::string SIMPLE_STRING_;
	  static const std::string COMPLEMENTARY_STRING_;
	  static const std::string FOLDEDCASCODE_STRING_;
	  static const std::string TELESCOPIC_STRING_;
	  static const std::string SYMMETRICAL_STRING_;

	  typedef std::map<std::string,TypeEnum> StringToTypeEnumMap;
	  typedef std::map<TypeEnum,std::string> TypeEnumToStringMap;
	  typedef std::map<std::string,FirstStageTypeEnum> StringToFirstStageTypeEnumMap;
	  typedef std::map<FirstStageTypeEnum,std::string> FirstStageTypeEnumToStringMap;

	  static bool isValidString(const std::string & ttStr);

	  static const StringToTypeEnumMap & getStringToTypeEnumMap();
	  static const TypeEnumToStringMap & getTypeEnumToStringMap();
	  static const StringToFirstStageTypeEnumMap & getStringToFirstStageTypeEnumMap();
	  static const FirstStageTypeEnumToStringMap & getFirstStageTypeEnumToStringMap();

	  static TypeEnum mapStringToTypeEnum(const std::string & ttStr);
	  static std::string mapTypeEnumToString(const TypeEnum & tt);
	  static FirstStageTypeEnum mapStringToFirstStageTypeEnum(const std::string & ttStr);
	  static std::string mapFirstStageTypeEnumToString(const FirstStageTypeEnum & tt);

	private:
	  bool hasBiasPart() const;
	  bool hasType() const;

	private:
	  const StructRec::Structure * helperStructure_;
	  std::vector<BiasPart*> biasParts_;
	  std::vector<LoadPart*> loadParts_;
	  TypeEnum typeEnum_;
	  FirstStageTypeEnum firstStageTypeEnum_;
	};

}



#endif /* STRUCTUREDANALOGDESIGN_INCL_PARTIONING_TRANSCONDUCTANCEPART_H_ */
