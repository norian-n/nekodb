#pragma once
#include "egDatabaseType.h"

class EgOneLayerType;

class EgLayersType {
public:
    std::string                 layersTypeName;
    EgDatabaseType*             metaInfoDatabase { nullptr };
    EgDataNodesType             layersStorage;

    std::unordered_map < EgDataNodeIDType, EgOneLayerType* > layersMap;

    EgLayersType() {}

    ~EgLayersType() { clear(); }
    
    void clear();

    int  ConnectLayers(const std::string& a_layersTypeName, EgDatabaseType& a_Database);
    void AddLayerInfo(const std::string& nodesName, const std::string& linksName);
    // void AddNodesType(const std::string& nodesName, EgDataNodeIDType layerNum);
    // void AddLinksType(const std::string& linksName, EgDataNodeIDType layerNum);

    void getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, EgDataNodeIDType layerID);

    int LoadLayers();
    int StoreLayers();
    
    // graphLayers[layerID]
    EgOneLayerType* operator[](EgDataNodeIDType layerID);

    // void PrintLayersInfo();
};