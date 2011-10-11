/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "util/AMTagReplacementParser.h"
#include "dataman/AMScan.h"

AMExporter::AMExporter(QObject *parent) : QObject(parent) {
	currentScan_ = 0;
	currentDataSourceIndex_ = 0;

	autoIndex_ = 0;

	keywordParser_ = new AMTagReplacementParser();

	file_ = new QFile(this);

	loadKeywordReplacementDictionary();
}

AMExporter::~AMExporter() {
	foreach(AMAbstractTagReplacementFunctor* f, keywordDictionary_)
		delete f;

	delete keywordParser_;
}

bool AMExporter::openFile(const QString &filePath)
{
	return openFile(file_, filePath);
}

bool AMExporter::openFile(QFile* file, const QString& filePath) {
	if(QFile::exists(filePath))
		return false;

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
	keywordParser_->setInitialText(inputString);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	return keywordParser_->getReplacedText();
}

void AMExporter::loadKeywordReplacementDictionary()
{
	keywordDictionary_.insert("name", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krName));
	keywordDictionary_.insert("number", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krNumber));
	keywordDictionary_.insert("notes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krNotes));
	keywordDictionary_.insert("date", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDate));
	keywordDictionary_.insert("time", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krTime));
	keywordDictionary_.insert("dateTime", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krDateTime));

	keywordDictionary_.insert("run", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRun));
	keywordDictionary_.insert("runName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunName));
	keywordDictionary_.insert("runStartDate", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunStartDate));
	keywordDictionary_.insert("runEndDate", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunEndDate));
	keywordDictionary_.insert("runNotes", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krRunNotes));
	keywordDictionary_.insert("facilityName", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krFacilityName));
	keywordDictionary_.insert("facilityDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krFacilityDescription));

	keywordDictionary_.insert("scanConfiguration", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krScanConfiguration));

	keywordDictionary_.insert("control", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControl));
	keywordDictionary_.insert("controlDescription", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlDescription));
	keywordDictionary_.insert("controlValue", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlValue));
	keywordDictionary_.insert("controlUnits", new AMTagReplacementFunctor<AMExporter>(this, &AMExporter::krControlUnits));
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
}


QString AMExporter::krName(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return currentScan_->evaluatedName();
		//return currentScan_->name();
	return "[??]";
}

QString AMExporter::krNumber(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return QString::number(currentScan_->number());
	return "[??]";
}

QString AMExporter::krNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(currentScan_)
		return currentScan_->notes();
	return "[??]";
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
	return r.name() % " " % AMDateTimeUtils::prettyDateRange(r.dateTime(), r.endDateTime());
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

QString AMExporter::krRunEndDate(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(currentScan_->database(), currentScan_->runId());

	if(arg.isEmpty())
		return r.endDateTime().toString("yyyy MM dd");
	else
		return r.endDateTime().toString(arg);
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

	QVariant v =  scanConfig->property(propertyName.toLatin1().constData());
	if(!v.isValid())
		return "[??]";

	return v.toString();
}

#include "dataman/info/AMControlInfoList.h"

QString AMExporter::krControl(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	const AMControlInfo& ci = currentScan_->scanInitialConditions()->at(index);
	return QString::number(ci.value()) % " " % ci.units();
}

QString AMExporter::krControlDescription(const QString& controlName) {
	if(!currentScan_)
		return "[??]";

	int index = currentScan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return currentScan_->scanInitialConditions()->at(index).name();	/// \todo Add description!
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

#include "dataman/AMSample.h"

QString AMExporter::krSampleElements(const QString& arg) {
	Q_UNUSED(arg)
	if(!currentScan_)
		return "[??]";

	AMSample s;
	s.loadFromDb(currentScan_->database(), currentScan_->sampleId());
	return s.elementString();
}

QString AMExporter::krSampleCreationDate(const QString& arg) {
	if(!currentScan_)
		return "[??]";

	AMSample s;
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

	AMSample s;
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
	/*
 if(ds->infoDescription().isEmpty()){
  qDebug() << "No info description, using name";
  return ds->name();
 }
 else{
  qDebug() << "Found info description";
  return ds->infoDescription();
 }
 */
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





