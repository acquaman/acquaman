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


#include "AMScanExemplarDictionary.h"

#include <QStringBuilder>
#include <QStringList>
#include <QVariant>

#include "util/AMTagReplacementParser.h"
#include "util/AMDateTimeUtils.h"
#include "dataman/AMScanExemplar.h"
#include "acquaman/AMScanConfiguration.h"

 AMScanExemplarDictionary::~AMScanExemplarDictionary(){}
AMScanExemplarDictionary::AMScanExemplarDictionary(AMScanExemplar *exemplar, QObject *parent) :
	AMOldScanParametersDictionary(parent)
{
	exemplar_ = exemplar;
	if(exemplar_)
		connect(exemplar_, SIGNAL(settingsChanged()), this, SLOT(reoperate()));
	loadKeywordReplacementDictionary();
}

bool AMScanExemplarDictionary::canOperateOnName() const{
	return true;
}

bool AMScanExemplarDictionary::canOperateOnExportName() const{
	return true;
}

bool AMScanExemplarDictionary::canOperate() const{
	return true;
}

void AMScanExemplarDictionary::setOperatingOnName(bool operatingOnName){
	if(operatingOnName && operatingOnExportName())
		operatingOnExportName_ = false;
	operatingOnName_ = operatingOnName;
}

void AMScanExemplarDictionary::setOperatingOnExportName(bool operatingOnExportName){
	if(operatingOnExportName && operatingOnName())
		operatingOnName_ = false;
	operatingOnExportName_ = operatingOnExportName;
}

void AMScanExemplarDictionary::loadKeywordReplacementDictionaryImplementation(){

}

void AMScanExemplarDictionary::operateImplementation(const QString &input){
	if(operatingOnName())
		exemplar_->setName(input);
	else if(operatingOnExportName())
		exemplar_->setExportName(input);
}

QString AMScanExemplarDictionary::krName(const QString& arg) {
	Q_UNUSED(arg)
	if(operatingOnName())
		return "Circular Reference @name";
	return exemplar_->name();
}


QString AMScanExemplarDictionary::krTechnique(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->technique();
}


QString AMScanExemplarDictionary::krNumber(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->number();
}

QString AMScanExemplarDictionary::krDate(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->dateTime().toString("yyyy MM dd");
	else
		return exemplar_->dateTime().toString(arg);
}

QString AMScanExemplarDictionary::krTime(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->dateTime().toString("hh:mm:ss");
	else
		return exemplar_->dateTime().toString(arg);
}

QString AMScanExemplarDictionary::krDateTime(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->dateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return exemplar_->dateTime().toString(arg);
}

QString AMScanExemplarDictionary::krRun(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->runName() % " " % AMDateTimeUtils::prettyDateRange(exemplar_->runStartDateTime(), exemplar_->runEndDateTime());
}


QString AMScanExemplarDictionary::krRunName(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->runName();
}

QString AMScanExemplarDictionary::krRunStartDate(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->dateTime().toString("yyyy MM dd");
	else
		return exemplar_->dateTime().toString(arg);
}

QString AMScanExemplarDictionary::krRunEndDate(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->runEndDateTime().toString("yyyy MM dd");
	else
		return exemplar_->runEndDateTime().toString(arg);
}


QString AMScanExemplarDictionary::krFacilityName(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->facilityName();
}

QString AMScanExemplarDictionary::krFacilityDescription(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->facilityDescription();
}

QString AMScanExemplarDictionary::krScanConfiguration(const QString& propertyName) {
	const AMScanConfiguration* scanConfig = exemplar_->scanConfiguration();
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

QString AMScanExemplarDictionary::krSample(const QString& arg) {
	Q_UNUSED(arg)
	return krSampleName();
}

QString AMScanExemplarDictionary::krSampleName(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->sampleName();
}

QString AMScanExemplarDictionary::krSampleElements(const QString& arg) {
	Q_UNUSED(arg)
	return exemplar_->sampleElements();
}

QString AMScanExemplarDictionary::krSampleCreationDate(const QString& arg) {
	if(arg.isEmpty())
		return exemplar_->sampleDateTime().toString("yyyy MM dd hh:mm:ss");
	else
		return exemplar_->sampleDateTime().toString(arg);
}

QString AMScanExemplarDictionary::krExportName(const QString &arg){
	Q_UNUSED(arg);
	if(operatingOnName() || operatingOnExportName())
		return "Circular Reference @exportName";
	return exemplar_->exportName();
}
