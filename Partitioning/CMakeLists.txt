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

get_property(PARTITIONING_SRC GLOBAL PROPERTY SOURCES_Partitioning)

add_library (Partitioning 
    ${PARTITIONING_SRC}
)

target_link_libraries(Partitioning 
        Control
)

comp_provide_function(ANALYSIS
    LIB Partitioning
    CLASS Partitioning::PartitioningAnalysisFactory  HEADER Partitioning/incl/PartitioningAnalysisFactory.h
)

find_package(PythonInterp)

function(partitioning_test_xml TEST_CIRCUIT)
    SET(TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${TEST_CIRCUIT})
    SET(TEST_REFERENCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/reference.xml)
    SET(TEST_INPUT_CKT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/input.ckt) 
    SET(TEST_SCRIPT ${TEST_DIR}/test.py)
    SET(TEST_ACST_IN_TYPE hspice)
    SET(TEST_RUN_DIR ${TEST_DIR})
    SET(TEST_DEVICE_TYPES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/deviceTypes.acst)
    SET(TEST_SUPPLY_NETS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/supplyNets.acst)
    SET(HSPICE_MAPPING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/HSpiceMapping.acst)
    SET(XML_LIB_FILE ${CMAKE_SOURCE_DIR}/StructRec/xml/AnalogLibrary.xml)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/testPartitioning.py ${TEST_SCRIPT})
    add_test(NAME PARTITIONING_XML_${TEST_CIRCUIT}
        COMMAND ${PYTHON_EXECUTABLE} ${TEST_SCRIPT})
endfunction(partitioning_test_xml)

partitioning_test_xml(miller)
partitioning_test_xml(foca)
partitioning_test_xml(focaOpAmp)
partitioning_test_xml(cascodeSymmetricalOTA)
partitioning_test_xml(complementaryOpAmp)
partitioning_test_xml(foldedCascodeOpAmpWithGainEnhancer)
partitioning_test_xml(foldedCascodeOpAmpWithSlewRateEnhancer)
partitioning_test_xml(foldedCascodeOpAmp)
partitioning_test_xml(fullyDifferentialOpAmpWithCMFB)
partitioning_test_xml(fullyDifferentialOpAmpWithRCCMFB)
partitioning_test_xml(fullyDifferentialTwoStageOpAmpWithCMFB)
partitioning_test_xml(lowPowerTwoStageOpAmp)
partitioning_test_xml(symmetricalOpAmp)
partitioning_test_xml(symmetricalOpAmpWithCrossCouplePair)
partitioning_test_xml(symmetricalOpAmpWithHighPSRR)
partitioning_test_xml(telescopicOpAmp)
partitioning_test_xml(threeStageOpAmp)
partitioning_test_xml(twoStageFoldedCascodeOpAmp)
partitioning_test_xml(twoStageOpAmpWithCascodeSecondStage)
partitioning_test_xml(twoStageOpAmpWithOneLoadTransistor)

