#include <iostream>
#include "egLayers.h"
#include "egOneLayer.h"

void EgLayers::clear() {
    // EG_LOG_STUB << "EgLayers::clear() " << FN;
    for (auto layersIter : layersMap) { // MEM_DELETE
        layersIter.second-> clear();
        delete layersIter.second;
    }
    layersMap.clear();
    layersStorage.clear();
}

int EgLayers::ConnectLayers(const std::string& a_layersTypeName, EgDatabase& a_Database) {
    layersTypeName = a_layersTypeName;
    metaInfoDatabase = &a_Database;
    if (layersStorage.ConnectSystemNodeType(a_layersTypeName + "_egLayersInfo", a_layersTypeName + "_egLayersInfo") != 0) {
    // if (layersStorage.Connect(a_layersTypeName + "_egLayersInfo", a_Database) != 0) { // FIXME check
        std::cout << "ConnectLayers() not found storage: " << a_layersTypeName << std::endl;
    }
    return 0;
}

inline void EgLayers::addLayerData(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, const std::string& nodesName, const std::string& linksName) {
    // PrintDataNodeBlueprint(*(layersStorage.dataNodeBlueprint));
    EgDataNode* newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["nodesNames"]    << nodesName; // FIXME literals
    (*newNode)["linksNames"]    << linksName;
    (*newNode)["layerWidth"]    << W;
    (*newNode)["layerHeight"]   << H;
    (*newNode)["parentLayerID"] << parentLayerID;
    layersStorage << newNode;
    newLayerID = layersStorage.getAddedNodeID();
    layersStorage.Store();
    // delete newNode;
    
    EgOneLayer *newLayer  = new EgOneLayer(*this, newLayerID, parentLayerID); // MEM_NEW --> clear()
    newLayer->layerWidth  = W;
    newLayer->layerHeight = H;
    newLayer->connectNodeType(nodesName, metaInfoDatabase);
    newLayer->connectLinkType(linksName, metaInfoDatabase);
    layersMap.insert(std::make_pair(newLayerID, newLayer));
}

void EgLayers::createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, const std::string& nodesName, const std::string& linksName) {
    addLayerData(newLayerID, parentLayerID, W, H, nodesName, linksName);
}

void EgLayers::createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
    const std::string& parentNodesName, const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint) {
    std::string newNodesName = parentNodesName + "_" + std::to_string(parentNodeID); // gen nodesSet and linksSet names
    std::string newLinksName = parentNodesName + "_" + std::to_string(parentNodeID);
    // EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, layerNodesBlueprint);
    metaInfoDatabase-> CreateLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);
    addLayerData(newLayerID, parentLayerID, W, H, newNodesName, newLinksName);
}

void EgLayers::createNewTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, const std::string& oldLayerName,
    const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint) {
    std::string newNodesName = std::string("TL_") + oldLayerName; // gen nodesSet and linksSet names
    std::string newLinksName = std::string("TL_") + oldLayerName;

    // EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, layerNodesBlueprint);
    metaInfoDatabase-> CreateLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);

    EgDataNodesSet newLayerNodes; // create header node for old layer
    newLayerNodes.Connect(newNodesName, *metaInfoDatabase);
    EgDataNode* oldLayerLabelNode = new EgDataNode(newLayerNodes.dataNodeBlueprint);
    (*oldLayerLabelNode)["name"]    << std::string("OldTopLayer"); // FIXME literals
    (*oldLayerLabelNode)["cornerX"] << 64;
    (*oldLayerLabelNode)["cornerY"] << 64;
    (*oldLayerLabelNode)["rectH"]   << 64;
    (*oldLayerLabelNode)["rectW"]   << 96;
    (*oldLayerLabelNode)["detailsLayerID"] << oldLayerID;
    newLayerNodes << *oldLayerLabelNode;
    // EgDataNodeIDType parentLayerID = newLayerNodes.getAddedNodeID();
    newLayerNodes.Store();
    // delete oldLayerLabelNode;

    addLayerData(newLayerID, 0/* parentLayerID */, W, H, newNodesName, newLinksName);
    updateParentID(oldLayerID, newLayerID);
    metaInfoDatabase-> ConnectLayersMetainfo();
    metaInfoDatabase->layersMetainfo-> updateTopLayerID(layersTypeName, newLayerID); // update top layer ID in metainfo
}

void EgLayers::updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H) {
    // EG_LOG_STUB << "layerID: " << DEC << layerID << " layersStorage: " << layersStorage.nodesSetName << " layersBP: " << layersStorage.nodeBlueprintName << FN;
    // EgDataNode& updNode = layersStorage[layerID];
    // PrintEgDataNodeFields (layersStorage[layerID]);
    layersStorage[layerID]["layerWidth"]  << W; // FIXME literals
    layersStorage[layerID]["layerHeight"] << H;
    layersStorage.MarkUpdatedDataNode(layerID);
    layersStorage.Store();
    auto layersIter = layersMap.find(layerID); // update loaded oneLayer info
    if (layersIter != layersMap.end()) {
        layersIter->second-> layerWidth  = W;
        layersIter->second-> layerHeight = H;
    }
}

void EgLayers::updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID) {
    EgDataNode& updNode = layersStorage[layerID];
    updNode["parentLayerID"] << parentLayerID;
    layersStorage.MarkUpdatedDataNode(layerID);
    layersStorage.Store();
    auto layersIter = layersMap.find(layerID); // update loaded oneLayer info
    if (layersIter != layersMap.end())
        layersIter->second-> parentLayerID = parentLayerID;
}

/* void EgLayersType::AddLinksType(const std::string& nodesName, EgDataNodeIDType layerNum) {
    EgDataNode *newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["name"]     << nodesName;
    (*newNode)["isLink"]   << (uint8_t) 0xFF;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
} */

/*
void EgLayers::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, 
                                         EgDataNodeIDType layerNum) {
    nodesNames.clear();
    linksNames.clear();
    EgDataNodeIDType theLayer;
    uint8_t isLink;
    std::string theName;
    if (layersStorage.isConnected) {
        for (auto nodesIter : layersStorage.dataMap) {
            (*nodesIter.second)["layerNum"] >> theLayer; // FIXME literals
            if (theLayer == layerNum) {
                (*nodesIter.second)["isLink"] >> isLink;
                (*nodesIter.second)["name"]   >> theName;
                if (isLink)
                    linksNames.insert(theName);
                else
                    nodesNames.insert(theName);
            }
        }
    }
} */

int EgLayers::LoadLayers() {
    clear();
    int res = layersStorage.LoadAllNodes();
    if (res) {
        std::cout << "LoadLayers() ERROR: " << layersTypeName << std::endl;
    } else {
        for (auto layersIter : layersStorage.dataMap) {
            EgDataNodeIDType parentLayerID;
            (*(layersIter.second))["parentLayerID"] >> parentLayerID; // FIXME literals
            EgOneLayer *newLayer = new EgOneLayer(*this, layersIter.first, parentLayerID); // MEM_NEW --> clear()
            (*(layersIter.second))["layerWidth"]  >> newLayer-> layerWidth;
            (*(layersIter.second))["layerHeight"] >> newLayer-> layerHeight;
            std::string name;
            (*(layersIter.second))["nodesNames"] >> name;
            // std::cout << "LoadLayers() nodeName: " << name << std::endl;
            newLayer-> connectNodeType(name, metaInfoDatabase);
            (*(layersIter.second))["linksNames"] >> name;
            // std::cout << "LoadLayers() linkName: " << name << std::endl;
            newLayer-> connectLinkType(name, metaInfoDatabase);
            layersMap.insert(std::make_pair(layersIter.first, newLayer));
        }
    }
    return res;
}

int EgLayers::StoreLayers() {
    return layersStorage.Store();
}

EgOneLayer* EgLayers::operator[](EgDataNodeIDType layerID) {
    auto iter = layersMap.find(layerID);
    if (iter != layersMap.end())
        return iter->second;
    return nullptr;
}

/*void PrintLayersInfo() {

}*/