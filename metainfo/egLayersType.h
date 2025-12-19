#pragma once
#include "egDatabaseType.h"

class EgLayersType {
public:
    std::string                 layersTypeName;
    EgDatabaseType*             metaInfoDatabase        { nullptr };
    EgDataNodeBlueprintType*    layersStorageBlueprint   { nullptr };
    EgDataNodesType             layersStorage;
    // EgDataNodesContainerType* linkDataStorage;

    EgLayersType() {}

    ~EgLayersType() { clear(); }
    
    void clear();

    int  ConnectLayers(const std::string& a_layersTypeName, EgDatabaseType& a_Database);

    void AddNodesType(const std::string& nodesName, EgLayerNumType layerNum);
    void AddLinksType(const std::string& linksName, EgLayerNumType layerNum);

    void getLayerNodesAndLinks(std::set<std::string>& nodesNames, std::set<std::string>& linksNames, EgLayerNumType layerNum);

    int LoadLayers();
    int StoreLayers();

    // void PrintLayersInfo();
};