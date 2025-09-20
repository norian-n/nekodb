#pragma once

// #define EG_NODE_OFFSETS_DEBUG

#include <vector>
#include <map>

#include "egDataNodeBlueprint.h"
#include "../service/egFileType.h"
#include "../service/egPtrArray.h"
/*
struct EgDataFieldsType {
    // EgFieldsCountType               fieldsCount {0};
    // uint64_t                        dataFieldSizeTmp;
    std::vector<EgByteArrayAbstractType*>   dataFields;  // FIXME move vector to dynamic array
    // std::vector<void*>              extDataPtrs;
};
*/
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
    // EgDataFieldsType            dataFieldsContainer;                // vector of egByteArrays
    EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;
    int insertIndex {0};

    std::map < EgBlueprintIDType, std::vector<EgDataNodeType*> >  inLinks; // links resolving to ptrs
    std::map < EgBlueprintIDType, std::vector<EgDataNodeType*> >  outLinks;

    EgDataNodeType() = delete; // {} // for debug only

    EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, bool initMe = true);
    EgDataNodeType(EgDataNodeBlueprintType* a_dataNodeBlueprint, void* a_serialDataPtr);
    ~EgDataNodeType() { /*std::cout << "EgDataNodeType destructor, ID = " << dataNodeID << std::endl; clear(); */ }

    void clear();
    void init();

    EgByteArrayAbstractType& operator[](std::string& fieldStrName);  // field data by name
    EgByteArrayAbstractType& operator[](const char* fieldCharName);
       
    void writeDataFieldsToFile(EgFileType &theFile);  // EgDataFieldsType& df,  local file operations
    void readDataFieldsFromFile(EgFileType& theFile);

    // EgDataNodeType& operator << (EgDataNodeType& egNode, const char* str); // { AddNextDataFieldFromCharStr(str, egNode); return egNode; }
    // EgDataNodeType& operator << (EgDataNodeType& egNode, std::string& s); //  { AddNextDataFieldFromCharStr(s.c_str(), egNode); return egNode; }
    // EgDataNodeType& operator << (EgDataNodeType& egNode, EgByteArraySlicerType& ba);

    void InsertDataFieldFromCharStr(const char* str);
    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr);

    EgDataNodeType& operator << (const char* str) { InsertDataFieldFromCharStr(str); return *this; }
    EgDataNodeType& operator << (std::string& s)  { InsertDataFieldFromCharStr(s.c_str()); return *this; }
    EgDataNodeType& operator << (EgByteArraySlicerType& ba) { InsertDataFieldFromByteArray(ba); return *this; }

    template <typename T> EgDataNodeType& operator << (T&& i) { AddNextDataFieldFromType<T>(i); return *this; }

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
};

// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal);

// reverse convert variable length dataset size to fixed length one for faster processing
uint8_t egConvertFlexToStatic(ByteType* flexibleVal, StaticLengthType& staticVal);

// ======================== Debug ========================

void PrintEgDataNodeTypeOffsets(const EgDataNodeType& dataNode);
void PrintEgDataNodeTypeFields (const EgDataNodeType& dataNode);

// ===================== Operators =======================


