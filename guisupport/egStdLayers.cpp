#include <iostream>
#include "egStdLayers.h"
#include "egOneLayer.h"
#include "egLiteralsGuisupport.h"
#include "../metainfo/egLiteralsMetainfo.h"

void EgStdLayers::clear() {
    // EG_LOG_STUB << "EgStdLayers::clear() " << FN;
    for (auto layersIter : layersMap) { // MEM_DELETE
        layersIter.second-> clear();
        delete layersIter.second;
    }
    layersMap.clear();
    layersSetStorage.clear();
}

int EgStdLayers::ConnectLayers(const std::string& a_layersSetName, EgDatabase& a_Database) {
    layersSetName = a_layersSetName;
    metaInfoDatabase = &a_Database;
    if ( ! layersSetStorage.ConnectSystemNodesSet(layersSetName + layersSetsFilePostfix, layersSetBPName) ) {
        EG_LOG_STUB << "ConnectLayers() not found layers set storage: " << layersSetName << FN;
        return -1;
    }
    return 0;
}

inline void EgStdLayers::addLayerData(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, const std::string& nodesName, const std::string& linksName) {
    // PrintDataNodeBlueprint(*(layersStorage.dataNodeBlueprint));
    EgDataNode* newNode = new EgDataNode(layersSetStorage.dataNodeBlueprint);
    (*newNode)[layerNodesSets_FieldName] << nodesName; // FIXME literals
    (*newNode)[layerLinks_FieldName] << linksName;
    (*newNode)[layerWidth_FieldName]      << W;
    (*newNode)[layerHeight_FieldName]     << H;
    (*newNode)[layerParentID_FieldName]   << parentLayerID;
    layersSetStorage << newNode;
    newLayerID = layersSetStorage.getAddedNodeID();
    layersSetStorage.Store();
    
    EgOneLayer *newLayer  = new EgOneLayer(*this, newLayerID, parentLayerID); // MEM_NEW --> clear()
    newLayer-> layerWidth  = W;
    newLayer-> layerHeight = H;
    newLayer-> addNodesSetToMap(nodesName, metaInfoDatabase);
    newLayer-> addLinksSetToMap(linksName, metaInfoDatabase);
    layersMap.insert(std::make_pair(newLayerID, newLayer));

    std::string darkLinksSetName = nodesName + oneLayerDarkNodesPostfix;
    metaInfoDatabase-> CreateFreeLinksSetByBlueprint(darkLinksSetName, stdFreeLinkBPName); // dark nodes connect links set 
}

void EgStdLayers::createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
    const std::string& nodesName, const std::string& linksName) {
    // metaInfoDatabase-> CreateNodesSetByBlueprint(nodesName, nodeWidgetBPName); // FIXME STUB move nodes metainfo to layers
    // metaInfoDatabase-> CreateBoundLinksSetByBlueprint(linksName, linkWidgetBPName, nodesName, nodesName);
    addLayerData(newLayerID, parentLayerID, W, H, nodesName, linksName);
}

void EgStdLayers::createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
    const std::string& parentNodesName, const std::string& parentLinksName) {
    std::string newNodesName = parentNodesName + "_" + std::to_string(parentNodeID); // gen nodesSet and linksSet names
    std::string newLinksName = parentLinksName + "_" + std::to_string(parentNodeID);
    // EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    createBlankLayer(newLayerID, parentLayerID, W, H, newNodesName, newLinksName);
    /* metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, layerNodesBlueprint);
    metaInfoDatabase-> CreateBoundLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);
    addLayerData(newLayerID, parentLayerID, W, H, newNodesName, newLinksName); */
}

void EgStdLayers::replaceTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, 
    const std::string& oldNodesName, const std::string& oldLinksName) {
    std::string newNodesName = std::string("TL_") + oldNodesName; // gen nodesSet and linksSet names
    std::string newLinksName = std::string("TL_") + oldLinksName; // FIXME literals

    // EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    // metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, oldLinksName);
    // metaInfoDatabase-> CreateBoundLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);

    EgDataNodesSet newLayerNodes; // create header node for old layer
    newLayerNodes.ConnectSystemNodesSet(newNodesName, nodeWidgetBPName);
    // newLayerNodes.Connect(newNodesName, *metaInfoDatabase);
    EgDataNode* oldLayerLabelNode = new EgDataNode(newLayerNodes.dataNodeBlueprint);
    (*oldLayerLabelNode)[nodeWidgetFieldLabelName] << std::string("OldTopLayer");
    (*oldLayerLabelNode)[nodeWidgetFieldXName]     << 64;
    (*oldLayerLabelNode)[nodeWidgetFieldYName]     << 64;
    (*oldLayerLabelNode)[nodeWidgetFieldHName]     << 64;
    (*oldLayerLabelNode)[nodeWidgetFieldWName]     << 96;
    (*oldLayerLabelNode)[nodeWidgetDetailsIDName]  << oldLayerID;
    newLayerNodes << *oldLayerLabelNode;
    // EgDataNodeIDType parentLayerID = newLayerNodes.getAddedNodeID();
    newLayerNodes.Store();

    addLayerData(newLayerID, 0/* parentLayerID */, W, H, newNodesName, newLinksName);
    updateParentID(oldLayerID, newLayerID);
    metaInfoDatabase-> ConnectLayersMetainfo();
    metaInfoDatabase->layersMetainfo-> updateTopLayerID(layersSetName, newLayerID); // update top layer ID in metainfo
}

void EgStdLayers::updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H) {
    // EG_LOG_STUB << "layerID: " << DEC << layerID << " layersStorage: " << layersStorage.nodesSetName << " layersBP: " << layersStorage.nodeBlueprintName << FN;
    layersSetStorage[layerID][layerWidth_FieldName]  << W; // FIXME literals
    layersSetStorage[layerID][layerHeight_FieldName] << H;
    layersSetStorage.MarkUpdatedDataNode(layerID);
    layersSetStorage.Store();
    auto layersIter = layersMap.find(layerID); // update loaded oneLayer info
    if (layersIter != layersMap.end()) {
        layersIter->second-> layerWidth  = W;
        layersIter->second-> layerHeight = H;
    }
}

void EgStdLayers::updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID) {
    EgDataNode& updNode = layersSetStorage[layerID];
    updNode[layerParentID_FieldName] << parentLayerID;
    layersSetStorage.MarkUpdatedDataNode(layerID);
    layersSetStorage.Store();
    auto layersIter = layersMap.find(layerID); // update loaded oneLayer info
    if (layersIter != layersMap.end())
        layersIter->second-> parentLayerID = parentLayerID;
}

int EgStdLayers::LoadLayers() {
    clear();
    bool res = layersSetStorage.LoadAllNodes();
    if (! res) {
        EG_LOG_STUB << "LoadLayers() ERROR: " << layersSetName << FN;
    } else {
        for (auto layersIter : layersSetStorage.dataMap) {
            EgDataNodeIDType parentLayerID;
            (*(layersIter.second))[layerParentID_FieldName] >> parentLayerID; // FIXME literals
            EgOneLayer *newLayer = new EgOneLayer(*this, layersIter.first, parentLayerID); // MEM_NEW --> clear()
            (*(layersIter.second))[layerWidth_FieldName]  >> newLayer-> layerWidth;
            (*(layersIter.second))[layerHeight_FieldName] >> newLayer-> layerHeight;
            std::string name;
            (*(layersIter.second))[layerNodesSets_FieldName] >> name;
            newLayer-> addNodesSetToMap(name, metaInfoDatabase);
            (*(layersIter.second))[layerLinks_FieldName] >> name;
            newLayer-> addLinksSetToMap(name, metaInfoDatabase);
            layersMap.insert(std::make_pair(layersIter.first, newLayer));
        }
    }
    return ! res;
}

int EgStdLayers::StoreLayers() {
    return layersSetStorage.Store();
}

EgOneLayer* EgStdLayers::operator[](EgDataNodeIDType layerID) {
    auto iter = layersMap.find(layerID);
    if (iter != layersMap.end())
        return iter->second;
    return nullptr;
}