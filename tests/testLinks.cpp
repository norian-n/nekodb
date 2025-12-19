#include "../links/egLinksType.h"
#include "../nodes/egDataNodesType.h"
#include <iostream>
#include <cstring>

using namespace std;

string field1 = "111111\0";
int field2 = 100;
int field3 = 200;
 
inline void addSampleDataNode(EgDataNodesType& dataNodes) {
    EgDataNodeType* newNode = new EgDataNodeType(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    dataNodes << newNode;
}

bool testLinksResolving(EgLinksType& testLinks, EgDataNodesType& fromType, EgDataNodesType& toType) {
    addSampleDataNode(fromType);  // nodeID == 1
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);

    addSampleDataNode(toType);
    addSampleDataNode(toType);    // nodeID == 2
    addSampleDataNode(toType);
    addSampleDataNode(toType);

    // testLinks.ConnectLinkToNodesTypes(fromType, toType);
    testLinks.ResolveNodesIDsToPtrs(fromType, toType);
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

bool testLinksStorage(EgLinksType& testLinks) {

    testLinks.AddRawLink(1, 2); // nodeID == 1, outLinks[0], 3 items
    testLinks.AddRawLink(1, 3);
    testLinks.AddRawLink(1, 4);

    testLinks.AddRawLink(3, 4);

    testLinks.StoreLinks();
    testLinks.LoadLinks();

    // PrintDataNodesContainer(*(testLinks.linksStorage));

    int count = testLinks.dataMap.size();
/*
    cout << "testLinksStorage() testLinks.dataMap.size(): " << count << endl;
    cout << "testLinksStorage() testLinks.dataMap addr: " << std::hex << (int64_t) &(testLinks.dataMap)
         << " container map addr: " << (int64_t) &(testLinks.linksDataStorage.nodesContainer-> dataNodes)
         << " fake map addr: " << (int64_t) &(testLinks.linksDataStorage.fDataMap) << std::dec << endl;
*/
    return (count == 4);
}

void initDatabase(EgDatabaseType& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeBlueprint("testLinksFrom"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodeBlueprint();

    graphDB.CreateNodeBlueprint("testLinksTo"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodeBlueprint();

    graphDB.CreateLinkWithDataBlueprint("testBoundLinksWData", "testLinksFrom", "testLinksTo");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitLinkBlueprint();
}

int main() {
    cout << "===== Test EgLinksType =====" << endl;
    std::remove("testLinks_arrowLinks.dnl"); // delete layout file
    std::remove("testLinks_arrowLinks.gdn"); // delete data nodes file       

    EgDatabaseType theDatabase;
    EgDataNodesType fromType;
    EgDataNodesType toType;
    EgLinksType testLinks;

    initDatabase(theDatabase);

    fromType.Connect("testLinksFrom", theDatabase);
    toType.Connect  ("testLinksTo", theDatabase);
    testLinks.ConnectLinks("testBoundLinksWData", theDatabase);
    // std::remove("testNodes.gdn");  // delete data nodes file

    bool testStor   = testLinksStorage(testLinks);
    bool testResolv = testLinksResolving(testLinks, fromType, toType);

    if (testStor && testResolv)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}