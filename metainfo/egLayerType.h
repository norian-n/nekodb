#pragma once
#include "egDatabaseType.h"

class EgLayerType {
public:
    EgBlueprintIDType           layerBlueprintID          { 0 };
    std::string                 linkTypeName;
    EgDatabaseType*             metaInfoDatabase    { nullptr };    
    EgDataNodeBlueprintType*    layerStorageBlueprint  { nullptr };
    EgDataNodesContainerType*   layerStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;

    EgLayerType(std::string& a_Name, EgDatabaseType& a_Database):
            linkTypeName(a_Name),
             metaInfoDatabase(&a_Database),           
            layerStorageBlueprint(new EgDataNodeBlueprintType(a_Name)),
            layerStorage (new EgDataNodesContainerType()) { layerStorage->init(layerStorageBlueprint); initFlexLinkBlueprint(layerStorageBlueprint); }

    ~EgLayerType() { clear(); delete layerStorage; delete layerStorageBlueprint; }
    
    void clear();
    void initFlexLinkBlueprint(EgDataNodeBlueprintType* layerBlueprint);
    void AddRawLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    void AddContainersLink(EgBlueprintIDType fromLayID, EgDataNodeIDType fromID, EgBlueprintIDType toLayID, EgDataNodeIDType toID);
    int LoadLinks();
    int StoreLinks();
    int ResolveNodesIDsToPtrs();
};

// ======================== Debug ========================

// void PrintResolvedLinksFlex(const EgDataNodeType& node);
