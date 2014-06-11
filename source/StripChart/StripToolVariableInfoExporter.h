#ifndef STRIPTOOLVARIABLEINFOEXPORTER_H
#define STRIPTOOLVARIABLEINFOEXPORTER_H

#include <QObject>
#include <QDir>

#include "StripChart/StripToolVariable.h"
#include "StripChart/StripTool0DVariableInfo.h"

class StripToolVariableInfoExporter : public QObject
{
    Q_OBJECT
public:
    explicit StripToolVariableInfoExporter(QObject *parent = 0);

public:
    QDir appDirectory() const;
    QDir exportDirectory() const;

public slots:
    void setExportDirectory(QDir newExportDirectory);
    bool exportInfo(StripToolVariableInfo* toExport);

private:
    QString variableNameToFilename(const QString &name) const;
    QList<QString> infoToExport(StripToolVariableInfo* toExport) const;

private:
    QDir exportDirectory_;

};

#endif // STRIPTOOLVARIABLEINFOEXPORTER_H
