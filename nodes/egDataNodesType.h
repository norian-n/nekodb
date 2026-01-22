#pragma once
#include "egDataNodesContainer.h"

typedef void (*serialLoadFunctionType)  (EgDataNode& dataNode);
typedef void (*serialStoreFunctionType) (EgDataNode& dataNode);

class EgLinksType;      // arrow links
class EgDatabaseType;   // peer database 

class EgDataNodesType { // "type" means c++ type, data metatype called "blueprint"
public:
    bool                        isConnected         {  false  };    // checked blueprint file and central egDb metadata
    bool                        isDataLoaded        {  false  };
    bool                        isDataUpdated       {  false  };

    std::string                 dataNodesName;
    EgDatabaseType*             metaInfoDatabase    { nullptr };    // nodes and links layout == blueprint == class == type info
    EgDataNodeBlueprintType*    dataNodeBlueprint   { nullptr };    // layout == blueprint == class == type of these data nodes
    EgDataNodesContainerType*   nodesContainer      { nullptr };    // data storage of all these nodes
    EgDataNodesMapType&         dataMap;                            // shortcut to container nodes map,  for (auto iter : dataMap)

    serialLoadFunctionType      serialLoadFunction  { nullptr };    // function for automated data load from node to void* localDataPtr
    serialStoreFunctionType     serialStoreFunction { nullptr };    // function for automated data store to node form void* localDataPtr

    EgDataNodesType();
    ~EgDataNodesType() { clear(); delete dataNodeBlueprint; delete nodesContainer; }

    void clear();
    int  ConnectSystemNodeType(std::string a_dataNodesName); // for local testing or internal database storages
    int  OpenLocalBlueprint();    

    int  Connect(const std::string& nodesNameStr, EgDatabaseType& myDB);

    int  AddDataNode(EgDataNode* newNode);
    int  AddDataNode(EgDataNode& newNode) { return AddDataNode(&newNode); }

    EgDataNodesType& operator << (EgDataNode* newNode) { AddDataNode(newNode); return *this; }
    EgDataNodesType& operator << (EgDataNode& newNode) { AddDataNode(newNode); return *this; }
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
    
    EgDataNode& operator[](EgDataNodeIDType nodeID);
};