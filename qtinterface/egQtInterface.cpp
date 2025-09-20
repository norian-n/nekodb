#include "egQtInterface.h"

inline void ByteArrayToQtByteArray(EgByteArrayAbstractType& byteArray, QByteArray& qtBA) {
    qtBA.clear();
    qtBA.append((const char *) byteArray.arrayData, (size_t) (byteArray.dataSize-1));
}

inline void QtByteArrayToByteArray(QByteArray& qtBA, EgByteArrayAbstractType& byteArray) {
    byteArray.dataSize  = (uint64_t) (qtBA.size()+1);
    byteArray.reallocDataArray();
    memcpy((void*)byteArray.arrayData, (void*) qtBA.data(), qtBA.size());
    byteArray.arrayData[byteArray.dataSize-1] = 0;
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

void egDataNodeFromList(EgDataNodeType& newNode, QList<QVariant>& addValues) {
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
}

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

EgDataNodeType& operator << (EgDataNodeType& dataNode, QString& qtStr) {
        std::string value =  qtStr.toStdString();
        dataNode << value;
        return dataNode;
}
