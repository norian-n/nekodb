#pragma once
#include <iostream>
#include <unordered_map>

#include "../nodes/egDataNodesSet.h"

class EgPalettesMetainfo { // addons metadate
public:
    EgDataNodesSet palettesMetainfo;
    bool           palettesMetainfoLoaded  { false };

    EgDatabase*          metaInfoDatabase  { nullptr };
    EgDataNodeBlueprint* palettesBlueprint { nullptr };

    EgPalettesMetainfo(EgDatabase* a_metaInfoDatabase) : metaInfoDatabase(a_metaInfoDatabase) { getPalettesMetainfo(); }
    
    ~EgPalettesMetainfo() { 
        clear(); 
    }
    
    void clear();
    void getPalettesMetainfo();

    inline void createPalettesMetainfo();

// ======================== layers metadata API ========================
    void AddPalettesToMetainfo(const std::string& linksTypeName); // EgBlueprintIDType& blueprintID, 
    void LoadPalettesInfo();

    bool palettesSetIDByName(const std::string& layersName, EgDataNodeIDType& layersTypeID);
    
    int  CreatePalettesSet(const std::string& paletteName);
    int  CreateOnePalette(const std::string& setName, const std::string& paletteName);
};