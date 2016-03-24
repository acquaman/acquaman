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


#include "AMScanDictionary.h"
#include <QStringBuilder>
#include "dataman/AMScan.h"
#include "util/AMTagReplacementParser.h"

#include "acquaman/AMScanConfiguration.h"	/// \todo Move to dataman!
#include "util/AMDateTimeUtils.h"
#include "dataman/AMRun.h"
#include "dataman/info/AMControlInfo.h"
#include "dataman/AMSamplePre2013.h"

 AMScanDictionary::~AMScanDictionary(){}
AMScanDictionary::AMScanDictionary(AMScan *scan, QObject *parent) :
	AMOldScanParametersDictionary(parent)
{
	scan_ = scan;
	if(scan_){
		connect(scan_, SIGNAL(nameChanged(QString)), this, SLOT(reoperate()));
		connect(scan_, SIGNAL(numberChanged(int)), this, SLOT(reoperate()));
		connect(scan_, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(reoperate()));
		connect(scan_, SIGNAL(sampleIdChanged(int)), this, SLOT(reoperate()));
		connect(scan_, SIGNAL(scanConfigurationChanged()), this, SLOT(reoperate()));
		connect(scan_, SIGNAL(loadedFromDb()), this, SLOT(reoperate()));
	}
	loadKeywordReplacementDictionary();
}

bool AMScanDictionary::canOperateOnName() const{
	return true;
}

bool AMScanDictionary::canOperateOnExportName() const{
	return true;
}

bool AMScanDictionary::canOperate() const{
	return !scan_->isReloading();
}

void AMScanDictionary::setOperatingOnName(bool operatingOnName){
	if(operatingOnName && operatingOnExportName())
		operatingOnExportName_ = false;
	operatingOnName_ = operatingOnName;
}

void AMScanDictionary::setOperatingOnExportName(bool operatingOnExportName){
	if(operatingOnExportName && operatingOnName())
		operatingOnName_ = false;
	operatingOnExportName_ = operatingOnExportName;
}

void AMScanDictionary::loadKeywordReplacementDictionaryImplementation()
{
	keywordDictionary_.insert("notes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krNotes));
	keywordDictionary_.insert("runNotes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRunNotes));
	keywordDictionary_.insert("control", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControl));
	keywordDictionary_.insert("controlName", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlName));
	keywordDictionary_.insert("controlDescription", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlDescription));
	keywordDictionary_.insert("controlValue", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlValue));
	keywordDictionary_.insert("controlUnits", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlUnits));
	keywordDictionary_.insert("allControls", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krAllControls));
	keywordDictionary_.insert("sampleNotes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSampleNotes));
}

#include <inttypes.h>
void AMScanDictionary::operateImplementation(const QString &input){
	if(operatingOnName()){

		if(scan_->name() != input)
			scan_->setName(input);
	}
	else if(operatingOnExportName()){

	}
}

QString AMScanDictionary::krName(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_)
		return scan_->name();
	return "[??]";
}


QString AMScanDictionary::krTechnique(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_ && scan_->scanConfiguration())
		return scan_->scanConfiguration()->technique();
	return "[??]";
}


QString AMScanDictionary::krNumber(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_)
		return QString::number(scan_->number());
	return "[??]";
}

QString AMScanDictionary::krNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_)
		return scan_->notes();
	return "[??]";
}

QString AMScanDictionary::krDate(const QString& arg) {
	if(!scan_)
		return "[??]";

	if(arg.isEmpty())
		return scan_->dateTime().toString("yyyy MM dd");
	else
		return scan_->dateTime().toString(arg);
}

QString AMScanDictionary::krTime(const QString& arg) {
	if(!scan_)
		return "[??]";

	if(arg.isEmpty())
		return scan_->dateTime().toString("hh:mm:ss");
	else
		return scan_->dateTime().toString(arg);
}

QString AMScanDictionary::krDateTime(const QString& arg) {
	if(!scan_)
		return "[??]";

	if(arg.isEmpty())
		return scan_->dateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return scan_->dateTime().toString(arg);
}

QString AMScanDictionary::krRun(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());
	// Making the end time a null QDateTime because it no longer exists.
	return r.name() % " " % AMDateTimeUtils::prettyDateRange(r.dateTime(), QDateTime());
}


QString AMScanDictionary::krRunName(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());
	return r.name();
}

QString AMScanDictionary::krRunStartDate(const QString& arg) {
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());

	if(arg.isEmpty())
		return r.dateTime().toString("yyyy MM dd");
	else
		return r.dateTime().toString(arg);
}

QString AMScanDictionary::krRunEndDate(const QString& arg) {
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());

	// Returning [error] so we know we broke this.
	Q_UNUSED(arg)
	return "[error]";
}

QString AMScanDictionary::krRunNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());
	return r.notes();
}


QString AMScanDictionary::krFacilityName(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMFacility f;
	f.loadFromDb(scan_->database(), 1); //AMFacility table will have only one entry, which is the definition of this facility
	return f.name();
}

QString AMScanDictionary::krFacilityDescription(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMFacility f;
	f.loadFromDb(scan_->database(), 1); //AMFacility table will have only one entry, which is the definition of this facility
	return f.description();
}

QString AMScanDictionary::krScanConfiguration(const QString& propertyName) {
	if(!scan_)
		return "[??]";

	const AMScanConfiguration* scanConfig = scan_->scanConfiguration();
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

	}
	if(!v.isValid())
		return "[??]";

	return v.toString();
}

QString AMScanDictionary::krControl(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	const AMControlInfo& ci = scan_->scanInitialConditions()->at(index);
	return QString::number(ci.value()) % " " % ci.units();
}

QString AMScanDictionary::krControlName(const QString &controlName)
{
	if (!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return scan_->scanInitialConditions()->at(index).name();
}

QString AMScanDictionary::krControlDescription(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return scan_->scanInitialConditions()->at(index).description();
}

QString AMScanDictionary::krControlValue(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return QString::number(scan_->scanInitialConditions()->at(index).value());
}

QString AMScanDictionary::krControlUnits(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return scan_->scanInitialConditions()->at(index).units();
}

QString AMScanDictionary::krAllControls(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	QStringList rv;
	for(int i=0; i<scan_->scanInitialConditions()->count(); i++) {
		const AMControlInfo& ci = scan_->scanInitialConditions()->at(i);
		rv << ci.name() % ": " % QString::number(ci.value()) % " " % ci.units();
	}

	return rv.join("\r\n");
}

QString AMScanDictionary::krSample(const QString& arg) {
	Q_UNUSED(arg)
	return krSampleName();
}

QString AMScanDictionary::krSampleName(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	return scan_->sampleName();
}

QString AMScanDictionary::krSampleElements(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(scan_->database(), scan_->sampleId());
	return s.elementString();
}

QString AMScanDictionary::krSampleCreationDate(const QString& arg) {
	if(!scan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(scan_->database(), scan_->sampleId());

	if(arg.isEmpty())
		return s.dateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return s.dateTime().toString(arg);
}

QString AMScanDictionary::krSampleNotes(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMSamplePre2013 s;
	s.loadFromDb(scan_->database(), scan_->sampleId());
	return s.notes();
}
