#pragma once

#include "../service/egByteArray.h"
#include "../service/egPtrArray.h"

class EgStrArray {
public:
    EgMultArrayCountType strCount {0};
    EgHamSlicerType   theHamSlicer;
    EgPtrArrayType<EgByteArrayAbstractType*>* byteArrayPtrs;

    EgStrArray() = delete;
    EgStrArray(EgMultArrayCountType count); // {} // for debug only
    ~EgStrArray() {  clear(); byteArrayPtrs-> clear(); }

    EgByteArrayAbstractType& operator[] (int strIndex);  // field data by name

    void clear();
    void init();

    void insertStr(const std::string& str, EgMultArrayCountType index);

    void InsertDataFieldFromByteArray(EgByteArrayAbstractType& ba, EgMultArrayCountType index);
    void InsertRawByteArrayPtr(EgByteArraySlicerType* baPtr, EgMultArrayCountType index);

    void loadFromByteArray(EgByteArrayAbstractType& ba);
    void storeToByteArray(EgByteArrayAbstractType& ba);
};

// ======================== Debug ========================
void PrintEgMultArrayFields (const EgStrArray& dataNode);