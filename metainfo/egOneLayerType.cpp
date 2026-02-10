#include <iostream>
#include "egOneLayerType.h"
#include "../links/egLinksType.h"

EgOneLayerType::EgOneLayerType(EgLayersType& a_Layers, EgDataNodeIDType a_layerID):  
    layers {&a_Layers}, layerID {a_layerID}  {
}

void EgOneLayerType::clear() {
    for (auto nodesIter : nodesMap) // MEM_DELETE
        delete nodesIter.second;
    nodesMap.clear();
    for (auto linksIter : linksMap) // MEM_DELETE
        delete linksIter.second;
    linksMap.clear(); // FIXME TODO delete nodes objects
}

void EgOneLayerType::getLayerNodes(EgDataNodesType*& graphNodes, serialLoadFunctionType loadNodeObjectFromDb, 
    serialStoreFunctionType storeNodeObjectToDb) {
    auto nodesIter = nodesMap.begin(); // FIXME only one nodes type -> many types
    if (nodesIter != nodesMap.end()) {
        // std::cout << "loadLayerNodes() name: " << nodesIter-> first << std::endl;
        graphNodes = nodesIter-> second;
        if ( ! graphNodes-> isDataLoaded ) {
            graphNodes-> serialLoadFunction  = loadNodeObjectFromDb;
            graphNodes-> serialStoreFunction = storeNodeObjectToDb;
            // graphNodes-> LoadAllNodes();
        }
    }
}

void EgOneLayerType::getLayerLinks(EgLinksType*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, 
    serialStoreFunctionType storeLinkObjectToDb) {
    auto linksIter = linksMap.begin(); // FIXME only one nodes type -> many types
    if (linksIter != linksMap.end()) {
        std::cout << "getLayerLinks() name: " << linksIter-> first << std::endl;
        graphLinks = linksIter-> second;
        if ( ! graphLinks-> linksDataStorage.isDataLoaded ) {
            graphLinks-> linksDataStorage.serialLoadFunction  = loadLinkObjectFromDb;
            graphLinks-> linksDataStorage.serialStoreFunction = storeLinkObjectToDb;
            // graphNodes-> LoadAllNodes();
        }
    }
}

void EgOneLayerType::addNodeType(const std::string& nodesName, EgDatabaseType* graphDB) {
    EgDataNodesType* graphNodes = new EgDataNodesType(); // MEM_NEW --> clear()
    graphNodes->Connect(nodesName, *graphDB);
    nodesMap.insert(std::make_pair(nodesName, graphNodes));
}

void EgOneLayerType::addLinkType(const std::string& linksName, EgDatabaseType* graphDB) {
    EgLinksType* graphLinks = new EgLinksType(); // MEM_NEW --> clear()
    graphLinks-> ConnectLinks(linksName, *graphDB);
    linksMap.insert(std::make_pair(linksName, graphLinks));
}


/*void EgOneLayerType::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, EgLayerNumType layerNum) {
    nodesNames.clear();
    linksNames.clear();
    EgLayerNumType theLayer;
    uint8_t isLink;
    std::string theName;
    if (layersContentStorage.isConnected) {
        for (auto nodesIter : layersContentStorage.dataMap) {
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
} */

/*void PrintLayersInfo() {

}*/