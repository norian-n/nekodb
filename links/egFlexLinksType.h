#pragma once
#include "../metainfo/egDatabase.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

// class EgDataNodeLinksType;
// EgDataNodeType notFound;  // dummy data node for GUI if no data found

//  ============================================================================

// egdb/meta/....

class EgFreeLinksType {
public:
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    // EgBlueprintIDType              linkTypeID          { 0 };
    EgBlueprintIDType           linkBlueprintID          { 0 };
    std::string                 linkTypeName;
    EgDatabase*             metaInfoDatabase    { nullptr };    
    EgDataNodeBlueprint*    linksStorageBlueprint  { nullptr };
    EgDataNodesContainer*   linksStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;

    EgFreeLinksType(std::string& a_Name, EgDatabase& a_Database):
            linkTypeName(a_Name),
             metaInfoDatabase(&a_Database),           
            linksStorageBlueprint(new EgDataNodeBlueprint(a_Name)),
            linksStorage (new EgDataNodesContainer()) { linksStorage->init(linksStorageBlueprint, linkTypeName); initFlexLinkBlueprint(linksStorageBlueprint); }

    ~EgFreeLinksType() { clear(); delete linksStorage; delete linksStorageBlueprint; }
    
    void clear();
    void initFlexLinkBlueprint(EgDataNodeBlueprint* linkBlueprint);
    void AddRawLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    void AddContainersLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    int LoadLinks();
    int StoreLinks();
    int ResolveNodesIDsToPtrs();
};

// ======================== Debug ========================

void PrintResolvedLinksFlex(const EgDataNode& node);
