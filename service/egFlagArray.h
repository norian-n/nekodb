#pragma once

#include "../metainfo/egCoreTypes.h"

class EgFlagArray {
public:
    EgByteType flagsByte {0};
    EgByteType bitmask   {0};

    EgFlagArray()  {}
    ~EgFlagArray() {}

    bool getFlag(int flagIndex) { 
        bitmask = 1 << flagIndex; 
        return flagsByte & bitmask; 
    }

    void setFlag(bool flag, int flagIndex) { 
        bitmask = 1 << flagIndex; 
        if (flag) 
            flagsByte |= bitmask;
        else
            flagsByte &= ~bitmask;        
    }

};