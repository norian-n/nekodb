#include <iostream>
#include <filesystem>

#include "egDatabase.h"
#include "egLiterals.h"

void EgDatabase::clear() {
    nodesTypesMetainfo.clear();
    linksTypesMetainfo.clear();
    nodesTypesMetainfoLoaded = false;
    linksTypesMetainfoLoaded = false;
    // FIXME TODO add layers
}

inline void EgDatabase::createSystemStorage(const std::string& storName, const std::string& fieldName, const std::string& fieldBP) {
    CreateNodeBlueprint(storName);
    AddNodeDataField(fieldName);
    AddNodeDataField(fieldBP);
    CommitSystemNodeBlueprint();
}

void EgDatabase::initDatabase() {
    // check if nodes and links metainfo exists
    if (nodesTypesMetainfo.ConnectSystemNodeType(nodesSetsStorageName, nodesSetsStorageName) != 0) {
        EG_LOG_STUB << "INIT: Not found nodes metadata storage, create new: " << nodesSetsStorageName << FN;
        // initNodesMetainfo();
        createSystemStorage(nodesSetsStorageName, nodesSetFieldName, nodesBPFieldName);
        nodesTypesMetainfo.ConnectSystemNodeType(nodesSetsStorageName, nodesSetsStorageName);
    }
    LoadDataNodesSetsInfo();
    if (linksTypesMetainfo.ConnectSystemNodeType(linksSetsStorageName,linksSetsStorageName) != 0) {
        EG_LOG_STUB << "INIT: Not found links metadata storage, create new: " << linksSetsStorageName << FN;
        // initLinksMetainfo();
        createLinksMetainfo();
        linksTypesMetainfo.ConnectSystemNodeType(linksSetsStorageName, linksSetsStorageName);
    }
    LoadLinksInfo();
    if (layersMetainfo.ConnectSystemNodeType(layersSetsStorageName, layersSetsStorageName) != 0) {
        EG_LOG_STUB << "INIT: Not found layers metadata storage, create new: " << layersSetsStorageName << FN;
        createLayersMetainfo();
        layersMetainfo.ConnectSystemNodeType(layersSetsStorageName, layersSetsStorageName);
    }
    LoadLayersInfo();
}

inline void EgDatabase::createLinksMetainfo() { // , , 
    CreateNodeBlueprint(linksSetsStorageName);
    AddNodeDataField(linksSetFieldName);
    AddNodeDataField(linksBPFieldName);
    AddNodeDataField(linksFromFieldName);
    AddNodeDataField(linksToFieldName);
    CommitSystemNodeBlueprint();
}

inline void EgDatabase::createLayersMetainfo() {
    CreateNodeBlueprint(layersSetsStorageName);
    AddNodeDataField("layersBlueprintID");
    AddNodeDataField("layersTypeName");    
    CommitSystemNodeBlueprint();
}

EgDataNodesSet* EgDatabase::GetNodesTypePtrByID(EgBlueprintIDType nodeTypeID) { // FIXME implement serialDataPtr on all types load
    return static_cast < EgDataNodesSet* > (nodesTypesMetainfo.nodesContainer->GetNodePtrByID (nodeTypeID)-> serialDataPtr);
}

int EgDatabase::AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName) {
    // EG_LOG_STUB << "nodesSetName: " << nodesSetName << " blueprintName: " << blueprintName << FN;
    EgBlueprintIDType stubID;
    if (!nodeTypeIDByName(nodesSetName, stubID)) { // metainfo Data Exists
        EG_LOG_STUB << "ERROR : nodes set name already exists : " << nodesSetName << FN;
        return -1;
    }
    // EG_LOG_STUB << "typeName : " << typeName << FN;
    EgDataNode* newNode = new EgDataNode(nodesTypesMetainfo.dataNodeBlueprint);
    // blueprintID = nodesTypesMetainfo.nodesContainer-> GetLastID() + 1;
    *newNode << nodesSetName;
    *newNode << blueprintName;
    nodesTypesMetainfo << newNode;
    nodesTypesMetainfo.Store();
    return 0;
}

int EgDatabase::AddLinksTypeInfo(const std::string& linksSetName, const std::string& blueprintName, 
                                 const std::string& nodesFrom, const std::string& nodesTo) {
    EgBlueprintIDType stubID;
    if (!linkTypeIDByName(linksSetName, stubID)) { // metainfo Data Exists
        EG_LOG_STUB << "ERROR : linkss set name already exists : " << linksSetName << FN;
        return -1;
    }
    EgDataNode* newNode = new EgDataNode(linksTypesMetainfo.dataNodeBlueprint);
    // blueprintID = linksTypesMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    (*newNode)[linksSetFieldName]  << linksSetName;
    (*newNode)[linksBPFieldName]   << blueprintName;
    (*newNode)[linksFromFieldName] << nodesFrom;
    (*newNode)[linksToFieldName]   << nodesTo;
    linksTypesMetainfo << newNode;
    linksTypesMetainfo.Store();
    return 0;
}

void EgDatabase::AddLayersTypeInfo(EgBlueprintIDType& blueprintID, const std::string& layersTypeName) {
    if (!layersTypeIDByName(layersTypeName, blueprintID)) // metainfo Data Exists
        return;
    // EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageBlueprint);
    EgDataNode* newNode = new EgDataNode(layersMetainfo.dataNodeBlueprint);
    blueprintID = layersMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    *newNode << blueprintID;
    *newNode << layersTypeName;
    layersMetainfo << newNode;
}

void EgDatabase::LoadDataNodesSetsInfo() {
    if (nodesTypesMetainfoLoaded) // load once
        return;
    nodesTypesMetainfoLoaded = ! nodesTypesMetainfo.LoadAllNodes(); // 0 is ok
}

void EgDatabase::LoadLinksInfo() {
    if (linksTypesMetainfoLoaded) // load once
        return;
    linksTypesMetainfoLoaded = ! linksTypesMetainfo.LoadAllNodes(); // 0 is ok
}

void EgDatabase::LoadLayersInfo() {
    if (layersMetainfoLoaded) // load once
        return;
    linksTypesMetainfoLoaded = ! layersMetainfo.LoadAllNodes(); // 0 is ok
}

bool EgDatabase::nodeTypeIDByName(const std::string& typeName, EgBlueprintIDType& nodeTypeID) {
    // EG_LOG_STUB << "check typeName : " << typeName << FN;
    // if (LoadDataNodesTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[nodesSetFieldName] >> currentName;
            // nodesBPFieldName
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == typeName) {
                nodeTypeID = nodesIter.first;
                return false; // typename exists
            }
        }
    // }
    return true;
}

bool EgDatabase::blueprintNameByNodeName(const std::string& nodesSetName, std::string& nodeBlueprintName) {
    // EG_LOG_STUB << "check typeName : " << nodesSetName << FN;
    // if (LoadDataNodesTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {
            std::string currentName;
            (*(nodesIter.second))[nodesSetFieldName] >> currentName;
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == nodesSetName) {
                // nodeTypeID = nodesIter.first;
                (*(nodesIter.second))[nodesBPFieldName] >> nodeBlueprintName;
                return false; // typename exists
            }
        }
    // }
    return true;
}

bool EgDatabase::blueprintNameByLinkName(const std::string& linksSetName, std::string& linkBlueprintName) {
    // EG_LOG_STUB << "check typeName : " << nodesSetName << FN;
    // if (LoadDataNodesTypesInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {
            std::string currentName;
            (*(nodesIter.second))[linksSetFieldName] >> currentName;
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == linksSetName) {
                // nodeTypeID = nodesIter.first;
                (*(nodesIter.second))[linksBPFieldName] >> linkBlueprintName;
                return false; // typename exists
            }
        }
    // }
    return true;
}

EgDataNode* EgDatabase::dataNodePtrByNodesType(EgDataNodesSet* nodesType) {
    // EG_LOG_STUB << "check typeName : " << typeName << FN;
    // if (LoadDataNodesTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[nodesSetFieldName] >> currentName;
            // nodesBPFieldName
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == nodesType-> nodesSetName) {
                return nodesIter.second; // typename exists
            }
        }
    // }
    return nullptr;
}


bool EgDatabase::linkTypeIDByName(const std::string& linkName, EgBlueprintIDType& linkTypeID) {
    // if (LoadLinksInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[linksSetFieldName] >> currentName;
            // EG_LOG_STUB << "linkTypeIDByName() currentName: " << currentName << " linkName: " << linkName << FN;
            if (currentName == linkName) {
                linkTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

bool EgDatabase::layersTypeIDByName(const std::string& layersName, EgBlueprintIDType& layersTypeID) {
    // if (LoadLayersInfo() == 0) { // files exist
        for (auto nodesIter : layersMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))["layersTypeName"] >> currentName;
            // EG_LOG_STUB << "layersTypeIDByName() currentName: " << currentName << " layersName: " << layersName << FN;
            if (currentName == layersName) {
                layersTypeID = nodesIter.first;
                // EG_LOG_STUB << "linkTypeIDByName() node type found: " << linkName << " " << std::dec << linkTypeID << FN;
                return false; // typename exists
            }
        }
    // }
    return true;
}

int EgDatabase::CreateNodesSetByBlueprint(const std::string& nodesSetName, const std::string& nodeBlueprintName) {
    // EG_LOG_STUB << "nodesSetName: " << nodesSetName << " nodeBlueprintName: " << nodeBlueprintName << FN;
    return AddDataNodesTypeInfo(nodesSetName, nodeBlueprintName);
}

int EgDatabase::CreateNodesSet(const std::string& nodesSetName) {
    return CreateNodeBlueprint(nodesSetName);
}

void EgDatabase::CommitNodesSet() {
    if (CreateNodeBlueprintFlag) {
        nodeBlueprint-> BlueprintInitCommit();
        AddDataNodesTypeInfo(nodeBlueprint-> blueprintName, nodeBlueprint-> blueprintName);
        // EG_LOG_STUB << "newBlueprint after: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << FN;
    }
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

int EgDatabase::CreateNodeBlueprint(const std::string& blueprintName) { //,  EgNodeTypeSettings& typeSettings)
    if (std::filesystem::exists(blueprintName + ".dnl")) { 
        EG_LOG_STUB << "ERROR: Data nodes type/blueprint already exists, cant CreateNodeType(): " << blueprintName << FN;
        CreateNodeBlueprintFlag = false; // abort next ops
        return -1;
    }
    nodeBlueprint = new EgDataNodeBlueprint(blueprintName);  // if someone failed to commit blueprint and leak mem, no big deal
    nodeBlueprint-> BlueprintInitStart();
    CreateNodeBlueprintFlag = true; // ok to add fields
    // EG_LOG_STUB << "newBlueprint before: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << FN;
    return 0;
}

void EgDatabase::AddNodeDataField(const std::string& fieldName) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddDataFieldName(fieldName);
    if (CreateLinkBlueprintFlag) {
        EG_LOG_STUB << "WARNING: AddNodeDataField() called inside link blueprint setup: ";
        if (linkBlueprint)
            EG_LOG_STUB << linkBlueprint->blueprintName << FN;
        EG_LOG_STUB << FN;
    }
}

void EgDatabase::AddIndex(const std::string& indexName, uint8_t indexSizeBytes, uint8_t indexSubType) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddIndex(indexName, indexSizeBytes, indexSubType);
}

void EgDatabase::CommitNodeBlueprint() {
    if (CreateNodeBlueprintFlag) {
        nodeBlueprint-> BlueprintInitCommit();
        // AddDataNodesTypeInfo(nodeBlueprint-> blueprintName, nodeBlueprint-> blueprintName);
        // EG_LOG_STUB << "newBlueprint after: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << FN;
    }
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

void EgDatabase::CommitSystemNodeBlueprint() { // dont add to nodes metainfo
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> BlueprintInitCommit();
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

int EgDatabase::CreateSimpleLinkSet(const std::string& linkTypeName, const std::string& nodesFrom, const std::string& nodesTo) {
    std::string fullLinkName = linkTypeName + linkSuffixName;
    if (std::filesystem::exists(fullLinkName + ".dnl")) { 
        EG_LOG_STUB << "ERROR: Link type/blueprint file already exists, cant CreateLinkBlueprint(): " << linkTypeName << FN;
        CreateLinkBlueprintFlag = false; // abort next ops
        return -1;
    }   
    linkBlueprint = new EgDataNodeBlueprint(fullLinkName); // if someone failed to commit blueprint and leak mem, no big deal
    linkBlueprint-> BlueprintInitStart();
    linkBlueprint-> AddDataFieldName("fromID"); // FIXME global const
    linkBlueprint-> AddDataFieldName("toID");   // FIXME global const
    // EG_LOG_STUB << "newBlueprint before: " << newBlueprint->blueprintName << " " << (int) newBlueprint-> blueprintID << FN;
    linkBlueprint->BlueprintInitCommit();
    AddLinksTypeInfo(fullLinkName, linkBlueprint->blueprintName, nodesFrom, nodesTo);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    linksTypesMetainfo.Store();
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
    return 0;
}

int EgDatabase::CreateLayersSet(const std::string& layersTypeName) {
    std::string fullLayersName = layersTypeName + std::string("_egLayersInfo"); // FIXME global const
    if (std::filesystem::exists(fullLayersName + ".dnl")) { 
        EG_LOG_STUB << "ERROR: Layer type/blueprint file already exists, cant CreateLayersBlueprint(): " << layersTypeName << FN;
        return -1;
    }   
    layersBlueprint = new EgDataNodeBlueprint(fullLayersName); // if someone failed to commit blueprint and leak mem, no big deal
    layersBlueprint-> BlueprintInitStart();
    layersBlueprint-> AddDataFieldName("nodesNames");
    layersBlueprint-> AddDataFieldName("linksNames");
    layersBlueprint-> AddDataFieldName("layerWidth");
    layersBlueprint-> AddDataFieldName("layerHeight");
    layersBlueprint-> AddDataFieldName("parentLayerID");
    layersBlueprint-> BlueprintInitCommit();
    AddLayersTypeInfo(layersBlueprint-> blueprintID, layersBlueprint-> blueprintName);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    layersMetainfo.Store();
    delete layersBlueprint;
    layersBlueprint = nullptr;
    return 0;
}

int EgDatabase::CreateLinkWithDataBlueprint(const std::string& linkTypeName) {
    std::string fullLinkName = linkTypeName + linkSuffixName;
    if (std::filesystem::exists(fullLinkName + ".dnl")) { 
        EG_LOG_STUB << "ERROR: Link type/blueprint file already exists, cant CreateLinkBlueprint(): " << linkTypeName << FN;
        CreateLinkBlueprintFlag = false; // abort next ops
        return -1;
    }
    linkBlueprint = new EgDataNodeBlueprint(fullLinkName); // if someone failed to commit blueprint and leak mem, no big deal
    linkBlueprint-> BlueprintInitStart();
    linkBlueprint-> AddDataFieldName("fromID"); // FIXME global const
    linkBlueprint-> AddDataFieldName("toID");   // FIXME global const
    // EG_LOG_STUB << "newBlueprint before: " << newBlueprint->blueprintName << " " << (int) newBlueprint-> blueprintID << FN;
    CreateLinkBlueprintFlag = true;
    return 0;
}

void EgDatabase::AddLinkDataField(const std::string& fieldName) {
    if (CreateLinkBlueprintFlag)
        linkBlueprint-> AddDataFieldName(fieldName);
    if (CreateNodeBlueprintFlag) {
        EG_LOG_STUB << "WARNING: AddLinkDataField() called inside data node blueprint setup: ";
        if (nodeBlueprint)
            EG_LOG_STUB << nodeBlueprint->blueprintName << FN;
        EG_LOG_STUB << FN;
    }
}

void EgDatabase::CommitLinkBlueprint() {
    if (CreateLinkBlueprintFlag) {
        linkBlueprint-> BlueprintInitCommit();
        // AddLinksTypeInfo(linkBlueprint-> blueprintName, linkBlueprint-> blueprintName); // FIXME check
        // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int) linkBlueprint-> blueprintID << FN;
        // linksTypesMetainfo.Store();
    }
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
}

int EgDatabase::CreateLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName, 
                                          const std::string& nodesFrom, const std::string& nodesTo) {
    return AddLinksTypeInfo(linksSetName, blueprintName, nodesFrom, nodesTo);
}