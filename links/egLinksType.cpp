#include <iostream>
#include "egLinksType.h"

void EgLinksType::clear() {
    // linksStorage->clear();
}
/*
void EgLinksType::initLinkBlueprint(EgDataNodeBlueprintType *linkBlueprint) {
    linkBlueprint-> BlueprintInitStart();
    linkBlueprint-> AddDataFieldName("fromID");
    linkBlueprint-> AddDataFieldName("toID");
    linkBlueprint-> blueprintSettings.isServiceType = true;
    linkBlueprint-> blueprintMode = egBlueprintActive; // virtual, do NOT commit to db
    // linkBlueprint-> BlueprintInitCommit();
}
*/
int EgLinksType::ConnectLinks(std::string& linkNameStr, EgDatabaseType& myDB) {

    metaInfoDatabase = &myDB;

    if (linksDataStorage.ConnectSystemNodeType(linkNameStr + "_arrowLinks") != 0) {
        std::cout << "ConnectLinks() not found storage: " << linkNameStr << std::endl;
    }

    return 0;
}

void EgLinksType::AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID) {
    EgDataNodeType *newNode = new EgDataNodeType(linksDataStorage.dataNodeBlueprint);
    *newNode << fromID;
    *newNode << toID;
    // PrintEgDataNodeTypeFields(*newNode);
    linksDataStorage << newNode;
}

int EgLinksType::AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID) {
    // check if nodes exists in the containers
    EgDataNodeType *fromNodePtr = fromDataNodes->GetNodePtrByID(fromID);
    EgDataNodeType *toNodePtr = toDataNodes->GetNodePtrByID(toID);
    // connect
    if (!(fromNodePtr && toNodePtr))
    {
        std::cout << linkTypeName << " : AddContainersLink() Error : Nodes ptrs NOT found for some IDs "
                  << std::dec << (int)fromID << " " << (int)toID << std::endl;
        return -1;
    }
    AddRawLink(fromID, toID);
    return 0;
}

int EgLinksType::LoadLinks() {
    linksDataStorage.clear();
    return linksDataStorage.LoadAllNodes();
}

int EgLinksType::StoreLinks() {
    return linksDataStorage.Store();
}
/*
void EgLinksType::ConnectLinkToNodesTypes(EgDataNodesType &from, EgDataNodesType &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
} */

int EgLinksType::AddLinkPtrsToNodes(EgDataNodeType& link,EgDataNodeType &from, EgDataNodeType &to) {
    std::cout << "AddLinkPtrsToNodes() linkID " << link.dataNodeID << " fromID: " << from.dataNodeID << " toID: " << to.dataNodeID << std::endl;
    
    auto iterFrom = from.outLinks.find(linkBlueprintID);
    if (iterFrom == from.outLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsFrom;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsFrom;
        newNodePtrsFrom.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(&link, &to));
        from.outLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsFrom));
    }
    else
        iterFrom->second.insert(std::pair<EgDataNodeType*, EgDataNodeType *>(&link, &to));

    auto iterTo = to.inLinks.find(linkBlueprintID);
    if (iterTo == to.inLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsTo;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
        newNodePtrsTo.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(&link, &from));
        to.inLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsTo));
    }
    else
        iterTo->second.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(&link, &from));

    return 0;
}

int EgLinksType::ResolveNodesIDsToPtrs(EgDataNodesType &from, EgDataNodesType &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
    if (!(fromDataNodes && toDataNodes))
    {
        std::cout << "ResolveNodesIDsToPtrs() Error : nodes types not connected for link type " << linkTypeName << std::endl;
        return -1;
    }
    // std::cout  << "ResolveLinksToPtrs() of \"" << linksDataStorage.dataNodesName << "\"" << std::endl;
    /*for (auto nodesIter : dataMap)
    {
        std::cout  << std::dec << (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("fromID").arrayData)) << " -> "
                   <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("toID").arrayData)) << std::endl;
        EgDataNodeType *fromNodePtr = fromDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("fromID").arrayData)));
        EgDataNodeType *toNodePtr = toDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("toID").arrayData)));
        // connect
        if (fromNodePtr && toNodePtr) {
            fromNodePtr-> outLinks.clear();
            fromNodePtr-> inLinks.clear();
            toNodePtr-> outLinks.clear();
            toNodePtr-> inLinks.clear();            
        }
    } */

    for (auto linksDataIter : dataMap)
    {
        // std::cout  << std::dec << (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").arrayData)) << " -> "
        //           <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").arrayData)) << std::endl;
        EgDataNodeType *fromNodePtr = fromDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").arrayData)));
        EgDataNodeType *toNodePtr = toDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").arrayData)));
        // connect
        if (fromNodePtr && toNodePtr)
        { // <EgBlueprintIDType, std::vector<EgDataNodeType*> >
            // std::cout  << " Ok " << std::endl;
            auto iterFrom = fromNodePtr->outLinks.find(linkBlueprintID);
            if (iterFrom == fromNodePtr->outLinks.end()) {
                // EgLinkIDsNodePtrsMapType newNodePtrs;
                EgLinkDataPtrsNodePtrsMapType newNodePtrs;
                newNodePtrs.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(linksDataIter.second, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrs));
            }
            else
                iterFrom->second.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(linksDataIter.second, toNodePtr));

            auto iterTo = toNodePtr->inLinks.find(linkBlueprintID);
            if (iterTo == toNodePtr->inLinks.end()) {
                // EgLinkIDsNodePtrsMapType newNodePtrsTo;
                EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
                newNodePtrsTo.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(linksDataIter.second, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsTo));
            }
            else
                iterTo->second.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(linksDataIter.second, fromNodePtr));
        }
        else
        {
            std::cout << " : ResolveNodesIDsToPtrs() ERROR : Nodes ptrs NOT found for some IDs of " << linksDataStorage.dataNodesName << std::dec 
                << " " << (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").arrayData)) << " -> "
                <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").arrayData)) << std::endl;
            return -2;
        }
    }
    return 0;
}

// ======================== Debug ========================

void PrintResolvedLinks(const EgDataNodeType& node) {
    if (node.outLinks.size()) {
        std::cout << "OUT links of node " << std::dec << (int)node.dataNodeID << " : ";
        for (auto outLinksIter : node.outLinks) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            for (auto outNodesIter : outLinksIter.second)
                std::cout << "OutLinkTypeID: " << (int)outLinksIter.first << " Node ID: " << (int)outNodesIter.second->dataNodeID << std::endl;
    }
    if (node.inLinks.size()) {
        std::cout << "IN links of node " << std::dec << (int)node.dataNodeID << " : ";
        for (auto inLinksIter : node.inLinks) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            for (auto inNodesIter : inLinksIter.second)
                std::cout << "InLinkTypeID: " << (int)inLinksIter.first << " Node ID: " << (int)inNodesIter.second->dataNodeID << std::endl;
    }
}