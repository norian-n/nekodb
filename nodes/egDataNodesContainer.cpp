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
/*
bool EgDataNodesContainer::LoadLocalBlueprint() {
    return dataNodeBlueprint-> LocalLoadBlueprint();
}*/

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

bool EgDataNodesContainer::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter == dataNodes.end())
        return false;
    auto delIter = deletedDataNodes.find(nodeID); // search del nodes, fail if found
    if (delIter != deletedDataNodes.end())
        return false;
    auto addIter = addedDataNodes.find(nodeID); // search added nodes, ok if found
    if (addIter != addedDataNodes.end())
        return true;
    (iter->second)->  makeIndexedFieldsCopy();
    updatedDataNodes.insert(std::make_pair(nodeID, iter->second));
    // std::cout << "MarkUpdatedDataNode() done" << std::endl;
    return true;
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

bool EgDataNodesContainer::StoreToLocalFile() {
    if ( !addedDataNodes.size() &&  !updatedDataNodes.size() && !deletedDataNodes.size() ) { // empty change sets
        // EG_LOG_STUB << "DEBUG: StoreToLocalFile() all nodes maps empty: " << LocalNodesFile-> localFileName << FN;
        return true;
    }
    bool isOk = LocalNodesFile-> StartFileUpdate(); //
    if (isOk) {
        isOk = LocalNodesFile-> UpdateNodesFile(addedDataNodes, deletedDataNodes, updatedDataNodes);
        LocalNodesFile-> nodesFile.close(); // close anyway
        updatedDataNodes.clear();
        deletedDataNodes.clear();
        addedDataNodes.clear();
    } else
        EG_LOG_STUB << "ERROR: can't open file to store nodes: " << LocalNodesFile-> localFileName << FN;    
    return isOk;
}

bool EgDataNodesContainer::LoadAllLocalFileNodes() {
    clear();
    EgFileOffsetType nextOffset{0};
    EgDataNode* newNode {nullptr};
    if ( ! LocalNodesFile->OpenFileToRead() ) {
        // EG_LOG_STUB << "ERROR: can't open file to read nodes: " << LocalNodesFile-> localFileName << FN;
        return false;
    }
    if ( ! LocalNodesFile-> getFirstNodeOffset(nextOffset) ) {
        EG_LOG_STUB << "ERROR: loadAllLocalNodes() can't get first offset: " << LocalNodesFile-> localFileName << ".gdn" << FN;
        return false;
    }
    while (nextOffset) {
        newNode = new EgDataNode(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        if ( ! LocalNodesFile-> ReadDataNode(newNode, nextOffset) ) {
            EG_LOG_STUB << "ERROR: can't read data node by offset: " << nextOffset << FN;
            return false;
        }
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return true;
}

bool EgDataNodesContainer::CherryPickNode(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter != dataNodes.end()) // node loaded
        return true;
    // clear();
    EgDataNode* localNode {nullptr};
    EgFileOffsetType nextOffset{0};
    if ( ! LocalNodesFile-> OpenFileToRead() ) {
        EG_LOG_STUB << "ERROR: can't open file to read nodes: " << LocalNodesFile-> localFileName << FN;
        return false;
    }
    localNode = new EgDataNode(dataNodeBlueprint);
    LocalNodesFile-> getFirstNodeOffset(nextOffset);
    while (nextOffset) { // scan all nodes FIXME TODO optimize read ID only
        localNode-> dataFileOffset = nextOffset; // save offset
        if ( ! LocalNodesFile-> ReadDataNodeIDOnly(localNode, nextOffset) ) {
            EG_LOG_STUB << "ERROR: can't read data node by offset: " << nextOffset << FN;
            return false;
        }
        if (localNode-> dataNodeID == nodeID) {
            LocalNodesFile-> ReadDataNode(localNode, nextOffset); // read data
            break;
        }
    }
    if (localNode-> dataNodeID == nodeID)
        dataNodes.insert(std::make_pair(localNode-> dataNodeID, localNode));
    else
        delete localNode;
    nodesCount = 1;
    LocalNodesFile-> nodesFile.close();
    return true;
}

bool EgDataNodesContainer::LoadLocalNodesByOffsets(std::set<EgFileOffsetType>& indexOffsets) {
    clear();
    EgFileOffsetType tmpOffset{0}; // fake next offset
    EgDataNode *newNode;
    if ( ! LocalNodesFile-> OpenFileToRead() ) {
        EG_LOG_STUB << "ERROR: can't open file to read nodes: " << LocalNodesFile-> localFileName << FN;
        return false;
    }
    for (auto nextOffset : indexOffsets) {
        // EG_LOG_STUB << "next offset: " << nextOffset << FN;
        newNode = new EgDataNode(dataNodeBlueprint);
        newNode-> dataFileOffset = nextOffset;
        if ( ! LocalNodesFile-> ReadDataNode(newNode, tmpOffset) ) {
            EG_LOG_STUB << "ERROR: can't read data node by offset: " << nextOffset << FN;
            return false;
        }
        dataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    }
    nodesCount = dataNodes.size();
    LocalNodesFile-> nodesFile.close();
    return true;
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
    return LoadLocalNodesByOffsets(offsetSet);
}

EgDataNodesContainer& EgDataNodesContainer::operator << (EgDataNode* newNode) {
    AddDataNode(newNode); 
    return *this; 
}

// ======================== Debug ========================

void EgDataNodesContainer::PrintDataNodesContainer() {
    std::cout << "EgDataNodesContainer nodes: " << LocalNodesFile-> localFileName << std::endl;
    for (auto nodesIter : dataNodes)
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