#include <iostream>
#include "egLinks.h"
#include "../metainfo/egLiterals.h"

void EgLinksSet::clear() {
    linksDataStorage.clear();
}

int EgLinksSet::ConnectLinks(const std::string& linkNameStr, EgDatabase& myDB) {
    if (linksDataStorage.isConnected) {
        EG_LOG_STUB << "WARNING: ConnectLinks(): trying to connect links set again: " << linkNameStr << FN;
    }
    metaInfoDatabase = &myDB;
    linkNameShort = linkNameStr;
    linkNameFull = linkNameStr + linkSuffixName;
    if ( myDB.blueprintNameByLinkName(linkNameStr, linkBlueprintName) ) { // look in links metainfo
        EG_LOG_STUB << "ERROR: ConnectLinks() set name not found in metainfo: " << linkNameStr << FN;
        return -1;
    }
    linkBlueprintNameFull = linkBlueprintName + linkSuffixName;
    if (linksDataStorage.ConnectSystemNodeType(linkNameFull, linkBlueprintNameFull) != 0) {
        EG_LOG_STUB << "ERROR: ConnectLinks() can't find storage: " << linkNameFull << " or blueprint " << linkBlueprintNameFull << FN;
        return -2;        
    }
    dataNodeBlueprint = linksDataStorage.dataNodeBlueprint; // shortcut ref FIXME check usage
    return 0;
}

void EgLinksSet::AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID) {
    EgDataNode *newNode = new EgDataNode(linksDataStorage.dataNodeBlueprint);
    *newNode << fromID;
    *newNode << toID;
    // PrintEgDataNodeTypeFields(*newNode);
    linksDataStorage << newNode;
}

int EgLinksSet::AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID) {
    EgDataNode *fromNodePtr = fromDataNodes->GetNodePtrByID(fromID);
    EgDataNode *toNodePtr = toDataNodes->GetNodePtrByID(toID);
    if (!(fromNodePtr && toNodePtr))
    {
        std::cout << linkNameShort << " : AddContainersLink() ERROR : Nodes ptrs NOT found, tried IDs:"
                  << std::dec << (int)fromID << " " << (int)toID << std::endl;
        return -1;
    }
    AddRawLink(fromID, toID);
    return 0;
}

void EgLinksSet::DeleteArrowLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (fromDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[]("fromID").dataChunk));
        EgDataNode* nodePtr = fromDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteOutLink(linkBlueprintID, linkDataNode);
    }
    if (toDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[]("toID").dataChunk));
        EgDataNode* nodePtr = toDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteInLink(linkBlueprintID, linkDataNode);
    }
    std::cout << "DeleteBidirectLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

void EgLinksSet::DeleteOutLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (toDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[]("toID").dataChunk));
        EgDataNode* nodePtr = toDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteInLink(linkBlueprintID, linkDataNode);
    }
    std::cout << "DeleteOutLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

void EgLinksSet::DeleteInLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (fromDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[]("fromID").dataChunk));
        EgDataNode* nodePtr = fromDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteOutLink(linkBlueprintID, linkDataNode);
    }
    std::cout << "DeleteInLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

int EgLinksSet::LoadLinks() {
    linksDataStorage.clear();
    return linksDataStorage.LoadAllNodes();
}

int EgLinksSet::StoreLinks() {
    return linksDataStorage.Store();
}
/*
void EgLinksType::ConnectLinkToNodesTypes(EgDataNodesType &from, EgDataNodesType &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
} */

int EgLinksSet::AddLinkPtrsToNodes(EgDataNode& link, EgDataNode &from, EgDataNode &to) {
    // std::cout << "AddLinkPtrsToNodes() linkID " << link.dataNodeID << " fromID: " << from.dataNodeID << " toID: " << to.dataNodeID << std::endl;
    auto iterFrom = from.outLinks.find(linkBlueprintID);
    if (iterFrom == from.outLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsFrom;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsFrom;
        newNodePtrsFrom.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &to));
        from.outLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsFrom));
    }
    else
        iterFrom->second.insert(std::pair<EgDataNode*, EgDataNode *>(&link, &to));

    auto iterTo = to.inLinks.find(linkBlueprintID);
    if (iterTo == to.inLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsTo;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
        newNodePtrsTo.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &from));
        to.inLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsTo));
    }
    else
        iterTo->second.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &from));

    return 0;
}

int EgLinksSet::ResolveNodesIDsToPtrs(EgDataNodesSet &from, EgDataNodesSet &to) {
    fromDataNodes = from.nodesContainer;
    toDataNodes = to.nodesContainer;
    if (!(fromDataNodes && toDataNodes))
    {
        std::cout << "ResolveNodesIDsToPtrs() Error : nodes types not connected for link type " << linkNameShort << std::endl;
        return -1;
    }
    for (auto linksDataIter : dataMap)
    {
        // std::cout  << std::dec << (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").dataChunk)) << " -> "
        //           <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").dataChunk)) << std::endl;
        EgDataNodeIDType fromNodeID = *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").dataChunk));
        EgDataNode *fromNodePtr = fromDataNodes-> GetNodePtrByID(fromNodeID);
        EgDataNodeIDType toNodeID = *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").dataChunk));
        EgDataNode *toNodePtr = toDataNodes-> GetNodePtrByID(toNodeID);
        if (fromNodePtr && toNodePtr) {
            auto iterFrom = fromNodePtr->outLinks.find(linkBlueprintID);
            if (iterFrom == fromNodePtr->outLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrs;
                newNodePtrs.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrs));
            } else
                iterFrom->second.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, toNodePtr));
            auto iterTo = toNodePtr->inLinks.find(linkBlueprintID);
            if (iterTo == toNodePtr->inLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
                newNodePtrsTo.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType>(linkBlueprintID, newNodePtrsTo));
            } else
                iterTo->second.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, fromNodePtr));
        }
        else
        {
            std::cout << " : ResolveNodesIDsToPtrs() ERROR : Nodes ptrs NOT found for some IDs of " << linksDataStorage.nodesSetName << std::dec 
                << " " << (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("fromID").dataChunk)) << " -> "
                <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[]("toID").dataChunk)) << std::endl;
            return -2;
        }
    }
    return 0;
}

// ======================== Debug ========================

void PrintResolvedLinks(const EgDataNode& node) {
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