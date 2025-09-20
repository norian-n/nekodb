#pragma once
#include "../metainfo/egDatabaseType.h"

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
    EgBlueprintIDType              linkTypeID          { 0 };
    std::string                 linkTypeName;
    EgDatabaseType*             metaInfoDatabase    { nullptr };    
    EgDataNodeBlueprintType*       linksStorageBlueprint  { nullptr };
    EgDataNodesContainerType*   linksStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;

    EgFreeLinksType(std::string& a_Name, EgDatabaseType& a_Database):
            linkTypeName(a_Name),
             metaInfoDatabase(&a_Database),           
            linksStorageBlueprint(new EgDataNodeBlueprintType(a_Name)),
            linksStorage (new EgDataNodesContainerType()) { linksStorage->init(linksStorageBlueprint); initFlexLinkBlueprint(linksStorageBlueprint); }

    ~EgFreeLinksType() { clear(); delete linksStorage; delete linksStorageBlueprint; }
    
    void clear();
    void initFlexLinkBlueprint(EgDataNodeBlueprintType* linkBlueprint);
    void AddRawLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    void AddContainersLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    int LoadLinks();
    int StoreLinks();
    int ResolveNodesIDsToPtrs();
};

// ======================== Debug ========================

void PrintResolvedLinksFlex(const EgDataNodeType& node);
