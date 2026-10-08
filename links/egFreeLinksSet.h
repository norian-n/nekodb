#pragma once
#include "../metainfo/egDatabase.h"

class EgFreeLinksSet { // any nodes set to any other link
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

    EgFreeLinksSet(): dataMap(linksDataStorage.dataMap) {}
    ~EgFreeLinksSet() { linksDataStorage.clear(); }
    
    void clear();

    int  ConnectLinks(const std::string& linkNameStr, EgDatabase& myDB); // check links in database metainfo

    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID, const std::string& fromSetName, const std::string& toSetName);
    // int  AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);

    EgFreeLinksSet& operator << (EgDataNode* newNode) { linksDataStorage.AddDataNode(newNode); return *this; }
    EgFreeLinksSet& operator << (EgDataNode& newNode) { linksDataStorage.AddDataNode(newNode); return *this; }

    EgDataNodeIDType getAddedNodeID() {return linksDataStorage.getAddedNodeID();}

    int LoadLinks();
    int StoreLinks();

    bool MarkUpdatedLink(EgDataNodeIDType linkNodeID) { return linksDataStorage.MarkUpdatedDataNode(linkNodeID); }
    
    void DeleteArrowLink(EgDataLinkIDType linkID);
    void DeleteInLink(EgDataLinkIDType linkID);    
    void DeleteOutLink(EgDataLinkIDType linkID);

    // int AddLinkPtrsToNodes(EgDataNode& link, EgDataNode& from, EgDataNode& to);
    // int ResolveNodesIDsToPtrs(EgDataNodesSet& from, EgDataNodesSet& to);
    // int ResolveNodesIDsToPtrs2();
};

// ======================== Debug ========================

void PrintResolvedLinksFlex(const EgDataNode& node);
