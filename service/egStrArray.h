#pragma once

#include "../service/egByteArray.h"
#include "../service/egPtrArray.h"

class EgStrArray {
public:
    EgMultArrayCountType strCount {0};

    // EgMultArrayCountType insertIndex;  // FIXME STUB 
    EgHamSlicerType   theHamSlicer;

    EgPtrArrayType<EgByteArrayAbstractType*>* byteArrayPtrs;
    // int insertIndex {0}; // stored index for AddNextDataFieldFromType() FIXME check reset

    EgStrArray() = delete;

    EgStrArray(EgMultArrayCountType count); // {} // for debug only

    ~EgStrArray() {  clear(); byteArrayPtrs-> clear(); }

    EgByteArrayAbstractType& operator[] (int strIndex);  // field data by name

    // EgMultArray& operator << (const char* str) { InsertDataFieldFromCharStr(str); return *this; }
    /* EgMultArray& operator << (std::string& s)              { InsertDataFieldFromStr(s); return *this; }
    EgMultArray& operator << (const std::string& s)        { InsertDataFieldFromStr(s); return *this; }
    EgMultArray& operator << (EgByteArrayAbstractType& ba) { InsertDataFieldFromByteArray(ba); return *this; } */

    /* template <typename T> EgMultArray& operator << (const T& value) {
        if (insertIndex < strCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray-> dataChunk, (void *)&value, sizeof(value));
            byteArrayPtrs->ptrsArray[insertIndex++] = byteArray;
        } else
            std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " << std::endl;
        return *this;
    } */

    void clear();
    void init();

    void insertStr(const std::string& str, EgMultArrayCountType index);

    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba, EgMultArrayCountType index);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr, EgMultArrayCountType index);

    void loadFromByteArray(EgByteArrayAbstractType& ba);
    void storeToByteArray(EgByteArrayAbstractType& ba);

    // void writeDataFieldsToFile (EgFileType& theFile);  // EgDataFieldsType& df,  local file operations
    // void readDataFieldsFromFile(EgFileType& theFile);
};

// ======================== Debug ========================
void PrintEgMultArrayFields (const EgStrArray& dataNode);

    /* template <typename T> EgDataNodeType& operator << (T&& rvalue) {
        if (insertIndex < fieldsCount) {
            EgByteArraySlicerType *byteArray = new EgByteArraySlicerType(theHamSlicer, sizeof(value)); // use ham slicer allocator
            memcpy((void *)byteArray->dataChunk, (void *)&value, sizeof(value));
            dataFieldsPtrs->ptrsArray[insertIndex++] = byteArray;
        } else
            std::cout << "ERROR: AddNextDataFieldFromType() fields count overflow: " <<  blueprintName << std::endl;
        return *this;
    } */