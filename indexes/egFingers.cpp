#include "egFingers.h"
#include "egIndexes.h"
#include <filesystem>

using namespace egIndexesSpace;

template <typename KeyType> inline void EgFingers<KeyType>::InitFinger(egFinger<KeyType>& theFinger) {
    theFinger.minKey = 0;
    theFinger.maxKey = 0;
    theFinger.myChunkIsLeaf = 0xff; // isLeaf == true
    theFinger.itemsCount = 0;
    theFinger.nextChunkOffset = 0; // egIndexesNamespace::indexHeaderSize;
    theFinger.myChunkOffset = 0;
    theFinger.myPosInChunkBytes = 0;
}

template <typename KeyType> EgFingers<KeyType>::EgFingers(std::string a_fingersName, EgIndexes<KeyType>* indexChunksPtr) :
        indexChunks(indexChunksPtr),
        fingersFileName(a_fingersName + ".fng"),
        fingersFileStream(a_fingersName + ".fng"),
        localStream(new egDataStream(fingersChunkSize)) {
    InitFinger(rootFinger);
}

//  ============================================================================
//          FINGERS TOP API
//  ============================================================================

template <typename KeyType> bool EgFingers<KeyType>::AddNewRootFinger(KeyType& Key, uint64_t indexesChunkOffset) {
    rootFinger.minKey        = Key;
    rootFinger.maxKey        = Key;
    rootFinger.myChunkIsLeaf   = 0xff; // isLeaf == true
    rootFinger.itemsCount    = 1;
    rootFinger.nextChunkOffset   = indexesChunkOffset; // egIndexesNamespace::indexHeaderSize;
    rootFinger.myChunkOffset     = 0;
    rootFinger.myPosInChunkBytes = 0;
    if (! fingersFileStream.openToWrite()) {
        std::cout << "fileStream.openToWrite() failed" << std::endl;
        return false;
    }
    rootFingerIsLoaded = StoreFingerToFile(rootFinger);
    fingersFileStream.close();
    return rootFingerIsLoaded;
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexesChunkToInsert(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    // FIXME TODO check global min/max and use first/last index chunk directly
    if (! fingersFileStream.openToUpdate()) {
        std::cout << "fileStream.openToRead() failed" << std::endl;
        return false;
    }
    bool res {true};
    fingersChain.clear();
    if (! rootFingerIsLoaded)
        res = res && LoadRootFingerFromFile(rootFinger, isRootFinger);
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        parentFinger = currentFinger;
        // EG_LOG_STUB << "parentFinger.itemsCount = " << parentFinger.itemsCount << FN;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset);; // sets parentFinger.itemsCount
        FindLastFinger(Key, CompareGT);
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    } 
    return res;
}
    // PrintFingerInfo(currentFinger, "currentFinger ");
template <typename KeyType> bool EgFingers<KeyType>::UpdateFingersChainUp() {  // ex UpdateFingersChainAfterInsert
    bool res = StoreFingerToFile(currentFinger); // update current FIXME check
    anyMinMaxChanged = true;
    while (! fingersChain.empty() && anyMinMaxChanged && res) { // go all chain up
        parentFinger = fingersChain.back();
        fingersChain.pop_back();
        if (FingerIsRoot(parentFinger))
            anyMinMaxChanged = UpdateMinMaxByFlags(rootFinger); // keep root updated
        else
            anyMinMaxChanged = UpdateMinMaxByFlags(parentFinger);
        res = res && fingersFileStream.good();
    }
    fingersChain.clear();
    fingersFileStream.close(); // opened in FindIndexesChunk
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::AddNewUpdateCurrentFinger() { // pair of FindIndexesChunkToInsert() , new is next pos to updated current
// !!!!! requires theKey rootFinger currentFinger newFinger and fingersChain set by FindIndexesChunkToInsert()
    bool newParentFingerRequired {false};
    bool res {true}; // result
    // bool appendMode = true;
    if (rootFinger.myChunkIsLeaf)         // no fingers chunks yet
        res = AddNewTopLevelChunk();   // add new chunk under root with currentFinger and newFinger
    else {
        while (!fingersChain.empty()) {
            parentFinger = fingersChain.back(); // get parent from chain
            fingersChain.pop_back();
            LoadFingersChunk(parentFinger.nextChunkOffset);; // autoset parentFinger.itemsCount
            // EG_LOG_STUB << "parentFinger.itemsCount = " << parentFinger.itemsCount << FN;
            if (parentFinger.itemsCount < egChunkCapacity) {
                res = res && InsertNewFingerToChunk(); // add new finger and update min/max of parent
                newParentFingerRequired = false;
                break; // new chunk is not needed, get out
            } else {
                if (currentFinger.myPosInChunkBytes == (egChunkCapacity - 1) * oneFingerSize) // last
                    AppendNewFingersChunk(newFinger);
                else {
                    // PrintFingerInfo(currentFinger, "currentFinger");
                    // PrintFingerInfo(newFinger, "newFinger ");
                    SaveLastFingerOfChunk(); // to lastFinger
                    InsertNewFingerToChunk();
                    AppendNewFingersChunk(lastFinger); // appendMode = (theKey >= parentFinger.maxKey); // FIXME check
                }
                newParentFingerRequired = true;
                currentFinger = parentFinger; // level up
            }
        }
        if (res && fingersChain.empty() && newParentFingerRequired) // new top chunk is required
            AddNewTopLevelChunk();                                  // add new chunk under root with currentFinger and newFinger
        while (!fingersChain.empty() && anyMinMaxChanged && res) {                                       // update min/max up
            parentFinger = fingersChain.back(); // get parent from chain
            fingersChain.pop_back();
            LoadFingersChunk(parentFinger.nextChunkOffset);; // FIXME optimize by min/max check
            if (FingerIsRoot(parentFinger)) { // keep updated rootFinger
                ReadChunkMinMaxToParentFinger(rootFinger);
                res = res && StoreFingerToFile(rootFinger);
            } else {
                ReadChunkMinMaxToParentFinger(parentFinger);
                res = res && StoreFingerToFile(parentFinger);
            }
        }
        fingersChain.clear();
    }
    fingersFileStream.close(); // opened in FindIndexesChunkToInsert()
    return res;
}

template <typename KeyType> inline bool EgFingers<KeyType>::DeleteTopChunk() {
    // EG_LOG_STUB << "parentFinger.itemsCount = " << parentFinger.itemsCount << " , FingerIsRoot = " << FingerIsRoot(parentFinger) << FN;
    // currentFinger.myChunkOffset = parentFinger.nextChunkOffset;
    LoadFingersChunk(parentFinger.nextChunkOffset);
    if (currentFinger.myPosInChunkBytes) // delete second, load first
        ReadFingerFromChunk(parentFinger, 0);
    else
        ReadFingerFromChunk(parentFinger, oneFingerSize);
    // PrintFingerInfo(currentFinger, "currentFinger");
    // PrintFingerInfo(parentFinger, "parentFinger");
    bool res = DeleteFingersChunk(currentFinger.myChunkOffset);
    rootFinger.minKey = parentFinger.minKey;
    rootFinger.maxKey = parentFinger.maxKey;
    rootFinger.nextChunkOffset = parentFinger.nextChunkOffset;
    rootFinger.myChunkIsLeaf   = currentFinger.myChunkIsLeaf;
    if (rootFinger.myChunkIsLeaf) { // FIXME check if count required
        indexChunks-> LoadIndexChunk(rootFinger.nextChunkOffset);
        rootFinger.itemsCount = indexChunks-> theChunkCount;
    } else
        GetCountDirect(rootFinger.itemsCount, rootFinger.nextChunkOffset); // parentFinger.itemsCount; // FIXME check itemsCount use cases
    res = res && StoreFingerToFile(rootFinger);
    // PrintFingerInfo(rootFinger, "rootFinger");
    if (rootFinger.myChunkIsLeaf)
        indexChunks->StoreFingerOffset(rootFinger.nextChunkOffset, 0);
    else
        res = res && StoreParentChunkOffsetDirect(rootFinger.nextChunkOffset, 0);
    fingersChain.clear();
    fingersFileStream.close(); // opened in FindIndexesChunk
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::DeleteCurrentFingerByChain() { // ex UpdateFingersChain() pair of FindIndexesChunkToInsert()
// !!!!! requires theKey rootFinger currentFinger newFinger and fingersChain set by FindIndexesChunkToInsert()
    bool delParentFingerRequired {false};
    bool res {true}; // result
    if (FingerIsRoot(currentFinger))
        return DeleteFingersFile();   // FIXME check
    while (! fingersChain.empty() && res) {
        parentFinger = fingersChain.back(); // get parent from chain
        fingersChain.pop_back();
        // PrintFingerInfo(currentFinger, "currentFinger");
        // PrintFingerInfo(parentFinger, "parentFinger");
        if (FingerIsRoot(parentFinger) && (parentFinger.itemsCount == 2)) // shortcut
            return DeleteTopChunk();
        if (parentFinger.itemsCount > 1) {
            LoadFingersChunk(parentFinger.nextChunkOffset);;
            res = res && DeleteFingerFromChunk(); // del finger and update min/max of parent
            delParentFingerRequired = false;
            break;  // get out
        } else {    // last finger in the chunk       
            DeleteFingersChunk(parentFinger.nextChunkOffset); // FIXME check
            delParentFingerRequired = true;
            currentFinger = parentFinger; // level up
        }
    }
    if (fingersChain.empty() && delParentFingerRequired && res) { // delete root
        fingersChain.clear();
        return DeleteFingersFile(); // FIXME check
    }
    while (! fingersChain.empty() && anyMinMaxChanged && res) { // update min/max up
        parentFinger = fingersChain.back(); // get parent from chain
        fingersChain.pop_back();
        LoadFingersChunk(parentFinger.nextChunkOffset);; // FIXME optimize by min/max check
        if (FingerIsRoot(parentFinger)) { // keep updated rootFinger
            ReadChunkMinMaxToParentFinger(rootFinger);
            res = res && StoreFingerToFile(rootFinger);
        } else {
            ReadChunkMinMaxToParentFinger(parentFinger);
            res = res && StoreFingerToFile(parentFinger);
        }
    }
    fingersChain.clear();
    fingersFileStream.close(); // opened in FindIndexesChunkToInsert()
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::DeleteFingersFile() {
    fingersFileStream.close();
    return (bool) std::remove(fingersFileName.c_str());
}

template <typename KeyType> inline bool EgFingers<KeyType>::DeleteFingerFromChunk() {
    parentFinger.nextChunkOffset = currentFinger.myChunkOffset;
    keysCountType fingerPosition = currentFinger.myPosInChunkBytes / oneFingerSize;
    GetChunkIsLeaf(currentFinger.myChunkIsLeaf);
    // EG_LOG_STUB << "keysCount = " << keysCount << ", fingerPosition = " << fingerPosition << ", myLocalLevel = " << myLocalLevel << FN;
    // PrintFingersChunk(fingersBA.data(), "fingers chunk before delete " + FNS);
    if (fingerPosition < parentFinger.itemsCount-1) // move tail if required FIXME use tail move function
        memmove (localStream-> bufData + currentFinger.myPosInChunkBytes, localStream-> bufData + currentFinger.myPosInChunkBytes+oneFingerSize,  
                 oneFingerSize*(parentFinger.itemsCount - 1 - fingerPosition));
    memset(localStream-> bufData + oneFingerSize*(parentFinger.itemsCount-1), 0, oneFingerSize); // clear last finger
    WriteCountToChunk(parentFinger.itemsCount-1);
    bool res = StoreFingersChunk(parentFinger.nextChunkOffset);
    if (FingerIsRoot(parentFinger)) { // keep updated rootFinger
        rootFinger.itemsCount--;
        ReadChunkMinMaxToParentFinger(rootFinger);
        res = res && StoreFingerToFile(rootFinger);
    } else {
        parentFinger.itemsCount--;
        ReadChunkMinMaxToParentFinger(parentFinger);
        res = res && StoreFingerToFile(parentFinger);
    }
    return res;
}

//  ============================================================================
//          BACKPTRS FUNCTIONS
//  ============================================================================

template <typename KeyType> inline bool EgFingers<KeyType>::GetParentFingerByOffset(uint64_t fingersChunkOffset, uint64_t nextOffset) {
    if (fingersChunkOffset == 0) { // is root
        parentFinger = rootFinger;
        return true;
    }
    bool res = LoadFingersChunk(fingersChunkOffset); // autoset parentFinger.itemsCount
    keysCountType localItemsCount = parentFinger.itemsCount;
    int fingerPosition {0};
    if (res) {
        do
            ReadFingerFromChunk(parentFinger, fingerPosition);
        while ((parentFinger.nextChunkOffset != nextOffset) && (++fingerPosition < localItemsCount));
        parentFinger.myChunkOffset = fingersChunkOffset;
    }
    return res && (parentFinger.nextChunkOffset == nextOffset);
}

template <typename KeyType> bool EgFingers<KeyType>::UpdateFingersByBackptrs() {
    bool res = StoreFingerToFile(currentFinger); // update current
    anyMinMaxChanged = true;
    while (! FingerIsRoot(currentFinger) && anyMinMaxChanged && res) { // go all chain up
        // PrintFingerInfo(currentFinger, "currentFinger ");
        res =res && GetParentFingerByOffset(currentFinger.parentChunkOffset, currentFinger.myChunkOffset);
        // PrintFingerInfo(parentFinger , "parentFinger ");
        if (FingerIsRoot(parentFinger))
            anyMinMaxChanged = UpdateMinMaxByFlags(rootFinger); // keep root updated
        else
            anyMinMaxChanged = UpdateMinMaxByFlags(parentFinger);
        currentFinger = parentFinger;
        res = res && fingersFileStream.good();
    }
    fingersFileStream.close(); // opened in FindIndexesChunk
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::DeleteFingerByBackptrs() { // ex UpdateFingersChain() pair of FindIndexesChunkToInsert()
// !!!!! requires theKey rootFinger currentFinger newFinger and fingersChain set by FindIndexesChunkToInsert()
    bool delParentFingerRequired {false};
    bool res {true}; // result
    if (FingerIsRoot(currentFinger))
        return DeleteFingersFile();   // FIXME check
    while (! FingerIsRoot(currentFinger) && res) {
        // PrintFingerInfo(currentFinger , "currentFinger");
        res =res && GetParentFingerByOffset(currentFinger.parentChunkOffset, currentFinger.myChunkOffset);
        GetCountDirect(parentFinger.itemsCount, parentFinger.nextChunkOffset);
        // PrintFingerInfo(parentFinger , "parentFinger");
        // EG_LOG_STUB << "parentFinger.itemsCount = " << parentFinger.itemsCount << FN;
        if (FingerIsRoot(parentFinger) && (parentFinger.itemsCount == 2)) // shortcut
            return DeleteTopChunk();
        if (parentFinger.itemsCount > 1) {
            res = res && DeleteFingerFromChunk(); // del finger and update min/max of parent
            delParentFingerRequired = false;
            break;  // get out
        } else {    // last finger in the chunk       
            DeleteFingersChunk(parentFinger.nextChunkOffset); // FIXME check
            delParentFingerRequired = true;
            currentFinger = parentFinger; // level up
        }
    }
    if ( FingerIsRoot(currentFinger) && delParentFingerRequired && res) // delete root       
         return DeleteFingersFile(); // FIXME check

    while (! FingerIsRoot(currentFinger) && anyMinMaxChanged && res) { // update min/max up
        res =res && GetParentFingerByOffset(currentFinger.parentChunkOffset, currentFinger.myChunkOffset);
        if (FingerIsRoot(currentFinger)) { // keep updated rootFinger
            ReadChunkMinMaxToParentFinger(rootFinger);
            res = res && StoreFingerToFile(rootFinger);
        } else {
            ReadChunkMinMaxToParentFinger(currentFinger);
            res = res && StoreFingerToFile(currentFinger);
        }
        currentFinger = parentFinger;
    }
    fingersFileStream.close(); // opened in 
    return res;
}

//  ============================================================================
//          FINGERS FILES OPERATIONS
//  ============================================================================

template <typename KeyType> inline bool EgFingers<KeyType>::LoadRootFingerFromFile(egFinger<KeyType>& theFinger, bool loadRootIsLeaf) {
    // EG_LOG_STUB << "fingerFile.fileName() = " << fingerFile.fileName() << FN;
    // EG_LOG_STUB << "fingerFile size = " << fingersFileStream.getFileSize() << FN;
    // EG_LOG_STUB << "fingerFile pos = " << theFinger.myChunkOffset+theFinger.myPosInChunkBytes << FN;
    fingersFileStream.seekRead(0); // theFinger.myChunkOffset+theFinger.myPosInChunkBytes);
    fingersFileStream >> theFinger.minKey;
    fingersFileStream >> theFinger.maxKey;
    // fingersFileStream >> theFinger.itemsCount;
    fingersFileStream >> theFinger.nextChunkOffset;
    // if (loadRootIsLeaf) {
        fingersFileStream >> theFinger.myChunkIsLeaf;
        if (fingersFileStream.good())
            rootFingerIsLoaded = true;
    // }
    // EG_LOG_STUB << "fingerFile is good = " << fingersFileStream.good() << FN;
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
    return fingersFileStream.good();
}

template <typename KeyType> inline bool EgFingers<KeyType>::StoreFingerToFile(egFinger<KeyType>& theFinger) {
    fingersFileStream.seekWrite(theFinger.myChunkOffset+theFinger.myPosInChunkBytes);
    fingersFileStream << theFinger.minKey;
    fingersFileStream << theFinger.maxKey;
    // fingersFileStream << theFinger.itemsCount;
    fingersFileStream << theFinger.nextChunkOffset;
    if (FingerIsRoot(theFinger)) { // copy to root and write leaf flag
        rootFinger = theFinger;
        fingersFileStream << theFinger.myChunkIsLeaf;
    }
    return fingersFileStream.good();
}

template <typename KeyType> inline bool EgFingers<KeyType>::LoadFingersChunk(uint64_t& chunkOffset) { // by parentFinger.nextChunkOffset to localStream-> bufData
    fingersFileStream.seekRead(chunkOffset);
    fingersFileStream.fileStream.read((char *) localStream-> bufData, fingersChunkSize);
    GetCountFromChunk(parentFinger.itemsCount);
    GetChunkIsLeaf(currentFinger.myChunkIsLeaf);
    currentFinger.myChunkOffset = chunkOffset;
    return fingersFileStream.good();
}

template <typename KeyType> inline bool EgFingers<KeyType>::StoreFingersChunk(uint64_t fingersChunkOffset) { // , char* chunkPtr = nullptr) {
    fingersFileStream.seekWrite(fingersChunkOffset);
    // if (! chunkPtr)
    fingersFileStream.fileStream.write((char *) localStream-> bufData, fingersChunkSize);
    // else
    //    fingersFileStream.write(chunkPtr, fingersChunkSize);
    // EG_LOG_STUB << "fingersChunkOffset" << hex << (int) fingersChunkOffset << FN;
    return fingersFileStream.good();
}

template <typename KeyType> inline bool EgFingers<KeyType>::StoreParentChunkOffsetDirect(uint64_t fingersChunkOffset, uint64_t parentChunkOffset) {
    fingersFileStream.seekWrite(fingersChunkOffset + parentChunkOffsetPosition); // parentOffsetPosition is const in this header
    fingersFileStream << parentChunkOffset;
    // EG_LOG_STUB << "fingersChunkOffset = " << std::hex << fingersChunkOffset << ", parentChunkOffset = " << parentChunkOffset << FN;
    return fingersFileStream.good();
}

template <typename KeyType> void EgFingers<KeyType>::SwapFingers() {
    uint64_t swapPosInChunkBytes = currentFinger.myPosInChunkBytes;
    egFinger<KeyType> swapFinger = newFinger;
    newFinger = currentFinger;
    newFinger.myPosInChunkBytes  = swapFinger.myPosInChunkBytes; // keep positions as is
    swapFinger.myPosInChunkBytes = currentFinger.myPosInChunkBytes;
    currentFinger = swapFinger;
    // PrintFingerInfo(currentFinger , "currentFinger ");
    // PrintFingerInfo(newFinger , "newFinger ");
} 

//  ============================================================================
//          FINGERS CHUNK OPERATIONS
//  ============================================================================

template <typename KeyType> inline void EgFingers<KeyType>::ClearFingersChunk() {
    memset(localStream-> bufData, 0, fingersChunkSize);
}

template <typename KeyType> inline void EgFingers<KeyType>::ReadFingerFromChunk(egFinger<KeyType>& theFinger, const int fingerPositionNum) {
    theFinger.myPosInChunkBytes = fingerPositionNum * oneFingerSize;
    // EG_LOG_STUB << "theFinger.myPosInChunkBytes = " << (int) theFinger.myPosInChunkBytes << FN;
    localStream  -> seek(theFinger.myPosInChunkBytes);
    *localStream >> theFinger.minKey;
    *localStream >> theFinger.maxKey;
    *localStream >> theFinger.nextChunkOffset;
    GetChunkIsLeaf(theFinger.myChunkIsLeaf);
    GetChunkParentOffset(theFinger.parentChunkOffset);
    // GetCountFromChunk(theFinger.itemsCount);
}

template <typename KeyType> inline void EgFingers<KeyType>::WriteFingerToChunk(egFinger<KeyType>& theFinger) {
    // EG_LOG_STUB << "theFinger.myPosInChunkBytes = " << (int) theFinger.myPosInChunkBytes << FN;
    localStream -> seek(theFinger.myPosInChunkBytes);
    *localStream << theFinger.minKey;
    *localStream << theFinger.maxKey;
    // *localStream << theFinger.itemsCount;
    *localStream << theFinger.nextChunkOffset; // theFinger.chunkOffset;
}

template <typename KeyType> inline void EgFingers<KeyType>::ReadChunkMinMaxToParentFinger(egFinger<KeyType>& theParentFinger) {
    anyMinMaxChanged = false; // class flag
    KeyType chunkMinKey, chunkMaxKey;
    localStream  -> seek(0);
    *localStream >> chunkMinKey;
    if (chunkMinKey != theParentFinger.minKey) {
        theParentFinger.minKey = chunkMinKey;
        anyMinMaxChanged = true;
    }
    localStream  -> seek((theParentFinger.itemsCount - 1) * oneFingerSize + sizeof(KeyType));
    *localStream >> chunkMaxKey;
    if (chunkMaxKey != theParentFinger.maxKey) {
        theParentFinger.maxKey = chunkMaxKey;
        anyMinMaxChanged = true;
    }
    // EG_LOG_STUB << "chunkMinKey = " << std::hex << (int) chunkMinKey << ", chunkMaxKey = " << (int) chunkMaxKey << FN;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetCountFromChunk(keysCountType& refCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream >> refCount;
}

template <typename KeyType> inline void EgFingers<KeyType>::WriteCountToChunk(keysCountType theCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream << theCount;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetCountDirect(keysCountType& refCount, uint64_t& chunkOffset) {
    fingersFileStream.seekRead(chunkOffset + chunkCountPosition);
    fingersFileStream >> refCount;
    // EG_LOG_STUB << "chunkOffset = " << std::hex << chunkOffset << ", Count = " << std::dec << refCount  << FN;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetChunkIsLeaf(uint8_t& isLeaf) {
    localStream  -> seek(chunkIsLeafPosition);
    *localStream >> isLeaf;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateChunkIsLeaf(uint8_t myChunkIsLeaf) {
    localStream  -> seek(chunkIsLeafPosition);
    *localStream << myChunkIsLeaf;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetChunkParentOffset(uint64_t& parentChunkOffset) {
    localStream  -> seek(parentChunkOffsetPosition);
    *localStream >> parentChunkOffset;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateChunkParentOffset(uint64_t parentChunkOffset) {
    localStream  -> seek(parentChunkOffsetPosition);
    *localStream << parentChunkOffset;
}

template <typename KeyType> inline void EgFingers<KeyType>::MoveTailToInsert(egMaxStreamSizeType fingerPositionBytes, egMaxStreamSizeType bytesToMove) {
    // EG_LOG_STUB << "fingerPositionBytes = " << fingerPositionBytes << ", bytesToMove = " << bytesToMove  << FN;
    memmove ((char *) (localStream-> bufData+fingerPositionBytes+oneFingerSize), (char*) (localStream-> bufData+fingerPositionBytes), bytesToMove);
    // PrintFingersChunk(chunkPtr, "chunk after tail move ");
}

template <typename KeyType> inline bool EgFingers<KeyType>::AddNewTopLevelChunk() {
    rootFinger.nextChunkOffset = fingersFileStream.getFileSize();
    rootFinger.minKey = currentFinger.minKey;
    rootFinger.maxKey = newFinger.maxKey;
    rootFinger.myChunkIsLeaf = 0; // root is not a leaf anymore
    rootFinger.itemsCount = 2;
    currentFinger.myChunkOffset = rootFinger.nextChunkOffset;
    currentFinger.myPosInChunkBytes = 0;
    newFinger.myChunkOffset = rootFinger.nextChunkOffset;
    newFinger.myPosInChunkBytes = oneFingerSize;
    ClearFingersChunk();
    WriteFingerToChunk(currentFinger);
    WriteFingerToChunk(newFinger);
    UpdateChunkIsLeaf(newFinger.myChunkIsLeaf);
    // UpdateChunkParentOffset(0);  // set to root, default 0 by clear
    bool res = StoreFingerToFile(rootFinger);
    WriteCountToChunk(2);
    res = res && StoreFingersChunk(rootFinger.nextChunkOffset);
    // EG_LOG_STUB << "rootFinger.nextChunkOffset = " << std::hex << rootFinger.nextChunkOffset  << FN;
    if (newFinger.myChunkIsLeaf) {
        indexChunks-> StoreFingerOffset(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset);
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, rootFinger.nextChunkOffset);
    } else {
        res = res && StoreParentChunkOffsetDirect(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset);
        res = res && StoreParentChunkOffsetDirect(newFinger.nextChunkOffset, rootFinger.nextChunkOffset);
    }
    return res;
}

template <typename KeyType> inline bool EgFingers<KeyType>::AppendNewFingersChunk(egFinger<KeyType>& theFinger) {
    // PrintFingerInfo(theFinger, "AppendNewFingersChunk : ");
    ClearFingersChunk();
    theFinger.myPosInChunkBytes = 0;
    bool localChunkIsLeaf = theFinger.myChunkIsLeaf;
    uint64_t localNextChunkOffset = theFinger.nextChunkOffset; // save to not overwrite in newFinger
    WriteFingerToChunk(theFinger); // saved last finger or new
    UpdateChunkIsLeaf(theFinger.myChunkIsLeaf); // FIXME check
    newFinger.nextChunkOffset = fingersFileStream.getFileSize(); // level up
    newFinger.itemsCount = 1;
    newFinger.myChunkIsLeaf = 0; // level up is not a leaf
    if (localChunkIsLeaf)
        indexChunks-> StoreFingerOffset(localNextChunkOffset, newFinger.nextChunkOffset);
    else
        StoreParentChunkOffsetDirect(localNextChunkOffset, newFinger.nextChunkOffset); // store ptr to parents chunk
    WriteCountToChunk(1);
    ReadChunkMinMaxToParentFinger(newFinger);
    // PrintFingerInfo(newFinger, "newFinger (level up): ");
    return StoreFingersChunk(newFinger.nextChunkOffset); // save new chunk, fingers already updated to upper level
}

template <typename KeyType> inline void EgFingers<KeyType>::SaveLastFingerOfChunk() {
    ReadFingerFromChunk(lastFinger, egChunkCapacity-1);
    // PrintFingerInfo(lastFinger, "lastFinger");
}

template <typename KeyType> inline bool EgFingers<KeyType>::FingerIsRoot(egFinger<KeyType>& theFinger) {
    return (theFinger.myChunkOffset == 0); // FIXME check
}

template <typename KeyType> inline bool EgFingers<KeyType>::InsertNewFingerToChunk() {
    // PrintFingerInfo(currentFinger, "currentFinger");
    // PrintFingerInfo(newFinger, "newFinger ");
    bool res {true};
    WriteFingerToChunk(currentFinger); // update
    newFinger.myChunkOffset = currentFinger.myChunkOffset;
    newFinger.myPosInChunkBytes = currentFinger.myPosInChunkBytes + oneFingerSize;
    // EG_LOG_STUB << "newFinger.myPosInChunkBytes = " << std::dec << newFinger.myPosInChunkBytes  << 
    //                ", parentFinger.itemsCount*size = " << parentFinger.itemsCount*oneFingerSize << FN;
    if (newFinger.myPosInChunkBytes < parentFinger.itemsCount*oneFingerSize) // not last
        if (parentFinger.itemsCount < egChunkCapacity)
            MoveTailToInsert(newFinger.myPosInChunkBytes, parentFinger.itemsCount*oneFingerSize-newFinger.myPosInChunkBytes);
        else
            MoveTailToInsert(newFinger.myPosInChunkBytes, (parentFinger.itemsCount-1)*oneFingerSize-newFinger.myPosInChunkBytes);       
    WriteFingerToChunk(newFinger); // myPosInChunkBytes
    // EG_LOG_STUB << "backlink = " << hex << currentFinger.myChunkOffset + posToInsert*oneFingerSize << ", newFinger.nextChunkOffset = " << hex << newFinger.nextChunkOffset << FN;
    // UpdateBackptrOffsets(parentFinger.nextChunkOffset, posToInsert, parentFinger.itemsCount+1); // update backlinks to tail
    if (parentFinger.itemsCount < egChunkCapacity)
        WriteCountToChunk(parentFinger.itemsCount+1);
    res = res && StoreFingersChunk(parentFinger.nextChunkOffset);
    // PrintFingersChunk(fingersBA.data(), "fingers chunk after insert ");
    if (parentFinger.itemsCount < egChunkCapacity)
        parentFinger.itemsCount++;
    ReadChunkMinMaxToParentFinger(parentFinger);
    if (! FingerIsRoot(parentFinger))
        res = res && StoreFingerToFile(parentFinger);
    else { // keep updated rootFinger
        // EG_LOG_STUB << "parentFinger is root" << FN;
        rootFinger.itemsCount = parentFinger.itemsCount;
        rootFinger.minKey = parentFinger.minKey;
        rootFinger.maxKey = parentFinger.maxKey;
        res = res && StoreFingerToFile(rootFinger);
    }    
    return res;
}

//  ============================================================================
//          FINGERS TREE OPERATIONS
//  ============================================================================

template <typename KeyType> inline bool EgFingers<KeyType>::UpdateMinMaxByFlags(egFinger<KeyType>& theFinger) {
    bool res {false};
    if (currentFinger.maxKey > theFinger.maxKey) {
        theFinger.maxKey = currentFinger.maxKey;
        res = true;
    }
    if (currentFinger.minKey < theFinger.minKey) {
        theFinger.minKey = currentFinger.minKey;
        res = true;
    }
    if (res)
        StoreFingerToFile(theFinger);
    return res;
}

template <typename KeyType> inline bool EgFingers<KeyType>::DeleteFingersChunk(uint64_t fingersChunkOffset) {
    uint64_t fileSize = fingersFileStream.getFileSize();
    // EG_LOG_STUB << "fileSize = " << std::dec << (int) fileSize <<"fingersChunkOffset + fingersChunkSize = " << fingersChunkOffset + fingersChunkSize << FN;
    if (fingersChunkOffset + fingersChunkSize == fileSize) { // check if last chunk to delete
        keysCountType chunkCount {0};
        int otherEmptyChunks{0}; // check if other chunks are empty
        int backCount{1};
        while (!chunkCount && (fingersChunkSize * backCount < fingersChunkOffset) && fingersFileStream.good()) {
            fingersFileStream.seekRead(fingersChunkOffset - (fingersChunkSize * backCount) + chunkCountPosition);
            fingersFileStream >> chunkCount;
            // EG_LOG_STUB << "chunkCount = " << std::dec << (int) chunkCount << FN;
            // EG_LOG_STUB << "offset shift = " << std::hex << fingersChunkSize * backCount << FN;
            if (!chunkCount)
                otherEmptyChunks++;
            backCount++;
        }
        // EG_LOG_STUB << "otherEmptyChunks = " << std::dec << otherEmptyChunks << FN;
        fingersFileStream.close();
        std::filesystem::path filename = fingersFileName.c_str(); // experimental::
        std::filesystem::resize_file(filename, fingersChunkOffset - (fingersChunkSize * otherEmptyChunks));  // experimental::
        fingersFileStream.openToUpdate();
    } else {
        memset(localStream-> bufData, 0, fingersChunkSize); // init zero chunk
        return StoreFingersChunk(fingersChunkOffset);
    }
    return fingersFileStream.good();
}

//  ============================================================================
//          FINGER LOOKUP FUNCTIONS
//  ============================================================================

template <typename KeyType> inline bool EgFingers<KeyType>::OpenFileStream() {
    fingersChain.clear();
    if (! fingersFileStream.openToUpdate()) {
        std::cout << "fileStream.openToRead() failed" << std::endl;
        return false;
    }
    if (! rootFingerIsLoaded)
        return LoadRootFingerFromFile(rootFinger, isRootFinger);
    return true;
}

template <typename KeyType> inline void EgFingers<KeyType>::FindFirstFinger(KeyType& Key, CompareFunctionType predicate) {
    int fingerPosition {0}; // parentFinger.itemsCount-1;
    do {
        ReadFingerFromChunk(currentFinger, fingerPosition);
        // EG_LOG_STUB << "fingerPosition = " << fingerPosition << " theKey = " << HEX << theKey 
        //            << " currentFinger.minKey = " << currentFinger.minKey << FN;
    } while (predicate(currentFinger.maxKey, Key) && (++fingerPosition <  parentFinger.itemsCount));
    // ((currentFinger.maxKey < Key) && (++fingerPosition <  parentFinger.itemsCount));
}

template <typename KeyType> inline void EgFingers<KeyType>::FindLastFinger(KeyType& Key, CompareFunctionType predicate) {   
    int fingerPosition { parentFinger.itemsCount-1 }; // last
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while (predicate(currentFinger.maxKey, Key) && (--fingerPosition >= 0));
    // while ((currentFinger.minKey >= Key) && (--fingerPosition >= 0));
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkEQ(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    // EG_LOG_STUB << "Key = " << std::hex << Key << FN;
    bool res {true};
    if (Key < rootFinger.minKey || Key > rootFinger.maxKey) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    // PrintFingerInfo(currentFinger, "currentFinger (FindIndexChunkEQ)");
    while (! currentFinger.myChunkIsLeaf && res) {
        if (Key < currentFinger.minKey || Key > currentFinger.maxKey) {
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindFirstFinger(Key, CompareLT); // (currentValue < key)
        // FindFingerGE(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkGreater(KeyType& Key, CompareFunctionType predicate) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    bool res {true};
    if (predicate(rootFinger.maxKey, Key)) { // (Key > rootFinger.maxKey) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        if (predicate(currentFinger.maxKey, Key)) { // (Key > currentFinger.maxKey) {
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindFirstFinger(Key, predicate); // (currentValue < key)
        // FindFingerGE(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkLess(KeyType& Key, CompareFunctionType predicate) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    bool res {true};
    if (predicate(rootFinger.minKey, Key)) {// (rootFinger.minKey >= Key) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        if (predicate(currentFinger.minKey, Key)) { // (Key <= currentFinger.minKey) { // FIXME check condition cases
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindLastFinger(Key, predicate); // (currentValue >= key)
        // FindFingerLT(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
}

//  ============================================================================
//          DEBUG FUNCTIONS
//  ============================================================================

template <typename KeyType> void EgFingers<KeyType>::PrintFingerInfo(egFinger<KeyType>& fingerInfo, const std::string theMessage) {
    EG_LOG_STUB << theMessage << std::endl; // << "Fingers file: " << fingersFileName << " " 
    EG_LOG_STUB << "fingerInfo.minKey = " << std::hex << (int) fingerInfo.minKey << std::endl;
    EG_LOG_STUB << "fingerInfo.maxKey = " << (int) fingerInfo.maxKey << std::endl;
    EG_LOG_STUB << "fingerInfo.itemsCount = " << std::dec << fingerInfo.itemsCount << std::endl;
    EG_LOG_STUB << "fingerInfo.nextChunkOffset = " << std::hex << fingerInfo.nextChunkOffset << std::endl;
    EG_LOG_STUB << "fingerInfo.myChunkIsLeaf = " << (bool) fingerInfo.myChunkIsLeaf << std::endl;
    EG_LOG_STUB << "fingerInfo.myChunkOffset = "  << fingerInfo.myChunkOffset << std::endl;
    EG_LOG_STUB << "fingerInfo.parentChunkOffset = " << fingerInfo.parentChunkOffset << std::endl;
    EG_LOG_STUB << "fingerInfo.myPosInChunkBytes = " << std::dec << (int) fingerInfo.myPosInChunkBytes << std::endl;
    // EG_LOG_STUB << "fingerInfo.parentPosInChunk = " << (int) fingerInfo.parentPosInChunk << std::endl;
}

template class EgFingers<uint32_t>;
