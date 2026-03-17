#pragma once
#include "../metainfo/egDatabase.h"

class EgLinksSet {
public:
    EgDataNodesSet linksDataStorage;
    EgDataNodesMapType& dataMap; // shortcut to container nodes map,  for (auto iter : dataMap)
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgBlueprintIDType   linkBlueprintID         { 0 };
    std::string         linkNameShort; // for API and metainfo
    std::string         linkNameFull;  // storage & BP name, + literal linkSuffixName "_egArrowLink"
    std::string         linkBlueprintName;
    std::string         linkBlueprintNameFull;

    EgDataNodeBlueprint*  dataNodeBlueprint { nullptr }; // storage shortcut
    EgDatabase*           metaInfoDatabase  { nullptr };
    EgDataNodesContainer* fromDataNodes     { nullptr };
    EgDataNodesContainer* toDataNodes       { nullptr };

    EgLinksSet(): dataMap(linksDataStorage.dataMap) {}
    ~EgLinksSet() { linksDataStorage.clear(); }
    
    void clear();

    int  ConnectLinks(const std::string& linkNameStr, EgDatabase& myDB); // check links in database metainfo

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    EgLinksSet& operator << (EgDataNode* newNode) { linksDataStorage.AddDataNode(newNode); return *this; }
    EgLinksSet& operator << (EgDataNode& newNode) { linksDataStorage.AddDataNode(newNode); return *this; }

    EgDataNodeIDType getAddedNodeID() {return linksDataStorage.getAddedNodeID();}

    int LoadLinks();
    int StoreLinks();

    int AddLinkPtrsToNodes(EgDataNode& link, EgDataNode& from, EgDataNode& to);
    int ResolveNodesIDsToPtrs(EgDataNodesSet& from, EgDataNodesSet& to);

    int  MarkUpdatedLink(EgDataNodeIDType linkNodeID) {return linksDataStorage.MarkUpdatedDataNode(linkNodeID);}
    
    void DeleteArrowLink(EgDataLinkIDType linkID);
    void DeleteInLink(EgDataLinkIDType linkID);    
    void DeleteOutLink(EgDataLinkIDType linkID);
};

void PrintResolvedLinks(const EgDataNode& node);