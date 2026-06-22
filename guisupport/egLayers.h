#pragma once
#include "../metainfo/egDatabase.h"

class EgOneLayer;

class EgLayers {
public:
    std::string             layersTypeName;
    EgDatabase*             metaInfoDatabase { nullptr };
    EgDataNodesSet          layersStorage;

    std::unordered_map < EgDataNodeIDType, EgOneLayer* > layersMap;

    EgLayers() {}

    ~EgLayers() { clear(); }
    
    void clear();

    int  ConnectLayers(const std::string& a_layersTypeName, EgDatabase& a_Database);
    int  LoadLayers();
    int  StoreLayers();

    void updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H);
    void updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID);

    inline void addLayerData(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, 
        const std::string& nodesName, const std::string& linksName);

    void createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, 
        const std::string& nodesName, const std::string& linksName);
    void createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
        const std::string& parentNodesName, const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint);
    void createNewTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, const std::string& oldLayerName,
        const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint);
    // void AddNodesType(const std::string& nodesName, EgDataNodeIDType layerNum);
    // void AddLinksType(const std::string& linksName, EgDataNodeIDType layerNum);

    // void getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, EgDataNodeIDType layerID);

    EgOneLayer* operator[](EgDataNodeIDType layerID);

    // void PrintLayersInfo();
};