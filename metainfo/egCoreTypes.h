#pragma once
#include <iostream>
#include <cstdint>
#include <cstring>
#include <map>
#include <unordered_map>
#include <unordered_set>

#define EG_LOG_STUB std::cout
#define FN " [" << __FUNCTION__  << "(), " << __FILE__ << "]" << std::endl
#define FNS std::string(" [") + std::string(__FUNCTION__) + std::string("(), ") + std::string(__FILE__) + std::string("]")
#define HEX std::hex
#define DEC std::dec

typedef unsigned char    EgByteType;
    // ID types
typedef uint32_t         EgDataNodeIDType;
typedef EgDataNodeIDType EgDataLinkIDType;
typedef uint16_t         EgBlueprintIDType;     // data nodes and links blueprint/layout/type/class ID type
    // count/size/length types
typedef uint16_t         EgFieldNameLengthType;
typedef uint16_t         EgFieldsCountType;
typedef uint16_t         EgStrSizeType;
typedef uint64_t         EgStaticLengthType;
typedef uint32_t         EgMultArrayCountType;
    // map types
class   EgDataNode;

typedef std::unordered_map <EgDataNodeIDType, EgDataNode*>  EgDataNodesMapType;
typedef std::map           <EgDataNodeIDType, EgDataNode*>  EgDataNodesOrdMapType;
typedef std::unordered_map <EgDataLinkIDType, EgDataNode*>  EgLinkIDsNodePtrsMapType;
typedef std::unordered_map <EgDataNode*, EgDataNode*>       EgLinkDataPtrsNodePtrsMapType;

const uint64_t egDefaultHamBrickSize = 1024 * 64; // TODO FIXME add flex bricksize to pre-launch system settings