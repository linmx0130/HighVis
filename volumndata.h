#ifndef VOLUMNDATA_H
#define VOLUMNDATA_H
#include <QString>
#include <QVector>
struct VolumnMetaData{
    QString name, type, url;
    int size[3];
    int spacing[3];
};

class VolumnData
{
public:
    VolumnData(const char* filename);
    VolumnMetaData getMetaData() const{
        return this->metadata;
    }
    QString getRawFilename() const{
        return this->rawFilename;
    }
    QByteArray getRawData() const{
        return rawData;
    }
    QVector<QVector<QVector<char>>> getParsedRawData() const;

private:
    VolumnMetaData metadata;
    QString rawFilename;
    VolumnMetaData parseMetaFile(const char* filename);
    void parseRawData();
    QByteArray rawData;
};

#endif // VOLUMNDATA_H
