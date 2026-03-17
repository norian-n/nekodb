#pragma once
#include "egDataNodesContainer.h"

typedef void (*serialLoadFunctionType)  (EgDataNode& dataNode);
typedef void (*serialStoreFunctionType) (EgDataNode& dataNode);

class EgLinksSet;      // arrow links
class EgDatabase;   // peer database 

class EgDataNodesSet { // "type" means c++ type, data metatype called "blueprint"
public:
    bool                    isConnected         {  false  };    // checked blueprint file and central egDb metadata
    bool                    isDataLoaded        {  false  };
    bool                    isDataChanged       {  false  };

    std::string             nodesSetName;
    std::string             nodeBlueprintName;

    EgDatabase*             metaInfoDatabase    { nullptr };    // nodes and links central info/metadata storage
    EgDataNodeBlueprint*    dataNodeBlueprint   { nullptr };    // layout == blueprint == class == type of these data nodes
    EgDataNodesContainer*   nodesContainer      { nullptr };    // data storage of all these nodes
    EgDataNodesMapType&     dataMap;                            // shortcut to container nodes map,  for (auto iter : dataMap)

    serialLoadFunctionType  serialLoadFunction  { nullptr };    // function for automated data load from node to void* localDataPtr
    serialStoreFunctionType serialStoreFunction { nullptr };    // function for automated data store to node form void* localDataPtr

    EgDataNodesSet();
    ~EgDataNodesSet();

    void clear();

    int  Connect(const std::string& nodesNameStr, EgDatabase& myDB);
    // int  Connect(const std::string& nodesNameStr, EgDatabaseType& myDB, const std::string& blueprintNameStr);

    EgDataNode& operator[](EgDataNodeIDType nodeID);

    EgDataNodeIDType  AddDataNode(EgDataNode* newNode);
    EgDataNodeIDType  AddDataNode(EgDataNode& newNode) { return AddDataNode(&newNode); }
    EgDataNodesSet& operator << (EgDataNode* newNode) { AddDataNode(newNode); return *this; }
    EgDataNodesSet& operator << (EgDataNode& newNode) { AddDataNode(newNode); return *this; }
    EgDataNodeIDType getAddedNodeID() { return nodesContainer->lastNodeID; }

    int  MarkUpdatedDataNode(EgDataNodeIDType nodeID);
    int  MarkUpdatedDataNode(EgDataNode& updNode) { return MarkUpdatedDataNode(updNode.dataNodeID); }

    void DeleteDataNode(EgDataNodeIDType delID);
    void DeleteDataNode(EgDataNode& delNode) { return DeleteDataNode(delNode.dataNodeID); }

    int  Store();
    int  LoadAllNodes();
    bool LoadNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue);
    // Projects.LoadIndexedNodes(IC<int>("owner", EQ, 2) &&  IC<int>("status", EQ, 3));
    // int  LoadNodesByOffsets() { return nodesContainer-> LoadLocalNodesByOffsets(indexOffsets); }
    int  ConnectSystemNodeType(const std::string dataNodesSetName, const std::string& blueprintName); // for internal database storages
};