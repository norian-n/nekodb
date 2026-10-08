#include "egDataNodeBlueprint.h"

bool EgDataNodeBlueprint::BlueprintInitStart() {
    blueprintFile.fileName = egDefaultFolderName + blueprintName + ".dnl";
    if (blueprintFile.checkIfExists()) {
        EG_LOG_STUB << "ERROR: data node blueprint blueprint file already exists: " << blueprintName << FN;
        return false;
    } else {
        fieldsCount = 0;
        dataFieldsNames.clear();
        // indexedFields.clear();
        blueprintInitMode = true; // egBlueprintInit;
    }
    return true;
}

void EgDataNodeBlueprint::AddDataFieldName(std::string fieldName) { // , uint8_t indexSizeBytes, uint8_t indexSubType) {
    if ( ! blueprintInitMode ) //  != egBlueprintInit)
        std::cout << "ERROR: Can't add field \"" << fieldName << "\" to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
    else
        dataFieldsNames.insert(std::make_pair(fieldName, fieldsCount++));
    // std::cout  << "AddDataField name: " << fieldName << " index: " << fieldsCount-1 << " to DataNodeBlueprint: " << blueprintName << std::endl;
}

void EgDataNodeBlueprint::AddIndex(std::string indexName, uint8_t indexSizeBytes, uint8_t indexSubType) {
    if ( ! blueprintInitMode) // != egBlueprintInit)
        std::cout << "ERROR: Can't add index \"" << indexName << "\" to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
    else if (dataFieldsNames.contains(indexName)) {
            EgIndexSettingsType indexSettings;
            indexSettings.indexSizeBytes = indexSizeBytes;
            indexSettings.indexFamilyType = indexSubType;
            indexedFields.insert(std::make_pair(indexName, indexSettings));
            // std::cout  << "AddDataField name: " << fieldName << " index: " << fieldsCount-1 << " to DataNodeBlueprint: " << blueprintName << std::endl;
         } else
            std::cout << "ERROR: AddIndex() for DataNodeBlueprint: \"" << blueprintName << "\" field name \"" << indexName << "\" not found" << std::endl;
}

void EgDataNodeBlueprint::BlueprintInitCommit() {
    if (blueprintInitMode) { //  == egBlueprintInit)
        LocalStoreBlueprint();
        blueprintInitMode = false; // egBlueprintActive;
    } else
        std::cout << "ERROR: Can't commit fields to DataNodeBlueprint: \"" << blueprintName << "\". Call BlueprintInitStart() first" << std::endl;
}

inline void EgDataNodeBlueprint::writeDataFieldsNames() {
    // blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)dataFieldsNames.size());
    for (auto fieldsIter : dataFieldsNames) {
        // blueprintFile.writeType<EgFieldNameLengthType>((EgFieldNameLengthType) fieldsIter.first.size());
        blueprintFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        blueprintFile.fileStream << fieldsIter.first;
        blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.second);
    }
}

inline void EgDataNodeBlueprint::readDataFieldsNames() {
    // EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType order{0};
    std::string fieldName;
    // blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    for (EgFieldsCountType i = 0; i < fieldsCount; i++) {
        fieldName.clear();
        blueprintFile >> fieldName;
        blueprintFile.readType<EgFieldsCountType>(order);
        // std::cout << "fieldName: " << fieldName << std::endl;
        dataFieldsNames.insert(std::make_pair(fieldName, order));
    }
}

inline void EgDataNodeBlueprint::writeIndexedFields() {
    blueprintFile.writeType<EgFieldsCountType>((EgFieldsCountType)indexedFields.size());
    for (auto fieldsIter : indexedFields) {
        blueprintFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        blueprintFile.fileStream << fieldsIter.first;
        blueprintFile << fieldsIter.second.indexFamilyType;
        blueprintFile << fieldsIter.second.indexSizeBytes;
    }
}

inline void EgDataNodeBlueprint::readIndexesFields() {
    EgFieldsCountType fieldsCountTmp{0};
    std::string indexName;
    EgIndexSettingsType indexSettings;
    blueprintFile.readType<EgFieldsCountType>(fieldsCountTmp);
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++) {
        indexName.clear();
        blueprintFile >> indexName;
        blueprintFile >> indexSettings.indexFamilyType;
        blueprintFile >> indexSettings.indexSizeBytes;
        indexedFields.insert(std::make_pair(indexName, indexSettings));
    }
}

bool EgDataNodeBlueprint::LocalStoreBlueprint() {
    blueprintFile.fileName = egDefaultFolderName + blueprintName + ".dnl";
    if (! blueprintFile.openToWrite()) {
        EG_LOG_STUB << "ERROR: cant open blueprint file: " << blueprintName << FN;
        return false;
    }
    blueprintFile << fieldsCount;
    writeDataFieldsNames();
    writeIndexedFields();
    blueprintFile.close();
    return true;
}

bool EgDataNodeBlueprint::LocalLoadBlueprint() {
    clear();

    blueprintFile.fileName = egDefaultFolderName + blueprintName + ".dnl";
    if (!blueprintFile.openToRead()) {
        // EG_LOG_STUB << "ERROR: can't open blueprint file: " << blueprintFile.fileName << FN;
        return false;
    }
    blueprintFile >> fieldsCount;
    readDataFieldsNames();
    readIndexesFields();

    blueprintFile.close();
    return true;
}

bool EgDataNodeBlueprint::isIndexedField(std::string& name) {
    return indexedFields.contains(name);
}

// ======================== Debug ========================

void PrintDataNodeBlueprint(EgDataNodeBlueprint& blueprint) {
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