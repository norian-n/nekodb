#include "../metainfo/egDatabaseType.h"
#include <iostream>
#include <cstring>

using namespace std;

bool initDatabase(EgDatabaseType& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("basicops"); // , typeSettings);

    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("status"); // , isIndexed create index
    graphDB.AddNodeDataField("imageType");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    

    graphDB.CommitNodeBlueprint();

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

string field1 = "name\0";
const char* field2 = "status\0";
string field3("image type");
string field4("100");
string field5("200");
int num1 = 100;
int num2 = 200;

inline void addSampleDataNode(EgDataNodesType& dataNodes) {
    EgDataNodeType* newNode = new EgDataNodeType(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    *newNode << num1;
    *newNode << num2;
    // dataNodes.AddDataNode(newNode);
    // PrintEgDataNodeTypeFields(*newNode);
    dataNodes << newNode;
}

bool testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes)
{
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);

    testDataNodes.DeleteDataNode(2);

    testDataNodes.MarkUpdatedDataNode(3);
/*
    cout << "dataNodes: " << testDataNodes.nodesContainer-> dataNodes.size() << endl;
    cout << "addedDataNodes: " << testDataNodes.nodesContainer-> addedDataNodes.size() << endl;
    cout << "deletedDataNodes: " << testDataNodes.nodesContainer-> deletedDataNodes.size() << endl;
    cout << "updatedDataNodes: " << testDataNodes.nodesContainer-> updatedDataNodes.size() << endl;
*/
    bool res = ((testDataNodes.nodesContainer-> addedDataNodes.size() == 8)
            && (testDataNodes.nodesContainer-> dataNodes.size() == 8)
            && (testDataNodes.nodesContainer-> deletedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> updatedDataNodes.size() == 0));

    testDataNodes.Store();

    // testDataNodes.nodesContainer->LocalNodesFile-> PrintNodesChain();

    // testShowResult(res, FNS);

    return res;
}

bool reloadData(EgDataNodesType& testDataNodes) {
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
    std::remove("basicops.dnl"); // delete layout file
    std::remove("basicops.gdn"); // delete data nodes file

    EgDatabaseType  graphDB;
    EgDataNodesType basicopsNodesType;

    cout << "===== Test basic database ops " << " =====" << endl;

    bool res = initDatabase(graphDB);

    // cout << "===== initDatabase() res: " << res << " =====" << endl;

    int result = basicopsNodesType.Connect("basicops", graphDB);

    // cout << "===== After connect result: " << std::dec << result << " =====" << endl;

    // PrintDataNodeLayout(*(locationsNodesType.dataNodeBlueprint));
    bool res1 = testEgDataNodesTypeBasicNodeOps(basicopsNodesType);
    bool res2 = reloadData(basicopsNodesType);

    if (res1 && res2)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}