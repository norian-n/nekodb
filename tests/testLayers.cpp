#include "../metainfo/egLayersType.h"
#include "../nodes/egDataNodesType.h"
#include <iostream>
#include <cstring>

using namespace std;

int main() {
    cout << "===== Test EgLayersType =====" << endl;
    std::remove("testlayers_egLayersInfo.dnl"); // delete layout file
    std::remove("testlayers_egLayersInfo.gdn"); // delete data nodes file    

    EgDatabaseType theDatabase;
    EgLayersType testLayers;

    theDatabase.CreateLayersBlueprint("testlayers");

    testLayers.ConnectLayers("testlayers", theDatabase);

    testLayers.AddNodesType("layer1nodes", 1);
    testLayers.AddNodesType("layer2nodes", 2);
    testLayers.AddLinksType("layer2links", 2);

    testLayers.StoreLayers();
    testLayers.LoadLayers();

    // testLayers.layersStorage.nodesContainer-> PrintDataNodesContainer();
    // cout << "testLayers.layersStorage.nodesContainer-> nodesCount: " << testLayers.layersStorage.nodesContainer-> nodesCount << endl;

    bool res = (testLayers.layersStorage.nodesContainer-> nodesCount == 3);

    if (res)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}