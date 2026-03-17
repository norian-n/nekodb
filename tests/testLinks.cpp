#include "../links/egLinks.h"
#include "../nodes/egDataNodesSet.h"
#include <iostream>
#include <cstring>

using namespace std;

string field1 {"111111"};
int field2 = 100;
int field3 = 200;
 
inline void addSampleDataNode(EgDataNodesSet& dataNodes) {
    EgDataNode* newNode = new EgDataNode(dataNodes.dataNodeBlueprint);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    dataNodes << newNode;
}

bool testLinksResolving(EgLinksSet& testLinks, EgDataNodesSet& fromType, EgDataNodesSet& toType) {
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

bool testLinksStorage(EgLinksSet& testLinks) {

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

void initDatabase(EgDatabase& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodesSet("testLinksFrom"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodesSet();

    graphDB.CreateNodesSet("testLinksTo"); // , typeSettings);
    graphDB.AddNodeDataField("name");
    graphDB.AddNodeDataField("x");
    graphDB.AddNodeDataField("y");    
    graphDB.CommitNodesSet();

    graphDB.CreateLinkWithDataBlueprint("testBoundLinksWData");
    graphDB.AddLinkDataField("x");
    graphDB.AddLinkDataField("y");
    graphDB.CommitLinkBlueprint();

    graphDB.CreateLinksSetByBlueprint("testBoundLinksWData", "testBoundLinksWData", "testLinksFrom", "testLinksTo");
}

int main() {
    cout << "===== Test EgLinksType =====" << endl;
    std::remove("testBoundLinksWData_egArrowLink.dnl"); // delete layout file
    std::remove("testBoundLinksWData_egArrowLink.gdn"); // delete data nodes file       

    EgDatabase theDatabase;
    EgDataNodesSet fromType;
    EgDataNodesSet toType;
    EgLinksSet testLinks;

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