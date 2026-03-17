#pragma once
#include "egHamSlicer.h"

template <typename T> class EgPtrArrayType { // dynamic array of pointers for data node content
public:
    uint64_t  ptrsCount           {0};       // pointers in the array
    // uint64_t  arrayCapacity    {0}; // FIXME TODO add capacity logic
    T* ptrsArray     {nullptr};              // primary content - array of pointers  
    EgHamSlicerType* theHamSlicer {nullptr}; // memory allocator
    EgHamBrickIDType brickID      {0};       // allocator block, 0 is unused

    EgPtrArrayType () = delete;
    EgPtrArrayType (EgHamSlicerType& a_HamSlicer, uint64_t initCount):
        ptrsCount(initCount),
        theHamSlicer(&a_HamSlicer)
    { init(); }
    ~EgPtrArrayType() { if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID); }

    inline void init() { if(ptrsCount) theHamSlicer-> getSlice(ptrsCount*sizeof(T), brickID, (ByteType*&) ptrsArray); }
    void clear() { if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID); ptrsCount = 0; brickID = 0; ptrsArray = nullptr; }

    void reallocPtrsArray() {
        if (brickID)
            theHamSlicer->freeSlice(brickID);
        if (ptrsCount)
            theHamSlicer->getSlice(ptrsCount * sizeof(T), brickID, (ByteType *&)ptrsArray);
        else {
            brickID = 0;
            ptrsArray = nullptr;
        }
    }
};

/* template <typename T> void PrintPtrsArray(EgPtrArrayType<T>& bArray) {
    std::cout << "PrintPtrArray ptrs count: " << std::dec << bArray.ptrsCount << " ";
    if (bArray.ptrsCount) {
        for (int i = 0; i < bArray.ptrsCount; i++)
            // std::cout << " \"" << std::hex << (int) field.arrayData[i] << "\"";
            std::cout << std::dec << i << ": "<< std::hex << (int64_t) bArray.ptrsArray[i] << ", ";
        std::cout << std::endl;
    }
} */