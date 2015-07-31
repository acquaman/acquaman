#ifndef AMSMAKEXPORTER_H
#define AMSMAKEXPORTER_H

#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterOptionSMAK.h"

/*!
	This class extends the generic exporter to write the files in a way that can be
	read by SMAK, but also have all the information of the scan.  Because SMAK does
	not require much of the data to visualize there are two data files.  The first
	will be SMAK specific where all the necessary data is in place to be easily visualized.
	The second will be a data file identical to AMExporter2DAscii because it has
	all of the data in a nice format.  Finally, the spectra files will be in SMAK's
	format.

		- tab delimeted instead of comma
		- every row is prefaced with an index.
  */
class AMSMAKExporter : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE explicit AMSMAKExporter(QObject *parent = 0);
	/// Destructor.
	virtual ~AMSMAKExporter();

	/// Description of the exporter.
	virtual QString description() const { return "SMAK 2D Map (plain text file)"; }
	/// More verbose description of the exporter.
	virtual QString longDescription() const {
		return "The SMAK 2D Map file format creates two data files.  One is in SMAK's format and one contains one table of data in a plain text file where each data source is in a single column.  You can choose which data sources to include.";
	}

	/// The exporter option class name.
	virtual QString exporterOptionClassName() const { return "AMExporterOptionSMAK"; }
	/// Creates the default exporter option for SMAK exporters.
	virtual AMExporterOption* createDefaultOption() const;
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
	/// Method that writes the SMAK data file.
	void writeSMAKFile();

	/// The y-range.  May be smaller than the scan size due to scans being cancelled or skipped.
	int yRange_;
	/// The x-index.  This is used for the last row where scans have been cancelled or skipped.
	int xIndex_;
};

#endif // AMSMAKEXPORTER_H
