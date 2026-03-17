#include "../metainfo/egDatabase.h"
#include <iostream>
#include <cstring>

using namespace std;

bool initNodesSets(EgDatabase& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("testBlueprint"); // , typeSettings);

    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("status");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");
    graphDB.AddNodeDataField("z");

    graphDB.CommitNodeBlueprint();

    graphDB.CreateNodesSetByBlueprint("testNodesSet1", "testBlueprint");
    graphDB.CreateNodesSetByBlueprint("testNodesSet2", "testBlueprint");

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

string field1 = "name";
const string field2 {"status"};
int num1 = 100;
int num2 = 200;
int num3 = 300;

inline void addSampleDataNode(EgDataNodesSet& dataNodes) {
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    *newNode << field2;
    *newNode << num1;
    *newNode << num2;
    *newNode << num3;
    dataNodes << newNode;
}

bool testEgDataNodesTypeBasicNodeOps(EgDataNodesSet& testDataNodes)
{
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    testDataNodes.Store();
    return (testDataNodes.nodesContainer-> dataNodes.size() == 3);
}

bool reloadData(EgDataNodesSet& testDataNodes) {
    // cout << "===== reloadData() " << " =====" << endl;

    testDataNodes.LoadAllNodes();
        // delete bad ID node
    testDataNodes.DeleteDataNode(2);
        // delete new node
    addSampleDataNode(testDataNodes);
    testDataNodes.DeleteDataNode(testDataNodes.nodesContainer-> lastNodeID);
        // delete real node
    testDataNodes.DeleteDataNode(3);
    
    testDataNodes.MarkUpdatedDataNode(4);
/*    
    cout << "dataNodes: " << testDataNodes.nodesContainer-> dataNodes.size() << endl;
    cout << "addedDataNodes: " << testDataNodes.nodesContainer-> addedDataNodes.size() << endl;
    cout << "deletedDataNodes: " << testDataNodes.nodesContainer-> deletedDataNodes.size() << endl;
    cout << "updatedDataNodes: " << testDataNodes.nodesContainer-> updatedDataNodes.size() << endl;
*/
    bool res = ((testDataNodes.nodesContainer-> addedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> dataNodes.size() == 7)
            && (testDataNodes.nodesContainer-> deletedDataNodes.size() == 1)
            && (testDataNodes.nodesContainer-> updatedDataNodes.size() == 1));

    return res;
}


int main() {
    std::remove("testBlueprint.dnl");
    std::remove("testNodesSet1.gdn");
    std::remove("testNodesSet2.gdn");

    EgDatabase  graphDB;
    EgDataNodesSet nodesSet1;
    EgDataNodesSet nodesSet2;

    cout << "===== Test nodes set by blueprint " << " =====" << endl;

    initNodesSets(graphDB);

    nodesSet1.Connect("testNodesSet1", graphDB);
    nodesSet2.Connect("testNodesSet2", graphDB);
    // cout << "===== After connect result: " << std::dec << result << " =====" << endl;

    bool res1 = testEgDataNodesTypeBasicNodeOps(nodesSet1);
    bool res2 = testEgDataNodesTypeBasicNodeOps(nodesSet2);

    if (res1 && res2)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}