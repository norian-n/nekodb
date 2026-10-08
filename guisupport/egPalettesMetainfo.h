#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"

class EgPalettesMetainfo { // addons metadate
public:
    EgDataNodesSet palettesMetainfo;
    bool           palettesMetainfoLoaded  { false };

    EgDatabase*          metaInfoDatabase  { nullptr };
    // EgDataNodeBlueprint* palettesBlueprint { nullptr };

    EgPalettesMetainfo(EgDatabase* a_metaInfoDatabase) : metaInfoDatabase(a_metaInfoDatabase) { getPalettesMetainfo(); }
    
    ~EgPalettesMetainfo() { clear(); }
    
    void clear();
    void getPalettesMetainfo();

    inline void createPalettesMetainfo();
    int  CreatePalettesSet(const std::string& paletteName);

    inline void AddPalettesToMetainfo(const std::string& palettesSetName); // EgBlueprintIDType& blueprintID, 
    void LoadPalettesInfo();

    bool palettesSetExists(const std::string& layersName); // , EgDataNodeIDType& layersTypeID
};