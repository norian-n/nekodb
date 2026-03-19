#include <iostream>
#include "egLayers.h"
#include "egOneLayer.h"
// #include "../links/egLinksType.h"

void EgLayers::clear() {
    for (auto layersIter : layersMap) // MEM_DELETE
        delete layersIter.second;
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

    // layersContentStorageBlueprint = layersContentStorage.dataNodeBlueprint;

    return 0;
}

inline void EgLayers::addLayerData(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, const std::string& nodesName, const std::string& linksName) {
    EgDataNode *newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["nodesNames"]    << nodesName;
    (*newNode)["linksNames"]    << linksName;
    (*newNode)["layerWidth"]    << W;
    (*newNode)["layerHeight"]   << H;
    (*newNode)["parentLayerID"] << parentLayerID;
    layersStorage << newNode;
    newLayerID = layersStorage.getAddedNodeID();
    layersStorage.Store();
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
    std::string newLinksName = parentNodesName + "_" + std::to_string(parentNodeID);;
    EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, layerNodesBlueprint);
    metaInfoDatabase-> CreateLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);
    addLayerData(newLayerID, parentLayerID, W, H, newNodesName, newLinksName);
}

void EgLayers::updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H) {
    EgDataNode& updNode = layersStorage[layerID];
    updNode["layerWidth"]  << W;
    updNode["layerHeight"] << H;
    layersStorage.MarkUpdatedDataNode(layerID);
    layersStorage.Store();
}

/* void EgLayersType::AddLinksType(const std::string& nodesName, EgDataNodeIDType layerNum) {
    EgDataNode *newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["name"]     << nodesName;
    (*newNode)["isLink"]   << (uint8_t) 0xFF;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
} */

void EgLayers::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, 
                                         EgDataNodeIDType layerNum) {
    nodesNames.clear();
    linksNames.clear();
    EgDataNodeIDType theLayer;
    uint8_t isLink;
    std::string theName;
    if (layersStorage.isConnected) {
        for (auto nodesIter : layersStorage.dataMap) {
            (*nodesIter.second)["layerNum"] >> theLayer;
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
}

int EgLayers::LoadLayers() {
    clear();
    int res = layersStorage.LoadAllNodes();
    if (res) {
        std::cout << "LoadLayers() ERROR: " << layersTypeName << std::endl;
    } else {
        for (auto layersIter : layersStorage.dataMap) {
            EgDataNodeIDType parentLayerID;
            (*(layersIter.second))["parentLayerID"] >> parentLayerID;
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