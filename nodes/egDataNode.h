#pragma once

#include "egDataNodeBlueprint.h"
#include "../service/egPtrArray.h"

class EgDataNode {
public:
    EgDataNodeIDType        dataNodeID         { 0 };
    EgFileOffsetType        dataFileOffset     { 0 };          // stored offset for local file operations speedup
    EgDataNodeBlueprint*    dataNodeBlueprint  { nullptr };    // blueprint == class == type of data nodes
    void*                   serialDataPtr      { nullptr };    // link to ext data for serialization

    EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;
    int insertIndex {0}; // stored index for AddNextDataFieldFromType() FIXME check reset

    std::unordered_map < std::string, EgByteArraySlicerType* >  indexedFieldsOldValues; // store old value for index update

    std::unordered_map < EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType >  inLinks;    // links IDs resolved to nodes ptrs by link type
    std::unordered_map < EgNodesSetIDType, EgLinkDataPtrsNodePtrsMapType >  outLinks;    

    EgDataNode() = delete; // blueprint required

    EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, bool initFieldsBA = true); // call initByteArrays flag
    EgDataNode(EgDataNodeBlueprint* a_dataNodeBlueprint, void* a_serialDataPtr);
    ~EgDataNode() { clear(); delete dataFieldsPtrs; /*std::cout << "EgDataNodeType destructor, ID = " << dataNodeID << std::endl; clear(); */ }

    EgByteArrayAbstractType& operator[](const std::string& fieldStrName);  // field data by name

    EgDataNode& operator << (std::string& s)              { InsertDataFieldFromStr(s); return *this; }
    EgDataNode& operator << (const std::string& s)        { InsertDataFieldFromStr(s); return *this; }
    EgDataNode& operator << (EgByteArrayAbstractType& ba) { InsertDataFieldFromByteArray(ba); return *this; }

    template <typename T> EgDataNode& operator << (const T& value) {
        if (insertIndex < dataNodeBlueprint-> fieldsCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(dataNodeBlueprint-> theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray-> dataChunk, (void *)&value, sizeof(value));
            dataFieldsPtrs-> ptrsArray[insertIndex++] = byteArray;
        } else
            EG_LOG_STUB << "ERROR: data node fields count overflow: " << dataNodeBlueprint-> blueprintName << FN;
        return *this;
    }

    void clear();
    void initByteArrays();

    bool fieldNameExists(const std::string& fieldStrName);
    void getDetailsLayerID (EgDataNodeIDType& thelayerID);

    void* getNextInLinkSerialPtr (EgNodesSetIDType linkBlueprintID, EgDataNode* prevLinkDataPtr); // link data, not linked node
    void* getNextOutLinkSerialPtr(EgNodesSetIDType linkBlueprintID, EgDataNode* prevLinkDataPtr);

    EgDataNode* getInLinkedNode(EgNodesSetIDType linkBlueprintID, EgDataNode* linkNodePtr); // node, not link
    EgDataNode* getOutLinkedNode(EgNodesSetIDType linkBlueprintID, EgDataNode* linkNodePtr);

    EgLinkDataPtrsNodePtrsMapType* getInLinksMap(EgNodesSetIDType linkBlueprintID);
    EgLinkDataPtrsNodePtrsMapType* getOutLinksMap(EgNodesSetIDType linkBlueprintID);

    void deleteInLink (EgNodesSetIDType linkBlueprintID, EgDataNode* delLinkNodePtr); // link data, not linked node
    void deleteOutLink(EgNodesSetIDType linkBlueprintID, EgDataNode* delLinkNodePtr);

    void InsertDataFieldFromStr(const std::string& s);
    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr);    

    void makeIndexedFieldsCopy(); // store old value for index update

    void writeDataFieldsToFile (EgFileType& theFile);  // EgDataFieldsType& df,  local file operations
    void readDataFieldsFromFile(EgFileType& theFile);
};

void PrintEgDataNodeFields (const EgDataNode& dataNode);  // debug