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

    // bool nodesTypesMetainfoLoaded {false};  // metainfo status flags
    // bool linksTypesMetainfoLoaded {false};

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
    inline void createStdLinkBlueprints();
    // inline void createLayersMetainfo();

// ======================== nodes metadata API ========================
    // int  CreateNodesSet(const std::string& nodesSetName); // blueprint and set at once
    // void CommitNodesSet();
    bool CreateNodeBlueprint(const std::string& blueprintName); // blueprint only, not set
    void AddNodeDataField   (const std::string& fieldName);
    void AddIndex(const std::string& indexName, uint8_t indexSizeBytes = 4, uint8_t indexSubType = egIndexesSpace::egIntFT);
    void CommitNodeBlueprint();         // use AddNodeDataField() prior to commit
    int  CreateNodesSetByBlueprint(const std::string& nodesSetName, const std::string& nodeBlueprintName);
// ======================== links metadata API ========================
    int  CreateLinkBlueprint(const std::string& linksBlueprintName);
    void AddLinkDataField(const std::string& fieldName);
    void CommitBoundLinkBlueprint(); // use AddLinkDataField() to add fields prior to commit
    void CommitFreeLinkBlueprint(); // use AddLinkDataField() to add fields prior to commit

    int  CreateFreeLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName);
    int  CreateBoundLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName,
        const std::string& nodesFrom, const std::string& nodesTo);
    int CreateStdBoundLinkSet(const std::string& linksSetName, const std::string& nodesFrom, const std::string& nodesTo);
// ======================== layers metadata API ========================
    void ConnectLayersMetainfo() { if (! layersMetainfo) layersMetainfo = new EgLayersMetainfo(this); }
    int  CreateLayersSet(const std::string& layersTypeName) { ConnectLayersMetainfo(); return layersMetainfo-> CreateLayersSet(layersTypeName); }
    bool topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID) { ConnectLayersMetainfo(); return layersMetainfo-> topLayerIDByName(layersName, layersTopID); }
// ======================== palettes metadata API ========================
    void ConnectPalettesMetainfo() { if (! palettesMetainfo) palettesMetainfo = new EgPalettesMetainfo(this); }
    int  CreatePalettesSet(const std::string& paletteName) { ConnectPalettesMetainfo(); return palettesMetainfo-> CreatePalettesSet(paletteName); }
// ======================== blueprints support funcs ========================
    int  CreateBlueprintByTemplate(const std::string& oldBPName, const std::string& newBPName, std::unordered_set<std::string>& addFields); // blueprint for guisupport

// nodes metadata internal funcs
    // int  AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName, const std::string& glamBPName);
    int  AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName);
    void LoadDataNodesSetsInfo();
    // int  StoreDataNodesTypesInfo();
    bool nodeTypeIDByName(const std::string& typeName, EgNodesSetIDType& nodeTypeID);
    EgDataNode* dataNodePtrByNodesType(EgDataNodesSet* nodesType);    

    bool blueprintNameByNodeName(const std::string& nodesSetName, std::string& nodeBlueprintName);
    EgDataNodesSet* GetNodesTypePtrByID(EgNodesSetIDType nodeTypeID);
// links metadata internal funcs
    int  AddLinksTypeInfo(const std::string& linksSetName, const std::string& blueprintName, const std::string& nodesFrom, const std::string& nodesTo);
    void LoadLinksInfo();
    bool linkTypeIDByName(const std::string& linkName, EgNodesSetIDType& linkTypeID);
    bool blueprintNameByLinkName(const std::string& nodesSetName, std::string& nodeBlueprintName, EgNodesSetIDType& linksSetID);

    inline bool LoadLocalFileNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& newNode);
    bool CherryPickNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& newNode, EgDataNodeBlueprint*& newNodeBP); // MEMLEAK delete after use
    
    // FIXME TODO : delete nodes, links, layers metadata
};