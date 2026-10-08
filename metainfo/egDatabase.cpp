#include <iostream>
#include <filesystem>

#include "egDatabase.h"
#include "egLiteralsMetainfo.h"
#include "../links/egLiteralsLinks.h"

// #include "../guisupport/egLayersMetainfo.h"

EgDatabase::EgDatabase() {
     initDatabase(); 
}

void EgDatabase::clear() {
    nodesTypesMetainfo.clear();
    linksTypesMetainfo.clear();
    // nodesTypesMetainfoLoaded = false;
    // linksTypesMetainfoLoaded = false;
}

void EgDatabase::initDatabase() {
    // check if nodes and links metainfo exists
    if ( ! nodesTypesMetainfo.ConnectSystemNodesSet(nodesSetsStorageName, nodesSetsStorageName) ) {
        EG_LOG_STUB << "INIT: Not found nodes metadata storage, create new: " << nodesSetsStorageName << FN;
        createNodesMetainfo();
    }
    LoadDataNodesSetsInfo();
    if ( ! linksTypesMetainfo.ConnectSystemNodesSet(linksSetsStorageName,linksSetsStorageName) ) {
        EG_LOG_STUB << "INIT: Not found links metadata storage, create new: " << linksSetsStorageName << FN;
        createStdLinkBlueprints();
        createLinksMetainfo();
    }
    LoadLinksInfo();
}

inline void EgDatabase::createNodesMetainfo() { // , , 
    CreateNodeBlueprint(nodesSetsStorageName);
    AddNodeDataField(nodesSet_FieldName);
    AddNodeDataField(nodesBPName_FieldName);
    AddNodeDataField(glamourBPName_FieldName);
    CommitNodeBlueprint();
}

inline void EgDatabase::createLinksMetainfo() { // , , 
    CreateNodeBlueprint(linksSetsStorageName);
    AddNodeDataField(linksSetName_FieldName);
    AddNodeDataField(linksBPName_FieldName);
    AddNodeDataField(linksFromID_FieldName);
    AddNodeDataField(linksToID_FieldName);
    CommitNodeBlueprint();
}

int  EgDatabase::CreateBlueprintByTemplate(const std::string& oldBPName, const std::string& newBPName, std::unordered_set<std::string>& addFields) {
    EgDataNodeBlueprint oldBlueprint(oldBPName);
    EgDataNodeBlueprint newBlueprint(newBPName);    
    bool oldFlag = oldBlueprint.LocalLoadBlueprint(); // check if exists
    bool newFlag = newBlueprint.BlueprintInitStart(); // check if NOT exists
    if ( ! (oldFlag && newFlag) ) {
        EG_LOG_STUB << "ERROR: can't clone node blueprint, oldBPName: " << oldBPName << " newBPName: " << newBPName << FN;
        return -1;
    }
    for (auto& fieldsIter : oldBlueprint.dataFieldsNames) // add old fields
        newBlueprint.AddDataFieldName(fieldsIter.first);
        // std::cout << "fieldName: \"" << fieldsIter.first;
    for (auto& field : addFields) // add extra fields
        newBlueprint.AddDataFieldName(field);
    newBlueprint.BlueprintInitCommit();
    // PrintDataNodeBlueprint(newBlueprint);
    return 0;
}

EgDataNodesSet* EgDatabase::GetNodesTypePtrByID(EgNodesSetIDType nodeTypeID) { // FIXME implement serialDataPtr on all types load
    return static_cast < EgDataNodesSet* > (nodesTypesMetainfo.nodesContainer->GetNodePtrByID (nodeTypeID)-> serialDataPtr);
}

int EgDatabase::AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName) {
    // EG_LOG_STUB << "nodesSetName: " << nodesSetName << " blueprintName: " << blueprintName << FN;
    EgNodesSetIDType stubID;
    if (!nodeTypeIDByName(nodesSetName, stubID)) { // metainfo Data Exists
        EG_LOG_STUB << "ERROR : nodes set name already exists : " << nodesSetName << FN;
        return -1;
    }
    // EG_LOG_STUB << "typeName : " << typeName << FN;
    EgDataNode* newNode = new EgDataNode(nodesTypesMetainfo.dataNodeBlueprint);
    // blueprintID = nodesTypesMetainfo.nodesContainer-> GetLastID() + 1;
    (*newNode)[nodesSet_FieldName] << nodesSetName;
    (*newNode)[nodesBPName_FieldName]  << blueprintName;
    nodesTypesMetainfo << newNode;
    nodesTypesMetainfo.Store();
    return 0;
}
/*
int EgDatabase::AddDataNodesTypeInfo(const std::string& nodesSetName, const std::string& blueprintName, const std::string& glamBPName) {
    // EG_LOG_STUB << "nodesSetName: " << nodesSetName << " blueprintName: " << blueprintName << FN;
    EgNodesSetIDType stubID;
    if (!nodeTypeIDByName(nodesSetName, stubID)) { // metainfo Data Exists
        EG_LOG_STUB << "ERROR : nodes set name already exists : " << nodesSetName << FN;
        return -1;
    }
    // EG_LOG_STUB << "typeName : " << typeName << FN;
    EgDataNode* newNode = new EgDataNode(nodesTypesMetainfo.dataNodeBlueprint);
    // blueprintID = nodesTypesMetainfo.nodesContainer-> GetLastID() + 1;
    (*newNode)[nodesSet_FieldName]  << nodesSetName;
    (*newNode)[nodesBPName_FieldName]   << blueprintName;
    (*newNode)[glamourBPName_FieldName] << glamBPName;
    nodesTypesMetainfo << newNode;
    nodesTypesMetainfo.Store();
    return 0;
} */

int EgDatabase::AddLinksTypeInfo(const std::string& linksSetName, const std::string& blueprintName, 
                                 const std::string& nodesFrom, const std::string& nodesTo) {
    EgNodesSetIDType stubID;
    if (!linkTypeIDByName(linksSetName, stubID)) { // metainfo Data Exists
        EG_LOG_STUB << "ERROR : linkss set name already exists : " << linksSetName << FN;
        return -1;
    }
    EgDataNode* newNode = new EgDataNode(linksTypesMetainfo.dataNodeBlueprint);
    // blueprintID = linksTypesMetainfo.nodesContainer-> lastNodeID + 1; // FIXME check if needed
    (*newNode)[linksSetName_FieldName]  << linksSetName;
    (*newNode)[linksBPName_FieldName]   << blueprintName;
    (*newNode)[linksFromID_FieldName] << nodesFrom;
    (*newNode)[linksToID_FieldName]   << nodesTo;
    linksTypesMetainfo << newNode;
    linksTypesMetainfo.Store();
    return 0;
}

void EgDatabase::LoadDataNodesSetsInfo() {
    if (! nodesTypesMetainfo.isConnected)
            nodesTypesMetainfo.ConnectSystemNodesSet(nodesSetsStorageName, nodesSetsStorageName);
    if (! nodesTypesMetainfo.isDataLoaded)
            nodesTypesMetainfo.LoadAllNodes();
}

void EgDatabase::LoadLinksInfo() {
    if (! linksTypesMetainfo.isConnected)    
        linksTypesMetainfo.ConnectSystemNodesSet(linksSetsStorageName, linksSetsStorageName);
    if (! linksTypesMetainfo.isDataLoaded)
            linksTypesMetainfo.LoadAllNodes();
}

bool EgDatabase::nodeTypeIDByName(const std::string& typeName, EgNodesSetIDType& nodeTypeID) {
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[nodesSet_FieldName] >> currentName;
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
        // EG_LOG_STUB << "nodesSetName : " << nodesSetName << FN;
        LoadDataNodesSetsInfo();
        for (auto nodesIter : nodesTypesMetainfo.nodesContainer-> dataNodes) {
            std::string currentName;
            (*(nodesIter.second))[nodesSet_FieldName] >> currentName;
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == nodesSetName) {
                // nodeTypeID = nodesIter.first;
                (*(nodesIter.second))[nodesBPName_FieldName] >> nodeBlueprintName;
                return false; // typename exists
            }
        }
    // }
    return true;
}

bool EgDatabase::blueprintNameByLinkName(const std::string& linksSetName, std::string& linkBlueprintName, EgNodesSetIDType& linksSetID) {
    // EG_LOG_STUB << "check typeName : " << nodesSetName << FN;
    // if (LoadDataNodesTypesInfo() == 0) { // files exist
        LoadLinksInfo();
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {
            std::string currentName;
            (*(nodesIter.second))[linksSetName_FieldName] >> currentName;
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == linksSetName) {
                // nodeTypeID = nodesIter.first;
                (*(nodesIter.second))[linksBPName_FieldName] >> linkBlueprintName;
                linksSetID = (EgNodesSetIDType) nodesIter.first;
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
            (*(nodesIter.second))[nodesSet_FieldName] >> currentName;
            // nodesBPFieldName
            // EG_LOG_STUB << "currentName : " << currentName << FN;
            if (currentName == nodesType-> nodesSetName) {
                return nodesIter.second; // typename exists
            }
        }
    // }
    return nullptr;
}


bool EgDatabase::linkTypeIDByName(const std::string& linkName, EgNodesSetIDType& linkTypeID) {
    // if (LoadLinksInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesMetainfo.nodesContainer-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName;
            (*(nodesIter.second))[linksSetName_FieldName] >> currentName;
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

int EgDatabase::CreateNodesSetByBlueprint(const std::string& nodesSetName, const std::string& nodeBlueprintName) {
    // EG_LOG_STUB << "nodesSetName: " << nodesSetName << " nodeBlueprintName: " << nodeBlueprintName << FN;
    return AddDataNodesTypeInfo(nodesSetName, nodeBlueprintName);
}

/*
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
} */

bool EgDatabase::CreateNodeBlueprint(const std::string& blueprintName) { //,  EgNodeTypeSettings& typeSettings)
    nodeBlueprint = new EgDataNodeBlueprint(blueprintName);  // if someone failed to commit blueprint and leak mem, no big deal
    if ( ! nodeBlueprint-> BlueprintInitStart() ) { // alredy exists
        EG_LOG_STUB << "ERROR: Data node blueprint already exists: " << blueprintName << FN;
        CreateNodeBlueprintFlag = false; // abort next ops
        delete nodeBlueprint;
        nodeBlueprint = nullptr;
        return false;
    }
    CreateNodeBlueprintFlag = true; // ok to add fields
    // EG_LOG_STUB << "newBlueprint before: " << nodeBlueprint->blueprintName << " " << (int) nodeBlueprint-> blueprintID << FN;
    return true;
}

void EgDatabase::AddNodeDataField(const std::string& fieldName) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddDataFieldName(fieldName);
    if (CreateLinkBlueprintFlag) {
        EG_LOG_STUB << "WARNING: AddNodeDataField() called inside link blueprint setup: ";
        if (linkBlueprint)
            EG_LOG_STUB << linkBlueprint-> blueprintName << FN;
        EG_LOG_STUB << FN;
    }
}

void EgDatabase::AddIndex(const std::string& indexName, uint8_t indexSizeBytes, uint8_t indexSubType) {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> AddIndex(indexName, indexSizeBytes, indexSubType);
}

void EgDatabase::CommitNodeBlueprint() {
    if (CreateNodeBlueprintFlag)
        nodeBlueprint-> BlueprintInitCommit();
    delete nodeBlueprint;
    nodeBlueprint = nullptr;
    CreateNodeBlueprintFlag = false;
}

inline void EgDatabase::createStdLinkBlueprints() {
    if (! CreateLinkBlueprint(stdBoundLinkBPName)) { // BP not exists yet
        // AddLinkDataField(linksFromID_FieldName);
        // AddLinkDataField(linksToID_FieldName);
        CommitBoundLinkBlueprint();
    }
    if (! CreateLinkBlueprint(stdFreeLinkBPName)) { // BP not exists yet
        // AddLinkDataField(linksFromID_FieldName);
        // AddLinkDataField(linksToID_FieldName);
        // AddLinkDataField(linksFromSetName_FieldName);
        // AddLinkDataField(linksToSetName_FieldName);
        CommitFreeLinkBlueprint();
    }
}

int EgDatabase::CreateStdBoundLinkSet(const std::string& linksSetName, const std::string& nodesFrom, const std::string& nodesTo) {
    /* if (CreateLinkBlueprint(linksSetName))
        return -1;
    AddLinkDataField(linksFromID_FieldName);
    AddLinkDataField(linksToID_FieldName);
    CommitLinkBlueprint(); */
    int res = AddLinksTypeInfo(linksSetName, stdBoundLinkBPName, nodesFrom, nodesTo);
    // EG_LOG_STUB << "newBlueprint after: " << linkBlueprint->blueprintName << " " << (int)linkBlueprint->blueprintID << FN;
    if (! res )
        linksTypesMetainfo.Store();
    return res;
}

int EgDatabase::CreateLinkBlueprint(const std::string& linksBlueprintName) {
    std::string fullLinkName = linksBlueprintName + linkPostfixName;
    linkBlueprint = new EgDataNodeBlueprint(fullLinkName); // if someone forget to commit blueprint and leak mem, no big deal
    if ( ! linkBlueprint-> BlueprintInitStart() ) { // alredy exists
        EG_LOG_STUB << "ERROR: Link blueprint file already exists: " << linksBlueprintName << FN;
        CreateLinkBlueprintFlag = false; // abort next ops
        delete linkBlueprint;
        linkBlueprint = nullptr;
        return -1;
    }
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

void EgDatabase::CommitBoundLinkBlueprint() {
    if (CreateLinkBlueprintFlag) {
        linkBlueprint-> AddDataFieldName(linksFromID_FieldName);
        linkBlueprint-> AddDataFieldName(linksToID_FieldName);
        linkBlueprint-> BlueprintInitCommit();
    }
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
}

void EgDatabase::CommitFreeLinkBlueprint() {
    if (CreateLinkBlueprintFlag) {
        linkBlueprint-> AddDataFieldName(linksFromID_FieldName);
        linkBlueprint-> AddDataFieldName(linksToID_FieldName);
        linkBlueprint-> AddDataFieldName(linksFromSetName_FieldName);
        linkBlueprint-> AddDataFieldName(linksToSetName_FieldName);
        linkBlueprint-> BlueprintInitCommit();
    }
    delete linkBlueprint;
    linkBlueprint = nullptr;
    CreateLinkBlueprintFlag = false;
}

int EgDatabase::CreateFreeLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName) {
    return AddLinksTypeInfo(linksSetName, blueprintName, "", "");
}

int EgDatabase::CreateBoundLinksSetByBlueprint(const std::string& linksSetName, const std::string& blueprintName, 
                                          const std::string& nodesFrom, const std::string& nodesTo) {
    return AddLinksTypeInfo(linksSetName, blueprintName, nodesFrom, nodesTo);
}


inline bool EgDatabase::LoadLocalFileNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& retNode) {
    bool res {false};
    clear();
    EgDataNode* localNode {nullptr};
    EgFileOffsetType nextOffset{0};
    EgDataNodesLocalFileType LocalNodesFile;
    LocalNodesFile.localFileName = nodesSetName;
    LocalNodesFile.dataNodeBlueprint = nodeBlueprint;
    if (!LocalNodesFile.OpenFileToRead()) {
        EG_LOG_STUB << "ERROR: CherryPickNode() can't open file " << LocalNodesFile.localFileName << ".gdn" << FN;
        return false;
    }
    localNode = new EgDataNode(nodeBlueprint); // MEMLEAK delete outside
    LocalNodesFile.getFirstNodeOffset(nextOffset);
    while (nextOffset) { // scan all nodes FIXME TODO optimize read ID only
        localNode-> dataFileOffset = nextOffset;
        LocalNodesFile.ReadDataNodeIDOnly(localNode, nextOffset);
        // EG_LOG_STUB << "localNode-> dataNodeID: " << localNode-> dataNodeID << FN;
        if (localNode-> dataNodeID == nodeID) {
            LocalNodesFile.ReadDataNode(localNode, nextOffset); // read data
            break;
        }
    }
    if (localNode-> dataNodeID == nodeID) {
        retNode = localNode;
        res = true; // ok
    } else {
        delete localNode;
        retNode = nullptr;
        res = false;
    }
    LocalNodesFile.nodesFile.close();
    return res; // ok
}

bool EgDatabase::CherryPickNode(const std::string& nodesSetName, EgDataNodeIDType nodeID, EgDataNode*& newNode, EgDataNodeBlueprint*& newNodeBP) {
    std::string nodeBlueprintName;
    newNode = nullptr;
    newNodeBP = nullptr;
    if (blueprintNameByNodeName(nodesSetName, nodeBlueprintName)) { // false if BP exists
        EG_LOG_STUB << "ERROR: nodes set not found: " << nodesSetName << FN;
        return false;
    }
    nodeBlueprint = new EgDataNodeBlueprint(nodeBlueprintName); // load blueprint MEMLEAK delete outside
    if ( ! nodeBlueprint-> LocalLoadBlueprint() ) {
        EG_LOG_STUB << "ERROR: can't open data nodes blueprint file: " << nodeBlueprintName << ".dnl" << FN;
        delete nodeBlueprint;
        nodeBlueprint = nullptr;
        return false;
    }    
    // check if index exists FIXME TODO
    // get node by index
    bool res = LoadLocalFileNode(nodesSetName, nodeID, newNode); // scan nodeID in local file
    if ( ! res ) { // fail to load 
        delete nodeBlueprint;
    } else
        newNodeBP = nodeBlueprint; // for delete after use
    nodeBlueprint = nullptr;
    return res;
}