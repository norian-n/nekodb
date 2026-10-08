#include <iostream>
#include "egFreeLinksSet.h"
#include "egLiteralsLinks.h"
#include "../metainfo/egLiteralsMetainfo.h"

void EgFreeLinksSet::clear() {
    linksDataStorage.clear();
}

int EgFreeLinksSet::ConnectLinks(const std::string& linkNameStr, EgDatabase& myDB) {
    if (linksDataStorage.isConnected) {
        EG_LOG_STUB << "WARNING: ConnectLinks(): trying to connect links set again: " << linkNameStr << FN;
    }
    metaInfoDatabase = &myDB;
    linkNameShort = linkNameStr;
    linkNameFull = linkNameStr + linkPostfixName;
    if ( myDB.blueprintNameByLinkName(linkNameStr, linkBlueprintName, linksSetID) ) { // look in links metainfo
        EG_LOG_STUB << "ERROR: ConnectLinks() set name not found in metainfo: " << linkNameStr << FN;
        return -1;
    }
    linkBlueprintNameFull = linkBlueprintName + linkPostfixName;
    if ( ! linksDataStorage.ConnectSystemNodesSet(linkNameFull, linkBlueprintNameFull) ) {
        EG_LOG_STUB << "ERROR: ConnectLinks() can't find storage: " << linkNameFull << " or blueprint " << linkBlueprintNameFull << FN;
        return -2;        
    }
    dataNodeBlueprint = linksDataStorage.dataNodeBlueprint; // shortcut ref FIXME check usage
    return 0;
}

void EgFreeLinksSet::AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID, const std::string& fromSetName, const std::string& toSetName) {
    EgDataNode *newNode = new EgDataNode(linksDataStorage.dataNodeBlueprint);
    (*newNode)[linksFromID_FieldName]      << fromID;
    (*newNode)[linksToID_FieldName]        << toID;
    (*newNode)[linksFromSetName_FieldName] << fromSetName;
    (*newNode)[linksToSetName_FieldName]   << toSetName;
    linksDataStorage << newNode;
}

/*
int EgFreeLinksSet::AddNodeContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID) {
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
} */

void EgFreeLinksSet::DeleteArrowLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (fromDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[](linksFromID_FieldName).dataChunk));
        EgDataNode* nodePtr = fromDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteOutLink(linksSetID, linkDataNode);
    }
    if (toDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[](linksToID_FieldName).dataChunk));
        EgDataNode* nodePtr = toDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteInLink(linksSetID, linkDataNode);
    }
    std::cout << "DeleteBidirectLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

void EgFreeLinksSet::DeleteOutLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (toDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[](linksToID_FieldName).dataChunk));
        EgDataNode* nodePtr = toDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteInLink(linksSetID, linkDataNode);
    }
    std::cout << "DeleteOutLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

void EgFreeLinksSet::DeleteInLink(EgDataLinkIDType linkID) {
    EgDataNode*  linkDataNode = &(linksDataStorage[linkID]);
    if (fromDataNodes) {
        EgDataNodeIDType nodeID = *(reinterpret_cast<EgDataNodeIDType *>(linkDataNode->operator[](linksFromID_FieldName).dataChunk));
        EgDataNode* nodePtr = fromDataNodes->GetNodePtrByID(nodeID);
        if (nodePtr)
            nodePtr-> deleteOutLink(linksSetID, linkDataNode);
    }
    std::cout << "DeleteInLink() link ID: " << std::dec << linkID << std::endl;
    linksDataStorage.DeleteDataNode(linkID);
}

int EgFreeLinksSet::LoadLinks() {
    linksDataStorage.clear();
    return linksDataStorage.LoadAllNodes();
}

int EgFreeLinksSet::StoreLinks() {
    return linksDataStorage.Store();
}


/*
int EgFreeLinksSet::AddLinkPtrsToNodes(EgDataNode& link, EgDataNode &from, EgDataNode &to) {
    EG_LOG_STUB << "AddLinkPtrsToNodes() linksSetID " << DEC << linksSetID << FN;
    // std::cout << "AddLinkPtrsToNodes() linkID " << link.dataNodeID << " fromID: " << from.dataNodeID << " toID: " << to.dataNodeID << std::endl;
    auto iterFrom = from.outLinks.find(linksSetID);
    if (iterFrom == from.outLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsFrom;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsFrom;
        newNodePtrsFrom.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &to));
        from.outLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrsFrom));
    }
    else
        iterFrom->second.insert(std::pair<EgDataNode*, EgDataNode *>(&link, &to));

    auto iterTo = to.inLinks.find(linksSetID);
    if (iterTo == to.inLinks.end()) {
        // EgLinkIDsNodePtrsMapType newNodePtrsTo;
        EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
        newNodePtrsTo.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &from));
        to.inLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrsTo));
    }
    else
        iterTo->second.insert(std::pair<EgDataNode*, EgDataNode*>(&link, &from));

    return 0;
}

int EgFreeLinksSet::ResolveNodesIDsToPtrs(EgDataNodesSet &from, EgDataNodesSet &to) {
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
        //           <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[](linksToID_FieldName).dataChunk)) << std::endl;
        EgDataNodeIDType fromNodeID = *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[](linksFromID_FieldName).dataChunk));
        EgDataNode *fromNodePtr = fromDataNodes-> GetNodePtrByID(fromNodeID);
        EgDataNodeIDType toNodeID = *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[](linksToID_FieldName).dataChunk));
        EgDataNode *toNodePtr = toDataNodes-> GetNodePtrByID(toNodeID);
        if (fromNodePtr && toNodePtr) {
            auto iterFrom = fromNodePtr->outLinks.find(linksSetID);
            if (iterFrom == fromNodePtr->outLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrs;
                newNodePtrs.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrs));
            } else
                iterFrom->second.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, toNodePtr));
            auto iterTo = toNodePtr->inLinks.find(linksSetID);
            if (iterTo == toNodePtr->inLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
                newNodePtrsTo.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrsTo));
            } else
                iterTo->second.insert(std::pair<EgDataNode*, EgDataNode*>(linksDataIter.second, fromNodePtr));
        }
        else
        {
            std::cout << " : ResolveNodesIDsToPtrs() ERROR : Nodes ptrs NOT found for some IDs of " << linksDataStorage.nodesSetName << std::dec 
                << " " << (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[](linksFromID_FieldName).dataChunk)) << " -> "
                <<  (int) *(reinterpret_cast<EgDataNodeIDType*> (linksDataIter.second->operator[](linksToID_FieldName).dataChunk)) << std::endl;
            return -2;
        }
    }
    return 0;
}

int EgFreeLinksSet::ResolveNodesIDsToPtrs2() {
    int res{0};
    if (!metaInfoDatabase)
    {
        std::cout << "ResolveLinksToPtrs() Error : meta info database not connected for " << linkNameShort << std::endl;
        return -1;
    }
    // std::cout  << "ResolveLinksToPtrs() of \"" << linkNameShort << "\"" << std::endl;
    for (auto nodesIter : linksDataStorage.dataMap)
    {
        // std::cout  << (int) *(nodesIter.second->operator[]("fromID").dataChunk) << " -> "
        //          << (int) *(nodesIter.second->operator[]("toID").dataChunk);
        EgDataNodesSet *fromDataNodes =
            (metaInfoDatabase->GetNodesTypePtrByID((EgNodesSetIDType) * (nodesIter.second->operator[]("fromBlueprintID").dataChunk)));
        if (!fromDataNodes)
        {
            std::cout << "ResolveNodesIDsToPtrs() Error : FROM nodes type not found for " << linkNameShort << std::endl;
            res = -1;
            continue;
        }
        // std::cout  << "ResolveNodesIDsToPtrs() : FROM nodes type found " <<  linkNameShort << std::endl;
        EgDataNode *fromNodePtr = fromDataNodes->nodesContainer->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("fromID").dataChunk));
        EgDataNodesSet *toDataNodes =
            (metaInfoDatabase->GetNodesTypePtrByID((EgNodesSetIDType) * (nodesIter.second->operator[]("toBlueprintID").dataChunk)));
        if (!toDataNodes)
        {
            std::cout << "ResolveNodesIDsToPtrs() Error : TO nodes type not found for " << linkNameShort << std::endl;
            res = -1;
            continue;
        }
        // std::cout  << "ResolveNodesIDsToPtrs() : TO nodes type found " <<  linkNameShort << std::endl;
        EgDataNode *toNodePtr = toDataNodes->nodesContainer->GetNodePtrByID((EgDataNodeIDType) * (nodesIter.second->operator[]("toID").dataChunk));
        // connect
        if (fromNodePtr && toNodePtr)
        { // <EgBlueprintIDType, std::vector<EgDataNodeType*> >
            // std::cout  << " Ok " << std::endl;
            auto iterFrom = fromNodePtr->outLinks.find(linksSetID);
            if (iterFrom == fromNodePtr->outLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrs;
                newNodePtrs.insert(std::pair<EgDataNode*, EgDataNode*>(nodesIter.second, toNodePtr));
                fromNodePtr->outLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrs));
            } else
                // iterFrom->second.push_back(toNodePtr);
                // iterFrom->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, toNodePtr));
                iterFrom->second.insert(std::pair<EgDataNode*, EgDataNode*>(nodesIter.second, toNodePtr));

            auto iterTo = toNodePtr->inLinks.find(linksSetID);
            if (iterTo == toNodePtr->inLinks.end()) {
                EgLinkDataPtrsNodePtrsMapType newNodePtrsTo;
                newNodePtrsTo.insert(std::pair<EgDataNode*, EgDataNode*>(nodesIter.second, fromNodePtr));
                toNodePtr->inLinks.insert(std::pair<EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType>(linksSetID, newNodePtrsTo));
            } else
                // iterTo->second.push_back(fromNodePtr);
                // iterTo->second.insert(std::pair<EgBlueprintIDType, EgDataNodeType *>(nodesIter.first, fromNodePtr));
                iterTo->second.insert(std::pair<EgDataNode*, EgDataNode*>(nodesIter.second, fromNodePtr));

        }
        else
        {
            std::cout << linkNameShort << " : ResolveNodesIDsToPtrs() Error : Nodes ptrs NOT found for some IDs " << (int)*(nodesIter.second->operator[]("fromID").dataChunk) << " and/or "
                      << (int)*(nodesIter.second->operator[]("toID").dataChunk) << std::endl;
            res = -2;
        }
    }
    return res;
} 
*/