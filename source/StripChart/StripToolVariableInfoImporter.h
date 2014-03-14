#ifndef STRIPTOOLVARIABLEINFOIMPORTER_H
#define STRIPTOOLVARIABLEINFOIMPORTER_H

#include <QObject>
#include <QDir>

#include "StripChart/StripToolVariableInfo.h"

class StripToolVariableInfoImporter : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariableInfoImporter(QObject *parent = 0);

signals:
    void importedInfo(StripToolVariableInfo* toAdd);

public:
    QDir importDirectory() const;
    bool infoExists(const QString &pvName) const;
    StripToolVariableInfo* loadInfoFromFile(const QString &pvName) const;

public slots:
    void setImportDirectory(QDir newImportDirectory);
    void importInfos();

private:
    QString filenameToVariableName(const QString &filename) const;
    QList<QString> getImportListFromDirectory(QDir importDirectory) const;
    void defaultSettings();

private:
    QDir importDirectory_;

};

#endif // STRIPTOOLVARIABLEINFOIMPORTER_H
