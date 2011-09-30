#include "AMScanDictionary.h"

#include <QStringBuilder>

#include "util/AMTagReplacementParser.h"
#include "dataman/AMScan.h"

AMScanDictionary::AMScanDictionary(AMScan *scan, QObject *parent) :
	QObject(parent)
{
	scan_ = scan;
	keywordParser_ = new AMTagReplacementParser();

	loadKeywordReplacementDictionary();
}


QString AMScanDictionary::parseKeywordString(const QString &inputString) {
	keywordParser_->setInitialText(inputString);
	keywordParser_->replaceAllUsingDictionary(keywordDictionary_);
	return keywordParser_->getReplacedText();
}

void AMScanDictionary::loadKeywordReplacementDictionary()
{
	//keywordDictionary_.insert("name", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krName));
	keywordDictionary_.insert("technique", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krTechnique));
	keywordDictionary_.insert("number", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krNumber));
	keywordDictionary_.insert("notes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krNotes));
	keywordDictionary_.insert("date", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krDate));
	keywordDictionary_.insert("time", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krTime));
	keywordDictionary_.insert("dateTime", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krDateTime));

	keywordDictionary_.insert("run", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRun));
	keywordDictionary_.insert("runName", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRunName));
	keywordDictionary_.insert("runStartDate", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRunStartDate));
	keywordDictionary_.insert("runEndDate", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRunEndDate));
	keywordDictionary_.insert("runNotes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krRunNotes));
	keywordDictionary_.insert("facilityName", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krFacilityName));
	keywordDictionary_.insert("facilityDescription", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krFacilityDescription));

	keywordDictionary_.insert("scanConfiguration", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krScanConfiguration));

	keywordDictionary_.insert("control", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControl));
	keywordDictionary_.insert("controlDescription", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlDescription));
	keywordDictionary_.insert("controlValue", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlValue));
	keywordDictionary_.insert("controlUnits", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krControlUnits));
	keywordDictionary_.insert("allControls", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krAllControls));

	keywordDictionary_.insert("sample", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSample));
	keywordDictionary_.insert("sampleName", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSampleName));
	keywordDictionary_.insert("sampleElements", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSampleElements));
	keywordDictionary_.insert("sampleCreationDate", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSampleCreationDate));
	keywordDictionary_.insert("sampleNotes", new AMTagReplacementFunctor<AMScanDictionary>(this, &AMScanDictionary::krSampleNotes));
}

/*
QString AMScanDictionary::krName(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_)
		return scan_->name();
	return "[??]";
}
*/

QString AMScanDictionary::krTechnique(const QString& arg) {
	Q_UNUSED(arg)
	if(scan_)
		return scan_->technique();
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

#include "util/AMDateTimeUtils.h"
#include "dataman/AMRun.h"

QString AMScanDictionary::krRun(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());
	return r.name() % " " % AMDateTimeUtils::prettyDateRange(r.dateTime(), r.endDateTime());
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

	if(arg.isEmpty())
		return r.endDateTime().toString("yyyy MM dd");
	else
		return r.endDateTime().toString(arg);
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

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());

	AMFacility f;
	f.loadFromDb(scan_->database(), r.facilityId());
	return f.name();
}

QString AMScanDictionary::krFacilityDescription(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMRun r;
	r.loadFromDb(scan_->database(), scan_->runId());

	AMFacility f;
	f.loadFromDb(scan_->database(), r.facilityId());
	return f.description();
}

#include "acquaman/AMScanConfiguration.h"	/// \todo Move to dataman!

QString AMScanDictionary::krScanConfiguration(const QString& propertyName) {
	if(!scan_)
		return "[??]";

	const AMScanConfiguration* scanConfig = scan_->scanConfiguration();
	if(!scanConfig)
		return "[??]";

	QVariant v =  scanConfig->property(propertyName.toLatin1().constData());
	if(!v.isValid())
		return "[??]";

	return v.toString();
}

#include "dataman/info/AMControlInfoList.h"

QString AMScanDictionary::krControl(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	const AMControlInfo& ci = scan_->scanInitialConditions()->at(index);
	return QString::number(ci.value()) % " " % ci.units();
}

QString AMScanDictionary::krControlDescription(const QString& controlName) {
	if(!scan_)
		return "[??]";

	int index = scan_->scanInitialConditions()->indexOf(controlName);
	if(index == -1)
		return "[??]";

	return scan_->scanInitialConditions()->at(index).name();	/// \todo Add description!
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

#include "dataman/AMSample.h"

QString AMScanDictionary::krSampleElements(const QString& arg) {
	Q_UNUSED(arg)
	if(!scan_)
		return "[??]";

	AMSample s;
	s.loadFromDb(scan_->database(), scan_->sampleId());
	return s.elementString();
}

QString AMScanDictionary::krSampleCreationDate(const QString& arg) {
	if(!scan_)
		return "[??]";

	AMSample s;
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

	AMSample s;
	s.loadFromDb(scan_->database(), scan_->sampleId());
	return s.notes();
}
