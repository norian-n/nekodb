#pragma once
#include "../metainfo/egDatabase.h"

class EgLinksSet {
public:
    EgNodesSetIDType    linksSetID          { 0 };
    std::string         linkNameShort; // for API and metainfo
    std::string         linkNameFull;  // storage file name, short + literal linkSuffixName "_egArrowLink"
    std::string         linkBlueprintName;
    std::string         linkBlueprintNameFull;

    EgDataNodesSet linksDataStorage;
    EgDataNodesMapType& dataMap; // shortcut to storage container nodes map,  for (auto iter : dataMap)

    EgDataNodeBlueprint*  dataNodeBlueprint { nullptr };
    EgDatabase*           metaInfoDatabase  { nullptr };
    EgDataNodesContainer* fromDataNodes     { nullptr }; // dynamic direct pointers
    EgDataNodesContainer* toDataNodes       { nullptr };

    EgLinksSet(): dataMap(linksDataStorage.dataMap) {}
    ~EgLinksSet() { linksDataStorage.clear(); }
    
    void clear();

    int  ConnectLinks(const std::string& linkNameStr, EgDatabase& myDB); // check links in database metainfo
    int  ConnectLayerLinks(const std::string& linkNameStr);

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    EgLinksSet& operator << (EgDataNode* newNode) { linksDataStorage.AddDataNode(newNode); return *this; }
    EgLinksSet& operator << (EgDataNode& newNode) { linksDataStorage.AddDataNode(newNode); return *this; }

    EgDataNodeIDType getAddedNodeID() {return linksDataStorage.getAddedNodeID();}

    int LoadLinks();
    int StoreLinks();

    int AddLinkPtrsToNodes(EgDataNode& link, EgDataNode& from, EgDataNode& to);
    int ResolveNodesIDsToPtrs(EgDataNodesSet& from, EgDataNodesSet& to);

    bool MarkUpdatedLink(EgDataNodeIDType linkNodeID) { return linksDataStorage.MarkUpdatedDataNode(linkNodeID); }
    
    void DeleteArrowLink(EgDataLinkIDType linkID);
    void DeleteInLink(EgDataLinkIDType linkID);    
    void DeleteOutLink(EgDataLinkIDType linkID);
};

void PrintResolvedLinks(const EgDataNode& node);