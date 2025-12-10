#pragma once

#include "egIndexes.h"
#include "../service/egByteArray.h"
// #include <set>

namespace egIndexConditionsNamespace {
    /*const char *const strEQ("==");
    const char *const strGE(">=");
    const char *const strLE("<=");
    const char *const strGT(">");
    const char *const strLT("<");
    const char *const strNE("!=");*/

    const int EQ{1}; // ("==");
    const int GE{2}; // (">=");
    const int LE{3}; // ("<=");
    const int GT{4}; // (">");
    const int LT{5}; // ("<");
    const int NE{6}; // ("!=");

    const int AND{7}; // ("&&");
    const int OR{8};  // ("||");
    const int NOT{9}; // ("!");
} // namespace

typedef uint16_t EgIndexConditionsCountType;
/*
class EgIndexConditionAbstractType {
public:
    virtual ~EgIndexConditionAbstractType() {}

    virtual void calcOffsets() { std::cout << "ERROR: calcOffsets() of abstract class called" << std::endl; }
};*/

class EgIndexConditionType { // logical node envelope
public:
    EgIndexConditionsCountType leafsCount = 0; // bool isLeaf
    std::set <uint64_t> subSet;
    EgIndexesAbstractType* myIndex {nullptr};
    int16_t logicOper;       // logical operation type - EQ GT LE
    // EgByteArrayAbstractType* keyBA;

    EgIndexConditionType* left   {nullptr};
    EgIndexConditionType* right  {nullptr};
    EgIndexConditionType* parent {nullptr};

    EgIndexConditionType() {}
    EgIndexConditionType(std::string a_FieldName, int an_oper);
    EgIndexConditionType(std::string a_FieldName, std::string str_oper);
    ~EgIndexConditionType() {}

    EgIndexConditionType& operator && (const EgIndexConditionType rvalue);
    EgIndexConditionType& operator || (const EgIndexConditionType rvalue);

    void calcOffsets();
};

using IC = EgIndexConditionType;
// template <typename T> using IC = EgIndexConditionType<T>; // alias

class EgIndexConditionsTree // conditional nodes tree operations
{
public:
    EgIndexConditionType* root {nullptr}; // EgIndexCondition*

    void recursiveCalcOffsets()  {} // FIXME STUB
    void recursiveClearOffsets() {}
    void recursiveDeleteNodes()  {}
    // int ralcTreeSet();       // calc final set of tree
    // void clearOffsets();     // clear offsets sets in nodes

    ~EgIndexConditionsTree() { if (root) recursiveDeleteNodes(); }
};