#include "../indexes/egFingers.h"
#include "../indexes/egFingers.cpp"
#include <iostream>
#include <cstring>
using namespace std;

EgIndexes<uint32_t> testIndexes("testFingersIndexes");
EgFingers<uint32_t> testFingers("testFingers", &testIndexes);

bool testAddRootFinger() {
    // cout << "===== Test EgFingers 1 " << " =====" << endl;
    uint32_t theKey {0x55};
    return testFingers.AddNewRootFinger(theKey, 0x55cc55bb55aa); ;
}

bool testAddFinger(uint32_t theKey, uint64_t theNextOffset, bool swapFingers = false) {
    bool res = testFingers.FindIndexesChunkToInsert(theKey);
    // cout << "res(FindIndexesChunkToInsert) = " << res << endl;
    if (res) {
        // forge new finger
        testFingers.newFinger.minKey = theKey;
        testFingers.newFinger.maxKey = theKey;
        testFingers.newFinger.myChunkIsLeaf = 0xff; // isLeaf == true
        // testFingers.newFinger.itemsCount = 0xa1;
        testFingers.newFinger.nextChunkOffset = theNextOffset; // egIndexesNamespace::indexHeaderSize;
        testFingers.newFinger.myChunkOffset = testFingers.currentFinger.myChunkOffset;
        // testFingers.newFinger.myPosInChunkBytes = posInChunk * testFingers.oneFingerSize; // testFingers.oneFingerSize * N
        if (swapFingers)
            testFingers.SwapFingers();
        res = res && testFingers.AddNewUpdateCurrentFinger();
        // cout << "res (AddNewUpdateCurrentFinger) = " << res << endl;
    }
    return res;
}

bool testDelFingerByChain(uint32_t theKey) {
    bool res2 {true};
    bool res1 = testFingers.FindIndexChunkEQ(theKey);
    if (res1)
        res2 = testFingers.DeleteCurrentFingerByChain();
    return res1 && res2;
}

bool testUpdFingerByPtr(uint32_t oldKey, uint32_t newKey) {
    bool res2 {true};
    bool res1 = testFingers.FindIndexChunkEQ(oldKey);
    if (res1) {
        // testFingers.theKey = newKey;
        testFingers.anyMinMaxChanged = true;
        testFingers.currentFinger.minKey = newKey;
        testFingers.currentFinger.maxKey = newKey;
        res2 = testFingers.UpdateFingersByBackptrs();
    }
    return res1 && res2;
}

bool testDelFingerByPtr(uint32_t theKey) {
    bool res2 {true};
    bool res1 = testFingers.FindIndexChunkEQ(theKey);
    if (res1) {
        // testFingers.theKey = theKey;
        res2 = testFingers.DeleteFingerByBackptrs();
    }
    return res1 && res2;
}

int main() {
    cout << "===== Test EgFingers Stage 1" << " =====" << endl;
    std::remove("egdb/testFingres.fng"); // delete file
    if (testAddRootFinger() && testAddFinger(0x66, 0x66cc55bb44aa)
                            && testAddFinger(0x10, 0x12cc11bb10aa, true) // swap fingers cause key is minimal
                            && testAddFinger(0x58, 0x58cc57bb56aa)
                            // && testAddFinger(0x77, 0x79cc78bb77aa))
                            && testAddFinger(0x22, 0x24cc23bb22aa))
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
    // std::remove("egdb/testFingres.fng"); // delete file
    // testFingers.rootFingerIsLoaded = false; // reset()
    cout << "===== Test EgFingers Stage 2" << " =====" << endl;
    if (testDelFingerByChain(0x58)  && testUpdFingerByPtr(0x55, 0x57) && testDelFingerByPtr(0x66))
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}