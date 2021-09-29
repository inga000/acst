#!/bin/bash
 
export LD_LIBRARY_PATH="{file path to acst}/gecode-release-6.2.0:${LD_LIBRARY_PATH}"
export OA_UNSUPPORTED_PLAT=linux_rhel40
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/acst $@
