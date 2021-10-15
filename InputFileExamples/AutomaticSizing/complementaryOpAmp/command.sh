#!/bin/sh

{file path to acst}/acst/build/bin/acst.sh --log-level-console DEBUG  --circuit-netlist complementaryOpAmp.hspice --output-file complementaryOpAmp.xml --analysis automaticsizing --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml --hspice-mapping-file HSpiceMapping.xcat --hspice-supplynet-file supplyNets.xcat --device-types-file deviceTypes.xcat --xml-technologie-file TechnologyFile.xml --xml-circuit-information-file CircuitParameterAndSpecifications.xml --transistor-model SHM --scaling 1mum --runtime 5

