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
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Invalid options specified for the XDI Format Ascii Exporter. Please report this bug to the Acquaman developers."));
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
	presetHeader = parseKeywordString(presetHeader);
	headerText.append("# ");
	headerText.append(presetHeader.replace("\n", "\n# "));

	headerText.append("#----------------------------------\n");

	QTextStream ts(file_);
	ts << headerText;
}

void AMExporterXDIFormat::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	int maxTableRows = 0;
	if(option_->columnHeaderIncluded()) {

		ts << "# ";

		QString columnText("");

		for(int c=0; c<mainTableDataSources_.count(); c++) {
			setCurrentDataSource(mainTableDataSources_.at(c));
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			if(ds->size(0) > maxTableRows)
				maxTableRows = ds->size(0); // convenient... lets figure this out while we're looping through anyway

			columnText = parseKeywordString(option_->columnHeader());
			columnText.replace(" ", "");
			columnText = columnText.left(columnText.indexOf("("));

			// 1D data sources:
			if(ds->rank() == 1) {

				if(mainTableIncludeX_.at(c))
					ts << (columnText + ".X")  << option_->columnDelimiter();
				ts << columnText << option_->columnDelimiter();
			}
			else {	// 2D
				if(mainTableIncludeX_.at(c))
					ts << (columnText + ".X") << option_->columnDelimiter();
				// need a loop over the second axis columns
				for(int cc=0; cc<ds->size(1); cc++) {
					setCurrentColumnIndex(cc);
					ts << columnText << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
				}
			}
		}
		ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();
	}


	// 2. rows
	for(int r=0; r<maxTableRows; r++) {

		// over rows within columns
		for(int c=0; c<mainTableDataSources_.count(); c++) {
			setCurrentDataSource(mainTableDataSources_.at(c));
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			bool doPrint = (ds->size(0) > r);

			// print x column?
			if(mainTableIncludeX_.at(c)) {
				if(doPrint)
					ts << ds->axisValue(0,r).toString(precision);
				ts << option_->columnDelimiter();
			}

			// 1D data sources:
			if(ds->rank() == 1) {
				if(doPrint)
					ts << ds->value(r).toString(precision);
				ts << option_->columnDelimiter();
			}
			else if(ds->rank() == 2) {
				// need a loop over the second axis columns
				for(int cc=0; cc<ds->size(1); cc++) {
					if(doPrint)
						ts << ds->value(AMnDIndex(r,cc)).toString(precision);
					ts << option_->columnDelimiter();
				}
			}
		}
		ts << option_->newlineDelimiter();
	}
}

void AMExporterXDIFormat::writeSeparateSections()
{
	// Castrated function.
}

AMExporterOption * AMExporterXDIFormat::createDefaultOption() const
{
	return new AMExporterOptionXDIFormat();
}


