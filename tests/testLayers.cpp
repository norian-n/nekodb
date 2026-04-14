#include "testTemplate.h"

#include "../metainfo/egLayers.h"
#include "../metainfo/egOneLayer.h"
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
    graphDB.AddNodeDataField("");
    graphDB.CommitNodeBlueprint();

    graphDB.CreateLinkWithDataBlueprint("testLinksBlueprint");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitLinkBlueprint();


    // graphDB.CreateNodesSetByBlueprint("layer2nodes", "testBlueprint");

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

inline void addSampleDataNode(EgDataNodesSet& dataNodes) {
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint); // FIXME change to node(nodesSet);
    (*newNode)["x"] << 1;
    (*newNode)["y"] << 2;
    (*newNode)["z"] << 3;
    dataNodes << newNode;
}


int main() {
    cout << "===== Test EgLayersType =====" << endl;
    std::remove("egdb/testlayers_egLayersInfo.dnl"); // delete layout file
    std::remove("egdb/testlayers_egLayersInfo.gdn"); // delete data nodes file    

    EgDatabase theDatabase;
    EgLayers   testLayers;

    initNodesSets(theDatabase); // create testBlueprint

    theDatabase.CreateLayersSet("testLayersSet");
    testLayers.ConnectLayers("testLayersSet", theDatabase);

    EgDataNodeIDType topLayerID;
    theDatabase.CreateNodesSetByBlueprint("topLayerNodes", "testNodesBlueprint");
    theDatabase.CreateLinksSetByBlueprint("topLayerLinks", "testLinksBlueprint", "topLayerNodes", "topLayerNodes");
    testLayers.createBlankLayer(topLayerID, 0, 1000, 600, "topLayerNodes", "topLayerLinks"); // "layer1links");  // create top layer
    
    EgDataNodesSet topLayerNodesSet;
    topLayerNodesSet.Connect("topLayerNodes", theDatabase);

    addSampleDataNode(topLayerNodesSet); // add 3 nodes to top layer
    addSampleDataNode(topLayerNodesSet);
    EgDataNodeIDType newNodeID = topLayerNodesSet.getAddedNodeID();

    // cout << "top LayerID: " << topLayerID << " top newNodeID: " << newNodeID << endl;

    // add details layers to 2 nodes
    EgDataNodeIDType newLayerID;
    testLayers.createDetailsLayer(newNodeID, newLayerID, topLayerID, 100, 100, "egDetailsNodesSet", "testNodesBlueprint", "testLinksBlueprint");
    addSampleDataNode(topLayerNodesSet);
    newNodeID = topLayerNodesSet.getAddedNodeID();
    testLayers.createDetailsLayer(newNodeID, newLayerID, topLayerID, 100, 100, "egDetailsNodesSet", "testNodesBlueprint", "testLinksBlueprint");

    std::string detLayerName = "egDetailsNodesSet_" + std::to_string(newNodeID); // details of new node

    EgDataNodesSet detailsLayerNodesSet;
    detailsLayerNodesSet.Connect(detLayerName, theDatabase);

    addSampleDataNode(detailsLayerNodesSet); // add 2 nodes to detail layer
    addSampleDataNode(detailsLayerNodesSet);
    newNodeID = detailsLayerNodesSet.getAddedNodeID();

    // cout << "details LayerID: " << newLayerID << " details newNodeID: " << newNodeID << endl;

    testLayers.updateWH(topLayerID, 111, 222);

    testLayers.StoreLayers();
    testLayers.LoadLayers();
    
    // cout << "top Layer W: " << testLayers[topLayerID]-> layerWidth << " H: " << testLayers[topLayerID]-> layerHeight << endl;

    // check nodes count in details 
    // cout << "detailsLayerNodesSet.nodesContainer-> nodesCount: " << detailsLayerNodesSet.nodesContainer-> nodesCount << endl;

    // testLayers.layersStorage.nodesContainer-> PrintDataNodesContainer();
    // cout << "testLayers.layersStorage.nodesContainer-> nodesCount: " << testLayers.layersStorage.nodesContainer-> nodesCount << endl;

    contributeTestResultToStatistics( (detailsLayerNodesSet.nodesContainer-> nodesCount == 2) ); // testInDevelopment
}