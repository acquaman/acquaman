#include "StripToolVariableInfoExporter.h"
#include <QDebug>

StripToolVariableInfoExporter::StripToolVariableInfoExporter(QObject *parent) :
    QObject(parent)
{
    exportDirectory_ = 0;
}



QDir StripToolVariableInfoExporter::exportDirectory() const {
    return exportDirectory_;
}



void StripToolVariableInfoExporter::setExportDirectory(QDir newExportDirectory) {
    if (newExportDirectory.exists()) {
        exportDirectory_ = newExportDirectory;

    } else {
        qDebug() << "StripToolVariableInfoExporter :: cannot set export directory to non-existant directory. No changes made to export directory.";
    }
}



bool StripToolVariableInfoExporter::exportInfo(StripToolVariableInfo *toExport) {
    if (!exportDirectory().exists()) {
        qDebug() << "StripToolVariableInfoExporter :: cannot export variable info to a directory that doesn't exist! No changes made.";
        return false;
    }

    QString filename = variableNameToFilename(toExport->name());
    QList<QString> newFileContents = infoToExport(toExport);

    if (newFileContents.empty()) {
        qDebug() << "StripToolVariableInfoExporter :: info has no information to export. Export successful.";
        return true;
    }

    QDir::setCurrent(exportDirectory().path());
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "StripToolVariableInfoExporter :: info export failed.";
        qDebug() << file.errorString();
        return false;
    }

    QTextStream out(&file);
    foreach( QString fileEntry, newFileContents)
        out << fileEntry << "\n";

    file.close();
    return true;

    qDebug() << "StripToolVariableInfoExporter :: info export successful.";
}



QString StripToolVariableInfoExporter::variableNameToFilename(const QString &name) const {
    QString filename = name;
    return filename.replace(":", "_").append(".txt");
}



QList<QString> StripToolVariableInfoExporter::infoToExport(StripToolVariableInfo *toExport) const {
    QList<QString> infoToExport;

    if (toExport->hasName()) {
        infoToExport << "Name : " + toExport->name();
        qDebug() << "Name : " << toExport->name();
    }

    if (toExport->hasCreationDateTime())
        qDebug() << "Created : " << toExport->creationDateTime();

    if (toExport->hasDescription())
        qDebug() << "Description : " << toExport->description();

    if (toExport->hasUnits())
        qDebug() << "Units : " << toExport->units();

    if (toExport->hasCheckState())
        qDebug() << "Check state : " << toExport->checkState();

    if (toExport->hasSelectionState())
        qDebug() << "Selection state : " << toExport->selectionState();

    if (toExport->hasGranularity())
        qDebug() << "Granularity : " << toExport->granularity();

    if (toExport->hasColor())
        qDebug() << "Color : " << toExport->colorName();

    return infoToExport;
}
