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
#  This file is part of acst, an analog circuit analysis, sizing and synthesis environment:
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

if(NOT GECODE_FEATURES)
    set(GECODE_FEATURES "driver" "flatzinc" "gist" "graph" "int" "iter" "kernel" "minimodel" "scheduling" "search" "set" "support")
endif()


set(GECODE_INCLUDE {file path to gecode directory, eg., usr/local/public/include/gecode-release-6.2.0})
if(GECODE_INCLUDE STREQUAL "GECODE_INCLUDE-NOTFOUND")
    message("ERROR: Gecode include directory not found!")
else()
    message("GECODE_INCLUDE: ${GECODE_INCLUDE}")
endif()

foreach(FEATURE ${GECODE_FEATURES})
    if(NOT EXISTS ${GECODE_INCLUDE}/gecode/${FEATURE}.hh)
        set(MESSAGE "Could not find ${GECODE_INCLUDE}/gecode/${FEATURE}.hh") ##Please check if this file path points to your gecode header files
        set(GECODE_INCLUDE "")
        break()
    endif()
endforeach(FEATURE)

set(GECODE_LIBS "")
foreach(FEATURE ${GECODE_FEATURES})
    list(APPEND GECODE_LIBS ${GECODE_INCLUDE}/libgecode${FEATURE}.so) ##Please check if this file path points to your gecode library files
endforeach(FEATURE)

message(${GECODE_LIBS})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Gecode ${MESSAGE} GECODE_INCLUDE GECODE_LIBS)
