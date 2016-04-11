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


#include "AMExporter.h"
#include <QFile>
#include <QDir>
#include <QStringBuilder>
#include <QMessageBox>

#include "util/AMTagReplacementParser.h"
#include "dataman/AMScan.h"

AMExporter::AMExporter(QObject *parent) : QObject(parent) {
	currentScan_ = 0;
	currentDataSourceIndex_ = 0;

	autoIndex_ = 0;
	overwriteAll_ = Default;

	keywordParser_ = new AMTagReplacementParser();

	file_ = new QFile(this);

	loadKeywordReplacementDictionary();
}

AMExporter::~AMExporter() {
	foreach(AMAbstractTagReplacementFunctor* f, keywordDictionary_)
		delete f;

	delete keywordParser_;
}

const QMetaObject* AMExporter::getMetaObject(){
	return metaObject();
}

QString AMExporter::currentRunExportFilePath()
{
	if (currentScan_)
		return krRunName();
	else
		return "";
}

bool AMExporter::openFile(const QString &filePath)
{
	return openFile(file_, filePath);
}

bool AMExporter::openFile(QFile* file, const QString& filePath) {

	bool fileExists = QFile::exists(filePath);

	if (fileExists && overwriteAll_ == None)
		return false;

	else if (fileExists && overwriteAll_ == Default){

		QMessageBox::StandardButton button = QMessageBox::question(0, "Overwrite file?",
									   QString("%1 already exists.  Do you want to overwrite this file?").arg(filePath.split("/").last()),
									   QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
									   QMessageBox::Yes);


		if (button == QMessageBox::YesToAll)
			overwriteAll_ = All;
		else if (button == QMessageBox::NoToAll)
			overwriteAll_ = None;

		if (button == QMessageBox::No || button == QMessageBox::NoToAll)
			return false;
	}

	QFileInfo fileInfo(filePath);
	if(!QDir::current().mkpath(fileInfo.path()))
		return false;

	if(file->isOpen())
		file->close();
	file->setFileName(filePath);

	if(!file->open(QFile::WriteOnly))
		return false;

	return true;
}

QString AMExporter::parseKeywordString(const QString &inputString) {
	currentlyParsing_ = inputString;
	keywordParser_->setInitialText(inputString);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	return keywordParser_->getReplacedText();
}

void AMExporter::loadKeywordReplacementDictionary()
{
	keywordDictionary_.insert("name", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krName));
	keywordDictionary_.insert("number", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krNumber));
	keywordDictionary_.insert("notes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krNotes));
	keywordDictionary_.insert("delimitedNotes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDelimitedNotes));
	keywordDictionary_.insert("simplifiedNotes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSimplifiedNotes));
	keywordDictionary_.insert("date", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDate));
	keywordDictionary_.insert("time", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krTime));
	keywordDictionary_.insert("dateTime", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDateTime));
	keywordDictionary_.insert("serial", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSerial));


	keywordDictionary_.insert("run", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRun));
	keywordDictionary_.insert("runName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunName));
	keywordDictionary_.insert("runStartDate", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunStartDate));
	keywordDictionary_.insert("runNotes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunNotes));
	keywordDictionary_.insert("facilityName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krFacilityName));
	keywordDictionary_.insert("facilityDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krFacilityDescription));

	keywordDictionary_.insert("scanConfiguration", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krScanConfiguration));

	keywordDictionary_.insert("control", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControl));
	keywordDictionary_.insert("controlName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlName));
	keywordDictionary_.insert("controlDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlDescription));
	keywordDictionary_.insert("controlValue", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlValue));
	keywordDictionary_.insert("controlUnits", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlUnits));
	keywordDictionary_.insert("controlSelection", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlEnum));
	keywordDictionary_.insert("allControls", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krAllControls));

	keywordDictionary_.insert("sample", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSample));
	keywordDictionary_.insert("sampleName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSampleName));
	keywordDictionary_.insert("sampleElements", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSampleElements));
	keywordDictionary_.insert("sampleCreationDate", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSampleCreationDate));
	keywordDictionary_.insert("sampleNotes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krSampleNotes));

	keywordDictionary_.insert("dataSet", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSource));
	keywordDictionary_.insert("dataSetName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceName));
	keywordDictionary_.insert("dataSetDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceDescription));
	keywordDictionary_.insert("dataSetUnits", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceUnits));
	keywordDictionary_.insert("dataSetSize", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceSize));
	keywordDictionary_.insert("dataSetInfoDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceInfoDescription));
	keywordDictionary_.insert("dataSetAxisValue", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceAxisValue));
	keywordDictionary_.insert("dataSetAxisUnits", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDataSourceAxisUnits));

	keywordDictionary_.insert("exportIndex", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krExporterAutoIncrement));

	keywordDictionary_.insert("fsIndex", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krFileSystemAutoIncrement));
}


QString AMExporter::krName(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return currentScan_->name();
	return "[??]";
}

QString AMExporter::krNumber(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return QString::number(currentScan_->number());
	return "[??]";
}

QString AMExporter::krSerial(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return QString::number(currentScan_->id());
	return "[??]";
}

QString AMExporter::krNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return currentScan_->notes();
	return "[??]";
}

QString AMExporter::krDelimitedNotes(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	return currentScan_->notes().replace("\n",QString("\n%1").arg(arg)).prepend(arg);

}

QString AMExporter::krSimplifiedNotes(const QString& arg) {
	Q_UNUSED(arg)

	if(!currentScan_)
		return "[??]";

	return currentScan_->notes().simplified();

}

QString AMExporter::krDate(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	if(arg.isEmpty())
		return currentScan_->dateTime().toString("yyyy MM dd");
	else
		return currentScan_->dateTime().toString(arg);
}

QString AMExporter::krTime(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	if(arg.isEmpty())
		return currentScan_->dateTime().toString("hh:mm:ss");
	else
		return currentScan_->dateTime().toString(arg);
}

QString AMExporter::krDateTime(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	if(arg.isEmpty())
		return currentScan_->dateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return currentScan_->dateTime().toString(arg);
}

#include "util/AMDateTimeUtils.h"
#include "dataman/AMRun.h"

QString AMExporter::krRun(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());
	return r.name() % " " % AMDateTimeUtils::prettyDate(r.dateTime());
}


QString AMExporter::krRunName(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());
	return r.name();
}

QString AMExporter::krRunStartDate(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());

	if(arg.isEmpty())
		return r.dateTime().toString("yyyy MM dd");
	else
		return r.dateTime().toString(arg);
}



QString AMExporter::krRunNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());
	return r.notes();
}


QString AMExporter::krFacilityName(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());

	AMFacility f;
	f.loadFromDb(currentScan_->database(), r.facilityId());
	return f.name();
}

QString AMExporter::krFacilityDescription(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());

	AMFacility f;
	f.loadFromDb(currentScan_->database(), r.facilityId());
	return f.description();
}

#include "acquaman/AMScanConfiguration.h"	/// \todo Move to dataman!
QString AMExporter::krScanConfiguration(const QString& propertyName) {
	if(!currentScan_)
		return "[??]";

	const AMScanConfiguration* scanConfig = currentScan_->scanConfiguration();
	if(!scanConfig)
		return "[??]";

	QStringList propertyArgs = propertyName.split('%');
	QVariant v =  scanConfig->property(propertyArgs.at(0).toLatin1().constData());
	if(propertyArgs.count() > 1){
		if(propertyArgs.at(1) == "double" && v.canConvert<double>()){
			double value = v.toDouble();
			QString retVal;
			int precision = 0;
			bool conversionOk;
			if(propertyArgs.count() > 2){
				precision = propertyArgs.at(2).toInt(&conversionOk);
				if(!conversionOk)
					precision = 0;
			}
			retVal.setNum(value, 'f', precision);
			return retVal;
		}
		else if(propertyArgs.at(1) == "enumConvert" && v.canConvert<int>()){
			if(scanConfig->canEnumConvert(propertyArgs.at(0)))
				return scanConfig->enumConvert(propertyArgs.at(0), v.toInt());
		}

	}
	if(!v.isValid())
		return "[??]";

	return v.toString();
}

#include "dataman/info/AMControlInfo.h"

QString AMExporter::krControl(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	const AMControlInfo& ci = currentScan_->scanInitialConditions()->at(index);
	return QString::number(ci.value()) % " " % ci.units();
}


QString AMExporter::krControlName(const QString &controlName)
{
	if (!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return currentScan_->scanInitialConditions()->at(index).name();
}

QString AMExporter::krControlDescription(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return currentScan_->scanInitialConditions()->at(index).description();
}

QString AMExporter::krControlValue(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return QString::number(currentScan_->scanInitialConditions()->at(index).value());
}

QString AMExporter::krControlUnits(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return currentScan_->scanInitialConditions()->at(index).units();
}

QString AMExporter::krControlEnum(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	const AMControlInfo& ci = currentScan_->scanInitialConditions()->at(index);

	const AMScanConfiguration* scanConfig = currentScan_->scanConfiguration();
	if(!scanConfig)
		return QString::number(ci.value()) % " " % ci.units();

	if(!ci.enumString().isEmpty())
		return ci.enumString();
	else if(scanConfig->canEnumConvert(controlName))
		return scanConfig->enumConvert(controlName, ci.value());
	else
		return QString::number(ci.value()) % " " % ci.units();
}

QString AMExporter::krAllControls(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	QStringList rv;
	for(int i=0; i<currentScan_->scanInitialConditions()->count(); i++) {
		const AMControlInfo& ci = currentScan_->scanInitialConditions()->at(i);
		rv << ci.name() % ": " % QString::number(ci.value()) % " " % ci.units();
	}

	return rv.join("\r\n");
}

QString AMExporter::krSample(const QString& arg) {
	Q_UNUSED(arg)
	return krSampleName();
}

QString AMExporter::krSampleName(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	return currentScan_->sampleName();
}

#include "dataman/AMSamplePre2013.h"

QString AMExporter::krSampleElements(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(currentScan_->database(), currentScan_->sampleId());
	return s.elementString();
}

QString AMExporter::krSampleCreationDate(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(currentScan_->database(), currentScan_->sampleId());

	if(arg.isEmpty())
		return s.dateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return s.dateTime().toString(arg);
}

QString AMExporter::krSampleNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(currentScan_->database(), currentScan_->sampleId());
	return s.notes();
}

QString AMExporter::krDataSource(const QString& dataSourceName) {
	return krDataSourceDescription(dataSourceName);
}

QString AMExporter::krDataSourceName(const QString &dataSourceName)
{
	if(!currentScan_)
		return "[??]";

	int dataSourceIndex;
	if(dataSourceName.isEmpty()) {	// use current index
		dataSourceIndex = currentDataSourceIndex_;
	}
	else
		dataSourceIndex = currentScan_->indexOfDataSource(dataSourceName);

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	return currentScan_->dataSourceAt(dataSourceIndex)->name();
}

QString AMExporter::krDataSourceDescription(const QString& dataSourceName) {
	if(!currentScan_)
		return "[??]";

	int dataSourceIndex;
	if(dataSourceName.isEmpty()) {	// use current index
		dataSourceIndex = currentDataSourceIndex_;
	}
	else
		dataSourceIndex = currentScan_->indexOfDataSource(dataSourceName);

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	AMDataSource* ds = currentScan_->dataSourceAt(dataSourceIndex);
	if(ds->description().isEmpty())
		return ds->name();
	else
		return ds->description();
}

QString AMExporter::krDataSourceUnits(const QString& dataSourceName) {
	if(!currentScan_)
		return "[??]";

	int dataSourceIndex;
	if(dataSourceName.isEmpty()) {	// use current index
		dataSourceIndex = currentDataSourceIndex_;
	}
	else
		dataSourceIndex = currentScan_->indexOfDataSource(dataSourceName);

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	return "[Data Source Units un-implemented]";
}

QString AMExporter::krDataSourceSize(const QString& dataSourceName) {
	if(!currentScan_)
		return "[??]";

	int dataSourceIndex;
	if(dataSourceName.isEmpty()) {	// use current index
		dataSourceIndex = currentDataSourceIndex_;
	}
	else
		dataSourceIndex = currentScan_->indexOfDataSource(dataSourceName);

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	return currentScan_->dataSourceAt(dataSourceIndex)->size().toString();

}

QString AMExporter::krDataSourceInfoDescription(const QString &dataSourceName) {
	if(!currentScan_)
		return "[??]";

	int dataSourceIndex;
	if(dataSourceName.isEmpty()) {	// use current index
		dataSourceIndex = currentDataSourceIndex_;
	}
	else
		dataSourceIndex = currentScan_->indexOfDataSource(dataSourceName);

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	AMDataSource* ds = currentScan_->dataSourceAt(dataSourceIndex);
	return ds->infoDescription();
}

QString AMExporter::krDataSourceAxisValue(const QString& dataSourceName) {
	if(!currentScan_)
		return "[??]";

	if(!dataSourceName.isEmpty())
		return "[??]";	// can't specify a data source here... Only the current is allowed

	int dataSourceIndex = currentDataSourceIndex_;

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	AMDataSource* ds = currentScan_->dataSourceAt(dataSourceIndex);
	if(ds->rank() != 2 || currentColumnIndex_ < 0 || currentColumnIndex_ >= ds->size(1))
		return "[??]";

	return ds->axisValue(1, currentColumnIndex_).toString();
}

QString AMExporter::krDataSourceAxisUnits(const QString& dataSourceName) {
	if(!currentScan_)
		return "[??]";

	if(!dataSourceName.isEmpty())
		return "[??]";	// can't specify a data source here... Only the current is allowed

	int dataSourceIndex = currentDataSourceIndex_;

	if(dataSourceIndex < 0 || dataSourceIndex >= currentScan_->dataSourceCount())
		return "[??]";

	AMDataSource* ds = currentScan_->dataSourceAt(dataSourceIndex);
	if(ds->rank() != 2)
		return "[??]";

	return ds->axisInfoAt(1).units;
}

QString AMExporter::krExporterAutoIncrement(const QString &arg){
	Q_UNUSED(arg)
	return QString("%1").arg(autoIndex_);
}

#include <QDir>
#include <QRegExp>
QString AMExporter::krFileSystemAutoIncrement(const QString &arg)
{
	Q_UNUSED(arg)

	if (!currentScan_)
		return "[??]";

	if (!currentScan_->scanConfiguration())
		return "[??]";

	if (currentScan_->indexType() != "fileSystem")
		return "[??]";

	// The default number for a scan is 0.  If it is still 0 then the scan has not yet been indexed.  If this value is non-zero (number > 0) then the scan has already
	// been assigned which likely means that the current file is an associated separate file (ie: data file with a spectra file accompanying it).
	// The other benefit for doing it this way is that now the exported file will have the same number as the name on the scan editor.
	if (currentScan_->number() == 0){

		// This will not be necessary once the general AMScanDictionary is implemented.
		AMScan *scan = const_cast<AMScan *>(currentScan_);

		QString newName = currentlyParsing_;
		newName = newName.mid(newName.lastIndexOf("/")+1);

		for (int i = 0; i < keywordParser_->replacementList().size(); i++)
			if (keywordParser_->replacementList().at(i).tag != "fsIndex")
				newName.replace(keywordParser_->replacementList().at(i).tag, keywordParser_->replacementList().at(i).replacement);

		newName.replace("$fsIndex", "*");
		newName.replace("$", "");

		QString finalTest = newName;

		QDir dir(destinationFolderPath_);
		dir.setNameFilters(QStringList() << newName);
		newName.replace("*", "[\\d{0,4}]\\");
		QStringList filtered = dir.entryList().filter(QRegExp(newName));

		int incr = 0;
		dir.setNameFilters(QStringList() << QString(finalTest).replace("*", QString::number(filtered.size())));

		while (!dir.entryList().isEmpty())
			dir.setNameFilters(QStringList() << QString(finalTest).replace("*", QString::number(filtered.size()+(++incr))));

		// I want the auto-index to start from 1.
		if (filtered.size()+incr == 0)
			scan->setNumber(1);
		else
			scan->setNumber(filtered.size()+incr);

		if(scan->database())
			scan->storeToDb(scan->database());
		else
			scan->storeToDb(AMDatabase::database("user"));
	}

	return QString::number(currentScan_->number());
}


QString AMExporter::removeNonPrintableCharacters(const QString &fileName) const{
	QString retVal = fileName;
	bool detectedNonprintable = false;
	for(int x = 0, size = fileName.size(); (x < size) && !detectedNonprintable; x++)
		if(!fileName.at(x).isPrint())
			detectedNonprintable = true;

	if(detectedNonprintable){
		retVal.clear();
		for(int x = 0, size = fileName.size(); x < size; x++)
			if(fileName.at(x).isPrint())
				retVal.append(fileName.at(x));
	}

	return retVal;
}
