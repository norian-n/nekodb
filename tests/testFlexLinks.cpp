#include "../links/egFlexLinksType.h"
#include "../nodes/egDataNodesType.h"
#include <iostream>
#include <cstring>

using namespace std;

string field1 = "111111\0";
int field2 = 100;
int field3 = 200;
 
inline void addSampleDataNode(EgDataNodesType& dataNodes) {
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    dataNodes << newNode;
}

bool testLinksResolving(EgFreeLinksType& testLinks, EgDataNodesType& fromType, EgDataNodesType& toType) {
    addSampleDataNode(fromType);  // nodeID == 1
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);

    addSampleDataNode(toType);
    addSampleDataNode(toType);    // nodeID == 2
    addSampleDataNode(toType);
    addSampleDataNode(toType);

    // testLinks.ConnectLinkToNodesTypes(fromType, toType);
    testLinks.ResolveNodesIDsToPtrs();
/*
    for (auto iter : fromType.nodesContainer-> dataNodes)
        PrintEgDataNodeTypeFields(fromType[iter.first]);

    for (auto iter : fromType.nodesContainer-> dataNodes)
        PrintResolvedLinks(*(iter.second));

    for (auto iter : toType.nodesContainer-> dataNodes)
        PrintResolvedLinks(*(iter.second));
*/
    auto linksIterFrom = fromType[1].outLinks.begin();  // nodeID == 1, outLinks[0], 3 items
    auto linksIterTo   = toType[2].inLinks.begin();     // nodeID == 2, inLinks[0], 1 item

    // cout << linksIterFrom->second.size() << endl;
    // cout << linksIterTo->second.size() << endl;

    return ((linksIterFrom->second.size() == 3) && (linksIterTo->second.size() == 1));
}

bool testLinksStorage(EgFreeLinksType& testLinks, EgBlueprintIDType fromNodesTypeID, EgBlueprintIDType toNodesTypeID) {

    testLinks.AddRawLink(fromNodesTypeID, 1, toNodesTypeID, 2); // nodeID == 1, outLinks[0], 3 items
    testLinks.AddRawLink(fromNodesTypeID, 1, toNodesTypeID, 3);
    testLinks.AddRawLink(fromNodesTypeID, 1, toNodesTypeID, 4);

    testLinks.AddRawLink(fromNodesTypeID, 3, toNodesTypeID, 4);

    testLinks.StoreLinks();
    testLinks.LoadLinks();

    // PrintDataNodesContainer(*(testLinks.linksStorage));

    int count = testLinks.linksStorage-> dataNodes.size();

    return (count == 4);
}

void initDatabase(EgDatabaseType& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("testFreeLinksFrom"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodeBlueprint();

    graphDB.CreateNodeBlueprint("testFreeLinksTo"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodeBlueprint();

    graphDB.CreateFreeLinkWithDataBlueprint("testFreeLinksWData");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitLinkBlueprint();
}

int main() {
    cout << "===== Test EgFreeLinksType =====" << endl;
    std::remove("testLinks.gdn");   // delete file

    EgDatabaseType theDatabase;
    EgDataNodesType fromType;
    EgDataNodesType toType;

    initDatabase(theDatabase);

    fromType.Connect("testFreeLinksFrom", theDatabase);
    toType.Connect  ("testFreeLinksTo", theDatabase);

    std::string name("testFreeLinksWData");

    EgFreeLinksType testLinks(name, theDatabase);
    // testLinks.Connect(theDatabase);
    // std::remove("testNodes.gdn");  // delete data nodes file

    EgBlueprintIDType fromNodesTypeID {0};
    if (theDatabase.nodeTypeIDByName(fromType.dataNodesName, fromNodesTypeID)) // reverse return logic - true if not found
        cout << "fromNodesTypeID not found in metainfo" << endl;

    EgBlueprintIDType toNodesTypeID {0};
    if (theDatabase.nodeTypeIDByName(toType.dataNodesName, toNodesTypeID)) // reverse return logic - true if not found
        cout << "toNodesTypeID not found in metainfo" << endl;

    bool testStor   = testLinksStorage(testLinks, fromNodesTypeID, toNodesTypeID);
    bool testResolv = testLinksResolving(testLinks, fromType, toType);

    if (testStor && testResolv)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}