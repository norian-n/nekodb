#include "egDataNodesLocalFile.h"
#include <iostream>

void EgDataNodesLocalFileType::initIndexes() {
    if (dataNodeBlueprint)
        for (auto fieldsIter : dataNodeBlueprint-> indexedFields) {
            EgIndexes<uint32_t>* newIndex = new EgIndexes<uint32_t>(fieldsIter.first);
            localIndexes.insert(std::make_pair(fieldsIter.first, static_cast<EgIndexesAbstractType*> (newIndex)));
        }
    else
        std::cout << "ERROR: initIndexes() dataNodeBlueprint not set  " << std::endl;    
}
// EgIndexes<uint32_t> testIndexes("testIndexes");

bool EgDataNodesLocalFileType::InitFile(std::string& layoutName) { // tests support
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
    std::string fieldName;
    for (auto newNodesIter : addedNodes) {
        // PrintEgDataNodeTypeFields(*(newNodesIter.second));
        // std::cout << "StoreToLocalFile() container fieldsCount: " << std::dec << (int) ((newNodesIter.second)-> dataNodeLayout-> fieldsCount) << std::endl;
        if (newNodesIter.first < nodesFileHeader.lastID) {
            std::cout << "ERROR: UpdateNodesFile() new node ID < last ID at " << layoutName << ".gdn" << std::endl;
            return false;
        }
        if (! WriteDataNode(newNodesIter.second))
            return false;
        for (auto index : localIndexes) { // add local indexes
            fieldName = index.first;
            localIndexes[fieldName]-> AddNewIndex(newNodesIter.second-> operator[] (fieldName), newNodesIter.second-> dataFileOffset);
        }
        nodesFileHeader.nodesCount++;
        // PrintHeader();
    }    
    for (auto updNodesIter : updatedNodes) {
        // PrintEgDataNodeTypeFields(*(updNodesIter.second));
        // std::cout << "UpdateNodesFile() old offset: " << std::hex << updNodesIter.second-> dataFileOffset << std::endl;        
        if (! DeleteDataNode(updNodesIter.second))
            return false;
        for (auto index : localIndexes) { // del old local indexes FIXME store and use old key
            fieldName = index.first;            
            if (updNodesIter.second-> indexedFieldsOldValues.contains (fieldName)) {
                localIndexes[fieldName]-> DeleteIndex(*(updNodesIter.second-> indexedFieldsOldValues[fieldName]), updNodesIter.second-> dataFileOffset);
            } else                
                std::cout << "ERROR: UpdateNodesFile() old field value has not been saved for field, index corrupted " << fieldName << std::endl;
        }            
        if (! WriteDataNode (updNodesIter.second))
            return false;
        for (auto index : localIndexes) { // add local indexes
            fieldName = index.first;
            localIndexes[fieldName]-> AddNewIndex(updNodesIter.second-> operator[] (fieldName), updNodesIter.second-> dataFileOffset);
        }             
        // std::cout << "UpdateNodesFile() new offset: " << updNodesIter.second-> dataFileOffset << std::dec << std::endl;             
            // update local indexes
    }
    for (auto delNodesIter : deletedNodes) { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // PrintEgDataNodeTypeFields(*(delNodesIter.second));
        if (! DeleteDataNode(delNodesIter.second))
            return false;
            // delete local indexes
        for (auto index : localIndexes) { // add local indexes
            fieldName = index.first;
            if (! delNodesIter.second-> indexedFieldsOldValues.contains (fieldName)) // node not updated
                localIndexes[fieldName]-> DeleteIndex(delNodesIter.second-> operator[] (fieldName), delNodesIter.second-> dataFileOffset);
            else
                localIndexes[fieldName]-> DeleteIndex(*(delNodesIter.second-> indexedFieldsOldValues[fieldName]), delNodesIter.second-> dataFileOffset);            
        }            
        nodesFileHeader.nodesCount--;
    }
    WriteHeader();
    // PrintHeader();
    // nodesFile.close(); // called in peer
    return localTransactOk;
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
    // std::cout << "WriteDataNode() dataFieldsContainer ptr " << std::hex << (uint64_t) theNode-> dataFieldsContainer << std::endl;
    EgFileOffsetType nodeOffset {nodesDataOffset}; // init to empty data pos
    if (nodesFileHeader.lastID > 0) // data not empty 
        nodeOffset = nodesFile.getFileSize();
    theNode-> dataFileOffset = nodeOffset;
    // std::cout  << "dataNodeID:" << std::dec << theNode-> dataNodeID;
    // std::cout << " nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    if (nodesFileHeader.lastNode) {
        nodesFile.seekWrite(nodesFileHeader.lastNode + sizeof(EgDataNodeIDType));
        nodesFile << nodeOffset; // update "next" pointer of prev node
    }
    nodesFile.seekWrite(nodeOffset);
    nodesFile << theNode-> dataNodeID;
    nodesFile.writeType<EgFileOffsetType>(0);   // pointer to next node = nullptr
    nodesFile << nodesFileHeader.lastNode;      // pointer to prev node
    theNode-> writeDataFieldsToFile(nodesFile);
    nodesFileHeader.lastNode = nodeOffset;      // upd last node ptr in the header
    nodesFileHeader.lastID = std::max(theNode-> dataNodeID, nodesFileHeader.lastID);
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::ReadDataNode(EgDataNodeType *theNode, EgFileOffsetType &nextOffset) {
    theNode-> indexedFieldsOldValues.clear();
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

/*
bool EgDataNodesLocalFileType::AddLocalIndex(std::string& indexName, EgDataNodeType* theNode) {
    localIndexes[indexName]-> AddNewIndex(theNode-> operator[] (indexName), theNode-> dataFileOffset);

    return true;
} */

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

