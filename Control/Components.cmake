#*******************************************************************************
#        Copyright (C) 2012
#        Institute for Electronic Design Automation
#        Univ.-Prof. Dr.-Ing. Ulf Schlichtmann
#        TU Muenchen
#        Arcisstrasse 21
#        D-80333 Muenchen
#        Germany
#*******************************************************************************
comp_get_libraries(ANALYSIS_LIBS ANALYSIS)

add_library (ManagedComponents
    Control/src/FactoryMap/AnalysesFactoryMap.cpp   
)
target_link_libraries(ManagedComponents ${ANALYSIS_LIBS})

comp_function_generate_list_file(
    FUNCTION ANALYSIS
    MACRO REGISTER_ALL_ANALYSISFACTORIES
    FILE ${CMAKE_BINARY_DIR}/Control/src/AnalysesFactoryMap.gen
)