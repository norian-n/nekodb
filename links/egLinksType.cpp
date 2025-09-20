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

void EgLinksType::ConnectLinkToNodesTypes(EgDataNodesType &from, EgDataNodesType &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
}

int EgLinksType::ResolveNodesIDsToPtrs() {
    if (!(fromDataNodes && fromDataNodes))
    {
        std::cout << "ResolveNodesIDsToPtrs() Error : nodes types not connected for link type " << linkTypeName << std::endl;
        return -1;
    }
    // std::cout  << "ResolveLinksToPtrs() of \"" << linkTypeName << "\"" << std::endl;
    for (auto nodesIter : dataMap)
    {
        // std::cout  << (int) *(nodesIter.second->operator[]("fromID").arrayData) << " -> "
        //          << (int) *(nodesIter.second->operator[]("toID").arrayData);
        EgDataNodeType *fromNodePtr = fromDataNodes->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("fromID").arrayData));
        EgDataNodeType *toNodePtr = toDataNodes->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("toID").arrayData));
        // connect
        if (fromNodePtr && toNodePtr)
        { // <EgBlueprintIDType, std::vector<EgDataNodeType*> >
            // std::cout  << " Ok " << std::endl;
            auto iterFrom = fromNodePtr->outLinks.find(linkTypeID);
            if (iterFrom == fromNodePtr->outLinks.end())
            {
                std::vector<EgDataNodeType *> newNodePtrs;
                newNodePtrs.push_back(toNodePtr);
                fromNodePtr->outLinks.insert(std::pair<EgBlueprintIDType, std::vector<EgDataNodeType *>>(linkTypeID, newNodePtrs));
            }
            else
                iterFrom->second.push_back(toNodePtr);

            auto iterTo = toNodePtr->inLinks.find(linkTypeID);
            if (iterTo == toNodePtr->inLinks.end())
            {
                std::vector<EgDataNodeType *> newNodePtrs;
                newNodePtrs.push_back(fromNodePtr);
                toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, std::vector<EgDataNodeType *>>(linkTypeID, newNodePtrs));
            }
            else
                iterTo->second.push_back(fromNodePtr);
        }
        else
        {
            std::cout << linkTypeName << " : ResolveNodesIDsToPtrs() Error : Nodes ptrs NOT found for some IDs " << (int)*(nodesIter.second->operator[]("fromID").arrayData) << " and/or "
                      << (int)*(nodesIter.second->operator[]("toID").arrayData) << std::endl;
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
                std::cout << "OutLinkTypeID: " << (int)outLinksIter.first << " Node ID: " << (int)outNodesIter->dataNodeID << std::endl;
    }
    if (node.inLinks.size()) {
        std::cout << "IN links of node " << std::dec << (int)node.dataNodeID << " : ";
        for (auto inLinksIter : node.inLinks) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            for (auto inNodesIter : inLinksIter.second)
                std::cout << "InLinkTypeID: " << (int)inLinksIter.first << " Node ID: " << (int)inNodesIter->dataNodeID << std::endl;
    }
}