#pragma once
#include "../metainfo/egDatabase.h"

class EgOneLayer;

class EgStdLayers { // layers set
public:
    std::string             layersSetName;
    EgDatabase*             metaInfoDatabase { nullptr };
    EgDataNodesSet          layersSetStorage;

    std::unordered_map < EgDataNodeIDType, EgOneLayer* > layersMap; // container of one layer data

    EgStdLayers() {}

    ~EgStdLayers() { clear(); }

    void clear();

    void createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
        const std::string& nodesName, const std::string& linksName);
    void createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
        const std::string& parentNodesName, const std::string& parentLinksName);
    void replaceTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, 
        const std::string& oldNodesName, const std::string& oldLinksName);

    inline void addLayerData(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
        const std::string& nodesName, const std::string& linksName);

    int  ConnectLayers(const std::string& a_layersSetName, EgDatabase& a_Database);
    int  LoadLayers();
    int  StoreLayers();

    void updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H);
    void updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID);

    EgOneLayer* operator[](EgDataNodeIDType layerID);

    // void PrintLayersInfo();
};