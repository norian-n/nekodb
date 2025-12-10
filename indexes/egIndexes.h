#pragma once
#include <iostream> // debug
#include <fstream>
#include <set>
#include <vector>
// #include <experimental/filesystem> 
#include "egCoreIndexTypes.h"
#include "../service/egByteArray.h"
#include "../service/egFileType.h"
#include "egFingers.h"
// template <typename KeyType> class EgFingers;

class EgIndexesAbstractType { public:
    virtual bool AddNewIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) = 0;
    virtual bool DeleteIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) = 0;
    virtual bool UpdateDataOffset(EgByteArrayAbstractType& keyBA, uint64_t oldDataOffset, uint64_t newDataOffset) = 0;

    virtual bool LoadAllDataEQ(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA) = 0;

    virtual ~EgIndexesAbstractType() {}
};

template <typename KeyType> class EgIndexes : public EgIndexesAbstractType { public: // indexes chain part of indexes-fingers complex
    const uint64_t            indexHeaderSize       = sizeof(uint64_t);
    const egMaxStreamSizeType oneIndexSize          = sizeof(KeyType) + sizeof(uint64_t); // key and data offset
    const egMaxStreamSizeType fwdPtrPosition        = egIndexesSpace::egChunkCapacity * oneIndexSize;
    const egMaxStreamSizeType backPtrPosition       = fwdPtrPosition   + sizeof(uint64_t);
    const egMaxStreamSizeType chunkCountPosition    = backPtrPosition  + sizeof(uint64_t);
    const egMaxStreamSizeType fingersChunkOffsetPosition  = chunkCountPosition + sizeof(keysCountType);
    const egMaxStreamSizeType indexChunkSize        =  fingersChunkOffsetPosition + sizeof(uint64_t);
    bool fingersChainFlag {true};
    KeyType  theKey;                // index key to process
    uint64_t theDataOffset;         // offsets in data nodes file
    uint64_t theNewDataOffset;
    uint64_t theOldDataOffset;
    keysCountType theChunkCount;    // indexes count in the chunk for chain opers
    uint64_t theIndexesChunkOffset; // file position for chain connect
    int      theIndexPosition;      // position in the chunk
    uint64_t prevOffsetPtr;         // chunks chain operations
    uint64_t nextOffsetPtr;
    uint64_t currentIndexQuantity;  // position savepoint for Load*DataNext functions
    uint64_t currentChunkOffset;    // chunk savepoint for Load*DataNext functions
    keysCountType  transactionPosInChunk; // chunk position savepoint for Load*DataNext functions
    uint64_t reallyLoaded  {0};     // first/next opers support
    uint64_t transactionID {0};     // FIXME TODO first/next opers support
    EgFingers<KeyType>*     fingersTree {nullptr};  // fingers tree object ptr
    egDataStream*           localStream {nullptr};  // chunk buffer operations
    EgIndexStruct<KeyType>  indexData;              // index data wrapper for flexibility
    EgFileType              indexFileStream;        // file operations
    std::string             indexFileName;
    
    EgIndexes(const std::string a_indexName);
    virtual ~EgIndexes() { delete localStream; }
        // top API
    bool AddNewIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) override;
    bool DeleteIndex(EgByteArrayAbstractType& keyBA, uint64_t dataOffset) override ;
    bool UpdateDataOffset(EgByteArrayAbstractType& keyBA, uint64_t oldDataOffset, uint64_t newDataOffset) override;
        // load data top API
    bool LoadAllDataFirst(std::set<uint64_t>& index_offsets, uint64_t& maxQuantity);
    bool LoadDataNextUp (std::set<uint64_t>& index_offsets, uint64_t& maxQuantity);

    bool LoadAllDataEQ(std::set<uint64_t>& index_offsets, EgByteArrayAbstractType& keyBA);
    
    bool LoadDataEQFirst(std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity); // FIXME TODO
    bool LoadDataEQNext (std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity);
    
    bool LoadDataGEFirst(std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity);
    bool LoadDataGTFirst(std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity);
    bool LoadDataLEFirst(std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity);
    bool LoadDataLTFirst(std::set<uint64_t>& index_offsets, KeyType& key,  uint64_t& maxQuantity);
        // service ops
    inline void DeleteIndexInChunk();
    inline bool RemoveChunkFromChain();
    inline void DeleteIndexesChunk(uint64_t indexesChunkOffset);    
    inline bool FindIndexByDataOffset();
    inline void InitIndexChunk(KeyType& key, uint64_t& dataOffset);
        // files ops
    inline bool CheckIfIndexFileExists(){ return indexFileStream.checkIfExists(); }
    inline bool OpenIndexFileToUpdate() { return indexFileStream.openToUpdate(); }
    inline bool DeleteIndexesFile(); // FIXME check file delete
    inline void WriteRootHeader(); // meta-info of indexes (first chunk for loadAll, empty chain(TBD)), also for non-zero offset
    bool LoadIndexChunk(uint64_t chunkOffset);             // to localStream-> bufData
    inline bool StoreIndexChunk(const uint64_t nextChunkOffset);  // from localStream-> bufData
    bool StoreFingerOffset(uint64_t indexesChunkOffset, uint64_t fingersChunkOffset);   // update finger backptr
        // chunk ops
    inline void GetCountFromChunk(keysCountType& refCount);
    inline void SetCountOfChunk(keysCountType chunkCount);
    inline uint64_t GetFingersChunkOffset();
        // service ops
    inline void InitMinMaxFlags();
    inline void SetMinMaxOfFinger(KeyType& key);
        // insert ops
    inline void FindIndexPositionToInsert(KeyType& key);
    inline void MoveTailToInsert(char* chunkPtr, int indexPosition);
    inline void ReadIndexOnly(KeyType& currentIndex, int indexPosition);
    inline void ReadIndexOnly(KeyType& currentIndex);
    inline void ReadIndexAndOffset(KeyType& currentIndex, uint64_t& dataOffset, int indexPosition);
    inline void InsertInsideIndexChunk(KeyType& key, uint64_t& dataOffset);
    inline void WriteIndexValues(KeyType key, uint64_t dataOffset, int position);    
    inline void SaveLastIndex(KeyType& key, uint64_t& dataOffset);
    inline void GetChainPointers(uint64_t& nextPtr, uint64_t& prevPtr);
    inline bool AppendIndexChunk(KeyType& key, uint64_t& dataOffset, uint64_t& fwdPtr, uint64_t& backPtr);
    // int SplitIndexChunk(); // TODO
    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    static bool CompareEQ (KeyType& currentIndex, KeyType& key) {
        // EG_LOG_STUB << "currentIndex : " << (int) currentIndex << " key: " << key << FN;
        return (currentIndex == key); }
    static bool CompareGT (KeyType& currentIndex, KeyType& key) {return (currentIndex > key);}
    static bool CompareGE (KeyType& currentIndex, KeyType& key) {return (currentIndex >= key);}
    static bool CompareLT (KeyType& currentIndex, KeyType& key) {return (currentIndex < key);}
    static bool CompareLE (KeyType& currentIndex, KeyType& key) {return (currentIndex <= key);}
        // load data up
    inline uint64_t ReadDataOffsetOnly(int indexPosition);
    inline void LoadDataFromChunkUp(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition, uint64_t indexesToLoad);
    inline void LoadDataFromChunkDown(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition, uint64_t indexesToLoad);
    inline bool LoadDataUp(std::set<uint64_t>& index_offsets);
    inline bool LoadDataPortionUp(std::set<uint64_t>& index_offsets, uint64_t maxQuantity);
    inline bool LoadDataPortionUpEqual(std::set<uint64_t>& index_offsets, uint64_t& maxQuantity);
    inline bool LoadDataPortionDown(std::set<uint64_t>& index_offsets, uint64_t maxQuantity);
    inline bool FindFirstIndexPos(CompareFunctionType predicate);
    inline bool LoadDataByPredicateUp(std::set<uint64_t>& index_offsets, CompareFunctionType predicate);
        // load data down
    inline void LoadDataFromChunkDown(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition);
    inline bool LoadDataDown(std::set<uint64_t>& index_offsets);
    inline bool FindLastIndexPos(CompareFunctionType predicate);
    inline bool LoadDataByPredicateDown(std::set<uint64_t>& index_offsets, CompareFunctionType predicate);
    inline bool LoadDataFromChunkEqual(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition);
    inline bool LoadDataUpEqual(std::set<uint64_t>& index_offsets);
    inline bool LoadDataEqual(std::set<uint64_t>& index_offsets);
    inline bool LoadDataFromChunkEqualCount(std::set<uint64_t>& index_offsets, uint64_t& nextChunkOffset, int indexPosition, uint64_t& maxQuantity);
        // debug
    inline bool checkIndexesChainFwd(uint64_t &doubleSpeedOffset);     // check indexes chain for loops
    inline bool checkIndexesChainBack();
};
