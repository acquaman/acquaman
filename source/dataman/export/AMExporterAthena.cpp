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


#include "AMExporterAthena.h"

#include "AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QFile>
#include <QTextStream>

 AMExporterAthena::~AMExporterAthena(){}
AMExporterAthena::AMExporterAthena(QObject *parent)
	: AMExporterGeneralAscii(parent)
{
}

QString AMExporterAthena::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
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

void AMExporterAthena::writeHeader()
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
	headerText.append("-------------------------------------\n"); // Athena header delimeter from the data.

	QTextStream ts(file_);
	ts << headerText;
}

void AMExporterAthena::writeMainTable()
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
					ts << columnText << "[" << ds->axisValue(1, cc).toString(option_->exportPrecision(ds->name())) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
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

void AMExporterAthena::writeSeparateSections()
{
	// Castrated function.
}
