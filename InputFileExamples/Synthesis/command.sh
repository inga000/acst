#!/bin/sh

{file path to acst}/acst/build/bin/acst.sh --log-level-console DEBUG --analysis synthesis --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml  --device-types-file deviceTypes.xcat --xml-technologie-file TechnologieFile.xml --xml-circuit-information-file CircuitSpecifications.xml --transistor-model SHM --scaling 1mum --HSPICE-netlist-dir {file path to acst}/acst/InputFileExamples/Synthesis/HspiceNetlist

