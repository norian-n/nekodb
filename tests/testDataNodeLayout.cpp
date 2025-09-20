#include "../nodes/egDataNodeBlueprint.h"
#include <iostream>
#include <cstring>
using namespace std;


bool testDataNodeBlueprint() {

    cout << "===== Test DataNodeBlueprint (2 errors must appear in negative tests) " << " =====" << endl;

    EgDataNodeBlueprintType testBlueprint("testNodes");
    EgIndexSettingsType indexSettings;

    std::remove("testNodes.dnl"); // delete file

    testBlueprint.BlueprintInitStart();

    testBlueprint.AddDataFieldName("field_1");
    testBlueprint.AddDataFieldName("testField 2");
    testBlueprint.AddDataFieldName("field 3");

    testBlueprint.blueprintSettings.useEntryNodes         = true;
    testBlueprint.blueprintSettings.useGUIsettings        = true;
    testBlueprint.blueprintSettings.useLinks              = true;
    testBlueprint.blueprintSettings.useNamedAttributes    = true;
    testBlueprint.blueprintSettings.useVisualSpace        = true;

    testBlueprint.BlueprintInitCommit();
    testBlueprint.AddDataFieldName("try to add field after commit");

    indexSettings.indexFamilyType   = egIntFT;
    indexSettings.indexSizeBits     = 32;
    testBlueprint.AddIndex("field_1",  indexSettings);

    indexSettings.indexFamilyType   = egHashFT;
    indexSettings.indexSizeBits     = 64;
    indexSettings.hashFunctionID    = 2;
    testBlueprint.AddIndex("field 3",  indexSettings);

    testBlueprint.AddIndex("bad index name", indexSettings);
    
    testBlueprint.LocalStoreBlueprint();
    int res = testBlueprint.LocalLoadBlueprint();
    // PrintDataNodeBlueprint(testBlueprint);

    return (res == 0);
}

int main() {
    if (testDataNodeBlueprint())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;    
}