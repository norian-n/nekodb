#pragma once
#include <iostream>

#include "egDataNodesContainer.h"
#include "../indexes/egIndexConditions.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

typedef void (*serialLoadFunctionType)  (EgDataNodeType& dataNode);
typedef void (*serialStoreFunctionType) (EgDataNodeType& dataNode);

class EgLinksType;      // arrow links
class EgDatabaseType;   // peer database 

//  ============================================================================

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
    int  ConnectSystemNodeType(std::string a_dataNodesName); // for local testing or inside database
    int  OpenLocalBlueprint();    

    int  Connect(const std::string& nodesNameStr, EgDatabaseType& myDB);

    int  AddDataNode(EgDataNodeType& newNode);
    int  AddDataNode(EgDataNodeType* newNode);
    EgDataNodesType& operator << (EgDataNodeType* newNode) { AddDataNode(newNode); return *this; }
    EgDataNodesType& operator << (EgDataNodeType& newNode) { AddDataNode(newNode); return *this; }
    EgDataNodeIDType getAddedNodeID() { return nodesContainer->lastNodeID; }

    int  MarkUpdatedDataNode(EgDataNodeIDType nodeID);
    int  MarkUpdatedDataNode(EgDataNodeType& updNode) { return MarkUpdatedDataNode(updNode.dataNodeID); } // wrapper
    void DeleteDataNode(EgDataNodeIDType delID);
    void DeleteDataNode(EgDataNodeType& delNode) { return DeleteDataNode(delNode.dataNodeID); } // wrapper

    int  Store();
    int  LoadAllNodes();
    bool LoadNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue);

    // Projects.LoadIndexedNodes(IC<int>("owner", EQ, 2) &&  IC<int>("status", EQ, 3));
    // int  LoadNodesByOffsets() { return nodesContainer-> LoadLocalNodesByOffsets(indexOffsets); }
    
    EgDataNodeType& operator[](EgDataNodeIDType nodeID);
};

// ===================== Operators =======================

// EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode);