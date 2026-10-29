#include <iostream>
#include <filesystem>

#include "egPalettesMetainfo.h"
#include "egLiteralsGuisupport.h"
#include "../metainfo/egDatabase.h"

void EgPalettesMetainfo::clear() {
    palettesMetainfo.clear();
    palettesMetainfoLoaded = false;
}

void EgPalettesMetainfo::getPalettesMetainfo() {
    if ( ! palettesMetainfo.ConnectSystemNodesSet(palettesMetainfoName, palettesMetainfoName) ) { // storage not found
        EG_LOG_STUB << "INIT: Not found palettes metadata storage, create new: " << palettesMetainfoName << FN;
        createPalettesMetainfo();
        palettesMetainfo.ConnectSystemNodesSet(palettesMetainfoName, palettesMetainfoName);
    }
    LoadPalettesInfo();
}

inline void EgPalettesMetainfo::createPalettesMetainfo() {
    if ( metaInfoDatabase-> CreateSystemBlueprint(palettesMetainfoName)) { // metainfo blueprint
        metaInfoDatabase-> AddNodeDataField(palettesNameField);
        metaInfoDatabase-> CommitSystemBlueprint();
    }
    if ( metaInfoDatabase-> CreateSystemBlueprint(palettesSetBPName)) { // palettesSet blueprint
        metaInfoDatabase-> AddNodeDataField(palettesNameField);
        metaInfoDatabase-> CommitSystemBlueprint();
    }
    if ( metaInfoDatabase-> CreateSystemBlueprint(onePaletteBPName)) { // onePalette blueprint
        metaInfoDatabase-> AddNodeDataField(paletteFieldIndexName);
        metaInfoDatabase-> AddNodeDataField(paletteFieldNodesName);
        metaInfoDatabase-> AddNodeDataField(paletteFieldNodeIDName);
        metaInfoDatabase-> AddNodeDataField(paletteWidgetNameName);
        metaInfoDatabase-> AddNodeDataField(paletteWidgetColorName);
        metaInfoDatabase-> CommitSystemBlueprint();
    }
}

inline void EgPalettesMetainfo::AddPalettesToMetainfo(const std::string& palettesSetName) { // EgBlueprintIDType& blueprintID, 
    // EgDataNodeIDType layersID {0};
    // if ( palettesSetExists(palettesSetName) ) // , layersID) ) // metainfo Data Exists
    //    return;
    EgDataNode* newNode = new EgDataNode(palettesMetainfo.dataNodeBlueprint);
    // blueprintID = palettesMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    (*newNode)[palettesNameField]    << palettesSetName;
    palettesMetainfo << newNode;
}

void EgPalettesMetainfo::LoadPalettesInfo() {
    if (palettesMetainfoLoaded) // load once
        return;
    palettesMetainfoLoaded = palettesMetainfo.LoadAllNodes();
}

bool EgPalettesMetainfo::palettesSetExists(const std::string& palettesName) { // , EgDataNodeIDType& layersTypeID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : palettesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[palettesName] >> currentName;
            EG_LOG_STUB << "paletteTypeIDByName() currentName: " << currentName << " palettesName: " << palettesName << FN;
            if (currentName == palettesName + "_egPalettesSet") {
                // layersTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return true; // typename exists
            }
        }
    // }
    return false;
}

int EgPalettesMetainfo::CreatePalettesSet(const std::string& paletteName) {
    std::string fullPalettesName = paletteName + std::string("_egPalettesSet"); // FIXME global const
    // if (std::filesystem::exists(fullPalettesName + ".dnl")) {  FIXME BUG - common BP
    // EgDataNodeIDType setIDstub;
    if ( palettesSetExists(fullPalettesName) ) { // , setIDstub)) {
        EG_LOG_STUB << "ERROR: PalettesSet name already exists, cant CreatePalettesSet(): " << paletteName << FN;
        return -1;
    }   
    metaInfoDatabase->CreateNodesSetByBlueprint(fullPalettesName, palettesSetBPName);
    AddPalettesToMetainfo(fullPalettesName); // palettesBlueprint-> blueprintName);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    palettesMetainfo.Store();
    // delete palettesBlueprint;
    // palettesBlueprint = nullptr;
    return 0;
}