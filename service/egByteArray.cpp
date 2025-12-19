#include <iostream>
#include "egByteArray.h"

void EgByteArraySlicerType::reallocDataArray(uint64_t newSize) {
    // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
    if (dataSize == newSize) // FIXME TODO data wipe option
        return;
    dataSize = newSize;
    if (brickID)
        theHamSlicer-> freeSlice(brickID);
    if (dataSize) 
        theHamSlicer-> getSlice(dataSize, brickID, arrayData);
    else {
        brickID = 0;
        arrayData = nullptr;
    }
}

void EgByteArraySysallocType::reallocDataArray(uint64_t newSize) {
    // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
    if (dataSize == newSize) // FIXME TODO data wipe option
        return;
    dataSize = newSize;
    delete arrayData;
    if (dataSize)
        arrayData = new ByteType[dataSize];
    else
        arrayData = nullptr;
}

EgByteArrayAbstractType& EgByteArrayAbstractType::operator=(const EgByteArrayAbstractType &rightBA) {
    dataSize = rightBA.dataSize;
    reallocDataArray(dataSize);
    memcpy((void *)arrayData, (void *)rightBA.arrayData, dataSize);
    return *this;
}

void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray) {
    // bool resizeFlag = byteArray.dataSize < newSize; // FIXME add capacity size
    int64_t newSize = strlen(str) + 1;
    if (newSize > 1) {
        byteArray.reallocDataArray(newSize);
        memcpy((void *)byteArray.arrayData, (void *)str, newSize);
    } else {
        byteArray.reallocDataArray(0);
    }
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str) {
        // str = reinterpret_cast<char*> (byteArray.arrayData);
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        memcpy((void*)str, (void*) byteArray.arrayData, byteArray.dataSize);
        return byteArray;
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, std::string& str) {
        str.assign((char*) byteArray.arrayData, byteArray.dataSize-1);
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str) {
        int64_t newSize = strlen(str)+1;
        if (newSize > 1) {
            byteArray.reallocDataArray(newSize);
            memcpy((void *)byteArray.arrayData, (void *)str, newSize);
        } else {
            byteArray.reallocDataArray(0);
        }
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, std::string& str) {
        int64_t newSize = str.size()+1;
        if (newSize > 1) {
            byteArray.reallocDataArray(newSize);
            memcpy((void *)byteArray.arrayData, (void *)str.c_str(), newSize);
        } else {
            byteArray.reallocDataArray(0);
        }
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
    }
    std::cout << std::endl;
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