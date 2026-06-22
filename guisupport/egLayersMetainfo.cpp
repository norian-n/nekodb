#include <iostream>
#include <filesystem>

#include "egLayersMetainfo.h"
#include "egLiteralsGuisupport.h"
#include "../metainfo/egDatabase.h"

void EgLayersMetainfo::clear() {
    layersMetainfo.clear();
    layersMetainfoLoaded = false;
}

void EgLayersMetainfo::initDatabase() {
    if (layersMetainfo.ConnectSystemNodeType(layersStorageName, layersStorageName) != 0) {
        EG_LOG_STUB << "INIT: Not found layers metadata storage, create new: " << layersStorageName << FN;
        createLayersMetainfo();
        layersMetainfo.ConnectSystemNodeType(layersStorageName, layersStorageName);
    }
    LoadLayersInfo();
}

inline void EgLayersMetainfo::createLayersMetainfo() { // FIXME STUB
    metaInfoDatabase-> CreateNodeBlueprint(layersStorageName);
    // AddNodeDataField("layersBlueprintID");
    metaInfoDatabase-> AddNodeDataField("layersTypeName");
    metaInfoDatabase-> AddNodeDataField("topLayerID");  
    metaInfoDatabase-> CommitSystemNodeBlueprint();
}

void EgLayersMetainfo::AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& layersTypeName) {
    EgDataNodeIDType layersID {0};
    if ( ! layersTypeIDByName(layersTypeName, layersID) ) // metainfo Data Exists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageBlueprint);
    EgDataNode* newNode = new EgDataNode(layersMetainfo.dataNodeBlueprint);
    blueprintID = layersMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    // (*newNode)["layersBlueprintID"] << blueprintID;
    (*newNode)["layersTypeName"]    << layersTypeName;
    (*newNode)["topLayerID"] << (EgDataNodeIDType) 1;
    layersMetainfo << newNode;
}

void EgLayersMetainfo::LoadLayersInfo() {
    if (layersMetainfoLoaded) // load once
        return;
    layersMetainfoLoaded = ! layersMetainfo.LoadAllNodes(); // 0 is ok
}

bool EgLayersMetainfo::layersTypeIDByName(const std::string& layersName, EgDataNodeIDType& layersTypeID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : layersMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))["layersTypeName"] >> currentName;
            EG_LOG_STUB << "layersTypeIDByName() currentName: " << currentName << " layersName: " << layersName << FN;
            if (currentName == layersName + "_egLayersInfo") {
                layersTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

bool EgLayersMetainfo::topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : layersMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))["layersTypeName"] >> currentName;
            // EG_LOG_STUB << "layersTypeIDByName() currentName: " << currentName << " layersName: " << layersName << FN;
            if (currentName == layersName + "_egLayersInfo") {
                (*(nodesIter.second))["topLayerID"] >> layersTopID;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

void EgLayersMetainfo::updateTopLayerID(const std::string& layersName, EgDataNodeIDType newTopLayerID) {
    EgDataNodeIDType layersID {0};
    if ( ! layersTypeIDByName(layersName, layersID)) { // typename exists
        EgDataNode& updNode = layersMetainfo[layersID];
        updNode["topLayerID"] << newTopLayerID;
        layersMetainfo.MarkUpdatedDataNode(layersID);
        layersMetainfo.Store();
    }
    EG_LOG_STUB << "layersID: " << layersID << " layersName: " << layersName << FN;
}

int EgLayersMetainfo::CreateLayersSet(const std::string& layersTypeName) {
    std::string fullLayersName = layersTypeName + std::string("_egLayersInfo"); // FIXME global const
    if (std::filesystem::exists(fullLayersName + ".dnl")) { 
        EG_LOG_STUB << "ERROR: Layer type/blueprint file already exists, cant CreateLayersBlueprint(): " << layersTypeName << FN;
        return -1;
    }   
    layersBlueprint = new EgDataNodeBlueprint(fullLayersName); // delete here, below
    layersBlueprint-> BlueprintInitStart();
    layersBlueprint-> AddDataFieldName("nodesNames");
    layersBlueprint-> AddDataFieldName("linksNames");
    layersBlueprint-> AddDataFieldName("layerWidth");
    layersBlueprint-> AddDataFieldName("layerHeight");
    layersBlueprint-> AddDataFieldName("parentLayerID");
    layersBlueprint-> BlueprintInitCommit();
    AddLayersTypeInfo(layersBlueprint-> blueprintID, layersBlueprint-> blueprintName);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    layersMetainfo.Store();
    delete layersBlueprint;
    layersBlueprint = nullptr;
    return 0;
}