#include <iostream>

#include "egDataNodesContainer.h"

void EgDataNodesContainer::init(EgDataNodeBlueprint* a_dataNodeBlueprint, const std::string& nodesSetName) { // blueprint from upper layer
    dataNodeBlueprint = a_dataNodeBlueprint;
    LocalNodesFile-> localFileName = nodesSetName;
    LocalNodesFile-> dataNodeBlueprint = dataNodeBlueprint;
    LocalNodesFile-> initIndexes();
    LocalNodesFile-> GetLastIDFromFile(lastNodeID);
}

void EgDataNodesContainer::clear() {
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
/*
int EgDataNodesContainerType::GetLastID() {
    LocalNodesFile-> GetLastIDFromFile(lastNodeID);          
    return 0; // std::cout << "lastNodeID: " << std::dec << lastNodeID << std::endl;  
}*/

int EgDataNodesContainer::LoadLocalBlueprint() {
    return dataNodeBlueprint->LocalLoadBlueprint();
}

EgDataNode *EgDataNodesContainer::GetNodePtrByID(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter != dataNodes.end()) {
        // std::cout << "GetNodePtrByID() node found for ID: " << std::dec <<  nodeID << std::endl;
        return iter->second;
    }
    // std::cout << "GetNodePtrByID() node NOT found for ID: " << std::dec <<  nodeID << std::endl;
    return nullptr;
}

EgDataNodeIDType EgDataNodesContainer::AddDataNode(EgDataNode *newNode) {
    newNode-> dataNodeID = 1 + lastNodeID++;
    nodesCount++;
    dataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    addedDataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    return newNode-> dataNodeID;
}

int EgDataNodesContainer::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
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

void EgDataNodesContainer::DeleteDataNode(const EgDataNodeIDType delID) {
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

int EgDataNodesContainer::StoreToLocalFile() {
    if ( !addedDataNodes.size() &&  !updatedDataNodes.size() && !deletedDataNodes.size() ) {
        std::cout << "DEBUG: StoreToLocalFile() all nodes maps empty: " <<  LocalNodesFile-> localFileName << std::endl;
        return 0;
    }
    bool isOk = LocalNodesFile-> StartFileUpdate();
    // std::cout << "StoreToLocalFile() start update" << std::endl;
    if (isOk) {
        isOk = LocalNodesFile-> UpdateNodesFile(addedDataNodes, deletedDataNodes, updatedDataNodes);
        LocalNodesFile-> nodesFile.close(); // close anyway
        updatedDataNodes.clear(); // ?
        deletedDataNodes.clear();
        addedDataNodes.clear();
    }
    if (! isOk)
        return -1;
    return 0;
}

int EgDataNodesContainer::LoadAllLocalFileNodes() {
    clear();
    EgFileOffsetType nextOffset{0};
    EgDataNode* newNode;
    if (!LocalNodesFile->OpenFileToRead()) {
        // EG_LOG_STUB << "ERROR: loadAllLocalNodes() can't open file " << LocalNodesFile-> localFileName << ".gdn" << FN;
        return -1;
    }
    LocalNodesFile-> getFirstNodeOffset(nextOffset);
    while (nextOffset) {
        newNode = new EgDataNode(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        LocalNodesFile-> ReadDataNode(newNode, nextOffset);
        /* if (newNode-> dataFileOffset == nextOffset) {
            EG_LOG_STUB << "ERROR: loadAllLocalNodes() nextOffset loop:  " << std::hex << nextOffset << std::dec << " " 
                      << LocalNodesFile-> localFileName << ".gdn" << FN;
            LocalNodesFile-> nodesFile.close();
            return -2;
        } */
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return 0;
}

int EgDataNodesContainer::LoadLocalNodesByOffsets(std::set<EgFileOffsetType>& index_offsets) {
    clear();
    EgFileOffsetType tmpOffset{0};
    EgDataNode *newNode;
    if (!LocalNodesFile->OpenFileToRead()) {
        // std::cout << "ERROR: loadAllLocalNodes() can't open file " << dataNodesTypeName << ".gdn" << std::endl;
        return -1;
    }

    for (auto nextOffset : index_offsets) {
        // std::cout  << "getFirstNodeOffset() nextOffset = " << std::hex << nextOffset << std::endl;
        newNode = new EgDataNode(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        LocalNodesFile-> ReadDataNode(newNode, tmpOffset);
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return 0;
}

bool EgDataNodesContainer::LoadLocalNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue) {
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

EgDataNodesContainer& EgDataNodesContainer::operator << (EgDataNode* newNode) {
    AddDataNode(newNode); 
    return *this; 
}

// ======================== Debug ========================

void EgDataNodesContainer::PrintDataNodesContainer() {
    std::cout << "EgDataNodesContainer nodes: " << LocalNodesFile-> localFileName << std::endl;
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

void EgDataNodesContainer::PrintNodesChain() {
    if (!LocalNodesFile->OpenFileToRead()) {
        std::cout << "ERROR: loadAllLocalNodes() can't open file " << LocalNodesFile-> localFileName << ".gdn" << std::endl;
        return;
    }
    LocalNodesFile-> PrintNodesChain();
    LocalNodesFile-> nodesFile.close();
}