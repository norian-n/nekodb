#pragma once

#include <map>
#include <set>
#include <iostream>

#include "../indexes/egCoreIndexTypes.h"
#include "../service/egByteArray.h"
#include "../service/egFileType.h"
#include "../service/egHamSlicer.h"

enum EgBlueprintModeEnum
{
    egBlueprintInit,        // add fields to the blueprint
    egBlueprintActive       // use blueprint to make data nodes
    // egBlueprintDataPump  // TBD: special data copy procedure with parameters "from field => to field", indexes rebuilding
};

struct EgNodeBlueprintSettingsType
{
    bool isServiceType      { false };  // hardlinked by ID to primary data nodes container
    bool useLinks           { false };  // some service node blueprints don't use links so it's optional
    bool useSubGraph        { false };  // add detail nodes inside this node
    bool useEntryNodes      { false };  // start points for custom graph operations
    bool useVisualSpace     { false };  // visual space node apperiance info 
    bool useGUIsettings     { false };  // store GUI elements features for this nodes blueprint
    bool useNamedAttributes { false };  // flexible named attributes/properties/tags
};

struct EgFieldSettingsType {
    EgFieldsCountType index {0};
    uint8_t indexSizeBytes {0}; // 0 means not indexed
    uint8_t indexSubType   {0}; // int, float, etc.
};

//  ============================================================================

class EgDataNodeBlueprintType
{
public:
    EgBlueprintModeEnum blueprintMode {egBlueprintActive};   // blueprint can't be edited freely, data pumping on changes is required
    EgBlueprintIDType   blueprintID;                    // Data Nodes Blueprint (DNL) ID for graph DB
    std::string         blueprintName;                  // == dataNodesTypeName

    EgHamSlicerType     theHamSlicer;                   // memory allocator for data nodes (node has only blueprint out connect)
    EgFileType          blueprintFile;                  // local files load & store support (if local files used)    

    EgNodeBlueprintSettingsType    blueprintSettings;   // add-ons for blueprint type
    EgFieldsCountType              fieldsCount {0};

    std::map < std::string, EgFieldsCountType/*EgFieldSettingsType*/ > dataFieldsNames;    // map  data node field names to fields order
    std::map < std::string, EgIndexSettingsType > indexedFields;

    EgByteArrayAbstractType egNotFound;
    const char* egNotFoundStr {"<Data Not Found>"};

    EgDataNodeBlueprintType(std::string a_blueprintName): blueprintName(a_blueprintName), blueprintFile(a_blueprintName) 
        { egNotFound.dataSize = strlen(egNotFoundStr)+1; egNotFound.arrayData = (unsigned char *) egNotFoundStr; }
    ~EgDataNodeBlueprintType() { /* std::cout << "EgDataNodeBlueprintType destructor " << std::endl; */ clear(); }

    void clear() { dataFieldsNames.clear(); indexedFields.clear(); }

    int  BlueprintInitStart();
    void AddDataFieldName(std::string fieldName);
    void AddIndex(std::string indexName, uint8_t indexSizeBytes = 4, uint8_t indexSubType = egIndexesSpace::egIntFT); // default int32
    void BlueprintInitCommit();

    bool isIndexedField(std::string& name);
    
    inline void writeDataFieldsNames();
    inline void readDataFieldsNames();
    inline void writeIndexedFields();
    inline void readIndexesFields();
    
    int  LocalStoreBlueprint();
    int  LocalLoadBlueprint();
};

// ======================== Debug ========================

void PrintDataNodeBlueprint(EgDataNodeBlueprintType& blueprint);