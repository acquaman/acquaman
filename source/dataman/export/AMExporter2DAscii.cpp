#include "AMExporter2DAscii.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

AMExporter2DAscii::AMExporter2DAscii(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
	xIndex_ = -1;
	yRange_ = -1;
}

AMExporter2DAscii::~AMExporter2DAscii()
{

}

bool AMExporter2DAscii::prepareDataSources()
{
	// This is a much stricter function than AMExporterGeneralAscii.  Only 2D data sources are allowed (and when we figure out the spectra files, then also 3D).
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
				return false;

			case 2:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << includeFirstColumn; // X and Y.

				if (includeFirstColumn)
					includeFirstColumn = false;

				break;

			case 3:
				if (option_->includeHigherDimensionSources())
					separateFileDataSources_ << i;
				break;
			}
		}

		int xRange = currentScan_->scanSize(0);
		int yRange = currentScan_->scanSize(1);
		QVector<double> fillerData = QVector<double>(yRange);
		currentScan_->dataSourceAt(0)->values(AMnDIndex(xRange-1,0), AMnDIndex(xRange-1, yRange-1), fillerData.data());

		for (int j = 0; j < yRange && yRange_ == -1; j++)
			if (fillerData.at(j) == -1)
				yRange_ = j+1;	// The +1 is because we want the next row.

		if (yRange_ != -1){

			fillerData = QVector<double>(xRange);
			currentScan_->dataSourceAt(0)->values(AMnDIndex(0, yRange_-1), AMnDIndex(xRange-1, yRange_-1), fillerData.data());

			for (int i = 0; i < xRange && xIndex_ == -1; i++)
				if (fillerData.at(i) == -1)
					xIndex_ = i;
		}
	}

	// There isn't much to a simple 2D map.  Put the X and Y in the first column and then put all the rest of the data.  I don't have any other options right now.
	else
		return false;

	return true;
}

QString AMExporter2DAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
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
	mainFileName_ = removeNonPrintableCharacters(mainFileName_);

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

void AMExporter2DAscii::writeHeader()
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

void AMExporter2DAscii::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	if(option_->columnHeaderIncluded()) {

		ts << "# ";

		for(int c=0; c<mainTableDataSources_.count(); c++) {

			setCurrentDataSource(mainTableDataSources_.at(c));

			if(mainTableIncludeX_.at(c))
				ts << currentScan_->rawData()->scanAxisAt(0).name << option_->columnDelimiter() << currentScan_->rawData()->scanAxisAt(1).name << option_->columnDelimiter();

			ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
		}
	}

	ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();

	// 2. rows
	int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
	int xRange = currentScan_->scanSize(0);

	for(int y = 0; y < yRange; y++) {

		for (int x = 0; x < xRange; x++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {
				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
				int precision = option_->exportPrecision(ds->name());
				// print x and y column?
				if(mainTableIncludeX_.at(c)) {

					ts << ds->axisValue(0, x).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(1, y).toString(precision);
					ts << option_->columnDelimiter();
				}

				ts << ds->value(AMnDIndex(x, y)).toString(precision);

				ts << option_->columnDelimiter();
			}

			ts << option_->newlineDelimiter();
		}
	}

	if (yRange_ != -1 && xIndex_ != -1){

		for (int i = 0; i < xIndex_; i++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {
				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
				int precision = option_->exportPrecision(ds->name());

				// print x and y column?
				if(mainTableIncludeX_.at(c)) {

					ts << ds->axisValue(0,i).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(1, yRange_-1).toString(precision);
					ts << option_->columnDelimiter();
				}

				ts << ds->value(AMnDIndex(i, yRange_-1)).toString(precision);

				ts << option_->columnDelimiter();
			}

			ts << option_->newlineDelimiter();
		}
	}

	ts << option_->newlineDelimiter();
}

void AMExporter2DAscii::writeSeparateSections()
{
	// Castrated function.
}

bool AMExporter2DAscii::writeSeparateFiles(const QString &destinationFolderPath)
{
	if (option_->higherDimensionsInRows()){

		for (int s = 0, sSize = separateFileDataSources_.size(); s < sSize; s++) {

			setCurrentDataSource(separateFileDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* source = currentScan_->dataSourceAt(currentDataSourceIndex_);

			QFile output;
			QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName() );
			separateFileName = removeNonPrintableCharacters(separateFileName);

			if(!openFile(&output, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			int spectraSize = source->size(2);
			QString columnDelimiter = option_->columnDelimiter();
			QString newLineDelimiter = option_->newlineDelimiter();
			QTextStream out(&output);
			int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
			int xRange = currentScan_->scanSize(0);

			for (int y = 0; y < yRange; y++){

				for (int x = 0; x < xRange; x++){

					QVector<double> data(spectraSize);
					source->values(AMnDIndex(x, y, 0), AMnDIndex(x, y, spectraSize-1), data.data());

					for (int i = 0; i < spectraSize; i++)
						out << data.at(i) << columnDelimiter;

					out << newLineDelimiter;
				}
			}

			if (yRange_ != -1 && xIndex_ != -1){

				for (int i = 0; i < xIndex_; i++){

					QVector<double> data(spectraSize);
					source->values(AMnDIndex(i, yRange_-1, 0), AMnDIndex(i, yRange_-1, spectraSize-1), data.data());

					for (int i = 0; i < spectraSize; i++)
						out << data.at(i) << columnDelimiter;

					out << newLineDelimiter;
				}

				out << newLineDelimiter;
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
			separateFileName = removeNonPrintableCharacters(separateFileName);

			if(!openFile(&output, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			int spectraSize = source->size(2);
			QString columnDelimiter = option_->columnDelimiter();
			QString newLineDelimiter = option_->newlineDelimiter();
			QTextStream out(&output);
			int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
			int xRange = currentScan_->scanSize(0);

			for (int i = 0; i < spectraSize; i++){

				for (int y = 0; y < yRange; y++)
					for (int x = 0; x < xRange; x++)
						out << double(source->value(AMnDIndex(x, y, i))) << columnDelimiter;

				if (yRange_ != -1 && xIndex_ != -1){

					for (int x = 0; x < xIndex_; x++)
						out << double(source->value(AMnDIndex(x, yRange_-1, i))) << columnDelimiter;
				}

				out << newLineDelimiter;
			}

			output.close();
		}
	}

	return true;
}
