#include "VESPERSExporter2DAscii.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

VESPERSExporter2DAscii::VESPERSExporter2DAscii(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
}

bool VESPERSExporter2DAscii::prepareDataSources()
{
	// This is a much stricter function than AMExporterGeneralAscii.  Only 2D data sources are allowed (and when we figure out the spectra files, then also 3D).
	mainTableDataSources_.clear();
	mainTableIncludeX_.clear();
	separateSectionDataSources_.clear();
	separateSectionIncludeX_.clear();
	separateFileDataSources_.clear();
	separateFileIncludeX_.clear();

	if (option_->includeAllDataSources() && option_->firstColumnOnly()){

		for (int i = 0; i < currentScan_->dataSourceCount(); i++){

			switch(currentScan_->dataSourceAt(i)->rank()){
			// No 0D or 1D data sources.
			case 0:
			case 1:
				return false;

			case 2:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << (i == 0 ? true : false); // X and Y.
			}
		}
	}

	// There isn't much to a simple 2D map.  Put the X and Y in the first column and then put all the rest of the data.  I don't have any other options right now.
	else
		return false;

	return true;
}

QString VESPERSExporter2DAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
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

void VESPERSExporter2DAscii::writeHeader()
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

	QTextStream ts(file_);
	ts << headerText;
}

void VESPERSExporter2DAscii::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	if(option_->columnHeaderIncluded()) {

		for(int c=0; c<mainTableDataSources_.count(); c++) {

			setCurrentDataSource(mainTableDataSources_.at(c));

			if(mainTableIncludeX_.at(c))
				ts << "H" << option_->columnDelimiter() << "V" << option_->columnDelimiter();

			ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
		}
	}

	ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();

	// 2. rows
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	if (!config)
		return;

	// This will return -1 if it fails.  This means any checks inside this loop will always fail if the CCD was not included.
	int indexOfCCDName = currentScan_->indexOfDataSource("CCDFileNumber");
	QString ccdFileName = config->ccdFileName();
	int yRange = currentScan_->scanSize(1);
	int xRange = currentScan_->scanSize(0);

	for(int y = 0; y < yRange; y++) {

		for (int x = 0; x < xRange; x++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {
				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);

				bool doPrint = (ds->size(0) > y);

				// print x and y column?
				if(mainTableIncludeX_.at(c)) {
					if(doPrint)
						ts << ds->axisValue(0,x).toString();
					ts << option_->columnDelimiter();

					if (doPrint)
						ts << ds->axisValue(1, y).toString();
					ts << option_->columnDelimiter();
				}

				if(doPrint && x == indexOfCCDName)
					ts << QString("%1_%2.spe").arg(ccdFileName).arg(int(ds->value(AMnDIndex(x, y))));
				else if (doPrint)
					ts << ds->value(AMnDIndex(x, y)).toString();
				ts << option_->columnDelimiter();
			}

			ts << option_->newlineDelimiter();
		}
	}

	ts << option_->newlineDelimiter();
}

void VESPERSExporter2DAscii::writeSeparateSections()
{
	// Castrated function.
}

bool VESPERSExporter2DAscii::writeSeparateFiles(const QString &destinationFolderPath)
{
	// This is cheating for the time being.
	QFileInfo sourceFileInfo(currentScan_->additionalFilePaths().first());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(AMUserSettings::userDataFolder % "/" % currentScan_->additionalFilePaths().first());

	QString originalFileName = sourceFileInfo.absoluteFilePath();
	QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName().replace("$dataSetName", "spectra") );

	if (QFile::exists(separateFileName))
		QFile::remove(separateFileName);

	return QFile::copy(originalFileName, separateFileName);
}
