#pragma once
#include <cstdint>
#include <map>

typedef unsigned char ByteType;
    // ID types
typedef uint32_t            EgDataNodeIDType;
typedef EgDataNodeIDType    EgDataLinkIDType;
typedef uint16_t            EgBlueprintIDType;     // data nodes and links blueprint/layout/type/class ID type
    // count/size/length types
typedef uint16_t    EgFieldNameLengthType;
typedef uint8_t     EgFieldsCountType;
typedef uint16_t    EgStrSizeType;
typedef uint64_t    StaticLengthType;
    // map types
class EgDataNodeType;
class EgDataLinkType;
typedef std::map <EgDataNodeIDType, EgDataNodeType*>  EgDataNodesMapType;
typedef std::map <EgDataLinkIDType, EgDataNodeType*>  EgLinkIDsNodePtrsMapType;
typedef std::map <EgDataNodeType*, EgDataNodeType*>   EgLinkDataPtrsNodePtrsMapType;

const uint64_t egDefaultHamBrickSize = 1024; // 1024*1; // TODO FIXME add flex bricksize to pre-launch system settings