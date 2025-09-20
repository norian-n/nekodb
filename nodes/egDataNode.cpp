#include <iostream>
#include "egDataNode.h"
#include "egDataNodeBlueprint.h"

//  ============================================================================
// EgByteArrayType egNotFound(0);
// const char* egNotFoundStr = "<Data Not Found>";

const int DATA_CONVERT_MAX_BYTES_COUNT      {10};   // 64 bits to up to 10 bytes 
StaticLengthType    egMask7f                {0x7f}; // get 7 bits to next byte
ByteType            egMask80                {0x80}; // not last byte flag
//  ============================================================================

EgDataNodeType::EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, bool initMe):
    dataNodeBlueprint(a_dataNodeBlueprint) {
    if (dataNodeBlueprint) {
        // std::cout << "EgDataNodeType() dataNodeBlueprint-> fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << std::endl;
        dataFieldsPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (dataNodeBlueprint->theHamSlicer, dataNodeBlueprint-> fieldsCount);
        if (initMe)
            init();
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: EgDataNodeType(): nullptr blueprint in initMe constructor" << std::endl;
}

EgDataNodeType::EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, void* a_serialDataPtr):
    dataNodeBlueprint(a_dataNodeBlueprint),
    serialDataPtr(a_serialDataPtr) {
    if (dataNodeBlueprint) {
        // std::cout << "EgDataNodeType() dataNodeBlueprint-> fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << std::endl;
        dataFieldsPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (dataNodeBlueprint->theHamSlicer, dataNodeBlueprint-> fieldsCount);
        init();
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: EgDataNodeType(): nullptr blueprint in a_serialDataPtr constructor" << std::endl;
}

void EgDataNodeType::init() {
    for (int i = 0; i < dataNodeBlueprint->fieldsCount; i++) {
        EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, 0); // FIXME STUB check size
        dataFieldsPtrs-> ptrsArray[i] = byteArray;
    }
}

void EgDataNodeType::clear() {
    /*
    for (auto fieldsIter : dataFieldsContainer.dataFields) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete fieldsIter;
    dataFieldsContainer.dataFields.clear(); */
    // for (int i = 0; i < dataNodeBlueprint->fieldsCount; i++) {  FIXME clear byte arrays
    dataFieldsPtrs-> clear();
    inLinks.clear();
    outLinks.clear();
}

EgByteArrayAbstractType& EgDataNodeType::operator[](std::string& fieldStrName) { // field value by name as stg::string
    if (dataNodeBlueprint) {
        auto iter = dataNodeBlueprint->dataFieldsNames.find(fieldStrName);
        if (iter != dataNodeBlueprint->dataFieldsNames.end())
            // return *dataFieldsContainer.dataFields[iter->second];
            return *(dataFieldsPtrs-> ptrsArray[iter->second]);
    }
    return dataNodeBlueprint-> egNotFound;
}

EgByteArrayAbstractType& EgDataNodeType::operator[](const char *fieldCharName) { // field value by name as char* literal
    if (dataNodeBlueprint) {
        auto iter = dataNodeBlueprint->dataFieldsNames.find(std::string(fieldCharName));
        if (iter != dataNodeBlueprint->dataFieldsNames.end())
            // return *dataFieldsContainer.dataFields[iter->second];
            return *(dataFieldsPtrs-> ptrsArray[iter->second]);
    }
    return dataNodeBlueprint-> egNotFound;
}

// ======================== Debug ========================

void PrintEgDataNodeTypeOffsets(const EgDataNodeType& dataNode){
    std::cout << "NodeID: " << std::dec << dataNode.dataNodeID
              << ", nodeOffset: 0x" << std::hex << dataNode.dataFileOffset
#ifdef EG_NODE_OFFSETS_DEBUG
              << ", next: 0x" << dataNode.nextNodeOffset
              << ", prev: 0x" << dataNode.prevNodeOffset 
#endif
              << std::endl;
}

void PrintEgDataNodeTypeFields(const EgDataNodeType& dataNode){
    std::cout << "NodeID: " << std::dec << dataNode.dataNodeID << " Fields: " << std::endl;
    // std::cout << "dataFieldsContainer size: " << dataNode.dataFieldsContainer.dataFields.size() << " Fields: " << std::endl;
    // for (const auto &field : dataNode.dataFieldsContainer.dataFields)
        // PrintByteArray(*field);
    for (int i =0; i < dataNode.dataNodeBlueprint->fieldsCount; i++)
        PrintByteArray(*(dataNode.dataFieldsPtrs->ptrsArray[i]));
}

// ======================== DataFields ========================

void EgDataNodeType::InsertDataFieldFromCharStr(const char* str) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        // std::cout << "AddNextDataFieldFromCharStr() in: " << str << std::endl;
        EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, strlen(str) + 1); // use ham slicer allocator
        memcpy((void *)byteArray->arrayData, (void *)str, byteArray->dataSize);
        // dataFieldsContainer.dataFields.push_back(byteArray);
        // PrintByteArray(*byteArray);
        // std::cout << "insertIndex: " << std::dec << insertIndex << " dataFieldsPtrsArray: " << std::hex << (int64_t) dataFieldsPtrs-> ptrsArray << std::endl;
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
}

void EgDataNodeType::InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr) {
    dataFieldsPtrs->ptrsArray[insertIndex++] = baPtr;
}

void EgDataNodeType::InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, ba.dataSize); // use ham slicer allocator
        // EgByteArrayType* byteArray = new EgByteArrayType(strlen(str)+1);
        memcpy((void *)byteArray->arrayData, (void *)ba.arrayData, byteArray->dataSize);
        // dataFieldsContainer.dataFields.push_back(byteArray);
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
}

// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal)
{
    StaticLengthType    buf         {0};
    int                 byteCount   {1};

    while (staticVal && (byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1))) {
        buf = staticVal & egMask7f; // get 7 bits to next byte
        flexibleVal[byteCount-1] = static_cast<ByteType>(buf);
            
        // std::cout << "staticVal out: " << std::dec << staticVal << std::endl;
        staticVal = staticVal >> 7; // shift static counter to get next 7 bits
        if (staticVal) {
            flexibleVal[byteCount-1] |=  egMask80; // not last byte
            // std::cout << "byte out: " << std::hex << (int) flexibleVal[byteCount-1] << std::endl; 
            byteCount++;
        }
        // else // last byte
            // std::cout << "last byte out: " << std::hex << (int) flexibleVal[byteCount-1] << std::endl;   
    }
    return byteCount;
}

// reverse convert variable length dataset size to fixed length one for faster processing
uint8_t egConvertFlexToStatic(ByteType* flexibleVal, StaticLengthType& staticVal)
{
    staticVal = 0;
    StaticLengthType    buf         {0};
    int                 byteCount   {1};

    while ( byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1) ) {
        buf = static_cast<ByteType> (flexibleVal[byteCount-1]) & egMask7f; // get 7 bits to next byte
        // std::cout << "byte in: " << hex << (int) flexibleVal[byteCount-1] << endl;        
        staticVal = (buf << 7 * (byteCount-1)) | staticVal;
        // std::cout << "buf in: " << std::dec << buf << std::endl;
        // std::cout << "staticVal in: " << std::dec << staticVal << std::endl;
            // check "continue" bit
        if ( !(flexibleVal[byteCount-1] & egMask80) ) // last byte
            break;
        byteCount++;
    }
    return byteCount;
}
/*
void EgDataNodeType::writeDataFieldsToFile(EgDataFieldsType& df, EgFileType &theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    // std::cout << "writeDataFieldsToFile() start, " << std::dec << df.dataFields.size() << std::endl;
    std::vector<EgByteArrayAbstractType*>::iterator field;
    // for (const auto &field : df.dataFields) {
    for(field = df.dataFields.begin(); field != df.dataFields.end(); ++field ) {
        if ( !(*field)) {
            std::cout  << "DEBUG: writeDataFieldsToFile() field ptr is NULL " << std::endl;
            return;
        } else {
            // PrintByteArray(*field);
            uint8_t lenSize = egConvertStaticToFlex((*field)->dataSize, lengthRawData);
            // std::cout << "lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << std::endl;
            theFile.fileStream.write((const char *)lengthRawData, lenSize);                    // write size
            theFile.fileStream.write((const char *)((*field)->arrayData), (*field)->dataSize); // write data
        }
    }
    // std::cout << "writeDataFieldsToFile() exit " << std::endl;
    theFile.fileStream.flush();
}
*/

void EgDataNodeType::writeDataFieldsToFile(EgFileType &theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h

    for (int i =0; i < dataNodeBlueprint->fieldsCount; i++) {
        EgByteArrayAbstractType* field = dataFieldsPtrs-> ptrsArray[i];
        if (!field) {
            std::cout  << "DEBUG: writeDataFieldsToFile() field ptr is NULL " << std::endl;
            return;
        } else {
            // PrintByteArray(*field);
            uint8_t lenSize = egConvertStaticToFlex(field-> dataSize, lengthRawData);
            // std::cout << "lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << std::endl;
            theFile.fileStream.write((const char *)lengthRawData, lenSize);                    // write size
            theFile.fileStream.write((const char *)(field->arrayData), field-> dataSize); // write data
        }
    }
    // std::cout << "writeDataFieldsToFile() exit " << std::endl;
    theFile.fileStream.flush();
}

void EgDataNodeType::readDataFieldsFromFile(EgFileType& theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArraySlicerType* newField;
    // df.dataFields.clear();
    // df.dataFields.resize(dataNodeBlueprint-> fieldsCount);
    // std::cout << "readDataFieldsFromFile() fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << std::endl;
    for (EgFieldsCountType i = 0; i < dataNodeBlueprint-> fieldsCount; i++) {
        uint64_t savePos = static_cast<uint64_t>(theFile.fileStream.tellg());
        uint64_t fileTailSize = theFile.getFileSize() - savePos;
        theFile.seekRead(savePos);
        theFile.fileStream.read((char *)lengthRawData, std::min((uint64_t)DATA_CONVERT_MAX_BYTES_COUNT, fileTailSize)); // read size
        uint64_t dataFieldSizeTmp;
        uint8_t lenSize = egConvertFlexToStatic(lengthRawData, dataFieldSizeTmp);
        // std::cout << "newField.dataSize: " << std::dec << (int) df.dataSize;
        newField = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, dataFieldSizeTmp); // +1
        theFile.seekRead(savePos + lenSize);
        theFile.fileStream.read((char *)(newField->arrayData), dataFieldSizeTmp); // read data
        // newField->arrayData[dataFieldSizeTmp] = 0;
        // std::cout << " newField.arrayData: " << (char *)(newField->arrayData) << std::endl;
        // df.dataFields[i] = newField;
        dataFieldsPtrs->ptrsArray[i] = newField;
    }
}

/*
void EgDataNodeType::readDataFieldsFromFile(EgDataFieldsType& df, EgFileType& theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArraySlicerType* newField;
    df.dataFields.clear();
    df.dataFields.resize(dataNodeBlueprint-> fieldsCount);
    // std::cout << "readDataFieldsFromFile() fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << std::endl;
    for (EgFieldsCountType i = 0; i < dataNodeBlueprint-> fieldsCount; i++) {
        uint64_t savePos = static_cast<uint64_t>(theFile.fileStream.tellg());
        uint64_t fileTailSize = theFile.getFileSize() - savePos;
        theFile.seekRead(savePos);
        theFile.fileStream.read((char *)lengthRawData, std::min((uint64_t)DATA_CONVERT_MAX_BYTES_COUNT, fileTailSize)); // read size
        uint64_t dataFieldSizeTmp;
        uint8_t lenSize = egConvertFlexToStatic(lengthRawData, dataFieldSizeTmp);
        // std::cout << "newField.dataSize: " << std::dec << (int) df.dataSize;
        newField = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, dataFieldSizeTmp); // +1
        theFile.seekRead(savePos + lenSize);
        theFile.fileStream.read((char *)(newField->arrayData), dataFieldSizeTmp); // read data
        // newField->arrayData[dataFieldSizeTmp] = 0;
        // std::cout << " newField.arrayData: " << (char *)(newField->arrayData) << std::endl;
        df.dataFields[i] = newField;
    }
}
*/
/*
template <typename T> void AddNextDataFieldFromType(T&& value, EgDataNodeType& theNode) {
    // EgByteArrayType* byteArray = new EgByteArrayType();
    EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(&(theNode.dataNodeBlueprint-> theHamSlicer), sizeof(value));  // use ham slicer allocator
    memcpy((void*)byteArray-> arrayData, (void*) &value, sizeof(value));
    // ByteArrayFromType<T> (value, *byteArray);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);
}
*/
// ===================== Operators ======================= // EgDataNodeType& egNode, 
/*
EgDataNodeType& EgDataNodeType::operator << (const char* str) { AddNextDataFieldFromCharStr(str, egNode); return *this; }
EgDataNodeType& EgDataNodeType::operator << (std::string& s)  { AddNextDataFieldFromCharStr(s.c_str(), egNode); return *this; }
EgDataNodeType& EgDataNodeType::operator << (EgByteArraySlicerType& ba) { AddNextDataFieldFromByteArray(ba, egNode); return *this; }
*/
// template <typename T> EgDataNodeType& operator << (EgDataNodeType& egNode, T&& i) { AddNextDataFieldFromType<T>(i, egNode); return egNode; }

