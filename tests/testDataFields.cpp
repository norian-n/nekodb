#include "../nodes/egDataNode.h"

#include <iostream>
#include <cstring>
using namespace std;

const int TEST_FIELDS_COUNT  {3}; 
const string fileName("test_data_fields.tdf");
const char* field1 = "testField1\0";
const char* field2 = "test some string 2\0";
const char* field3 = "test3\0";

const int DATA_CONVERT_MAX_BYTES_COUNT_COPY  {10}; 

ByteType            lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT_COPY];
StaticLengthType    inputLength {123456789};
StaticLengthType    testLength;

EgFileType          testFile(fileName);
// EgDataFieldsType    testDataFields;
// EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;

EgHamSlicerType     hamSlicer;
EgHamSlicerType     hamSlicerTest;

bool testHamSlicer() {
    EgHamBrickIDType     brickID;
    ByteType*            slicePtr;
    ByteType*            slicePtr2;

    hamSlicerTest.getSlice(500, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(500, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(600, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(300, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(300, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    // PrintHamSlices(hamSlicerTest);
    hamSlicerTest.freeSlice(1);
    hamSlicerTest.freeSlice(1);
    hamSlicerTest.freeSlice(2);
    // PrintHamSlices(hamSlicerTest);
    hamSlicerTest.freeSlice(2);
    hamSlicerTest.freeSlice(3);
    // PrintHamSlices(hamSlicerTest);
    hamSlicerTest.getSlice(500, brickID, slicePtr);
    // cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(600, brickID, slicePtr2);
    /// cout << "brickID: " << std::dec << brickID << " ,slicePtr: " << std::hex << (uint64_t) slicePtr2 << std::endl;
    // PrintHamSlices(hamSlicerTest);
    return ( slicePtr  == hamSlicerTest.hamBricks[4].brickPtr
          && slicePtr2 == hamSlicerTest.hamBricks[5].brickPtr
          && hamSlicerTest.hamBricks.size() == 2 
          && hamSlicerTest.hamBricksByFree.size() == 2 );
}

void addSampleDataFields(EgDataNodeType& testNode) {
    // testDataFields.fieldsCount = TEST_FIELDS_COUNT;

    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();
    byteArray-> arrayData = (ByteType*) field1;                     // no alloc, ptr to global mem
    byteArray-> dataSize  = strlen(field1)+1;
    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);

    byteArray = new EgByteArraySysallocType(strlen(field2)+1);              // sys heap alloc
    memcpy((void*)byteArray-> arrayData, (void*) field2, byteArray-> dataSize);
    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);

    byteArray = new EgByteArraySlicerType(&hamSlicer, strlen(field3)+1);  // use ham slicer allocator
    memcpy((void*)byteArray-> arrayData, (void*) field3, byteArray-> dataSize);
    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);
}

int main() {
    std::remove(fileName.c_str()); // delete file

    cout << "===== Test length convertor ( " << inputLength << " ) =====" << endl;

    int byteCountIn  = egConvertStaticToFlex(inputLength, lengthRawData); // up to 0xffffffffffffffff
    int byteCountOut = egConvertFlexToStatic(lengthRawData, testLength);

    cout << "byteCountIn: " << byteCountIn << " byteCountOut: " << byteCountOut << " testLength: " << testLength << endl;

    cout << "===== Test hamSlicer =====" << endl;

    if (testHamSlicer())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
/*
    cout << "===== Test egDataFields (" << field1 << " " << strlen(field1) << " " 
                                        << field2 << " " << strlen(field2) << " "
                                        << field3 << " " << strlen(field3) <<  ") =====" << endl;
*/
    EgDataNodeBlueprintType testBlueprint("testNodes");

    testBlueprint.BlueprintInitStart();
    testBlueprint.AddDataFieldName("nodesBlueprintID");
    testBlueprint.AddDataFieldName("nodesTypeName");
    testBlueprint.AddDataFieldName("nodesTypeData");
    testBlueprint.blueprintSettings.isServiceType = true;
    testBlueprint.blueprintMode = egBlueprintActive; // virtual, do NOT commit to db

    EgDataNodeType testNodeWrite(&testBlueprint);
    EgDataNodeType testNodeRead(&testBlueprint);

    addSampleDataFields(testNodeWrite);
    // cout << "===== testNodeWrite =====" << endl;
    // PrintEgDataNodeTypeFields(testNodeWrite);

    testFile.openToWrite();
    testNodeWrite.writeDataFieldsToFile(testFile);
    testFile.close();

    testFile.openToRead();
    testNodeRead.readDataFieldsFromFile(testFile);
    testFile.close();

    // cout << "===== testNodeRead =====" << endl;
    // PrintEgDataNodeTypeFields(testNodeRead);

    // for (const auto& field : testDataFields.dataFields)
    //    PrintByteArray(*field) ;

    // cout << endl << endl;

    // testDataFields.dataFields.clear();

    // cout << "===== Tests end =====" << endl << endl;

    return 0;
}
