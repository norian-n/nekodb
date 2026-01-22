#pragma once
#include <cstdint>

typedef uint32_t egMaxStreamSizeType;

class EgDataStream {
public:
    egMaxStreamSizeType bufSize  {0};
    egMaxStreamSizeType bufIndex {0};
    uint8_t*   bufData           {nullptr};

    void seek(egMaxStreamSizeType idx) { bufIndex = idx; } 
    inline bool indexOk(egMaxStreamSizeType dataTypeSize) const { return bufIndex + dataTypeSize <= bufSize ;}

    EgDataStream(egMaxStreamSizeType buf_size) :
        bufSize(buf_size), bufData(new uint8_t[buf_size]) {}
    ~EgDataStream() { delete bufData; }

    template <typename T> EgDataStream& operator>>(T&& i) {
        if (indexOk(sizeof(T))) {
            i = reinterpret_cast<T&&>(bufData[bufIndex]);
            bufIndex += sizeof(T);
        }
        return *this;
    }

    template <typename T> EgDataStream& operator<< (T&& i) {
        if (indexOk(sizeof(T))) {
            reinterpret_cast<T&&>(bufData[bufIndex]) = i;
            bufIndex += sizeof(T);
        }
        return *this;
    }

    template <typename T> EgDataStream& operator<< (const T&& i) {
        if (indexOk(sizeof(T))) {
            reinterpret_cast<T&&>(bufData[bufIndex]) = i;
            bufIndex += sizeof(T);
        }
        return *this;
    }    
};