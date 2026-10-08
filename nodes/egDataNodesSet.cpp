#include <iostream>

#include "egDataNodesSet.h"
#include "../metainfo/egDatabase.h"

EgDataNodeBlueprint bpNotFound("nodeNotFound");
EgDataNode nodeNotFound(&bpNotFound);  // dummy data node for GUI if no data found

EgDataNodesSet::EgDataNodesSet(): 
    nodesContainer (new EgDataNodesContainer), // MEM_NEW --> destructor
    dataMap(nodesContainer-> dataNodes) {}     // ref

EgDataNodesSet::~EgDataNodesSet() { 
    clear(); 
    delete dataNodeBlueprint; // MEM_DEL <-- Connect, ConnectSystemNodesSet
    delete nodesContainer;    // MEM_DEL <-- constructor
}

bool EgDataNodesSet::Connect(const std::string& nodesNameStr, EgDatabase& myDB) {
    if (isConnected) {
        // EG_LOG_STUB << "WARNING: Connect(): trying to connect data nodes type again: " << nodesNameStr << FN;
        return true;
    }
    metaInfoDatabase = &myDB;
    isConnected = false;
    nodesSetName = nodesNameStr;
    // EgDataNode* metaInfoNode = metaInfoDatabase-> dataNodePtrByNodesType(this);
    if (myDB.blueprintNameByNodeName(nodesSetName, nodeBlueprintName) ) { //metaInfoNode) {
        EG_LOG_STUB << "ERROR: Connect() dataNodes type not found in metainfo: " << nodesSetName << FN;
        return false;
    }
    // EG_LOG_STUB << "DEBUG nodeBlueprintName : " << nodeBlueprintName << FN;
    if ( ! dataNodeBlueprint )
        dataNodeBlueprint = new EgDataNodeBlueprint(nodeBlueprintName); // TODO use BP ID // MEM_NEW --> destructor
    if ( ! dataNodeBlueprint-> LocalLoadBlueprint() ) {
        EG_LOG_STUB << "ERROR: Connect() can't open data nodes blueprint file " << nodeBlueprintName << ".dnl" << FN;
        return false;
    }
    // metaInfoNode-> serialDataPtr = static_cast < void* > (this);  // FIXME check & doc shortcut, parallel issue
    nodesContainer-> init(dataNodeBlueprint, nodesSetName);
    isConnected = true;
    // EG_LOG_STUB << "connected nodes set: " << nodesSetName << FN;
    return true;
}
/*
int EgDataNodesType::Connect(const std::string& nodesNameStr, EgDatabaseType& myDB, const std::string& blueprintNameStr) {
    if (isConnected) {
        std::cout << "WARNING: Connect(): trying to connect data nodes type again: " << nodesNameStr << std::endl;
        return 0;
    }
    metaInfoDatabase = &myDB;
    isConnected = false;
    dataNodesName = nodesNameStr;
    EgDataNode* metaInfoNode = metaInfoDatabase-> dataNodePtrByNodesType(this);
    if (! metaInfoNode) {
        std::cout << "ERROR: Connect() dataNodes type not found in metainfo: " << dataNodesName << std::endl;
        return -2;
    }
    // FIXME TODO get BP name from metainfo by nodes name
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprintType(blueprintNameStr); // TODO use BP ID // MEM_NEW --> destructor
    if (dataNodeBlueprint-> LocalLoadBlueprint()) {
        std::cout << "ERROR: Connect() can't open data nodes blueprint file " << dataNodesName << ".dnl" << std::endl;
        return -1;
    }
    metaInfoNode-> serialDataPtr = static_cast < void* > (this);  // FIXME check & doc shortcut
    nodesContainer-> init(dataNodeBlueprint);
    isConnected = true;
    return 0;
} */

bool EgDataNodesSet::ConnectSystemNodesSet(const std::string dataNodesSetName, const std::string& blueprintName) { // sailent connect, no metainfo register
    if (isConnected) return true;
    isConnected = false;
    nodesSetName = dataNodesSetName;
    nodeBlueprintName = blueprintName;
    if ( ! dataNodeBlueprint )
        dataNodeBlueprint = new EgDataNodeBlueprint(blueprintName); // MEM_NEW --> destructor
    if ( ! dataNodeBlueprint-> LocalLoadBlueprint() ) {
        // EG_LOG_STUB << "ERROR: can't find blueprint: " << blueprintName << FN;
        return false;
    }
    nodesContainer-> init(dataNodeBlueprint, nodesSetName);
    isConnected = true;
    return true;
}

void EgDataNodesSet::clear() {
    nodesContainer-> clear();
    isDataLoaded  = false;
    isDataChanged = false;
}

EgDataNodeIDType EgDataNodesSet::AddDataNode(EgDataNode* newNode) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return 0;
    }
    EgDataNodeIDType res = nodesContainer-> AddDataNode(newNode);
    isDataChanged = true;
    return res;
}

bool EgDataNodesSet::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    // EG_LOG_STUB << dataNodesName << " nodeID : " << std::dec << nodeID << FN;
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return false;
    }
    bool res = nodesContainer-> MarkUpdatedDataNode(nodeID);
    isDataChanged = isDataChanged || res; // changed if marked by container
    return res;
}

void EgDataNodesSet::DeleteDataNode(EgDataNodeIDType delID) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return;
    }
    nodesContainer-> DeleteDataNode(delID);
    isDataChanged = true;
}

bool EgDataNodesSet::Store() {
    /* bool emptyMaps = !nodesContainer-> addedDataNodes.size() &&  !nodesContainer-> updatedDataNodes.size() 
        && !nodesContainer-> deletedDataNodes.size();
    if ( emptyMaps && isDataChanged )
        EG_LOG_STUB << "DEBUG: Store() emptyMaps && isDataUpdated logical conflict at nodes set: " << nodesSetName << FN; */
    if (! isDataChanged)
        return true;
    if (! isConnected) {
        EG_LOG_STUB << "ERROR: Store() - nodes set is not connected to database: " << nodesSetName << FN;
        return false;
    }
    if (serialStoreFunction && (nodesContainer-> addedDataNodes.size() || nodesContainer-> updatedDataNodes.size())) {
        for (auto addedNodesIter : nodesContainer-> addedDataNodes)
            if (addedNodesIter.second-> serialDataPtr)
                (*serialStoreFunction)(*addedNodesIter.second);
        for (auto updNodesIter : nodesContainer-> updatedDataNodes)
            if (updNodesIter.second-> serialDataPtr)
                (*serialStoreFunction)(*updNodesIter.second);
    }
    isDataChanged = false; // FIXME check err proc logic
    return nodesContainer-> StoreToLocalFile();
}

bool EgDataNodesSet::LoadAllNodes() {
    if (! isConnected) {
        EG_LOG_STUB << "ERROR: LoadAllNodes() - nodes set is not connected to database: " << nodesSetName << FN;
        return false;
    }
    isDataLoaded = nodesContainer-> LoadAllLocalFileNodes();
    // EG_LOG_STUB << "res: " << res << FN;
    if (serialLoadFunction)
        for (auto nodesIter : nodesContainer-> dataNodes) {
            // EG_LOG_STUB << "call serialLoadFunction for node: " << nodesIter.second-> dataNodeID << FN;
            (*serialLoadFunction)(*nodesIter.second);
        }
    return isDataLoaded;
}

bool EgDataNodesSet::LoadNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue) {
    isDataLoaded = nodesContainer-> LoadLocalNodesEQ(indexName, fieldValue);
    return isDataLoaded;
}

bool EgDataNodesSet::CherryPickNode(EgDataNodeIDType nodeID) {
    return nodesContainer-> CherryPickNode(nodeID);
}


EgDataNode& EgDataNodesSet::operator[](EgDataNodeIDType nodeID) {
    EgDataNode* nodePtr = nodesContainer-> GetNodePtrByID(nodeID);
    if (nodePtr)
        return *nodePtr;
    return nodeNotFound;
}

// ===================== Operators =======================

// EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode) { nodesServType.nodesContainer-> AddDataNode(newNode); return nodesServType; }
