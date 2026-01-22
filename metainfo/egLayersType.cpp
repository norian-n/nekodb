#include <iostream>
#include "egLayersType.h"

void EgLayersType::clear() {
    layersStorage.clear();
}

int EgLayersType::ConnectLayers(const std::string& a_layersTypeName, EgDatabaseType& a_Database) {
    layersTypeName = a_layersTypeName;
    metaInfoDatabase = &a_Database;
    if (layersStorage.ConnectSystemNodeType(a_layersTypeName + "_egLayersInfo") != 0) {
        std::cout << "ConnectLayers() not found storage: " << a_layersTypeName << std::endl;
    }

    layersStorageBlueprint = layersStorage.dataNodeBlueprint;

    return 0;
}

void EgLayersType::AddNodesType(const std::string& nodesName, EgLayerNumType layerNum) {
    EgDataNode *newNode = new EgDataNode(layersStorageBlueprint);
    (*newNode)["name"]     << nodesName.c_str();
    (*newNode)["isLink"]   << (uint8_t) 0;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
}

void EgLayersType::AddLinksType(const std::string& nodesName, EgLayerNumType layerNum) {
    EgDataNode *newNode = new EgDataNode(layersStorageBlueprint);
    (*newNode)["name"]     << nodesName.c_str();
    (*newNode)["isLink"]   << (uint8_t) 0xFF;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
}

void EgLayersType::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, EgLayerNumType layerNum) {
    nodesNames.clear();
    linksNames.clear();
    EgLayerNumType theLayer;
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

int EgLayersType::LoadLayers() {
    layersStorage.clear();
    return layersStorage.LoadAllNodes();
}

int EgLayersType::StoreLayers() {
    return layersStorage.Store();
}

/*void PrintLayersInfo() {

}*/