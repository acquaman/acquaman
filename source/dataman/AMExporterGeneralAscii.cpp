#include "AMExporterGeneralAscii.h"
#include "AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include <QStringBuilder>
#include <QFile>
#include <QTextStream>
#include "util/AMErrorMonitor.h"

AMExporterGeneralAscii::AMExporterGeneralAscii(QObject *parent) :
	AMExporter(parent)
{
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

QString AMExporterGeneralAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option)
{
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
	qDebug() << "Input file name:" << destinationFolderPath % "/" % option->fileName();
	mainFileName_ = parseKeywordString( destinationFolderPath % "/" % option->fileName() );
	qDebug() << "Export file name:" << mainFileName_;

	if(!openFile(mainFileName_)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -3, "Export failed: Could not open the file '" % mainFileName_ % "' for writing.  Check that you have permission to save files there, and that a file with that name doesn't already exists."));
		return QString();
	}


	writeHeader();
	writeMainTable();
	writeSeparateSections();
	writeSeparateFiles();

	file_->close();
	return mainFileName_;
}

bool AMExporterGeneralAscii::prepareDataSources() {

	mainTableDataSources_.clear();
	mainTableIncludeX_.clear();
	separateSectionDataSources_.clear();
	separateFileDataSources_.clear();

	if(option_->includeAllDataSources()) {
		// assumptions: 1D goes in main table. 0D, 2D and higher goes in separate section.
		for(int i=0; i<currentScan_->dataSourceCount(); i++) {
			switch(currentScan_->dataSourceAt(i)->rank()) {
			case 0:
				separateSectionDataSources_ << i;
				break;
			case 1:
				mainTableDataSources_ << i;
				mainTableIncludeX_ << true;
				break;
			default:
				separateSectionDataSources_ << i;
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
					AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Moving " % QString::number(dataSource->rank()) % "D data to its own section in the exported file.  Only 1D and 2D data is allowed in the main table."));
				}
				break;

			case AMExporterOptionGeneral::SeparateSectionsMode:
				separateSectionDataSources_ << dataSourceIndex;
				break;
			case AMExporterOptionGeneral::SeparateFilesMode:
				separateFileDataSources_ << dataSourceIndex;
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
					ts << parseKeywordString(option_->columnHeader()) << "[" << ds->axisValue(1, cc).toString() << ds->axisInfoAt(1).units << "]" << option_->columnDelimiter();
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

			bool doPrint = (ds->size(0) > r);

			// print x column?
			if(mainTableIncludeX_.at(c)) {
				if(doPrint)
					ts << ds->axisValue(0,r).toString();
				ts << option_->columnDelimiter();
			}

			// 1D data sources:
			if(ds->rank() == 1) {
				if(doPrint)
					ts << ds->value(r).toString();
				ts << option_->columnDelimiter();
			}
			else {
				// need a loop over the second axis columns
				for(int cc=0; cc<ds->size(1); cc++) {
					if(doPrint)
						ts << ds->value(AMnDIndex(r,cc)).toString();
					ts << option_->columnDelimiter();
				}
			}
		}
		ts << option_->newlineDelimiter();
	}
}

void AMExporterGeneralAscii::writeSeparateSections()
{
}

void AMExporterGeneralAscii::writeSeparateFiles()
{
}

void AMExporterGeneralAscii::normalizeLineEndings(QString &inputString)
{
	inputString.replace("\r\n", "\n");
}

void AMExporterGeneralAscii::convertNormalizedLineEndingsTo(const QString &eol, QString &inputString)
{
	inputString.replace(QChar('\n'), eol);
}
