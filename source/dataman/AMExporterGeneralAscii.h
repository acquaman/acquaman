#ifndef AMEXPORTERGENERALASCII_H
#define AMEXPORTERGENERALASCII_H

#include "dataman/AMExporter.h"

class AMExporterGeneralAscii : public AMExporter
{
	Q_OBJECT
public:
	Q_INVOKABLE explicit AMExporterGeneralAscii(QObject *parent = 0);

	virtual QString description() const { return "Plain Text (Adjustable)"; }
	virtual QString longDescription() const {
		return "The Plain Text file format creates one or more data tables in a plain text file.  You can choose which data sources to include, and which tables to put them in.  You can also specify what to include in an optional header, and where to put higher-dimensional (2D, 3D, etc.) data.";
	}
	virtual QString exporterOptionClassName() const { return "AMExporterOptionGeneralAscii"; }

	/// \todo
	virtual bool isValidFor(const AMScan *scan, const AMExporterOption *option) const { return false; }

	/// \todo
	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option) const { return QString(); }

	virtual AMExporterOption* createDefaultOption() const;

signals:

public slots:

};

#endif // AMEXPORTERGENERALASCII_H
