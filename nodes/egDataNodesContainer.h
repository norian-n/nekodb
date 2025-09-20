#pragma once
#include <cstdint>
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

    EgDataNodesContainerType(): // DEBUG - standalone blueprint
            LocalNodesFile (new EgDataNodesLocalFileType()) {}
    void init(EgDataNodeBlueprintType* a_dataNodeBlueprint); // get blueprint from upper layer
    ~EgDataNodesContainerType() { clear(); /* delete dataNodeBlueprint;*/ delete LocalNodesFile; } // FIXME check dynamic blueprint

    int GetLastID();
    void clear();
    int LoadLocalBlueprint();

    EgDataNodeType* GetNodePtrByID(EgDataNodeIDType nodeID);

    int  AddDataNode(EgDataNodeType* newNode);
    int  MarkUpdatedDataNode(EgDataNodeIDType nodeID);
    void DeleteDataNode(const EgDataNodeIDType delID);

    EgDataNodesContainerType& operator << (EgDataNodeType* newNode);

    int  StoreToLocalFile();
    int  LoadAllLocalFileNodes();
        // ======================== Debug ========================
    void PrintDataNodesContainer();
    void PrintNodesChain();
};