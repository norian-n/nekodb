#include "egHamSlicer.h"

EgHamSlicerType::~EgHamSlicerType() {
    for (auto [freeSize, brickPtr] : hamBricksByFree)
        delete brickPtr;
    hamBricksByFree.clear(); 
    hamBricks.clear(); 
}

bool EgHamSlicerType::initBrick(uint64_t sliceSize) { // create ham brick MEM_NEW --> freeSlice()
    newBrickPtr = new EgHamBrickType;
    uint64_t allocSize = std::max<uint64_t> (hamBrickSize, sliceSize);
    newBrickPtr-> dataPtr = new EgByteType[allocSize];
    newBrickPtr-> brickID  = nextID++;
    newBrickPtr-> freeSize = hamBrickSize - sliceSize;
    newBrickPtr-> usedSlicesCount = sliceSize > 0 ? 1 : 0;
    if (newBrickPtr-> dataPtr) {
        hamBricks.insert(std::make_pair(newBrickPtr-> brickID, newBrickPtr)); // copy to map FIXME dont copy
        hamBricksByFree.insert(std::make_pair(newBrickPtr-> freeSize, newBrickPtr));
    } else
        delete newBrickPtr; // cant alloc
    return (newBrickPtr-> dataPtr);
}

bool EgHamSlicerType::getSlice(uint64_t sliceSize, EgHamBrickIDType &brickID, EgByteType *&slicePtr) {
    bool freeSpaceFound{false};
    for (auto [freeSize, brickPtr] : hamBricksByFree) // search bricks by free size
        if (freeSize >= sliceSize) { // get existing ham brick
            brickID  = brickPtr-> brickID;
            slicePtr = brickPtr-> dataPtr + hamBrickSize - brickPtr-> freeSize;
            brickPtr-> freeSize -= sliceSize;
            brickPtr-> usedSlicesCount++;
            auto nodeHandler = hamBricksByFree.extract(freeSize); // update key of map with magic 17 code
            nodeHandler.key() = brickPtr->freeSize;
            hamBricksByFree.insert(std::move(nodeHandler));
            freeSpaceFound = true;
            break;
        }
    if ( ! freeSpaceFound) // no free space found, create new ham brick
        if (initBrick(sliceSize)) {
            brickID  = newBrickPtr-> brickID;
            slicePtr = newBrickPtr-> dataPtr;
        }
        else
            return false;
    // std::cout << "EgHamSlicerType getSlice() ok " << brickID << " " <<  sliceSize << std::endl;
    return true;
}

void EgHamSlicerType::freeSlice(EgHamBrickIDType brickID) {
    auto iter = hamBricks.find(brickID); // search hamBricks by ID
    if (iter != hamBricks.end()) {
        iter->second-> usedSlicesCount--;
        if ( ! iter->second-> usedSlicesCount ) { // all slices released, search hamBricksByFree by its freeSize range to remove
            for (auto [sizeIter, rangeEnd] = hamBricksByFree.equal_range(iter->second-> freeSize); sizeIter != rangeEnd; ++sizeIter)
                if (sizeIter->second->brickID == brickID) {
                    hamBricksByFree.erase(sizeIter);
                    break;
                }
            delete iter->second-> dataPtr; // free mem if ham brick totally consumed, TODO FIXME recycle brick
            delete iter->second;
            hamBricks.erase(iter);
        }
    }
}

void PrintHamSlices(EgHamSlicerType& theSlicer) {
    std::cout << "PrintHamSlices hamBricks: " << theSlicer.hamBricks.size() << std::endl;
    for (auto bricsIter : theSlicer.hamBricks) {
        std::cout << std::dec << bricsIter.first
                  << " ID: "                   << bricsIter.second-> brickID
                  << " , freeSize: "             << bricsIter.second-> freeSize
                  << " , usedSlicesCount: "      << bricsIter.second-> usedSlicesCount
                  << " , brickPtr: " << std::hex << (uint64_t) bricsIter.second-> dataPtr << std::endl;
    }
    std::cout << "PrintHamSlices hamBricksByFree: " << theSlicer.hamBricksByFree.size() << std::endl;
    for (auto bricsFreeIter : theSlicer.hamBricksByFree) {
        std::cout << std::dec << "freeSize: " << bricsFreeIter.first
                  << " , ID: " << bricsFreeIter.second-> brickID << std::endl;
    }
}