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


#include "VESPERSExporterLineScanAscii.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

 VESPERSExporterLineScanAscii::~VESPERSExporterLineScanAscii(){}
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
	mainFileName_ = removeNonPrintableCharacters(mainFileName_);

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
	VESPERSSpatialLineScanConfiguration *lineConfig = qobject_cast<VESPERSSpatialLineScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	VESPERSEnergyScanConfiguration *energyConfig = qobject_cast<VESPERSEnergyScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	VESPERSTimeScanConfiguration *timeConfig = qobject_cast<VESPERSTimeScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));

	QString ccdString = QString();
	int shiftOffset = 0;
	// This will return -1 if it fails.  This means any checks inside this loop will always fail if the CCD was not included.
	int indexOfCCDName = -1;

	if (lineConfig){

		ccdFileName = lineConfig->ccdFileName();

		if (lineConfig->ccdDetector() == VESPERS::Roper){

			ccdString = ccdFileName % "_%1.spe";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("RoperFileNumber");
		}

		else if (lineConfig->ccdDetector() == VESPERS::Mar){

			ccdString = ccdFileName % "_%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("MarFileNumber");
		}

		else if (lineConfig->ccdDetector() == VESPERS::Pilatus){

			ccdString = ccdFileName % "-%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("PilatusFileNumber");
		}
	}

	else if (energyConfig){

		ccdFileName = energyConfig->ccdFileName();

		if (energyConfig->ccdDetector() == VESPERS::Roper){

			ccdString = ccdFileName % "_%1.spe";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("RoperFileNumber");

		}

		else if (energyConfig->ccdDetector() == VESPERS::Mar){

			ccdString = ccdFileName % "_%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("MarFileNumber");
		}

		else if (energyConfig->ccdDetector() == VESPERS::Pilatus){

			ccdString = ccdFileName % "-%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("PilatusFileNumber");
		}
	}

	else if (timeConfig){

		ccdFileName = timeConfig->ccdFileName();

		if (timeConfig->ccdDetector() == VESPERS::Roper){

			ccdString = ccdFileName % "_%1.spe";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("RoperFileNumber");

		}

		else if (timeConfig->ccdDetector() == VESPERS::Mar){

			ccdString = ccdFileName % "_%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("MarFileNumber");
		}

		else if (timeConfig->ccdDetector() == VESPERS::Pilatus){

			ccdString = ccdFileName % "-%1.tif";
			shiftOffset = -1;	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.
			indexOfCCDName = currentScan_->indexOfDataSource("PilatusFileNumber");
		}
	}

	else
		return;

	// Extra step for legacy scans.
	if (!ccdString.isNull() && indexOfCCDName == -1)
		indexOfCCDName = currentScan_->indexOfDataSource("CCDFileNumber");

	int xRange = currentScan_->scanSize(0);

	for (int x = 0; x < xRange; x++){

		// over rows within columns
		for(int c=0; c<mainTableDataSources_.count(); c++) {
			setCurrentDataSource(mainTableDataSources_.at(c));
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			// print x column?
			if(mainTableIncludeX_.at(c)) {

				ts << ds->axisValue(0,x).toString(precision);
				ts << option_->columnDelimiter();
			}

			if(c == indexOfCCDName)
				ts << QString(ccdString).arg(int(ds->value(AMnDIndex(x))) + shiftOffset);
			else
				ts << ds->value(AMnDIndex(x)).toString(precision);

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
