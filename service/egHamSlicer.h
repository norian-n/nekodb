#pragma once
#include "../metainfo/egCoreTypes.h"

typedef uint32_t EgHamBrickIDType;

struct EgHamBrickType {
    EgHamBrickIDType brickID;
    EgByteType*      dataPtr;
    uint64_t         freeSize;
    uint64_t         usedSlicesCount; // if 0 del or recycle
};

class EgHamSlicerType {
public:
    EgHamBrickIDType    nextID       {1};        // ham bricks counter
    EgHamBrickType*     newBrickPtr  {nullptr};
    // EgHamBrickType      newHamBrick;
    uint64_t            hamBrickSize {egDefaultHamBrickSize};

    std::unordered_map <EgHamBrickIDType, EgHamBrickType*>  hamBricks;   // all ham bricks FIXME TODO use ptrs, dont copy
    std::multimap <uint64_t, EgHamBrickType*>    hamBricksByFree;       // free slices of bricks lookup

    EgHamSlicerType () {}
    ~EgHamSlicerType();

    bool initBrick(uint64_t sliceSize);
    bool getSlice(uint64_t sliceSize, EgHamBrickIDType& brickID, EgByteType*& slicePtr);
    void freeSlice(EgHamBrickIDType brickID);
};

void PrintHamSlices(EgHamSlicerType& theSlicer);