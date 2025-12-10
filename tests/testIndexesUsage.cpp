#include "../metainfo/egDatabaseType.h"
#include <iostream>
#include <cstring>

using namespace std;

bool initDatabase(EgDatabaseType& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("indexops");

    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");

    graphDB.AddIndex("x", 4, egIndexesSpace::egIntFT);
    graphDB.AddIndex("y", 4, egIndexesSpace::egIntFT);

    graphDB.CommitNodeBlueprint();

    // graphDB.AddLinkType("basicops_linktype"); // , "locations", "locations"); // create link type

    return true; // ((graphDB.nodesTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
        // &&  (graphDB.linksTypesMetainfo.nodesContainer-> dataNodes.size() == 1));
}

string field1 = "name\0";
// const char* field2 = "status\0";
// string field3("image type");
// string field4("100");
// string field5("200");
int num1 = 0xab;
int num2 = 0xcd;

inline void addSampleDataNode(EgDataNodesType& dataNodes, int x, int y) {
    EgDataNodeType* newNode = new EgDataNodeType(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    // *newNode << field2;
    // *newNode << field3;
    *newNode << x;
    *newNode << y;
    // dataNodes.AddDataNode(newNode);
    // PrintEgDataNodeTypeFields(*newNode);
    dataNodes << newNode;
}

bool testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes)
{
    addSampleDataNode(testDataNodes, 0xa1c1, 0xb1d1);
    addSampleDataNode(testDataNodes, 0xa2c2, 0xb2d2);
    addSampleDataNode(testDataNodes, 0xa3c3, 0xb3d3);
    
    addSampleDataNode(testDataNodes, 0xa4c4, 0xb4d4);
    addSampleDataNode(testDataNodes, 0xa5c5, 0xb5d5);
    addSampleDataNode(testDataNodes, 0xa6c6, 0xb6d6);
    
    addSampleDataNode(testDataNodes, 0xa7c7, 0xb7d7);
    addSampleDataNode(testDataNodes, 0xa8c8, 0xb8d8);
    addSampleDataNode(testDataNodes, 0xa9c9, 0xb9d9);

    testDataNodes.DeleteDataNode(2);

    testDataNodes.MarkUpdatedDataNode(3);

    testDataNodes.Store();
    EgByteArraySysallocType fieldValue(4);
    fieldValue << (int) 0xa7c7;
    testDataNodes.LoadNodesEQ("x", fieldValue);   
/*
    cout << "dataNodes: " << testDataNodes.nodesContainer-> dataNodes.size() << endl;
    cout << "addedDataNodes: " << testDataNodes.nodesContainer-> addedDataNodes.size() << endl;
    cout << "deletedDataNodes: " << testDataNodes.nodesContainer-> deletedDataNodes.size() << endl;
    cout << "updatedDataNodes: " << testDataNodes.nodesContainer-> updatedDataNodes.size() << endl;
*/
    bool res =  ((testDataNodes.nodesContainer-> addedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> dataNodes.size() == 1)
            && (testDataNodes.nodesContainer-> deletedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> updatedDataNodes.size() == 0));

    return res;
}

bool reloadData(EgDataNodesType& testDataNodes) {
    // cout << "===== reloadData() " << " =====" << endl;

    testDataNodes.LoadAllNodes();
        // delete bad ID node
    testDataNodes.DeleteDataNode(2);
        // delete new node
    //addSampleDataNode(testDataNodes);
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
    std::remove("indexops.dnl"); // delete layout file
    std::remove("indexops.gdn"); // delete data nodes file

    EgDatabaseType  graphDB;
    EgDataNodesType indexOpsNodesType;

    cout << "===== Test indexes ops " << " =====" << endl;

    bool res = initDatabase(graphDB);

    // cout << "===== initDatabase() res: " << res << " =====" << endl;

    int result = indexOpsNodesType.Connect("indexops", graphDB);

    // cout << "===== After connect result: " << std::dec << result << " =====" << endl;

    // PrintDataNodeLayout(*(locationsNodesType.dataNodeBlueprint));
    bool res1 = testEgDataNodesTypeBasicNodeOps(indexOpsNodesType);
    // bool res2 = false; // reloadData(indexOpsNodesType);

    if (res1) // && res2)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}