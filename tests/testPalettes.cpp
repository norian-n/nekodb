#include "testTemplate.h"

#include "../guisupport/egPalettes.h"
#include "../guisupport/egOnePalette.h"
#include "../nodes/egDataNodesSet.h"

using namespace std;

bool initNodesSets(EgDatabase& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("testNodesBlueprint"); // , typeSettings);
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");
    graphDB.AddNodeDataField("z");
    graphDB.CommitNodeBlueprint();

    graphDB.CreateLinkBlueprint("testLinksBlueprint");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitLinkBlueprint();

    // graphDB.CreateNodesSetByBlueprint("layer2nodes", "testBlueprint");

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

inline void addSampleDataNode(EgDataNodesSet& dataNodes) {
    // PrintDataNodeBlueprint(*(dataNodes.dataNodeBlueprint));
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint); // FIXME change to node(nodesSet);
    (*newNode)["x"] << 1;
    (*newNode)["y"] << 2;
    (*newNode)["z"] << 3;
    dataNodes << newNode;
}


int main() {
    cout << "===== Test palettes =====" << endl;
    // std::remove("egdb/testLayersSet_egLayersInfo.dnl"); // delete layout file
    // std::remove("egdb/testLayersSet_egLayersInfo.gdn"); // delete data nodes file    

    EgDatabase theDatabase;
    EgPalettes testPalettes;

    theDatabase.CreatePalettesSet("testPalettesSet");
    testPalettes.ConnectPalettesSet("testPalettesSet", theDatabase);
    testPalettes.CreateOnePalette("testOnePalette");
    testPalettes.CreateOnePalette("testTwoPalette");

    EgOnePalette testOneP;

    testOneP.ConnectPalette("testOnePalette", theDatabase);
    testOneP.addPaletteWidgetData("testNodesSetName 1", "widgetName 1", 1, 1, 1);
    testOneP.addPaletteWidgetData("testNodesSetName 2", "widgetName 2", 2, 2, 2);

    testOneP.StorePalette();
    testOneP.LoadPalette();

    EgDataNode* testNode {nullptr};
    EgDataNodeBlueprint* newNodeBP {nullptr};
    theDatabase.CherryPickNode("basicops", 1, testNode, newNodeBP); // FIXME STUB
    if (testNode)
        PrintEgDataNodeFields(*testNode);

    delete newNodeBP;
    delete testNode;

/*  
    // cout << "top Layer W: " << testLayers[topLayerID]-> layerWidth << " H: " << testLayers[topLayerID]-> layerHeight << endl;

    // check nodes count in details 
    // cout << "detailsLayerNodesSet.nodesContainer-> nodesCount: " << detailsLayerNodesSet.nodesContainer-> nodesCount << endl;

    // testLayers.layersStorage.nodesContainer-> PrintDataNodesContainer();
    // cout << "testLayers.layersStorage.nodesContainer-> nodesCount: " << testLayers.layersStorage.nodesContainer-> nodesCount << endl;
*/
    contributeTestResultToStatistics( testInDevelopment ); // testLayers.layersStorage.nodesContainer-> nodesCount == 3 )
}