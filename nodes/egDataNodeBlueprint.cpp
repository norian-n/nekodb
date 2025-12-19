#include <iostream>
#include "egDataNodeBlueprint.h"
/*
EgDataNodeIDType EgDataNodeBlueprintType::getNextID() {
    nextIDchanged = true;
    return nextNodeID++;
}
*/
/*
int EgDataNodeBlueprintType::AddDataFieldsNames(std::vector<std::string> &fieldsNames, EgNodeBlueprintSettingsType &settings) {
    if (BlueprintInitStart()) // commit not initiated
        return -1;
    for (auto fieldName : fieldsNames)
        AddDataFieldName(fieldName);
    blueprintSettings = settings;
    BlueprintInitCommit();
    return 0;
} */

int EgDataNodeBlueprintType::BlueprintInitStart() {
    blueprintFile.fileName = blueprintName + ".dnl";
    if (blueprintFile.checkIfExists())
    {
        std::cout << "ERROR: Can't create DataNodeBlueprint: \"" << blueprintName << "\" - file already exists, use data pumping" << std::endl;
        return -1;
    }
    else
    {
        fieldsCount = 0;
        dataFieldsNames.clear();
        // indexedFields.clear();
        blueprintMode = egBlueprintInit;
    }
    return 0;
}

void EgDataNodeBlueprintType::AddDataFieldName(std::string fieldName) { // , uint8_t indexSizeBytes, uint8_t indexSubType) {
    if (blueprintMode != egBlueprintInit)
        std::cout << "ERROR: Can't add field \"" << fieldName << "\" to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
    else
        dataFieldsNames.insert(std::make_pair(fieldName, fieldsCount++));
    // std::cout  << "AddDataField name: " << fieldName << " index: " << fieldsCount-1 << " to DataNodeBlueprint: " << blueprintName << std::endl;
}

void EgDataNodeBlueprintType::AddIndex(std::string indexName, uint8_t indexSizeBytes, uint8_t indexSubType) {
    if (blueprintMode != egBlueprintInit)
        std::cout << "ERROR: Can't add index \"" << indexName << "\" to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
    else {
        if (dataFieldsNames.contains(indexName))
        {
            EgIndexSettingsType indexSettings;
            indexSettings.indexSizeBytes = indexSizeBytes;
            indexSettings.indexFamilyType = indexSubType;
            indexedFields.insert(std::make_pair(indexName, indexSettings));
            // std::cout  << "AddDataField name: " << fieldName << " index: " << fieldsCount-1 << " to DataNodeBlueprint: " << blueprintName << std::endl;
        } else
            std::cout << "ERROR: AddIndex() for DataNodeBlueprint: \"" << blueprintName << "\" field name \"" << indexName << "\" not found" << std::endl;
    }
}

void EgDataNodeBlueprintType::BlueprintInitCommit() {
    if (blueprintMode == egBlueprintInit)
    {
        LocalStoreBlueprint();
        blueprintMode = egBlueprintActive;
    }
    else
        std::cout << "ERROR: Can't commit fields to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
}

inline void EgDataNodeBlueprintType::writeDataFieldsNames() {
    // blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)dataFieldsNames.size());
    for (auto fieldsIter : dataFieldsNames)
    { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // blueprintFile.writeType<EgFieldNameLengthType>((EgFieldNameLengthType) fieldsIter.first.size());
        blueprintFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        blueprintFile.fileStream << fieldsIter.first;
        blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.second);
    }
}

inline void EgDataNodeBlueprintType::readDataFieldsNames() {
    // EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType order{0};
    std::string fieldName;
    // blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    // std::cout << "readDataFieldsNames() fieldsCountTmp: " << std::dec << (int) fieldsCountTmp << std::endl;
    for (EgFieldsCountType i = 0; i < fieldsCount; i++)
    {
        fieldName.clear();
        blueprintFile >> fieldName;
        blueprintFile.readType<EgFieldsCountType>(order);
        // std::cout << "fieldName: " << fieldName << std::endl;
        dataFieldsNames.insert(std::make_pair(fieldName, order));
    }
}

inline void EgDataNodeBlueprintType::writeIndexedFields() {
    blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)indexedFields.size());
    for (auto fieldsIter : indexedFields)
    {
        blueprintFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        blueprintFile.fileStream << fieldsIter.first;
        blueprintFile << fieldsIter.second.indexFamilyType;
        blueprintFile << fieldsIter.second.indexSizeBytes;
    }
}

inline void EgDataNodeBlueprintType::readIndexesFields() {
    EgFieldsCountType fieldsCountTmp{0};
    std::string indexName;
    EgIndexSettingsType indexSettings;
    blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++)
    {
        indexName.clear();
        blueprintFile >> indexName;
        blueprintFile >> indexSettings.indexFamilyType;
        blueprintFile >> indexSettings.indexSizeBytes;
        indexedFields.insert(std::make_pair(indexName, indexSettings));
    }
}

int EgDataNodeBlueprintType::LocalStoreBlueprint() {
    blueprintFile.fileName = blueprintName + ".dnl";
    blueprintFile.openToWrite();

    blueprintFile << fieldsCount;
    // std::cout << "LocalStoreBlueprint() fieldsCount: " << std::dec << (int) fieldsCount << std::endl;
    // blueprintFile << nodesCount;
    // blueprintFile << nextNodeID;

    blueprintFile << blueprintSettings.isServiceType;
    blueprintFile << blueprintSettings.useLinks;
    blueprintFile << blueprintSettings.useSubGraph;
    blueprintFile << blueprintSettings.useEntryNodes;
    blueprintFile << blueprintSettings.useVisualSpace;
    blueprintFile << blueprintSettings.useNamedAttributes;
    blueprintFile << blueprintSettings.useGUIsettings;

    writeDataFieldsNames();
    writeIndexedFields();

    blueprintFile.close();
    return 0;
}

int EgDataNodeBlueprintType::LocalLoadBlueprint() {
    clear();

    blueprintFile.fileName = blueprintName + ".dnl";
    if (!blueprintFile.openToRead())
        return -1;

    blueprintFile >> fieldsCount;

    // std::cout << "LocalLoadBlueprint() fieldsCount: " << std::dec << (int) fieldsCount << std::endl;
    // blueprintFile >> nodesCount;
    // blueprintFile >> nextNodeID;

    blueprintFile >> blueprintSettings.isServiceType;
    blueprintFile >> blueprintSettings.useLinks;
    blueprintFile >> blueprintSettings.useSubGraph;
    blueprintFile >> blueprintSettings.useEntryNodes;
    blueprintFile >> blueprintSettings.useVisualSpace;
    blueprintFile >> blueprintSettings.useNamedAttributes;
    blueprintFile >> blueprintSettings.useGUIsettings;

    // std::cout << "Names file pos: " << std::hex << (int) blueprintFile.fileStream.tellg() << std::endl;
    readDataFieldsNames();
    readIndexesFields();

    blueprintFile.close();
    return 0;
}

bool EgDataNodeBlueprintType::isIndexedField(std::string& name) {
    return indexedFields.contains(name);
}

// ======================== Debug ========================

void PrintDataNodeBlueprint(EgDataNodeBlueprintType& blueprint) {
    std::cout << "fieldsCount: " << std::dec << blueprint.fieldsCount;    
    // std::cout << "nodesCount: " << std::dec << blueprint.nodesCount;
    // std::cout << " nextNodeID: " << blueprint.nextNodeID << std::endl;

    /* std::cout << "isServiceType: " << blueprint.blueprintSettings.isServiceType;
    std::cout << " useLinks: " << blueprint.blueprintSettings.useLinks;
    std::cout << " useSubGraph: " << blueprint.blueprintSettings.useSubGraph;
    std::cout << " useEntryNodes: " << blueprint.blueprintSettings.useEntryNodes;
    std::cout << " useVisualSpace: " << blueprint.blueprintSettings.useVisualSpace;
    std::cout << " useNamedAttributes: " << blueprint.blueprintSettings.useNamedAttributes;
    std::cout << " useGUIsettings: " << blueprint.blueprintSettings.useGUIsettings << std::endl; */
    // fields
    std::cout << "Fields (" << blueprint.dataFieldsNames.size() << "):" << std::endl;
    for (auto fieldsIter : blueprint.dataFieldsNames)
    { // [first, second] = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        std::cout << "fieldName: \"" << fieldsIter.first;
        std::cout << "\" fieldNum: " << (int) fieldsIter.second;
        std::cout << std::endl;
    }
    // indexes
    std::cout << "Indexes (" << blueprint.indexedFields.size() << "):" << std::endl;
    for (auto indIter : blueprint.indexedFields)
    { // [first, second] = indexedFields.begin(); indIter != indexedFields.end(); ++indIter) {
        std::cout << "indexName: \"" << indIter.first;
        std::cout << "\" indexFamilyType: " << (int) indIter.second.indexFamilyType;
        std::cout << " indexSizeBytes: "  << (int) indIter.second.indexSizeBytes;
        // std::cout << " index hashFunctionID: " << indIter.second.hashFunctionID;
        std::cout << std::endl;
    }
}