#*******************************************************************************
#        Copyright (C) 2012
#        Institute for Electronic Design Automation
#        Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
#        TU Muenchen
#        Arcisstrasse 21
#        D-80333 Muenchen
#        Germany
#*******************************************************************************
find_path(CPPUNIT_INCLUDE_DIR cppunit/Test.h)
find_library(CPPUNIT_LIBRARIES cppunit)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CppUnit DEFAULT_MSG CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARIES)
