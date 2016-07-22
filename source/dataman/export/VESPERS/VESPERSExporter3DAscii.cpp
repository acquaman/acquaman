/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	xRange_ = -1;
	yRange_ = -1;
	wireIndex_ = -1;
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
				break;

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

		int xRange = currentScan_->scanSize(0);
		int yRange = currentScan_->scanSize(1);
		int wireRange = currentScan_->scanSize(2);
		QVector<double> fillerData = QVector<double>(yRange);
		currentScan_->dataSourceAt(0)->values(AMnDIndex(xRange-1,0,0), AMnDIndex(xRange-1, yRange-1, 0), fillerData.data());

		for (int j = 0; j < yRange && yRange_ == -1; j++)
			if (int(fillerData.at(j)) == -1)
				yRange_ = j+1;

		if (yRange_ != -1){

			fillerData = QVector<double>(xRange);
			currentScan_->dataSourceAt(0)->values(AMnDIndex(0, yRange_-1, wireRange-1), AMnDIndex(xRange-1, yRange_-1, wireRange-1), fillerData.data());

			for (int i = 0; i < xRange && xRange_ == -1; i++)
				if (int(fillerData.at(i)) == -1)
					xRange_ = i+1;
		}

		if (xRange_ != -1){

			fillerData = QVector<double>(wireRange);
			currentScan_->dataSourceAt(0)->values(AMnDIndex(xRange_-1, yRange_-1, 0), AMnDIndex(xRange_-1, yRange_-1, wireRange-1), fillerData.data());

			for (int k = 0; k < xRange && wireIndex_ == -1; k++)
				if (int(fillerData.at(k)) == -1)
					wireIndex_ = k;
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
	int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
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
					int precision = option_->exportPrecision(ds->name());

					// print x and y column?
					if(mainTableIncludeX_.at(c)) {

						ts << ds->axisValue(0, x).toString(precision);
						ts << option_->columnDelimiter();
						ts << ds->axisValue(1, y).toString(precision);
						ts << option_->columnDelimiter();
						ts << ds->axisValue(2, z).toString(precision);
						ts << option_->columnDelimiter();
					}

					if(c == indexOfCCDName)
						ts << QString(ccdString).arg(int(ds->value(AMnDIndex(x, y, z)))-1);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
					else
						ts << ds->value(AMnDIndex(x, y, z)).toString(precision);

					ts << option_->columnDelimiter();
				}

				ts << option_->newlineDelimiter();
			}
		}
	}

	if (yRange_ != -1 && xRange_ != -1){

		xRange = xRange_ == -1 ? currentScan_->scanSize(0) : (xRange_-1);

		for (int x = 0; x < xRange; x++){

			for (int z = 0; z < zRange; z++){

				// over rows within columns
				for(int c=0; c<mainTableDataSources_.count(); c++) {
					setCurrentDataSource(mainTableDataSources_.at(c));
					AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
					int precision = option_->exportPrecision(ds->name());

					// print x and y column?
					if(mainTableIncludeX_.at(c)) {

						ts << ds->axisValue(0, x).toString(precision);
						ts << option_->columnDelimiter();
						ts << ds->axisValue(1, yRange_-1).toString(precision);
						ts << option_->columnDelimiter();
						ts << ds->axisValue(2, z).toString(precision);
						ts << option_->columnDelimiter();
					}

					if(c == indexOfCCDName)
						ts << QString(ccdString).arg(int(ds->value(AMnDIndex(x, yRange_-1, z)))-1);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
					else
						ts << ds->value(AMnDIndex(x, yRange_-1, z)).toString(precision);

					ts << option_->columnDelimiter();
				}

				ts << option_->newlineDelimiter();
			}
		}
	}

	if (yRange_ != -1 && xRange_ != -1 && wireIndex_ == -1){

		for (int z = 0; z < wireIndex_; z++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {
				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
				int precision = option_->exportPrecision(ds->name());

				// print x and y column?
				if(mainTableIncludeX_.at(c)) {

					ts << ds->axisValue(0, xRange_-1).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(1, yRange_-1).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(2, z).toString(precision);
					ts << option_->columnDelimiter();
				}

				if(c == indexOfCCDName)
					ts << QString(ccdString).arg(int(ds->value(AMnDIndex(xRange_-1, yRange_-1, z)))-1);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
				else
					ts << ds->value(AMnDIndex(xRange_-1, yRange_-1, z)).toString(precision);

				ts << option_->columnDelimiter();
			}

			ts << option_->newlineDelimiter();
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
			separateFileName = removeNonPrintableCharacters(separateFileName);

			if(!openFile(&output, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			int spectraSize = source->size(3);
			QString columnDelimiter = option_->columnDelimiter();
			QString newLineDelimiter = option_->newlineDelimiter();
			QTextStream out(&output);
			int zRange = currentScan_->scanSize(2);
			int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
			int xRange = currentScan_->scanSize(0);

			for (int y = 0; y < yRange; y++){

				for (int x = 0; x < xRange; x++){

					for (int z = 0; z < zRange; z++){

						QVector<double> data(spectraSize);
						source->values(AMnDIndex(x, y, z, 0), AMnDIndex(x, y, z, spectraSize-1), data.data());

						for (int i = 0; i < spectraSize; i++)
							out << data.at(i) << columnDelimiter;

						out << newLineDelimiter;
					}
				}
			}

			if (yRange_ != -1 && xRange_ != -1){

				xRange = xRange_ == -1 ? currentScan_->scanSize(0) : (xRange_-1);

				for (int x = 0; x < xRange; x++){

					for (int z = 0; z < zRange; z++){

						QVector<double> data(spectraSize);
						source->values(AMnDIndex(x, yRange_-1, z, 0), AMnDIndex(x, yRange_-1, z, spectraSize-1), data.data());

						for (int i = 0; i < spectraSize; i++)
							out << data.at(i) << columnDelimiter;

						out << newLineDelimiter;
					}
				}

				out << newLineDelimiter;
			}

			if (yRange_ != -1 && xRange_ != -1 && wireIndex_ == -1){

				for (int z = 0; z < wireIndex_; z++){

					QVector<double> data(spectraSize);
					source->values(AMnDIndex(xRange_-1, yRange_-1, z, 0), AMnDIndex(xRange_-1, yRange_-1, z, spectraSize-1), data.data());

					for (int i = 0; i < spectraSize; i++)
						out << data.at(i) << columnDelimiter;

					out << newLineDelimiter;
				}
			}

			output.close();
		}
	}

	return true;
}
