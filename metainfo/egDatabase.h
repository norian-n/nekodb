#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"

class EgDatabase { // metadata of nodes sets, links sets, layers sets
public:
    EgDataNodesSet nodesTypesMetainfo;      // metadata stored in system nodes sets
    EgDataNodesSet linksTypesMetainfo;
    EgDataNodesSet layersMetainfo;
    bool nodesTypesMetainfoLoaded {false};  // metainfo status flags
    bool linksTypesMetainfoLoaded {false};
    bool layersMetainfoLoaded     {false};

    bool CreateNodeBlueprintFlag {false};   // create status flags
    bool CreateLinkBlueprintFlag {false};
    EgDataNodeBlueprint* nodeBlueprint {nullptr}; // blueprint pointers for create ops
    EgDataNodeBlueprint* linkBlueprint {nullptr};
    EgDataNodeBlueprint* layersBlueprint {nullptr};

    EgDatabase() { initDatabase(); }
    
    ~EgDatabase() { 
        clear(); 
        /*delete nodesTypesStorageBlueprint; delete linksTypesStorageBlueprint; delete nodesTypesStorage; delete linksTypesStorage; */
    }
    
    void clear();
    void initDatabase();
    inline void createSystemStorage(const std::string& storName, const std::string& fieldName, const std::string& fieldBP);
    inline void createLinksMetainfo();
    inline void createLayersMetainfo();

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
    int  CreateLinkWithDataBlueprint(const std::string& linkTypeName);
    void AddLinkDataField(const std::string& fieldName);
    void CommitLinkBlueprint(); // use AddLinkDataField() to add fields prior to commit

    int  CreateLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName, const std::string& nodesFrom, const std::string& nodesTo);  // FIXME TODO STUB    

    int  CreateFreeLinkBlueprint(const std::string& linkTypeName) { /* FIXME TODO STUB */ return 0; } // between any node types, 4 IDs per link
    int  CreateFreeLinkWithDataBlueprint(const std::string& linkTypeName) { /* FIXME TODO STUB */ return 0; }
// ======================== layers metadata API ========================
    void AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& linksTypeName);
    void LoadLayersInfo();
    bool layersTypeIDByName(const std::string& layersName, EgBlueprintIDType& layersTypeID);

    int  CreateLayersSet(const std::string& layersTypeName);

// nodes metadata internal funcs
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
    
    // FIXME TODO : delete nodes, links, layers metadata
};