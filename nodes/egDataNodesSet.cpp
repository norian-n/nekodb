#include <iostream>

#include "egDataNodesSet.h"
#include "../metainfo/egDatabase.h"

EgDataNodeBlueprint bpNotFound("nodeNotFound");
EgDataNode nodeNotFound(&bpNotFound);  // dummy data node for GUI if no data found

EgDataNodesSet::EgDataNodesSet(): 
    nodesContainer (new EgDataNodesContainer), // MEM_NEW --> destructor
    dataMap(nodesContainer-> dataNodes) {}         // real container init at Connect()

EgDataNodesSet::~EgDataNodesSet() { 
    clear(); 
    delete dataNodeBlueprint; // MEM_DEL <-- Connect, ConnectSystemNodeType
    delete nodesContainer;    // MEM_DEL <-- constructor
}

int EgDataNodesSet::Connect(const std::string& nodesNameStr, EgDatabase& myDB) {
    if (isConnected) {
        std::cout << "WARNING: Connect(): trying to connect data nodes type again: " << nodesNameStr << std::endl;
        return 0;
    }
    metaInfoDatabase = &myDB;
    isConnected = false;
    nodesSetName = nodesNameStr;
    // EgDataNode* metaInfoNode = metaInfoDatabase-> dataNodePtrByNodesType(this);
    if (myDB.blueprintNameByNodeName(nodesSetName, nodeBlueprintName) ) { //metaInfoNode) {
        EG_LOG_STUB << "ERROR: Connect() dataNodes type not found in metainfo: " << nodesSetName << FN;
        return -2;
    }
    // EG_LOG_STUB << "DEBUG nodeBlueprintName : " << nodeBlueprintName << FN;
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprint(nodeBlueprintName); // TODO use BP ID // MEM_NEW --> destructor
    if (dataNodeBlueprint-> LocalLoadBlueprint()) {
        EG_LOG_STUB << "ERROR: Connect() can't open data nodes blueprint file " << nodeBlueprintName << ".dnl" << FN;
        return -1;
    }
    // metaInfoNode-> serialDataPtr = static_cast < void* > (this);  // FIXME check & doc shortcut, parallel issue
    nodesContainer-> init(dataNodeBlueprint, nodesSetName);
    isConnected = true;
    return 0;
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

int EgDataNodesSet::ConnectSystemNodeType(const std::string dataNodesSetName, const std::string& blueprintName) { // sailent connect, no metainfo register
    if (isConnected) return 0;
    isConnected = false;
    nodesSetName = dataNodesSetName;
    if (! dataNodeBlueprint)
        dataNodeBlueprint = new EgDataNodeBlueprint(blueprintName); // MEM_NEW --> destructor
    if (dataNodeBlueprint-> LocalLoadBlueprint()) {
        // EG_LOG_STUB << "ERROR: can't find blueprint: " << blueprintName << FN;
        return -1;
    }
    nodesContainer-> init(dataNodeBlueprint, nodesSetName);
    isConnected = true;
    // std::cout << "ConnectSystemNodeType() done for " << dataNodesSetName << " , " << blueprintName << std::endl;
    return 0;
}

void EgDataNodesSet::clear() {
    nodesContainer-> clear();
    isDataLoaded  = false;
    isDataChanged = false;
}

/*int EgDataNodesType::OpenLocalBlueprint() {
    return dataNodeBlueprint-> LocalLoadBlueprint();
}*/

EgDataNodeIDType EgDataNodesSet::AddDataNode(EgDataNode* newNode) {
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return 0;
    }
    EgDataNodeIDType res = nodesContainer-> AddDataNode(newNode);
    isDataChanged = true; // isDataUpdated || ! (bool) res;
    return res;
}

int EgDataNodesSet::MarkUpdatedDataNode(EgDataNodeIDType nodeID) {
    // EG_LOG_STUB << dataNodesName << " nodeID : " << std::dec << nodeID << FN;
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return -1;
    }
    int res = nodesContainer-> MarkUpdatedDataNode(nodeID);
    isDataChanged = isDataChanged || ! (bool) res;
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

int EgDataNodesSet::Store() {
    bool emptyMaps = !nodesContainer-> addedDataNodes.size() &&  !nodesContainer-> updatedDataNodes.size() 
        && !nodesContainer-> deletedDataNodes.size();
    if ( emptyMaps && isDataChanged )
        std::cout << "DEBUG: Store() emptyMaps && isDataUpdated logical conflict at nodes type " << nodesSetName << std::endl;
    if (! isDataChanged)
        return 0;
    if (! isConnected) {
        std::cout << "ERROR: EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
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
    isDataChanged = false; // FIXME check err proc logic
    return nodesContainer-> StoreToLocalFile();
}

int EgDataNodesSet::LoadAllNodes() {
    if (! isConnected) {
        std::cout << "ERROR: LoadAllNodes(): EgDataNodesType is not connected to database: " << nodesSetName << std::endl;
        return -1;
    }
    int res = nodesContainer-> LoadAllLocalFileNodes();
    // std::cout << "LoadAllNodes() res: " << res << std::endl;
    if (serialLoadFunction)
        for (auto nodesIter : nodesContainer-> dataNodes) {
            (*serialLoadFunction)(*nodesIter.second);
            // std::cout << "iter nodeID: " << nodesIter.second-> dataNodeID << std::endl;
        }
    isDataLoaded = ! (bool) res;
    return res;
}

bool EgDataNodesSet::LoadNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue) {
    isDataLoaded = nodesContainer-> LoadLocalNodesEQ(indexName, fieldValue);
    return isDataLoaded;
}

EgDataNode& EgDataNodesSet::operator[](EgDataNodeIDType nodeID) {
    EgDataNode* nodePtr = nodesContainer-> GetNodePtrByID(nodeID);
    if (nodePtr)
        return *nodePtr;
    return nodeNotFound;
}

// ===================== Operators =======================

// EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode) { nodesServType.nodesContainer-> AddDataNode(newNode); return nodesServType; }
