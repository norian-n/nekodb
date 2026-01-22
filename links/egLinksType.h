#pragma once
#include "../metainfo/egDatabaseType.h"

class EgLinksType {
public:
    EgDataNodesType linksDataStorage;
    EgDataNodesMapType& dataMap; // shortcut to container nodes map,  for (auto iter : dataMap)
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgBlueprintIDType   linkBlueprintID         { 0 };
    std::string         linkTypeName;

    EgDataNodeBlueprintType*  dataNodeBlueprint { nullptr }; // storage shortcut
    EgDatabaseType*           metaInfoDatabase  { nullptr };
    EgDataNodesContainerType* fromDataNodes     { nullptr };
    EgDataNodesContainerType* toDataNodes       { nullptr };

    EgLinksType(): dataMap(linksDataStorage.dataMap) {}
    ~EgLinksType() { linksDataStorage.clear(); }
    
    void clear();

    int  ConnectLinks(const std::string& linkNameStr, EgDatabaseType& myDB); // check links in database metainfo

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    EgLinksType& operator << (EgDataNode* newNode) { linksDataStorage.AddDataNode(newNode); return *this; }
    EgLinksType& operator << (EgDataNode& newNode) { linksDataStorage.AddDataNode(newNode); return *this; }

    EgDataNodeIDType getAddedNodeID() {return linksDataStorage.getAddedNodeID();}

    int LoadLinks();
    int StoreLinks();

    int AddLinkPtrsToNodes(EgDataNode& link, EgDataNode& from, EgDataNode& to);
    int ResolveNodesIDsToPtrs(EgDataNodesType& from, EgDataNodesType& to);

    int  MarkUpdatedLink(EgDataNodeIDType linkNodeID) {return linksDataStorage.MarkUpdatedDataNode(linkNodeID);}
    
    void DeleteArrowLink(EgDataLinkIDType linkID);
    void DeleteInLink(EgDataLinkIDType linkID);    
    void DeleteOutLink(EgDataLinkIDType linkID);
};

void PrintResolvedLinks(const EgDataNode& node);