#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesType.h"

const std::string nodesTypesStorageName("egNodesTypesMetainfo");
const std::string linksTypesStorageName("egLinksTypesMetainfo");
const std::string layersStorageName    ("egLayersTypesMetainfo");

//  ============================================================================

class EgDatabaseType {
public:
    EgDataNodesType nodesTypesMetainfo;
    EgDataNodesType linksTypesMetainfo;
    EgDataNodesType layersMetainfo;
    bool nodesTypesMetainfoLoaded {false};
    bool linksTypesMetainfoLoaded {false};
    bool layersMetainfoLoaded     {false};    

    EgDataNodeBlueprintType* nodeBlueprint {nullptr};
    EgDataNodeBlueprintType* linkBlueprint {nullptr};
    EgDataNodeBlueprintType* layersBlueprint {nullptr};
    bool CreateNodeBlueprintFlag {false};
    bool CreateLinkBlueprintFlag {false};

    std::unordered_map <EgBlueprintIDType, EgDataNodesType*>  dataNodesTypes; // FIXME check if needed, use storage containers

    EgDatabaseType() { initDatabase(); }
    
    ~EgDatabaseType() { 
        clear(); 
        /*delete nodesTypesStorageBlueprint; delete linksTypesStorageBlueprint; delete nodesTypesStorage; delete linksTypesStorage; */
    }
    
    void clear();

    void initDatabase();
    void initNodesMetainfo();
    void initLinksMetainfo();
    void initLayersMetainfo();

    int InsertDataNodesTypeToMap(EgBlueprintIDType dntID, EgDataNodesType* dntPtr);
    EgDataNodesType* GetNodeTypePtrByID(EgBlueprintIDType nodeTypeID);
        // nodes metadata
    void AddDataNodesTypeInfo(EgBlueprintIDType& blueprintID, const std::string& typeName);
    int  LoadDataNodesTypesInfo();
    int  StoreDataNodesTypesInfo();
    bool nodeTypeIDByName(const std::string& typeName, EgBlueprintIDType& nodeTypeID);

    int  CreateNodeBlueprint(const std::string& blueprintName);
    void AddNodeDataField   (const std::string& fieldName);
    void AddIndex(const std::string& indexName, uint8_t indexSizeBytes = 4, uint8_t indexSubType = egIndexesSpace::egIntFT);
    void CommitNodeBlueprint();         // use AddNodeDataField() prior to commit
    void CommitSystemNodeBlueprint();   // dont add info to blueprints metadata storage (e.g. to create system storages)
        // links metadata
    void AddLinksTypeInfo(EgBlueprintIDType& blueprintID, const std::string& linksTypeName);
    int  LoadLinksInfo();
    int  StoreLinksInfo();
    bool linkTypeIDByName(const std::string& linkName, EgBlueprintIDType& linkTypeID);

    int  CreateLinkBlueprint(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo); // between 2 (1) node types only
    int  CreateLinkWithDataBlueprint(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo);
    void AddLinkDataField(const std::string& fieldName);
    void CommitLinkBlueprint(); // use AddLinkDataField() to add fields prior to commit

    int  CreateFreeLinkBlueprint(const std::string& linkTypeName) { /* FIXME TODO */ return 0; } // between any node types, 4 IDs per link
    int  CreateFreeLinkWithDataBlueprint(const std::string& linkTypeName) { /* FIXME TODO */ return 0; }
        // layers metadata
    void AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& linksTypeName);
    int  LoadLayersInfo();
    int  StoreLayersInfo();
    bool layersTypeIDByName(const std::string& layersName, EgBlueprintIDType& layersTypeID);

    int CreateLayersBlueprint(const std::string& layersTypeName);
    // FIXME TODO : delete nodes, links, layers blueprints
};