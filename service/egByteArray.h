#pragma once
#include "egHamSlicer.h"

const EgStaticLengthType    egMask7f    {0x7f}; // get 7 bits to next byte
const EgByteType            egMask80    {0x80}; // not last byte flag

// ========= Byte Array Length Convertors  ===============
const int DATA_CONVERT_MAX_BYTES_COUNT      {10};   // 64 bits to up to 10 bytes 

class EgByteArrayAbstractType {
public:
    uint64_t  dataSize          {0};
    // uint64_t  dataChunkCapacity {0}; // FIXME TODO add capacity logic
    EgByteType* dataChunk         {nullptr};

    EgByteArrayAbstractType () {}
    EgByteArrayAbstractType (uint64_t init_size) 
        : dataSize(init_size)
        // , dataChunkCapacity(init_size)
        {}
    EgByteArrayAbstractType (EgByteType* init_data, uint64_t init_size)
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
            // ========= Byte Array Length Convertors  ===============
    uint8_t egConvertStaticToFlex(EgStaticLengthType staticVal, EgByteType* flexibleVal);  // convert fixed length of byte array to variable
    uint8_t egConvertFlexToStatic(EgByteType* flexibleVal, EgStaticLengthType& staticVal); // convert variable length to fixed
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
        { if(init_size) dataChunk = new EgByteType[init_size]; }
    virtual ~EgByteArraySysallocType() { /*std::cout << "destr. of "; PrintByteArray(*this);*/
        if(dataSize) delete dataChunk;
    }

    void reallocDataChunk(uint64_t newSize) override;
};

// void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray);

template <typename T> void CopyBAFromFixedType(T&& value, EgByteArrayAbstractType& byteArray) { // , bool forceWipe = false
    // std::cout << "ByteArrayFromFixedType() value: " << value << std::endl;
    byteArray.reallocDataChunk(sizeof(value)); // sets byteArray.dataSize
    memcpy((void*)byteArray.dataChunk, (void*) &value, byteArray.dataSize);
}

template <typename T> void CopyBAFromVarType(T& value, EgByteArrayAbstractType& byteArray, uint64_t theSize) { // , bool forceWipe = false
    // std::cout << "ByteArrayFromFixedType() value: " << value << std::endl;
    byteArray.reallocDataChunk(theSize); // sets byteArray.dataSize
    memcpy((void*)byteArray.dataChunk, (void*) &value, byteArray.dataSize);
}

void CopyBAFromStr(std::string& str, EgByteArrayAbstractType& byteArray);

// EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str);
// EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, std::string& str);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const std::string& str);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, std::string& str);

// ======================== Debug ========================
void PrintByteArray(EgByteArrayAbstractType& bArray, bool isStr = true);