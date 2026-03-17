#include <iostream>
#include "egOneLayer.h"
#include "../links/egLinks.h"

EgOneLayer::EgOneLayer(EgLayers& a_Layers, EgDataNodeIDType a_layerID):  
    layers {&a_Layers}, layerID {a_layerID}  {
}

void EgOneLayer::clear() {
    for (auto nodesIter : nodesMap) // MEM_DELETE
        delete nodesIter.second; // .nodesSetPtr;
    nodesMap.clear();
    for (auto linksIter : linksMap) // MEM_DELETE
        delete linksIter.second;
    linksMap.clear(); // FIXME TODO delete nodes objects
}

/*
void EgOneLayerType::setNodesInfo (std::string nodesName, serialLoadFunctionType loadNodeFunc, serialStoreFunctionType storeNodeFunc) {
    auto nodesIter = nodesMap.find(nodesName);
    if (nodesIter != nodesMap.end()) {
        nodesIter-> second.loadNodeFromDb = loadNodeFunc;
        nodesIter-> second.storeNodeToDb  = storeNodeFunc;
    }
} */

void EgOneLayer::getLayerNodes(EgDataNodesSet*& graphNodes, serialLoadFunctionType loadNodeObjectFromDb, 
    serialStoreFunctionType storeNodeObjectToDb) {
    auto nodesIter = nodesMap.begin(); // FIXME only one nodes type -> many types
    if (nodesIter != nodesMap.end()) {
        // std::cout << "loadLayerNodes() name: " << nodesIter-> first << std::endl;
        graphNodes = nodesIter-> second; // .nodesSetPtr;
        if ( ! graphNodes-> isDataLoaded ) {
            graphNodes-> serialLoadFunction  = loadNodeObjectFromDb;
            graphNodes-> serialStoreFunction = storeNodeObjectToDb;
            // graphNodes-> LoadAllNodes();
        }
    }
}

void EgOneLayer::getLayerLinks(EgLinksSet*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, 
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

void EgOneLayer::connectNodeType(const std::string& nodesName, EgDatabase* graphDB) {
    // nodesSetInfo nodesInfo;
    EgDataNodesSet* graphNodes = new EgDataNodesSet(); // MEM_NEW --> clear()
    graphNodes->Connect(nodesName, *graphDB);
    // nodesInfo.nodesSetPtr = graphNodes;
    nodesMap.insert(std::make_pair(nodesName, graphNodes)); // nodesInfo));
}

void EgOneLayer::connectLinkType(const std::string& linksName, EgDatabase* graphDB) {
    EgLinksSet* graphLinks = new EgLinksSet(); // MEM_NEW --> clear()
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