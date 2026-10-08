#include <iostream>
#include <filesystem>

#include "egLayersMetainfo.h"
#include "egLiteralsGuisupport.h"
#include "../metainfo/egDatabase.h"
#include "../links/egLiteralsLinks.h"

void EgLayersMetainfo::clear() {
    layersSetsMetainfo.clear();
    layersMetainfoLoaded = false;
}

void EgLayersMetainfo::ConnectOrCreateStorage() {
    if ( ! layersSetsMetainfo.ConnectSystemNodesSet(layersMetainfoStorageName, layersMetainfoStorageName) ) {
        EG_LOG_STUB << "INIT: Not found layers metadata storage, create new: " << layersMetainfoStorageName << FN;
        CreateLayersBlueprints();
        layersSetsMetainfo.ConnectSystemNodesSet(layersMetainfoStorageName, layersMetainfoStorageName);
    }
    LoadLayersInfo();
}

inline void EgLayersMetainfo::CreateLayersBlueprints() { // FIXME STUB
    if ( metaInfoDatabase->CreateNodeBlueprint(layersMetainfoStorageName) ) { // layers sets metainfo blueprint
        metaInfoDatabase->AddNodeDataField(layersFieldNameName);
        metaInfoDatabase->AddNodeDataField(layersFieldTopLayerIDName);
        metaInfoDatabase->CommitNodeBlueprint();
    }
    if ( metaInfoDatabase->CreateNodeBlueprint(layersSetBPName) ) { // layers set blueprint
        metaInfoDatabase->AddNodeDataField(layerNodesSets_FieldName);
        metaInfoDatabase->AddNodeDataField(layerLinks_FieldName);
        // metaInfoDatabase->AddNodeDataField(layerDarkNodesLink_FieldName);
        metaInfoDatabase->AddNodeDataField(layerWidth_FieldName);
        metaInfoDatabase->AddNodeDataField(layerHeight_FieldName);
        metaInfoDatabase->AddNodeDataField(layerParentID_FieldName);
        metaInfoDatabase->CommitNodeBlueprint();
    }
    if ( metaInfoDatabase-> CreateNodeBlueprint(nodeWidgetBPName) ) { // nodeWidget blueprint
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldLabelName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldXName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldYName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldWName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldHName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldColorName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldImageName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetDetailsIDName);
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldNodesName);  // "boundNodesSet") FIXME remove to free link
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldNodeIDName); // "boundNodeID"
        metaInfoDatabase-> AddNodeDataField(nodeWidgetFieldDescName);   // "widgetDescription"
        metaInfoDatabase-> CommitNodeBlueprint();
    }
    if ( ! metaInfoDatabase-> CreateLinkBlueprint(linkWidgetBPName)) { // layer links blueprint
        metaInfoDatabase->AddLinkDataField(linkWidgetStartX_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetStartY_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetEndX_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetEndY_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetPortFrom_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetPortTo_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetCoordFrom_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetCoordTo_FName);
        metaInfoDatabase->AddLinkDataField(linkWidgetLineType_FName);
        metaInfoDatabase-> CommitBoundLinkBlueprint();
    }
}

void EgLayersMetainfo::LoadLayersInfo() {
    if (layersMetainfoLoaded) // load once
        return;
    layersMetainfoLoaded = layersSetsMetainfo.LoadAllNodes(); // 0 is ok
}

bool EgLayersMetainfo::layersSetNameExists(const std::string& layersName) {
    std::string currentName;
    for (auto nodesIter : layersSetsMetainfo.dataMap) {
        (*(nodesIter.second))[layersFieldNameName] >> currentName;
        // EG_LOG_STUB << "layersSetNameExists() currentName: " << currentName << " layersName: " << layersName << FN;
        if (currentName == layersName) { //  + "_egLayersInfo"
            // EG_LOG_STUB << "layersSetNameExists() node type found: " << layersName << FN;
            return true; // typename exists
        }
    }
    return false;
}

int EgLayersMetainfo::CreateLayersSet(const std::string& layersSetName) {
    if ( layersSetNameExists(layersSetName) ) { // name exists
        EG_LOG_STUB << "ERROR: Layers set name already exists, cant CreateLayersSet(): " << layersSetName << FN;
        return -1;
    }
    EgDataNode* newNode = new EgDataNode(layersSetsMetainfo.dataNodeBlueprint);
    (*newNode)[layersFieldNameName]       << layersSetName;
    (*newNode)[layersFieldTopLayerIDName] << (EgDataNodeIDType) 1; // top layer default ID is 1
    layersSetsMetainfo << newNode;
    layersSetsMetainfo.Store();
    return 0;
}

bool EgLayersMetainfo::layersSetIDByName(const std::string& layersName, EgDataNodeIDType& layersID) {
    std::string currentName;
    for (auto nodesIter : layersSetsMetainfo.dataMap) {
        (*(nodesIter.second))[layersFieldNameName] >> currentName;
        if (currentName == layersName) {
            layersID = nodesIter.first;
            return false; // typename exists
        }
    }
    return true;
}

bool EgLayersMetainfo::topLayerIDByName(const std::string& layersName, EgDataNodeIDType& layersTopID) {
    std::string currentName;
    for (auto nodesIter : layersSetsMetainfo.dataMap) {
        (*(nodesIter.second))[layersFieldNameName] >> currentName;
        if (currentName == layersName) {
            (*(nodesIter.second))[layersFieldTopLayerIDName] >> layersTopID;
            // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
            return false; // typename exists
        }
    }
    return true;
}

void EgLayersMetainfo::updateTopLayerID(const std::string& layersName, EgDataNodeIDType newTopLayerID) {
    EgDataNodeIDType layersID {0};
    if (! layersSetIDByName(layersName, layersID)) { // name exists
        layersSetsMetainfo[layersID][layersFieldTopLayerIDName] << newTopLayerID;
        layersSetsMetainfo.MarkUpdatedDataNode(layersID);
        layersSetsMetainfo.Store();
    }
    // EG_LOG_STUB << "layersID: " << layersID << " layersName: " << layersName << FN;
}