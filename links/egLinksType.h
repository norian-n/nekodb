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
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgBlueprintIDType   linkBlueprintID         { 0 };
    std::string         linkTypeName;

    EgDataNodeBlueprintType*  dataNodeBlueprint {nullptr}; // storage shortcut
    EgDatabaseType*           metaInfoDatabase  { nullptr };
    EgDataNodesContainerType* fromDataNodes     { nullptr };
    EgDataNodesContainerType* toDataNodes       { nullptr };

    EgLinksType(): dataMap(linksDataStorage.dataMap) {}
    ~EgLinksType() { linksDataStorage.clear(); }
    
    void clear();

    int  ConnectLinks(std::string& linkNameStr, EgDatabaseType& myDB); // links have another register place in metainfo
    int  ConnectLinks(const char* linkName, EgDatabaseType& myDB)
        { std::string name(linkName); return ConnectLinks(name, myDB); } // wrapper

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    EgLinksType& operator << (EgDataNodeType* newNode) { linksDataStorage.AddDataNode(newNode); return *this; }
    EgLinksType& operator << (EgDataNodeType& newNode) { linksDataStorage.AddDataNode(newNode); return *this; }

    EgDataNodeIDType getAddedNodeID() {return linksDataStorage.getAddedNodeID();}

    int LoadLinks();
    int StoreLinks();

    int AddLinkPtrsToNodes(EgDataNodeType& link, EgDataNodeType& from, EgDataNodeType& to);
    int ResolveNodesIDsToPtrs(EgDataNodesType& from, EgDataNodesType& to);

    int  MarkUpdatedLink(EgDataNodeIDType linkNodeID) {return linksDataStorage.MarkUpdatedDataNode(linkNodeID);}
    void DeleteLink(EgDataLinkIDType linkID);
};

// ======================== Debug ========================

void PrintResolvedLinks(const EgDataNodeType& node);