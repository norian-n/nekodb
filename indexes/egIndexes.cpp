#include "egIndexes.h"
#include "egFingers.h"
#include <filesystem>
using namespace egIndexesSpace;

template <typename KeyType> EgIndexes<KeyType>::EgIndexes(const std::string a_indexName) :
        fingersTree(new EgFingers<KeyType>(a_indexName, this)),
        indexFileName(a_indexName + ".ind"),
        indexFileStream(a_indexName + ".ind"),
        localStream(new EgDataStream(indexChunkSize))
{ }

//  ============================================================================
//          INDEXES TOP API
//  ============================================================================

template <typename KeyType> bool EgIndexes<KeyType>::AddNewIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) {
    KeyType key = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    bool res {true};
    if(! CheckIfIndexFileExists()) { // add new index and finger FIXME TODO files
        res = res && indexFileStream.openToWrite();
        if(res) {
            WriteRootHeader();
            InitIndexChunk(key, dataOffset);
            res = res && StoreIndexChunk(indexHeaderSize);
            if(res) {
                if (fingersTree-> CheckIfFingerFileExists()) 
                    fingersTree-> DeleteFingersFile();
                res = res && fingersTree-> AddNewRootFinger(key, indexHeaderSize);
            }
        }
    } else {
        res = res && fingersTree-> FindIndexesChunkToInsert(key);
        if(res) {
            res = res && OpenIndexFileToUpdate();
            // EG_LOG_STUB << "currentFinger.nextChunkOffset(hex) = " << std::hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;
            LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // sets theChunkCount
            // EG_LOG_STUB << "theChunkCount = " << std::dec << (int) theChunkCount << FN;
            if(theChunkCount < egChunkCapacity) {
                InsertInsideIndexChunk(key, dataOffset);
                theChunkCount++;
                SetCountOfChunk(theChunkCount);
                res = res && StoreIndexChunk(fingersTree-> currentFinger.nextChunkOffset);
                if(res) { // TODO min max
                    SetMinMaxOfFinger(key);
                    res = res && fingersTree-> UpdateFingersChainUp();
                }
            } else {
                KeyType savedKey;
                uint64_t savedDataOffset, savedFwdPtr, savedBackPtr;
                uint64_t newChunkOffset = indexFileStream.getFileSize();
                GetChainPointers(savedFwdPtr, savedBackPtr);
                // EG_LOG_STUB << "fwdPtr = " << std::hex << savedFwdPtr << ", backPtr = " << savedBackPtr << FN;
                if (key >= fingersTree-> currentFinger.maxKey) { // add last
                    localStream-> seek(fwdPtrPosition); // update fwdptr to new chunk
                    *localStream << newChunkOffset;
                    res = res && StoreIndexChunk(fingersTree->currentFinger.nextChunkOffset);
                    res = res && AppendIndexChunk(key, dataOffset, savedFwdPtr, fingersTree-> currentFinger.nextChunkOffset);
                } else {
                    SaveLastIndex(savedKey, savedDataOffset);
                    // EG_LOG_STUB << "savedKey = " << std::hex << (int) savedKey << ", savedDataOffset = " << savedDataOffset << FN;
                    InsertInsideIndexChunk(key, dataOffset);
                    localStream-> seek(fwdPtrPosition); // update fwdptr to new chunk
                    *localStream << newChunkOffset;
                    ReadIndexOnly(fingersTree->currentFinger.minKey, 0); // update fimger values
                    ReadIndexOnly(fingersTree->currentFinger.maxKey, egChunkCapacity-1);
                    res = res && StoreIndexChunk(fingersTree->currentFinger.nextChunkOffset);
                    fingersTree-> newFinger.nextChunkOffset = newChunkOffset; // moved up from function to optimize calls
                    res = res && AppendIndexChunk(savedKey, savedDataOffset, savedFwdPtr, fingersTree-> currentFinger.nextChunkOffset);
                }
                res = res && fingersTree->AddNewUpdateCurrentFinger();
            }
        }
    }
    indexFileStream.close();
    return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::DeleteIndexInChunk() {
    if (theIndexPosition < (theChunkCount - 1)) // move tail
        memmove (localStream-> bufData + theIndexPosition*oneIndexSize, localStream-> bufData + (theIndexPosition+1)*oneIndexSize,  
                 oneIndexSize*(theChunkCount - theIndexPosition - 1));
    memset(localStream-> bufData + (theChunkCount - 1)*oneIndexSize, 0, oneIndexSize); // erase last
}

template <typename KeyType> inline bool EgIndexes<KeyType>::RemoveChunkFromChain() {
    uint64_t savedFwdPtr, savedBackPtr;
    GetChainPointers(savedFwdPtr, savedBackPtr);
    // EG_LOG_STUB << "savedFwdPtr = " << std::hex << savedFwdPtr << ", savedBackPtr = " << savedBackPtr << FN;
    // if (! savedFwdPtr && ! savedBackPtr) // FIXME check if last chunk, delete files
    if (savedBackPtr) {        
        indexFileStream.seekWrite(savedBackPtr + fwdPtrPosition);
        indexFileStream << savedFwdPtr;
    } else { // update header
        indexFileStream.seekWrite(0);
        indexFileStream << savedFwdPtr;
    }
    if (savedFwdPtr)
    {
        indexFileStream.seekWrite(savedFwdPtr + backPtrPosition);
        indexFileStream << savedBackPtr;
    }
    return indexFileStream.good();
}

template <typename KeyType> inline void EgIndexes<KeyType>::DeleteIndexesChunk(uint64_t indexesChunkOffset) {
    uint64_t fileSize = indexFileStream.getFileSize();
    // EG_LOG_STUB << "fileSize = " << std::dec << (int) fileSize <<"fingersChunkOffset + indexChunkSize = " << fingersChunkOffset + indexChunkSize << FN;
    if (indexesChunkOffset + indexChunkSize == fileSize) { // check if last chunk to delete
        keysCountType chunkCount {0};
        int otherEmptyChunks{0}; // check if other chunks are empty
        int backCount{1};
        while (!chunkCount && (indexChunkSize * backCount < indexesChunkOffset) && indexFileStream.good()) {
            indexFileStream.seekRead(indexesChunkOffset - (indexChunkSize * backCount) + chunkCountPosition);
            indexFileStream >> chunkCount;
            // EG_LOG_STUB << "chunkCount = " << std::dec << (int) chunkCount << FN;
            // EG_LOG_STUB << "offset shift = " << std::hex << indexChunkSize * backCount << FN;
            if (!chunkCount)
                otherEmptyChunks++;
            backCount++;
        }
        // EG_LOG_STUB << "otherEmptyChunks = " << std::dec << otherEmptyChunks << FN;
        indexFileStream.close();
        std::filesystem::path filename = indexFileName.c_str(); // experimental::
        std::filesystem::resize_file(filename, indexesChunkOffset - (indexChunkSize * otherEmptyChunks));
        indexFileStream.openToUpdate();
    } else {
        memset(localStream-> bufData, 0, indexChunkSize); // init zero chunk
        StoreIndexChunk(indexesChunkOffset);
    }
}

template <typename KeyType> bool EgIndexes<KeyType>::DeleteIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) {
    KeyType key = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    theKey = key;
    theDataOffset = dataOffset;
    bool res = fingersTree-> FindIndexChunkEQ(key); // get currentFinger
    if (res) {
        res = res && OpenIndexFileToUpdate();
        if (res) {
            res = res && FindIndexByDataOffset(); // uses currentFinger, theKey and theDataOffset sets theIndexesChunkOffset, theIndexPosition, fingersChainFlag
            if (res) {
                if (! fingersChainFlag) {
                    fingersTree-> currentFinger.myChunkOffset = GetFingersChunkOffset();
                    fingersTree-> currentFinger.nextChunkOffset = theIndexesChunkOffset;
                }
                if (theChunkCount > 1) {  // not last index in the chunk
                    DeleteIndexInChunk(); // set currentFinger for update
                    theChunkCount--;
                    SetCountOfChunk(theChunkCount);
                    res = res && StoreIndexChunk(theIndexesChunkOffset);
                    ReadIndexOnly(fingersTree->currentFinger.minKey, 0); // update fimger values
                    ReadIndexOnly(fingersTree->currentFinger.maxKey, theChunkCount-1);
                    if (fingersChainFlag)
                        fingersTree-> UpdateFingersChainUp();
                    else
                        fingersTree-> UpdateFingersByBackptrs(); // FIXME set minMaxUpdated
                } else {
                    RemoveChunkFromChain();
                    DeleteIndexesChunk(theIndexesChunkOffset);
                    if (fingersChainFlag)
                        res = res && fingersTree-> DeleteCurrentFingerByChain();
                    else
                        res = res && fingersTree-> DeleteFingerByBackptrs();
                }
            }
        }
    }
    indexFileStream.close();
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::UpdateDataOffset(EgByteArrayAbstractType& keyBA, uint64_t oldDataOffset, uint64_t newDataOffset) {
    KeyType key = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    bool res {true};
    theKey = key;
    theDataOffset = oldDataOffset;
    res = res && fingersTree-> FindIndexChunkEQ(key); // get currentFinger
    if (res) {
        res = res && OpenIndexFileToUpdate();
        if (res) {
            res = res && FindIndexByDataOffset(); // sets theIndexesChunkOffset, theIndexPosition
            if (res) {
                indexFileStream.seekWrite(theIndexesChunkOffset + theIndexPosition*oneIndexSize + sizeof(KeyType));
                indexFileStream << newDataOffset;
                res = res && indexFileStream.good();
            }
        }
    }
    fingersTree-> fingersChain.clear();
    fingersTree-> fingersFileStream.close();
    indexFileStream.close();
    return res;
}

//  ============================================================================
//          INDEX OPERATIONS
//  ============================================================================

template <typename KeyType> inline void EgIndexes<KeyType>::InitIndexChunk(KeyType& key, uint64_t& dataOffset) {
    keysCountType initCount {1};
    memset(localStream-> bufData, 0, indexChunkSize);
    localStream-> seek(0);          // write first key of all
    *localStream << key;
    *localStream << dataOffset;  // data node offset
    localStream-> seek(chunkCountPosition); // write indexes count
    *localStream << initCount;
}

template <typename KeyType> inline void EgIndexes<KeyType>::InitMinMaxFlags() {
    fingersTree-> anyMinMaxChanged = false;
    // fingersTree-> minValueChanged = false;
    // fingersTree-> maxValueChanged = false;
}

template <typename KeyType> inline void EgIndexes<KeyType>::SetMinMaxOfFinger(KeyType& key) {
    // EG_LOG_STUB << "key = " << std::hex << (int) key << FN;
    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
    if (key < fingersTree-> currentFinger.minKey)
        fingersTree-> currentFinger.minKey = key;
    else if (key > fingersTree-> currentFinger.maxKey)
        fingersTree-> currentFinger.maxKey = key;
}

//  ============================================================================
//          INDEX FILE OPERATIONS (indexFileStream)
//  ============================================================================

template <typename KeyType> inline void EgIndexes<KeyType>::WriteRootHeader() {
    indexFileStream.seekWrite(0);
    uint64_t localHeaderSize {indexHeaderSize};
    indexFileStream << localHeaderSize;       // first chunk offset
    // uint64_t placeholder { 0xAFAFAFAF };
    // indexFileStream << placeholder; // reserved for chunks recycling list
    // return indexFileStream.good();  // paranoid mode
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadIndexChunk(uint64_t chunkOffset) {
    indexFileStream.seekRead(chunkOffset);
    // if (! indexFileStream.seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << std::hex << fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.read((char *) localStream-> bufData, indexChunkSize);
    GetCountFromChunk(theChunkCount);
    // EG_LOG_STUB << "theChunkCount : " << theChunkCount << " indexFileStream.good(): " << indexFileStream.good() << FN;
    // EG_LOG_STUB << "bad(): " << indexFileStream.fileStream.bad() << " fail(): " << indexFileStream.fileStream.fail() << " eof(): " << indexFileStream.fileStream.eof() << FN;
    // EG_LOG_STUB << "is_open(): " << indexFileStream.fileStream.is_open() << FN;
    return indexFileStream.good();
}

template <typename KeyType> inline bool EgIndexes<KeyType>::StoreIndexChunk(const uint64_t chunkOffset) {
    // PrintIndexesChunk(localStream-> bufData, "chunk to save " + FNS);
    indexFileStream.seekWrite(chunkOffset); // FIXME check result in paranoid mode
    // EG_LOG_STUB << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << std::hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.write((char *) localStream-> bufData, indexChunkSize);
    return indexFileStream.good();
}

template <typename KeyType> bool EgIndexes<KeyType>::StoreFingerOffset(uint64_t indexesChunkOffset, uint64_t fingersChunkOffset)
{
    indexFileStream.seekWrite(indexesChunkOffset + fingersChunkOffsetPosition);
    indexFileStream << fingersChunkOffset;
    return indexFileStream.good();
}

//  ============================================================================
//          INDEX CHUNK OPERATIONS (localStream)
//  ============================================================================

template <typename KeyType> inline void EgIndexes<KeyType>::GetCountFromChunk(keysCountType& refCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream >> refCount;
}

template <typename KeyType> inline void EgIndexes<KeyType>::SetCountOfChunk(keysCountType chunkCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream << chunkCount;
}


template <typename KeyType> inline uint64_t EgIndexes<KeyType>::GetFingersChunkOffset() {
    uint64_t fingersChunkOffset {0};
    localStream  -> seek(fingersChunkOffsetPosition);
    *localStream >> fingersChunkOffset;
    return fingersChunkOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::MoveTailToInsert(char* chunkPtr, int indexPosition) {
    // EG_LOG_STUB << "chunk before memmove" << QByteArray(chunk, egChunkCapacity*oneIndexSize).toHex() << FN;
    // EG_LOG_STUB << "indexPosition = " << std::dec << indexPosition << FN;
    // EG_LOG_STUB << "theChunkCount = " << std::dec << theChunkCount << FN;
    memmove (chunkPtr + (indexPosition+1)*oneIndexSize, chunkPtr + indexPosition*oneIndexSize,
             oneIndexSize*(theChunkCount - indexPosition));
    // PrintIndexesChunk(chunk, "chunk after memmove " + FNS);
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(KeyType& currentIndex, int indexPosition) {
    localStream-> seek(indexPosition*oneIndexSize);
    *localStream >> currentIndex;
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(KeyType& currentIndex) {
    ReadIndexOnly(currentIndex, theIndexPosition); // FIXME check
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexAndOffset(KeyType& currentIndex, uint64_t& dataOffset, int indexPosition) {
    localStream  -> seek(indexPosition*oneIndexSize);
    *localStream >> currentIndex;
    *localStream >> dataOffset; 
}

template <typename KeyType> inline void EgIndexes<KeyType>::InsertInsideIndexChunk(KeyType& key, uint64_t& dataOffset) {
    // bool res {true};
    // theKey = key;
    FindIndexPositionToInsert(key); // sets theIndexPosition
    // EG_LOG_STUB << "theChunkCount = " << std::dec << (int) theChunkCount << FN;
    if (theIndexPosition < theChunkCount) // not last
        MoveTailToInsert((char*) localStream-> bufData, theIndexPosition);
    // EG_LOG_STUB << "theIndexPosition = " << std::dec << (int) theIndexPosition << FN;
    WriteIndexValues(key, dataOffset, theIndexPosition); // write index key and offset
    // return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::WriteIndexValues(KeyType key, uint64_t dataOffset, int position) {
    localStream-> seek(position*oneIndexSize);
    *localStream << key;
    *localStream << dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::SaveLastIndex(KeyType& key, uint64_t& dataOffset) {
    localStream-> seek((egChunkCapacity-1) * oneIndexSize);
    *localStream >> key;
    *localStream >> dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::GetChainPointers(uint64_t& nextPtr, uint64_t& prevPtr) {
    localStream-> seek(fwdPtrPosition);
    *localStream >> nextPtr;
    *localStream >> prevPtr;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::AppendIndexChunk(KeyType& key, uint64_t& dataOffset, uint64_t& fwdPtr, uint64_t& backPtr) {
    bool res {true};
    // EG_LOG_STUB << "key = " << std::hex << (int) key << ", dataOffset = " << dataOffset << FN;
    memset(localStream-> bufData, 0, indexChunkSize); // clear
    WriteIndexValues(key, dataOffset, 0);
    SetCountOfChunk(1);
    localStream-> seek(fwdPtrPosition);
    *localStream << fwdPtr;
    *localStream << backPtr;
    fingersTree-> newFinger.minKey     = key;
    fingersTree-> newFinger.maxKey     = key;
    fingersTree-> newFinger.nextChunkOffset = indexFileStream.getFileSize();
    fingersTree-> newFinger.myChunkIsLeaf = 0xFF; // is leaf
    if(fwdPtr) {
        indexFileStream.seekWrite(fwdPtr + backPtrPosition); // backptr of next chunk
        indexFileStream << fingersTree-> newFinger.nextChunkOffset;
        res = res && indexFileStream.good();
    }   
    res = res && StoreIndexChunk(fingersTree-> newFinger.nextChunkOffset); // save new chunk, fingers already updated to upper level
    return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::FindIndexPositionToInsert(KeyType& key) {
    KeyType currentIndex;
    theIndexPosition = theChunkCount;
    while (theIndexPosition > 0) {
        ReadIndexOnly(currentIndex, theIndexPosition-1);
        if (currentIndex > key)
           theIndexPosition--;
        else
            break;
    }
}

template <typename KeyType> inline uint64_t EgIndexes<KeyType>::ReadDataOffsetOnly(int indexPosition) {
    uint64_t dataOffset;
    localStream-> seek(indexPosition*oneIndexSize + sizeof(KeyType));
    *localStream >> dataOffset;
    return dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::LoadDataFromChunkUp(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, 
                                                                                int indexPosition, uint64_t indexesToLoad) {
    int indexCount {0};
    while (indexPosition < theChunkCount && indexCount < indexesToLoad) {
        index_offsets.insert(ReadDataOffsetOnly(indexPosition++));
        indexCount++;
    }
    currentIndexQuantity += indexCount; // update global position
    transactionPosInChunk = indexPosition;
    if (indexPosition == theChunkCount) {   // loaded all indexes
        localStream-> seek(fwdPtrPosition);
        *localStream >> nextChunkOffset;
        transactionPosInChunk = 0;
    }
    reallyLoaded += (uint64_t) indexCount;
}

template <typename KeyType> inline void EgIndexes<KeyType>::LoadDataFromChunkDown(std::set<uint64_t>& index_offsets, uint64_t& prevChunkOffset, 
                                                                                int indexPosition, uint64_t indexesToLoad) {
    int indexCount {0};
    while (indexPosition >= 0 && indexCount < indexesToLoad) {
        index_offsets.insert(ReadDataOffsetOnly(indexPosition--));        
        indexCount++;
    }
    currentIndexQuantity += indexCount; // update global position
    transactionPosInChunk = indexPosition < 0 ? 0 : indexPosition;
    if (indexPosition < 0) {   // loaded all indexes
        localStream-> seek(backPtrPosition);
        *localStream >> prevChunkOffset;
        if (prevChunkOffset) {
            GetCountFromChunk(transactionPosInChunk);
            transactionPosInChunk--;
        }
    }
    reallyLoaded += (uint64_t) indexCount;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataUp(std::set<uint64_t>& index_offsets) {
    bool res {true};
    uint64_t nextChunkOffset {0};
    LoadDataFromChunkUp(index_offsets, nextChunkOffset, theIndexPosition, egChunkCapacity);
    while (nextChunkOffset && res) {
        res = res && LoadIndexChunk(nextChunkOffset);
        if (res) {
            LoadDataFromChunkUp(index_offsets, nextChunkOffset, 0, egChunkCapacity);
            res = res && indexFileStream.good();
        }
    }
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataPortionUp(std::set<uint64_t>& index_offsets, uint64_t maxQuantity) {
    bool res {true};
    reallyLoaded = 0;
    uint64_t chunkIndexesToLoad = (maxQuantity >= egChunkCapacity) ? egChunkCapacity : maxQuantity;
    if  (currentChunkOffset && maxQuantity)
        LoadDataFromChunkUp(index_offsets, currentChunkOffset, theIndexPosition, chunkIndexesToLoad);
    if (maxQuantity)
        maxQuantity = (maxQuantity > chunkIndexesToLoad) ? maxQuantity-chunkIndexesToLoad : 0;
    while (currentChunkOffset && maxQuantity && res) {
        res = res && LoadIndexChunk(currentChunkOffset);
        if (res) {
            LoadDataFromChunkUp(index_offsets, currentChunkOffset, 0, chunkIndexesToLoad);
            res = res && indexFileStream.good();
            maxQuantity = (maxQuantity > chunkIndexesToLoad) ? maxQuantity-chunkIndexesToLoad : 0;
        }
    }
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataPortionDown(std::set<uint64_t>& index_offsets, uint64_t maxQuantity) {
    bool res {true};
    reallyLoaded = 0;
    uint64_t chunkIndexesToLoad = (maxQuantity >= egChunkCapacity) ? egChunkCapacity : maxQuantity;
    if  (currentChunkOffset && maxQuantity)
        LoadDataFromChunkDown(index_offsets, currentChunkOffset, theIndexPosition, chunkIndexesToLoad);
    if (maxQuantity)
        maxQuantity = (maxQuantity > chunkIndexesToLoad) ? maxQuantity-chunkIndexesToLoad : 0;
    while (currentChunkOffset && maxQuantity && res) {
        res = res && LoadIndexChunk(currentChunkOffset);
        if (res) {
            LoadDataFromChunkDown(index_offsets, currentChunkOffset, 0, chunkIndexesToLoad);
            res = res && indexFileStream.good();
            maxQuantity = (maxQuantity > chunkIndexesToLoad) ? maxQuantity-chunkIndexesToLoad : 0;
        }
    }
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::FindFirstIndexPos(CompareFunctionType myCompareFunc) {
    KeyType currentIndex;
    theIndexPosition = 0;
    // EG_LOG_STUB << "theChunkCount : " << theChunkCount << FN;
    while (theIndexPosition < theChunkCount) {
        ReadIndexOnly(currentIndex);
        // EG_LOG_STUB << "currentIndex : " << (int) currentIndex << FN;
        if (myCompareFunc(currentIndex,theKey))
            return true;
        theIndexPosition++;
    }
    return false; // index not found in chunk
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataByPredicateUp(std::set<uint64_t>& index_offsets, CompareFunctionType predicate) {
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res) {
        res = FindFirstIndexPos(predicate); // sets theIndexPosition
        if (res)
            res = LoadDataUp(index_offsets);
    }
    return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::LoadDataFromChunkDown(std::set<uint64_t>& index_offsets, uint64_t& prevChunkOffset, int indexPosition) {
    while (indexPosition >= 0)
        index_offsets.insert(ReadDataOffsetOnly(indexPosition--));
    localStream-> seek(backPtrPosition);
    *localStream >> prevChunkOffset;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataDown(std::set<uint64_t>& index_offsets) {
    bool res {true};
    uint64_t prevChunkOffset {0};
    LoadDataFromChunkDown(index_offsets, prevChunkOffset, theIndexPosition);
    while (prevChunkOffset && res) {
        res = res && LoadIndexChunk(prevChunkOffset);
        if (res) {
            LoadDataFromChunkDown(index_offsets, prevChunkOffset, theChunkCount-1);
            res = res && indexFileStream.good();
        }
    }
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::FindLastIndexPos(CompareFunctionType myCompareFunc) {
    KeyType currentIndex;
    theIndexPosition = theChunkCount-1;
    while (theIndexPosition >= 0) {
        ReadIndexOnly(currentIndex);
        if (myCompareFunc(currentIndex,theKey))
            return true;
        theIndexPosition--;
    }
    return false; // index not found in chunk
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataByPredicateDown(std::set<uint64_t>& index_offsets, CompareFunctionType predicate) {
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res) {
        res = FindLastIndexPos(predicate); // sets theIndexPosition
        if (res)
            res = LoadDataDown(index_offsets);
    }
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataFirst(std::set<uint64_t>& index_offsets, uint64_t& maxQuantity) {
    currentIndexQuantity = 0;
    indexFileStream.seekRead(0); // get first index offset from header
    indexFileStream >> currentChunkOffset;
    bool res = LoadIndexChunk(currentChunkOffset); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData
    theIndexPosition = 0;
    if (res)
        res = LoadDataPortionUp(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataNextUp(std::set<uint64_t>& index_offsets, uint64_t& maxQuantity) {
    bool res = LoadIndexChunk(currentChunkOffset); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData
    theIndexPosition = transactionPosInChunk;
    if (res)
        res = LoadDataPortionUp(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataFromChunkEqualCount(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, 
                                                                                        int indexPosition, uint64_t& indexesToLoad) {
    bool res {true};
    int indexCount {0};
    KeyType currentIndex;
    uint64_t dataOffset;
    indexPosition = transactionPosInChunk;
    while (indexPosition < theChunkCount && indexCount < indexesToLoad) {
        ReadIndexAndOffset(currentIndex, dataOffset, indexPosition++);
        if (currentIndex == theKey) {
            index_offsets.insert(dataOffset);
            indexCount++;
        } else {
            res = false;
            break; // stop loading
        }
    }
    currentIndexQuantity += indexCount;     // update global position
    transactionPosInChunk = indexPosition;
    if (indexPosition == theChunkCount) {   // loaded all indexes
        localStream-> seek(fwdPtrPosition);
        *localStream >> nextChunkOffset;
        transactionPosInChunk = 0;
    }
    reallyLoaded += (uint64_t) indexCount;
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataPortionUpEqual(std::set<uint64_t>& index_offsets, uint64_t& maxQuantity) {
    bool res {true};
    uint64_t nextChunkOffset {0};
    if (! LoadDataFromChunkEqualCount(index_offsets, nextChunkOffset, theIndexPosition, maxQuantity)) // FIXME TODO as LoadDataFromChunkUp
        return indexFileStream.good();
    while (nextChunkOffset && res) {
        res = res && LoadIndexChunk(nextChunkOffset);
        if (res) {
            if (! LoadDataFromChunkEqualCount(index_offsets, nextChunkOffset, 0, maxQuantity))
                break;
            res = res && indexFileStream.good();
        }
    }
    return res && indexFileStream.good();
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataEQFirst(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    if (! fingersTree-> FindIndexChunkEQ(key))
        return false;
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareEQ);
    if (res)
        res = LoadDataPortionUpEqual(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataEQ(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) {
    theKey = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    // EG_LOG_STUB << "key : " << std::hex << (int) theKey << FN;
    if (! fingersTree-> FindIndexChunkEQ(theKey))
        return false;
    if ( ! indexFileStream.openToRead()) {
        EG_LOG_STUB << "ERROR: cant open indexes file: " << indexFileName << FN;
        return false;       
    }
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset);
    // EG_LOG_STUB << "nextChunkOffset : " << fingersTree-> currentFinger.nextChunkOffset << FN;
    if (res)
        res = FindFirstIndexPos(CompareEQ);
    if (res)
        res = LoadDataUpEqual(index_offsets);
    indexFileStream.close();
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataGE(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) {
    theKey = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    if (! fingersTree-> FindIndexChunkGE(theKey))
        return false;
    if ( ! indexFileStream.openToRead()) {
        EG_LOG_STUB << "ERROR: cant open indexes file: " << indexFileName << FN;
        return false;       
    }
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareGE);
    if (res)
        res = LoadDataUp(index_offsets);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataGT(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) {
    theKey = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    if (! fingersTree-> FindIndexChunkGT(theKey))
        return false;
    if ( ! indexFileStream.openToRead()) {
        EG_LOG_STUB << "ERROR: cant open indexes file: " << indexFileName << FN;
        return false;       
    }
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareGT);
    if (res)
        res = LoadDataUp(index_offsets);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataLE(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) {
    theKey = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    if (! fingersTree-> FindIndexChunkLE(theKey))
        return false;
    if ( ! indexFileStream.openToRead()) {
        EG_LOG_STUB << "ERROR: cant open indexes file: " << indexFileName << FN;
        return false;       
    }
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareLE);
    if (res)
        res = LoadDataDown(index_offsets);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadAllDataLT(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) {
    theKey = *(reinterpret_cast<KeyType*> (keyBA.dataChunk));
    if (! fingersTree-> FindIndexChunkLT(theKey))
        return false;
    if ( ! indexFileStream.openToRead()) {
        EG_LOG_STUB << "ERROR: cant open indexes file: " << indexFileName << FN;
        return false;       
    }
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareLT);
    if (res)
        res = LoadDataDown(index_offsets);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataEQNext(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    bool res = LoadIndexChunk(currentChunkOffset); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData
    theIndexPosition = transactionPosInChunk;
    if (res)
        res = LoadDataPortionUpEqual(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataGEFirst(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    if (! fingersTree-> FindIndexChunkGE(key))
        return false;
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareGE);
    if (res)
        res = LoadDataPortionUp(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataGTFirst(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    if (! fingersTree-> FindIndexChunkGT(key))
        return false;
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res)
        res = FindFirstIndexPos(CompareGT);
    if (res)
        res = LoadDataPortionUp(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataLEFirst(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    if (! fingersTree-> FindIndexChunkLE(key))
        return false;
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // to localStream-> bufData
    if (res)
        res = FindLastIndexPos(CompareLE);
    if (res)
        res = LoadDataPortionDown(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::LoadDataLTFirst(std::set<uint64_t>& index_offsets, KeyType& key, uint64_t& maxQuantity) {
    if (! fingersTree-> FindIndexChunkLT(key))
        return false;
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res)
        res = FindLastIndexPos(CompareLT);
    if (res)
        res = LoadDataPortionDown(index_offsets, maxQuantity);
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataFromChunkEqual(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition) {
    KeyType currentIndex;
    uint64_t dataOffset;
    while (indexPosition < theChunkCount) {
        ReadIndexAndOffset(currentIndex, dataOffset, indexPosition++);
        if (currentIndex == theKey)
            index_offsets.insert(dataOffset);
        else
            return false; // stop loading
    }
    localStream-> seek(fwdPtrPosition);
    *localStream >> nextChunkOffset;
    return true;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataUpEqual(std::set<uint64_t>& index_offsets) {
    bool res {true};
    uint64_t nextChunkOffset {0};
    if (! LoadDataFromChunkEqual(index_offsets, nextChunkOffset, theIndexPosition))
        return indexFileStream.good();
    while (nextChunkOffset && res) {
        res = res && LoadIndexChunk(nextChunkOffset);
        if (res) {
            if (! LoadDataFromChunkEqual(index_offsets, nextChunkOffset, 0))
                break;
            res = res && indexFileStream.good();
        }
    }
    return res && indexFileStream.good();
}


template <typename KeyType> inline bool EgIndexes<KeyType>::LoadDataEqual(std::set<uint64_t>& index_offsets) {
    bool res = LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData
    if (res) {
        res = FindFirstIndexPos(CompareEQ); // sets theIndexPosition
        if (res)
            res = LoadDataUpEqual(index_offsets);
    }
    return res;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::FindIndexByDataOffset() { // uses theKey, theDataOffset sets theIndexesChunkOffset, theIndexPosition, fingersChainFlag
    KeyType currentIndex;
    uint64_t dataOffset;
    bool res {true};
    fingersChainFlag = true;
    theIndexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    while (theIndexesChunkOffset && res) {
        res = res && LoadIndexChunk(theIndexesChunkOffset); // sets theChunkCount
        theIndexPosition = 0;
        while (theIndexPosition < theChunkCount) { // look for offset in the chunk
            ReadIndexAndOffset(currentIndex, dataOffset, theIndexPosition);
            if (currentIndex == theKey && dataOffset == theDataOffset) // got it
                return true;
            if (currentIndex > theKey) // out of range
                return false;
            theIndexPosition++;
        }
        fingersChainFlag = false;               // chunk moved to next, use backptrs
        localStream  -> seek(fwdPtrPosition);   // go to next chunk by chain ptr
        *localStream << theIndexesChunkOffset;
    }
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesChainFwd(uint64_t &doubleSpeedOffset)
{
    if (doubleSpeedOffset)
    {
        indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize + sizeof(uint64_t)));
        indexFileStream >> doubleSpeedOffset;

        if (doubleSpeedOffset == nextOffsetPtr)
            return false;

        if (doubleSpeedOffset)
        {
            indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize + sizeof(uint64_t)));
            indexFileStream >> doubleSpeedOffset;

            if (doubleSpeedOffset == nextOffsetPtr)
                return false;
        }
    }

    return true;
}

template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesChainBack()
{
    uint64_t doubleSpeedOffset = prevOffsetPtr;

    while (prevOffsetPtr && doubleSpeedOffset)
    {
        indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize));
        indexFileStream >> doubleSpeedOffset;

        if (doubleSpeedOffset == prevOffsetPtr)
            return false;

        if (doubleSpeedOffset)
        {
            indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize));
            indexFileStream >> doubleSpeedOffset;

            if (doubleSpeedOffset == prevOffsetPtr)
                return false;
        }

        indexFileStream.seekRead(prevOffsetPtr + (egChunkCapacity * oneIndexSize));
        indexFileStream >> prevOffsetPtr;
    }

    return true;
}

template class EgIndexes<uint32_t>;