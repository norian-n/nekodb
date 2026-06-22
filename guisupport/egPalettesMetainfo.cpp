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
    if (palettesMetainfo.ConnectSystemNodeType(palettesMetainfoName, palettesMetainfoName)) { // storage not found
        EG_LOG_STUB << "INIT: Not found palettes metadata storage, create new: " << palettesMetainfoName << FN;
        createPalettesMetainfo();
        palettesMetainfo.ConnectSystemNodeType(palettesMetainfoName, palettesMetainfoName);
    }
    LoadPalettesInfo();
}

inline void EgPalettesMetainfo::createPalettesMetainfo() {
    if ( ! metaInfoDatabase-> CreateNodeBlueprint(palettesMetainfoName)) { // metainfo blueprint
        metaInfoDatabase-> AddNodeDataField(palettesNameField);
        metaInfoDatabase-> CommitSystemNodeBlueprint();
    }
    if ( ! metaInfoDatabase-> CreateNodeBlueprint(palettesSetBPName)) { // palettesSet blueprint
        metaInfoDatabase-> AddNodeDataField(palettesNameField);
        metaInfoDatabase-> CommitSystemNodeBlueprint();
    }
    if ( ! metaInfoDatabase-> CreateNodeBlueprint(onePaletteBPName)) { // onePalette blueprint
        metaInfoDatabase-> AddNodeDataField(paletteFieldIndexName);
        metaInfoDatabase-> AddNodeDataField(paletteFieldNodesName);
        metaInfoDatabase-> AddNodeDataField(paletteFieldNodeIDName);
        metaInfoDatabase-> AddNodeDataField(paletteWidgetNameName);
        metaInfoDatabase-> AddNodeDataField(paletteWidgetColorName);
        metaInfoDatabase-> CommitSystemNodeBlueprint();
    }
}

void EgPalettesMetainfo::AddPalettesToMetainfo(const std::string& palettesName) { // EgBlueprintIDType& blueprintID, 
    EgDataNodeIDType layersID {0};
    if ( ! palettesSetIDByName(palettesName, layersID) ) // metainfo Data Exists
        return;
    EgDataNode* newNode = new EgDataNode(palettesMetainfo.dataNodeBlueprint);
    // blueprintID = palettesMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    (*newNode)[palettesNameField]    << palettesName;
    palettesMetainfo << newNode;
}

void EgPalettesMetainfo::LoadPalettesInfo() {
    if (palettesMetainfoLoaded) // load once
        return;
    palettesMetainfoLoaded = ! palettesMetainfo.LoadAllNodes(); // 0 is ok
}

bool EgPalettesMetainfo::palettesSetIDByName(const std::string& palettesName, EgDataNodeIDType& layersTypeID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : palettesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[palettesName] >> currentName;
            EG_LOG_STUB << "paletteTypeIDByName() currentName: " << currentName << " palettesName: " << palettesName << FN;
            if (currentName == palettesName + "_egPalettesSet") {
                layersTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

int EgPalettesMetainfo::CreatePalettesSet(const std::string& paletteName) {
    std::string fullPalettesName = paletteName + std::string("_egPalettesSet"); // FIXME global const
    // if (std::filesystem::exists(fullPalettesName + ".dnl")) {  FIXME BUG - common BP
    EgDataNodeIDType setIDstub;
    if (! palettesSetIDByName(fullPalettesName, setIDstub)) {
        EG_LOG_STUB << "ERROR: PalettesSet name already exists, cant CreatePalettesSet(): " << paletteName << FN;
        return -1;
    }   
    metaInfoDatabase->CreateNodesSetByBlueprint(fullPalettesName, palettesSetBPName);
    AddPalettesToMetainfo(fullPalettesName); // palettesBlueprint-> blueprintName);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    palettesMetainfo.Store();
    delete palettesBlueprint;
    palettesBlueprint = nullptr;
    return 0;
}