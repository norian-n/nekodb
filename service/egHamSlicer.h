#pragma once
#include "../metainfo/egCoreTypes.h"

typedef uint32_t EgHamBrickIDType;

struct EgHamBrickType {
    EgHamBrickIDType brickID;
    ByteType* brickPtr;
    uint64_t freeSize;
    uint64_t usedSlicesCount;
};

class EgHamSlicerType {
public:
    EgHamBrickIDType    nextID      {1};        // ham bricks counter
    EgHamBrickType*     newBrickPtr {nullptr};
    EgHamBrickType      newHamBrick;
    uint64_t            hamBrickSize {egDefaultHamBrickSize};
    // uint64_t  dataSize      {0};

    std::unordered_map <EgHamBrickIDType, EgHamBrickType>  hamBricks;   // all ham bricks
    std::multimap <uint64_t, EgHamBrickType*>    hamBricksByFree;       // free slices of bricks lookup

    EgHamSlicerType () { initBrick(0); }
    ~EgHamSlicerType() { hamBricksByFree.clear(); hamBricks.clear(); }

    bool initBrick(uint64_t sliceSize) { // get new ham brick
        newBrickPtr = nullptr;
        if (sliceSize > hamBrickSize)
            return false;
        newHamBrick.brickPtr = new ByteType[hamBrickSize];
        newHamBrick.brickID  = nextID++;
        newHamBrick.freeSize = hamBrickSize - sliceSize;
        newHamBrick.usedSlicesCount = sliceSize > 0 ? 1 : 0;
        if (newHamBrick.brickPtr) {
            auto bricsIter = hamBricks.insert(std::make_pair(newHamBrick.brickID, newHamBrick)); // copy to map
            newBrickPtr = &(bricsIter.first-> second);
            hamBricksByFree.insert(std::make_pair(newHamBrick.freeSize, newBrickPtr));
        }
        // std::cout << "EgHamSlicerType initBrick() exit" << std::endl;
        return (newHamBrick.brickPtr);
    }
    bool getSlice(uint64_t sliceSize, EgHamBrickIDType& brickID, ByteType*& slicePtr) {
        bool found {false};
            // search by size
        for (auto [first, second] : hamBricksByFree) // 11 auto bricsIter :, <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            if (first >= sliceSize) { // get existing ham brick
                brickID = second-> brickID;
                slicePtr = second-> brickPtr + hamBrickSize - second->freeSize;
                second-> freeSize -= sliceSize;
                second-> usedSlicesCount++;
                auto nodeHandler = hamBricksByFree.extract(first); // update key of map with magic 17 code
                nodeHandler.key() = second->freeSize;
                hamBricksByFree.insert(std::move(nodeHandler));
                found = true;
                break;
            }
        if (!found) { // get new ham brick
            if (initBrick(sliceSize)) {
                brickID  = newHamBrick.brickID;
                slicePtr = newHamBrick.brickPtr;
            } else
                return false;
        }
        // std::cout << "EgHamSlicerType getSlice() ok " << brickID << " " <<  sliceSize << std::endl;
        return true; 
    }
    void freeSlice(EgHamBrickIDType brickID) {  // check if ham brick totally consumed TODO recycle brick
        auto iter = hamBricks.find(brickID); // search hamBricks by ID
        if (iter != hamBricks.end())
            iter-> second.usedSlicesCount--;
        if (!iter-> second.usedSlicesCount) { // all slices released, search hamBricksByFree by its freeSize range to remove
            for (auto [sizeIter, rangeEnd] = hamBricksByFree.equal_range(iter->second.freeSize); sizeIter != rangeEnd; ++sizeIter)
                if (sizeIter->second-> brickID == brickID) {
                    hamBricksByFree.erase(sizeIter);
                    break;
                }
            delete iter-> second.brickPtr;
            hamBricks.erase(iter);
        }
    }
};

void PrintHamSlices(EgHamSlicerType theSlicer);