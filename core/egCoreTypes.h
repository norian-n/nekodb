#pragma once
#include <cstdint>
#include <cstring>
#include <map>

typedef unsigned char ByteType;
    // ID types
typedef uint32_t    EgDataNodeIDType;
typedef EgDataNodeIDType EgDataLinkIDType;
typedef uint16_t    EgBlueprintIDType;     // data nodes layout (DNL) ID type, links layout type IDs type
    // count/size/length types
typedef uint16_t    EgFieldNameLengthType;
typedef uint8_t     EgFieldsCountType;
typedef uint16_t    EgStrSizeType;

typedef uint64_t    StaticLengthType;

class EgDataNodeType;

typedef std::map <EgDataNodeIDType, EgDataNodeType*>  EgDataNodesMapType;
typedef std::map <EgDataLinkIDType, EgDataNodeType*>  EgDataLinksMapType;

const uint64_t egDefaultHamBrickSize = 1024; // 1024*1; // TODO FIXME add flex bricksize to pre-launch system settings