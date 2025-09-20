#pragma once
#include "../core/egCoreTypes.h"
#include "egHamSlicer.h"

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

class EgByteArraySlicerType : public EgByteArrayAbstractType { // egBA with ham slicer mem allocator
public:
    EgHamSlicerType* theHamSlicer {nullptr};
    EgHamBrickIDType brickID      {0};

    EgByteArraySlicerType () = delete;
    EgByteArraySlicerType (EgHamSlicerType* a_HamSlicer, uint64_t init_size = 0): EgByteArrayAbstractType(init_size)
        , theHamSlicer(a_HamSlicer)
        // allocMode(egHamSliceAlloc),
        // dataSize(init_size)
    { if(init_size) theHamSlicer-> getSlice(dataSize, brickID, arrayData); }
    EgByteArraySlicerType (EgHamSlicerType& a_HamSlicer, uint64_t init_size = 0): EgByteArrayAbstractType(init_size)
        , theHamSlicer(&a_HamSlicer)
    { if(init_size) theHamSlicer-> getSlice(dataSize, brickID, arrayData); }
    virtual ~EgByteArraySlicerType() {
        if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID);
    }

    EgByteArraySlicerType& operator = (const EgByteArraySlicerType& rightBA);

    void reallocDataArray() override;
};

class EgByteArraySysallocType : public EgByteArrayAbstractType { // egBA with system mem allocator
public:
    EgByteArraySysallocType () {} 
    EgByteArraySysallocType (uint64_t init_size): EgByteArrayAbstractType(init_size)
        { if(init_size) arrayData = new ByteType[init_size]; }
    virtual ~EgByteArraySysallocType() { /*std::cout << "destr. of "; PrintByteArray(*this);*/
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

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum);

void PrintByteArray(EgByteArrayAbstractType& bArray, bool isStr = true);