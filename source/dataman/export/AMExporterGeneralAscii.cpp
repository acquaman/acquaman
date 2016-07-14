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


#include "AMExporterGeneralAscii.h"
#include "AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include <QStringBuilder>
#include <QFile>
#include <QTextStream>
#include "util/AMErrorMonitor.h"

 AMExporterGeneralAscii::~AMExporterGeneralAscii(){}
AMExporterGeneralAscii::AMExporterGeneralAscii(QObject *parent) :
	AMExporter(parent)
{
}

const QMetaObject* AMExporterGeneralAscii::getMetaObject(){
	return metaObject();
}

AMExporterOption * AMExporterGeneralAscii::createDefaultOption() const {
	return new AMExporterOptionGeneralAscii();
}

bool AMExporterGeneralAscii::isValidFor(const AMScan *scan, const AMExporterOption *iOption) const
{
	// wrong
	const AMExporterOptionGeneralAscii* option = qobject_cast<const AMExporterOptionGeneralAscii*>(iOption);
	if(!option) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Invalid options specified for the General Ascii Exporter. Please report this bug to the Acquaman developers."));
		return false;
	}

	if(!option->includeAllDataSources()) {	// if includeAllDataSources is set, don't care about required specifications.
		QStringList dataSources = option->dataSources();

		// missing a required data source
		for(int i=dataSources.count()-1; i>=0; i--) {
			if(option->isDataSourceRequired(i) &&
					scan->indexOfDataSource(dataSources.at(i)) == -1) {

				AMErrorMon::report(AMErrorReport(this,
												 AMErrorReport::Alert,
												 -3,
												 "Export failed: The scan '" % scan->fullName() % "' does not contain the required data set '" % dataSources.at(i) % "'."));
				return false;
			}
		}
	}

	return true;
}

QString AMExporterGeneralAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex)
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

bool AMExporterGeneralAscii::prepareDataSources() {

	mainTableDataSources_.clear();
	mainTableIncludeX_.clear();
	separateSectionDataSources_.clear();
	separateSectionIncludeX_.clear();
	separateFileDataSources_.clear();
	separateFileIncludeX_.clear();

	if(option_->includeAllDataSources() && !option_->firstColumnOnly()) {
		// assumptions: 1D goes in main table. 0D, 2D and higher goes in separate section.
		for(int i=0; i<currentScan_->dataSourceCount(); i++) {
			switch(currentScan_->dataSourceAt(i)->rank()) {
			case 0:
				separateSectionDataSources_ << i;
				separateSectionIncludeX_ << false;	// default false for 0D (scalar point) data
				break;
			case 1:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << true;
				break;
			default:

				if (option_->includeHigherDimensionSources()){

					if (option_->separateHigherDimensionalSources()){

						separateFileDataSources_ << i;
						separateFileIncludeX_ << false;
					}
					else{

						separateSectionDataSources_ << i;
						separateSectionIncludeX_ << true;
					}
				}
				break;
			}
		}
	}

	else if (option_->includeAllDataSources() && option_->firstColumnOnly()){

		bool includeFirstColumn = true;

		// assumptions: 1D goes in main table. 0D, 2D and higher goes in separate section.
		for(int i=0; i<currentScan_->dataSourceCount(); i++) {

			switch(currentScan_->dataSourceAt(i)->rank()) {

			case 0:
				separateSectionDataSources_ << i;
				separateSectionIncludeX_ << false;	// default false for 0D (scalar point) data
				break;

			case 1:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << includeFirstColumn;

				if (includeFirstColumn)
					includeFirstColumn = false;

				break;

			default:

				if (option_->includeHigherDimensionSources()){

					if (option_->separateHigherDimensionalSources()){

						separateFileDataSources_ << i;
						separateFileIncludeX_ << false;
					}
					else{

						separateSectionDataSources_ << i;
						separateSectionIncludeX_ << true;
					}
				}

				break;
			}
		}
	}

	// otherwise look at what the user specified
	else {
		QStringList dataSources = option_->dataSources();
		for(int i=0; i<dataSources.count(); i++) {	// i iterates through data sources in the option: these are ones that the user requested to include

			int dataSourceIndex = currentScan_->indexOfDataSource(dataSources.at(i));

			if(dataSourceIndex == -1) {	// missing data source?
				if(option_->isDataSourceRequired(i)) {
					AMErrorMon::report(AMErrorReport(this,
													 AMErrorReport::Alert,
													 -3,
													 "Export failed: The scan '" % currentScan_->fullName() % "' does not contain the required data set '" % dataSources.at(i) % "'."));
					return false;
				}
				else
					continue;
			}

			AMDataSource* dataSource = currentScan_->dataSourceAt(dataSourceIndex);

			switch(option_->dataSourceOrganizeMode(i)) {
			case  AMExporterOptionGeneralAscii::CombineInColumnsMode:
				if(dataSource->rank() > 0 && dataSource->rank() < 3) {
					mainTableDataSources_ << dataSourceIndex;
					mainTableIncludeX_ << !option_->isDataSourceAxisValueColumnOmitted(i);
				}
				else {
					separateSectionDataSources_ << dataSourceIndex;	// only 1D and 2D data allowed in the main table.  I don't know how else to break it to you...
					separateSectionIncludeX_ << !option_->isDataSourceAxisValueColumnOmitted(i);
					AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Moving " % QString::number(dataSource->rank()) % "D data to its own section in the exported file.  Only 1D and 2D data is allowed in the main table."));
				}
				break;

			case AMExporterOptionGeneral::SeparateSectionsMode:
				separateSectionDataSources_ << dataSourceIndex;
				separateSectionIncludeX_ << !option_->isDataSourceAxisValueColumnOmitted(i);
				break;
			case AMExporterOptionGeneral::SeparateFilesMode:
				separateFileDataSources_ << dataSourceIndex;
				separateFileIncludeX_ << !option_->isDataSourceAxisValueColumnOmitted(i);
			}
		}
	}

	return true;
}

void AMExporterGeneralAscii::writeHeader()
{
	if(!option_->headerIncluded())
		return;

	QString headerText = parseKeywordString(option_->headerText());

	normalizeLineEndings(headerText);
	if(headerText.right(1) != "\n")
		headerText.append("\n");	// ensure ends in newline
	convertNormalizedLineEndingsTo(option_->newlineDelimiter(), headerText);

	QTextStream ts(file_);
	ts << headerText;
}

void AMExporterGeneralAscii::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	int maxTableRows = 0;
	if(option_->columnHeaderIncluded()) {
		for(int c=0; c<mainTableDataSources_.count(); c++) {
			setCurrentDataSource(mainTableDataSources_.at(c));
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			if(ds->size(0) > maxTableRows)
				maxTableRows = ds->size(0); // convenient... lets figure this out while we're looping through anyway

			// 1D data sources:
			if(ds->rank() == 1) {
				if(mainTableIncludeX_.at(c))
					ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
				ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
			}
			else {	// 2D
				if(mainTableIncludeX_.at(c))
					ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
				// need a loop over the second axis columns
				for(int cc=0; cc<ds->size(1); cc++) {
					setCurrentColumnIndex(cc);
					ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
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

void AMExporterGeneralAscii::writeSeparateSections()
{
	QTextStream ts(file_);

	if (option_->higherDimensionsInRows()){

		for(int s=0; s<separateSectionDataSources_.count(); s++) {

			ts << option_->newlineDelimiter();

			setCurrentDataSource(separateSectionDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			// section header?
			if(option_->sectionHeaderIncluded()) {
				ts << parseKeywordString(option_->sectionHeader());
				ts << option_->newlineDelimiter();
			}

			// column header?
			if(option_->columnHeaderIncluded()) {
				// 1D data sources:
				if(ds->rank() == 0) {
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
				}
				else if(ds->rank() == 1) {
					if(separateSectionIncludeX_.at(s))
						ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
				}
				else if(ds->rank() == 2) {	// 2D
					if(separateSectionIncludeX_.at(s))
						ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
					// need a loop over the second axis columns
					for(int cc=0; cc<ds->size(1); cc++) {
						setCurrentColumnIndex(cc);
						ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
					}
				}
				ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();
			}

			// table
			switch(ds->rank()) {
			case 0:
				ts << ds->value(AMnDIndex()).toString(precision) << option_->columnDelimiter() << option_->newlineDelimiter();
				break;
			case 1: {
				int maxTableRows = ds->size(0);
				for(int r=0; r<maxTableRows; r++) {
					if(separateSectionIncludeX_.at(s)) {
						ts << ds->axisValue(0,r).toString(precision) << option_->columnDelimiter();
					}
					ts << ds->value(r).toString(option_->exportPrecision(ds->name())) << option_->columnDelimiter() << option_->newlineDelimiter();
				}
			}
				break;
			case 2: {
				int maxTableRows = ds->size(0);
				for(int r=0; r<maxTableRows; r++) {
					if(separateSectionIncludeX_.at(s))
						ts << ds->axisValue(0,r).toString(precision) << option_->columnDelimiter();
					// need a loop over the second axis columns
					for(int cc=0; cc<ds->size(1); cc++) {
						ts << ds->value(AMnDIndex(r,cc)).toString(precision) << option_->columnDelimiter();
					}
					ts << option_->newlineDelimiter();
				}
			}
				break;
			default:
				/// \todo Implement 3D
				break;
			}
		}
	}

	// For writing out in columns.  Essentially transposing the file.
	else{

		for(int s=0; s<separateSectionDataSources_.count(); s++) {

			setCurrentDataSource(separateSectionDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			// section header?
			if(option_->sectionHeaderIncluded()) {
				ts << parseKeywordString(option_->sectionHeader());
				ts << option_->newlineDelimiter();
			}

			// If including the X values.
			if (separateSectionIncludeX_.at(s)){

				switch(ds->rank()){

				case 0:
					break;

				case 1:
				case 2:{

					ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();

					int maxTableColumns = ds->size(0);

					for(int column = 0; column < maxTableColumns; column++)
						ts << ds->axisValue(0,column).toString(precision) << option_->columnDelimiter();

					ts << option_->newlineDelimiter();

					break;
				}

				default:
					/// \todo 3D.
					break;
				}
			}

			// Main table with column headers prefacing rows.
			switch (ds->rank()){

			case 0:{

				if(option_->columnHeaderIncluded())
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();

				ts << ds->value(AMnDIndex()).toString(precision) << option_->columnDelimiter() << option_->newlineDelimiter();

				break;
			}

			case 1:{

				if (option_->columnHeaderIncluded())
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();

				int maxTableColumns = ds->size(0);

				for(int column = 0; column < maxTableColumns; column++)
					ts << ds->value(column).toString(precision) << option_->columnDelimiter();

				ts << option_->newlineDelimiter();

				break;
			}

			case 2:{

				for (int cc = 0; cc < ds->size(1); cc++){

					if (option_->columnHeaderIncluded())
						ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();

					int maxTableColumns = ds->size(0);

					for (int column = 0; column < maxTableColumns; column++)
						ts << ds->value(AMnDIndex(column, cc)).toString(precision) << option_->columnDelimiter();

					ts << option_->newlineDelimiter();
				}

				break;
			}

			default:
				/// \todo 3D
				break;
			}
		}
	}
}

bool AMExporterGeneralAscii::writeSeparateFiles(const QString& destinationFolderPath)
{
	if (option_->higherDimensionsInRows()){

		for(int s=0; s<separateFileDataSources_.count(); s++) {

			setCurrentDataSource(separateFileDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			QFile file;
			QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName() );
			separateFileName = removeNonPrintableCharacters(separateFileName);

			if(!openFile(&file, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			QTextStream ts(&file);

			// section header?
			if(option_->sectionHeaderIncluded()) {
				ts << parseKeywordString(option_->sectionHeader());
				ts << option_->newlineDelimiter();
			}

			// column header?
			if(option_->columnHeaderIncluded()) {
				// 1D data sources:
				if(ds->rank() == 0) {
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
				}
				else if(ds->rank() == 1) {
					if(separateFileIncludeX_.at(s))
						ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
				}
				else if(ds->rank() == 2) {	// 2D
					if(separateFileIncludeX_.at(s))
						ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();
					// need a loop over the second axis columns
					for(int cc=0; cc<ds->size(1); cc++) {
						setCurrentColumnIndex(cc);
						ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
					}
				}
				ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();
			}

			// table
			switch(ds->rank()) {
			case 0:
				ts << ds->value(AMnDIndex()).toString(precision) << option_->columnDelimiter() << option_->newlineDelimiter();
				break;
			case 1: {
				int maxTableRows = ds->size(0);
				for(int r=0; r<maxTableRows; r++) {
					if(separateFileIncludeX_.at(s)) {
						ts << ds->axisValue(0,r).toString(precision) << option_->columnDelimiter();
					}
					ts << ds->value(r).toString(precision) << option_->columnDelimiter() << option_->newlineDelimiter();
				}
			}
				break;
			case 2: {
				int maxTableRows = ds->size(0);
				for(int r=0; r<maxTableRows; r++) {
					if(separateFileIncludeX_.at(s))
						ts << ds->axisValue(0,r).toString(precision) << option_->columnDelimiter();
					// need a loop over the second axis columns
					for(int cc=0; cc<ds->size(1); cc++) {
						ts << ds->value(AMnDIndex(r,cc)).toString(precision) << option_->columnDelimiter();
					}
					ts << option_->newlineDelimiter();
				}
			}
				break;
			default:
				/// \todo Implement 3D
				break;
			}
		}
	}

	// For writing out in columns.  Essentially transposing the file.
	else{

		for(int s=0; s<separateFileDataSources_.count(); s++) {

			setCurrentDataSource(separateFileDataSources_.at(s));	// sets currentDataSourceIndex_
			AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
			int precision = option_->exportPrecision(ds->name());

			QFile file;
			QString separateFileName = parseKeywordString( destinationFolderPath % "/" % option_->separateSectionFileName() );
			separateFileName = removeNonPrintableCharacters(separateFileName);

			if(!openFile(&file, separateFileName)) {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -4, "Export failed (partially): You selected to create separate files for certain data sets. Could not open the file '" % separateFileName % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
				return false;
			}

			QTextStream ts(&file);

			// section header?
			if(option_->sectionHeaderIncluded()) {
				ts << parseKeywordString(option_->sectionHeader());
				ts << option_->newlineDelimiter();
			}

			// If including the X values.
			if (separateFileIncludeX_.at(s)){

				switch(ds->rank()){

				case 0:
					break;

				case 1:
				case 2:{

					ts << parseKeywordString(option_->columnHeader()) << ".X" << option_->columnDelimiter();

					int maxTableColumns = ds->size(0);

					for(int column = 0; column < maxTableColumns; column++)
						ts << ds->axisValue(0,column).toString(precision) << option_->columnDelimiter();

					ts << option_->newlineDelimiter();

					break;
				}

				default:
					/// \todo 3D.
					break;
				}
			}

			// Main table with column headers prefacing rows.
			switch (ds->rank()){

			case 0:{

				if(option_->columnHeaderIncluded())
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();

				ts << ds->value(AMnDIndex()).toString(precision) << option_->columnDelimiter() << option_->newlineDelimiter();

				break;
			}

			case 1:{

				if (option_->columnHeaderIncluded())
					ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();

				int maxTableColumns = ds->size(0);

				for(int column = 0; column < maxTableColumns; column++)
					ts << ds->value(column).toString(precision) << option_->columnDelimiter();

				ts << option_->newlineDelimiter();

				break;
			}

			case 2:{

				for (int cc = 0; cc < ds->size(1); cc++){

					if (option_->columnHeaderIncluded())
						ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString(precision) << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();

					int maxTableColumns = ds->size(0);

					for (int column = 0; column < maxTableColumns; column++)
						ts << ds->value(AMnDIndex(column, cc)).toString(precision) << option_->columnDelimiter();

					ts << option_->newlineDelimiter();
				}

				break;
			}

			default:
				/// \todo 3D
				break;
			}
		}
	}

	return true;
}

void AMExporterGeneralAscii::normalizeLineEndings(QString &inputString)
{
	inputString.replace("\r\n", "\n");
}

void AMExporterGeneralAscii::convertNormalizedLineEndingsTo(const QString &eol, QString &inputString)
{
	inputString.replace(QChar('\n'), eol);
}
