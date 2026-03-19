#pragma once
// #include <iostream>
#include "../service/egByteArray.h"
#include "../nodes/egDataNodesSet.h"
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

/* struct egPointSize {
    int origXW; // x or width
    int origYH; // y or height
    int scaledXW;
    int scaledYH;
}; */

struct egPoint {
    int origX; // x or width
    int origY; // y or height
    int scaledX;
    int scaledY;
};

struct egSize {
    int origW; // x or width
    int origH; // y or height
    int scaledW;
    int scaledH;
};

struct egRect {
    egPoint corner;
    egSize size;
};

const int maxZoom {10};

void resetToOrigSize  (egSize& size);
void resetToOrigPoint (egPoint& point);
void resetToOrigRect  (egRect& rect);

void origToScaledScalar (int origVal, int& scaledVal, int zoomFactor);
void origToScaledCanvas (int origVal, int& scaledVal, int zoomFactor, int canvas);
void origToScaledSize   (egSize& size, int zoomFactor);
void origToScaledPoint  (egPoint& point, int zoomFactor);
void origToScaledPointCanvas  (egPoint& point, int zoomFactor, egPoint& canvas);
void origToScaledRect   (egRect& rect, int zoomFactor);
void origToScaledRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas);

void scaledToOrigScalar (int scaledVal, int& origVal, int zoomFactor);
void scaledToOrigCanvas (int scaledVal, int& origVal, int zoomFactor, int canvas);
void scaledToOrigSize   (egSize& size, int zoomFactor);
void scaledToOrigPoint(egPoint& point, int zoomFactor);
void scaledToOrigPointCanvas  (egPoint& point, int zoomFactor, egPoint& canvas);
void scaledToOrigRect   (egRect& rect, int zoomFactor);
void scaledToOrigRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas);

void scaledToOrigLayer   (egRect& rect, int zoomFactor);
void origToScaledLayer   (egRect& rect, int zoomFactor);

inline void ByteArrayToQtByteArray(EgByteArrayAbstractType& byteArray, QByteArray& qtBA);
inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayAbstractType& byteArray);

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayAbstractType& byteArray);
EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QByteArray& qtBA);

// void egDataNodeFromList(EgDataNode& newNode, QList<QVariant>& addValues);

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QString& qtStr);
EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, QString& qtStr);

// EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, int& intNum);
// EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, int intNum);

// EgDataNode& operator << (EgDataNode& dataNode, QString& qtStr);
