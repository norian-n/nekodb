#pragma once
#include "egHamSlicer.h"

template <typename T> class EgPtrArrayType {
public:
    uint64_t  ptrsCount           {0};
    // uint64_t  arrayCapacity    {0}; // FIXME TODO add capacity logic
    T* ptrsArray     {nullptr};
    EgHamSlicerType* theHamSlicer {nullptr};
    EgHamBrickIDType brickID      {0};

    EgPtrArrayType () = delete;
    EgPtrArrayType (EgHamSlicerType& a_HamSlicer, uint64_t init_size):
        ptrsCount(init_size),
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

/*    EgPtrArrayType (EgHamSlicerType* a_HamSlicer, uint64_t init_size):
        ptrsCount(init_size),
        theHamSlicer(a_HamSlicer)
    { init(); } */

// EgPtrArrayType& operator = (const EgPtrArrayType& rightBA);
// std::cout << "EgPtrArrayType() brickID: " << std::dec << brickID << " ptr: "<< std::hex << (int64_t) ptrsArray << std::endl;

/* template <typename T> void PrintPtrsArray(EgPtrArrayType<T>& bArray) {
    std::cout << "PrintPtrArray ptrs count: " << std::dec << bArray.ptrsCount << " ";
    if (bArray.ptrsCount) {
        for (int i = 0; i < bArray.ptrsCount; i++)
            // std::cout << " \"" << std::hex << (int) field.arrayData[i] << "\"";
            std::cout << std::dec << i << ": "<< std::hex << (int64_t) bArray.ptrsArray[i] << ", ";
        std::cout << std::endl;
    }
} */