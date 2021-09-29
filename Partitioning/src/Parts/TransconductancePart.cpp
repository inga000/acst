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

#include "Partitioning/incl/Parts/TransconductancePart.h"

#include "Partitioning/incl/Results/Component.h"
#include "Partitioning/incl/Results/Result.h"


#include "Core/incl/Circuit/Device/DeviceType/MosfetDeviceType.h"

#include "AutomaticSizing/incl/CircuitInformation/CircuitParameter.h"


#include "Core/incl/Common/BacktraceAssert.h"

namespace Partitioning {

	const std::string TransconductancePart::FIRSTSTAGE_STRING_ = "firstStage";
   const std::string TransconductancePart::PRIMARYSECONDSTAGE_STRING_ = "primarySecondStage";
   const std::string TransconductancePart::SECONDARYSECONDSTAGE_STRING_ = "secondarySecondStage";
   const std::string TransconductancePart::THIRDSTAGE_STRING_ = "thirdStage";
   const std::string TransconductancePart::FEEDBACK_STRING_ = "feedBack";
   const std::string TransconductancePart::COMPLEMENTARY_STRING_ = "complementary";

   TransconductancePart::TransconductancePart(int & num) :
           typeEnum_(TYPE_UNINITIALIZED),
		   helperStructure_(NULL)
   {
		const PartName & name = PartName("TransconductancePart");
	    PartId id;
	    id.setName(name);
	    id.setId(num);
	    setPartId(id);
	    num++;
   }

   TransconductancePart::~TransconductancePart()
   {
   }

   void TransconductancePart::setType(const std::string string)
   {
      typeEnum_ = mapStringToEnum(string);
   }

   bool TransconductancePart::isValidString(const std::string & str)
   {
       return getStringToEnumMap().find(str) != getStringToEnumMap().end();
   }

   std::string TransconductancePart::getType() const
   {
       assert(hasType());
       return mapEnumToString(typeEnum_);
   }

   TransconductancePart::TypeEnum TransconductancePart::mapStringToEnum(const std::string & str)
   {
       assert(isValidString(str));
       return getStringToEnumMap().at(str);
   }

   bool TransconductancePart::isInitialized() const
   {
       return hasType() && hasMainStructures() && hasPartId();
   }

   std::vector<const Core::Net*> TransconductancePart::getOutputNets()
   {
	   std::vector<const Core::Net*> outputNets;

	   Core::MosfetDeviceType mosfet;

	   if(isFirstStage() && hasHelperStructure())
	   {
		   std::vector<const StructRec::Structure *> arrayChildren = getHelperStructure().getArrayChildren();

		   for(auto & array : arrayChildren)
		   {
			   const Core::Device & device = **array->getDevices().begin();
			   outputNets.push_back(&device.findNet(mosfet.drain()));


		   }
	   }
	   else
	   {
		   for(auto & mainStruct : getMainStructures())
		   {
			   std::vector<const StructRec::Structure *> arrayChildren = mainStruct->getArrayChildren();

			   for(auto & array : arrayChildren)
			   {
				   const Core::Device & device = **array->getDevices().begin();
				   outputNets.push_back(&device.findNet(mosfet.drain()));

			   }
		   }
	   }
	   return outputNets;
   }


   bool TransconductancePart::isFirstStage() const
   {
       assert(hasType());
       return typeEnum_ == TYPE_FIRSTSTAGE;
   }

   bool TransconductancePart::isPrimarySecondStage() const
   {
       assert(hasType());
       return typeEnum_ == TYPE_PRIMARYSECONDSTAGE;
   }

   bool TransconductancePart::isSecondarySecondStage() const
   {
       assert(hasType());
       return typeEnum_ == TYPE_SECONDARYSECONDSTAGE;
   }

   bool TransconductancePart::isThirdStage() const
   {
      assert(hasType());
      return typeEnum_ == TYPE_THIRDSTAGE;
   }

   bool TransconductancePart::isHigherStage() const
   {
      assert(hasType());
      return typeEnum_ == TYPE_THIRDSTAGE || typeEnum_ == TYPE_PRIMARYSECONDSTAGE || typeEnum_ == TYPE_SECONDARYSECONDSTAGE;
   }

   bool TransconductancePart::isFeedBack() const
   {
       assert(hasType());
       return typeEnum_ == TYPE_FEEDBACK;
   }

   bool TransconductancePart::isComplementary() const
   {
       assert(isInitialized());
       return typeEnum_ == TYPE_COMPLEMENTARY;
   }

   void TransconductancePart::setHelperStructure(
		const StructRec::Structure& helperStructure)
   {
	   helperStructure_ = & helperStructure;
   }

   void TransconductancePart::addBiasPart(BiasPart& biasPart)
   {
	   biasParts_.push_back(& biasPart);
   }

   void TransconductancePart::addLoadPart(LoadPart& loadPart)
   {
	   loadParts_.push_back(&loadPart);
   }

   const StructRec::Structure& TransconductancePart::getHelperStructure() const
   {
	   assert(hasHelperStructure());
	   return * helperStructure_;
   }

   std::vector<BiasPart*>& TransconductancePart::getBiasPart()
   {
	   assert(hasBiasPart());
	   return biasParts_;
   }

   std::vector<LoadPart*>& TransconductancePart::getLoadPart()
   {
	   assert(hasLoadPart());
	   return loadParts_;
   }

   bool TransconductancePart::hasHelperStructure() const
   {
	   return helperStructure_ != NULL;
   }

   std::string TransconductancePart::mapEnumToString(const TypeEnum & tt)
   {
       return getEnumToStringMap().at(tt);
   }

   const TransconductancePart::StringToEnumMap & TransconductancePart::getStringToEnumMap()
   {
       static StringToEnumMap theMap;

       if(theMap.empty()) {
           theMap[FIRSTSTAGE_STRING_] = TYPE_FIRSTSTAGE;
           theMap[PRIMARYSECONDSTAGE_STRING_] = TYPE_PRIMARYSECONDSTAGE;
           theMap[SECONDARYSECONDSTAGE_STRING_] = TYPE_SECONDARYSECONDSTAGE;
           theMap[THIRDSTAGE_STRING_] = TYPE_THIRDSTAGE;
           theMap[FEEDBACK_STRING_] = TYPE_FEEDBACK;
           theMap[COMPLEMENTARY_STRING_] = TYPE_COMPLEMENTARY;
       }
       return theMap;
   }

   const TransconductancePart::EnumToStringMap & TransconductancePart::getEnumToStringMap()
   {
       static EnumToStringMap theMap;

       if(theMap.empty()) {
           theMap[TYPE_FIRSTSTAGE] = FIRSTSTAGE_STRING_;
           theMap[TYPE_PRIMARYSECONDSTAGE] = PRIMARYSECONDSTAGE_STRING_;
           theMap[TYPE_SECONDARYSECONDSTAGE] = SECONDARYSECONDSTAGE_STRING_;
           theMap[TYPE_THIRDSTAGE] = THIRDSTAGE_STRING_;
           theMap[TYPE_FEEDBACK] = FEEDBACK_STRING_;
           theMap[TYPE_COMPLEMENTARY] = COMPLEMENTARY_STRING_;
       }
       return theMap;
   }

   bool TransconductancePart::hasBiasPart() const
   {
	   return !biasParts_.empty();
   }

	bool TransconductancePart::hasBiasPart(PartId partId) const
	{
		bool hasBiasPart = false;

		for(std::vector<BiasPart*>::const_iterator it = biasParts_.begin(); it != biasParts_.end(); it++)
		{
			BiasPart * posPart = *it;
			if(posPart->getPartId() == partId)
			{
				hasBiasPart = true;
				break;
			}
		}
		return hasBiasPart;
	}

   bool TransconductancePart::hasLoadPart() const
   {
	   return !loadParts_.empty();
   }

   bool TransconductancePart::hasType() const
   {
	   return typeEnum_ != TYPE_UNINITIALIZED;
   }

   void TransconductancePart::print(std::ostream &stream) const
   {
	   stream << " Type: " << getType() << std::endl;
	   if(hasHelperStructure())
	   {
		   stream << " HelperStructure: " << getHelperStructure().getIdentifier().toStr() << std::endl;
	   }
	   stream << " BiasParts: " << std::endl;
	   for(std::vector<BiasPart*>::const_iterator it_bias = biasParts_.begin(); it_bias != biasParts_.end(); it_bias++)
	   {
		   BiasPart * biasPart = *it_bias;
		   stream << "  " << biasPart->getPartId().toStr() << " ";
		   biasPart->printMainStructures(stream);
		   stream << std::endl;
	   }

	   if(hasLoadPart())
	   {
		   stream << " LoadParts: " << std::endl ;
		   for(std::vector<LoadPart*>::const_iterator it_load = loadParts_.begin(); it_load != loadParts_.end(); it_load++)
		   {
		   		   LoadPart * loadPart = *it_load;
		   		   stream << "  " << loadPart->getPartId().toStr() << " ";
		   		   loadPart->printMainStructures(stream);
		   		   stream << std::endl;
		   	   }
	   }
   }

   void TransconductancePart::writeXml(Core::XmlNode& xmlNode, Core::XmlDocument& doc) const
   {
	   Core::XmlNode& gmPart = Core::RapidXmlUtils::addNode(xmlNode, doc, "gmPart");
	   Core::RapidXmlUtils::addAttr(gmPart, doc, "type", getType());
	   Part::writeXml(gmPart, doc);
   }


   Component & TransconductancePart::findComponentNotConnectedToVref(
		   const AutomaticSizing::CircuitParameter & circuitParameter, const Result & partitioningResult) const
   {
	   assert(isFeedBack(), "Vref only exist for feedback components" );
	   std::vector<Component *> transPartComps = partitioningResult.getBelongingComponents(*this);
	   if(circuitParameter.hasReferenceVoltagePin())
	   {
		   Component * wantedComp =nullptr;
		   for(auto & comp : transPartComps)
		   {
			   const StructRec::StructureNet & gateNet = comp->getArray().findNet(StructRec::StructurePinType("MosfetNormalArray", "Gate"));
			   if(!circuitParameter.isReferenceVoltageNet(gateNet.getIdentifier()))
			   {
				   wantedComp = comp;
			   }
		   }

		   assert(wantedComp != nullptr, "There should be at least on component, which is connected to Vref");
		   return * wantedComp;
	   }
	   else
	   {
		   return **transPartComps.begin();
	   }
   }

   std::vector<Component*> TransconductancePart::findSupplyConnectedComponents(const Result & partitioningResult) const
	{
	   std::vector<Component*> supplyConnectedComps;
	   std::vector<Component *> transPartComps = partitioningResult.getBelongingComponents(*this);

		   for(auto & comp : transPartComps)
		   {
			   const StructRec::StructureNet & sourceNet = comp->getArray().findNet(StructRec::StructurePinType(comp->getArray().getStructureName(), "Source"));
			   if(sourceNet.isSupply())
			   {
				   supplyConnectedComps.push_back(comp);
			   }
		   }

		   assert( !supplyConnectedComps.empty(), "There a no supply connected components");
		   return supplyConnectedComps;
	}

   bool TransconductancePart::isBiasPart() const
   {
	   return false;
   }

   bool TransconductancePart::isLoadPart() const
   {
	   return false;
   }

   bool TransconductancePart::isCapacitancePart() const
   {
   	  return false;
   }

   bool TransconductancePart::isTransconductancePart() const
   {
	   return true;
   }

	bool TransconductancePart::isUndefinedPart() const
	{
		return false;
	}

	bool TransconductancePart::isResistorPart() const
	{
		return false;
	}

	bool TransconductancePart::isCommonModeSignalDetectorPart() const
	{
		return false;
	}

	bool TransconductancePart::isPositiveFeedbackPart() const
	{
		return false;
	}
}



