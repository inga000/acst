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

get_property(HSPICE_SRC GLOBAL PROPERTY SOURCES_HSpice)

add_library(HSpice 
            ${HSPICE_SRC}
            )

target_link_libraries(HSpice 
    Control
    Core
)
### hspice outputfile test
function(hspice_outputfile_test TEST_CIRCUIT)
    SET(TEST_EXECUTABLE HSpice${TEST_CIRCUIT}OutputFileTest)
    SET(TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/${TEST_CIRCUIT})
    SET(TEST_REFERENCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/${TEST_CIRCUIT}/reference.ckt)
    SET(TEST_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/test/${TEST_CIRCUIT}/result.ckt)
    SET(TEST_SCRIPT ${TEST_DIR}/test_ckt.sh)
    
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/test_ckt.sh ${TEST_SCRIPT})
        
    add_dependencies(${TEST_BUILD_TARGET} ${TEST_EXECUTABLE})
    add_test(NAME  ${TEST_EXECUTABLE} COMMAND  /bin/sh ${TEST_SCRIPT})
    
endfunction(hspice_outputfile_test)

#hspice_outputfile_test(AllDevices)
#hspice_outputfile_test(DoubleInverter)
#hspice_outputfile_test(DoubleInverterWithProperties)