#include "../nodes/egDataNode.h"

#include <iostream>
#include <cstring>
using namespace std;

const int TEST_FIELDS_COUNT  {3}; 
const string fileName("test_data_fields.tdf");
const std::string field1 {"testField1"};
const std::string field2 {"test some string 2"};
const std::string field3 {"test3"};

const int DATA_CONVERT_MAX_BYTES_COUNT_COPY  {10};

EgFileType          testFile(fileName);
// EgDataFieldsType    testDataFields;
// EgPtrArrayType<EgByteArrayAbstractType*>* dataFieldsPtrs;

EgHamSlicerType     hamSlicer;
EgHamSlicerType     hamSlicerTest;

bool testHamSlicer() {
    EgHamBrickIDType     brickID;
    EgHamBrickIDType     brickID2;
    EgHamBrickIDType     brickID3;
    // EgHamBrickIDType     brickID4;
    EgByteType*          slicePtr;
    EgByteType*          slicePtr2;

    hamSlicerTest.hamBrickSize = 1024;

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
    // PrintHamSlices(hamSlicerTest);
    hamSlicerTest.freeSlice(2);
    hamSlicerTest.freeSlice(2);
    hamSlicerTest.freeSlice(3);
    // PrintHamSlices(hamSlicerTest);
    hamSlicerTest.getSlice(1000, brickID2, slicePtr);
    // cout << "brickID: " << std::dec << brickID2 << " ,slicePtr: " << std::hex << (uint64_t) slicePtr << std::endl;
    hamSlicerTest.getSlice(600, brickID3, slicePtr2);
    // cout << "brickID: " << std::dec << brickID3 << " ,slicePtr: " << std::hex << (uint64_t) slicePtr2 << std::endl;
    // PrintHamSlices(hamSlicerTest);
    return ( slicePtr  == hamSlicerTest.hamBricks[brickID2]-> dataPtr
          && slicePtr2 == hamSlicerTest.hamBricks[brickID3]-> dataPtr
          && hamSlicerTest.hamBricks.size() == 2 
          && hamSlicerTest.hamBricksByFree.size() == 2 );
}

void testLengthConvertor() {
    EgByteType            lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT_COPY];
    EgStaticLengthType    inputLength  {123456789};
    EgStaticLengthType    outputLength {0};

    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();

    cout << "===== Test length convertor =====" << endl; // ( input " << inputLength << " ) =====" << endl;

    int byteCountIn  = byteArray-> egConvertStaticToFlex(inputLength, lengthRawData); // up to 0xffffffffffffffff
    int byteCountOut = byteArray-> egConvertFlexToStatic(lengthRawData, outputLength);

    // cout << "byteCountIn: " << byteCountIn << " byteCountOut: " << byteCountOut << " output: " << testLength << endl;

    if (inputLength == outputLength)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}

void addSampleDataFields(EgDataNode& testNode) {
    // testDataFields.fieldsCount = TEST_FIELDS_COUNT;

    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();
    byteArray-> dataChunk = (EgByteType*) field1.c_str();                     // no alloc, ptr to global mem
    byteArray-> dataSize  = field1.size();

    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);

    byteArray = new EgByteArraySysallocType(field2.size());              // sys heap alloc
    memcpy((void*)byteArray-> dataChunk, (void*) field2.c_str(), byteArray-> dataSize);
    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);

    byteArray = new EgByteArraySlicerType(&hamSlicer, field3.size());  // use ham slicer allocator
    memcpy((void*)byteArray-> dataChunk, (void*) field3.c_str(), byteArray-> dataSize);
    // testDataFields.dataFields.push_back(byteArray);
    testNode.InsertDataFieldFromByteArray(*byteArray);
}

int main() {
    std::string path = "egdb/" + fileName;
    std::remove(path.c_str()); // delete file

    cout << "===== Test hamSlicer =====" << endl;

    if (testHamSlicer())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;

    testLengthConvertor();
/*
    cout << "===== Test egDataFields (" << field1 << " " << strlen(field1) << " " 
                                        << field2 << " " << strlen(field2) << " "
                                        << field3 << " " << strlen(field3) <<  ") =====" << endl;
*/

    cout << "===== Test DataFields =====" << endl;

    EgDataNodeBlueprint testBlueprint("testNodes");

    testBlueprint.BlueprintInitStart();
    testBlueprint.AddDataFieldName("nodesBlueprintID");
    testBlueprint.AddDataFieldName("nodesTypeName");
    testBlueprint.AddDataFieldName("nodesTypeData");
    // testBlueprint.blueprintSettings.isServiceType = true;
    testBlueprint.blueprintMode = egBlueprintActive; // virtual, do NOT commit to db

    EgDataNode testNodeWrite(&testBlueprint);
    EgDataNode testNodeRead(&testBlueprint);

    addSampleDataFields(testNodeWrite);
    // cout << "===== testNodeWrite =====" << endl;
    // PrintEgDataNodeTypeFields(testNodeWrite);

    testFile.openToWrite();
    testNodeWrite.writeDataFieldsToFile(testFile);
    testFile.close();

    testFile.openToRead();
    testNodeRead.readDataFieldsFromFile(testFile);
    testFile.close();

    std::string name;

    testNodeRead["nodesTypeName"] >> name;

    // cout << "name: " << name << endl;

    if (name == "test some string 2")
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;

    // cout << "===== testNodeRead =====" << endl;
    // PrintEgDataNodeTypeFields(testNodeRead);

    // for (const auto& field : testDataFields.dataFields)
    //    PrintByteArray(*field) ;

    // cout << endl << endl;

    // testDataFields.dataFields.clear();

    // cout << "===== Tests end =====" << endl << endl;

    return 0;
}
