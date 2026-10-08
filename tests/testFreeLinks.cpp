#include "testTemplate.h"

#include "../guisupport/egStdLayers.h"
#include "../guisupport/egOneLayer.h"
#include "../nodes/egDataNodesSet.h"
#include "../guisupport/egLiteralsGuisupport.h"
#include "../links/egFreeLinksSet.h"
#include "../metainfo/egLiteralsMetainfo.h"

using namespace std;

bool initNodesSets(EgDatabase& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    /* graphDB.CreateNodeBlueprint("testNodesBlueprint"); // , typeSettings);
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");
    graphDB.AddNodeDataField("z");
    graphDB.CommitNodeBlueprint(); */

    graphDB.CreateLinkBlueprint("testLinksBlueprint");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitBoundLinkBlueprint();

    // graphDB.CreateNodesSetByBlueprint("layer2nodes", "testBlueprint");

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

inline void addSampleDataNode(EgDataNodesSet& dataNodes) {
    // PrintDataNodeBlueprint(*(dataNodes.dataNodeBlueprint));
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint); // FIXME change to node(nodesSet);
    (*newNode)[nodeWidgetFieldXName] << 1;
    (*newNode)[nodeWidgetFieldYName] << 2;
    (*newNode)[nodeWidgetFieldWName] << 3;
    (*newNode)[nodeWidgetFieldHName] << 4;
    dataNodes << newNode;
}


int main() {
    cout << "===== Test EgFreeLinksSet =====" << endl;
    std::remove("egdb/testLayersSet_egLayersInfo.dnl"); // delete layout file
    std::remove("egdb/testLayersSet_egLayersInfo.gdn"); // delete data nodes file    

    EgDatabase theDatabase;
    EgStdLayers   testLayers;
    EgFreeLinksSet freeLinksSet;

    theDatabase.CreateFreeLinksSetByBlueprint("testFreeLinks", stdFreeLinkBPName);

    freeLinksSet.ConnectLinks("testFreeLinks", theDatabase);
    freeLinksSet.AddRawLink( 1, 2, "set1", "set2" );

    freeLinksSet.StoreLinks();

    /* initNodesSets(theDatabase); // create testLinksBlueprint

    theDatabase.CreateLayersSet("testLayersSet");
    testLayers.ConnectLayers("testLayersSet", theDatabase);

    theDatabase.CreateNodesSetByBlueprint("topLayerNodes", nodeWidgetBPName); // "testNodesBlueprint");
    theDatabase.CreateBoundLinksSetByBlueprint("topLayerLinks", "testLinksBlueprint", "topLayerNodes", "topLayerNodes");

    EgDataNodeIDType topLayerID;
    testLayers.createBlankLayer(topLayerID, 0, 1000, 600, "topLayerNodes", "topLayerLinks"); // "layer1links");  // create top layer
    
    EgDataNodesSet topLayerNodesSet;
    topLayerNodesSet.Connect("topLayerNodes", theDatabase);

    addSampleDataNode(topLayerNodesSet); // add 3 nodes to top layer
    addSampleDataNode(topLayerNodesSet);
    EgDataNodeIDType newNodeID = topLayerNodesSet.getAddedNodeID();

    // cout << "top LayerID: " << topLayerID << " top newNodeID: " << newNodeID << endl;

    // add details layers to 2 nodes
    EgDataNodeIDType newLayerID;
    testLayers.createDetailsLayer(newNodeID, newLayerID, topLayerID, 100, 100, "egDetailsNodesSet", nodeWidgetBPName, "testLinksBlueprint");
    addSampleDataNode(topLayerNodesSet);
    newNodeID = topLayerNodesSet.getAddedNodeID();
    testLayers.createDetailsLayer(newNodeID, newLayerID, topLayerID, 100, 100, "egDetailsNodesSet", nodeWidgetBPName, "testLinksBlueprint");

    testLayers.updateWH(topLayerID, 111, 222);

    std::string detLayerName = "egDetailsNodesSet_" + std::to_string(newNodeID); // details of new node

    EgDataNodesSet detailsLayerNodesSet;
    detailsLayerNodesSet.Connect(detLayerName, theDatabase);

    addSampleDataNode(detailsLayerNodesSet); // add 2 nodes to detail layer
    addSampleDataNode(detailsLayerNodesSet);
    newNodeID = detailsLayerNodesSet.getAddedNodeID();

    // cout << "details LayerID: " << newLayerID << " details newNodeID: " << newNodeID << endl;

    // testLayers.updateWH(topLayerID, 111, 222);

    testLayers.StoreLayers();
    testLayers.LoadLayers(); */
    
    // cout << "top Layer W: " << testLayers[topLayerID]-> layerWidth << " H: " << testLayers[topLayerID]-> layerHeight << endl;

    // check nodes count in details 
    // cout << "detailsLayerNodesSet.nodesContainer-> nodesCount: " << detailsLayerNodesSet.nodesContainer-> nodesCount << endl;

    // testLayers.layersStorage.nodesContainer-> PrintDataNodesContainer();
    // cout << "testLayers.layersStorage.nodesContainer-> nodesCount: " << testLayers.layersStorage.nodesContainer-> nodesCount << endl;

    contributeTestResultToStatistics( testInDevelopment); // testLayers.layersSetStorage.nodesContainer-> nodesCount == 3 );
}