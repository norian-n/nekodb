#include "egQtInterface.h"


void resetToOrigSize  (egSize& size) {
    size.scaledW = size.origW;
    size.scaledH = size.origH;
}
void resetToOrigPoint (egPoint& point) {
    point.scaledX = point.origX;
    point.scaledY = point.origY;
}
void resetToOrigRect  (egRect& rect) {
    resetToOrigPoint(rect.corner);
    resetToOrigSize (rect.size);
}

void origToScaledScalar (int origVal, int& scaledVal, int zoomFactor) {
    scaledVal = origVal * (maxZoom - zoomFactor)/maxZoom;
}
void scaledToOrigScalar (int scaledVal, int& origVal, int zoomFactor) {
    origVal = scaledVal * maxZoom / (maxZoom - zoomFactor);
}

void origToScaledCanvas (int origVal, int& scaledVal, int zoomFactor, int canvas) {
    scaledVal = origVal * (maxZoom - zoomFactor)/maxZoom + canvas;
}
void scaledToOrigCanvas (int scaledVal, int& origVal, int zoomFactor, int canvas) {
    origVal = (scaledVal - canvas) * maxZoom / (maxZoom - zoomFactor);
}

void origToScaledSize (egSize& size, int zoomFactor) {
    origToScaledScalar(size.origW, size.scaledW, zoomFactor);
    origToScaledScalar(size.origH, size.scaledH, zoomFactor);    
}
void scaledToOrigSize(egSize& size, int zoomFactor) {
    scaledToOrigScalar(size.scaledW, size.origW, zoomFactor);
    scaledToOrigScalar(size.scaledH, size.origH, zoomFactor);
}

void origToScaledPointCanvas (egPoint& point, int zoomFactor, egPoint& canvas) {
    origToScaledCanvas(point.origX, point.scaledX, zoomFactor, canvas.scaledX);
    origToScaledCanvas(point.origY, point.scaledY, zoomFactor, canvas.scaledY);    
}
void scaledToOrigPointCanvas(egPoint& point, int zoomFactor, egPoint& canvas) {
    scaledToOrigCanvas(point.scaledX, point.origX, zoomFactor, canvas.scaledX);
    scaledToOrigCanvas(point.scaledY, point.origY, zoomFactor, canvas.scaledY);
}

void origToScaledPoint (egPoint& point, int zoomFactor) {
    origToScaledScalar(point.origX, point.scaledX, zoomFactor);
    origToScaledScalar(point.origY, point.scaledY, zoomFactor);    
}
void scaledToOrigPoint(egPoint& point, int zoomFactor) {
    scaledToOrigScalar(point.scaledX, point.origX, zoomFactor);
    scaledToOrigScalar(point.scaledY, point.origY, zoomFactor);
}

void origToScaledRect   (egRect& rect, int zoomFactor) {
    origToScaledPoint(rect.corner, zoomFactor);
    origToScaledSize (rect.size, zoomFactor);
}
void scaledToOrigRect   (egRect& rect, int zoomFactor) {
    scaledToOrigPoint(rect.corner, zoomFactor);
    scaledToOrigSize (rect.size, zoomFactor);
}

void origToScaledRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas) {
    origToScaledPointCanvas(rect.corner, zoomFactor, canvas);
    origToScaledSize (rect.size, zoomFactor);    
}
void scaledToOrigRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas) {
    scaledToOrigPointCanvas(rect.corner, zoomFactor, canvas);
    scaledToOrigSize (rect.size, zoomFactor);    
}

void scaledToOrigLayer   (egRect& rect, int zoomFactor) {
    rect.corner.origX = 0;
    rect.corner.origY = 0;
    rect.corner.scaledX = 0;
    rect.corner.scaledY = 0;
    scaledToOrigSize (rect.size, zoomFactor);
}
void origToScaledLayer   (egRect& rect, int zoomFactor) {
    origToScaledSize (rect.size, zoomFactor);
    rect.corner.scaledX = (rect.size.origW - rect.size.scaledW) / 2;
    rect.corner.scaledY = (rect.size.origH - rect.size.scaledH) / 2;
}

inline void ByteArrayToQtByteArray(EgByteArrayAbstractType& byteArray, QByteArray& qtBA) {
    qtBA.clear();
    qtBA.append((const char *) byteArray.dataChunk, (size_t) (byteArray.dataSize));
}

inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayAbstractType& byteArray) {
    byteArray.reallocDataChunk(qtBA.size());
    memcpy((void*)byteArray.dataChunk, (void*) qtBA.data(), qtBA.size());
    // byteArray.dataChunk[byteArray.dataSize-1] = 0;
    // PrintByteArray(byteArray);
}

QByteArray& operator >> (QByteArray& qtBA, EgByteArrayAbstractType& byteArray) {
    QtByteArrayToByteArray(qtBA, byteArray);
    return qtBA;
}

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QByteArray& qtBA) {
    ByteArrayToQtByteArray(byteArray, qtBA);
    return byteArray;
}

/*
void egDataNodeFromList(EgDataNode& newNode, QList<QVariant>& addValues) {
    for (int i = 0; i < addValues.size(); ++i) {
        // std::cout << "egDataNodeFromList() value qt type: " << std::dec << addValues.at(i).type() << std::endl;
        if (addValues.at(i).type() != 10) { // int type (type 2) FIXME add other QVariant types
            int value = addValues.at(i).toInt();
            // std::cout << "egDataNodeFromList() value: " << std::dec << value << std::endl;
            newNode << value;
        } else { // string type 10
            QByteArray qArray = addValues.at(i).toByteArray();
            EgByteArraySlicerType* egArray = new EgByteArraySlicerType (newNode.dataNodeBlueprint-> theHamSlicer, qArray.size()+1);
            qArray >> *egArray;
            newNode.InsertRawByteArrayPtr(egArray);
        }
    }
} */

EgByteArrayAbstractType& operator >> (EgByteArrayAbstractType& byteArray, QString& qtStr) {
        QByteArray tmpBA;
        // PrintByteArray(byteArray);
        byteArray >> tmpBA;
        // std::cout << "tmpBA: " << tmpBA.toStdString() << std::endl;
        QString tmpStr(tmpBA);
        // std::cout << "tmpStr: " << tmpStr.toStdString() << std::endl;
        qtStr = tmpStr;
        return byteArray;
}

EgByteArrayAbstractType& operator << (EgByteArrayAbstractType& byteArray, QString& qtStr) {
        // PrintByteArray(byteArray);
        byteArray << qtStr.toStdString();
        // std::cout << "tmpBA: " << tmpBA.toStdString() << std::endl;
        // std::cout << "tmpStr: " << tmpStr.toStdString() << std::endl;
        return byteArray;
}

/*
EgDataNode& operator << (EgDataNode& dataNode, QString& qtStr) {
        std::string value =  qtStr.toStdString();
        dataNode << value;
        return dataNode;
}*/
