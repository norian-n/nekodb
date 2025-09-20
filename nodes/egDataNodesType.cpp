#include <iostream>

#include "egDataNodesType.h"
#include "../metainfo/egDatabaseType.h"

EgDataNodeBlueprintType bpNotFound("nodeNotFound");
EgDataNodeType nodeNotFound(&bpNotFound);  // dummy data node for GUI if no data found
EgDataNodesMapType fakeDataMap;

EgDataNodesType::EgDataNodesType(): 
    nodesContainer (new EgDataNodesContainerType), 
    dataMap(nodesContainer-> dataNodes), fDataMap(fakeDataMap) {}// real container init at Connect() so set fake ref

int EgDataNodesType::Connect(std::string& nodesNameStr, EgDatabaseType &myDB) {
    if (isConnected) {
        std::cout << "WARNING: Connect(): trying to connect data nodes type again: " << nodesNameStr << std::endl;
        return 0;
    }
    metaInfoDatabase = &myDB;
    isConnected = false;
    dataNodesName = nodesNameStr;
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprintType(dataNodesName);
    if (OpenLocalBlueprint()) {
        std::cout << "ERROR: Connect() can't open data nodes blueprint file " << dataNodesName << ".dnl" << std::endl;
        return -1;
    }
    if (metaInfoDatabase->nodeTypeIDByName(dataNodesName, dataNodeBlueprint->blueprintID)) { // reverse return logic - true if not found
        std::cout << "ERROR: Connect() dataNodes type not found in metainfo: " << dataNodesName << std::endl;
        return -2;
    }
    metaInfoDatabase-> InsertDataNodesTypeToMap(dataNodeBlueprint-> blueprintID, this);    
    nodesContainer->init(dataNodeBlueprint);
    isConnected = true;
    return 0;
}
/*
int EgDataNodesType::ConnectLink(std::string& linkNameStr, EgDatabaseType &myDB) {
    if (isConnected) {
        std::cout << "WARNING: Connect() trying to connect data nodes again, use Reconnect(): " << dataNodesName << std::endl;
        return 0;
    }
    metaInfoDatabase = &myDB;
    isConnected = false;
    dataNodesName = linkNameStr + "_arrowLinks";
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprintType(dataNodesName);
    if (! nodesContainer)
        nodesContainer = new EgDataNodesContainerType(dataNodesName, dataNodeBlueprint);
    if (OpenLocalBlueprint()) {
        std::cout << "ERROR: Connect() can't open data links blueprint " << dataNodesName << ".dnl" << std::endl;
        return -1;
    }
    if (metaInfoDatabase->linkTypeIDByName(dataNodesName, dataNodeBlueprint->blueprintID)) { // reverse return logic - true if not found
        std::cout << "ERROR: Connect() dataLinks ID not found in metainfo: " << dataNodesName << std::endl;
        return -2;
    }
    metaInfoDatabase-> InsertDataNodesTypeToMap(dataNodeBlueprint-> blueprintID, this);
    nodesContainer-> GetLastID();
    isConnected = true;
    return 0;
} */

int EgDataNodesType::ConnectSystemNodeType(std::string a_dataNodesName) { // sailent connect, no metainfo register
    if (isConnected) return 0;
    isConnected = false;
    dataNodesName = a_dataNodesName;
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprintType(a_dataNodesName);
    if (OpenLocalBlueprint()) {
        // std::cout << "ConnectSystemNodeType() can't open data nodes blueprint " << dataNodesName << ".dnl" << std::endl;
        return -1;
    }
    nodesContainer->init(dataNodeBlueprint);
    isConnected = true;
    return 0;
}

void EgDataNodesType::clear() {
    nodesContainer-> clear();
}

int EgDataNodesType::OpenLocalBlueprint() {
    return dataNodeBlueprint-> LocalLoadBlueprint();
}

int EgDataNodesType::AddDataNode(EgDataNodeType& newNode) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return -1;
    }
    return nodesContainer-> AddDataNode(&newNode);
}

int EgDataNodesType::AddDataNode(EgDataNodeType* newNode) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return -1;
    }
    return nodesContainer-> AddDataNode(newNode);
}

int EgDataNodesType::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return -1;
    }   
    return nodesContainer-> MarkUpdatedDataNode(nodeID);
}

void EgDataNodesType::DeleteDataNode(EgDataNodeIDType delID) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return;
    }
    return nodesContainer-> DeleteDataNode(delID);
}

int EgDataNodesType::Store() {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return -1;
    }
    if (serialStoreFunction && (nodesContainer-> addedDataNodes.size() || nodesContainer-> updatedDataNodes.size())) {
        for (auto addedNodesIter : nodesContainer-> addedDataNodes)
            if (addedNodesIter.second-> serialDataPtr)
                (*serialStoreFunction)(*addedNodesIter.second);
        for (auto updNodesIter : nodesContainer-> updatedDataNodes)
            if (updNodesIter.second-> serialDataPtr)
                (*serialStoreFunction)(*updNodesIter.second);
    }
    return nodesContainer-> StoreToLocalFile();
}

int EgDataNodesType::LoadAllNodes() {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << dataNodesName << std::endl;
        return -1;
    }
    int res = nodesContainer-> LoadAllLocalFileNodes();
    if (serialLoadFunction)
        for (auto nodesIter : nodesContainer-> dataNodes)
            (*serialLoadFunction)(*nodesIter.second);
    return res;
}

EgDataNodeType &EgDataNodesType::operator[](EgDataNodeIDType nodeID) {
    EgDataNodeType *nodePtr = nodesContainer-> GetNodePtrByID(nodeID);
    if (nodePtr)
        return *nodePtr;
    return nodeNotFound;
}

// ===================== Operators =======================

// EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode) { nodesServType.nodesContainer-> AddDataNode(newNode); return nodesServType; }
