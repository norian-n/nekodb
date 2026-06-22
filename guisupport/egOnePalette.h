#pragma once
#include "../metainfo/egDatabase.h"

class EgOnePalette {
public:
    std::string             paletteTypeName;
    EgDatabase*             metaInfoDatabase { nullptr };
    EgDataNodesSet          paletteStorage;

    // std::unordered_map<std::string, EgDataNodesSet*> paletteMap;

    // EgDataNodesMapType dataNodes;

    EgOnePalette() {}

    ~EgOnePalette() { clear(); }
    
    void clear();

    int  ConnectPalette(const std::string& a_paletteTypeName, EgDatabase& a_Database);
    int  LoadPalette();
    int  StorePalette();

    // void updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H);
    // void updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID);

    void addPaletteWidgetData(const std::string& nodesSetName, const std::string& widgetName, EgDataNodeIDType nodeID, uint32_t paletteIndex, uint32_t paletteFill);

    /* void createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, 
        const std::string& nodesName, const std::string& linksName);
    void createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
        const std::string& parentNodesName, const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint);
    void createNewTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, const std::string& oldLayerName,
        const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint); */

    // void AddNodesType(const std::string& nodesName, EgDataNodeIDType layerNum);
    // void AddLinksType(const std::string& linksName, EgDataNodeIDType layerNum);

    // void getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, EgDataNodeIDType layerID);

    // EgOneLayer* operator[](EgDataNodeIDType layerID);

    // void PrintPaletteInfo();
};