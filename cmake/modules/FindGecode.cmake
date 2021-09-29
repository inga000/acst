#*******************************************************************************
#        Copyright (C) 2012
#        Institute for Electronic Design Automation
#        Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
#        TU Muenchen
#        Arcisstrasse 21
#        D-80333 Muenchen
#        Germany
#*******************************************************************************
if(NOT GECODE_FEATURES)
    set(GECODE_FEATURES "driver" "flatzinc" "gist" "graph" "int" "iter" "kernel" "minimodel" "scheduling" "search" "set" "support")
endif()


set(GECODE_INCLUDE {file path to gecode}/gecode-release-6.2.0)
if(GECODE_INCLUDE STREQUAL "GECODE_INCLUDE-NOTFOUND")
    message("ERROR: Gecode include directory not found!")
else()
    message("GECODE_INCLUDE: ${GECODE_INCLUDE}")
endif()

foreach(FEATURE ${GECODE_FEATURES})
    if(NOT EXISTS ${GECODE_INCLUDE}/gecode/${FEATURE}.hh)
        set(MESSAGE "Could not find ${GECODE_INCLUDE}/gecode/${FEATURE}.hh")
        set(GECODE_INCLUDE "")
        break()
    endif()
endforeach(FEATURE)

set(GECODE_LIBS "")
foreach(FEATURE ${GECODE_FEATURES})
    list(APPEND GECODE_LIBS ${GECODE_INCLUDE}/libgecode${FEATURE}.so)
endforeach(FEATURE)

message(${GECODE_LIBS})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Gecode ${MESSAGE} GECODE_INCLUDE GECODE_LIBS)
