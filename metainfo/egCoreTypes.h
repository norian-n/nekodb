#pragma once
#include <iostream>
#include <cstdint>
#include <cstring>
#include <map>
#include <unordered_map>

typedef unsigned char ByteType;
    // ID types
typedef uint32_t            EgDataNodeIDType;
typedef EgDataNodeIDType    EgDataLinkIDType;
typedef uint16_t            EgBlueprintIDType;     // data nodes and links blueprint/layout/type/class ID type
typedef uint16_t            EgLayerNumType;
    // count/size/length types
typedef uint16_t            EgFieldNameLengthType;
typedef uint8_t             EgFieldsCountType;
typedef uint16_t            EgStrSizeType;
typedef uint64_t            StaticLengthType;
    // map types
class EgDataNode;
class EgDataLinkType;
typedef std::unordered_map <EgDataNodeIDType, EgDataNode*>  EgDataNodesMapType;
typedef std::map <EgDataNodeIDType, EgDataNode*>            EgDataNodesOrdMapType;
typedef std::unordered_map <EgDataLinkIDType, EgDataNode*>  EgLinkIDsNodePtrsMapType;
typedef std::unordered_map <EgDataNode*, EgDataNode*>   EgLinkDataPtrsNodePtrsMapType;

const uint64_t egDefaultHamBrickSize = 1024; // 1024*1; // TODO FIXME add flex bricksize to pre-launch system settings