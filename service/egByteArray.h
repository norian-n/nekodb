#pragma once
#include "egHamSlicer.h"

class EgByteArrayAbstractType {
public:
    uint64_t  dataSize          {0};
    // uint64_t  dataChunkCapacity {0};
    ByteType* dataChunk         {nullptr};

    EgByteArrayAbstractType () {}
    EgByteArrayAbstractType (uint64_t init_size) 
        : dataSize(init_size)
        // , dataChunkCapacity(init_size)
        {}
    EgByteArrayAbstractType (ByteType* init_data, uint64_t init_size)
        : dataSize(init_size)
        // , dataChunkCapacity(init_size)
        , dataChunk(init_data) {}
    virtual ~EgByteArrayAbstractType() {}

    virtual void reallocDataChunk(uint64_t newSize) { std::cout << "ERROR: reallocDataChunk() of abstract class called" << std::endl; }

    EgByteArrayAbstractType& operator = (const EgByteArrayAbstractType& rightBA);

    template<typename T> EgByteArrayAbstractType& operator >> (T& value) {
        value = *(reinterpret_cast<T*> (this-> dataChunk));
        return *this; }

    template<typename T> EgByteArrayAbstractType& operator << (const T& value) {
        reallocDataChunk(sizeof(T));    
        *(reinterpret_cast<T*> (dataChunk)) = value;
        return *this; }        
};

class EgByteArraySlicerType : public EgByteArrayAbstractType { // egBA with ham slicer mem allocator
public:
    EgHamSlicerType* theHamSlicer {nullptr};
    EgHamBrickIDType brickID      {0};

    EgByteArraySlicerType () = delete;
    EgByteArraySlicerType (EgHamSlicerType* a_HamSlicer, uint64_t init_size = 0): EgByteArrayAbstractType(init_size)
        , theHamSlicer(a_HamSlicer)
    { if(init_size) theHamSlicer-> getSlice(dataSize, brickID, dataChunk); }

    EgByteArraySlicerType (EgHamSlicerType& a_HamSlicer, uint64_t init_size = 0): EgByteArrayAbstractType(init_size)
        , theHamSlicer(&a_HamSlicer)
    { if(init_size) theHamSlicer-> getSlice(dataSize, brickID, dataChunk); }

    EgByteArraySlicerType (EgByteArraySlicerType& copySliceBA) // copy constructor
    { dataSize = copySliceBA.dataSize; if(dataSize) { theHamSlicer-> getSlice(dataSize, brickID, dataChunk); 
      memcpy((void*)dataChunk, (void*) copySliceBA.dataChunk, dataSize);} }

    virtual ~EgByteArraySlicerType() {
        if (theHamSlicer && brickID) theHamSlicer-> freeSlice(brickID);
    }

    void reallocDataChunk(uint64_t newSize) override;
};

class EgByteArraySysallocType : public EgByteArrayAbstractType { // egBA with system mem allocator
public:
    EgByteArraySysallocType () {} 
    EgByteArraySysallocType (uint64_t init_size): EgByteArrayAbstractType(init_size)
        { if(init_size) dataChunk = new ByteType[init_size]; }
    virtual ~EgByteArraySysallocType() { /*std::cout << "destr. of "; PrintByteArray(*this);*/
        if(dataSize) delete dataChunk;
    }

    void reallocDataChunk(uint64_t newSize) override;
};

void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray);

template <typename T> void ByteArrayFromType(T&& value, EgByteArrayAbstractType& byteArray) {
    // std::cout << "ByteArrayFromType() value: " << value << std::endl;
    byteArray.reallocDataChunk(sizeof(value));
    memcpy((void*)byteArray.dataChunk, (void*) &value, byteArray.dataSize);
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, std::string& str);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const std::string& str);

// ======================== Debug ========================
void PrintByteArray(EgByteArrayAbstractType& bArray, bool isStr = true);