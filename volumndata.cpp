#include "volumndata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <string>
#include <QDebug>

VolumnMetaData VolumnData::parseMetaFile(const char *filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("Can't open file %s!", filename);
    }
    QByteArray buf = file.readAll();
    file.close();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(buf);
    QJsonObject metadata = jsonDocument.object();
    VolumnMetaData ret;
    ret.name = metadata.take("name").toString();
    ret.type = metadata.take("type").toString();
    ret.url = metadata.take("url").toString();
    QJsonArray sizeArray = metadata.take("size").toArray();
    for (int i=0;i<3;++i) {
        ret.size[i] = sizeArray.at(i).toInt();
    }
    QJsonArray spacingArray = metadata.take("spacing").toArray();
    for (int i=0;i<3;++i) {
        ret.spacing[i] = spacingArray.at(i).toInt();
    }
    return ret;
}
void VolumnData::parseRawData(){
    QFile file(this->rawFilename);
    if (!file.open(QIODevice::ReadOnly)){
        qFatal("Can't open file %s!", this->rawFilename.toStdString().c_str());
    }
    this->rawData = file.readAll();
    file.close();
}
VolumnData::VolumnData(const char* filename)
{
    metadata = parseMetaFile(filename);
    this->rawFilename = metadata.url.split('/').last();
    QString qFilename(filename);
    while(qFilename.length() > 0) {
        QChar c = qFilename.at(qFilename.length()-1);
        if (c!='/' && c!= '\\') {
            qFilename = qFilename.remove(qFilename.length()-1, 1);
        } else {
            break;
        }
    }
    qFilename = qFilename + this->rawFilename;
    this->rawFilename = qFilename;
    this->parseRawData();
}
QVector<QVector<QVector<char>>> VolumnData::getParsedRawData() const {
    QVector<QVector<QVector<char>>> ret;
    int pointer = 0;
    for (int z=0;z<this->metadata.size[2]; ++z) {
        QVector<QVector<char>> plane;
        for (int y=0;y<this->metadata.size[1]; ++y) {
            QVector<char> row;
            for (int x=0;x<this->metadata.size[0]; ++x) {
                row.push_back(rawData.at(pointer));
                pointer ++;
            }
            plane.push_back(row);
        }
        ret.push_back(plane);
    }
    return ret;
}
