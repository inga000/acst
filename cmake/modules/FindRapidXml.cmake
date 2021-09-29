#*******************************************************************************
#        Copyright (C) 2012
#        Institute for Electronic Design Automation
#        Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
#        TU Muenchen
#        Arcisstrasse 21
#        D-80333 Muenchen
#        Germany
#*******************************************************************************
find_path(RAPIDXML_INCLUDE_DIR rapidxml.hpp)

if(RAPIDXML_INCLUDE_DIR STREQUAL "RAPIDXML_INCLUDE_DIR-NOTFOUND")
    message(FATAL_ERROR "Rapidxml NOT found.")
else()
    message("Rapidxml found.")
    message("  includes: ${RAPIDXML_INCLUDE_DIR}")
endif()

