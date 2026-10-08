#pragma once
#include "egStdLayers.h"
#include "../links/egFreeLinksSet.h"

class EgOneLayer { // used by egLayers to store nodes and links of one layer
public:
    EgStdLayers*        layers        { nullptr };
    EgDataNodeIDType layerID       { 0 };
    EgDataNodeIDType parentLayerID { 0 };
    uint32_t         layerWidth    {0}; // load from layers storage
    uint32_t         layerHeight   {0};

    EgFreeLinksSet   darkNodesLinks;

    std::unordered_map<std::string, EgDataNodesSet*> nodesMap;
    std::unordered_map<std::string, EgLinksSet*>     linksMap;

    EgOneLayer() = delete;
    EgOneLayer(EgStdLayers& a_Layers, EgDataNodeIDType a_layerID, EgDataNodeIDType a_parentLayerID);

    ~EgOneLayer() { clear(); }
    
    void clear();

    // void setNodesInfo (std::string nodesName, serialLoadFunctionType loadNodeFunc, serialStoreFunctionType storeNodeFunc);

    void getLayerNodes(EgDataNodesSet*& graphNodes, serialLoadFunctionType loadNodeObjectFromDb, serialStoreFunctionType storeNodeObjectToDb);
    void getLayerLinks(EgLinksSet*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, serialStoreFunctionType storeLinkObjectToDb);
    // void getLayerDarkNodesLinks();

    void getDarkNodeID(EgDataNodeIDType widgetID, EgDataNodeIDType& darkNodeID); // FIXME add set name

    void addNodesSetToMap(const std::string& nodesName, EgDatabase* graphDB);
    void addLinksSetToMap(const std::string& linksName, EgDatabase* graphDB);

    // EgDataNodesType* getNextNodesType() { return nullptr; }

    // void PrintLayersInfo();
};