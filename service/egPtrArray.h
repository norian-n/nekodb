#pragma once
#include "../core/egCoreTypes.h"
#include "egHamSlicer.h"

/*
class EgByteArrayAbstractType {
public:
    uint64_t  dataSize      {0};
    uint64_t  arrayCapacity {0};
    ByteType* arrayData     {nullptr};

    EgByteArrayAbstractType () {}
    EgByteArrayAbstractType (uint64_t init_size): 
        dataSize(init_size),
        arrayCapacity(init_size) {}
    EgByteArrayAbstractType (ByteType* init_data, uint64_t init_size):
        dataSize(init_size),
        arrayCapacity(init_size),
        arrayData(init_data) {}
    virtual ~EgByteArrayAbstractType() {}

    virtual void reallocDataArray() { std::cout << "ERROR: reallocDataArray() of abstract class called" << std::endl; }
};
*/

template <typename T> class EgPtrArrayType { // : public EgByteArrayAbstractType { // egBA with ham slicer mem allocator
public:
    uint64_t  ptrsCount      {0};
    // uint64_t  arrayCapacity {0};
    T* ptrsArray     {nullptr};
    EgHamSlicerType* theHamSlicer {nullptr};
    EgHamBrickIDType brickID      {0};

    EgPtrArrayType () = delete;
    EgPtrArrayType (EgHamSlicerType* a_HamSlicer, uint64_t init_size):
        ptrsCount(init_size),
        theHamSlicer(a_HamSlicer)
    { init(); }
    EgPtrArrayType (EgHamSlicerType& a_HamSlicer, uint64_t init_size):
        ptrsCount(init_size),
        theHamSlicer(&a_HamSlicer)
    { init(); }

    virtual ~EgPtrArrayType() { if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID); }

    inline void init() { if(ptrsCount) theHamSlicer-> getSlice(ptrsCount*sizeof(T), brickID, (ByteType*&) ptrsArray); 
        // std::cout << "EgPtrArrayType() brickID: " << std::dec << brickID << " ptr: "<< std::hex << (int64_t) ptrsArray << std::endl;
    }

    void clear() { if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID); ptrsCount = 0; brickID = 0; ptrsArray = nullptr; }

    // EgPtrArrayType& operator = (const EgPtrArrayType& rightBA);

    void reallocPtrsArray() {
        // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
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

template <typename T> void PrintPtrsArray(EgPtrArrayType<T>& bArray) {
    std::cout << "PrintPtrArray ptrs count: " << std::dec << bArray.ptrsCount << " ";
    if (bArray.ptrsCount) {
        for (int i = 0; i < bArray.ptrsCount; i++)
            // std::cout << " \"" << std::hex << (int) field.arrayData[i] << "\"";
            std::cout << std::dec << i << ": "<< std::hex << (int64_t) bArray.ptrsArray[i] << ", ";
        std::cout << std::endl;
    }
}

// template <typename T>  void PrintPtrsArray(EgPtrArrayType<T>& bArray);

/*
class EgByteArraySysallocType : public EgByteArrayAbstractType { // egBA with system mem allocator
public:
    EgByteArraySysallocType () {} 
    EgByteArraySysallocType (uint64_t init_size): EgByteArrayAbstractType(init_size)
        { if(init_size) arrayData = new ByteType[init_size]; }
    virtual ~EgByteArraySysallocType() { // std::cout << "destr. of "; PrintByteArray(*this);
        if(dataSize) delete arrayData;
    }

    EgByteArraySysallocType& operator = (const EgByteArraySysallocType& rightBA);

    void reallocDataArray() override;
};

void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray);

template <typename T> void ByteArrayFromType(T&& value, EgByteArrayAbstractType& byteArray) {
    // std::cout << "ByteArrayFromType() value: " << value << std::endl;
    byteArray.dataSize  = sizeof(value);
    byteArray.reallocDataArray();
    memcpy((void*)byteArray.arrayData, (void*) &value, byteArray.dataSize);
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum);
*/