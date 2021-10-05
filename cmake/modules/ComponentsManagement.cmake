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

include(CheckIncludeFileCXX)

function(comp_def_function name)
    define_property(GLOBAL PROPERTY COMP_FUNC_${name}_HEADERS BRIEF_DOCS "Headers for function ${name}" 
                FULL_DOCS "Lists all header names providing a function of type ${name}.")
    define_property(GLOBAL PROPERTY COMP_FUNC_${name}_LIBS BRIEF_DOCS "Libraries for function ${name}" 
                FULL_DOCS "Lists all library names providing a function of type ${name}.")
    define_property(GLOBAL PROPERTY COMP_FUNC_${name}_CLASSES BRIEF_DOCS "Classes for function ${name}" 
                FULL_DOCS "Lists all class names providing a function of type ${name}.")
    define_property(GLOBAL PROPERTY COMP_FUNC_${name}_RO BRIEF_DOCS "Read only flag for function ${name}" 
                FULL_DOCS "Indicates weahter functions of type ${name} are read only.")
    set_property(GLOBAL PROPERTY COMP_FUNC_${name}_RO FALSE)
endfunction(comp_def_function)

function(comp_provide_function NAME)
    set(LAST_ITEM "VALUE")
    set(HEADERS "")
    set(LIBS "")
    set(CLASSES "")
    foreach(i IN LISTS ARGN)
        if(${LAST_ITEM} STREQUAL "VALUE")
            # we expect a symbol now
            set(THIS_ITEM ${i})
        elseif(${LAST_ITEM} STREQUAL "CLASS")
            list(APPEND CLASSES ${i})
            set(THIS_ITEM "VALUE")
        elseif(${LAST_ITEM} STREQUAL "HEADER")
            list(APPEND HEADERS ${i})
            set(THIS_ITEM "VALUE")
            find_file(THIS_HEADER NAMES ${i} PATHS ${CMAKE_SOURCE_DIR})
            if(NOT THIS_HEADER)
                message(FATAL_ERROR "Header file ${i} not found!")
            endif()
        elseif(${LAST_ITEM} STREQUAL "LIB")
            list(APPEND LIBS ${i})
            set(THIS_ITEM "VALUE")
        else()
            message(FATAL_ERROR "${LAST_ITEM} is no valid argument")
        endif()
        set(LAST_ITEM ${THIS_ITEM})
    endforeach(i)
    if(NOT (${LAST_ITEM} STREQUAL "VALUE"))
        message(FATAL_ERROR "Value missing for ${LAST_ITEM}.")
    endif()
    
    get_property(NAME_VALID GLOBAL PROPERTY COMP_FUNC_${NAME}_RO DEFINED)
    if(NOT NAME_VALID)
        message(FATAL_ERROR "No function ${NAME} defined!")
    endif()
    get_property(READ_ONLY GLOBAL PROPERTY COMP_FUNC_${NAME}_RO)
    if(READ_ONLY)
        message(FATAL_ERROR "Function ${NAME} is read only at this point!")
    endif()
    
    set_property(GLOBAL APPEND PROPERTY COMP_FUNC_${NAME}_HEADERS ${HEADERS})
    set_property(GLOBAL APPEND PROPERTY COMP_FUNC_${NAME}_LIBS ${LIBS})
    set_property(GLOBAL APPEND PROPERTY COMP_FUNC_${NAME}_CLASSES ${CLASSES})
    
    message("Extended function ${NAME} by")
    message("  libs:    ${LIBS}")
    message("  headers: ${HEADERS}")
    message("  class:   ${CLASSES}")
endfunction(comp_provide_function)

function(comp_get_libraries var name)
    get_property(LIBS_EXISTS GLOBAL PROPERTY COMP_FUNC_${name}_LIBS DEFINED)
    if(NOT LIBS_EXISTS)
        message(FATAL_ERROR "No function named ${name}!")
    endif()
    get_property(LIBS GLOBAL PROPERTY COMP_FUNC_${name}_LIBS)
    set(${var} ${LIBS} PARENT_SCOPE)
    set_property(GLOBAL PROPERTY COMP_FUNC_${name}_RO TRUE)
endfunction()

function(comp_function_generate_list_file)
    cmake_parse_arguments(ARGS "" "FILE;FUNCTION;MACRO" "" ${ARGN})
    #parse_arguments(ARGS "FILE;FUNCTION;MACRO" "" ${ARGN})
    
    set(OUTPUT "// Automatic generated file for function ${ARGS_FUNCTION}\n")
    get_property(HEADER GLOBAL PROPERTY COMP_FUNC_${ARGS_FUNCTION}_HEADERS)
    foreach(i IN LISTS HEADER)
        set(OUTPUT "${OUTPUT}#include \"${i}\"\n")
    endforeach()
    set(OUTPUT "${OUTPUT}#define ${ARGS_MACRO}()\\\n")
    get_property(CLASSES GLOBAL PROPERTY COMP_FUNC_${ARGS_FUNCTION}_CLASSES)
    foreach(i IN LISTS CLASSES)
        set(OUTPUT "${OUTPUT}  reg<${i}>()\;\\\n")
    endforeach()
    set(OUTPUT "${OUTPUT}\n")
    file(WRITE ${ARGS_FILE} ${OUTPUT})
    message("List file ${ARGS_FILE} generated.")
    set_property(GLOBAL PROPERTY COMP_FUNC_${ARGS_FUNCTION}_RO TRUE)
endfunction(comp_function_generate_list_file)

