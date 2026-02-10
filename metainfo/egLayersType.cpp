#include <iostream>
#include "egLayersType.h"
#include "egOneLayerType.h"
// #include "../links/egLinksType.h"

void EgLayersType::clear() {
    for (auto layersIter : layersMap) // MEM_DELETE
        delete layersIter.second;
    layersMap.clear();
    layersStorage.clear();
}

int EgLayersType::ConnectLayers(const std::string& a_layersTypeName, EgDatabaseType& a_Database) {
    layersTypeName = a_layersTypeName;
    metaInfoDatabase = &a_Database;
    if (layersStorage.ConnectSystemNodeType(a_layersTypeName + "_egLayersInfo") != 0) {
    // if (layersStorage.Connect(a_layersTypeName + "_egLayersInfo", a_Database) != 0) { // FIXME check
        std::cout << "ConnectLayers() not found storage: " << a_layersTypeName << std::endl;
    }

    // layersContentStorageBlueprint = layersContentStorage.dataNodeBlueprint;

    return 0;
}

void EgLayersType::AddLayerInfo(const std::string& nodesName, const std::string& linksName) {
    EgDataNode *newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["nodesNames"]  << nodesName.c_str();
    (*newNode)["linksNames"]  << linksName.c_str();
    (*newNode)["layerWidth"]  << (uint32_t) 300; // FIXME STUB
    (*newNode)["layerHeight"] << (uint32_t) 200;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
}

/* void EgLayersType::AddLinksType(const std::string& nodesName, EgDataNodeIDType layerNum) {
    EgDataNode *newNode = new EgDataNode(layersStorage.dataNodeBlueprint);
    (*newNode)["name"]     << nodesName.c_str();
    (*newNode)["isLink"]   << (uint8_t) 0xFF;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    layersStorage << newNode;
} */

void EgLayersType::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, 
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

int EgLayersType::LoadLayers() {
    clear();
    int res = layersStorage.LoadAllNodes();
    if (res) {
        std::cout << "LoadLayers() ERROR: " << layersTypeName << std::endl;
    } else {
        for (auto layersIter : layersStorage.dataMap) {
            EgOneLayerType *newLayer = new EgOneLayerType(*this, layersIter.first); // MEM_NEW --> clear()
            std::string name;
            (*(layersIter.second))["nodesNames"] >> name;
            std::cout << "LoadLayers() nodeName: " << name << std::endl;
            newLayer-> addNodeType(name, metaInfoDatabase);
            (*(layersIter.second))["linksNames"] >> name;
            std::cout << "LoadLayers() linkName: " << name << std::endl;
            newLayer-> addLinkType(name, metaInfoDatabase);
            layersMap.insert(std::make_pair(layersIter.first, newLayer));
        }
    }
    return res;
}

int EgLayersType::StoreLayers() {
    return layersStorage.Store();
}

EgOneLayerType* EgLayersType::operator[](EgDataNodeIDType layerID) {
    auto iter = layersMap.find(layerID);
    if (iter != layersMap.end())
        return iter->second;
    return nullptr;
}

/*void PrintLayersInfo() {

}*/