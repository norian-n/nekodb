#pragma once

// #define EG_NODE_OFFSETS_DEBUG

#include <map>

#include "egDataNodeBlueprint.h"
#include "../service/egPtrArray.h"

class EgDataNodeType {
public:
    EgDataNodeIDType        dataNodeID      { 0 };
    EgFileOffsetType        dataFileOffset  { 0 };          // stored offsets for local file operations speedup
#ifdef EG_NODE_OFFSETS_DEBUG
    EgFileOffsetType        nextNodeOffset  { 0 };
    EgFileOffsetType        prevNodeOffset  { 0 };
#endif
    EgDataNodeBlueprintType*    dataNodeBlueprint   { nullptr };    // blueprint == class == type of data nodes
    void*                       serialDataPtr       { nullptr };    // link to ext data for serialization

    EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;
    int insertIndex {0}; // stored index for AddNextDataFieldFromType() FIXME check reset

    std::map < std::string, EgByteArraySlicerType* >  indexedFieldsOldValues; // store old value for index update

    std::map < EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType >  inLinks;   // links IDs resolving to ptrs
    std::map < EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType >  outLinks;    

    EgDataNodeType() = delete; // {} // for debug only

    EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, bool initMe = true);
    EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, void* a_serialDataPtr);
    ~EgDataNodeType() { /*std::cout << "EgDataNodeType destructor, ID = " << dataNodeID << std::endl; clear(); */ }

    void clear();
    void init();

    EgLinkDataPtrsNodePtrsMapType* getInLinksMap(EgBlueprintIDType linkBlueprintID);
    EgLinkDataPtrsNodePtrsMapType* getOutLinksMap(EgBlueprintIDType linkBlueprintID);

    void* getNextInLinkSerialPtr (EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr); // link data, not linked node
    void* getNextOutLinkSerialPtr(EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr);

    void deleteInLink (EgBlueprintIDType linkBlueprintID, EgDataNodeType* delLinkNodePtr); // link data, not linked node
    void deleteOutLink(EgBlueprintIDType linkBlueprintID, EgDataNodeType* delLinkNodePtr);

    EgDataNodeType* getInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* linkNodePtr); // node, not link
    EgDataNodeType* getOutLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* linkNodePtr);     

    // EgDataNodeType* getNextInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr); // node, not link
    // EgDataNodeType* getNextOutLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr);     

    EgByteArrayAbstractType& operator[](std::string& fieldStrName);  // field data by name
    EgByteArrayAbstractType& operator[](const char* fieldCharName);

    void InsertDataFieldFromCharStr(const char* str);
    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr);

    EgDataNodeType& operator << (const char* str) { InsertDataFieldFromCharStr(str); return *this; }
    EgDataNodeType& operator << (std::string& s)  { InsertDataFieldFromCharStr(s.c_str()); return *this; }
    EgDataNodeType& operator << (EgByteArraySlicerType& ba) { InsertDataFieldFromByteArray(ba); return *this; }

    template <typename T> EgDataNodeType& operator << (T&& rvalue) { AddNextDataFieldFromType<T>(rvalue); return *this; }

    template <typename T> void AddNextDataFieldFromType(T&& value) {
        if (insertIndex < dataNodeBlueprint->fieldsCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray->arrayData, (void *)&value, sizeof(value));
            // dataFieldsContainer.dataFields.push_back(byteArray);
            dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray; // static_cast<EgByteArrayAbstractType> (byteArray);
            // PrintPtrsArray<EgByteArrayAbstractType*> (*dataFieldsPtrs);
        } else
            std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
    }

    void makeIndexedFieldCopy(std::string& fieldName);

    void writeDataFieldsToFile(EgFileType &theFile);  // EgDataFieldsType& df,  local file operations
    void readDataFieldsFromFile(EgFileType& theFile);
};

// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal);

// reverse convert variable length dataset size to fixed length one for faster processing
uint8_t egConvertFlexToStatic(ByteType* flexibleVal, StaticLengthType& staticVal);

// ======================== Debug ========================

void PrintEgDataNodeOffsets(const EgDataNodeType& dataNode);
void PrintEgDataNodeFields (const EgDataNodeType& dataNode);