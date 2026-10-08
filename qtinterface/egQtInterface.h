#pragma once

#include "../service/egByteArray.h"
#include "../nodes/egDataNodesSet.h"
#include "../guisupport/egRectangles.h" // pipe to rabbits hole for Qt apps

#include <QByteArray>
#include <QList>
#include <QVariant>
#include <QString>

void ByteArrayToQtByteArray(EgByteArrayAbstractType& byteArray, QByteArray& qtBA);
void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayAbstractType& byteArray);

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayAbstractType& byteArray);
EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QByteArray& qtBA);

// void egDataNodeFromList(EgDataNode& newNode, QList<QVariant>& addValues);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QString& qtStr);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, QString& qtStr);

void byteArrayFromQtStr(EgByteArrayAbstractType& byteArray, QString& qtStr);

// EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum);
// EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum);

// EgDataNode& operator << (EgDataNode& dataNode, QString& qtStr);
