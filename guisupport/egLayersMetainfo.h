#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"

class EgLayersMetainfo { // addons metadate
public:
    EgDataNodesSet layersSetsMetainfo;
    bool layersMetainfoLoaded     {false};

    EgDatabase*          metaInfoDatabase { nullptr };
    EgDataNodeBlueprint* layersBlueprint  { nullptr };

    EgLayersMetainfo(EgDatabase* a_metaInfoDatabase) : metaInfoDatabase(a_metaInfoDatabase) { ConnectOrCreateStorage(); }
    
    ~EgLayersMetainfo() {  clear(); delete layersBlueprint; }
    
    void clear();
    void ConnectOrCreateStorage();
    inline void CreateLayersBlueprints();

    int  CreateLayersSet(const std::string& layersSetName);
    void LoadLayersInfo();

    bool layersSetNameExists(const std::string& layersName);
    bool layersSetIDByName(const std::string& layersName, EgDataNodeIDType& layersID);
    bool topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID);

    void updateTopLayerID(const std::string& layersName, EgDataNodeIDType topLayerID);
};