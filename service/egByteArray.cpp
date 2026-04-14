#include <iostream>
#include "egByteArray.h"

void EgByteArraySlicerType::

reallocDataChunk(uint64_t newSize) {
    // EG_LOG_STUB << "dataSize: " << DEC << (int) dataSize << " newSize: " << (int) newSize << " brickID: " << brickID << FN;
    if (dataSize == newSize) // FIXME TODO data wipe option
        return;
    if (dataSize < newSize) { // FIXME TODO data wipe option
        if (brickID)
            theHamSlicer->freeSlice(brickID);
        if (newSize)
            theHamSlicer->getSlice(newSize, brickID, dataChunk);
        else {
            brickID = 0;
            dataChunk = nullptr;
        }
    }
    dataSize = newSize;
}

void EgByteArraySysallocType::reallocDataChunk(uint64_t newSize) {
    if (dataSize == newSize) // FIXME TODO data wipe option
        return;
    if (dataSize < newSize) { // FIXME TODO data wipe option
        delete dataChunk;
        if (newSize)
            dataChunk = new EgByteType[newSize];
        else
            dataChunk = nullptr;
    }
    dataSize = newSize;
}

EgByteArrayAbstractType& EgByteArrayAbstractType::operator=(const EgByteArrayAbstractType &rightBA) {
    dataSize = rightBA.dataSize;
    reallocDataChunk(dataSize);
    memcpy((void *)dataChunk, (void *)rightBA.dataChunk, dataSize);
    return *this;
}

/* void ByteArrayFromCharStr(const char* str, EgByteArrayAbstractType& byteArray) {
    // bool resizeFlag = byteArray.dataSize < newSize; // FIXME add capacity size
    int64_t newSize = strlen(str) + 1;
    if (newSize > 1) {
        byteArray.reallocDataChunk(newSize);
        memcpy((void *)byteArray.dataChunk, (void *)str, newSize);
    } else {
        byteArray.reallocDataChunk(0);
    }
} */

/* EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, char* str) {
        // str = reinterpret_cast<char*> (byteArray.dataChunk);
        // std::cout << "byteArray to int: " << std::dec << intNum << std::endl;
        // PrintByteArray(byteArray);
        memcpy((void*)str, (void*) byteArray.dataChunk, byteArray.dataSize);
        return byteArray;
} */

/* EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const char* str) {
        int64_t newSize = strlen(str)+1;
        if (newSize > 1) {
            byteArray.reallocDataChunk(newSize);
            memcpy((void *)byteArray.dataChunk, (void *)str, newSize);
        } else {
            byteArray.reallocDataChunk(0);
        }
        return byteArray;
} */

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, std::string& str) {
        str.assign((char*) byteArray.dataChunk, byteArray.dataSize);
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, const std::string& str) {
        int64_t newSize = str.size();
        if (newSize > 0) {
            byteArray.reallocDataChunk(newSize);
            memcpy((void *)byteArray.dataChunk, (void *)str.c_str(), newSize);
        } else {
            byteArray.reallocDataChunk(0);
        }
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, std::string& str) {
        int64_t newSize = str.size();
        if (newSize > 0) {
            byteArray.reallocDataChunk(newSize);
            memcpy((void *)byteArray.dataChunk, (void *)str.c_str(), newSize);
        } else {
            byteArray.reallocDataChunk(0);
        }
        return byteArray;
}

// convert fixed length dataset size to variable length one to save file space 
uint8_t EgByteArrayAbstractType::egConvertStaticToFlex(EgStaticLengthType staticVal, EgByteType* flexibleVal)
{
    EgStaticLengthType    buf         {0};
    int                 byteCount   {1};
    while (staticVal && (byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1))) {
        buf = staticVal & egMask7f; // get 7 bits to next byte
        flexibleVal[byteCount-1] = static_cast<EgByteType>(buf);
        staticVal = staticVal >> 7; // shift static counter to get next 7 bits
        if (staticVal) {
            flexibleVal[byteCount-1] |=  egMask80; // not last byte
            byteCount++;
        } 
    }
    return byteCount;
}

uint8_t EgByteArrayAbstractType::egConvertFlexToStatic(EgByteType* flexibleVal, EgStaticLengthType& staticVal)
{
    staticVal = 0;
    EgStaticLengthType buf         {0};
    int                byteCount   {1};
    while ( byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1) ) {
        buf = static_cast<EgByteType> (flexibleVal[byteCount-1]) & egMask7f; // get 7 bits to next byte       
        staticVal = (buf << 7 * (byteCount-1)) | staticVal;
        // EG_LOG_STUB << "staticVal: " << DEC << staticVal << FN;
        if ( !(flexibleVal[byteCount-1] & egMask80) ) // check "continue" bit - last byte
            break;
        byteCount++;
    }
    return byteCount;
}

void PrintByteArray(EgByteArrayAbstractType& bArray, bool isStr) {
    std::cout << " size: " << std::dec << bArray.dataSize;
    if (bArray.dataSize) {
        // if ((bArray.dataSize > 1) && !bArray.dataChunk[bArray.dataSize - 1] && bArray.dataChunk[0] > 0x29)

        if (isStr) {
            std::string str;
            bArray >> str;
            std::cout << " Str: \"" << str << "\"";
        }
        else
            std::cout << " Int: " << (int)*(bArray.dataChunk);
        std::cout << " Hex: " << std::hex;
        for (int i = 0; i < bArray.dataSize; i++)
            // std::cout << " \"" << std::hex << (int) field.dataChunk[i] << "\"";
            std::cout << (int)bArray.dataChunk[i] << " ";
    }
    std::cout << std::endl;
}

void CopyBAFromStr(std::string& str, EgByteArrayAbstractType& byteArray) { // , bool forceWipe = false
    // std::cout << "ByteArrayFromFixedType() value: " << value << std::endl;
    byteArray.reallocDataChunk(str.size()); // sets byteArray.dataSize
    memcpy((void*)byteArray.dataChunk, (void*) str.c_str(), byteArray.dataSize);
}