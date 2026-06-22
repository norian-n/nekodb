#include <iostream>
#include "egPalettes.h"
#include "egLiteralsGuisupport.h"

void EgPalettes::clear() {
    // EG_LOG_STUB << "EgPalettes::clear() " << FN;
    paletteStorage.clear();
}

int EgPalettes::ConnectPalettesSet(const std::string& a_paletteTypeName, EgDatabase& a_Database) {
    paletteTypeName = a_paletteTypeName;
    metaInfoDatabase = &a_Database;
    if (paletteStorage.ConnectSystemNodeType(a_paletteTypeName + "_egPalettesSet", palettesSetBPName) != 0) {
    // if (paletteStorage.Connect(a_paletteTypeName + "_egPaletteInfo", a_Database) != 0) { // FIXME check
        std::cout << "ConnectPalette() not found storage: " << a_paletteTypeName << std::endl;
    }
    return 0;
}

bool EgPalettes::onePaletteIDByName(const std::string& paletteName, EgDataNodeIDType& layersTypeID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : paletteStorage.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[palettesNameField] >> currentName;
            EG_LOG_STUB << "paletteTypeIDByName() currentName: " << currentName << " paletteName: " << paletteName << FN;
            if (currentName == paletteName + "_egPalettesSet") {
                layersTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

int EgPalettes::CreateOnePalette(const std::string& paletteName) {
    std::string fullPaletteName = paletteName + std::string("_egPalettesSet");
    // check if one palette already exists
    
    // metaInfoDatabase->CreateNodesSetByBlueprint(fullPaletteName, onePaletteBPName);

    EgDataNode* newNode = new EgDataNode(paletteStorage.dataNodeBlueprint);
    (*newNode)[palettesNameField]    << paletteName;
    paletteStorage << newNode;
    paletteStorage.Store();

    return 0;
}

int EgPalettes::LoadPalette() {
    clear();
    int res = paletteStorage.LoadAllNodes();
    if (res) {
        std::cout << "LoadPalette() ERROR: " << paletteTypeName << std::endl;
    } else {
        for (auto paletteIter : paletteStorage.dataMap) {
            EgDataNodeIDType parentLayerID;
            (*(paletteIter.second))["parentLayerID"] >> parentLayerID; // FIXME literals
            /* paletteMap.insert(std::make_pair(paletteIter.first, newLayer)); */
        }
    }
    return res;
}

int EgPalettes::StorePalette() {
    return paletteStorage.Store();
}

/*
EgOneLayer* EgPalettes::operator[](EgDataNodeIDType layerID) {
    auto iter = paletteMap.find(layerID);
    if (iter != paletteMap.end())
        return iter->second;
    return nullptr;
} */
