#!/bin/sh

{file path to acst}/acst/build/bin/acst.sh --log-level-console DEBUG --analysis toplibgen --xml-structrec-library-file {file path to acst}/acst/StructRec/xml/AnalogLibrary.xml  --device-types-file deviceTypes.xcat --HSPICE-netlist-dir {file path to acst}/acst/InputFileExamples/TopologyLibraryGeneration/NetlistsWithLabels
