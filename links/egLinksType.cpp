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

    dataMap = linksDataStorage.dataMap; // reset reference

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

int EgLinksType::AddLinkPtrsToNodes(EgDataLinkIDType linkID,EgDataNodeType &from, EgDataNodeType &to) {
    EgDataLinksMapType newNodePtrsFrom;
    newNodePtrsFrom.insert(std::pair<EgDataLinkIDType, EgDataNodeType *>(linkID, &to));
    from.outLinks.insert(std::pair<EgBlueprintIDType, EgDataLinksMapType>(linkTypeID, newNodePtrsFrom));

    EgDataLinksMapType newNodePtrsTo;
    newNodePtrsTo.insert(std::pair<EgDataLinkIDType, EgDataNodeType *>(linkID, &from));
    to.inLinks.insert(std::pair<EgBlueprintIDType, EgDataLinksMapType>(linkTypeID, newNodePtrsTo));

    return 0;
}

int EgLinksType::ResolveNodesIDsToPtrs(EgDataNodesType &from, EgDataNodesType &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
    /* if (!(fromDataNodes && toDataNodes))
    {
        std::cout << "ResolveNodesIDsToPtrs() Error : nodes types not connected for link type " << linkTypeName << std::endl;
        return -1;
    } */
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

    for (auto nodesIter : dataMap)
    {
        // std::cout  << std::dec << (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("fromID").arrayData)) << " -> "
        //           <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("toID").arrayData)) << std::endl;
        EgDataNodeType *fromNodePtr = fromDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("fromID").arrayData)));
        EgDataNodeType *toNodePtr = toDataNodes-> GetNodePtrByID(*(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("toID").arrayData)));
        // connect
        if (fromNodePtr && toNodePtr)
        { // <EgBlueprintIDType, std::vector<EgDataNodeType*> >
            // std::cout  << " Ok " << std::endl;
            auto iterFrom = fromNodePtr->outLinks.find(linkTypeID);
            if (iterFrom == fromNodePtr->outLinks.end())
            {
                EgDataLinksMapType newNodePtrs; // std::vector<EgDataNodeType *> newNodePtrs;
                // newNodePtrs.push_back(toNodePtr);
                newNodePtrs.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgBlueprintIDType, EgDataLinksMapType>(linkTypeID, newNodePtrs));
            }
            else
                // iterFrom->second.push_back(toNodePtr);
                iterFrom->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, toNodePtr));

            auto iterTo = toNodePtr->inLinks.find(linkTypeID);
            if (iterTo == toNodePtr->inLinks.end())
            {
                // std::vector<EgDataNodeType *> newNodePtrs;
                // newNodePtrs.push_back(fromNodePtr);
                // toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, std::vector<EgDataNodeType *>>(linkTypeID, newNodePtrs));

                EgDataLinksMapType newNodePtrs; // std::vector<EgDataNodeType *> newNodePtrs;
                // newNodePtrs.push_back(toNodePtr);
                newNodePtrs.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, EgDataLinksMapType>(linkTypeID, newNodePtrs));
            }
            else
                // iterTo->second.push_back(fromNodePtr);
                iterTo->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, fromNodePtr));
        }
        else
        {
            std::cout << " : ResolveNodesIDsToPtrs() ERROR : Nodes ptrs NOT found for some IDs of " << linksDataStorage.dataNodesName << std::dec 
                << " " << (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("fromID").arrayData)) << " -> "
                <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (nodesIter.second->operator[]("toID").arrayData)) << std::endl;
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