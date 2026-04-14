#include <iostream>
#include "egStrArray.h"
#include "../metainfo/egLiterals.h"

EgStrArray::EgStrArray(EgMultArrayCountType count):
    strCount(count) {

        // EG_LOG_STUB << "EgDataNodeType()  fieldsCount: " << blueprintName << " " << std::dec << (int)  fieldsCount << FN;
        byteArrayPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (theHamSlicer,  strCount);
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
}

void EgStrArray::init() {
    for (int i = 0; i < strCount; i++) {
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType( theHamSlicer, 0); // FIXME STUB check size
        byteArrayPtrs-> ptrsArray[i] = byteArray;
    }
}

void EgStrArray::clear() {
    for (int i=0; i < strCount; i++) {
        delete byteArrayPtrs-> ptrsArray[i]; // del byteArrays
        byteArrayPtrs-> ptrsArray[i] = nullptr;
    }
}

EgByteArrayAbstractType& EgStrArray::operator[](int strIndex) { // value by index
    return  *(byteArrayPtrs->ptrsArray[strIndex]);
}

void EgStrArray::storeToByteArray(EgByteArrayAbstractType& ba) {
    // FIXME process empty multarray
    uint8_t lenSize {0};
    uint64_t totalSize {0};
    EgByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArrayAbstractType* field = byteArrayPtrs-> ptrsArray[0];

    for (int i=0; i < strCount; i++) { // total data length (with dynamic size fields)
        if (field->dataSize) // not empty field
            lenSize = field-> egConvertStaticToFlex(field->dataSize, lengthRawData);
        else
            lenSize = 1; 
        totalSize += (byteArrayPtrs->ptrsArray[i]-> dataSize + lenSize);
    }

    // lenSize = field-> egConvertStaticToFlex(totalSize, lengthRawData);
    ba.reallocDataChunk(totalSize); // + lenSize);

    // memcpy((void*) ba.dataChunk, (void*) lengthRawData, lenSize);
    // EG_LOG_STUB << "totalSize: " << DEC << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << FN;
    // PrintByteArray(ba, false);

    uint64_t insertIndex {0};
    for (int i =0; i < strCount; i++) {
        field = byteArrayPtrs-> ptrsArray[i];
        if (!field) {
            EG_LOG_STUB  << "DEBUG: field ptr is NULL " << FN;
            return;
        } else {
            if (field-> dataSize) { // not empty field
                lenSize = field-> egConvertStaticToFlex(field-> dataSize, lengthRawData);
                // EG_LOG_STUB << "insertIndex: " << DEC << insertIndex << " lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << FN;
                memcpy((void*) (ba.dataChunk + insertIndex), (void*) lengthRawData, lenSize);
                memcpy((void*) (ba.dataChunk + insertIndex + lenSize), (void*) field-> dataChunk, field-> dataSize);
                insertIndex = insertIndex + lenSize + field-> dataSize;
            } else {
                // EG_LOG_STUB << "DEBUG: zero field size" << FN;
                *(ba.dataChunk + insertIndex) = (EgByteType) 0;
                insertIndex++;
            }
        }
    }
}

void EgStrArray::loadFromByteArray(EgByteArrayAbstractType& ba) {
    EgByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArrayAbstractType tmpField (0); // convert length stub
    EgByteArraySlicerType*  newField;
    uint64_t readIndex        {0};
    uint64_t dataFieldSizeTmp {0};
    uint8_t  lenSize          {0};

    for (int i =0; i < strCount; i++) {
        memcpy((void*) lengthRawData, (void*) (ba.dataChunk + readIndex), std::min<uint64_t> (DATA_CONVERT_MAX_BYTES_COUNT, ba.dataSize));
        // EG_LOG_STUB << "lengthRawData[0]: " << HEX << (int) lengthRawData [0] << FN;
        lenSize = tmpField.egConvertFlexToStatic(lengthRawData, dataFieldSizeTmp);
        newField = new EgByteArraySlicerType(theHamSlicer, dataFieldSizeTmp);
        // EG_LOG_STUB << "newField lenSize: " << DEC << (int) lenSize << " dataFieldSizeTmp: " << dataFieldSizeTmp  << " dataSize: " << newField-> dataSize << FN;
        if (dataFieldSizeTmp) // not empty field
            memcpy((void*) newField-> dataChunk, (void*) (ba.dataChunk + readIndex + lenSize), dataFieldSizeTmp);
        byteArrayPtrs-> ptrsArray[i] = newField;
        readIndex += newField-> dataSize + lenSize;
    }
}

void EgStrArray::insertStr(const std::string& str, EgMultArrayCountType index) {
    if (index < strCount) {
        // EG_LOG_STUB << "AddNextDataFieldFromCharStr() in: " << str << FN;
        EgByteArraySlicerType* newByteArray = new EgByteArraySlicerType(theHamSlicer, str.size()); // use ham slicer allocator
        // byteArray << str;
        if (newByteArray-> dataSize)
            memcpy((void*) newByteArray->dataChunk, (void*) str.c_str(), newByteArray-> dataSize);
        // PrintByteArray(*newByteArray);
        // EG_LOG_STUB << "insertIndex: " << std::dec << insertIndex << " dataFieldsPtrsArray: " << std::hex << (int64_t) dataFieldsPtrs-> ptrsArray << FN;
        byteArrayPtrs->ptrsArray[index] = newByteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*byteArrayPtrs);
    } else
        EG_LOG_STUB << "ERROR: index overflow: " << DEC << index << FN;
}

void EgStrArray::InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr, EgMultArrayCountType index) {
    if (index < strCount) {
        byteArrayPtrs-> ptrsArray[index] = baPtr;
    } else
        EG_LOG_STUB << "ERROR: index overflow: " << DEC << index << FN;
}

void EgStrArray::InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba, EgMultArrayCountType index) {
    if (index < strCount) {
        EgByteArraySlicerType* newByteArray = new EgByteArraySlicerType(theHamSlicer, ba.dataSize); // use ham slicer allocator
        memcpy((void *)newByteArray->dataChunk, (void *)ba.dataChunk, ba.dataSize);
        // dataFieldsContainer.dataFields.push_back(byteArray);
        byteArrayPtrs->ptrsArray[index] = newByteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        EG_LOG_STUB << "ERROR: index overflow: " << DEC << index << FN;
}

// ======================== Debug ========================

void PrintEgMultArrayFields(const EgStrArray& dataNode){
    // EG_LOG_STUB << "PrintEgDataNodeFields() NodeID: " << std::dec << dataNode.dataNodeID << " Fields: " << FN;
    /* for (auto fieldsIter : dataNode. dataFieldsNames) {
        EG_LOG_STUB << fieldsIter.first << " : ";
        PrintByteArray(*(dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]), 
            dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]->dataSize != 4);  // FIXME stub
        // EG_LOG_STUB << FN;
    } */
}