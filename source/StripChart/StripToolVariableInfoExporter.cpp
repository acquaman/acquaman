/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "StripToolVariableInfoExporter.h"
#include <QDebug>

StripToolVariableInfoExporter::StripToolVariableInfoExporter(QObject *parent) :
    QObject(parent)
{
    exportDirectory_ = 0;
}



QDir StripToolVariableInfoExporter::exportDirectory() const
{
    return exportDirectory_;
}



void StripToolVariableInfoExporter::setExportDirectory(QDir newExportDirectory)
{
    if (newExportDirectory.exists()) {
        exportDirectory_ = newExportDirectory;

    } else {
        qDebug() << "StripToolVariableInfoExporter :: cannot set export directory to non-existant directory. No changes made to export directory.";
    }
}



bool StripToolVariableInfoExporter::exportInfo(StripToolVariableInfo *toExport)
{
    if (!exportDirectory().exists()) {
        qDebug() << "StripToolVariableInfoExporter :: cannot export variable info to a directory that doesn't exist! No changes made.";
        return false;
    }

    QString filename = variableNameToFilename(toExport->name());
    QList<QString> newFileContents = infoToExport(toExport);

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



QString StripToolVariableInfoExporter::variableNameToFilename(const QString &name) const
{
    QString filename = name;
    return filename.replace(":", "_").append(".txt");
}



QList<QString> StripToolVariableInfoExporter::infoToExport(StripToolVariableInfo *toExport) const
{
    QList<QString> infoToExport;

    if (toExport->hasName()) {
        infoToExport << "Name : " + toExport->name();
//        qDebug() << "Name : " << toExport->name();
    }

    if (toExport->importAutomatically() == true)
        infoToExport << "Auto import : true";
    else
        infoToExport << "Auto import : false";

    if (toExport->hasCreationDateTime()) {
        infoToExport << "Created : " + toExport->creationDateTime();
//        qDebug() << "Created : " << toExport->creationDateTime();
    }

    if (toExport->hasDescription()) {
        infoToExport << "Description : " + toExport->description();
        qDebug() << "Description : " << toExport->description();
    }

    if (toExport->hasUnits()) {
        infoToExport << "Units : " + toExport->units();
//        qDebug() << "Units : " << toExport->units();
    }

    if (toExport->hasGranularity()) {
        infoToExport << "Granularity : " + QString::number(toExport->granularity());
//        qDebug() << "Granularity : " << toExport->granularity();
    }

    if (toExport->hasAverageOverPoints()) {
        infoToExport << "Average : " + QString::number(toExport->averageOverPoints());
    }

    if (toExport->hasColor()) {
        infoToExport << "Color : " + toExport->colorName();
//        qDebug() << "Color : " << toExport->colorName();
    }

    if (toExport->hasCustomAxisMax()) {
        infoToExport << "Max : " + QString::number(toExport->customAxisMax());
    }

    if (toExport->hasCustomAxisMin()) {
        infoToExport << "Min : " + QString::number(toExport->customAxisMin());
    }

    return infoToExport;
}
