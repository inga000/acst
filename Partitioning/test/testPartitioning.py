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

def readPartitioningResult(filename):
    xml = minidom.parse(filename)
    partitioningList = xml.documentElement.getElementsByTagName("circuit_partitioning_results")
    if(len(partitioningList) != 1):
        print("Error: Multiple <circuit_partitioning_results> tags.")
        exit(-1)
    
    gmParts = readGmParts(partitioningList[0])
    loadParts = readLoadParts(partitioningList[0])
    biasParts = readBiasParts(partitioningList[0])
    undefinedParts = readUndefinedParts(partitioningList[0])
    capacitances = readCapacitances(partitioningList[0])
    resistorParts = readResistorParts(partitioningList[0])
    commonModeSignalDetectorParts = readCommonModeSignalDetectorParts(partitioningList[0])
    positiveFeedbackParts = readPositiveFeedbackParts(partitioningList[0])
            
    result = {"gmParts" : gmParts, "loadParts" : loadParts, "biasParts" : biasParts, "undefinedParts" : undefinedParts, "capacitances" : capacitances, "resistorParts" : resistorParts, "commonModeSignalDetectorParts" : commonModeSignalDetectorParts, "positiveFeedbackParts" : positiveFeedbackParts}
    
    return result

def readGmParts(resultsNode):
    gmPartsNodeList = getNodes(resultsNode, "gmParts")
    if len(gmPartsNodeList) > 1:
        print("Error: Multiple <gmParts> tags.")
        exit(-1)

    if len(gmPartsNodeList) == 1:
    	gmParts = []
    	gmPartNodeList = getNodes(gmPartsNodeList[0], "gmPart")
    	for gmPartNode in gmPartNodeList:
            typeAttribute = gmPartNode.getAttribute("type")
            firstStageTypeAttribute = gmPartNode.getAttribute("firstStageType")
        
    	    structures = []
            structureNodeList = getNodes(gmPartNode, "structure")
            for structureNode in structureNodeList:            
                structure = readStructure(structureNode)
                structures.append(structure)

            gmPart = {"type" : typeAttribute, "firstStageType" : firstStageTypeAttribute, "structure" : structures}
            gmParts.append(gmPart)

        return gmParts 

def readLoadParts(resultsNode):
    loadPartsNodeList = getNodes(resultsNode, "loadParts")
    if len(loadPartsNodeList) > 1:
        print("Error: Multiple <loadParts> tags.")
        exit(-1)

    if len(loadPartsNodeList) == 1:
    	loadParts = []
    	loadPartNodeList = getNodes(loadPartsNodeList[0], "loadPart")
    	for loadPartNode in loadPartNodeList:		
    	    structureNodeList = getNodes(loadPartNode, "structure")

    	    for structureNode in structureNodeList:            
    	        loadPart = readStructure(structureNode)
    	        loadParts.append(loadPart)

        return loadParts 

def readBiasParts(resultsNode):
    biasPartsNodeList = getNodes(resultsNode, "biasParts")
    if len(biasPartsNodeList) > 1:
        print("Error: Multiple <biasParts> tags.")
        exit(-1)

    if len(biasPartsNodeList) == 1:
    	biasParts = []
    	biasPartNodeList = getNodes(biasPartsNodeList[0], "biasPart")
    	for biasPartNode in biasPartNodeList:		
    	    structureNodeList = getNodes(biasPartNode, "structure")
        
    	    for structureNode in structureNodeList:            
    	        biasPart = readStructure(structureNode)
    	        biasParts.append(biasPart)

        return biasParts 

def readCapacitances(resultsNode):
    capacitancesNodeList = getNodes(resultsNode, "capacitances")
    if len(capacitancesNodeList) > 1:
        print("Error: Multiple <capacitances> tags.")
        exit(-1)

    if len(capacitancesNodeList) == 1:
    	capacitances = []
    	capacitanceNodeList = getNodes(capacitancesNodeList[0], "capacitance")
    	for capacitanceNode in capacitanceNodeList:
            typeAttribute = capacitanceNode.getAttribute("type")

            structureNodeList = getNodes(capacitanceNode, "structure")
            if len(structureNodeList) != 1:
        	print("Error: Several <structure> tags.")
        	exit(-1)
            structure = readStructure(structureNodeList[0])

            capacitance = {"type" : typeAttribute, "structure" : structure}
            capacitances.append(capacitance)

    	return capacitances 

def readUndefinedParts(resultsNode):
    undefinedPartsNodeList = getNodes(resultsNode, "undefinedParts")
    if len(undefinedPartsNodeList) > 1:
        print("Error: Multiple <undefinedParts> tags.")
        exit(-1)

    if len(undefinedPartsNodeList) == 1:
    	undefinedParts = []
    	undefinedPartNodeList = getNodes(undefinedPartsNodeList[0], "undefinedPart")
    	for undefinedPartNode in undefinedPartNodeList:		
    	    structureNodeList = getNodes(undefinedPartNode, "structure")
    	    if len(structureNodeList) != 1:
    	        print("Error: Several <structure> tags.")
    	        exit(-1)

    	    undefinedPart = readStructure(structureNodeList[0])
    	    undefinedParts.append(undefinedPart)

    	return undefinedParts 

def readResistorParts(resultsNode):
    resistorPartsNodeList = getNodes(resultsNode, "resistorParts")
    if len(resistorPartsNodeList) > 1:
        print("Error: Multiple <resistorParts> tags.")
        exit(-1)

    if len(resistorPartsNodeList) == 1:
    	resistorParts = []
    	resistorPartNodeList = getNodes(resistorPartsNodeList[0], "resistorPart")
    	for resistorPartNode in resistorPartNodeList:		
    	    structureNodeList = getNodes(undefinedPartNode, "structure")
    	    if len(structureNodeList) != 1:
    	        print("Error: Several <structure> tags.")
    	        exit(-1)

    	    resistorPart = readStructure(structureNodeList[0])
    	    resistorParts.append(resistorPart)

    	return resistorParts

def readCommonModeSignalDetectorParts(resultsNode):
    commonModeSignalDetectorPartsNodeList = getNodes(resultsNode, "commonModeSignalDetectorParts")
    if len(commonModeSignalDetectorPartsNodeList) > 1:
        print("Error: Multiple <commonModeSignalDetectorParts> tags.")
        exit(-1)

    if len(commonModeSignalDetectorPartsNodeList) == 1:
    	commonModeSignalDetectorParts = []
    	commonModeSignalDetectorPartNodeList = getNodes(commonModeSignalDetectorPartsNodeList[0], "commonModeSignalDetectorPart")
    	for commonModeSignalDetectorPartNode in commonModeSignalDetectorPartNodeList:		
    	    structureNodeList = getNodes(undefinedPartNode, "structure")
    	    if len(structureNodeList) != 1:
    	        print("Error: Several <structure> tags.")
    	        exit(-1)

    	    commonModeSignalDetectorPart = readStructure(structureNodeList[0])
    	    commonModeSignalDetectorParts.append(commonModeSignalDetectorPart)

    	return commonModeSignalDetectorParts

def readPositiveFeedbackParts(resultsNode):
    positiveFeedbackPartsNodeList = getNodes(resultsNode, "positiveFeedbackParts")
    if len(positiveFeedbackPartsNodeList) > 1:
        print("Error: Multiple <undefinedParts> tags.")
        exit(-1)

    if len(positiveFeedbackPartsNodeList) == 1:
    	positiveFeedbackParts = []
    	positiveFeedbackPartNodeList = getNodes(positiveFeedbackPartsNodeList[0], "positiveFeedbackPart")
    	for positiveFeedbackPartNode in positiveFeedbackPartNodeList:		
    	    structureNodeList = getNodes(positiveFeedbackPartNode, "structure")

    	    for structureNode in structureNodeList:            
    	        positiveFeedbackPart = readStructure(structureNode)
    	        positiveFeedbackParts.append(positiveFeedbackPart)

    	return positiveFeedbackParts 

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
        print("structuresA:%s"%structuresA)
        print("structuresB:%s"%structuresB)
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
    
    equalGmParts = compareGmParts(resA, resB)
    print("Equal gmParts: ", equalGmParts)
    
    equalLoadParts = compareLoadParts(resA, resB)
    print("Equal loadParts: ", equalLoadParts)
    
    equalBiasParts = compareBiasParts(resA, resB)
    print("Equal biasParts: ", equalBiasParts)

    equalUndefinedParts = compareUndefinedParts(resA, resB)
    print("Equal undefinedParts: ", equalUndefinedParts)

    equalCapacitances = compareCapacitances(resA, resB)
    print("Equal capacitances: ", equalCapacitances)

    equalResistorParts = compareResistorParts(resA, resB)
    print("Equal resistorParts: ", equalResistorParts)

    equalCommonModeSignalDetectorParts = compareCommonModeSignalDetectorParts(resA, resB)
    print("Equal commonModeSignalDetectorParts: ", equalCommonModeSignalDetectorParts)

    equalPositiveFeedbackParts = comparePositiveFeedbackParts(resA, resB)
    print("Equal positiveFeedbackParts: ", equalPositiveFeedbackParts)

    return equalGmParts and equalLoadParts and equalBiasParts and equalUndefinedParts and equalCapacitances and equalResistorParts and equalCommonModeSignalDetectorParts and equalPositiveFeedbackParts
                
def compareGmParts(resA, resB):
    gmPartsA = resA["gmParts"]
    gmPartsB = resB["gmParts"]
    print("Compare gm parts")
   
    equalGmParts = True
    for partA in gmPartsA:
        typeA = partA["type"]
        firstStageTypeA = partA["firstStageType"]
        strucA = partA["structure"]
        
        hasEqualPart = False
        index = 1
        for partB in gmPartsB:
            typeB = partB["type"]
            firstStageTypeB = partB["firstStageType"]
            strucB = partB["structure"]
            
            if (typeA == typeB) and (firstStageTypeA == firstStageTypeB):
                print("<<<<<< Compare structures of gm part type ", typeA, " Round: ", index, " >>>>>>>")
                index += 1
                if compareStructures(strucA, strucB):
                    hasEqualPart = True
                    print("Equal structures found")
                    break

        if not hasEqualPart:
            equalGmParts = False
            break
    
    return equalGmParts

def compareStructures(strucsA, strucsB):    
    if(len(strucsA) != len(strucsB)):
        return False;
        
    equalStructures = True
    for strucA in strucsA:
        hasEqualStructure = False
        for strucB in strucsB:
            if compareStructure(strucA, strucB):
                hasEqualStructure = True
                break

                
        if not hasEqualStructure:
            equalStructures = False;
            print("No equal Structures found!")
            print("Structure A:")
            print(strucsA)
            print("Structure B:")
            print(strucsB)
            break 

    return equalStructures

def compareCapacitances(resA, resB):
    capsA = resA["capacitances"]
    capsB = resB["capacitances"]
    
    equalCapacitances = True
    for capA in capsA:
        typeA = capA["type"]
        strucA = capA["structure"]
        
        hasEqualPart = False
        for capB in capsB:
            typeB = capB["type"]
            strucB = capB["structure"]
            
            if (typeA == typeB) and compareStructure(strucA, strucB):
                hasEqualPart = True
                break
        
        if not hasEqualPart:
            equalCapacitances = False
            break
    
    return equalCapacitances
    
def compareLoadParts(resA, resB):
    loadPartsA = resA["loadParts"]
    loadPartsB = resB["loadParts"]
    print("Compare load parts structures")
    return compareStructures(loadPartsA, loadPartsB)

def compareBiasParts(resA, resB):
    biasPartsA = resA["biasParts"]
    biasPartsB = resB["biasParts"]
    print("Compare bias parts structures")
    return compareStructures(biasPartsA, biasPartsB)
    
def compareUndefinedParts(resA, resB):
    undefinedPartsA = resA["undefinedParts"]
    undefinedPartsB = resB["undefinedParts"]
    print("Compare undefined parts structures")
    return compareStructures(undefinedPartsA, undefinedPartsB)
   
def compareResistorParts(resA, resB):
    resistorPartsA = resA["resistorParts"]
    resistorPartsB = resB["resistorParts"]
    print("Compare resistor parts structures")
    return compareStructures(resistorPartsA, resistorPartsB)

def compareCommonModeSignalDetectorParts(resA, resB):
    commonModeSignalDetectorPartsA = resA["commonModeSignalDetectorParts"]
    commonModeSignalDetectorPartsB = resB["commonModeSignalDetectorParts"]
    print("Compare common mode signal detector parts structures")
    return compareStructures(commonModeSignalDetectorPartsA, commonModeSignalDetectorPartsB)

def comparePositiveFeedbackParts(resA, resB):
    positiveFeedbackPartsA = resA["undefinedParts"]
    positiveFeedbackPartsB = resB["undefinedParts"]
    print("Compare positive feedback parts structures")
    return compareStructures(positiveFeedbackPartsA, positiveFeedbackPartsB)
    
    
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


circuit = "${TEST_INPUT_CKT_FILE}"
reference = "${TEST_REFERENCE_FILE}"
acst = "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BINARY_NAME}.sh"
tmpDir = "${TEST_DIR}"
outFile = path.join(tmpDir,"result.xml")
inType = "${TEST_ACST_IN_TYPE}"
runDir = "${TEST_RUN_DIR}"
deviceTypesFile = "${TEST_DEVICE_TYPES_FILE}"
supplyNetsFile = "${TEST_SUPPLY_NETS_FILE}"
HSpiceMappingFile = "${HSPICE_MAPPING_FILE}"
xmlLibFile = "${XML_LIB_FILE}"

if len("${EXTRA_PARAMS}") != 0: 
    extraParams="${EXTRA_PARAMS}".split(" ")
else:
    extraParams=[]
    
cmdLine = [acst, "--circuit-netlist",circuit,"--device-types-file",deviceTypesFile,"--hspice-supplynet-file",supplyNetsFile,"--hspice-mapping-file",HSpiceMappingFile,"--analysis", "partitioning"]+extraParams+["--output-format", "xml","--output-file",outFile,"--xml-structrec-library-file",xmlLibFile,"--log-level-console","OFF"]

print("Executing '"+" ".join(cmdLine)+"'")

try:
    check_call(cmdLine,cwd=runDir)
except:
    print("Executing binary failed")
    exit(-1)
    

ref = readPartitioningResult(reference)
print("Reference:")
print(ref)

res = readPartitioningResult(outFile)
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
