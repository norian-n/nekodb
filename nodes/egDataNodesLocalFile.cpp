#include "egDataNodesLocalFile.h"
#include <iostream>

inline bool EgDataNodesLocalFileType::InitFile(std::string& layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    bool isOk = nodesFile.openToWrite();
    if (isOk)
    {
        nodesFile.writeType<EgFileOffsetType>(0);
        nodesFile.writeType<EgFileOffsetType>(0);
        nodesFile.close();
    }
    return isOk;
}

inline bool EgDataNodesLocalFileType::ReadHeader() {
    nodesFile.seekRead(0);
    nodesFile >> nodesFileHeader.lastID;
    nodesFile >> nodesFileHeader.nodesCount;
    nodesFile >> nodesFileHeader.firstNode;
    nodesFile >> nodesFileHeader.lastNode;
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::WriteHeader() {
    nodesFile.seekWrite(0);
    nodesFile << nodesFileHeader.lastID;
    nodesFile << nodesFileHeader.nodesCount;
    nodesFile << nodesFileHeader.firstNode;
    nodesFile << nodesFileHeader.lastNode;
    return nodesFile.fileStream.good();
}

inline void EgDataNodesLocalFileType::InitNewHeader() {
    nodesFileHeader.lastID = 0;
    nodesFileHeader.nodesCount = 0;
    nodesFileHeader.firstNode = nodesDataOffset;
    nodesFileHeader.lastNode  = 0;
}

bool EgDataNodesLocalFileType::GetLastID(std::string& layoutName, EgDataNodeIDType& lastID) {
    EgDataNodeIDType lastNodeID {0};
    nodesFile.fileName = layoutName + ".gdn";
    bool isOk = nodesFile.openToRead();
    if (isOk) {
        nodesFile.seekRead(lastIDOffset);
        nodesFile >> lastNodeID;
        isOk = nodesFile.fileStream.good();
    }
    nodesFile.close();
    lastID = lastNodeID;
    // std::cout << "GetLastID() name: " << layoutName << ".gdn" << " lastNodeID:" << std::dec << lastNodeID << std::endl;
    return isOk;
}

bool EgDataNodesLocalFileType::OpenFileToRead(std::string& layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    return nodesFile.openToRead();
}

bool EgDataNodesLocalFileType::OpenFileToUpdate(std::string& layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    bool isOk = nodesFile.openToUpdate();
    if (!isOk)
    {
        isOk = InitFile(layoutName);
        if (isOk)
            isOk = nodesFile.openToUpdate();
    }
    return isOk;
}

bool EgDataNodesLocalFileType::StartFileUpdate(std::string& layoutName) {
    nodesFile.fileName = layoutName + ".gdn";
    bool inSubTransact = nodesFile.openToUpdate();
    if (!inSubTransact) { // no file exist
        inSubTransact = nodesFile.openToWrite();
        if (!inSubTransact)
            return false;
        InitNewHeader();
        nodesFileHeader = nodesFileHeader;
        inSubTransact =WriteHeader();
    } else {
        inSubTransact = ReadHeader();
        nodesFileHeader = nodesFileHeader;
    }
    return inSubTransact;
}

bool EgDataNodesLocalFileType::UpdateNodesFile(std::string& layoutName, 
    EgDataNodesMapType& addedNodes, EgDataNodesMapType& deletedNodes, EgDataNodesMapType& updatedNodes) {
/*
    bool inSubTransact = StartFileUpdate(layoutName);
    if (!inSubTransact) {
        std::cout << "ERROR: UpdateNodesFile() can't process header of " << layoutName << ".gdn" << std::endl;
        return false;
    }
*/
    // PrintHeader();
    for (auto newNodesIter : addedNodes) {
        // PrintEgDataNodeTypeFields(*(newNodesIter.second));
        // std::cout << "StoreToLocalFile() container fieldsCount: " << std::dec << (int) ((newNodesIter.second)-> dataNodeLayout-> fieldsCount) << std::endl;
        if (newNodesIter.first < nodesFileHeader.lastID) {
            std::cout << "ERROR: UpdateNodesFile() new node ID < last ID at " << layoutName << ".gdn" << std::endl;
            return false;
        }
        if (! WriteDataNode(newNodesIter.second))
            return false;
        nodesFileHeader.nodesCount++;
        // PrintHeader();
    }    
    for (auto updNodesIter : updatedNodes) {
        // PrintEgDataNodeTypeFields(*(updNodesIter.second));
        if (! DeleteDataNode(updNodesIter.second))
            return false;
        if (! WriteDataNode (updNodesIter.second))
            return false;
    }
    for (auto delNodesIter : deletedNodes) { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // PrintEgDataNodeTypeFields(*(delNodesIter.second));
        if (! DeleteDataNode(delNodesIter.second))
            return false;
        nodesFileHeader.nodesCount--;
    }
    WriteHeader();
    // PrintHeader();
    // nodesFile.close(); // called in peer
    return inSubTransact;
}

bool EgDataNodesLocalFileType::getFirstNodeOffset(EgFileOffsetType& theOffset) {
    nodesFile.seekRead(firstNodePtrOffset);
    nodesFile >> theOffset;
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::getLastNodeOffset(EgFileOffsetType& theOffset) {
    nodesFile.seekRead(lastNodePtrOffset);
    nodesFile >> theOffset;
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::WriteDataNode(EgDataNodeType* theNode) { // write to end, update header
    /* if ( !theNode) {
        std::cout  << "DEBUG: WriteDataNode() theNode ptr is NULL " << std::endl;
    } */
    // std::cout << "WriteDataNode() container fieldsCount: " << std::dec << (int) (theNode-> dataNodeLayout-> fieldsCount) << std::endl;
    // std::cout << "WriteDataNode() container size: " << std::dec << theNode-> dataFieldsContainer.dataFields.size() << std::endl;
    // std::cout << "WriteDataNode() ptr " << std::hex << (uint64_t) theNode-> dataFieldsContainer << std::endl;
    EgFileOffsetType nodeOffset {nodesDataOffset};
    if (nodesFileHeader.lastID > 0)
        nodeOffset = nodesFile.getFileSize();
    theNode-> dataFileOffset = nodeOffset;
    // std::cout  << "dataNodeID:" << std::dec << theNode-> dataNodeID;
    // std::cout << " nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    // nodesFileHeader.lastNode
/*
    EgFileOffsetType headerLastNodeOffset{0};
    nodesFile.seekRead(lastNodePtrOffset); // save previous last node in chain
    nodesFile >> headerLastNodeOffset;
    // std::cout << "nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    nodesFile.seekWrite(lastNodePtrOffset); // update last node in chain ptr
    nodesFile << nodeOffset;                       // to this node offset
*/
    if (nodesFileHeader.lastNode) {
        nodesFile.seekWrite(nodesFileHeader.lastNode + sizeof(EgDataNodeIDType));
        nodesFile << nodeOffset; // update "next" pointer of prev node
    }
    // nodesFileHeader.firstNode
/*    EgFileOffsetType headerFirstNodeOffset{0};
    nodesFile.seekRead(firstNodePtrOffset);
    nodesFile >> headerFirstNodeOffset;

    if (!headerFirstNodeOffset)
    {
        nodesFile.seekWrite(firstNodePtrOffset);
        nodesFile << nodeOffset; // update "next" pointer of prev node
    }
    */
    // std::cout << "WriteDataNode() nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    nodesFile.seekWrite(nodeOffset);
    nodesFile << theNode-> dataNodeID;
    nodesFile.writeType<EgFileOffsetType>(0); // pointer to next node = nullptr
    nodesFile << nodesFileHeader.lastNode;        // pointer to prev node
    // std::cout << "ID: " << std::dec << theNode-> dataNodeID << ", headerLastNodeOffset: " <<  std::hex << headerLastNodeOffset << std::endl;
    // std::cout << "container size: " << std::dec << (int) theNode-> dataNodeLayout-> fieldsCount << " " << theNode-> dataFieldsContainer.dataFields.size() << std::endl;
    theNode-> writeDataFieldsToFile(nodesFile);
    nodesFileHeader.lastNode = nodeOffset; // upd last ptr
    nodesFileHeader.lastID = std::max(theNode-> dataNodeID, nodesFileHeader.lastID);
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::ReadDataNode(EgDataNodeType *theNode, EgFileOffsetType &nextOffset) {
    nodesFile.seekRead(theNode->dataFileOffset);
    nodesFile >> theNode->dataNodeID;
    nodesFile >> nextOffset;
    // std::cout  << "theNode-> dataFileOffset = " << std::hex << theNode-> dataFileOffset;
    // std::cout  << " , dataNodeID = " << std::dec << theNode-> dataNodeID;
    // std::cout  << " , nextOffset = " << std::hex << nextOffset << std::endl;
    nodesFile.seekRead(theNode->dataFileOffset + sizeof(EgDataNodeIDType) + 2 * sizeof(EgFileOffsetType));
    theNode-> readDataFieldsFromFile(nodesFile);
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::DeleteDataNode(EgDataNodeType *theNode) {
    EgFileOffsetType nextNodeOffset{0};
    EgFileOffsetType prevNodeOffset{0};
    nodesFile.seekRead(theNode->dataFileOffset + sizeof(EgDataNodeIDType));
    nodesFile >> nextNodeOffset;
    nodesFile >> prevNodeOffset;
    /* std::cout << "DeleteDataNode() dataFileOffset: " <<  std::hex << theNode-> dataFileOffset;
    std::cout << " , prevNodeOffset: " <<  std::hex << theNode-> prevNodeOffset;
    std::cout << " , nextNodeOffset: " <<  std::hex << theNode-> nextNodeOffset << std::endl; */
    if (prevNodeOffset) {
        nodesFile.seekWrite(prevNodeOffset + sizeof(EgDataNodeIDType));
        nodesFile << nextNodeOffset;
    } else
        nodesFileHeader.firstNode = nextNodeOffset;// nodesFile.seekWrite(0); // update first node of chain ptr

    if (nextNodeOffset) {
        nodesFile.seekWrite(nextNodeOffset + sizeof(EgDataNodeIDType) + sizeof(EgFileOffsetType));
        nodesFile << prevNodeOffset;
    } else
        nodesFileHeader.lastNode = prevNodeOffset; // nodesFile.seekWrite(sizeof(EgFileOffsetType)); // update last node in chain ptr

    nodesFile.seekWrite(theNode->dataFileOffset + sizeof(EgDataNodeIDType)); // FIXME TODO - history chain
    nodesFile.writeType<EgFileOffsetType>(0);
    nodesFile.writeType<EgFileOffsetType>(0);
    return nodesFile.fileStream.good();
}

// ======================== Debug ========================

void EgDataNodesLocalFileType::PrintHeader() {
    std::cout  << "PrintHeader(): " << std::endl;  
    std::cout  << " nodesFileHeader.lastID: " << std::dec << nodesFileHeader.lastID << std::endl;
    std::cout  << " nodesFileHeader.nodesCount: " << nodesFileHeader.nodesCount << std::endl;
    std::cout  << " nodesFileHeader.firstNode: " << std::hex << nodesFileHeader.firstNode << std::endl;
    std::cout  << " nodesFileHeader.lastNode: " << nodesFileHeader.lastNode << std::endl;
}


void EgDataNodesLocalFileType::PrintNodesChain() {
    EgDataNodeIDType dataNodeID      { 0 };
    EgFileOffsetType nextOffset;
    EgFileOffsetType prevOffset;
    getFirstNodeOffset(nextOffset);
    getLastNodeOffset(prevOffset);
    std::cout  << "PrintNodesChain(): ";
    std::cout  << " getFirstNodeOffset()= " << std::hex << nextOffset;
    std::cout  << " getLastNodeOffset()= "  << prevOffset << std::endl;   
    while (nextOffset) {
        nodesFile.seekRead(nextOffset);
        nodesFile >> dataNodeID;
        nodesFile >> nextOffset;
        nodesFile >> prevOffset;
        std::cout << " dataNodeID: " << std::dec << dataNodeID;
        std::cout << " nextOffset: " << std::hex << nextOffset;
        std::cout << " prevOffset: " << prevOffset << std::endl;
    }
}

void ReadDataNodeAndOffsets(EgFileType &theNodesFile, EgDataNodeType *theNode) {
    theNodesFile.seekRead(theNode->dataFileOffset);
    theNodesFile >> theNode->dataNodeID;
#ifdef EG_NODE_OFFSETS_DEBUG
    theNodesFile >> theNode->nextNodeOffset;
    theNodesFile >> theNode->prevNodeOffset;
#endif
    theNode-> readDataFieldsFromFile(theNodesFile);
}

