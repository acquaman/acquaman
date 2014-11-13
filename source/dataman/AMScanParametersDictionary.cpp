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


#include "AMScanParametersDictionary.h"

#include <QStringBuilder>
#include "util/AMTagReplacementParser.h"

 AMScanParametersDictionary::~AMScanParametersDictionary(){}
AMScanParametersDictionary::AMScanParametersDictionary(AMScanParametersDictionary::OperatingParameter operatingOn, QObject *parent) :
	QObject(parent)
{
	if(canOperateOn(operatingOn))
		setOperatingOn(operatingOn);
	else
		operatingOn_ = AMScanParametersDictionary::OperateOnNone;
}

const QString& AMScanParametersDictionary::input() const{
	return input_;
}

const QString& AMScanParametersDictionary::output() const{
	return output_;
}

const QString& AMScanParametersDictionary::reOperateOutput(){
	reOperate();
	return output_;
}

bool AMScanParametersDictionary::canOperateOn(AMScanParametersDictionary::OperatingParameter operatingOn) const{
	if(operatingOn != AMScanParametersDictionary::OperateOnNone)
		return false;
	return true;
}

AMScanParametersDictionary::OperatingParameter AMScanParametersDictionary::operatingOn() const{
	return operatingOn_;
}

void AMScanParametersDictionary::setInput(const QString &input){
	QString oldInput = input_;
	input_ = input;
	keywordParser_->setInitialText(input);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	QString oldOutput = output_;
	output_ = keywordParser_->getReplacedText();
	operateImplementation(output_);
	if(oldOutput != output_)
		emit outputChanged(output_);
	if(oldInput != input_)
		emit inputChanged(input_);
}

void AMScanParametersDictionary::setOperatingOn(AMScanParametersDictionary::OperatingParameter operatingOn){
	if(!canOperate() || !canOperateOn(operatingOn))
		return;
	operatingOn_ = operatingOn;
}

void AMScanParametersDictionary::reOperate(){
	if(!canOperate())
		return;
	keywordParser_->setInitialText(input_);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	QString oldOutput = output_;
	output_ = keywordParser_->getReplacedText();
	operateImplementation(output_);
	if(oldOutput != output_)
		emit outputChanged(output_);
}

void AMScanParametersDictionary::loadKeywordReplacementDictionary()
{
	keywordDictionary_.insert("name", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krName));
	keywordDictionary_.insert("technique", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krTechnique));
	keywordDictionary_.insert("number", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krNumber));
	keywordDictionary_.insert("date", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krDate));
	keywordDictionary_.insert("time", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krTime));
	keywordDictionary_.insert("dateTime", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krDateTime));

	keywordDictionary_.insert("run", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krRun));
	keywordDictionary_.insert("runName", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krRunName));
	keywordDictionary_.insert("runStartDate", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krRunStartDate));
	keywordDictionary_.insert("runEndDate", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krRunEndDate));
	keywordDictionary_.insert("facilityName", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krFacilityName));
	keywordDictionary_.insert("facilityDescription", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krFacilityDescription));

	keywordDictionary_.insert("scanConfiguration", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krScanConfiguration));

	keywordDictionary_.insert("sample", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krSample));
	keywordDictionary_.insert("sampleName", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krSampleName));
	keywordDictionary_.insert("sampleElements", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krSampleElements));
	keywordDictionary_.insert("sampleCreationDate", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krSampleCreationDate));

	keywordDictionary_.insert("exportName", new AMTagReplacementFunctor<AMScanParametersDictionary>(this, &AMScanParametersDictionary::krExportName));
	loadKeywordReplacementDictionaryImplementation();
}

QString AMScanParametersDictionary::krName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krTechnique(const QString &arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krNumber(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krTime(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krDateTime(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krRun(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krRunName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krRunStartDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krRunEndDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krFacilityName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krFacilityDescription(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krScanConfiguration(const QString& propertyName){
	Q_UNUSED(propertyName);
	return QString();
}

QString AMScanParametersDictionary::krSample(const QString& arg){
	Q_UNUSED(arg);return QString();
}

QString AMScanParametersDictionary::krSampleName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krSampleElements(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krSampleCreationDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMScanParametersDictionary::krExportName(const QString &arg){
	Q_UNUSED(arg);
	return QString();
}


/***************************************************/

AMOldScanParametersDictionary::AMOldScanParametersDictionary(QObject *parent) :
	QObject(parent)
{
	operatingOnName_ = false;
	operatingOnExportName_ = false;
	operatingNow_ = false;
	lastInputString_ = "";
	keywordParser_ = new AMTagReplacementParser();
}

AMOldScanParametersDictionary::~AMOldScanParametersDictionary()
{
	delete keywordParser_;
}

bool AMOldScanParametersDictionary::operatingOnName() const{
	return operatingOnName_;
}

bool AMOldScanParametersDictionary::operatingOnExportName() const{
	return operatingOnExportName_;
}

void AMOldScanParametersDictionary::reoperate(){
	if(canOperate())
		parseKeywordStringAndOperate(lastInputString_);
}

QString AMOldScanParametersDictionary::parseKeywordString(const QString &inputString) {
	lastInputString_ = inputString;
	keywordParser_->setInitialText(inputString);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	QString parsedString = keywordParser_->getReplacedText();
	if(!operatingNow_)
		emit parsed(parsedString);
	return parsedString;
}

QString AMOldScanParametersDictionary::parseKeywordStringAndOperate(const QString &inputString){
	operatingNow_ = true;
	QString parsedString = parseKeywordString(inputString);
	operateImplementation(parsedString);
	operatingNow_ = false;
	emit parsed(parsedString);
	return parsedString;
}

void AMOldScanParametersDictionary::loadKeywordReplacementDictionary()
{
	keywordDictionary_.insert("name", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krName));
	keywordDictionary_.insert("technique", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krTechnique));
	keywordDictionary_.insert("number", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krNumber));
	keywordDictionary_.insert("date", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krDate));
	keywordDictionary_.insert("time", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krTime));
	keywordDictionary_.insert("dateTime", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krDateTime));

	keywordDictionary_.insert("run", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krRun));
	keywordDictionary_.insert("runName", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krRunName));
	keywordDictionary_.insert("runStartDate", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krRunStartDate));
	keywordDictionary_.insert("runEndDate", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krRunEndDate));
	keywordDictionary_.insert("facilityName", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krFacilityName));
	keywordDictionary_.insert("facilityDescription", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krFacilityDescription));

	keywordDictionary_.insert("scanConfiguration", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krScanConfiguration));

	keywordDictionary_.insert("sample", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krSample));
	keywordDictionary_.insert("sampleName", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krSampleName));
	keywordDictionary_.insert("sampleElements", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krSampleElements));
	keywordDictionary_.insert("sampleCreationDate", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krSampleCreationDate));

	keywordDictionary_.insert("exportName", new AMTagReplacementFunctor<AMOldScanParametersDictionary>(this, &AMOldScanParametersDictionary::krExportName));
	loadKeywordReplacementDictionaryImplementation();
}

QString AMOldScanParametersDictionary::krName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krTechnique(const QString &arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krNumber(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krTime(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krDateTime(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krRun(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krRunName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krRunStartDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krRunEndDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krFacilityName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krFacilityDescription(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krScanConfiguration(const QString& propertyName){
	Q_UNUSED(propertyName);
	return QString();
}

QString AMOldScanParametersDictionary::krSample(const QString& arg){
	Q_UNUSED(arg);return QString();
}

QString AMOldScanParametersDictionary::krSampleName(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krSampleElements(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krSampleCreationDate(const QString& arg){
	Q_UNUSED(arg);
	return QString();
}

QString AMOldScanParametersDictionary::krExportName(const QString &arg){
	Q_UNUSED(arg);
	return QString();
}
