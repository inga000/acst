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
import sys
from subprocess import check_call
from os import path
from xml.dom import minidom

# ['struc:', (u'MosfetDifferentialPair[1] ', u'n', u'x10'), ['structures:', [['struc:', (u'MosfetNormalArray[22] ', u'n', u'x10'), ['structures:', []], ['pins:', ('pin:', u'Bulk', u'/0'), 
#('pin:', u'Drain', u'/net33'), ('pin:', u'Gate', u'/net31'), ('pin:', u'Source', u'/net32')], ['devices:', ('device', u'/m40', u'Mosfet', u'n', u'x10')]], 
#['struc:', (u'MosfetNormalArray[23] ', u'n', u'x11'), ['structures:', []], 
#['pins:', ('pin:', u'Bulk', u'/0'), ('pin:', u'Drain', u'/net42'), ('pin:', u'Gate', u'/net41'), ('pin:', u'Source', u'/net32')], 
#['devices:', ('device', u'/m41', u'Mosfet', u'n',, u'x11')]]]],
#],...
#
#]


def readStructure(structure):
    strucRes = ["struc:"]
    
    name = structure.getAttribute("name")     
    techType = structure.getAttribute("techType")
    instance = structure.getAttribute("instance")
    
    name = name.rsplit('[',1)[0]
    
    if name != "":
        strucRes.append((name,techType, instance))
        
    structures = ["structures:"]
    structuresNodesList = getNodes(structure, "structure")

    subStrucs = []
    for struc in structuresNodesList:
        childStrucs = readStructure(struc) # recursion
        subStrucs.append(childStrucs)
    structures.append(subStrucs)
    strucRes.append(structures)
        
    pins = ["pins:"]
    pinsNode = requestFirstNode(structure, "pins")
    if pinsNode:
        pinNodesList = getNodes(pinsNode, "pin")
        for pin in pinNodesList:
            name = pin.getAttribute("name")
            net = pin.getAttribute("net")
            pins.append(("pin:",name,net))
    strucRes.append(pins) 

    devices = ["devices:"]
    devicesNode = requestFirstNode(structure,"devices")
    if devicesNode:
        devicesNodeList = getNodes(devicesNode, "device")
        for device in devicesNodeList:
            name = device.getAttribute("name")
            deviceType = device.getAttribute("deviceType")
            techType = device.getAttribute("techType")   
            instance = device.getAttribute("instance")
            devices.append(("device",name,deviceType,techType,instance))
    strucRes.append(devices)

    return strucRes

def requestFirstNode(node, name):
    currNode = node.firstChild
    while currNode:
        if (currNode.localName == name):
            return currNode
        currNode = currNode.nextSibling
    return None

def getSingleNode(node, name):
    currNode = node.firstChild
    i = 0
    while currNode:
        if (currNode.localName == name):
            if i == 0:
                i += 1
                retVal = currNode
        currNode = currNode.nextSibling
    if i == 1:
        return retVal
    elif i > 1:
        print("Error: Multiple <%s> tags.".format(name))
        exit(-1)
    elif i == 0:
        print("Error: <%s> tag not found".format(name))
        exit(-1)
    
def getNodes(node, name):
    currNode = node.firstChild
    retVal = []
    while currNode:
        if (currNode.localName == name):
            retVal.append(currNode)
        currNode = currNode.nextSibling
    return retVal
            


def readCircuit(filename):
    xml = minidom.parse(filename)
    circuitList = xml.documentElement.getElementsByTagName("structure_recognition_results")
    if(len(circuitList) != 1):
        print("Error: Multiple <structure_recognition_results> tags.")
        exit(-1)
    
    
    
    structureNodesList = getNodes(circuitList[0], "structure")
    
    circuit = []
    for struc in structureNodesList:
        strucRes = readStructure(struc)
        circuit.append(strucRes)
    
    return circuit

def compareNestedLists(listA, listB):
    for a,b in zip(listA,listB):
        if type(a) != type(b):
            print("compare failed:")
            print("a:", a)
            print("b:", b)
            return False
        
        if type([]) == type(a):
            equal = compareNestedLists(a,b) #recursion
            if equal == False:
                return False
        if a != b:
            print("compare failed:")
            print("a:", a)
            print("b:", b)
            return False
    return True
            

def compareStructure(strucA, strucB):
    # index 0 is the "struc:" string
    if(strucA[0] != "struc:" or strucB[0] != "struc:"):
        exit(-1)
    
    #index 1 is the attribute tuple
    if (strucA[1] != strucB[1]): 
        return False
    
    #index 2 is the structures list ["structures:", ["struc:",...], ...]
    structuresA = strucA[2]
    structuresB = strucB[2]
    
    if(len(structuresA)==0 or len(structuresB)==0):
        print("ERROR: Unexpected format for structures. Empty structures. Compare failed.")
        exit(-1)
    
    if(structuresA[0] != "structures:" or structuresB[0] != "structures:"):
        print("ERROR: Unexpected format for structures. Compare failed.")
        print("structuresA:%s".format(structuresA))
        print("structuresB:%s".format(structuresB))
        exit(-1)
        
    if( len(structuresA) != len(structuresB)):
        print("Length Mismatch")
        print("strucA",strucA[1])
        print("strucB",strucB[1])
        return False
        
    if len(structuresA) == 2 and len(structuresB) == 2:
        for a in structuresA[1]:
            i = 0
            for b in structuresB[1]:
                if compareStructure(a,b) == True:
                    i += 1
            if i != 1:
                print("Structure Mismatch:")
                print("a",a)
                print("structuresB",structuresB[1])
                return False
        for b in structuresB[1]:
            i = 0
            for a in structuresA[1]:
                if compareStructure(a,b) == True:
                    i += 1
            if i != 1:
                print("Structure Mismatch:")
                print("b",b)
                print("structuresA",structuresA[1])
                return False
        
    #index 3 is the pins list ["pins:", ("pin:", ...), ...]
    pinsA = strucA[3] 
    pinsB = strucB[3]
    if(pinsA[0] != "pins:" or pinsB[0] != "pins:"):
        print("ERROR: Unexpected format for pins. Compare failed.")
        print(pinsA)
        print(pinsB)
        exit(-1)
    
    if len(pinsA) != len(pinsB):
        return False
        
    if len(pinsA) > 1:
        for a in pinsA[1:]:
            i = 0
            for b in pinsB[1:]:
                if a == b: i+=1
            if i!=1:
                print("Pin Mismatch:")
                print("a",a)
                print("pinsB",pinsB[1:])
                return False
        for b in pinsB[1:]:
            i = 0
            for a in pinsA[1:]:
                if a == b: i+=1
            if i!=1:
                print("Pin Mismatch:")
                print("b",b)
                print("pinsA",pinsA[1:])
                return False
      
    #index 4 is the devices list ["devices:", ("device:", ...), ...]
    devicesA = strucA[4] 
    devicesB = strucB[4]
    if(devicesA[0] != "devices:" or devicesB[0] != "devices:"):
        print("ERROR: Unexpected format for devices. Compare failed.")
        print(devicesA)
        print(devicesB)
        exit(-1)
    
    if len(devicesA) != len(devicesB):
        return False
        
    if len(devicesA) > 1:
        for a in devicesA[1:]:
            i = 0
            for b in devicesB[1:]:
                if a == b: i+=1
            if i!=1:
                print("Device Mismatch:")
                print("a",a)
                print("devicesB",devicesB[1:])
                return False
        for b in devicesB[1:]:
            i = 0
            for a in devicesA[1:]:
                if a == b: i+=1
            if i!=1:
                print("Device Mismatch:")
                print("b",b)
                print("devicesA",devicesA[1:])
                return False

    return True
        
    
    
    
    
def compareResults(resA, resB):
    for a in resA:
        i = 0
        for b in resB:
            if compareStructure(a,b) == True:
                i += 1
        if i != 1:
            print("Toplevel Structure Mismatch.")
            print("b:", [i[0:2] for i in resB])
            print("a:", a[0:2])
            return False
        
    for b in resB:
        i = 0
        for a in resA:
            if compareStructure(a,b) == True:
                i += 1
        if i != 1:
            print("Toplevel Structure Mismatch.")
            print("a:", [i[0:2] for i in resA])
            print("b:", b[0:2])
            return False
    return True
                

#A = readCircuit("/tmp/testA.xml")
#B = readCircuit("/tmp/testB.xml")
#
#print A
#equal = compareResults(A,B)
#if not equal:
#    print "Test failed."
#    exit(-1)
#else:
#    print "Test passed."
#    exit(0)

tmpDir = "${TEST_DIR}"

acst = "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BINARY_NAME}.sh"
circuit = "${TEST_INPUT_CKT_FILE}"
reference = "${TEST_REFERENCE_FILE}"
outFile = path.join(tmpDir,"result.xml")

cktDcvs = "${CKT_DCVS}"
cktPassGateLc = "${CKT_PASSGATE_LC}"
cktPassGateLcKeeper = "${CKT_PASSGATE_LC_KEEPER}"
cktSingleSupplyLC = "${CKT_SINGLESUPPLY_LC}"
cktSingleSupplyTVVS = "${CKT_SINGLESUPPLY_TVVS}"

xmlLibFileArray = "${XML_LIB_FILE_ARRAY}"
xmlLibFilePassGateLC = "${XML_LIB_FILE_PASSGATE_LC}"
xmlLibFileSingleSupplyLC = "${XML_LIB_FILE_SINGLESUPPLY_LC}"
xmlLibFileLevelConverters = "${XML_LIB_FILE_LEVEL_CONVERTERS}"

inType = "${TEST_ACST_IN_TYPE}"
runDir = "${TEST_RUN_DIR}"
deviceTypesFile = "${TEST_DEVICE_TYPES_FILE}"
hspiceMappingFile = "${HSPICE_MAPPING_FILE}"
hspiceSupplyNetFile = "${HSPICE_SUPPLY_NET_FILE}"

if len("${EXTRA_PARAMS}") != 0: 
    extraParams="${EXTRA_PARAMS}".split(" ")
else:
    extraParams=[]
    
cmdLineDcvs = [acst,"--circuit-netlist", cktDcvs, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile,"--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "rulegen", "--structure-name", "dcvs", "--xml-structrec-library-file", xmlLibFileArray, "--log-level-console","OFF"] 
cmdLinePassGateLC  = [acst,"--circuit-netlist", cktPassGateLc, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile, "--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "rulegen", "--structure-name", "passGateLC", "--xml-structrec-library-file", xmlLibFileArray, "--log-level-console","OFF"] 
cmdLinePassGateLCKeeper = [acst,"--circuit-netlist", cktPassGateLcKeeper, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile, "--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "rulegen", "--structure-name", "passGateLCKeeper", "--xml-structrec-library-file", xmlLibFilePassGateLC, "--log-level-console","OFF"] 
cmdLineSingleSupplyLC =  [acst,"--circuit-netlist", cktSingleSupplyLC, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile, "--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "rulegen", "--structure-name", "singleSupplyLC", "--xml-structrec-library-file", xmlLibFileArray, "--log-level-console","DEBUG"] 
cmdLineSingleSupplyTVVS = [acst, "--circuit-netlist", cktSingleSupplyTVVS, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile, "--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "rulegen", "--structure-name", "singleSupplyTVVS", "--xml-structrec-library-file", xmlLibFileSingleSupplyLC, "--log-level-console","OFF"]
cmdLineAllConverters = [acst, "--circuit-netlist", circuit, "--device-types-file", deviceTypesFile, "--hspice-mapping-file", hspiceMappingFile, "--hspice-supplynet-file", hspiceSupplyNetFile, "--analysis", "structrec", "--output-format", "xml","--output-file", outFile, "--xml-structrec-library-file", xmlLibFileLevelConverters, "--log-level-console","OFF"]


try:
    print("Executing '"+" ".join(cmdLineDcvs)+"'")
    check_call(cmdLineDcvs,cwd=runDir)
    
    print("Executing '"+" ".join(cmdLinePassGateLC)+"'")
    check_call(cmdLinePassGateLC, cwd=runDir)
    
    print("Executing '"+" ".join(cmdLinePassGateLCKeeper)+"'")
    check_call(cmdLinePassGateLCKeeper, cwd=runDir)
    
    print("Executing '"+" ".join(cmdLineSingleSupplyLC)+"'")
    check_call(cmdLineSingleSupplyLC, cwd=runDir)
    
    print("Executing '"+" ".join(cmdLineSingleSupplyTVVS)+"'")
    check_call(cmdLineSingleSupplyTVVS, cwd=runDir)
    
    print("Executing '"+" ".join(cmdLineAllConverters)+"'")
    check_call(cmdLineAllConverters, cwd=runDir)    
except:
    print("Executing binary failed")
    exit(-1)
    

ref = readCircuit(reference)
print("Reference:")
print(ref)

res = readCircuit(outFile)
print("Result:")
print(res)

#equal = compareNestedLists(ref,res)
equal = compareResults(ref,res)
if not equal:
    print("Test failed.")
    exit(-1)
else:
    print("Test passed.")
    exit(0)
