#include <iostream>
#include "egDataNodeBlueprint.h"
/*
EgDataNodeIDType EgDataNodeBlueprintType::getNextID() {
    nextIDchanged = true;
    return nextNodeID++;
}
*/

int EgDataNodeBlueprintType::AddDataFieldsNames(std::vector<std::string> &fieldsNames, EgNodeBlueprintSettingsType &settings) {
    if (BlueprintInitStart()) // commit not initiated
        return -1;
    for (auto fieldName : fieldsNames)
        AddDataFieldName(fieldName);
    blueprintSettings = settings;
    BlueprintInitCommit();
    return 0;
}

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
        indexedFields.clear();
        blueprintMode = egBlueprintInit;
    }
    return 0;
}

void EgDataNodeBlueprintType::AddDataFieldName(std::string fieldName) {
    if (blueprintMode != egBlueprintInit)
        std::cout << "ERROR: Can't add field \"" << fieldName << "\" to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
    else
        dataFieldsNames.insert(std::make_pair(fieldName, fieldsCount++));
    // std::cout  << "AddDataField name: " << fieldName << " index: " << fieldsCount-1 << " to DataNodeBlueprint: " << blueprintName << std::endl;
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

void EgDataNodeBlueprintType::AddIndex(std::string fieldName, EgIndexSettingsType &indexSet) {
    auto iter = dataFieldsNames.find(fieldName);
    if (iter != dataFieldsNames.end())
    {
        indexedFields.insert(std::make_pair(iter->second, indexSet));
    }
    else
        std::cout << "ERROR: AddIndex for DataNodeBlueprint: \"" << blueprintName << "\" field name \"" << fieldName << "\" not found" << std::endl;
}

inline void EgDataNodeBlueprintType::writeDataFieldsNames() {
    blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)dataFieldsNames.size());
    for (auto fieldsIter : dataFieldsNames)
    { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // blueprintFile.writeType<EgFieldNameLengthType>((EgFieldNameLengthType) fieldsIter.first.size());
        blueprintFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        blueprintFile.fileStream << fieldsIter.first;
        blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.second);
    }
}

inline void EgDataNodeBlueprintType::readDataFieldsNames() {
    EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType order{0};
    std::string fieldName;
    blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    // std::cout << "fieldsCountTmp: " << std::dec << (int) fieldsCountTmp << std::endl;
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++)
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
    { // [first, second] = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.first);
        blueprintFile << fieldsIter.second.indexFamilyType;
        blueprintFile << fieldsIter.second.indexSizeBits;
        blueprintFile << fieldsIter.second.hashFunctionID;
    }
}

inline void EgDataNodeBlueprintType::readIndexesFields() {
    EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType index{0};
    EgIndexSettingsType indexSettings;
    blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++)
    {
        blueprintFile.readType<EgFieldsCountType>(index);
        blueprintFile >> indexSettings.indexFamilyType;
        blueprintFile >> indexSettings.indexSizeBits;
        blueprintFile >> indexSettings.hashFunctionID;
        indexedFields.insert(std::make_pair(index, indexSettings));
    }
}

int EgDataNodeBlueprintType::LocalStoreBlueprint() {
    blueprintFile.fileName = blueprintName + ".dnl";
    blueprintFile.openToWrite();

    blueprintFile << fieldsCount;
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

// ======================== Debug ========================

void PrintDataNodeBlueprint(EgDataNodeBlueprintType& blueprint) {
    std::cout << "fieldsCount: " << std::dec << blueprint.fieldsCount;    
    // std::cout << "nodesCount: " << std::dec << blueprint.nodesCount;
    // std::cout << " nextNodeID: " << blueprint.nextNodeID << std::endl;

    std::cout << "isServiceType: " << blueprint.blueprintSettings.isServiceType;
    std::cout << " useLinks: " << blueprint.blueprintSettings.useLinks;
    std::cout << " useSubGraph: " << blueprint.blueprintSettings.useSubGraph;
    std::cout << " useEntryNodes: " << blueprint.blueprintSettings.useEntryNodes;
    std::cout << " useVisualSpace: " << blueprint.blueprintSettings.useVisualSpace;
    std::cout << " useNamedAttributes: " << blueprint.blueprintSettings.useNamedAttributes;
    std::cout << " useGUIsettings: " << blueprint.blueprintSettings.useGUIsettings << std::endl;
    // fields
    std::cout << "Fields (" << blueprint.dataFieldsNames.size() << "):" << std::endl;
    for (auto fieldsIter : blueprint.dataFieldsNames)
    { // [first, second] = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        std::cout << "fieldName: \"" << fieldsIter.first;
        std::cout << "\" fieldIndex: " << (int)fieldsIter.second;
        std::cout << std::endl;
    }
    // indexes
    std::cout << "Indexes (" << blueprint.indexedFields.size() << "):" << std::endl;
    for (auto indIter : blueprint.indexedFields)
    { // [first, second] = indexedFields.begin(); indIter != indexedFields.end(); ++indIter) {
        std::cout << "index fieldNum: \"" << (int)indIter.first;
        std::cout << " index indexFamilyType: " << indIter.second.indexFamilyType;
        std::cout << " index indexSizeBits: " << indIter.second.indexSizeBits;
        std::cout << " index hashFunctionID: " << indIter.second.hashFunctionID;
        std::cout << std::endl;
    }
}