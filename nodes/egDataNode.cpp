#include <iostream>
#include "egDataNode.h"
#include "../metainfo/egLiterals.h"

EgDataNode::EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, bool initMe):
    dataNodeBlueprint(a_dataNodeBlueprint) {
    if (dataNodeBlueprint) {
        // EG_LOG_STUB << "EgDataNodeType() dataNodeBlueprint-> fieldsCount: " << dataNodeBlueprint->blueprintName << " " << std::dec << (int) dataNodeBlueprint-> fieldsCount << FN;
        dataFieldsPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (dataNodeBlueprint->theHamSlicer, dataNodeBlueprint-> fieldsCount);
        if (initMe)
            init();
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        EG_LOG_STUB << "ERROR: EgDataNodeType(): nullptr blueprint in initMe constructor" << FN;
}

EgDataNode::EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, void* a_serialDataPtr):
    dataNodeBlueprint(a_dataNodeBlueprint),
    serialDataPtr(a_serialDataPtr) {
    if (dataNodeBlueprint) {
        // EG_LOG_STUB << "EgDataNodeType() dataNodeBlueprint-> fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << FN;
        dataFieldsPtrs = new EgPtrArrayType<EgByteArrayAbstractType*> (dataNodeBlueprint->theHamSlicer, dataNodeBlueprint-> fieldsCount);
        init();
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        EG_LOG_STUB << "ERROR: EgDataNodeType(): nullptr blueprint in a_serialDataPtr constructor" << FN;
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

void EgDataNode::getDetailsLayerID (EgDataNodeIDType& theLayerID) {
    if ( fieldNameExists(detailsLayerIDName) )
        this-> operator[](detailsLayerIDName) >> theLayerID;
    else
        theLayerID = 0;
}

EgLinkDataPtrsNodePtrsMapType* EgDataNode::getInLinksMap(EgBlueprintIDType linkBlueprintID) {
    // EG_LOG_STUB << "linkBlueprintID: " << linkBlueprintID << FN;
    auto inLinksMapIter = inLinks.find(linkBlueprintID);
    if (inLinksMapIter != inLinks.end()) {
        // EG_LOG_STUB << "getInLinksMap() the map found for ID: " << linkBlueprintID << FN;        
        return &(inLinksMapIter-> second);
    }
    EG_LOG_STUB << "getInLinksMap() the map NOT found for ID: " << linkBlueprintID << FN; 
    return nullptr;
}

EgLinkDataPtrsNodePtrsMapType* EgDataNode::getOutLinksMap(EgBlueprintIDType linkBlueprintID) {
    // EG_LOG_STUB << "linkBlueprintID: " << linkBlueprintID << FN;
    auto outLinksMapIter = outLinks.find(linkBlueprintID);
    if (outLinksMapIter != outLinks.end()) {
        // EG_LOG_STUB << "getOutLinksMap() the map found for ID: " << linkBlueprintID << FN;        
        return &(outLinksMapIter-> second);
    }
    EG_LOG_STUB << "getOutLinksMap() the map NOT found for ID: " << linkBlueprintID << FN; 
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
                    // EG_LOG_STUB << "getNextInLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << FN;
                    return inLinksMapIter->first-> serialDataPtr;
                }
            }
        } else { // get first
            auto inLinksMapIter = inLinksMap-> begin();
            if (inLinksMapIter != inLinksMap-> end()) {
                // EG_LOG_STUB << "getNextInLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << FN;
                return inLinksMapIter->first-> serialDataPtr;
            }            
        }
    }
    // EG_LOG_STUB << "getNextInLinkSerialPtr() NOT found for BP ID: " << linkBlueprintID << " prev: " << std::hex << prevLinkDataPtr << FN;
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
                    // EG_LOG_STUB << "getNextOutLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << FN;
                    return outLinksMapIter->first-> serialDataPtr;
                }
            }
        } else { // get first
            auto outLinksMapIter = outLinksMap-> begin();
            if (outLinksMapIter != outLinksMap-> end()) {
                // EG_LOG_STUB << "getNextOutLinkSerialPtr() found next LinkDataPtr, prev: " << std::hex << prevLinkDataPtr << FN;
                return outLinksMapIter->first-> serialDataPtr;
            }            
        }
    }
    // EG_LOG_STUB << "getNextInLinkSerialPtr() NOT found for BP ID: " << linkBlueprintID << " prev: " << std::hex << prevLinkDataPtr << FN;
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
            EG_LOG_STUB << "deleteInLink() deleted link: " << std::hex << delLinkNodePtr << FN;
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
            EG_LOG_STUB << "deleteOutLink() deleted link: " << std::hex << delLinkDataPtr << FN;
        }
    }
}

EgDataNode* EgDataNode::getInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNode* linkNodePtr) {
    EgLinkDataPtrsNodePtrsMapType* inLinksMap = getInLinksMap(linkBlueprintID);
    if (inLinksMap) {
        auto inLinksMapIter = inLinksMap->find(linkNodePtr);
        if (inLinksMapIter != inLinksMap->end()) {
            return inLinksMapIter-> second;
            // EG_LOG_STUB << "getInLinkedNode() link: " << std::hex << linkNodePtr << FN;
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
            // EG_LOG_STUB << "deleteOutLink() deleted link: " << std::hex << delLinkDataPtr << FN;
        }
    }
    return nullptr;
}

bool EgDataNode::fieldNameExists(const std::string& fieldStrName) {
    auto iter = dataNodeBlueprint-> dataFieldsNames.find(fieldStrName);
    return (iter != dataNodeBlueprint-> dataFieldsNames.end());
}

EgByteArrayAbstractType& EgDataNode::operator[](const std::string& fieldStrName) { // field value by name as stg::string
    auto iter = dataNodeBlueprint-> dataFieldsNames.find(fieldStrName);
    if (iter != dataNodeBlueprint-> dataFieldsNames.end()) {
        // EG_LOG_STUB << "EgDataNodeType[]: " << dataNodeBlueprint->blueprintName << " field found: " << fieldStrName << FN;
        return *(dataFieldsPtrs->ptrsArray[iter->second]);
    }
    EG_LOG_STUB << "ERROR: EgDataNodeType[]: " << dataNodeBlueprint-> blueprintName << " field NOT found: " << fieldStrName << FN;
    return dataNodeBlueprint-> egNotFound;
}

/* void EgDataNode::InsertDataFieldFromCharStr(const char* str) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        // EG_LOG_STUB << "AddNextDataFieldFromCharStr() in: " << str << FN;
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, strlen(str) + 1); // use ham slicer allocator
        memcpy((void *)byteArray->dataChunk, (void *)str, byteArray->dataSize);
        // PrintByteArray(*byteArray);
        // EG_LOG_STUB << "insertIndex: " << std::dec << insertIndex << " dataFieldsPtrsArray: " << std::hex << (int64_t) dataFieldsPtrs-> ptrsArray << FN;
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        EG_LOG_STUB << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << FN;
} */

void EgDataNode::InsertDataFieldFromStr(const std::string& str) {
    if (insertIndex < dataNodeBlueprint->fieldsCount) {
        // EG_LOG_STUB << "AddNextDataFieldFromCharStr() in: " << str << FN;
        EgByteArraySlicerType* byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, str.size()); // use ham slicer allocator
        // byteArray << str;
        memcpy((void*) byteArray->dataChunk, (void*) str.c_str(), byteArray->dataSize);
        // PrintByteArray(*byteArray);
        // EG_LOG_STUB << "insertIndex: " << std::dec << insertIndex << " dataFieldsPtrsArray: " << std::hex << (int64_t) dataFieldsPtrs-> ptrsArray << FN;
        dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
    } else
        EG_LOG_STUB << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << FN;
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
        EG_LOG_STUB << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << FN;
}

void EgDataNode::writeDataFieldsToFile(EgFileType &theFile) {
    EgByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    for (int i =0; i < dataNodeBlueprint->fieldsCount; i++) {
        EgByteArrayAbstractType* field = dataFieldsPtrs-> ptrsArray[i];
        if (!field) {
            EG_LOG_STUB  << "DEBUG: writeDataFieldsToFile() field ptr is NULL " << FN;
            return;
        } else {
            // PrintByteArray(*field);
            if (field-> dataSize) { // not empty field
                uint8_t lenSize = field-> egConvertStaticToFlex(field->dataSize, lengthRawData);
                // EG_LOG_STUB << "writeDataFieldsToFile() lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << FN;
                theFile.fileStream.write((const char*)lengthRawData, lenSize);              // write size
                theFile.fileStream.write((const char*)(field->dataChunk), field->dataSize); // write data
            } else {
                // EG_LOG_STUB << "writeDataFieldsToFile(): zero field size" << FN;
                theFile.fileStream << (EgByteType) 0;
            }
        }
    }
    // EG_LOG_STUB << "writeDataFieldsToFile() exit " << FN;
    theFile.fileStream.flush();
}

void EgDataNode::readDataFieldsFromFile(EgFileType& theFile) {
    EgByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArraySlicerType* newField;
    // df.dataFields.clear();
    // df.dataFields.resize(dataNodeBlueprint-> fieldsCount);
    // EG_LOG_STUB << "readDataFieldsFromFile() fieldsCount: " << std::dec << (int) dataNodeBlueprint-> fieldsCount << FN;
    for (EgFieldsCountType i = 0; i < dataNodeBlueprint-> fieldsCount; i++) {
        uint64_t savePos = static_cast<uint64_t>(theFile.fileStream.tellg());
        uint64_t fileTailSize = theFile.getFileSize() - savePos;
        theFile.seekRead(savePos);
        theFile.fileStream.read((char *)lengthRawData, std::min<uint64_t> ((uint64_t)DATA_CONVERT_MAX_BYTES_COUNT, fileTailSize)); // read size
        uint64_t dataFieldSizeTmp;
        uint8_t lenSize = newField-> egConvertFlexToStatic(lengthRawData, dataFieldSizeTmp);
        // EG_LOG_STUB << "newField.dataSize: " << std::dec << (int) df.dataSize;
        newField = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, dataFieldSizeTmp); // +1
        if (dataFieldSizeTmp) { // not empty field
            theFile.seekRead(savePos + lenSize);
            theFile.fileStream.read((char *)(newField->dataChunk), dataFieldSizeTmp); // read data
        } else {
            theFile.seekRead(savePos + 1);
            // EG_LOG_STUB << "readDataFieldsFromFile(): zero field size" << FN;
        }
        // newField->arrayData[dataFieldSizeTmp] = 0;
        // EG_LOG_STUB << " newField.arrayData: " << (char *)(newField->arrayData) << FN;
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
    EG_LOG_STUB << "PrintEgDataNodeFields() NodeID: " << std::dec << dataNode.dataNodeID << " Fields: " << FN;
    for (auto fieldsIter : dataNode.dataNodeBlueprint-> dataFieldsNames) {
        EG_LOG_STUB << fieldsIter.first << " : ";
        PrintByteArray(*(dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]), 
            dataNode.dataFieldsPtrs->ptrsArray[fieldsIter.second]->dataSize != 4);  // FIXME stub
        // EG_LOG_STUB << FN;
    }
}
/*
void PrintEgDataNodeOffsets(const EgDataNodeType& dataNode){
    EG_LOG_STUB << "PrintEgDataNodeOffsets() NodeID: " << std::dec << dataNode.dataNodeID
              << ", nodeOffset: 0x" << std::hex << dataNode.dataFileOffset
#ifdef EG_NODE_OFFSETS_DEBUG
              << ", next: 0x" << dataNode.nextNodeOffset
              << ", prev: 0x" << dataNode.prevNodeOffset 
#endif
              << FN;
} */