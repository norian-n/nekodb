#pragma once
#include "egLayersType.h"

class EgOneLayerType {
public:
    EgLayersType* layers              { nullptr };
    EgDataNodeIDType  layerID         { 0 };
    // EgBlueprintIDType nodeBlueprintID { 0 };
    // EgDataNodeIDType  nodeID          { 0 };
    // EgDataNodesType*  graphNodes { nullptr };
     
    std::unordered_map<std::string, EgDataNodesType*> nodesMap;
    std::unordered_map<std::string, EgLinksType*>     linksMap;

    EgOneLayerType() = delete;
    EgOneLayerType(EgLayersType& a_Layers, EgDataNodeIDType a_layerID);

    ~EgOneLayerType() { clear(); }
    
    void clear();

    // typedef void (*serialLoadFunctionType)  (EgDataNode& dataNode);
    // typedef void (*serialStoreFunctionType) (EgDataNode& dataNode);
    void getLayerNodes(EgDataNodesType*& graphNodes, serialLoadFunctionType loadNodeObjectFromDb, serialStoreFunctionType storeNodeObjectToDb);
    void getLayerLinks(EgLinksType*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, serialStoreFunctionType storeLinkObjectToDb);    

    void addNodeType(const std::string& nodesName, EgDatabaseType* graphDB);
    void addLinkType(const std::string& linksName, EgDatabaseType* graphDB);

    // EgDataNodesType* getNextNodesType() { return nullptr; }

    // void PrintLayersInfo();
};