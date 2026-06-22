#include <iostream>
#include "egOnePalette.h"
#include "egLiteralsGuisupport.h"

void EgOnePalette::clear() {
    // EG_LOG_STUB << "EgOnePalette::clear() " << FN;
    paletteStorage.clear();
}

int EgOnePalette::ConnectPalette(const std::string& a_paletteTypeName, EgDatabase& a_Database) {
    paletteTypeName = a_paletteTypeName;
    metaInfoDatabase = &a_Database;
    if (paletteStorage.ConnectSystemNodeType(a_paletteTypeName + "_egOnePalette", onePaletteBPName) != 0) { // FIXME literal
    // if (paletteStorage.Connect(a_paletteTypeName + "_egPaletteInfo", a_Database) != 0) { // FIXME check
        std::cout << "ConnectPalette() not found storage: " << a_paletteTypeName << std::endl;
    }
    return 0;
}

void EgOnePalette::addPaletteWidgetData(const std::string& nodesSetName, const std::string& widgetName, EgDataNodeIDType nodeID, uint32_t paletteIndex, uint32_t paletteFill) {
    EgDataNode* newNode = new EgDataNode(paletteStorage.dataNodeBlueprint);
    (*newNode)[paletteFieldIndexName]  << paletteIndex;
    (*newNode)[paletteFieldNodesName]  << nodesSetName;
    (*newNode)[paletteFieldNodeIDName] << nodeID;
    (*newNode)[paletteWidgetNameName]  << widgetName;
    (*newNode)[paletteWidgetColorName] << paletteFill;
    paletteStorage << newNode;
    paletteStorage.Store();
}

/*

void EgOnePalette::createBlankLayer(EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H, const std::string& nodesName, const std::string& linksName) {
    addLayerData(newLayerID, parentLayerID, W, H, nodesName, linksName);
}

void EgOnePalette::createDetailsLayer(EgDataNodeIDType parentNodeID, EgDataNodeIDType& newLayerID, EgDataNodeIDType parentLayerID, uint32_t W, uint32_t H,
    const std::string& parentNodesName, const std::string& layerNodesBlueprint, const std::string& layerLinksBlueprint) {
    std::string newNodesName = parentNodesName + "_" + std::to_string(parentNodeID); // gen nodesSet and linksSet names
    std::string newLinksName = parentNodesName + "_" + std::to_string(parentNodeID);
    // EG_LOG_STUB << "newNodesName: " << newNodesName << " newLinksName: " << newLinksName << FN;
    metaInfoDatabase-> CreateNodesSetByBlueprint(newNodesName, layerNodesBlueprint);
    metaInfoDatabase-> CreateLinksSetByBlueprint(newLinksName, layerLinksBlueprint, newNodesName, newNodesName);
    addLayerData(newLayerID, parentLayerID, W, H, newNodesName, newLinksName);
}

void EgOnePalette::createNewTopLayer(EgDataNodeIDType oldLayerID, EgDataNodeIDType& newLayerID, uint32_t W, uint32_t H, const std::string& oldLayerName,
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

    addLayerData(newLayerID, 0, W, H, newNodesName, newLinksName);
    updateParentID(oldLayerID, newLayerID);
    metaInfoDatabase-> ConnectPaletteMetainfo();
    metaInfoDatabase->paletteMetainfo-> updateTopLayerID(paletteTypeName, newLayerID); // update top layer ID in metainfo
}

void EgOnePalette::updateWH(EgDataNodeIDType layerID, uint32_t W, uint32_t H) {
    // EG_LOG_STUB << "layerID: " << DEC << layerID << " paletteStorage: " << paletteStorage.nodesSetName << " paletteBP: " << paletteStorage.nodeBlueprintName << FN;
    // EgDataNode& updNode = paletteStorage[layerID];
    // PrintEgDataNodeFields (paletteStorage[layerID]);
    paletteStorage[layerID]["layerWidth"]  << W; // FIXME literals
    paletteStorage[layerID]["layerHeight"] << H;
    paletteStorage.MarkUpdatedDataNode(layerID);
    paletteStorage.Store();
    auto paletteIter = paletteMap.find(layerID); // update loaded oneLayer info
    if (paletteIter != paletteMap.end()) {
        paletteIter->second-> layerWidth  = W;
        paletteIter->second-> layerHeight = H;
    }
}

void EgOnePalette::updateParentID(EgDataNodeIDType layerID, EgDataNodeIDType parentLayerID) {
    EgDataNode& updNode = paletteStorage[layerID];
    updNode["parentLayerID"] << parentLayerID;
    paletteStorage.MarkUpdatedDataNode(layerID);
    paletteStorage.Store();
    auto paletteIter = paletteMap.find(layerID); // update loaded oneLayer info
    if (paletteIter != paletteMap.end())
        paletteIter->second-> parentLayerID = parentLayerID;
}

*/

/* void EgOnePaletteType::AddLinksType(const std::string& nodesName, EgDataNodeIDType layerNum) {
    EgDataNode *newNode = new EgDataNode(paletteStorage.dataNodeBlueprint);
    (*newNode)["name"]     << nodesName;
    (*newNode)["isLink"]   << (uint8_t) 0xFF;
    (*newNode)["layerNum"] << layerNum;
    // PrintEgDataNodeFields(*newNode);
    paletteStorage << newNode;
} */

/*
void EgOnePalette::getLayerNodesAndLinks(std::unordered_set<std::string>& nodesNames, std::unordered_set<std::string>& linksNames, 
                                         EgDataNodeIDType layerNum) {
    nodesNames.clear();
    linksNames.clear();
    EgDataNodeIDType theLayer;
    uint8_t isLink;
    std::string theName;
    if (paletteStorage.isConnected) {
        for (auto nodesIter : paletteStorage.dataMap) {
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

int EgOnePalette::LoadPalette() {
    clear();
    int res = paletteStorage.LoadAllNodes();
    if (res) {
        std::cout << "LoadPalette() ERROR: " << paletteTypeName << std::endl;
    } /* else {
        for (auto paletteIter : paletteStorage.dataMap) { // load each node separately
            // EgDataNodeIDType parentLayerID;
            int32_t paletteIndex;
            (*(paletteIter.second))[paletteFieldIndexName] >> paletteIndex; // FIXME literals
            EgDataNode* newNode;
            // newNode = new EgDataNode(dataNodeBlueprint);
            // newNode-> dataFileOffset = nextOffset;
            // LocalNodesFile-> ReadDataNode(newNode, nextOffset);
            // dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
        }
    } */
    return res;
}

int EgOnePalette::StorePalette() {
    return paletteStorage.Store();
}

/*
EgOneLayer* EgOnePalette::operator[](EgDataNodeIDType layerID) {
    auto iter = paletteMap.find(layerID);
    if (iter != paletteMap.end())
        return iter->second;
    return nullptr;
} *&/

/*void PrintPaletteInfo() {

}*/