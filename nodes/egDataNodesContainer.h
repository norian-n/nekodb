#pragma once
#include "egDataNodesLocalFile.h"

class EgDataNodesContainer {
public:
    EgDataNodeIDType    nodesCount  {0};
    EgDataNodeIDType    lastNodeID  {0};
    EgDataNodeBlueprint*    dataNodeBlueprint   { nullptr };    // blueprint == class == type of data nodes
    EgDataNodesLocalFileType*   LocalNodesFile      { nullptr };    // data files *.gdn load/store support
        // data nodes content and changes tracking
    EgDataNodesMapType dataNodes;        // active nodes container
    EgDataNodesOrdMapType addedDataNodes;
    EgDataNodesMapType updatedDataNodes;
    EgDataNodesMapType deletedDataNodes; // TODO : clear all addons on node delete

    EgDataNodesContainer():
            LocalNodesFile (new EgDataNodesLocalFileType()) {}
    void init(EgDataNodeBlueprint* a_dataNodeBlueprint, const std::string& nodesSetName); // get info from upper layer TODO init indexes by blueprint
    ~EgDataNodesContainer() { clear(); /* delete dataNodeBlueprint;*/ delete LocalNodesFile; } // FIXME check dynamic blueprint

    void clear();
    // int  GetLastID();
    int  LoadLocalBlueprint();
    EgDataNode* GetNodePtrByID(EgDataNodeIDType nodeID);

    EgDataNodeIDType AddDataNode(EgDataNode* newNode);
    int  MarkUpdatedDataNode(EgDataNodeIDType nodeID);
    void DeleteDataNode(const EgDataNodeIDType delID);

    EgDataNodesContainer& operator << (EgDataNode* newNode);

    int  StoreToLocalFile();
    int  LoadAllLocalFileNodes();

    bool LoadLocalNodesEQ(const std::string& indexName, EgByteArrayAbstractType& fieldValue);

    int  LoadLocalNodesByOffsets(std::set<EgFileOffsetType>& index_offsets);
        // ======================== Debug ========================
    void PrintDataNodesContainer();
    void PrintNodesChain();
};