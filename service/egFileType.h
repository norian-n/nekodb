#pragma once
#include <fstream>
#include <filesystem>

#include "../core/egCoreTypes.h"
#include "egDataStream.h"

typedef uint64_t        EgFileOffsetType;

const   uint32_t    streamBufSize {1024};

class EgFileType {
public:
    std::string  fileName;
    std::fstream fileStream;

    egDataStream dataStream;

    EgFileType():dataStream(streamBufSize) {}
    EgFileType(const std::string& a_name): fileName(a_name), dataStream(streamBufSize) {}
    ~EgFileType() { if (fileStream.is_open()) fileStream.close(); }

    inline bool good() { return fileStream.good(); }

    // inline bool checkIfExists()   { fileStream.open(fileName, std::ios::in | std::ios::binary); bool ret = fileStream.is_open(); fileStream.close(); return ret; }
    inline bool checkIfExists()   { return std::filesystem::exists(fileName); }
    inline EgFileOffsetType getFileSize() { fileStream.seekg (0, std::ios::end); return (EgFileOffsetType) fileStream.tellg(); }

    inline bool openToRead()   { if (fileStream.is_open()) return true; fileStream.open(fileName, std::ios::in  | std::ios::binary); return fileStream.is_open(); } 
    inline bool openToWrite()  { if (fileStream.is_open()) return true; fileStream.open(fileName, std::ios::out | std::ios::binary); return fileStream.is_open(); } 
    inline bool openToUpdate() { if (fileStream.is_open()) return true; fileStream.open(fileName, std::ios::in  | std::ios::out | std::ios::binary); return fileStream.is_open(); }

    inline void close() { fileStream.close(); } 

    inline void seekRead (EgFileOffsetType position) { fileStream.seekg(position); }
    inline void seekWrite(EgFileOffsetType position) { fileStream.seekp(position); }
    inline void seekWriteToEnd(EgFileOffsetType& position) { fileStream.seekg (0, std::ios::end); position = (EgFileOffsetType) fileStream.tellg(); }

    inline void writeBool( bool theValue)  { fileStream << (ByteType) theValue; }
    inline void readBool ( bool& theValue) { ByteType tmpBool; fileStream >> tmpBool; theValue = tmpBool; }

    template<typename T> inline void writeType (T theValue)  {
        dataStream.seek(0); dataStream << theValue; fileStream.write((const char*) dataStream.bufData, sizeof(T)); }

    template<typename T> inline void readType  (T& theValue) {
        fileStream.read ((char*) dataStream.bufData, sizeof(T)); dataStream.seek(0); dataStream >> theValue; }

    inline void readStr ( std::string& theValue) { 
        EgStrSizeType strSize {0}; readType<EgStrSizeType>(strSize); fileStream.read ((char*) dataStream.bufData, strSize);
        theValue.insert(0, (const char*) dataStream.bufData, strSize); }
    
    // inline void writeStr ( std::string& theValue) { writeType<EgStrSizeType>( (EgStrSizeType) theValue.size()); fileStream << theValue; }

    // inline void readBuf (ByteType* bufData, uint64_t count) { fileStream.read ((char*) bufData, count); }
    // inline void writeBuf(ByteType* bufData, uint64_t count) { fileStream.write((char*) bufData, count); }

    // inline void readBufPos (ByteType* bufData, EgFileOffsetType position, uint64_t count) { seekRead(position);  fileStream.read ((char*) bufData, count); }
    // inline void writeBufPos(ByteType* bufData, EgFileOffsetType position, uint64_t count) { seekWrite(position); fileStream.write((char*) bufData, count); }
};

template <typename T> inline EgFileType& operator >> (EgFileType &egFile, T& i) { egFile.readType<T>(i); return egFile; }
template <> inline EgFileType& operator >> (EgFileType &egFile, std::string& s) { egFile.readStr(s); return egFile; }
template <> inline EgFileType& operator >> (EgFileType &egFile, bool& b)        { egFile.readBool(b); return egFile; }

template <typename T> inline EgFileType& operator << (EgFileType &egFile, T& i) { egFile.writeType<T>(i); return egFile; }
// template <> inline EgFileType& operator << (EgFileType &egFile, std::string& s) { egFile.writeStr(s); return egFile; }
template <> inline EgFileType& operator << (EgFileType &egFile, bool& b)        { egFile.writeBool(b); return egFile; }

/*
template <typename T> EgFileType& operator >> (EgFileType &egFile, T& i);
template <> EgFileType& operator >> (EgFileType &egFile, std::string& s);
template <> EgFileType& operator >> (EgFileType &egFile, bool& b);

template <typename T> EgFileType& operator << (EgFileType &egFile, T& i);
// template <> EgFileType& operator << (EgFileType &egFile, std::string& s);
template <> EgFileType& operator << (EgFileType &egFile, bool& b);
*/