#!/bin/bash

#TEST_CMD='${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_EXECUTABLE} ${TEST_OUTPUT_FILE}'
TEST_CMD='${TEST_BINARY} --input-format hspice --input-file ${TEST_CIRCUIT_INPUTFILE} --device-types-file ${TEST_DEVICE_TYPES_FILE} --hspice-mapping-file ${TEST_HSPICEMAPPING_FILE} --hspice-supplynet-file ${TEST_HSPICE_SUPPLYNET_FILE} --analysis file-format-converter --circuit-type flat --output-format hspice --output-file ${TEST_OUTPUT_FILE}'

rm -f ${TEST_OUTPUT_FILE}
echo "executing: $TEST_CMD"
$TEST_CMD
cat ${TEST_OUTPUT_FILE} | diff -b - ${TEST_REFERENCE_FILE}
