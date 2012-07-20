#include "VESPERSExporterLineScanAscii.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

VESPERSExporterLineScanAscii::VESPERSExporterLineScanAscii(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
}

QString VESPERSExporterLineScanAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
{
	setCurrentAutoIndex(autoIndex);
	setCurrentFilename(option->fileName());
	setDestinationFolderPath(destinationFolderPath);

	// prepare scan and option
	setCurrentScan(scan);
	option_ = qobject_cast<const AMExporterOptionGeneralAscii*>(option);
	if(!option_) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Invalid options specified for the General Ascii Exporter. Please report this bug to the Acquaman developers."));
		return QString();
	}

	// prepare data sources
	if(!prepareDataSources())
		return QString();

	// prepare export file
	mainFileName_ = parseKeywordString( destinationFolderPath % "/" % option->fileName() );
	qDebug() << "Wants to save as " << mainFileName_;

	if(!openFile(mainFileName_)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "Export failed: Could not open the file '" % mainFileName_ % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
		return QString();
	}


	writeHeader();
	writeMainTable();
	writeSeparateSections();
	if(!writeSeparateFiles(destinationFolderPath)) {
		file_->close();
		return QString();
	}

	file_->close();
	return mainFileName_;
}

void VESPERSExporterLineScanAscii::writeHeader()
{
	if(!option_->headerIncluded())
		return;

	QString headerText = parseKeywordString(option_->headerText());
	headerText.prepend("# ");

	normalizeLineEndings(headerText);
	if(headerText.right(1) != "\n")
		headerText.append("\n");	// ensure ends in newline
	convertNormalizedLineEndingsTo(option_->newlineDelimiter(), headerText);

	headerText.replace("\n", "\n# ");
	headerText.append("-------------------------------------\n");

	QTextStream ts(file_);
	ts << headerText;
}

void VESPERSExporterLineScanAscii::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	if(option_->columnHeaderIncluded()) {

		ts << "# ";

		for(int c=0; c<mainTableDataSources_.count(); c++) {

			setCurrentDataSource(mainTableDataSources_.at(c));

			if(mainTableIncludeX_.at(c))
				ts << currentScan_->rawData()->scanAxisAt(0).name << option_->columnDelimiter();

			ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
		}
	}

	ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();

	// 2. rows
	QString ccdFileName;
	VESPERSSpatialLineScanConfiguration *config = qobject_cast<VESPERSSpatialLineScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	VESPERSEnergyScanConfiguration *energyConfig = qobject_cast<VESPERSEnergyScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));

	if (config)
		ccdFileName = config->ccdFileName();
	else if (energyConfig)
		ccdFileName = energyConfig->ccdFileName();
	else
		return;


	// This will return -1 if it fails.  This means any checks inside this loop will always fail if the CCD was not included.
	int indexOfCCDName = currentScan_->indexOfDataSource("CCDFileNumber");
	int xRange = currentScan_->scanSize(0);

	for (int x = 0; x < xRange; x++){

		// over rows within columns
		for(int c=0; c<mainTableDataSources_.count(); c++) {
			setCurrentDataSource(mainTableDataSources_.at(c));
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);

			bool doPrint = (ds->size(0) > x);

			// print x column?
			if(mainTableIncludeX_.at(c)) {
				if(doPrint)
					ts << ds->axisValue(0,x).toString();
				ts << option_->columnDelimiter();
			}

			if(doPrint && c == indexOfCCDName)
				ts << QString("%1_%2.spe").arg(ccdFileName).arg(int(ds->value(AMnDIndex(x))));
			else if (doPrint)
				ts << ds->value(AMnDIndex(x)).toString();

			ts << option_->columnDelimiter();
		}

		ts << option_->newlineDelimiter();
	}

	ts << option_->newlineDelimiter();
}

void VESPERSExporterLineScanAscii::writeSeparateSections()
{
	// Castrated function.
}
