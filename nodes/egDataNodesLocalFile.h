#pragma once
#include "egDataNode.h"
#include "egDataNodeBlueprint.h"
#include "../indexes/egIndexConditions.h"

struct EgLocalFileHeader {
        EgDataNodeIDType lastID;
        EgDataNodeIDType nodesCount;
        EgFileOffsetType firstNode;
        EgFileOffsetType lastNode;
};

const int lastIDOffset          {0};
const int nodesCountOffset      {sizeof(EgDataNodeIDType)};
const int firstNodePtrOffset    {sizeof(EgDataNodeIDType)*2};
const int lastNodePtrOffset     {sizeof(EgDataNodeIDType)*2 + sizeof(EgFileOffsetType)};
const int nodesDataOffset       {sizeof(EgDataNodeIDType)*2 + sizeof(EgFileOffsetType)*2};

class EgDataNodesLocalFileType
{
public:
    EgFileType nodesFile;
    bool localTransactOk {false};
    EgLocalFileHeader nodesFileHeader;
    EgDataNodeBlueprintType* dataNodeBlueprint   { nullptr };    // blueprint == class == type of data nodes    

    std::map < std::string, EgIndexesAbstractType* >  localIndexes;
    EgIndexConditionsTree* index_tree       {nullptr};              // indexed fields operations    

    EgDataNodesLocalFileType() {}
    ~EgDataNodesLocalFileType() {}

    void initIndexes();

    bool InitFile(std::string& layoutName);
    bool GetLastID(std::string& layoutName, EgDataNodeIDType& lastID);

    inline bool ReadHeader();    // from opened nodesFile to beforeHeader
    bool WriteHeader();   // from afterHeader to opened nodesFile
    inline void InitNewHeader(); // new beforeHeader
    void PrintHeader();

    bool OpenFileToRead(std::string& layoutName);
    bool OpenFileToUpdate(std::string& layoutName);
    bool StartFileUpdate(std::string& layoutName);
    bool UpdateNodesFile(std::string& layoutName, 
        EgDataNodesMapType& addedNodes, EgDataNodesMapType& deletedNodes, EgDataNodesMapType& updatedNodes);

    bool getFirstNodeOffset(EgFileOffsetType& theOffset);
    bool getLastNodeOffset(EgFileOffsetType& theOffset);

    bool WriteDataNode(EgDataNodeType* theNode);
    bool ReadDataNode(EgDataNodeType* theNode, EgFileOffsetType& nextOffset);
    bool DeleteDataNode(EgDataNodeType* theNode);

    // bool AddLocalIndex(std::string& indexName, EgDataNodeType* theNode);

    void PrintNodesChain();
};

// ======================== Debug ========================

void ReadDataNodeAndOffsets(EgFileType &theNodesFile, EgDataNodeType *theNode);