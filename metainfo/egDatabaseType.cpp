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
    LoadTypesInfo();
    if (linksTypesMetainfo.ConnectSystemNodeType(linksTypesStorageName) != 0) {
        std::cout << "initDatabase() not found metadata storage, create new: " << linksTypesStorageName << std::endl;
        initLinksMetainfo();
        linksTypesMetainfo.ConnectSystemNodeType(linksTypesStorageName);
    }
    LoadLinksInfo();
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

void EgDatabaseType::AddDataNodesTypeInfo(EgBlueprintIDType& blueprintID, std::string &typeName) {
    if (!nodeTypeIDByName(typeName, blueprintID))// metainfoDataExists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(nodesTypesStorageBlueprint);
    EgDataNodeType* newNode = new EgDataNodeType(nodesTypesMetainfo.dataNodeBlueprint);
    blueprintID = nodesTypesMetainfo.nodesContainer-> lastNodeID + 1;
    *newNode << blueprintID;
    *newNode << typeName;
    nodesTypesMetainfo << newNode;
}

void EgDatabaseType::AddLinksTypeInfo(EgBlueprintIDType& blueprintID, std::string &linksTypeName) {
    if (!linkTypeIDByName(linksTypeName, blueprintID))// metainfoDataExists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageBlueprint);
    EgDataNodeType* newNode = new EgDataNodeType(linksTypesMetainfo.dataNodeBlueprint);
    blueprintID = linksTypesMetainfo.nodesContainer-> lastNodeID + 1;
    *newNode << blueprintID;
    *newNode << linksTypeName;
    linksTypesMetainfo << newNode;
}

int EgDatabaseType::LoadTypesInfo() {
    if (nodesTypesMetainfoLoaded)
        return 0;
    int res = nodesTypesMetainfo.LoadAllNodes();
    nodesTypesMetainfoLoaded = !res; // ok
    return res;
}

int EgDatabaseType::StoreTypesInfo() {
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

bool EgDatabaseType::nodeTypeIDByName(std::string& typeName, EgBlueprintIDType& nodeTypeID) {
    if (LoadTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["nodesTypeName"].arrayData);
            // std::cout << "nodeTypeIDByName() currentName: " << currentName << " typeName: " << typeName << std::endl;
            if (currentName == typeName) {
                nodeTypeID = nodesIter.first;
                // std::cout << "nodeTypeIDByName() node type found: " << typeName << " " << std::dec << nodeTypeID << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

bool EgDatabaseType::linkTypeIDByName(std::string& linkName, EgBlueprintIDType& linkTypeID) {
    if (LoadLinksInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["linksTypeName"].arrayData);
            std::cout << "linkTypeIDByName() currentName: " << currentName << " linkName: " << linkName << std::endl;
            if (currentName == linkName) {
                linkTypeID = nodesIter.first;
                // std::cout << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

int EgDatabaseType::CreateNodeBlueprint(const char* name) { //,  EgNodeTypeSettings& typeSettings)
    std::string strName(name);
    if (std::filesystem::exists(strName + ".dnl")) { 
        std::cout << "ERROR: Data nodes type/blueprint already exists, cant CreateNodeType(): " << name << std::endl;
        CreateNodeBlueprintFlag = false; // abort next ops
        return -1;
    }
    nodeBlueprint = new EgDataNodeBlueprintType(strName);  // if someone failed to commit blueprint and leak mem, no big deal
    nodeBlueprint-> BlueprintInitStart();
    CreateNodeBlueprintFlag = true; // ok to add fields
    // std::cout << "newBlueprint before: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << std::endl;
    return 0;
}

void EgDatabaseType::AddNodeDataField(std::string& fieldName) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddDataFieldName(fieldName);
}

void EgDatabaseType::CommitNodeBlueprint() {
    if (CreateNodeBlueprintFlag) {
        nodeBlueprint-> BlueprintInitCommit();
        AddDataNodesTypeInfo(nodeBlueprint-> blueprintID, nodeBlueprint-> blueprintName);
        // std::cout << "newBlueprint after: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << std::endl;
        StoreTypesInfo();
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

int EgDatabaseType::CreateLinkBlueprint(std::string& linkTypeName, std::string& nodesFrom, std::string& nodesTo) {
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

int EgDatabaseType::CreateLinkWithDataBlueprint(std::string& linkTypeName, std::string& nodesFrom, std::string& nodesTo) {
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

void EgDatabaseType::AddLinkDataField(std::string& fieldName) {
    if (CreateLinkBlueprintFlag)
        linkBlueprint-> AddDataFieldName(fieldName);
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