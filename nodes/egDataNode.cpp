#include <iostream>
#include "egDataNode.h"
#include "egDataNodeBlueprint.h"

// ========= Byte Array Length Convertors  ===============
const int DATA_CONVERT_MAX_BYTES_COUNT      {10};   // 64 bits to up to 10 bytes 
StaticLengthType    egMask7f                {0x7f}; // get 7 bits to next byte
ByteType            egMask80                {0x80}; // not last byte flag
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
//  ============================================================================

EgDataNode::EgDataNode(EgDataNodeBlueprintType* a_dataNodeBlueprint, bool initMe):
    dataNodeBlueprint(a_dataNodeBlueprint) {
    if (dataNodeBlueprint) {
        // std::cout << "EgDataNodeType() dataNodeBlueprint-> fieldsCount: " << dataNodeBlueprint->blueprintName << " " << std::dec << (int) dataNodeBlueprint-> fieldsCount << std::endl;
        dataFieldsPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (dataNodeBlueprint->theHamSlicer, dataNodeBlueprint-> fieldsCount);
        if (initMe)
            init();
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: EgDataNodeType(): nullptr blueprint in initMe constructor" << std::endl;
}

EgDataNode::EgDataNode(EgDataNodeBlueprintType* a_dataNodeBlueprint, void* a_serialDataPtr):
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

void EgDataNode::init() {
    for (int i = 0; i < dataNodeBlueprint->fieldsCount; i++) {
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, 0); // FIXME STUB check size
        dataFieldsPtrs-> ptrsArray[i] = byteArray;
    }
}

void EgDataNode::clear() {
    dataFieldsPtrs-> clear();
    inLinks.clear();
    outLinks.clear();
    indexedFieldsOldValues.clear();
}

EgLinkDataPtrsNodePtrsMapType* EgDataNode::getInLinksMap(EgBlueprintIDType linkBlueprintID) {
    auto inLinksMapIter = inLinks.find(linkBlueprintID);
    if (inLinksMapIter != inLinks.end()) {
        // std::cout << "getInLinksMap() the map found for ID: " << linkBlueprintID << std::endl;        
        return &(inLinksMapIter-> second);
    }
    std::cout << "getInLinksMap() the map NOT found for ID: " << linkBlueprintID << std::endl; 
    return nullptr;
}

EgLinkDataPtrsNodePtrsMapType* EgDataNode::getOutLinksMap(EgBlueprintIDType linkBlueprintID) {
    auto outLinksMapIter = outLinks.find(linkBlueprintID);
    if (outLinksMapIter != outLinks.end()) {
        // std::cout << "getOutLinksMap() the map found for ID: " << linkBlueprintID << std::endl;        
        return &(outLinksMapIter-> second);
    }
    std::cout << "getOutLinksMap() the map NOT found for ID: " << linkBlueprintID << std::endl; 
    return nullptr;
}

void* EgDataNode::getNextInLinkSerialPtr(EgBlueprintIDType linkBlueprintID, EgDataNode* prevLinkDataPtr) {
    EgLinkDataPtrsNodePtrsMapType* inLinksMap = getInLinksMap(linkBlueprintID);
    if (inLinksMap) {
        if (prevLinkDataPtr) { // null to get first, else get next
            auto inLinksMapIter = inLinksMap-> find(prevLinkDataPtr);
            if (inLinksMapIter != inLinksMap-> end()) {
                ++inLinksMapIter;
                if (inLinksMapIter != inLinksMap-> end()) {
                    // std::cout << "getNextInLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << std::endl;
                    return inLinksMapIter->first-> serialDataPtr;
                }
            }
        } else { // get first
            auto inLinksMapIter = inLinksMap-> begin();
            if (inLinksMapIter != inLinksMap-> end()) {
                // std::cout << "getNextInLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << std::endl;
                return inLinksMapIter->first-> serialDataPtr;
            }            
        }
    }
    std::cout << "getNextInLinkSerialPtr() NOT found for BP ID: " << linkBlueprintID << " prev: " << std::hex << prevLinkDataPtr << std::endl;
    return nullptr;
}

void* EgDataNode::getNextOutLinkSerialPtr(EgBlueprintIDType linkBlueprintID, EgDataNode* prevLinkDataPtr) {
    EgLinkDataPtrsNodePtrsMapType* outLinksMap = getOutLinksMap(linkBlueprintID);
    if (outLinksMap) {
        if (prevLinkDataPtr) { // null to get first, else get next
            auto outLinksMapIter = outLinksMap-> find(prevLinkDataPtr);
            if (outLinksMapIter != outLinksMap-> end()) {
                ++outLinksMapIter;
                if (outLinksMapIter != outLinksMap-> end()) {
                    // std::cout << "getNextOutLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << std::endl;
                    return outLinksMapIter->first-> serialDataPtr;
                }
            }
        } else { // get first
            auto outLinksMapIter = outLinksMap-> begin();
            if (outLinksMapIter != outLinksMap-> end()) {
                // std::cout << "getNextOutLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << std::endl;
                return outLinksMapIter->first-> serialDataPtr;
            }            
        }
    }
    std::cout << "getNextInLinkSerialPtr() NOT found for BP ID: " << linkBlueprintID << " prev: " << std::hex << prevLinkDataPtr << std::endl;
    return nullptr;
}

void EgDataNode::deleteInLink (EgBlueprintIDType linkBlueprintID, EgDataNode* delLinkNodePtr) {
    EgLinkDataPtrsNodePtrsMapType* inLinksMap = getInLinksMap(linkBlueprintID);
    if (inLinksMap) {
        auto inLinksMapIter = inLinksMap->find(delLinkNodePtr);
        if (inLinksMapIter != inLinksMap->end()) {
            inLinksMap->erase(inLinksMapIter);
            inLinksMapIter = inLinksMap->begin();
            if (inLinksMapIter == inLinksMap->end()) // empty map
                inLinks.erase(linkBlueprintID);
            std::cout << "deleteInLink() deleted link: " << std::hex << delLinkNodePtr << std::endl;
        }
    }
}

void EgDataNode::deleteOutLink (EgBlueprintIDType linkBlueprintID, EgDataNode* delLinkDataPtr) {
    EgLinkDataPtrsNodePtrsMapType* outLinksMap = getOutLinksMap(linkBlueprintID);
    if (outLinksMap) {
        auto outLinksMapIter = outLinksMap->find(delLinkDataPtr);
        if (outLinksMapIter != outLinksMap->end()) {
            outLinksMap->erase(outLinksMapIter);
            outLinksMapIter = outLinksMap-> begin();
            if (outLinksMapIter == outLinksMap->end()) // empty map
                outLinks.erase(linkBlueprintID);
            std::cout << "deleteOutLink() deleted link: " << std::hex << delLinkDataPtr << std::endl;
        }
    }
}

EgDataNode* EgDataNode::getInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNode* linkNodePtr) {
    EgLinkDataPtrsNodePtrsMapType* inLinksMap = getInLinksMap(linkBlueprintID);
    if (inLinksMap) {
        auto inLinksMapIter = inLinksMap->find(linkNodePtr);
        if (inLinksMapIter != inLinksMap->end()) {
            return inLinksMapIter-> second;
            // std::cout << "getInLinkedNode() link: " << std::hex << linkNodePtr << std::endl;
        }
    }
    return nullptr;
}

EgDataNode* EgDataNode::getOutLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNode* linkNodePtr) {
    EgLinkDataPtrsNodePtrsMapType* outLinksMap = getOutLinksMap(linkBlueprintID);
    if (outLinksMap) {
        auto outLinksMapIter = outLinksMap->find(linkNodePtr);
        if (outLinksMapIter != outLinksMap->end()) {
            return outLinksMapIter-> second;
            // std::cout << "deleteOutLink() deleted link: " << std::hex << delLinkDataPtr << std::endl;
        }
    }
    return nullptr;
}

EgByteArrayAbstractType& EgDataNode::operator[](const std::string& fieldStrName) { // field value by name as stg::string
    auto iter = dataNodeBlueprint-> dataFieldsNames.find(fieldStrName);
    if (iter != dataNodeBlueprint-> dataFieldsNames.end()) {
        // std::cout << "EgDataNodeType[]: " << dataNodeBlueprint->blueprintName << " field found: " << fieldStrName << std::endl;
        return *(dataFieldsPtrs->ptrsArray[iter->second]);
    }
    std::cout << "ERROR: EgDataNodeType[]: " << dataNodeBlueprint-> blueprintName << " field NOT found: " << fieldStrName << std::endl;
    return dataNodeBlueprint-> egNotFound;
}

void EgDataNode::InsertDataFieldFromCharStr(const char* str) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        // std::cout << "AddNextDataFieldFromCharStr() in: " << str << std::endl;
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, strlen(str) + 1); // use ham slicer allocator
        memcpy((void *)byteArray->dataChunk, (void *)str, byteArray->dataSize);
        // PrintByteArray(*byteArray);
        // std::cout << "insertIndex: " << std::dec << insertIndex << " dataFieldsPtrsArray: " << std::hex << (int64_t) dataFieldsPtrs-> ptrsArray << std::endl;
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
}

void EgDataNode::InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr) {
    dataFieldsPtrs-> ptrsArray[insertIndex++] = baPtr;
}

void EgDataNode::InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, ba.dataSize); // use ham slicer allocator
        memcpy((void *)byteArray->dataChunk, (void *)ba.dataChunk, byteArray->dataSize);
        // dataFieldsContainer.dataFields.push_back(byteArray);
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
}

void EgDataNode::writeDataFieldsToFile(EgFileType &theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    for (int i =0; i < dataNodeBlueprint->fieldsCount; i++) {
        EgByteArrayAbstractType* field = dataFieldsPtrs-> ptrsArray[i];
        if (!field) {
            std::cout  << "DEBUG: writeDataFieldsToFile() field ptr is NULL " << std::endl;
            return;
        } else {
            // PrintByteArray(*field);
            if (field-> dataSize) { // not empty field
                uint8_t lenSize = egConvertStaticToFlex(field->dataSize, lengthRawData);
                // std::cout << "writeDataFieldsToFile() lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << std::endl;
                theFile.fileStream.write((const char *)lengthRawData, lenSize);              // write size
                theFile.fileStream.write((const char *)(field->dataChunk), field->dataSize); // write data
            } else {
                // std::cout << "writeDataFieldsToFile(): zero field size" << std::endl;
                theFile.fileStream << (ByteType) 0;
            }
        }
    }
    // std::cout << "writeDataFieldsToFile() exit " << std::endl;
    theFile.fileStream.flush();
}

void EgDataNode::readDataFieldsFromFile(EgFileType& theFile) {
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
        if (dataFieldSizeTmp) { // not empty field
            theFile.seekRead(savePos + lenSize);
            theFile.fileStream.read((char *)(newField->dataChunk), dataFieldSizeTmp); // read data
        } else {
            theFile.seekRead(savePos + 1);
            // std::cout << "readDataFieldsFromFile(): zero field size" << std::endl;
        }
        // newField->arrayData[dataFieldSizeTmp] = 0;
        // std::cout << " newField.arrayData: " << (char *)(newField->arrayData) << std::endl;
        // df.dataFields[i] = newField;
        dataFieldsPtrs->ptrsArray[i] = newField;
    }
}

void EgDataNode::makeIndexedFieldsCopy() {
    for (auto fieldsIter : dataNodeBlueprint-> indexedFields) {
        std::string fieldName = fieldsIter.first;
        if (! indexedFieldsOldValues.contains (fieldName)) { // copy once
            EgByteArraySlicerType* fieldCopyPtr = new EgByteArraySlicerType(static_cast<EgByteArraySlicerType&> (this-> operator[](fieldName)));
            indexedFieldsOldValues.insert(std::make_pair(fieldName, fieldCopyPtr));
        }
    }
}

// ======================== Debug ========================

void PrintEgDataNodeFields(const EgDataNode& dataNode){
    std::cout << "PrintEgDataNodeFields() NodeID: " << std::dec << dataNode.dataNodeID << " Fields: " << std::endl;
    for (auto fieldsIter : dataNode.dataNodeBlueprint-> dataFieldsNames) {
        std::cout << fieldsIter.first << " : ";
        PrintByteArray(*(dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]), 
            dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]->dataSize != 4);  // FIXME stub
        // std::cout << std::endl;
    }
}
/*
void PrintEgDataNodeOffsets(const EgDataNodeType& dataNode){
    std::cout << "PrintEgDataNodeOffsets() NodeID: " << std::dec << dataNode.dataNodeID
              << ", nodeOffset: 0x" << std::hex << dataNode.dataFileOffset
#ifdef EG_NODE_OFFSETS_DEBUG
              << ", next: 0x" << dataNode.nextNodeOffset
              << ", prev: 0x" << dataNode.prevNodeOffset 
#endif
              << std::endl;
} */