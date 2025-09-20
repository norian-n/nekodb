
#include <iostream>
#include <cstring>
using namespace std;

// !! compiler option required -D EG_NODE_OFFSETS_DEBUG

// #include "../nodes/egDataNodesType.h"
#include "../nodes/egDataNodesLocalFile.h"

EgDataNodeBlueprintType testBlueprint("testLocalFile");

const int TEST_FIELDS_COUNT  {3}; 
const char* field1 = "testField1\0";
const char* field2 = "test some string 2\0";
const char* field3 = "test3\0";

inline void addSampleDataFields(EgDataNodeType& testDataNode) {
    // cout << "===== addSampleDataFields() in " << " =====" << endl;
    testDataNode << field1;
    testDataNode << field2;
    testDataNode << field3;
    // cout << "===== addSampleDataFields() out " << " =====" << endl;
}

bool initDataNodeBlueprint() {
    // cout << "===== Test DataNodeBlueprint " << " =====" << endl;

    testBlueprint.BlueprintInitStart();

    testBlueprint.AddDataFieldName("field_1");
    testBlueprint.AddDataFieldName("testField 2");
    testBlueprint.AddDataFieldName("my_field 3");

    testBlueprint.blueprintSettings.useEntryNodes         = true;
    testBlueprint.blueprintSettings.useGUIsettings        = true;
    testBlueprint.blueprintSettings.useLinks              = true;
    testBlueprint.blueprintSettings.useNamedAttributes    = true;
    testBlueprint.blueprintSettings.useVisualSpace        = true;

    testBlueprint.BlueprintInitCommit();

    testBlueprint.LocalStoreBlueprint();
    testBlueprint.LocalLoadBlueprint();
    
    return true;
}


int main()
{
    int nodesCount  {0};

    cout << "===== Test egDataNodeLocalFile " /* (\"" << field1 << "\" " << " \""
         << field2 << "\" " << " \""
         << field3 << "\" " << ")*/ << "=====" << endl;

    std::remove("testLocalFile.dnl"); // delete blueprint file
    std::remove("testLocalFile.gdn"); // delete blueprint file

    EgDataNodesLocalFileType testLocalFile;
    initDataNodeBlueprint();

    EgDataNodeType testDataNode(&testBlueprint);
    EgDataNodeType testNextNode(&testBlueprint);
    // EgDataNodeBlueprintType testBlueprint("testLocalFile");
    // testDataNode.dataNodeBlueprint = &testBlueprint;

    // testDataNode.dataFieldsContainer.fieldsCount = TEST_FIELDS_COUNT;
    addSampleDataFields(testDataNode);
    // PrintEgDataNodeTypeFields(testDataNode);
        // write
    std::string fname {"testLocalFile"};
    testLocalFile.StartFileUpdate(fname);
    testDataNode.dataNodeID = 12121212;
    testLocalFile.WriteDataNode(&testDataNode);
    // testLocalFile.PrintHeader();
    testDataNode.dataNodeID = 38383838;
    testLocalFile.WriteDataNode(&testDataNode);
    // testLocalFile.PrintHeader();
    testDataNode.dataNodeID = 44444444;
    testLocalFile.WriteDataNode(&testDataNode);
    // testLocalFile.PrintHeader();
    testLocalFile.WriteHeader();
    testLocalFile.nodesFile.close();

    /* testLocalFile.OpenFileToRead("testLocalFile");
    testLocalFile.nodesFile.seekRead(100500);
    cout << "File fail(): " << testLocalFile.nodesFile.fileStream.fail() 
         << " file eof(): " << testLocalFile.nodesFile.fileStream.eof()<< endl;
    testLocalFile.nodesFile.close();
    return 0; */

        // read nodes
    testDataNode.clear();
    // std::string fname {"testLocalFile"};
    testLocalFile.OpenFileToRead(fname);
    testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
    nodesCount = 0;
    while (testNextNode.dataFileOffset) {
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);
        // PrintEgDataNodeTypeOffsets(testNextNode);
        // PrintEgDataNodeTypeFields(testNextNode);
        testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
        nodesCount++;
    }
    // testLocalFile.PrintNodesChain();
    testLocalFile.nodesFile.close();

    if (nodesCount == 3)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;

/*
        // delete node
    cout << "===== Test DeleteDataNode() - delete second one if nodes count >= 3 =====" << endl;
    if (nodesCount >= 3)
    {
        testLocalFile.OpenFileToUpdate("testLocalFile");
        testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);

        testDataNode.dataFileOffset = testNextNode.nextNodeOffset;
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testDataNode);
        testLocalFile.DeleteDataNode(&testDataNode);
        testLocalFile.nodesFile.close();

        testLocalFile.OpenFileToRead("testLocalFile");
        testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
        while (testNextNode.dataFileOffset)
        {
            ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);
            PrintEgDataNodeTypeOffsets(testNextNode);
            testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
        }
    }

    testLocalFile.nodesFile.close();

    */

    // std::cout << "nextNodeOffset: 0x" << hex << testDataNode.nextNodeOffset << std::endl;

    /*uint64_t nodeOffset =  testLocalFile.nodesFile.getFileSize();
    std::cout << "WriteDataNode() nodeOffset: " << nodeOffset << std::endl;
    testLocalFile.nodesFile.fileStream.seekp(nodeOffset);
    testLocalFile.nodesFile.writeInt(1); */

    /* testLocalFile.nodesFile.close();
    testLocalFile.nodesFile.openToUpdate();
    testLocalFile.nodesFile.seekWrite(0); */

    return 0;
}
