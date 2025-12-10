#pragma once
#include <iostream>

#include "../nodes/egDataNodesType.h"

//  ============================================================================
    const std::string nodesTypesStorageName("nodesTypesStorageBlueprint");
    const std::string linksTypesStorageName("linksTypesStorageBlueprint");
    const std::string layersStorageName    ("layersStorageBlueprint");
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

    std::map <EgBlueprintIDType, EgDataNodesType*>  dataNodesTypes; // FIXME check if needed, use storage containers

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

    void AddDataNodesTypeInfo(EgBlueprintIDType& blueprintID, std::string& typeName);
    void AddLinksTypeInfo(EgBlueprintIDType& blueprintID, std::string& linksTypeName);
    void AddLayersTypeInfo(EgBlueprintIDType& blueprintID, std::string& linksTypeName);
    int  LoadTypesInfo();
    int  StoreTypesInfo();
    int  LoadLinksInfo();
    int  StoreLinksInfo();
    int  LoadLayersInfo();
    int  StoreLayersInfo();

    bool nodeTypeIDByName(std::string& typeName, EgBlueprintIDType& nodeTypeID);
    bool linkTypeIDByName(std::string& linkName, EgBlueprintIDType& linkTypeID);
    bool layersTypeIDByName(std::string& layersName, EgBlueprintIDType& layersTypeID);
//  ============================================================================    
    int  CreateNodeBlueprint (std::string& name) { return CreateNodeBlueprint(name.c_str());  }   // wrapper
    int  CreateNodeBlueprint (const std::string& name) { return CreateNodeBlueprint(name.c_str()); } // wrapper
    int  CreateNodeBlueprint (const char* name);
    void AddNodeDataField(std::string& fieldName);
    void AddNodeDataField(const char* fieldName) { std::string name(fieldName); return AddNodeDataField(name); } // wrapper
    void AddIndex(std::string indexName, uint8_t indexSizeBytes = 4, uint8_t indexSubType = egIndexesSpace::egIntFT);

    // void AddIndex(const char*  indexName) { std::string name(indexName); return AddIndex(name); } // wrapper    
    void CommitNodeBlueprint();         // use AddDataField() prior to commit
    void CommitSystemNodeBlueprint();   // use AddDataField() prior to commit
//  ============================================================================
    int  CreateLinkBlueprint(std::string& linkTypeName, std::string& nodesFrom, std::string& nodesTo); // between 2 (1) node types only
    int  CreateLinkBlueprint(const char* name, const char* nodesFrom, const char* nodesTo) {
        std::string nameStr(name); std::string nodeFStr(nodesFrom); std::string nodeTStr(nodesTo); 
        return CreateLinkBlueprint(nameStr, nodeFStr, nodeTStr);  } // wrapper

    int  CreateLinkWithDataBlueprint(std::string& linkTypeName, std::string& nodesFrom, std::string& nodesTo);
    int  CreateLinkWithDataBlueprint(const char* name, const char* nodesFrom, const char* nodesTo) {
        std::string nameStr(name); std::string nodeFStr(nodesFrom); std::string nodeTStr(nodesTo); 
        return CreateLinkWithDataBlueprint(nameStr, nodeFStr, nodeTStr);  } // wrapper
    void AddLinkDataField(std::string& fieldName);
    void AddLinkDataField(const char* typeName) { std::string name(typeName); return AddLinkDataField(name); } // wrapper
    void CommitLinkBlueprint(); // use AddLinkField() prior to commit
//  ============================================================================
    int  CreateFreeLinkBlueprint() { /* FIXME TODO */ return 0; } // between any node types, 4 IDs per link

    int  CreateFreeLinkWithDataBlueprint(std::string& linkTypeName) { /* FIXME TODO */ return 0; }
    int  CreateFreeLinkWithDataBlueprint(const char* linkTypeName) 
        { std::string name(linkTypeName); return CreateFreeLinkWithDataBlueprint(name); } // wrapper
//  ============================================================================
    int CreateLayersBlueprint(std::string& layersTypeName);

    // FIXME TODO delete nodes, links, layers blueprint
};