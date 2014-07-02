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
