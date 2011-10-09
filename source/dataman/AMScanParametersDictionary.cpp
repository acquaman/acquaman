#include "AMScanParametersDictionary.h"

#include <QStringBuilder>
#include "util/AMTagReplacementParser.h"


AMScanParametersDictionary::AMScanParametersDictionary(QObject *parent) :
	QObject(parent)
{
	operatingOnName_ = false;
	operatingOnExportName_ = false;
	operatingNow_ = false;
	lastInputString_ = "";
	keywordParser_ = new AMTagReplacementParser();
}

bool AMScanParametersDictionary::operatingOnName() const{
	return operatingOnName_;
}

bool AMScanParametersDictionary::operatingOnExportName() const{
	return operatingOnExportName_;
}

void AMScanParametersDictionary::reoperate(){
	parseKeywordStringAndOperate(lastInputString_);
}

QString AMScanParametersDictionary::parseKeywordString(const QString &inputString) {
	lastInputString_ = inputString;
	keywordParser_->setInitialText(inputString);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	QString parsedString = keywordParser_->getReplacedText();
	if(!operatingNow_)
		emit parsed(parsedString);
	return parsedString;
}

QString AMScanParametersDictionary::parseKeywordStringAndOperate(const QString &inputString){
	operatingNow_ = true;
	QString parsedString = parseKeywordString(inputString);
	operateImplementation(parsedString);
	operatingNow_ = false;
	emit parsed(parsedString);
	return parsedString;
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
