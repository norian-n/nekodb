#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"

class EgLayersMetainfo { // addons metadate
public:
    EgDataNodesSet layersMetainfo;
    bool layersMetainfoLoaded     {false};

    EgDatabase*          metaInfoDatabase { nullptr };
    EgDataNodeBlueprint* layersBlueprint  { nullptr };

    EgLayersMetainfo(EgDatabase* a_metaInfoDatabase) : metaInfoDatabase(a_metaInfoDatabase) { initDatabase(); }
    
    ~EgLayersMetainfo() {  clear(); delete layersBlueprint; }
    
    void clear();
    void initDatabase();
    inline void createLayersMetainfo();

    void AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& linksTypeName);
    void LoadLayersInfo();

    bool layersTypeIDByName(const std::string& layersName, EgDataNodeIDType& layersTypeID);
    bool topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID);

    void updateTopLayerID(const std::string& layersName, EgDataNodeIDType topLayerID);
    int  CreateLayersSet(const std::string& layersTypeName);
};