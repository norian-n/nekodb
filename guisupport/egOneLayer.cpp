#include <iostream>
#include "egOneLayer.h"
#include "../links/egLinks.h"
#include "../links/egLiteralsLinks.h"
#include "../guisupport/egLiteralsGuisupport.h"

EgOneLayer::EgOneLayer(EgStdLayers& a_Layers, EgDataNodeIDType a_layerID, EgDataNodeIDType a_parentLayerID):  
    layers {&a_Layers}, layerID {a_layerID}, parentLayerID {a_parentLayerID}  {
}

void EgOneLayer::clear() {
    // EG_LOG_STUB << "layerID: " << layerID << " parentLayerID: " << parentLayerID << FN;
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
        graphNodes = nodesIter-> second; // .nodesSetPtr;
        // if ( ! graphNodes-> isDataLoaded ) {
            graphNodes-> serialLoadFunction  = loadNodeObjectFromDb;
            graphNodes-> serialStoreFunction = storeNodeObjectToDb;
            // graphNodes-> LoadAllNodes();
        // }
        if ( ! graphNodes-> ConnectSystemNodesSet(nodesIter-> first, nodeWidgetBPName) )
            EG_LOG_STUB << "ERROR: can't connect layer nodes: " << nodesIter-> first << FN;
    }
}

void EgOneLayer::getLayerLinks(EgLinksSet*& graphLinks, serialLoadFunctionType loadLinkObjectFromDb, 
    serialStoreFunctionType storeLinkObjectToDb) {
    auto linksIter = linksMap.begin(); // FIXME only one links type -> many types
    if (linksIter != linksMap.end()) {
        // std::cout << "getLayerLinks() name: " << linksIter-> first << std::endl;
        graphLinks = linksIter-> second;
        // if ( ! graphLinks-> linksDataStorage.isDataLoaded ) {
            graphLinks-> linksDataStorage.serialLoadFunction  = loadLinkObjectFromDb;
            graphLinks-> linksDataStorage.serialStoreFunction = storeLinkObjectToDb;
            // graphNodes-> LoadAllNodes();
        // }
        // EG_LOG_STUB << "getLayerLinks() links set name: " << linksIter-> first << FN;
        if ( graphLinks-> ConnectLayerLinks(linksIter-> first) != 0 )
            EG_LOG_STUB << "ERROR: can't connect layer links: " << linksIter-> first << FN;
    }
}

/*
void EgOneLayer::getLayerDarkNodesLinks() {
    auto nodesIter = nodesMap.begin(); // gen dark links name
    if (nodesIter != nodesMap.end()) {
        // graphNodes = nodesIter-> second;
    }
} */

/*      AddLinkDataField(linksFromID_FieldName);
        AddLinkDataField(linksToID_FieldName);
        AddLinkDataField(linksFromSetName_FieldName);
        AddLinkDataField(linksToSetName_FieldName); */

void EgOneLayer::getDarkNodeID(EgDataNodeIDType widgetID, EgDataNodeIDType& darkNodeID) {
    darkNodeID = 0; // not found
    EgDataNodeIDType currentWidgetID {0};
    std::string darkNodesLinksName;
    auto nodesIter = nodesMap.begin(); // FIXME only one nodes type -> many types
    if (nodesIter != nodesMap.end()) {
        darkNodesLinksName = nodesIter-> first + oneLayerDarkNodesPostfix;
    } // FIXME else error
    if (! darkNodesLinks.linksDataStorage.isConnected)
        darkNodesLinks.ConnectLinks(darkNodesLinksName, *(layers-> metaInfoDatabase));
    darkNodesLinks.LoadLinks();
    // search widgetID  linksToID_FieldName
    for (auto layersIter : darkNodesLinks.linksDataStorage.dataMap) {
        (*(layersIter.second))[linksToID_FieldName]  >> currentWidgetID;
        // EG_LOG_STUB << "currentWidgetID: " << currentWidgetID << FN;
        if (currentWidgetID == widgetID) {
            (*(layersIter.second))[linksFromID_FieldName]  >> darkNodeID;
            break;
        }
    }
}

void EgOneLayer::addNodesSetToMap(const std::string& nodesName, EgDatabase* graphDB) {
    // nodesSetInfo nodesInfo;
    EgDataNodesSet* graphNodes = new EgDataNodesSet(); // MEM_NEW --> clear()
    graphNodes->nodesSetName = nodesName;
    // graphNodes->Connect(nodesName, *graphDB);
    // nodesInfo.nodesSetPtr = graphNodes;
    nodesMap.insert(std::make_pair(nodesName, graphNodes)); // nodesInfo));
}

void EgOneLayer::addLinksSetToMap(const std::string& linksName, EgDatabase* graphDB) {
    EgLinksSet* graphLinks = new EgLinksSet(); // MEM_NEW --> clear()
    graphLinks-> linkNameShort = linksName; // FIXME check full name
    // graphLinks-> ConnectLinks(linksName, *graphDB);
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