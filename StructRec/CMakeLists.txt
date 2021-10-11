#**************************************************************************************************
#  Main authors:
#     Inga Abel <inga.abel@tum.de>, 
#	  Maximilian Neuner <maximilian.neuner@tum.de>, 
#	  Michael Eick <michael.eick@tum.de>
#
# 
#  Copyright (C) 2021
#  Chair of Electronic Design Automation
#  Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
#  TU Muenchen
#  Arcisstrasse 21
#  D-80333 Muenchen
#  Germany
#
#  This file is part of acst, a analog circuit analysis, sizing and synthesis enviroment:
#
#  Permission is hereby granted, free of charge, to any person obtaining
#  a copy of this software and associated documentation files (the
#  "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish,
#  distribute, sublicense, and/or sell copies of the Software, and to
#  permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
#  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#**************************************************************************************************

entering_message()



add_subdirectory(src)

get_property(STRUCTREC_SRC GLOBAL PROPERTY SOURCES_StructRec)

add_library (StructRec 
    ${STRUCTREC_SRC}
)

target_link_libraries(StructRec 
        Control
)

comp_provide_function(ANALYSIS
    LIB StructRec
    CLASS StructRec::StructureRecognitionAnalysisFactory  HEADER StructRec/incl/StructureRecognitionAnalysisFactory.h
)

comp_provide_function(ANALYSIS
    LIB StructRec
    CLASS StructRec::StructureRecognitionRuleGenerationFactory  HEADER StructRec/incl/StructureRecognitionRuleGenerationFactory.h
)


find_package(PythonInterp)

function(structrec_test_xml TEST_CIRCUIT)
    SET(TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${TEST_CIRCUIT})
    SET(TEST_REFERENCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/reference.xml)
    SET(TEST_INPUT_CKT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/input.ckt) 
    SET(TEST_SCRIPT ${TEST_DIR}/testStrucRec.py)
    SET(TEST_ACST_IN_TYPE hspice)
    SET(TEST_RUN_DIR ${TEST_DIR})
    SET(TEST_DEVICE_TYPES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/deviceTypes.acst)
    SET(HSPICE_MAPPING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/HSpiceMapping.acst)
    SET(HSPICE_SUPPLY_NET_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/supplyNets.acst)
    SET(XML_LIB_FILE ${CMAKE_SOURCE_DIR}/StructRec/xml/AnalogLibrary.xml)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/testStructRec.py ${TEST_SCRIPT})
    add_test(NAME STRUCREC_XML_${TEST_CIRCUIT}
        COMMAND ${PYTHON_EXECUTABLE} ${TEST_SCRIPT})
endfunction(structrec_test_xml)

function(rulegen_test_xml TEST_CIRCUIT)
    SET(TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${TEST_CIRCUIT})
    SET(TEST_REFERENCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/reference.xml)
    SET(TEST_INPUT_CKT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/input.ckt) 
    SET(TEST_SCRIPT ${TEST_DIR}/testRuleGen.py)
    SET(TEST_ACST_IN_TYPE hspice)
    SET(TEST_RUN_DIR ${TEST_DIR})
    SET(TEST_DEVICE_TYPES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/deviceTypes.acst)
    SET(HSPICE_MAPPING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/HSpiceMapping.acst)
    SET(HSPICE_SUPPLY_NET_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/supplyNets.acst)
    SET(CKT_DCVS ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/dcvs.ckt)
    SET(CKT_PASSGATE_LC ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/passGateLC.ckt)
    SET(CKT_PASSGATE_LC_KEEPER ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/passGateLCKeeper.ckt)
    SET(CKT_SINGLESUPPLY_LC ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/singleSupplyLC.ckt)
    SET(CKT_SINGLESUPPLY_TVVS ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/inputFiles/singleSupplyTVVS.ckt)
    SET(XML_LIB_FILE_ARRAY ${CMAKE_SOURCE_DIR}/StructRec/test/data/levelConverters/Libraries/ArrayLibrary.xml)
    SET(XML_LIB_FILE_PASSGATE_LC ${CMAKE_SOURCE_DIR}/StructRec/test/data/levelConverters/Libraries/passGateLCLibrary.xml)
    SET(XML_LIB_FILE_SINGLESUPPLY_LC ${CMAKE_SOURCE_DIR}/StructRec/test/data/levelConverters/Libraries/singleSupplyLCLibrary.xml)
    SET(XML_LIB_FILE_LEVEL_CONVERTERS ${CMAKE_SOURCE_DIR}/StructRec/test/data/levelConverters/Libraries/LevelConverterLibrary.xml)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/testRuleGen.py ${TEST_SCRIPT})
    add_test(NAME RULEGEN_XML_${TEST_CIRCUIT}
        COMMAND ${PYTHON_EXECUTABLE} ${TEST_SCRIPT})
endfunction(rulegen_test_xml)


structrec_test_xml(all_current_mirrors)
structrec_test_xml(bicmos_with_array)
structrec_test_xml(cmos_buffer)
structrec_test_xml(cross_coupled)
structrec_test_xml(foca)
structrec_test_xml(miller_with_array)
structrec_test_xml(cascodeSymmetricalOTA)
structrec_test_xml(complementaryOpAmp)
structrec_test_xml(foldedCascodeOpAmpWithGainEnhancer)
structrec_test_xml(foldedCascodeOpAmpWithSlewRateEnhancer)
structrec_test_xml(foldedCascodeOpAmp)
structrec_test_xml(fullyDifferentialOpAmpWithCMFB)
structrec_test_xml(fullyDifferentialOpAmpWithRCCMFB)
structrec_test_xml(fullyDifferentialTwoStageOpAmpWithCMFB)
structrec_test_xml(lowPowerTwoStageOpAmp)
structrec_test_xml(symmetricalOpAmp)
structrec_test_xml(symmetricalOpAmpWithCrossCouplePair)
structrec_test_xml(symmetricalOpAmpWithHighPSRR)
structrec_test_xml(telescopicOpAmp)
structrec_test_xml(threeStageOpAmp)
structrec_test_xml(twoStageFoldedCascodeOpAmp)
structrec_test_xml(twoStageOpAmpWithCascodeSecondStage)
structrec_test_xml(twoStageOpAmpWithOneLoadTransistor)

structrec_test_xml(ds_with_ccm)
structrec_test_xml(ota_dpInThreeInstances)
structrec_test_xml(ccm_twoHL)
structrec_test_xml(hierarchical_ota)
structrec_test_xml(OTA3HL)
structrec_test_xml(twoOTAs)
structrec_test_xml(twoOTAsOnDifferentHL)

rulegen_test_xml(levelConverters)
