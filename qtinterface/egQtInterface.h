#pragma once
// #include <iostream>
#include "../service/egByteArray.h"
#include "../nodes/egDataNodesType.h"
#include <QByteArray>
#include <QList>
#include <QVariant>
#include <QString>

/*
class QByteArray {
public:
    QByteArray(const char *data, int size = -1) {}
    void clear() {}
    void append(const char *str, int len) {}
    int size()    { return 0; }
    char * data() { return nullptr; }
};
*/

inline void ByteArrayToQtByteArray(EgByteArrayAbstractType& byteArray, QByteArray& qtBA);
inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayAbstractType& byteArray);

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayAbstractType& byteArray);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QByteArray& qtBA);

void egDataNodeFromList(EgDataNodeType& newNode, QList<QVariant>& addValues);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QString& qtStr);

// EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum);
// EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum);

EgDataNodeType& operator << (EgDataNodeType& dataNode, QString& qtStr);
