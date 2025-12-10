#pragma once
#include <iostream> // debug
#include <fstream>
#include <vector>
// #include <experimental/filesystem> 
#include "egCoreIndexTypes.h"
#include "../service/egByteArray.h"
#include "../service/egFileType.h"
// #include "egIndexes.h"
template <typename KeyType> class EgIndexes;
// Finger in the file: KeyMin KeyMax nextChunkOffset isLeaf(root only)
// Chunk in the file:  FingersArray parentFingerOffset chunkIsLeaf fingersCount 
template <typename KeyType> class EgFingers { public: // fingers tree (N-M type) part of indexes-fingers complex
    const egMaxStreamSizeType nextChunkOffsetPosition  = sizeof(KeyType) * 2; //  + sizeof(keysCountType); 
    const egMaxStreamSizeType oneFingerSize        = nextChunkOffsetPosition + sizeof(uint64_t);  // last is next chunk offset
    const egMaxStreamSizeType rootHeaderSize       = oneFingerSize + sizeof(uint8_t); // bool isLeaf as uint8_t
    const egMaxStreamSizeType parentChunkOffsetPosition = egIndexesSpace::egChunkCapacity * oneFingerSize;
    const egMaxStreamSizeType chunkIsLeafPosition  = parentChunkOffsetPosition + sizeof(uint64_t);
    const egMaxStreamSizeType chunkCountPosition   = chunkIsLeafPosition  + sizeof(uint8_t);
    const egMaxStreamSizeType fingersChunkSize     = chunkCountPosition  + + sizeof(keysCountType); // bool isLeaf as uint8_t
    const bool  isRootFinger  = true;  // mnemonic
    const bool  notRootFinger = false; // mnemonic
    bool        rootFingerIsLoaded {false};
    bool        anyMinMaxChanged;
    std::string fingersFileName;
    egFinger<KeyType> rootFinger;
    egFinger<KeyType> parentFinger;
    egFinger<KeyType> currentFinger;
    egFinger<KeyType> newFinger;
    egFinger<KeyType> lastFinger;
    EgIndexes<KeyType>*               indexChunks {nullptr}; // ptr to related indexes object, set by upper IndexesFiles interface class
    std::vector < egFinger<KeyType> > fingersChain;          // store fingers path for update
    EgFileType        fingersFileStream;     // file operations
    egDataStream*     localStream {nullptr}; // chunk buffer operations
    EgFingers(std::string a_fingersName, EgIndexes<KeyType>* indexChunksPtr);
    ~EgFingers() { delete localStream; }
    inline void InitFinger(egFinger<KeyType>& theFinger);
        // top API
    bool AddNewRootFinger(KeyType& Key, uint64_t indexesChunkOffset);
    bool FindIndexesChunkToInsert(KeyType& Key); // sets currentFinger and fingersChain
    bool UpdateFingersChainUp();        // uses currentFinger and fingersChain
    bool AddNewUpdateCurrentFinger();   // uses current and new fingers and fingersChain
    bool DeleteCurrentFingerByChain();  // uses currentFinger and fingersChain
    bool UpdateFingersByBackptrs();     // uses currentFinger (another index chunk, can't use chain)
    bool DeleteFingerByBackptrs();      // uses currentFinger (another index chunk, can't use chain)
        // files
    bool CheckIfFingerFileExists(){ return fingersFileStream.checkIfExists(); }
    bool DeleteFingersFile();
        // file ops
    inline bool LoadRootFingerFromFile (egFinger<KeyType>& theFinger, bool loadRootIsLeaf);
    inline bool StoreFingerToFile  (egFinger<KeyType>& theFinger);
    inline bool LoadFingersChunk  (uint64_t& chunkOffset); // to localStream-> bufData
    inline bool StoreFingersChunk (uint64_t fingersChunkOffset); // from localStream-> bufData
    inline bool StoreParentChunkOffsetDirect(uint64_t fingersChunkOffset, uint64_t parentChunkOffset);
    void SwapFingers(); // swaps current and new fingers for finger tests (if new key < current key)
        // chunk operations
    inline void ClearFingersChunk ();
    inline void ReadFingerFromChunk (egFinger<KeyType>& theFinger, const int fingerPositionNum);
    inline void WriteFingerToChunk (egFinger<KeyType>& theFinger); // QDataStream &localFingersStream,
    inline void ReadChunkMinMaxToParentFinger (egFinger<KeyType>& theParentFinger);
    inline bool GetParentFingerByOffset(uint64_t fingersChunkOffset, uint64_t nextOffset); // uses current finger
        // finger ops
    inline bool FingerIsRoot(egFinger<KeyType>& theFinger);
    inline void GetCountFromChunk(keysCountType& refCount);
    inline void WriteCountToChunk(keysCountType theCount);
    inline void GetCountDirect(keysCountType& refCount, uint64_t& chunkOffset);
    inline void GetChunkIsLeaf(uint8_t& isLeaf);
    inline void UpdateChunkIsLeaf(const uint8_t chunkIsLeaf);
    inline void GetChunkParentOffset(uint64_t& parentChunkOffset);
    inline void UpdateChunkParentOffset(uint64_t parentChunkOffset);
        // insert ops
    inline bool AppendNewFingersChunk(egFinger<KeyType>& theFinger);
    inline bool AddNewTopLevelChunk(); // ex AddNewSubRootChunk(); FIXME level number used
    inline void MoveTailToInsert(egMaxStreamSizeType fingerPositionBytes, egMaxStreamSizeType bytesToMove);
    inline void SaveLastFingerOfChunk();
    inline bool InsertNewFingerToChunk(); // bool replaceLast InsertSplittedFinger();
    // int SplitFingersChunk(); // TODO
        // update ops
    inline bool UpdateMinMaxByFlags(egFinger<KeyType>& theFinger);
        // delete ops
    inline bool DeleteFingerFromChunk(); // uses current and chunk
    inline bool DeleteFingersChunk(uint64_t fingersChunkOffset);
    inline bool DeleteTopChunk();
        // lookups
    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    // static bool CompareEQ (KeyType& currentValue, KeyType& key) {return (currentValue == key);}
    static bool CompareGT (KeyType& currentValue, KeyType& key) {return (currentValue > key);}
    static bool CompareGE (KeyType& currentValue, KeyType& key) {return (currentValue >= key);}
    static bool CompareLT (KeyType& currentValue, KeyType& key) {return (currentValue < key);}
    static bool CompareLE (KeyType& currentValue, KeyType& key) {return (currentValue <= key);}
    inline bool OpenFileStream();
    bool FindIndexChunkEQ(KeyType& Key);    // first finger for key greater or equal then finger's min value
    inline void FindFirstFinger(KeyType& Key, CompareFunctionType predicate);
    inline void FindLastFinger(KeyType& Key, CompareFunctionType predicate);
    bool  FindIndexChunkLess(KeyType& Key, CompareFunctionType predicate);
    bool FindIndexChunkLT(KeyType& Key) { return FindIndexChunkLess(Key, CompareGE); }
    bool FindIndexChunkLE(KeyType& Key) { return FindIndexChunkLess(Key, CompareGT); }
    bool  FindIndexChunkGreater(KeyType& Key, CompareFunctionType predicate);
    bool FindIndexChunkGE(KeyType& Key) { return FindIndexChunkGreater(Key, CompareLT); }
    bool FindIndexChunkGT(KeyType& Key) { return FindIndexChunkGreater(Key, CompareLE); }
        // debug
    void PrintFingerInfo(egFinger<KeyType>& fingerInfo, const std::string theMessage);
};