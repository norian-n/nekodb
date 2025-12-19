#include "../metainfo/egDatabaseType.h"
#include <iostream>
#include <cstring>
using namespace std;


bool testDatabase() {
    EgDatabaseType testDatabase;
    cout << "===== Test EgDatabaseType " << " =====" << endl;

    string str1("nodesType1");
    testDatabase.AddDataNodesTypeInfo(1, str1);

    string str2("linksType1");
    testDatabase.AddLinksTypeInfo(str2);

    testDatabase.StoreDataNodesTypesInfo();
    testDatabase.LoadDataNodesTypesInfo();

    return ((testDatabase.nodesTypesStorage-> dataNodes.size() == 1) 
        &&  (testDatabase.linksTypesStorage-> dataNodes.size() == 1));
}

int main() {
    std::remove("nodesTypesStorageLayout.gdn"); // delete file
    std::remove("linksTypesStorageLayout.gdn"); // delete file

    if (testDatabase())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;    
}