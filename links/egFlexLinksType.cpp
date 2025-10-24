#include <iostream>
#include "egFlexLinksType.h"

void EgFreeLinksType::clear() {
    linksStorage->clear();
}

void EgFreeLinksType::initFlexLinkBlueprint(EgDataNodeBlueprintType *linkBlueprint) {
    linkBlueprint->BlueprintInitStart();
    linkBlueprint->AddDataFieldName("fromBlueprintID");
    linkBlueprint->AddDataFieldName("fromID");
    linkBlueprint->AddDataFieldName("toBlueprintID");
    linkBlueprint->AddDataFieldName("toID");
    linkBlueprint->blueprintSettings.isServiceType = true;
    linkBlueprint->blueprintMode = egBlueprintActive; // virtual, do NOT commit to db
}

void EgFreeLinksType::AddRawLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID) {
    EgDataNodeType *newNode = new EgDataNodeType(linksStorageBlueprint);
    *newNode << fromLayID;
    *newNode << fromID;
    *newNode << toLayID;
    *newNode << toID;
    // PrintEgDataNodeTypeFields(*newNode);
    *linksStorage << newNode;
}

void EgFreeLinksType::AddContainersLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID) {
    // TODO check if nodes exists in the containers
    AddRawLink(fromLayID, fromID, toLayID, toID);
}

int EgFreeLinksType::LoadLinks() {
    linksStorage->clear();
    return linksStorage->LoadAllLocalFileNodes();
}

int EgFreeLinksType::StoreLinks() {
    return linksStorage->StoreToLocalFile();
}

int EgFreeLinksType::ResolveNodesIDsToPtrs() {
    int res{0};
    if (!metaInfoDatabase)
    {
        std::cout << "ResolveLinksToPtrs() Error : meta info database not connected for " << linkTypeName << std::endl;
        return -1;
    }
    // std::cout  << "ResolveLinksToPtrs() of \"" << linkTypeName << "\"" << std::endl;
    for (auto nodesIter : linksStorage->dataNodes)
    {
        // std::cout  << (int) *(nodesIter.second->operator[]("fromID").arrayData) << " -> "
        //          << (int) *(nodesIter.second->operator[]("toID").arrayData);
        EgDataNodesType *fromDataNodes =
            (metaInfoDatabase->GetNodeTypePtrByID((EgBlueprintIDType) * (nodesIter.second->operator[]("fromBlueprintID").arrayData)));
        if (!fromDataNodes)
        {
            std::cout << "ResolveNodesIDsToPtrs() Error : FROM nodes type not found for " << linkTypeName << std::endl;
            res = -1;
            continue;
        }
        // std::cout  << "ResolveNodesIDsToPtrs() : FROM nodes type found " <<  linkTypeName << std::endl;
        EgDataNodeType *fromNodePtr = fromDataNodes->nodesContainer->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("fromID").arrayData));
        EgDataNodesType *toDataNodes =
            (metaInfoDatabase->GetNodeTypePtrByID((EgBlueprintIDType) * (nodesIter.second->operator[]("toBlueprintID").arrayData)));
        if (!toDataNodes)
        {
            std::cout << "ResolveNodesIDsToPtrs() Error : TO nodes type not found for " << linkTypeName << std::endl;
            res = -1;
            continue;
        }
        // std::cout  << "ResolveNodesIDsToPtrs() : TO nodes type found " <<  linkTypeName << std::endl;
        EgDataNodeType *toNodePtr = toDataNodes->nodesContainer->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("toID").arrayData));
        // connect
        if (fromNodePtr && toNodePtr)
        { // <EgBlueprintIDType, std::vector<EgDataNodeType*> >
            // std::cout  << " Ok " << std::endl;
            auto iterFrom = fromNodePtr->outLinks.find(linkBlueprintID);
            if (iterFrom == fromNodePtr->outLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrs;
                newNodePtrs.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(nodesIter.second, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrs));
            } else
                // iterFrom->second.push_back(toNodePtr);
                // iterFrom->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, toNodePtr));
                iterFrom->second.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(nodesIter.second, toNodePtr));

            auto iterTo = toNodePtr->inLinks.find(linkBlueprintID);
            if (iterTo == toNodePtr->inLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
                newNodePtrsTo.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(nodesIter.second, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsTo));
            } else
                // iterTo->second.push_back(fromNodePtr);
                // iterTo->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, fromNodePtr));
                iterTo->second.insert(std::pair<EgDataNodeType*, EgDataNodeType*>(nodesIter.second, fromNodePtr));

        }
        else
        {
            std::cout << linkTypeName << " : ResolveNodesIDsToPtrs() Error : Nodes ptrs NOT found for some IDs " << (int)*(nodesIter.second->operator[]("fromID").arrayData) << " and/or "
                      << (int)*(nodesIter.second->operator[]("toID").arrayData) << std::endl;
            res = -2;
        }
    }
    return res;
}

// ======================== Debug ========================

void PrintResolvedLinksFlex(const EgDataNodeType& node) {
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