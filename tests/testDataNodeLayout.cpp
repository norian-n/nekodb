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

    testBlueprint.AddIndex("field 3", 4, egIndexesSpace::egIntFT);
    testBlueprint.AddIndex("bad index name", 4, egIndexesSpace::egFloatFT);

    testBlueprint.BlueprintInitCommit();
    testBlueprint.AddDataFieldName("try to add field after commit");
    
    testBlueprint.LocalStoreBlueprint();
    int res = testBlueprint.LocalLoadBlueprint();
    
    PrintDataNodeBlueprint(testBlueprint);

    return (res == 0);
}

int main() {
    if (testDataNodeBlueprint())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;    
}