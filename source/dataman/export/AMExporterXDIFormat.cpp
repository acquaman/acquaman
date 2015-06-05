#include "AMExporterXDIFormat.h"

#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QFile>
#include <QTextStream>

AMExporterXDIFormat::AMExporterXDIFormat(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
}

AMExporterXDIFormat::~AMExporterXDIFormat()
{

}

QString AMExporterXDIFormat::description() const
{
	return "XDI Specification (XAS ASCII text file)";
}

QString AMExporterXDIFormat::longDescription() const
{
	return "The XDI specification conforms to the XDI specification grammar found at (https://github.com/XraySpectroscopy/XAS-Data-Interchange/wiki/Xdispec).  It is a column based ascii text file.";
}

QString AMExporterXDIFormat::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
{
	setCurrentAutoIndex(autoIndex);
	setCurrentFilename(option->fileName());
	setDestinationFolderPath(destinationFolderPath);

	// prepare scan and option
	setCurrentScan(scan);
	option_ = qobject_cast<const AMExporterOptionXDIFormat *>(option);

	if(!option_) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Invalid options specified for the General Ascii Exporter. Please report this bug to the Acquaman developers."));
		return QString();
	}

	// prepare data sources
	if(!prepareDataSources())
		return QString();

	// prepare export file
	mainFileName_ = parseKeywordString( destinationFolderPath % "/" % option->fileName() );
	mainFileName_ = removeNonPrintableCharacters(mainFileName_);

	if(!openFile(mainFileName_)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "Export failed: Could not open the file '" % mainFileName_ % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
		return QString();
	}

	writeHeader();
	writeMainTable();

	if(option_->includeHigherDimensionSources() && !writeSeparateFiles(destinationFolderPath)) {
		file_->close();
		return QString();
	}

	file_->close();
	return mainFileName_;
}

void AMExporterXDIFormat::writeHeader()
{
	const AMExporterOptionXDIFormat *xdiOption = qobject_cast<const AMExporterOptionXDIFormat *>(option_);

	QString headerText = "# XDI/1.0\n";
	headerText.append(QString("# Element.symbol: %1\n").arg(xdiOption->elementSymbol()));
	headerText.append(QString("# Element.edge: %1\n").arg(xdiOption->elementEdge()));

	headerText.append(QString("# Column.1: energy eV\n"));

	for (int i = 0, size = mainTableDataSources_.size(); i < size; i++)
		headerText.append(QString("# Column.%1: %2\n").arg(i+2).arg(currentScan_->dataSourceAt(mainTableDataSources_.at(i))->name()));

	headerText.append("#/////////////////////////////////\n");

	QString presetHeader = option_->headerText();
	headerText.append(presetHeader.replace("\n", "\n# "));

	headerText.append("#----------------------------------\n");

	QTextStream ts(file_);
	ts << headerText;
}

void AMExporterXDIFormat::writeMainTable()
{
}

void AMExporterXDIFormat::writeSeparateSections()
{
	// Castrated function.
}


