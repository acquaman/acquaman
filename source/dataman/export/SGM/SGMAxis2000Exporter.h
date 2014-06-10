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
