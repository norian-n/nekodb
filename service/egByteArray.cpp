#include <iostream>
#include "egByteArray.h"

void EgByteArraySlicerType::reallocDataArray() {
    // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
    if (brickID)
        theHamSlicer-> freeSlice(brickID);
    if (dataSize) 
        theHamSlicer-> getSlice(dataSize, brickID, arrayData);
    else {
        brickID = 0;
        arrayData = nullptr;
    }
}

EgByteArraySlicerType& EgByteArraySlicerType::operator = (const EgByteArraySlicerType& rightBA) {
    // bool resizeFlag = (dataSize < rightBA.dataSize) || (! brickID); // FIXME add capacity size
    bool resizeFlag = (dataSize != rightBA.dataSize);
    dataSize = rightBA.dataSize;
    if (resizeFlag)
        reallocDataArray();
    memcpy((void*)arrayData, (void*) rightBA.arrayData, dataSize);
    return *this;
}

void EgByteArraySysallocType::reallocDataArray() {
    // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
    delete arrayData;
    if (dataSize)
        arrayData = new ByteType[dataSize];
    else
        arrayData = nullptr;
}

EgByteArraySysallocType& EgByteArraySysallocType::operator = (const EgByteArraySysallocType& rightBA) {
    // bool resizeFlag = (dataSize < rightBA.dataSize) || (! brickID); // FIXME add capacity size
    bool resizeFlag = (dataSize != rightBA.dataSize);
    dataSize = rightBA.dataSize;
    if (resizeFlag)
        reallocDataArray();
    memcpy((void*)arrayData, (void*) rightBA.arrayData, dataSize);
    return *this;
}

void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray) {
    // bool resizeFlag = byteArray.dataSize < newSize; // FIXME add capacity size
    int64_t newSize = strlen(str) + 1;
    if (newSize > 1) {
        byteArray.dataSize = newSize;
        byteArray.reallocDataArray();
        memcpy((void *)byteArray.arrayData, (void *)str, newSize);
    } else {
        byteArray.dataSize = 0;
        byteArray.reallocDataArray();
    }
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str) {
        // str = reinterpret_cast<char*> (byteArray.arrayData);
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        memcpy((void*)str, (void*) byteArray.arrayData, byteArray.dataSize);
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str) {
        // *(reinterpret_cast<int*> (byteArray.arrayData)) = intNum;
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        int64_t newSize = strlen(str)+1;
        if (newSize > 1) {
            byteArray.dataSize = newSize;
            byteArray.reallocDataArray();
            memcpy((void *)byteArray.arrayData, (void *)str, newSize);
        } else {
            byteArray.dataSize = 0;
            byteArray.reallocDataArray();
        }
        return byteArray;
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum) {
        intNum = *(reinterpret_cast<int*> (byteArray.arrayData));
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum) {
        byteArray.dataSize = sizeof(int);
        byteArray.reallocDataArray();    
        *(reinterpret_cast<int*> (byteArray.arrayData)) = intNum;
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        return byteArray;
}

void PrintByteArray(EgByteArrayAbstractType& bArray, bool isStr) {
    std::cout << " size: " << std::dec << bArray.dataSize;
    if (bArray.dataSize) {
        // if ((bArray.dataSize > 1) && !bArray.arrayData[bArray.dataSize - 1] && bArray.arrayData[0] > 0x29)
        if (isStr)
            std::cout << " Str: \"" << (char *)bArray.arrayData << "\"";
        else
            std::cout << " Int: " << (int)*(bArray.arrayData);
        std::cout << " Hex: " << std::hex;
        for (int i = 0; i < bArray.dataSize; i++)
            // std::cout << " \"" << std::hex << (int) field.arrayData[i] << "\"";
            std::cout << (int)bArray.arrayData[i] << " ";
        std::cout << std::endl;
    }
}

void PrintHamSlices(EgHamSlicerType theSlicer) {
    std::cout << "PrintHamSlices hamBricks: " << theSlicer.hamBricks.size() << std::endl;
    for (auto bricsIter : theSlicer.hamBricks) {
        std::cout << std::dec << bricsIter.first
                  << " ID: "                   << bricsIter.second.brickID
                  << " , freeSize: "             << bricsIter.second.freeSize
                  << " , usedSlicesCount: "      << bricsIter.second.usedSlicesCount
                  << " , brickPtr: " << std::hex << (uint64_t) bricsIter.second.brickPtr << std::endl;
    }
    std::cout << "PrintHamSlices hamBricksByFree: " << theSlicer.hamBricksByFree.size() << std::endl;
    for (auto bricsFreeIter : theSlicer.hamBricksByFree) {
        std::cout << std::dec << "freeSize: " << bricsFreeIter.first
                  << " , ID: " << bricsFreeIter.second-> brickID << std::endl;
    }
}