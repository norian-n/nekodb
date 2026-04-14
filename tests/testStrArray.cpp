#include "testTemplate.h"

#include "../service/egStrArray.h"

using namespace std;

int main() {
    std::cout << "=============== Test StrArray ================COUNTME" << std::endl;

    EgStrArray testStrArray(3);
    string testStr {"test string "};
    EgStrArray outStrArray(3);
    string outStr;

    EgHamSlicerType     hamSlicer;
    EgByteArraySlicerType byteArray ( hamSlicer, 0 );

    testStrArray.insertStr(testStr + "1", 0);
    testStrArray.insertStr("", 1);
    testStrArray.insertStr(testStr + "003", 2);

    testStrArray.storeToByteArray(byteArray);
    // PrintByteArray(byteArray, false);
    outStrArray.loadFromByteArray(byteArray);

    // testStrArray.init();
    // testStrArray[0] << testStr;

    // outStrArray[0] >> outStr;
    // cout << "outStr 0: " << outStr << endl;
    // outStrArray[1] >> outStr;
    // cout << "outStr 1: " << outStr << endl;
    outStrArray[2] >> outStr;
    // cout << "outStr 2: " << outStr << endl;

    contributeTestResultToStatistics(testStr + "003" == outStr); // testInDevelopment); 
}