#include <iostream>
#include <filesystem>

#include "egDatabaseType.h"

void EgDatabaseType::clear() {
    nodesTypesMetainfo.clear();
    linksTypesMetainfo.clear();
    nodesTypesMetainfoLoaded = false;
    linksTypesMetainfoLoaded = false;
}

void EgDatabaseType::initDatabase() {
    // check if nodes and links metainfo exists
    if (nodesTypesMetainfo.ConnectSystemNodeType(nodesTypesStorageName) != 0) {
        std::cout << "initDatabase() not found metadata storage, create new: " << nodesTypesStorageName << std::endl;
        initNodesMetainfo();
        nodesTypesMetainfo.ConnectSystemNodeType(nodesTypesStorageName);
    }
    LoadDataNodesTypesInfo();
    if (linksTypesMetainfo.ConnectSystemNodeType(linksTypesStorageName) != 0) {
        std::cout << "initDatabase() not found metadata storage, create new: " << linksTypesStorageName << std::endl;
        initLinksMetainfo();
        linksTypesMetainfo.ConnectSystemNodeType(linksTypesStorageName);
    }
    LoadLinksInfo();
    if (layersMetainfo.ConnectSystemNodeType(layersStorageName) != 0) {
        std::cout << "initDatabase() not found metadata storage, create new: " << layersStorageName << std::endl;
        initLayersMetainfo();
        layersMetainfo.ConnectSystemNodeType(layersStorageName);
    }
    LoadLayersInfo();
}

void EgDatabaseType::initNodesMetainfo() {
    CreateNodeBlueprint(nodesTypesStorageName);

    AddNodeDataField("nodesBlueprintID");
    AddNodeDataField("nodesTypeName");

    CommitSystemNodeBlueprint();
}

void EgDatabaseType::initLinksMetainfo() {
    CreateNodeBlueprint(linksTypesStorageName);

    AddNodeDataField("linksBlueprintID");
    AddNodeDataField("linksTypeName");

    CommitSystemNodeBlueprint();
}

void EgDatabaseType::initLayersMetainfo() {
    CreateNodeBlueprint(layersStorageName);

    AddNodeDataField("layersBlueprintID");
    AddNodeDataField("layersTypeName");    

    CommitSystemNodeBlueprint();
}

int EgDatabaseType::InsertDataNodesTypeToMap(EgBlueprintIDType dntID, EgDataNodesType *dntPtr) {
    auto iter = dataNodesTypes.find(dntID); // search if already connected
    if (iter != dataNodesTypes.end())
        return -1;
    dataNodesTypes.insert(std::pair<EgBlueprintIDType, EgDataNodesType *>(dntID, dntPtr));
    return 0;
}

EgDataNodesType *EgDatabaseType::GetNodeTypePtrByID(EgBlueprintIDType nodeTypeID) {
    auto iter = dataNodesTypes.find(nodeTypeID); // search all nodes
    if (iter != dataNodesTypes.end())
        return iter->second;
    return nullptr;
}

void EgDatabaseType::AddDataNodesTypeInfo(EgBlueprintIDType& blueprintID, const std::string& typeName) {
    if (!nodeTypeIDByName(typeName, blueprintID))// metainfoDataExists
        return;
    // EG_LOG_STUB << "typeName : " << typeName << FN;
    EgDataNodeType* newNode = new EgDataNodeType(nodesTypesMetainfo.dataNodeBlueprint);
    blueprintID = nodesTypesMetainfo.nodesContainer-> GetLastID() + 1;
    *newNode << blueprintID;
    *newNode << typeName;
    nodesTypesMetainfo << newNode;
}

void EgDatabaseType::AddLinksTypeInfo(EgBlueprintIDType& blueprintID, const std::string& linksTypeName) {
    if (!linkTypeIDByName(linksTypeName, blueprintID))// metainfoDataExists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageBlueprint);
    EgDataNodeType* newNode = new EgDataNodeType(linksTypesMetainfo.dataNodeBlueprint);
    blueprintID = linksTypesMetainfo.nodesContainer-> GetLastID() + 1;
    *newNode << blueprintID;
    *newNode << linksTypeName;
    linksTypesMetainfo << newNode;
}

void EgDatabaseType::AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& layersTypeName) {
    if (!layersTypeIDByName(layersTypeName, blueprintID))// metainfoDataExists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageBlueprint);
    EgDataNodeType* newNode = new EgDataNodeType(layersMetainfo.dataNodeBlueprint);
    blueprintID = layersMetainfo.nodesContainer-> GetLastID() + 1;
    *newNode << blueprintID;
    *newNode << layersTypeName;
    layersMetainfo << newNode;
}

int EgDatabaseType::LoadDataNodesTypesInfo() {
    if (nodesTypesMetainfoLoaded)
        return 0;
    int res = nodesTypesMetainfo.LoadAllNodes();
    nodesTypesMetainfoLoaded = !res; // ok
    return res;
}

int EgDatabaseType::StoreDataNodesTypesInfo() {
    return nodesTypesMetainfo.Store();
}

int EgDatabaseType::LoadLinksInfo() {
    if (linksTypesMetainfoLoaded)
        return 0;
    int res = linksTypesMetainfo.LoadAllNodes();
    linksTypesMetainfoLoaded = !res; // ok
    return res;
}

int EgDatabaseType::StoreLinksInfo() {
    return linksTypesMetainfo.Store();
}

int EgDatabaseType::LoadLayersInfo() {
    if (layersMetainfoLoaded)
        return 0;
    int res = layersMetainfo.LoadAllNodes();
    layersMetainfoLoaded = !res; // ok
    return res;
}

int EgDatabaseType::StoreLayersInfo() {
    return layersMetainfo.Store();
}

bool EgDatabaseType::nodeTypeIDByName(const std::string& typeName, EgBlueprintIDType& nodeTypeID) {
    // EG_LOG_STUB << "check typeName : " << typeName << FN;
    if (LoadDataNodesTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["nodesTypeName"].arrayData);
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == typeName) {
                nodeTypeID = nodesIter.first;
                return false; // typename exists
            }
        }
    }
    return true;
}

bool EgDatabaseType::linkTypeIDByName(const std::string& linkName, EgBlueprintIDType& linkTypeID) {
    if (LoadLinksInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["linksTypeName"].arrayData);
            // std::cout << "linkTypeIDByName() currentName: " << currentName << " linkName: " << linkName << std::endl;
            if (currentName == linkName) {
                linkTypeID = nodesIter.first;
                // std::cout << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

bool EgDatabaseType::layersTypeIDByName(const std::string& layersName, EgBlueprintIDType& layersTypeID) {
    if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : layersMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["layersTypeName"].arrayData);
            std::cout << "layersTypeIDByName() currentName: " << currentName << " layersName: " << layersName << std::endl;
            if (currentName == layersName) {
                layersTypeID = nodesIter.first;
                // std::cout << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

int EgDatabaseType::CreateNodeBlueprint(const std::string& blueprintName) { //,  EgNodeTypeSettings& typeSettings)
    if (std::filesystem::exists(blueprintName + ".dnl")) { 
        std::cout << "ERROR: Data nodes type/blueprint already exists, cant CreateNodeType(): " << blueprintName << std::endl;
        CreateNodeBlueprintFlag = false; // abort next ops
        return -1;
    }
    nodeBlueprint = new EgDataNodeBlueprintType(blueprintName);  // if someone failed to commit blueprint and leak mem, no big deal
    nodeBlueprint-> BlueprintInitStart();
    CreateNodeBlueprintFlag = true; // ok to add fields
    // std::cout << "newBlueprint before: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << std::endl;
    return 0;
}

void EgDatabaseType::AddNodeDataField(const std::string& fieldName) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddDataFieldName(fieldName);
    if (CreateLinkBlueprintFlag) {
        std::cout << "WARNING: AddNodeDataField() called inside link blueprint setup: ";
        if (linkBlueprint)
            std::cout << linkBlueprint->blueprintName << std::endl;
        std::cout << std::endl;
    }
}

void EgDatabaseType::AddIndex(const std::string& indexName, uint8_t indexSizeBytes, uint8_t indexSubType) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddIndex(indexName, indexSizeBytes, indexSubType);
}

void EgDatabaseType::CommitNodeBlueprint() {
    if (CreateNodeBlueprintFlag) {
        nodeBlueprint-> BlueprintInitCommit();
        AddDataNodesTypeInfo(nodeBlueprint-> blueprintID, nodeBlueprint-> blueprintName);
        // std::cout << "newBlueprint after: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << std::endl;
        StoreDataNodesTypesInfo();
    }
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

void EgDatabaseType::CommitSystemNodeBlueprint() { // dont add to nodes metainfo
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> BlueprintInitCommit();
    // AddDataNodesTypeInfo(nodeBlueprint-> blueprintID, nodeBlueprint-> blueprintName); // FIXME STUB
    // std::cout << "newBlueprint after: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << std::endl;
    // StoreTypesInfo();
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

int EgDatabaseType::CreateLinkBlueprint(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo) {
    std::string fullLinkName = linkTypeName + std::string("_arrowLinks"); // FIXME global const
    if (std::filesystem::exists(fullLinkName + ".dnl")) { 
        std::cout << "ERROR: Link type/blueprint file already exists, cant CreateLinkBlueprint(): " << linkTypeName << std::endl;
        CreateLinkBlueprintFlag = false; // abort next ops
        return -1;
    }   
    linkBlueprint = new EgDataNodeBlueprintType(fullLinkName); // if someone failed to commit blueprint and leak mem, no big deal
    linkBlueprint-> BlueprintInitStart();
    linkBlueprint-> AddDataFieldName("fromID"); // FIXME global const
    linkBlueprint-> AddDataFieldName("toID");   // FIXME global const
    // std::cout << "newBlueprint before: " << newBlueprint->blueprintName << " " << (int) newBlueprint-> blueprintID << std::endl;
    linkBlueprint->BlueprintInitCommit();
    AddLinksTypeInfo(linkBlueprint->blueprintID, linkBlueprint->blueprintName);
    // std::cout << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << std::endl;
    StoreLinksInfo();
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
    return 0;
}

int EgDatabaseType::CreateLayersBlueprint(const std::string& layersTypeName) {
    std::string fullLayersName = layersTypeName + std::string("_egLayersInfo"); // FIXME global const
    if (std::filesystem::exists(fullLayersName + ".dnl")) { 
        std::cout << "ERROR: Layer type/blueprint file already exists, cant CreateLayersBlueprint(): " << layersTypeName << std::endl;
        return -1;
    }   
    layersBlueprint = new EgDataNodeBlueprintType(fullLayersName); // if someone failed to commit blueprint and leak mem, no big deal
    layersBlueprint-> BlueprintInitStart();
    layersBlueprint-> AddDataFieldName("layerNum");
    layersBlueprint-> AddDataFieldName("isLink");
    layersBlueprint-> AddDataFieldName("name");
    layersBlueprint-> BlueprintInitCommit();
    AddLayersTypeInfo(layersBlueprint-> blueprintID, layersBlueprint-> blueprintName);
    // std::cout << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << std::endl;
    StoreLayersInfo();
    delete layersBlueprint;
    layersBlueprint = nullptr;
    return 0;
}

int EgDatabaseType::CreateLinkWithDataBlueprint(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo) {
    std::string fullLinkName = linkTypeName + std::string("_arrowLinks"); // FIXME global const
    if (std::filesystem::exists(fullLinkName + ".dnl")) { 
        std::cout << "ERROR: Link type/blueprint file already exists, cant CreateLinkBlueprint(): " << linkTypeName << std::endl;
        CreateLinkBlueprintFlag = false; // abort next ops
        return -1;
    }
    linkBlueprint = new EgDataNodeBlueprintType(fullLinkName); // if someone failed to commit blueprint and leak mem, no big deal
    linkBlueprint-> BlueprintInitStart();
    linkBlueprint-> AddDataFieldName("fromID"); // FIXME global const
    linkBlueprint-> AddDataFieldName("toID");   // FIXME global const
    // std::cout << "newBlueprint before: " << newBlueprint->blueprintName << " " << (int) newBlueprint-> blueprintID << std::endl;
    CreateLinkBlueprintFlag = true;
    return 0;
}

void EgDatabaseType::AddLinkDataField(const std::string& fieldName) {
    if (CreateLinkBlueprintFlag)
        linkBlueprint-> AddDataFieldName(fieldName);
    if (CreateNodeBlueprintFlag) {
        std::cout << "WARNING: AddLinkDataField() called inside data node blueprint setup: ";
        if (nodeBlueprint)
            std::cout << nodeBlueprint->blueprintName << std::endl;
        std::cout << std::endl;
    }
}

void EgDatabaseType::CommitLinkBlueprint() {
    if (CreateLinkBlueprintFlag) {
        linkBlueprint-> BlueprintInitCommit();
        AddLinksTypeInfo(linkBlueprint->blueprintID, linkBlueprint-> blueprintName);
        // std::cout << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int) linkBlueprint-> blueprintID << std::endl;
        StoreLinksInfo();
    }
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
}