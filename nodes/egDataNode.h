#pragma once

#include "egDataNodeBlueprint.h"
#include "../service/egPtrArray.h"

class EgDataNode {
public:
    EgDataNodeIDType         dataNodeID         { 0 };
    EgFileOffsetType         dataFileOffset     { 0 };          // stored offset for local file operations speedup
    EgDataNodeBlueprint* dataNodeBlueprint  { nullptr };    // blueprint == class == type of data nodes
    void*                    serialDataPtr      { nullptr };    // link to ext data for serialization

    EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;
    int insertIndex {0}; // stored index for AddNextDataFieldFromType() FIXME check reset

    std::unordered_map < std::string, EgByteArraySlicerType* >  indexedFieldsOldValues; // store old value for index update

    std::unordered_map < EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType >  inLinks;   // links IDs resolving to ptrs
    std::unordered_map < EgBlueprintIDType, EgLinkDataPtrsNodePtrsMapType >  outLinks;    

    EgDataNode() = delete; // {} // for debug only

    EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, bool initMe = true);
    EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, void* a_serialDataPtr);
    ~EgDataNode() { /*std::cout << "EgDataNodeType destructor, ID = " << dataNodeID << std::endl; clear(); */ }

    EgByteArrayAbstractType& operator[](const std::string& fieldStrName);  // field data by name

    // EgDataNode& operator << (const char* str) { InsertDataFieldFromCharStr(str); return *this; }
    EgDataNode& operator << (std::string& s)  { InsertDataFieldFromStr(s); return *this; }
    EgDataNode& operator << (const std::string& s)  { InsertDataFieldFromStr(s); return *this; }
    EgDataNode& operator << (EgByteArrayAbstractType& ba) { InsertDataFieldFromByteArray(ba); return *this; }

    template <typename T> EgDataNode& operator << (const T& value) {
        if (insertIndex < dataNodeBlueprint->fieldsCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray-> dataChunk, (void *)&value, sizeof(value));
            dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        } else
            std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
        return *this;
    }

    void clear();
    void init();

    bool fieldNameExists(const std::string& fieldStrName);
    void getDetailsLayerID (EgDataNodeIDType& thelayerID);

    void* getNextInLinkSerialPtr (EgBlueprintIDType linkBlueprintID, EgDataNode* prevLinkDataPtr); // link data, not linked node
    void* getNextOutLinkSerialPtr(EgBlueprintIDType linkBlueprintID, EgDataNode* prevLinkDataPtr);

    EgDataNode* getInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNode* linkNodePtr); // node, not link
    EgDataNode* getOutLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNode* linkNodePtr);

    EgLinkDataPtrsNodePtrsMapType* getInLinksMap(EgBlueprintIDType linkBlueprintID);
    EgLinkDataPtrsNodePtrsMapType* getOutLinksMap(EgBlueprintIDType linkBlueprintID);

    void deleteInLink (EgBlueprintIDType linkBlueprintID, EgDataNode* delLinkNodePtr); // link data, not linked node
    void deleteOutLink(EgBlueprintIDType linkBlueprintID, EgDataNode* delLinkNodePtr);

    // EgDataNodeType* getNextInLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr); // node, not link
    // EgDataNodeType* getNextOutLinkedNode(EgBlueprintIDType linkBlueprintID, EgDataNodeType* prevLinkDataPtr);     

    // void InsertDataFieldFromCharStr(const char* str);
    void InsertDataFieldFromStr(const std::string& s);
    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr);    

    void makeIndexedFieldsCopy(); // store old value for index update

    void writeDataFieldsToFile (EgFileType& theFile);  // EgDataFieldsType& df,  local file operations
    void readDataFieldsFromFile(EgFileType& theFile);
};

// ======================== Debug ========================
// void PrintEgDataNodeOffsets(const EgDataNodeType& dataNode);
void PrintEgDataNodeFields (const EgDataNode& dataNode);

    /* template <typename T> EgDataNodeType& operator << (T&& rvalue) {
        if (insertIndex < dataNodeBlueprint->fieldsCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint->theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray->dataChunk, (void *)&value, sizeof(value));
            dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        } else
            std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << dataNodeBlueprint-> blueprintName << std::endl;
        return *this;
    } */