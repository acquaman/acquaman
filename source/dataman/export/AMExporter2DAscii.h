#ifndef AMEXPORTER2DASCII_H
#define AMEXPORTER2DASCII_H

#include "dataman/export/AMExporterGeneralAscii.h"

/*!
	This class extends the generic exporter to write the files in a way that is similar in style as the current dacq output.
	This is puts all the data for the data sources in single columns, even though all the data sources inside of the maps are
	all 2D data.

	Due to the simple extension to AMExporterGeneralAscii, AMExporter2DAscii uses AMExporterOptionGeneralAscii for its
	exporter option.  It does not require anything in addition to that.
  */
class AMExporter2DAscii : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE explicit AMExporter2DAscii(QObject *parent = 0);
	/// Destructor.
	virtual ~AMExporter2DAscii();

	/// Description of the exporter.
	virtual QString description() const { return "2D Map Style (plain text file)"; }
	/// More verbose description of the exporter.
	virtual QString longDescription() const {
		return "The 2D Map Style file format creates one table of data in a plain text file where each data source is in a single column.  You can choose which data sources to include.";
	}

	/// Exports the scan.
	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex = 0);

protected:
	/// Parse the data sources in option_, find their index within the scan, and fill mainTableDataSources_, separateSectionDataSources_, and separateFileDataSources_.  Returns true if all required data sources are found.
	virtual bool prepareDataSources();
	/// Method that writes the header piece of the exported file.
	virtual void writeHeader();
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
	/// Method that writes the separate sections of the data file.  Athena doesn't like separate sections, so this function is castrated.
	virtual void writeSeparateSections();
	/// Method that writes the separate files for other data sources.
	virtual bool writeSeparateFiles(const QString& destinationFolderPath);

	/// The y-range.  May be smaller than the scan size due to scans being cancelled or skipped.
	int yRange_;
	/// The x-index.  This is used for the last row where scans have been cancelled or skipped.
	int xIndex_;
};

#endif // AMEXPORTER2DASCII_H
