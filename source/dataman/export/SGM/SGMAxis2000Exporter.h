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


#ifndef SGMAXIS2000EXPORTER_H
#define SGMAXIS2000EXPORTER_H

#include "dataman/export/AMExporterGeneralAscii.h"

class SGMAxis2000Exporter : public AMExporterGeneralAscii
{
Q_OBJECT
public:
 	virtual ~SGMAxis2000Exporter();
    Q_INVOKABLE SGMAxis2000Exporter(QObject *parent = 0);

    virtual QString description() const { return "aXis 2000 SGM (plain text file)"; }
    virtual QString longDescription() const { return "The aXis 2000 SGM file format creates two data files. One is a header file describing the scan parameters and data. The other is the actual data in columns."; }

    virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex);
};

#endif // SGMAXIS2000EXPORTER_H
