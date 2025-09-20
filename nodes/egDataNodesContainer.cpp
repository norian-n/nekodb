#include <iostream>

#include "egDataNodesContainer.h"

void EgDataNodesContainerType::init(EgDataNodeBlueprintType* a_dataNodeBlueprint) { // blueprint from upper layer
    dataNodeBlueprint = a_dataNodeBlueprint;
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
}

int EgDataNodesContainerType::GetLastID() {
    LocalNodesFile-> GetLastID(dataNodeBlueprint-> blueprintName, lastNodeID);          
    return 0; // std::cout << "lastNodeID: " << std::dec << lastNodeID << std::endl;
    // else
    //    return -1;    
}

int EgDataNodesContainerType::LoadLocalBlueprint() {
    return dataNodeBlueprint->LocalLoadBlueprint();
}

EgDataNodeType *EgDataNodesContainerType::GetNodePtrByID(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter != dataNodes.end())
        return iter->second;
    return nullptr;
}

int EgDataNodesContainerType::AddDataNode(EgDataNodeType *newNode) {
    newNode-> dataNodeID = 1 + lastNodeID++; // dataNodeBlueprint-> getNextID();
    dataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    addedDataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    return 0;
}

int EgDataNodesContainerType::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter == dataNodes.end())
        return -1;
    auto delIter = deletedDataNodes.find(nodeID); // search all nodes
    if (delIter != deletedDataNodes.end())
        return -2;
    auto addIter = addedDataNodes.find(nodeID); // search added nodes
    if (addIter != addedDataNodes.end())
        return -3;
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
    if ( !addedDataNodes.size() &&  !updatedDataNodes.size() && !deletedDataNodes.size()) {
        std::cout << "DEBUG: StoreToLocalFile() all nodes maps empty: " << dataNodeBlueprint-> blueprintName << std::endl;
        return 0;
    }
    bool isOk = LocalNodesFile->StartFileUpdate(dataNodeBlueprint-> blueprintName);
    // std::cout << "StoreToLocalFile() start update" << std::endl;
    if (isOk) {
        isOk = LocalNodesFile-> UpdateNodesFile(dataNodeBlueprint-> blueprintName, addedDataNodes, deletedDataNodes, updatedDataNodes);
/*
    for (auto delNodesIter : deletedDataNodes) { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // PrintEgDataNodeTypeFields(*(delNodesIter.second));
        LocalNodesFile-> DeleteDataNode(delNodesIter.second);
    }
    // std::cout << "===== After DeleteDataNode() =====" << std::endl;
    for (auto updNodesIter : updatedDataNodes)
    {
        // PrintEgDataNodeTypeFields(*(updNodesIter.second));
        LocalNodesFile-> DeleteDataNode(updNodesIter.second);
        LocalNodesFile-> WriteDataNode (updNodesIter.second);
    }
    // std::cout << "===== After WriteDataNode 1 =====" << std::endl;
    for (auto newNodesIter : addedDataNodes) {
        // PrintEgDataNodeTypeFields(*(newNodesIter.second));
        // std::cout << "StoreToLocalFile() container fieldsCount: " << std::dec << (int) ((newNodesIter.second)-> dataNodeBlueprint-> fieldsCount) << std::endl;
        std::cout << "StoreToLocalFile() container size: " << std::dec << (int) ((newNodesIter.second)-> dataFieldsContainer.dataFields.size()) << std::endl;
        // std::cout << "newNodesIter.second: " << std::hex << (uint64_t) &((newNodesIter.second)-> dataFieldsContainer.dataFields) << std::endl;
        // std::cout << "* (newNodesIter.second) : " << std::hex << (uint64_t) &((*(newNodesIter.second)).dataFieldsContainer.dataFields) << std::endl;
        LocalNodesFile-> WriteDataNode(newNodesIter.second);
    }
*/
    // std::cout << "===== After WriteDataNode 2 =====" << std::endl;
        LocalNodesFile-> nodesFile.close(); // close anyway
        updatedDataNodes.clear(); // ?
        deletedDataNodes.clear();
        addedDataNodes.clear();
    }
    if (! isOk)
        return -1;
    // std::cout << "===== StoreToLocalFile() exit =====" << std::endl;
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
        LocalNodesFile->ReadDataNode(newNode, nextOffset);
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    LocalNodesFile-> nodesFile.close();
    return 0;
}

EgDataNodesContainerType& EgDataNodesContainerType::operator << (EgDataNodeType* newNode) {
    AddDataNode(newNode); 
    return *this; 
}

// ======================== Debug ========================

void EgDataNodesContainerType::PrintDataNodesContainer() {
    std::cout << "EgDataNodesContainer nodes: " << dataNodeBlueprint-> blueprintName << std::endl;
    for (auto nodesIter : dataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        PrintEgDataNodeTypeFields(*(nodesIter.second));
    if (addedDataNodes.size()) {
         std::cout << "EgDataNodesContainer added nodes: " << std::endl;
        for (auto nodesIter : addedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
    }
    if (deletedDataNodes.size()) {
        std::cout << "EgDataNodesContainer deleted nodes: " << std::endl;
        for (auto nodesIter : deletedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
    }
    if (updatedDataNodes.size()) {    
        std::cout << "EgDataNodesContainer updated nodes: " << std::endl;
        for (auto nodesIter : updatedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
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