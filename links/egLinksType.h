#pragma once
#include "../metainfo/egDatabaseType.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

// class EgDataNodeLinksType;
// EgDataNodeType notFound;  // dummy data node for GUI if no data found

//  ============================================================================

class EgLinksType {
public:
    EgDataNodesType linksDataStorage;
    EgDataNodesMapType& dataMap; // shortcut to container nodes map,  for (auto iter : dataMap)
    // bool linksDataStorageLoaded {false};
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgBlueprintIDType   linkTypeID          { 0 };
    std::string         linkTypeName;

    EgDatabaseType*             metaInfoDatabase    { nullptr };
    // EgDataNodeBlueprintType*       linksStorageBlueprint  { nullptr };
    // EgDataNodesContainerType*   linksStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;

    EgDataNodesContainerType* fromDataNodes         { nullptr };
    EgDataNodesContainerType* toDataNodes           { nullptr };

    EgLinksType(): dataMap(linksDataStorage.dataMap) {}
/*
    EgLinksType(std::string a_Name): // , EgDatabaseType* a_Database):
            linkTypeName(a_Name),
            // metaInfoDatabase(a_Database),           
            linksStorageBlueprint(new EgDataNodeBlueprintType(a_Name)),
            linksStorage (new EgDataNodesContainerType(a_Name, linksStorageBlueprint)) { initLinkBlueprint(linksStorageBlueprint); }
*/
    ~EgLinksType() { /* clear();  delete linksStorage; delete linksStorageBlueprint; */ }
    
    void clear();

    int  ConnectLinks(std::string& linkNameStr, EgDatabaseType& myDB); // links have another register place in metainfo
    int  ConnectLinks(const char* linkName, EgDatabaseType& myDB)
        { std::string name(linkName); return ConnectLinks(name, myDB); } // wrapper

    // void initLinkBlueprint(EgDataNodeBlueprintType* linkBlueprint);
    // int  Connect(EgDatabaseType& a_Database); // To Database

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    int LoadLinks();
    int StoreLinks();

    int AddLinkPtrsToNodes(EgDataLinkIDType linkID, EgDataNodeType &from, EgDataNodeType &to);
    int ResolveNodesIDsToPtrs(EgDataNodesType &from, EgDataNodesType &to);
};

// ======================== Debug ========================

void PrintResolvedLinks(const EgDataNodeType& node);