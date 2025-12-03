#pragma once
#include <cstdint>
#include <set>
#include "egDataNodesLocalFile.h"

class EgDataNodesContainerType {
public:
    EgDataNodeIDType    nodesCount  {0};
    EgDataNodeIDType    lastNodeID  {0}; 
    EgDataNodeBlueprintType*    dataNodeBlueprint   { nullptr };    // blueprint == class == type of data nodes
    EgDataNodesLocalFileType*   LocalNodesFile      { nullptr };    // data files *.gdn load/store support
        // data nodes content and changes tracking
    EgDataNodesMapType dataNodes;        // active nodes container
    EgDataNodesMapType addedDataNodes;
    EgDataNodesMapType updatedDataNodes;
    EgDataNodesMapType deletedDataNodes; // TODO : clear all addons on node delete

    EgDataNodesContainerType():
            LocalNodesFile (new EgDataNodesLocalFileType()) {}
    void init(EgDataNodeBlueprintType* a_dataNodeBlueprint); // get blueprint from upper layer TODO init indexes by blueprint
    ~EgDataNodesContainerType() { clear(); /* delete dataNodeBlueprint;*/ delete LocalNodesFile; } // FIXME check dynamic blueprint

    void clear();
    int  GetLastID();
    int  LoadLocalBlueprint();
    EgDataNodeType* GetNodePtrByID(EgDataNodeIDType nodeID);

    int  AddDataNode(EgDataNodeType* newNode);
    int  MarkUpdatedDataNode(EgDataNodeIDType nodeID);
    void DeleteDataNode(const EgDataNodeIDType delID);

    EgDataNodesContainerType& operator << (EgDataNodeType* newNode);

    int  StoreToLocalFile();
    int  LoadAllLocalFileNodes();
    int  LoadLocalNodesByOffsets(std::set<EgFileOffsetType>& index_offsets);
        // ======================== Debug ========================
    void PrintDataNodesContainer();
    void PrintNodesChain();
};