#include <iostream>

#include "egDataNodesContainer.h"

void EgDataNodesContainerType::init(EgDataNodeBlueprintType* a_dataNodeBlueprint) { // blueprint from upper layer
    dataNodeBlueprint = a_dataNodeBlueprint;
    LocalNodesFile-> dataNodeBlueprint = dataNodeBlueprint;
    LocalNodesFile-> initIndexes();
    LocalNodesFile-> GetLastID(dataNodeBlueprint-> blueprintName, lastNodeID); 
}

void EgDataNodesContainerType::clear() {
    for (auto nodesIter : dataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete nodesIter.second;
    dataNodes.clear();
    for (auto nodesIter : deletedDataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete nodesIter.second;
    deletedDataNodes.clear();
    addedDataNodes.clear();
    updatedDataNodes.clear();
    nodesCount = 0;
}

int EgDataNodesContainerType::GetLastID() {
    LocalNodesFile-> GetLastID(dataNodeBlueprint-> blueprintName, lastNodeID);          
    return 0; // std::cout << "lastNodeID: " << std::dec << lastNodeID << std::endl;  
}

int EgDataNodesContainerType::LoadLocalBlueprint() {
    return dataNodeBlueprint->LocalLoadBlueprint();
}

EgDataNodeType *EgDataNodesContainerType::GetNodePtrByID(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter != dataNodes.end()) {
        // std::cout << "GetNodePtrByID() node found for ID: " << std::dec <<  nodeID << std::endl;
        return iter->second;
    }
    // std::cout << "GetNodePtrByID() node NOT found for ID: " << std::dec <<  nodeID << std::endl;
    return nullptr;
}

int EgDataNodesContainerType::AddDataNode(EgDataNodeType *newNode) {
    newNode-> dataNodeID = 1 + lastNodeID++; // dataNodeBlueprint-> getNextID();
    dataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    nodesCount++;
    addedDataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    return 0;
}

int EgDataNodesContainerType::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter == dataNodes.end())
        return -1;
    auto delIter = deletedDataNodes.find(nodeID); // search del nodes
    if (delIter != deletedDataNodes.end())
        return -2;
    auto addIter = addedDataNodes.find(nodeID); // search added nodes
    if (addIter != addedDataNodes.end())
        return -3;
    (iter->second)->  makeIndexedFieldsCopy();
    updatedDataNodes.insert(std::make_pair(nodeID, iter->second));
    // std::cout << "MarkUpdatedDataNode() done" << std::endl;
    return 0;
}

void EgDataNodesContainerType::DeleteDataNode(const EgDataNodeIDType delID) {
    bool nodeFound{false};
    auto delIter = deletedDataNodes.find(delID); // search deleted nodes
    if (delIter != deletedDataNodes.end())
        return;
    auto iter = dataNodes.find(delID); // search all nodes
    if (iter != dataNodes.end())
    {
        nodeFound = true;
        dataNodes.erase(iter);
        nodesCount--;
    }
    auto addIter = addedDataNodes.find(delID); // search added nodes
    if (addIter != addedDataNodes.end())
    {
        addedDataNodes.erase(addIter);
        return;
    }
    else if (nodeFound)
    {
        deletedDataNodes.insert(std::make_pair(delID, iter->second));
        auto updIter = updatedDataNodes.find(delID); // search updated nodes
        if (updIter != updatedDataNodes.end())
            updatedDataNodes.erase(updIter);
    }
}

int EgDataNodesContainerType::StoreToLocalFile() {
    if ( !addedDataNodes.size() &&  !updatedDataNodes.size() && !deletedDataNodes.size() ) {
        std::cout << "DEBUG: StoreToLocalFile() all nodes maps empty: " << dataNodeBlueprint-> blueprintName << std::endl;
        return 0;
    }
    bool isOk = LocalNodesFile-> StartFileUpdate(dataNodeBlueprint-> blueprintName);
    // std::cout << "StoreToLocalFile() start update" << std::endl;
    if (isOk) {
        isOk = LocalNodesFile-> UpdateNodesFile(dataNodeBlueprint-> blueprintName, addedDataNodes, deletedDataNodes, updatedDataNodes);
        LocalNodesFile-> nodesFile.close(); // close anyway
        updatedDataNodes.clear(); // ?
        deletedDataNodes.clear();
        addedDataNodes.clear();
    }
    if (! isOk)
        return -1;
    return 0;
}

int EgDataNodesContainerType::LoadAllLocalFileNodes() {
    clear();
    EgFileOffsetType nextOffset{0};
    EgDataNodeType *newNode;
    if (!LocalNodesFile->OpenFileToRead(dataNodeBlueprint-> blueprintName)) {
        // std::cout << "ERROR: loadAllLocalNodes() can't open file " << dataNodesTypeName << ".gdn" << std::endl;
        return -1;
    }
    LocalNodesFile-> getFirstNodeOffset(nextOffset); // FIXME check read
    while (nextOffset) {
        // std::cout  << "getFirstNodeOffset() nextOffset = " << std::hex << nextOffset << std::endl;
        newNode = new EgDataNodeType(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        LocalNodesFile-> ReadDataNode(newNode, nextOffset);
        if (newNode-> dataFileOffset == nextOffset) {
            std::cout << "ERROR: loadAllLocalNodes() nextOffset loop:  " << std::hex << nextOffset << std::dec << " " 
                      << dataNodeBlueprint-> blueprintName << ".gdn" << std::endl;
            return -2;
        }
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return 0;
}

int EgDataNodesContainerType::LoadLocalNodesByOffsets(std::set<EgFileOffsetType>& index_offsets) {
    clear();
    EgFileOffsetType tmpOffset{0};
    EgDataNodeType *newNode;
    if (!LocalNodesFile->OpenFileToRead(dataNodeBlueprint-> blueprintName)) {
        // std::cout << "ERROR: loadAllLocalNodes() can't open file " << dataNodesTypeName << ".gdn" << std::endl;
        return -1;
    }

    for (auto nextOffset : index_offsets) {
        // std::cout  << "getFirstNodeOffset() nextOffset = " << std::hex << nextOffset << std::endl;
        newNode = new EgDataNodeType(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        LocalNodesFile-> ReadDataNode(newNode, tmpOffset);
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return 0;
}

bool EgDataNodesContainerType::LoadLocalNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue) {
    std::set <uint64_t> offsetSet;
    clear();
    auto indexIter = LocalNodesFile-> localIndexes.find(indexName);
    if (indexIter == LocalNodesFile-> localIndexes.end()) {
        std::cout  << "LoadLocalNodesEQ() index name not found: " << indexName << std::endl;
        return false;
    }
    indexIter->second-> LoadAllDataEQ(offsetSet, fieldValue);
    // std::cout  << "LoadLocalNodesEQ() offsetSet size : " << offsetSet.size() << std::endl;
    LoadLocalNodesByOffsets(offsetSet);
    return true;
}

EgDataNodesContainerType& EgDataNodesContainerType::operator << (EgDataNodeType* newNode) {
    AddDataNode(newNode); 
    return *this; 
}

// ======================== Debug ========================

void EgDataNodesContainerType::PrintDataNodesContainer() {
    std::cout << "EgDataNodesContainer nodes: " << dataNodeBlueprint-> blueprintName << std::endl;
    for (auto nodesIter : dataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        PrintEgDataNodeFields(*(nodesIter.second));
    if (addedDataNodes.size()) {
         std::cout << "EgDataNodesContainer added nodes: " << std::endl;
        for (auto nodesIter : addedDataNodes)
            PrintEgDataNodeFields(*(nodesIter.second));
    }
    if (deletedDataNodes.size()) {
        std::cout << "EgDataNodesContainer deleted nodes: " << std::endl;
        for (auto nodesIter : deletedDataNodes)
            PrintEgDataNodeFields(*(nodesIter.second));
    }
    if (updatedDataNodes.size()) {    
        std::cout << "EgDataNodesContainer updated nodes: " << std::endl;
        for (auto nodesIter : updatedDataNodes)
            PrintEgDataNodeFields(*(nodesIter.second));
    }
}

void EgDataNodesContainerType::PrintNodesChain() {
    if (!LocalNodesFile->OpenFileToRead(dataNodeBlueprint-> blueprintName)) {
        std::cout << "ERROR: loadAllLocalNodes() can't open file " << dataNodeBlueprint-> blueprintName << ".gdn" << std::endl;
        return;
    }
    LocalNodesFile-> PrintNodesChain();
    LocalNodesFile-> nodesFile.close();
}