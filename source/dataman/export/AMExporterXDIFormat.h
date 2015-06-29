#ifndef AMEXPORTERXDIFORMAT_H
#define AMEXPORTERXDIFORMAT_H

#include "dataman/export/AMExporterGeneralAscii.h"

/// This class implements the XDI XAS file format.
/*!
	This class uses the XAS Data Interchange Specification (XDI) which is meant
	to act as a global specification that XAS analysis suites can trust behave
	a certain way.  This implements all the required components of the grammar.
  */
class AMExporterXDIFormat : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMExporterXDIFormat(QObject *parent = 0);
	/// Destructor.
	virtual ~AMExporterXDIFormat();

	/// Description of the exporter.
	virtual QString description() const;
	/// Verbose description of the exporter.
	virtual QString longDescription() const;

	/// Exports the scan.
	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex = 0);
	/// The exporter option class name.
	virtual QString exporterOptionClassName() const { return "AMExporterOptionXDIFormat"; }
	/// Creates the default XDI exporter option.
	virtual AMExporterOption *createDefaultOption() const;

protected:
	/// Method that writes the header piece of the exported file.
	virtual void writeHeader();
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
	/// Method that writes the separate sections of the data file.  Athena doesn't like separate sections, so this function is castrated.
	virtual void writeSeparateSections();
};

#endif // AMEXPORTERXDIFORMAT_H
