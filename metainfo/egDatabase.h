#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"
#include "../guisupport/egLayersMetainfo.h"
#include "../guisupport/egPalettesMetainfo.h"

class EgDatabase { // metadata of nodes sets, links sets, layers sets
public:
    EgDataNodesSet nodesTypesMetainfo;      // metadata stored in system nodes sets
    EgDataNodesSet linksTypesMetainfo;

    EgLayersMetainfo*   layersMetainfo   {nullptr};
    EgPalettesMetainfo* palettesMetainfo {nullptr};

    bool nodesTypesMetainfoLoaded {false};  // metainfo status flags
    bool linksTypesMetainfoLoaded {false};

    bool CreateNodeBlueprintFlag  {false};   // create status flags
    bool CreateLinkBlueprintFlag  {false};
    EgDataNodeBlueprint* nodeBlueprint {nullptr}; // blueprint pointers for create ops
    EgDataNodeBlueprint* linkBlueprint {nullptr};

    EgDatabase();
    
    ~EgDatabase() { 
        clear();
        delete layersMetainfo; delete nodeBlueprint; delete linkBlueprint;
        /*delete nodesTypesStorageBlueprint; delete linksTypesStorageBlueprint; delete nodesTypesStorage; delete linksTypesStorage; */
    }
    
    void clear();
    void initDatabase();
    // inline void createSystemStorage(const std::string& storName, const std::string& fieldName, const std::string& fieldBP);
    inline void createNodesMetainfo();
    inline void createLinksMetainfo();
    // inline void createLayersMetainfo();

// ======================== nodes metadata API ========================
    int  CreateNodesSet(const std::string& nodesSetName); // blueprint and set at once
    void AddNodeDataField   (const std::string& fieldName);
    void AddIndex(const std::string& indexName, uint8_t indexSizeBytes = 4, uint8_t indexSubType = egIndexesSpace::egIntFT);
    void CommitNodesSet();

    int  CreateNodeBlueprint(const std::string& blueprintName); // blueprint only, not set
    void CommitNodeBlueprint();         // use AddNodeDataField() prior to commit
    void CommitSystemNodeBlueprint();   // dont add info to blueprints metadata storage (e.g. to create system storages from scratch)
    int  CreateNodesSetByBlueprint(const std::string& nodesSetName, const std::string& nodeBlueprintName);
// ======================== links metadata API ========================
    int  CreateSimpleLinkSet(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo); // between 2 (1) node types only
    int  CreateLinkBlueprint(const std::string& linkTypeName);
    void AddLinkDataField(const std::string& fieldName);
    void CommitLinkBlueprint(); // use AddLinkDataField() to add fields prior to commit
    // void CommitFreeLinkBlueprint(); // FIXME TODO

    int  CreateLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName, const std::string& nodesFrom, const std::string& nodesTo);  // FIXME TODO STUB    

    int  CreateFreeLinkBlueprint(const std::string& linkTypeName) { /* FIXME TODO STUB */ return 0; } // between any node types, 4 IDs per link
    int  CreateFreeLinkWithDataBlueprint(const std::string& linkTypeName) { /* FIXME TODO STUB */ return 0; }
// ======================== layers metadata API ========================
    void ConnectLayersMetainfo() { if (! layersMetainfo) layersMetainfo = new EgLayersMetainfo(this); }
    int  CreateLayersSet(const std::string& layersTypeName) { ConnectLayersMetainfo(); return layersMetainfo-> CreateLayersSet(layersTypeName); }
    bool topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID) { ConnectLayersMetainfo(); return layersMetainfo-> topLayerIDByName(layersName, layersTopID); }
// ======================== palettes metadata API ========================
    void ConnectPalettesMetainfo() { if (! palettesMetainfo) palettesMetainfo = new EgPalettesMetainfo(this); }
    int  CreatePalettesSet(const std::string& paletteName) { ConnectPalettesMetainfo(); return palettesMetainfo-> CreatePalettesSet(paletteName); }
    // ======================== gui support funcs ========================
    int  CreateBlueprintByTemplate(const std::string& oldBPName, const std::string& newBPName, std::unordered_set<std::string>& addFields); // blueprint for guisupport

// nodes metadata internal funcs
    int  AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName, const std::string& glamBPName);
    int  AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName);
    void LoadDataNodesSetsInfo();
    // int  StoreDataNodesTypesInfo();
    bool nodeTypeIDByName(const std::string& typeName, EgBlueprintIDType& nodeTypeID);
    EgDataNode* dataNodePtrByNodesType(EgDataNodesSet* nodesType);    

    bool blueprintNameByNodeName(const std::string& nodesSetName, std::string& nodeBlueprintName);
    EgDataNodesSet* GetNodesTypePtrByID(EgBlueprintIDType nodeTypeID);
// links metadata internal funcs
    int  AddLinksTypeInfo(const std::string& linksSetName, const std::string& blueprintName, const std::string& nodesFrom, const std::string& nodesTo);
    void LoadLinksInfo();
    bool linkTypeIDByName(const std::string& linkName, EgBlueprintIDType& linkTypeID);
    bool blueprintNameByLinkName(const std::string& nodesSetName, std::string& nodeBlueprintName);

    inline int LoadLocalFileNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& newNode);
    int  CherryPickNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& newNode, EgDataNodeBlueprint*& newNodeBP); // MEMLEAK delete after use
    
    // FIXME TODO : delete nodes, links, layers metadata
};