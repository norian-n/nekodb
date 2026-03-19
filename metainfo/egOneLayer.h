#pragma once
#include "egLayers.h"

/* struct nodesSetInfo {
    // std::string      nodesName; --> map.first
    EgDataNodesType* nodesSetPtr;
    serialLoadFunctionType  loadNodeFromDb;
    serialStoreFunctionType storeNodeToDb;
}; */

class EgOneLayer {
public:
    EgLayers*        layers        { nullptr };
    EgDataNodeIDType layerID       { 0 };
    EgDataNodeIDType parentLayerID { 0 };

    uint32_t        layerWidth     {0}; // load from layers storage
    uint32_t        layerHeight    {0};

    // std::unordered_map < std::string, nodesSetInfo > nodesMap;

    std::unordered_map<std::string, EgDataNodesSet*> nodesMap;
    std::unordered_map<std::string, EgLinksSet*>     linksMap;

    EgOneLayer() = delete;
    EgOneLayer(EgLayers& a_Layers, EgDataNodeIDType a_layerID, EgDataNodeIDType a_parentLayerID);

    ~EgOneLayer() { clear(); }
    
    void clear();

    void setNodesInfo (std::string nodesName, serialLoadFunctionType loadNodeFunc, serialStoreFunctionType storeNodeFunc);

    // typedef void (*serialLoadFunctionType)  (EgDataNode& dataNode);
    // typedef void (*serialStoreFunctionType) (EgDataNode& dataNode);
    void getLayerNodes(EgDataNodesSet*& graphNodes, serialLoadFunctionType loadNodeObjectFromDb, serialStoreFunctionType storeNodeObjectToDb);
    void getLayerLinks(EgLinksSet*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, serialStoreFunctionType storeLinkObjectToDb);    

    void connectNodeType(const std::string& nodesName, EgDatabase* graphDB);
    void connectLinkType(const std::string& linksName, EgDatabase* graphDB);

    // EgDataNodesType* getNextNodesType() { return nullptr; }

    // void PrintLayersInfo();
};