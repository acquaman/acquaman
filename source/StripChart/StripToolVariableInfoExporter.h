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
