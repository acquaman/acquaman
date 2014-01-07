#include "VESPERSExporter3DAscii.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERS3DScanConfiguration.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

 VESPERSExporter3DAscii::~VESPERSExporter3DAscii(){}
VESPERSExporter3DAscii::VESPERSExporter3DAscii(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
}

bool VESPERSExporter3DAscii::prepareDataSources()
{
	// This is a much stricter function than AMExporterGeneralAscii.  Only 3D data sources are allowed (and when we figure out the spectra files, then also 3D).
	mainTableDataSources_.clear();
	mainTableIncludeX_.clear();
	separateSectionDataSources_.clear();
	separateSectionIncludeX_.clear();
	separateFileDataSources_.clear();
	separateFileIncludeX_.clear();

	if (option_->includeAllDataSources() && option_->firstColumnOnly()){

		bool includeFirstColumn = true;

		for (int i = 0; i < currentScan_->dataSourceCount(); i++){

			switch(currentScan_->dataSourceAt(i)->rank()){
			// No 0D or 1D data sources.
			case 0:
			case 1:
			case 2:
				return false;

			case 3:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << includeFirstColumn;  // X and Y and Z.

				if (includeFirstColumn)
					includeFirstColumn = false;

				break;

			case 4:
				if (option_->includeHigherDimensionSources())
					separateFileDataSources_ << i;
				break;
			}
		}
	}

	// There isn't much to a simple 3D map.  Put the X, Y, and Z in the first column and then put all the rest of the data.  I don't have any other options right now.
	else
		return false;

	return true;
}

QString VESPERSExporter3DAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
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

	if(!openFile(mainFileName_)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "Export failed: Could not open the file '" % mainFileName_ % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
		return QString();
	}


	writeHeader();
	writeMainTable();
	writeSeparateSections();
	if(option_->includeHigherDimensionSources() && !writeSeparateFiles(destinationFolderPath)) {
		file_->close();
		return QString();
	}

	file_->close();
	return mainFileName_;
}

void VESPERSExporter3DAscii::writeHeader()
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

void VESPERSExporter3DAscii::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	if(option_->columnHeaderIncluded()) {

		ts << "# ";

		for(int c=0; c<mainTableDataSources_.count(); c++) {

			setCurrentDataSource(mainTableDataSources_.at(c));

			if(mainTableIncludeX_.at(c))
				ts << currentScan_->rawData()->scanAxisAt(0).name << option_->columnDelimiter() << currentScan_->rawData()->scanAxisAt(1).name << option_->columnDelimiter() << currentScan_->rawData()->scanAxisAt(2).name << option_->columnDelimiter();

			ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
		}
	}

	ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();

	// 2. rows
	VESPERS3DScanConfiguration *config = qobject_cast<VESPERS3DScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	if (!config)
		return;

	// This will return -1 if it fails.  This means any checks inside this loop will always fail if the CCD was not included.
	int indexOfCCDName = currentScan_->indexOfDataSource("CCDFileNumber");
	QString ccdFileName = config->ccdFileName();
	int zRange = currentScan_->scanSize(2);
	int yRange = currentScan_->scanSize(1);
	int xRange = currentScan_->scanSize(0);

	QString ccdString;

	if (config->ccdDetector() == VESPERS::Roper)
		ccdString = ccdFileName % "_%1.spe";

	else if (config->ccdDetector() == VESPERS::Mar)
		ccdString = ccdFileName % "_%1.tif";

	else if (config->ccdDetector() == VESPERS::Pilatus)
		ccdString = ccdFileName % "-%1.tif";
	else
		ccdString = "";

	for(int y = 0; y < yRange; y++) {

		for (int x = 0; x < xRange; x++){

			for (int z = 0; z < zRange; z++){

				// over rows within columns
				for(int c=0; c<mainTableDataSources_.count(); c++) {
					setCurrentDataSource(mainTableDataSources_.at(c));
					AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);

					// print x and y column?
					if(mainTableIncludeX_.at(c)) {

						ts << ds->axisValue(0, x).toString();
						ts << option_->columnDelimiter();
						ts << ds->axisValue(1, y).toString();
						ts << option_->columnDelimiter();
						ts << ds->axisValue(2, z).toString();
						ts << option_->columnDelimiter();
					}

					if(c == indexOfCCDName)
						ts << QString(ccdString).arg(int(ds->value(AMnDIndex(x, y, z)))-1);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
					else
						ts << ds->value(AMnDIndex(x, y, z)).toString();

					ts << option_->columnDelimiter();
				}

				ts << option_->newlineDelimiter();
			}
		}
	}

	ts << option_->newlineDelimiter();
}

void VESPERSExporter3DAscii::writeSeparateSections()
{
	// Castrated function.
}

bool VESPERSExporter3DAscii::writeSeparateFiles(const QString &destinationFolderPath)
{
	if (option_->higherDimensionsInRows()){

		for (int s = 0, sSize = separateFileDataSources_.size(); s < sSize; s++) {

			setCurrentDataSource(separateFileDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* source = currentScan_->dataSourceAt(currentDataSourceIndex_);

			QFile output;
			QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName() );

			if(!openFile(&output, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			int spectraSize = source->size(3);
			QString columnDelimiter = option_->columnDelimiter();
			QString newLineDelimiter = option_->newlineDelimiter();
			QTextStream out(&output);

			for (int y = 0, ySize = source->size(1); y < ySize; y++){

				for (int x = 0, xSize = source->size(0); x < xSize; x++){

					for (int z = 0, zSize = source->size(2); z < zSize; z++){

						QVector<double> data(spectraSize);
						source->values(AMnDIndex(x, y, z, 0), AMnDIndex(x, y, z, spectraSize-1), data.data());

						for (int i = 0; i < spectraSize; i++)
							out << data.at(i) << columnDelimiter;

						out << newLineDelimiter;
					}
				}
			}

			output.close();
		}
	}

	else{

		for (int s = 0, sSize = separateFileDataSources_.size(); s < sSize; s++) {

			setCurrentDataSource(separateFileDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* source = currentScan_->dataSourceAt(currentDataSourceIndex_);

			QFile output;
			QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName() );

			if(!openFile(&output, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			int spectraSize = source->size(2);
			QString columnDelimiter = option_->columnDelimiter();
			QString newLineDelimiter = option_->newlineDelimiter();
			QTextStream out(&output);

			for (int i = 0; i < spectraSize; i++){

				for (int y = 0, ySize = source->size(1); y < ySize; y++)
					for (int x = 0, xSize = source->size(0); x < xSize; x++)
						out << double(source->value(AMnDIndex(x, y, i))) << columnDelimiter;

				out << newLineDelimiter;
			}

			output.close();
		}
	}

	return true;
}
