#include "../indexes/egIndexes.h"
// #include "../indexes/egIndexes.cpp"
// #include "../indexes/egFingers.cpp"
#include <iostream>
// #include <cstring>
using namespace std;

EgIndexes<uint32_t> testIndexes("testIndexes");

bool testAddIndex(uint32_t theKey, uint64_t dataOffset) {
    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();
    byteArray-> arrayData = (ByteType*) &theKey;                     // no alloc, ptr to global mem
    byteArray-> dataSize  = sizeof(theKey);
    return testIndexes.AddNewIndex(*byteArray, dataOffset);
}

bool testAddFirstChunk() {
    return testAddIndex(0x55, 0xabcdef) && testAddIndex(0x66, 0x12345678) && testAddIndex(0x33, 0x112233aabb) && testAddIndex(0x44, 0xcdef4455);
}

bool testUpdateDataOffset(uint32_t theKey, uint64_t oldDataOffset, uint64_t newDataOffset) {
    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();
    byteArray-> arrayData = (ByteType*) &theKey;                     // no alloc, ptr to global mem
    byteArray-> dataSize  = sizeof(theKey);
    return testIndexes.UpdateDataOffset(*byteArray, oldDataOffset, newDataOffset);
}

bool testDeleteIndex(uint32_t theKey, uint64_t dataOffset) {
    EgByteArrayAbstractType* byteArray = new EgByteArrayAbstractType();
    byteArray-> arrayData = (ByteType*) &theKey;                     // no alloc, ptr to global mem
    byteArray-> dataSize  = sizeof(theKey);    
    return testIndexes.DeleteIndex(*byteArray, dataOffset);
}

int main() {
    cout << "===== Test EgIndexes " << " =====" << endl;
    std::remove("testIndexes.fng"); // delete files
    std::remove("testIndexes.ind"); // delete files

    if ( testAddFirstChunk() && testAddIndex(0x35, 0x11111111)
                             && testUpdateDataOffset(0x44, 0xcdef4455, 0x222222)
                             && testDeleteIndex(0x55, 0xabcdef)
                             && testDeleteIndex(0x66, 0x12345678) )
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}